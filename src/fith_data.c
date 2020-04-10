/* fith compiler */
/* PUBLIC DOMAIN */

static void *
logged_malloc(size_t bytes)
{
	void *ptr;
	s64 a;
	ptr = malloc(bytes);
	a= (s64)ptr;
	SQL3_QUERY_insert_ptr_addr(fdb,
		"INSERT INTO ptrs VALUES("
		"?i@a);");
	SQL3_BIND_insert_ptr_addr();
	SQL3_STEP_insert_ptr_addr();
	SQL3_RESET_insert_ptr_addr();
	return ptr;
}

static s64
fith_json_array_length(u8 *json)
{
	s64 res;
	
	
	SQL3_QUERY_json_array_length(fdb,
		"SELECT =?i:res json_array_length(?t@json);");
	
	SQL3_BIND_json_array_length();
	
	SQL3_STEP_json_array_length();
	SQL3_COL_json_array_length();
	SQL3_RESET_json_array_length();
	return res;
}

static s64
fith_json_extract(u8 *json, u8 *key, Data *val)
{
	u8 buff[256];
	const u8 *res;
	u8 *ret;
	
	
	SQL3_QUERY_json_extract(fdb,
		"SELECT =?t:res json_type(?t@json, ?t@buff),json_extract(?t@json, ?t@buff);");
	
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_extract();
	
	/* prepare SQL query */
	SQL3_STEP_json_extract();
	SQL3_COL_json_extract();
	switch(res[0]) {
		case 0:   // NULL
		res = (const u8 *)"NULL";
		goto copy_exit;
		case 'a': // array
		goto text_exit;
		case 'f': // false
		goto copy_exit;
		case 'i': // integer
		val->i=sqlite3_column_int64(fdb_stmt_array[fdb_enum3], 1);
		goto reset_exit;
		case 'n': // null
		goto copy_exit;
		case 'o': // object
		goto text_exit;
		case 'r': // real
		val->d=sqlite3_column_double(fdb_stmt_array[fdb_enum3], 1);
		goto reset_exit;
		case 't': // true or text
		if (res[1]=='r') { // true
			goto copy_exit;
		}
		goto text_exit;
		default: return -1;
	}
	text_exit:
	res=sqlite3_column_text(fdb_stmt_array[fdb_enum3], 1);
	copy_exit:
	ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
	strcpy((char *)ret, (const char *)res);
	val->s = ret;
	reset_exit:
	SQL3_RESET_json_extract();
	return 0;
}

//TODO stay below

static u8 *
fith_json_set_j(u8 *json, u8 *key, u8 *val)
{
	u8 buff[256];
	const u8 *res;
	u8 *ret;
	
	
	SQL3_QUERY_json_set_j(fdb,
		"SELECT =?t:res json_set(?t@json, ?t@buff, json(?t@val));");
	
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_j();
	
	/* prepare SQL query */
	//~ n = 
	SQL3_STEP_json_set_j();
	SQL3_COL_json_set_j();
	ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_j();
	return ret;
}

static u8 *
fith_json_set_j_internal(u8 *json, u8 *key, u8 *val)
{
	const u8 *res;
	u8 *ret;
	u32 json_length, j_scale, new_scale;
	u8 buff[256];
	
	json_length=strlen((const char *)json);
	j_scale=json_length/64+1;
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_j();
	
	SQL3_STEP_json_set_j();
	SQL3_COL_json_set_j();
	// res now holds string
	new_scale=strlen((const char *)res)/64+1;
	if((json_length>2)&&(j_scale>=new_scale)) { // not a new json creation
		ret = json;
	} else { // new or previous size too small
		ret = logged_malloc(new_scale*64);
	}
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_j();
	return ret;
}

static u8 *
fith_json_set_s(u8 *json, u8 *key, u8 *val)
{
	u8 buff[256];
	const u8 *res;
	u8 *ret;
	
	
	SQL3_QUERY_json_set_s(fdb,
		"SELECT =?t:res json_set(?t@json, ?t@buff, ?t@val);");
	
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_s();
	
	/* prepare SQL query */
	//~ n = 
	SQL3_STEP_json_set_s();
	SQL3_COL_json_set_s();
	ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_s();
	return ret;
}

static u8 *
fith_json_set_s_internal(u8 *json, u8 *key, u8 *val)
{
	const u8 *res;
	u8 *ret;
	u32 json_length, j_scale, new_scale;
	u8 buff[256];
	
	json_length=strlen((const char *)json);
	j_scale=json_length/64+1;
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_s();
	
	SQL3_STEP_json_set_s();
	SQL3_COL_json_set_s();
	// res now holds string
	new_scale=strlen((const char *)res)/64+1;
	if((json_length>2)&&(j_scale>=new_scale)) { // not a new json creation
		ret = json;
	} else { // new or previous size too small
		ret = logged_malloc(new_scale*64);
	}
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_s();
	return ret;
}

static u8 *
fith_json_set_i(u8 *json, u8 *key, s64 val)
{
	u8 buff[256];
	const u8 *res;
	u8 *ret;
	
	
	SQL3_QUERY_json_set_i(fdb,
		"SELECT =?t:res json_set(?t@json, ?t@buff, ?i@val);");
	
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_i();
	
	/* prepare SQL query */
	//~ n = 
	SQL3_STEP_json_set_i();
	SQL3_COL_json_set_i();
	ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_i();
	return ret;
}

