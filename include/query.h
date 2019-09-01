#ifndef __QUERY_H
#define __QUERY_H

#include <kore/kore.h>
#include <kore/pgsql.h>

/* This Symbolic constant can be overridden */
#ifndef __QUERY_COLVAL_MAX_SISE
#define __QUERY_COLVAL_MAX_SISE 64
#endif

struct keyval {
	int count;
	const char *keys[__QUERY_COLVAL_MAX_SISE];
	const char *values[__QUERY_COLVAL_MAX_SISE];
};

int kore_keyval_init(struct keyval *);
int kore_keyval_add(struct keyval *, const char *, const char *);
char *kore_keyval_get(struct keyval *, const char *);
int kore_pgsql_query_run(
	struct kore_pgsql *,
	const char *,
	struct keyval *,
	struct kore_buf *);

#define kore_pgsql_query_select(pgsql, query, keyval, kore_buf) kore_pgsql_query_run(pgsql, query, keyval, kore_buf) 
#define kore_pgsql_query_insert(pgsql, query, keyval) kore_pgsql_query_run(pgsql, query, keyval, NULL) 
#define kore_pgsql_query_update(pgsql, query, keyval) kore_pgsql_query_run(pgsql, query, keyval, NULL) 
#define kore_pgsql_query_delete(pgsql, query, keyval) kore_pgsql_query_run(pgsql, query, keyval, NULL) 

#define kore_keyval_reset(keyval) kore_keyval_init(keyval)
#define kore_value_add(keyval, value) kore_keyval_add(keyval, NULL, value)

#endif
