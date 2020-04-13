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
	s64 step;
	
	
	SQL3_QUERY_json_extract(fdb,
		"SELECT =?t:res json_type(?t@json, ?t@buff),json_extract(?t@json, ?t@buff);");
	
	buff[0]='$';
	buff[1]=0;
	strcat((char *)buff,(const char *)key);
	SQL3_BIND_json_extract();
	/* prepare SQL query */
	step = SQL3_STEP_json_extract();
	if (step == SQLITE_ROW) {
		step=0;
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
			default: step=-1;goto reset_exit;
		}
		text_exit:
		res=sqlite3_column_text(fdb_stmt_array[fdb_enum3], 1);
		copy_exit:
		ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
		strcpy((char *)ret, (const char *)res);
		val->s = ret;
	} else {step=-1;}
	reset_exit:
	SQL3_RESET_json_extract();
	return step;
}


#define JEACH_TEXT "SELECT json_each.type,json_each.value FROM json_each(?);"
static s64
fith_json_each(u8 *json, Data *val, sqlite3_stmt **sql)
{
	const u8 *res;
	u8 *ret;
	s64 step;
	sqlite3_stmt *s;
	
	if (*sql)
	{
		s=*sql;
	} else {
		sqlite3_prepare_v2(fdb,
						JEACH_TEXT,
						sizeof JEACH_TEXT-1,
						&s,
						0);
		sqlite3_bind_text(s, 1, (const char *)json, -1, SQLITE_TRANSIENT);
		*sql=s;
	}
	
	/* prepare SQL query */
	step = sqlite3_step(s);
	if ((step = (step == SQLITE_ROW))) {
		res=sqlite3_column_text(s, 0);
		switch(res[0]) {
			case 0:   // NULL
			res = (const u8 *)"NULL";
			goto copy_exit;
			case 'a': // array
			goto text_exit;
			case 'f': // false
			goto copy_exit;
			case 'i': // integer
			val->i=sqlite3_column_int64(s, 1);
			goto reset_exit;
			case 'n': // null
			goto copy_exit;
			case 'o': // object
			goto text_exit;
			case 'r': // real
			val->d=sqlite3_column_double(s, 1);
			goto reset_exit;
			case 't': // true or text
			if (res[1]=='r') { // true
				goto copy_exit;
			}
			goto text_exit;
			default: return -1;
		}
		text_exit:
		res=sqlite3_column_text(s, 1);
		copy_exit:
		ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
		strcpy((char *)ret, (const char *)res);
		val->s = ret;
	}
	reset_exit:
	return step;
}

//TODO stay below

static s64
fith_json_sort(u8 *json, Data *val)
{
	const u8 *res;
	u8 *ret;
	s64 step;
	
	SQL3_QUERY_json_sort(fdb,
		"SELECT json_group_array(value)=?t:res FROM (SELECT json_each.value FROM json_each(?t@json) ORDER BY json_each.value);");
		//"SELECT json_group_array(json_each.value)=?t:res  FROM json_each(?t@json) ORDER BY json_each.value;"); BAD!!
	
	SQL3_BIND_json_sort();
	/* prepare SQL query */
	step = SQL3_STEP_json_sort();
	if (step == SQLITE_ROW) {
		step=0;
		SQL3_COL_json_sort();
		ret = logged_malloc(((strlen((const char *)res)/64+1)*64));
		strcpy((char *)ret, (const char *)res);
		val->s = ret;
	} else {step=-1;}
	SQL3_RESET_json_sort();
	return step;
}

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

static __inline s64 CMP_INT(s64 x,s64 y)
{
	return ((x) - (y));
}

/*  Used in mergesort. */
static __inline void INSERTION_SORT_s64(s64 *dsti, const size_t size) 
{
	size_t i=1;
	s64 *dst=dsti;
	
	for (; i < size; i++) 
	{
		size_t j;
		s64 x;

		/* If this entry is already correct, just move along */
		if (CMP_INT(dst[i - 1], dst[i]) <= 0)
		{
			continue;
		}

		/* Else we need to find the right place while shifting everything over */
		x = dst[i];
		j = i;

		for (;;) 
		{
			dst[j] = dst[j-1];
			j--;
			if ( ((j == 0)||(CMP_INT(dst[j-1], x) <= 0)) )
			{
				break;
			}
		}
		dst[j] = x;
	}
}

/* Standard merge sort */
void MERGE_SORT_RECURSIVE_s64(s64 *newdsti, s64 *dsti, const size_t size) {
	s64 *newdst=newdsti;
	s64 *dst=dsti;
	const size_t middle = size / 2;
	size_t out = 0;
	size_t i = 0;
	size_t j = middle;

	if (size <= 16) {
		INSERTION_SORT_s64(dst, size);
		return;
	}

	MERGE_SORT_RECURSIVE_s64(newdst, dst, middle);
	MERGE_SORT_RECURSIVE_s64(newdst, &dst[middle], size - middle);

	if (CMP_INT(dst[middle - 1], dst[middle]) <= 0)
		{ return; }

	/* copy off left side */
	for (; out < middle; out++) {
		newdst[out]=dst[out];
	}
	out=0;

	for (; i < middle && j < size; ++out) {
		if (CMP_INT(newdst[i], dst[j]) <= 0) {
			dst[out] = newdst[i++];
		} else {
			dst[out] = dst[j++];
		}
	}

	for (; i < middle; ++out) {
		dst[out] = newdst[i++];
	}
}

/*  Used in mergesort. */
static int REVERSE_SORT_s64(s64 *dsti, const size_t size) 
{
	s64 *dst=dsti;
	s64 x;
	size_t i;
	size_t j;
	size_t n;
	size_t p;
	size_t num_reverse;
	size_t off;

	/* filter for reverse runs */
	for (i = 1; i < size; i++)
	{
		/* If this entry is already correct, just move along */
		if (CMP_INT(dst[i - 1], dst[i]) <= 0) 
		{
			if(i>33)
			{
				return 0;
			}
			continue;
		} else 
		{
			off=i-1;
			num_reverse=0;
			do{
				num_reverse++;
				i++;
			} while((i < size)&&(CMP_INT(dst[i - 1], dst[i]) > 0));
			n=num_reverse;
			num_reverse++;

			j=num_reverse/2; /* num items to cpy */
			for (p = 0; p < j; p++) /* top part */
			{
				x=dst[p+off];
				dst[p+off]=dst[n+off];
				dst[n+off]=x;
				n--;
			}
			if(num_reverse==size)
			{
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

/* Standard merge sort */
void MERGE_SORT_s64(s64 *dst, const size_t size) {
	s64 *newdst;

	/* don't bother sorting an array of size <= 1 */
	if (size <= 1) {
		return;
	}

	if (size <= 32) {
		INSERTION_SORT_s64(dst, size);
		return;
	}

	if(REVERSE_SORT_s64(dst, size))
	{
		return;
	}

	newdst = malloc(size/2);

	MERGE_SORT_RECURSIVE_s64(newdst, dst, size);
	free(newdst);
}


