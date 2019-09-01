# Library for Kore framework

A very clean and easy posgresql query execution library for kore framework. It is not complete though, you can use it for the fields and data which does not contain binary data because for all result returned from database is treated as string even you can only add string values in `keyval` and not binaray data.
All queries are parameterised so it's safe from sql injection attack.
All the input, output, parameter is treated as c string (NUL terminated).
In select query data is returned as json array, always, even when single record is fetched from database.

### Select query
__Example 1:__ Simple select query with no parameter
```c
const char *query = "SELECT * FROM temp_table";

struct kore_pgsql 	sql;
struct kore_buf   	*kore_buf;
u_int8_t			*data;
size_t				len;

kore_pgsql_init(&sql);

/* Setup our database structure with database name */
if (!kore_pgsql_setup(&sql, "db", KORE_PGSQL_SYNC)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}

/* You must kore_buf_alloc the kore_buf before passing it to
 * kore_pgsql_query_select() function */
kore_buf = kore_buf_alloc(1024);
if (!kore_pgsql_query_select(&sql, query, &keyval, kore_buf)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	kore_buf_free(kore_buf);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}
/* Releases kore_buf and frees kore_buf memory therefor don't call
 * kore_buf_free() on kore_buf after calling kore_buf_release() */
data = kore_buf_release(kore_buf, &len);
http_response(req, 200, data, len);

kore_pgsql_cleanup(&sql);
kore_free(data);

/* data varibale will point to json string (result of query) and json string
 * will look like this: */
[{"id":1,"full_name":"person name 1"}, {"id":2,"full_name":"person name 2"}, {"id":1,"full_name":"Person name 3"}, {"id":1,"full_name":"Person name 4"}, {"id":1,"full_name":"Person name 5"}, {"id":1,"full_name":"Person name 6"}, {"id":1,"full_name":"Person name 7"}]
```
__JSON result (formatted for you to see better view)__
```
[
	{"id":1,"full_name":"person name 1"},
	{"id":2,"full_name":"person name 2"},
	{"id":1,"full_name":"Person name 3"},
	{"id":1,"full_name":"Person name 4"},
	{"id":1,"full_name":"Person name 5"},
	{"id":1,"full_name":"Person name 6"},
	{"id":1,"full_name":"Person name 7"}
]
```

__Example 2:__ Select query with no parameter
```c
const char *query = "SELECT * FROM temp_table WHERE full_name LIKE $1 "
					" AND user_id = $2";

struct kore_pgsql 	sql;
struct kore_buf   	*kore_buf;
struct keyval		keyval;
u_int8_t			*data;
size_t				len;

kore_pgsql_init(&sql);
kore_keyval_init(&keyval);

/* Setup our database structure with database name */
if (!kore_pgsql_setup(&sql, "db", KORE_PGSQL_SYNC)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}

/* Add values in keyval
 * (Order of function must be same as your parameters)
 * You don't need to free keyval, it doesn't alloc any memory from heap */
kore_value_add(&keyval, "%ta%"); /* This will be binded to param $1 */
kore_value_add(&keyval, "100"); /* This will be binded to param $2 */

/* If you have more values to add just call kore_value_add() function
 * again and again
 * Max values can be added keyval is 64 which is more than enough for most
 * queries I think.
 * If you ever need more than that then you can
 * #define __QUERY_COLVAL_MAX_SISE 100 before including query.h file. */


/* You must kore_buf_alloc the kore_buf before passing it to
 * kore_pgsql_query_select() function */
kore_buf = kore_buf_alloc(1024);
if (!kore_pgsql_query_select(&sql, query, &keyval, kore_buf)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	kore_buf_free(kore_buf);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}
/* Releases kore_buf and frees kore_buf memory therefor don't call
 * kore_buf_free() on kore_buf after calling kore_buf_release() */
data = kore_buf_release(kore_buf, &len);
http_response(req, 200, data, len);

kore_pgsql_cleanup(&sql);
kore_free(data);

```

### Insert query
```c
const char *query = "INSERT INTO temp_table(id, full_name, email) "
					"VALUES ($1, $2, $3)";
struct kore_pgsql sql;
struct keyval keyval; 			/* Structure to hold values to be inserted */

/* Add values in keyval
 * (Order of function must be same as your parameters)
 * You don't need to free keyval, it doesn't alloc any memory from heap */

kore_value_add(&keyval, "value1"); /* This will be binded to param $1 */
kore_value_add(&keyval, "value2"); /* This will be binded to param $2 */
kore_value_add(&keyval, "value3"); /* This will be binded to param $3 */

kore_pgsql_init(&sql);

/* Setup our database structure with database name */
if (!kore_pgsql_setup(&sql, "db", KORE_PGSQL_SYNC)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}

if (!kore_pgsql_query_insert(&sql, query, &keyval)) {
	kore_pgsql_logerror(&sql);
}

kore_pgsql_cleanup(&sql);

```

### Update query
_Note_: All integers should be added in keyval as `c string`. You can use sprintf to convert integer or any numeric data type to string
Example: `sprintf(int_to_str, "%d", int_number);`
```c
const char *query = "UPDATE temp_table SET full_name = $1 WHERE id = $2";
struct kore_pgsql sql;
struct keyval keyval; 			/* Structure to hold values to be inserted */

kore_value_add(&keyval, "Name updated"); /* This will be binded to param $1 */
kore_value_add(&keyval, "100"); /* This will be binded to param $2 */

kore_pgsql_init(&sql);

/* Setup our database structure with database name */
if (!kore_pgsql_setup(&sql, "db", KORE_PGSQL_SYNC)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}

if (!kore_pgsql_query_update(&sql, query, &keyval)) {
	kore_pgsql_logerror(&sql);
}

kore_pgsql_cleanup(&sql);

```

### Delete query
_Note_: All integers should be added in keyval as `c string`. You can use sprintf to convert integer or any numeric data type to string
Example: `sprintf(int_to_str, "%d", int_number);`

__Example 1:__
```c
const char *query = "DELETE FROM temp_table WHERE id = $1";

struct kore_pgsql sql;
struct keyval keyval; 			/* Structure to hold values to be inserted */

kore_value_add(&keyval, "100"); /* This will be binded to param $1 */

kore_pgsql_init(&sql);

/* Setup our database structure with database name */
if (!kore_pgsql_setup(&sql, "db", KORE_PGSQL_SYNC)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}

if (!kore_pgsql_query_delete(&sql, query, &keyval)) {
	kore_pgsql_logerror(&sql);
}

kore_pgsql_cleanup(&sql);

```

__Example 2:__
When you don't have any parameters to bind then send NULL as the keyval argument in koer\_pgsql\_query\_delete() function.
You can do the same for update and select when you don't have any parameter to bind.
```c
const char *query = "DELETE FROM temp_table";
struct kore_pgsql sql;

kore_pgsql_init(&sql);

/* Setup our database structure with database name */
if (!kore_pgsql_setup(&sql, "db", KORE_PGSQL_SYNC)) {
	kore_pgsql_logerror(&sql);
	kore_pgsql_cleanup(&sql);
	http_response(req, 500, NULL, 0);
	return (KORE_RESULT_ERROR);
}

/* Pass NULL in keyval argument */
if (!kore_pgsql_query_delete(&sql, query, NULL)) {
	kore_pgsql_logerror(&sql);
}

kore_pgsql_cleanup(&sql);

```
