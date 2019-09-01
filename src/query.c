#include <kore/kore.h>
#include <kore/pgsql.h>
#include <ctype.h>

#include "query.h"
#include "str.h"

static int is_query_select(const char *);
static const char *prepare_select_query_buf(struct kore_buf **, const char *);
static void prepare_json_result(struct kore_buf *, struct kore_pgsql *);

int kore_keyval_init(struct keyval *keyval)
{
	keyval->count = 0;
	memset(keyval->keys, 0, sizeof(char *) * __QUERY_COLVAL_MAX_SISE);
	memset(keyval->values, 0, sizeof(char *) * __QUERY_COLVAL_MAX_SISE);

	return (KORE_RESULT_OK);
}

int kore_keyval_add(struct keyval *keyval, const char *key, const char *value)
{
	if (keyval->count >= __QUERY_COLVAL_MAX_SISE) {
		kore_log(LOG_NOTICE, "kore_keyval_add: all space exhausted");
		return (KORE_RESULT_ERROR);
	}
	
	keyval->keys[keyval->count] = key;
	keyval->values[keyval->count] = value;
	keyval->count += 1;

	return (KORE_RESULT_OK);
}

int kore_pgsql_query_run(struct kore_pgsql *pgsql, const char *query,
	struct keyval *keyval, struct kore_buf *kore_buf)
{
	int 				ret, is_select;
	int 				*lengths, *formats;
	const char 			**values;
	const char 			*queryptr;
	struct kore_buf		*query_buf;
	int					nvalues;

	is_select 			= 0;
	lengths = formats 	= NULL;
	values 				= NULL;
	queryptr 			= query;
	nvalues				= 0;

	if (keyval == NULL)
		nvalues = 0;
	else
		nvalues = keyval->count;

	if (nvalues > 0) {
		lengths = kore_calloc(nvalues, sizeof(int));
		formats = kore_calloc(nvalues, sizeof(int));
		values  = keyval->values;
	}
	for (int i = 0; i < nvalues; i++) {
		lengths[i] = 0;
		formats[i] = 0;
	}

	 /* Trim leading whitespace present in query (if any) */
	while (*queryptr && isspace(*queryptr))
		queryptr++;

	/* If query is SELECT then modify the query for json select */
	if (is_query_select(queryptr)) {
		is_select = 1;
		queryptr = prepare_select_query_buf(&query_buf, queryptr);
	}

	/* Fire off parameterised query */
	ret = kore_pgsql_query_param_fields(pgsql, queryptr, 0, nvalues, 
			values, lengths, formats);

	/* Perfrom other actions required by select sql query */
	if (is_select) {
		prepare_json_result(kore_buf, pgsql);
		/* Free the space that was acquired for select */
		kore_buf_free(query_buf);
	}


	kore_free(lengths);
	kore_free(formats);

	return ret;
}

static int is_query_select(const char *query)
{
	char part[7];

	kore_strlcpy(part, query, 7);
	kore_strtolower(part);

	return !strncmp(part, "select", 6);
}

static const char *prepare_select_query_buf(struct kore_buf **query_buf, const char *query)
{
	*query_buf = kore_buf_alloc(1024);
	kore_buf_appendf(*query_buf, "SELECT row_to_json(t) FROM (%s) AS t", query);
	
	return kore_buf_stringify(*query_buf, NULL);
}

static void prepare_json_result(struct kore_buf *kore_buf, struct kore_pgsql *pgsql)
{
	char *value;

	int len = kore_pgsql_ntuples(pgsql);

	if (len > 0)
		kore_buf_appendf(kore_buf, "[");
	else
		kore_buf_appendf(kore_buf, "[]");

	for (int i = 0; i < len; i++) {
		if (!(value = kore_pgsql_getvalue(pgsql, i, 0)))
			continue;

		if (i != (len - 1)) {
			kore_buf_appendf(kore_buf, "%s, ", value);
		}
		else {
			kore_buf_appendf(kore_buf, "%s]", value);
		}
	}
}