static u8 *
fith_json_set_i_internal(u8 *json, u8 *key, s64 val)
{
	const u8 *res;
	u8 *ret;
	u32 json_length, j_scale, new_scale;
	u8 buff[256];
	
	json_length=strlen((const char *)json);
	j_scale=json_length/64+1;
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_i();
	
	SQL3_STEP_json_set_i();
	SQL3_COL_json_set_i();
	// res now holds string
	new_scale=strlen((const char *)res)/64+1;
	if((json_length>2)&&(j_scale>=new_scale)) { // not a new json creation
		ret = json;
	} else { // new or previous size too small
		ret = logged_malloc(new_scale*64);
	}
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_i();
	return ret;
}

static u8 *
fith_json_set_d(u8 *json, u8 *key, f64 val)
{
	u8 buff[256];
	const u8 *res;
	u8 *ret;
	
	
	SQL3_QUERY_json_set_d(fdb,
		"SELECT =?t:res json_set(?t@json, ?t@buff, ?d@val);");
	
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_d();
	
	/* prepare SQL query */
	//~ n = 
	SQL3_STEP_json_set_d();
	SQL3_COL_json_set_d();
	ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_d();
	return ret;
}

static u8 *
fith_json_set_d_internal(u8 *json, u8 *key, f64 val)
{
	const u8 *res;
	u8 *ret;
	u32 json_length, j_scale, new_scale;
	u8 buff[256];
	
	json_length=strlen((const char *)json);
	j_scale=json_length/64+1;
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	
	SQL3_BIND_json_set_d();
	
	SQL3_STEP_json_set_d();
	SQL3_COL_json_set_d();
	// res now holds string
	new_scale=strlen((const char *)res)/64+1;
	if((json_length>2)&&(j_scale>=new_scale)) { // not a new json creation
		ret = json;
	} else { // new or previous size too small
		ret = logged_malloc(new_scale*64);
	}
	strcpy((char *)ret, (const char *)res);
	SQL3_RESET_json_set_d();
	return ret;
}

// "SELECT json_set('{}', '$.type', 'bear');"

static void
save_function_addr(u8 *start, u64 len, u8 *addr)
{
	s64 a=(s64)addr;
	//~ s32 n;
	//~ for(u32 u=0; u<len; u++)
	//~ {
		//~ fputc (start[u], stdout);
	//~ }
	//~ fputc ('\n', stdout);
	SQL3_QUERY_insert_fn_addr(fdb,
		"INSERT INTO fns(name, addr) VALUES("
		"?t@start$len, ?i@a)"
		"ON CONFLICT(name) DO UPDATE SET addr=excluded.addr;");
	
	SQL3_BIND_insert_fn_addr();
	
	/* prepare SQL query */
	//~ n = 
	SQL3_STEP_insert_fn_addr();
	SQL3_RESET_insert_fn_addr();
	//printf("SQL3_STEP_insert_fn_addr returned %d, len=%d\n",n,len);
}

static s64
get_function_addr(u8 *start, u64 len)
{
	s64 a;
	s32 n;
	//~ for(u32 u=0; u<len; u++)
	//~ {
		//~ fputc (start[u], stdout);
	//~ }
	//~ fputc ('\n', stdout);
	SQL3_QUERY_get_fn_addr(fdb,
		"SELECT addr=?i:a FROM fns WHERE name=?t@start$len;");
	
	SQL3_BIND_get_fn_addr();
	
	/* prepare SQL query */
	n = SQL3_STEP_get_fn_addr();
	//printf("SQL3_STEP_get_fn_addr returned %d, len=%d\n",n,len);
	if (n!=SQLITE_ROW){ // we have a previous entry
		SQL3_RESET_get_fn_addr();
		return 0;
	}
	SQL3_COL_get_fn_addr();
	SQL3_RESET_get_fn_addr();
	return a;
}

static void
save_variable(u8 *start, u64 len, s64 val)
{
	s64 a=(s64)val;
	//~ s32 n;
	//~ for(u32 u=0; u<len; u++)
	//~ {
		//~ fputc (start[u], stdout);
	//~ }
	//~ fputc ('\n', stdout);
	SQL3_QUERY_insert_variable(fdb,
		"INSERT INTO vars(name, val) VALUES("
		"?t@start$len, ?i@a)"
		"ON CONFLICT(name) DO UPDATE SET val=excluded.val;");
	
	SQL3_BIND_insert_variable();
	
	/* prepare SQL query */
	//~ n = 
	SQL3_STEP_insert_variable();
	SQL3_RESET_insert_variable();
	//printf("SQL3_STEP_insert_fn_addr returned %d, len=%d\n",n,len);
}

static s32
get_variable(u8 *start, u64 len, s64 *val)
{
	s64 a;
	s32 n;
	//~ for(u32 u=0; u<len; u++)
	//~ {
		//~ fputc (start[u], stdout);
	//~ }
	//~ fputc ('\n', stdout);
	SQL3_QUERY_get_variable(fdb,
		"SELECT val=?i:a FROM vars WHERE name=?t@start$len;");
	
	SQL3_BIND_get_variable();
	
	/* prepare SQL query */
	n = SQL3_STEP_get_variable();
	//printf("SQL3_STEP_get_fn_addr returned %d, len=%d\n",n,len);
	if (n!=SQLITE_ROW){ // we have a previous entry
		SQL3_RESET_get_variable();
		return 0;
	}
	SQL3_COL_get_variable();
	*val=a;
	SQL3_RESET_get_variable();
	return 1;
}



