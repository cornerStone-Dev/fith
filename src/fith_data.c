/* fith compiler */
/* PUBLIC DOMAIN */

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



