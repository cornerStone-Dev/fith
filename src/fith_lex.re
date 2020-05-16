/* fith compiler */
/* PUBLIC DOMAIN */


/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
	
	mcm = "(" [^)\x03]* ")"; // WILL NOT WORK ON "**/" ending!!!!
	scm = "\\" [^\n\x03]* "\n";
	wsp = ([ \n\t\r] | scm | mcm)+; // removed \v
	//macro = "#" ([^\n] | "\\\n")* "\n";
	//local_macro = "#@" ([^\n] | "\\\n")* "\n";
	// integer literals
	oct = "0" [0-7]*;
	dec = [1-9][0-9]*;
	hex = '0x' [0-9A-F]+; // a-f removed
	// floating literals
	frc = [0-9]* "." [0-9]+ | [0-9]+ ".";
	exp = 'e' [+-]? [0-9]+;
	flt = "-"? (frc exp? | [0-9]+ exp) [fFlL]?;
	string_lit = ['] ([^'\x00\x03] | ([\\] [']))* ['];
	//string_lit_chain = string_lit ([ \n\t\r]* string_lit)+;
	//string_lit_chain = ([^"\n] | ([\\] ["]))* ("\n" | ["]);
	string_lit_chain = ([^'\n] | ([\\] [']))* "\n";
	string_lit_end = ([^'\n] | ([\\] [']))* ['];
	mangled_string_lit = ['] ([^'\x00\x03] | ([\\] [']))* "\x00";
	char_lit = [`] ([^`\x03] | ([\\] [`]))* [`];
	integer = "-"? (oct | dec | hex);
	lblock =     "{";
	rblock =     "}";
	lparen =     "(";
	rparen =     ")";
	lbracket =   "[";
	rbracket =   "]";
	comma =      ",";
	star =       "*";
	atsign =     "@";
	dollar =     "$";
	semi =     ";";
	function =   "fn";
	function_call = [a-zA-Z_]([a-zA-Z_0-9?!#.[-]|"]")*;
	function_call_addr = [a-zA-Z_][a-zA-Z_0-9?-]*"@";
	function_definition = [a-zA-Z_][a-zA-Z_0-9?-]* ":";
	var = "$" function_call; // push value on stack, if exists
	var_assign = "=$" function_call; // pop top of stack and assign to value, create variable
	// json get
	var_get_json = "$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+; // get JSON string
	// json set
	var_assign_array = "=$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+; // assign JSON string
	var_assign_json_s = "=$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+".s"; // assign JSON string
	var_assign_json_i = "=$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+".i"; // assign JSON string
	var_assign_json_d = "=$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+".d"; // assign JSON string
	var_assign_json_j = "=$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+".j"; // assign JSON string
	var_addr = "@" function_call; // push address on stack

	
*/                                   // end of re2c block


static int lex_options(/*const*/ u8 * YYCURSOR) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ //u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	/*const*/ u8 * start;
	
	//YYCURSOR = *YYCURSOR_p;

//loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { printf("Invalid Option: %s, Operation aborted\n",start); return 0; } //   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal
	
	"--repl" {
		return 1;
	}
	
	//[a-zA-Z_/0-9-]+ ".fith" {
	[a-zA-Z_/0-9] [a-zA-Z_/0-9-]* ".fith" {
		return 2;
	}
	
	*/                               // end of re2c block
}

static int lex_string_lit_chain(u8 ** YYCURSOR_p)
{                                    //
	u8 * YYCURSOR;
	u8 *start;
	u8 *startMangledString;
	YYCURSOR = *YYCURSOR_p;
	startMangledString = YYCURSOR;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { goto loop; }//   default rule with its semantic action
	//[\x00] { return 1; }             // EOF rule with null sentinal
	
	string_lit_chain {
		*(YYCURSOR-1) = 0;
		startMangledString = (u8*)stpcpy((char *)startMangledString,
										(const char *)start);
		*YYCURSOR_p = startMangledString;
		goto loop;
	}
	
	string_lit_end {
		if(startMangledString==start)
		{
			*(YYCURSOR-1) = 0;
			return 0;
		}
		*(YYCURSOR-1) = 0;
		startMangledString = (u8*)stpcpy((char *)startMangledString,
										(const char *)start);
		*YYCURSOR_p = startMangledString;
		return 1;
	}

	*/                               // end of re2c block
}

static int lex_if_else(/*const*/ u8 ** YYCURSOR_p, u32 is_else) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	/*const*/ //u8 * start;
	u32 num_ifs=0;
	u32 num_je=0;
	
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer
	//start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { /*start =YYCURSOR;*/ goto loop; }//   default rule with its semantic action
	[\x03] { return 1; }             // EOF rule with null sentinal
	
	
	wsp {
		goto loop;
	}
	
	string_lit {
		goto loop;
	}
	
	";" {
		if (is_else==2){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		goto loop;
	}
	
	"if" {
		num_ifs++;
		goto loop;
	}
	
	"else" {
		if ( (num_ifs==0) && (is_else==0) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		goto loop;
	}
	
	"then" {
		if (num_ifs==0 && ((is_else==0)||(is_else==1)) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		num_ifs--;
		goto loop;
	}
	
	"jeach" {
		num_je++;
		goto loop;
	}
	
	"jdone" {
		if ( (is_else==3) && (num_je==0) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		num_je--;
		goto loop;
	}

	*/                               // end of re2c block
}

static int lex(u8 * YYCURSOR, u32 * line_num, Context * c) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ //u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	u8 * start;
	
	//YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer
	start = YYCURSOR;
	if (c->is_step)
	{
		if(c->is_step==2)
		{
			c->is_step = 3;
			// copy source off
			strncpy((char *)c->file_name_buff, (const char *)YYCURSOR, 11);
			// overwrite source with loop back to debug
			//stpcpy((char *)c->stecpot, " debug");
			c->stecpot=YYCURSOR;
			stpcpy((char *)YYCURSOR, "debug ");
			//printf("YYCURSOR::%s\n",YYCURSOR);
		} else {
			c->is_step = 2;
		}
	}

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		u8 *s=start;
		u8 *f=YYCURSOR;
		while (*s!='\n'){
			s--;
		}
		s++;
		while (*f!='\n'){
			f++;
		}
		s++;
		printf("lex failure ");
		for (u32 ss=0; ss <(f-s); ss++){
			fputc ( s[ss], stdout);
		}
		fputc ( '\n', stdout);
		goto loop; 
	} //   default rule with its semantic action start =YYCURSOR;
	[\x03] { return 0; }             // EOF rule with 0x03 sentinal
	
	wsp {
		while (start!=YYCURSOR){
			if(*start=='\n'){
				*line_num+=1;
				//printf("wsp, %d\n", *line_num);
			}
			start++;
		}
		if(c->is_step==2)
		{
			c->is_step = 1;
		}
		goto loop;
	}
 
	integer {
		c->stk->i = strtol( (const char *)start , NULL, 0);
		INCREMENT_STACK
		goto loop;
	}
	
	flt {
		c->stk->d = atof( (const char *)start );
		INCREMENT_STACK
		goto loop;
	}
	
	string_lit { 
		start++;
		c->stk->s = start;
		// concatenate all multiline strings
		if(lex_string_lit_chain(&start))
		{
			// start is final null of created mangled_string_lit
			start++;
			// fill in with spaces until end
			while (start!=YYCURSOR){*start=' ';start++;}
		}
		INCREMENT_STACK
		goto loop;
	}
	
	mangled_string_lit {
		c->stk->s = start+1;
		INCREMENT_STACK
		goto loop;
	}

	char_lit {
		goto loop;
	}

	";" {
		// pop command stack
		c->cstk--;
		// goto to return address
		YYCURSOR = c->cstk->s;
		goto loop;
	}
	
	":" {
		c->stk->s = YYCURSOR;
		INCREMENT_STACK
		YYCURSOR-=lex_if_else(&YYCURSOR, 2); // skip definition
		goto loop;
	}
	
	"call" {
		DECREMENT_STACK
		// save off return
		c->cstk->s = YYCURSOR;
		c->cstk++;
		// jump to function
		YYCURSOR = c->stk->s;
		goto loop;
	}
	
	"load" {
		DECREMENT_STACK
		(c->stk+1)->s = load_file(c->stk->s,1);
		// save off return in command stack
		c->cstk->s = YYCURSOR;
		c->cstk++;
		YYCURSOR = (c->stk+1)->s;
		goto loop;
	}
	
	"return" {
		// pop command stack
		c->cstk--;
		// goto to return address
		YYCURSOR = c->cstk->s;
		goto loop;
	}
	
	"p" {
		DECREMENT_STACK
		if(((*c->stk->s)&0xC0)==0x80)
		{
			lex_printFSON(c->stk->s);
			
		} else {
			printf("%s",(const char *)c->stk->s);
		}
		//~ printf((const char *)c->stk->s);
		printf("\n");
		goto loop;
	}

	"ion-set-d" { // need to write the path
		// set string function for ion objects
		// if path is object, does not exist, then create it
		// if path is an array, fail if past bounds. Needs append operator.
		// arguments...[index]? float 'path' ionObject
		//                  -2   -1    =     +1
		STACK_CHECK(-3)
		DECREMENT_STACK
		DECREMENT_STACK
		u32 path_len;
		u32 fson_length;
		s32 search_res;
		u8 *path = c->stk->s;
		const u8 *fson_start = (c->stk+1)->s;
		const u8 *fson_cursor = (c->stk+1)->s+4;
		Data insert_val = *(c->stk-1);
		
		// handels special case of append at top level
		if(*path == '[')
		{
			if(*(path+1)== '#')
			{
				if(*fson_cursor != ION_ARR_START)
				{
					printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
					return 0;
				}
				fson_length = ION_getLen(fson_start);
				(c->stk-1)->s = ION_appVal(fson_start, insert_val, 0, fson_length);
				goto loop;
			} else {
				goto process_array0;
			}
		}
		
		move_on0:
		if(*path == '.') // we are looking for a key
		{
			if(*fson_cursor != ION_OBJ_START)
			{
				printf("ERROR lex_findPath: not a ION Obj.\n");
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			//printf("lex_searchObj.%s\n", path);
			search_res = lex_searchObj(&fson_cursor, path, path_len);
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on0;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 0);
			} else
			if ( (search_res==1) && (*path!=0) )
			{
				// path not found AND path not complete
				// failure case, no set is possible
				printf("Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				path-=path_len;
				(c->stk-1)->s = ION_newKeyVal(fson_start, fson_cursor, insert_val, 0, path);
			}
		} else if (*path == '[') {
			process_array0:
			if(*fson_cursor != ION_ARR_START)
			{
				printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			search_res = lex_searchArray(&fson_cursor, path, path_len, c);
			// modifies stack pointer
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on0;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 0);
			} else
			if ( (search_res>0) && (*path!=0) )
			{
				// path not found or path is pecial ending AND path not complete
				// failure case, no set is possible
				printf("Error: Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				printf("Error: Index not found, no insert made.\n");
				goto loop;
			} else
			if ( (search_res==2) && (*path==0) )
			{
				// path '#' or '?' AND path is complete
				// handel only '#' for now.
				// we now need to insert key and value pair
				(c->stk-1)->s = ION_newValInsert(fson_start, fson_cursor, insert_val, 0);
			}
		} else {
			printf("ERROR lex_findPath: not a valid path ION Obj.\n");
			return 0;
		}
		goto loop;
	}

	"ion-set-i" { // need to write the path
		// set string function for ion objects
		// if path is object, does not exist, then create it
		// if path is an array, fail if past bounds. Needs append operator.
		// arguments...[index]? int 'path' ionObject
		//                  -2   -1   =     +1
		STACK_CHECK(-3)
		DECREMENT_STACK
		DECREMENT_STACK
		u32 path_len;
		u32 fson_length;
		s32 search_res;
		u8 *path = c->stk->s;
		const u8 *fson_start = (c->stk+1)->s;
		const u8 *fson_cursor = (c->stk+1)->s+4;
		Data insert_val = *(c->stk-1);
		
		// handels special case of append at top level
		if(*path == '[')
		{
			if(*(path+1)== '#')
			{
				if(*fson_cursor != ION_ARR_START)
				{
					printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
					return 0;
				}
				fson_length = ION_getLen(fson_start);
				(c->stk-1)->s = ION_appVal(fson_start, insert_val, 1, fson_length);
				goto loop;
			} else {
				goto process_array1;
			}
		}
		
		move_on1:
		if(*path == '.') // we are looking for a key
		{
			if(*fson_cursor != ION_OBJ_START)
			{
				printf("ERROR lex_findPath: not a ION Obj.\n");
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			//printf("lex_searchObj.%s\n", path);
			search_res = lex_searchObj(&fson_cursor, path, path_len);
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on1;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 1);
			} else
			if ( (search_res==1) && (*path!=0) )
			{
				// path not found AND path not complete
				// failure case, no set is possible
				printf("Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				path-=path_len;
				(c->stk-1)->s = ION_newKeyVal(fson_start, fson_cursor, insert_val, 1, path);
			}
		} else if (*path == '[') {
			process_array1:
			if(*fson_cursor != ION_ARR_START)
			{
				printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			search_res = lex_searchArray(&fson_cursor, path, path_len, c);
			// modifies stack pointer
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on1;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 1);
			} else
			if ( (search_res>0) && (*path!=0) )
			{
				// path not found or path is pecial ending AND path not complete
				// failure case, no set is possible
				printf("Error: Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				printf("Error: Index not found, no insert made.\n");
				goto loop;
			} else
			if ( (search_res==2) && (*path==0) )
			{
				// path '#' or '?' AND path is complete
				// handel only '#' for now.
				// we now need to insert key and value pair
				(c->stk-1)->s = ION_newValInsert(fson_start, fson_cursor, insert_val, 1);
			}
		} else {
			printf("ERROR lex_findPath: not a valid path ION Obj.\n");
			return 0;
		}
		goto loop;
	}

	"ion-set-s" { // need to write the path
		// set string function for ion objects
		// if path is object, does not exist, then create it
		// if path is an array, fail if past bounds. Needs append operator.
		// arguments...[index]? str 'path' ionObject
		//                  -2   -1   =     +1
		STACK_CHECK(-3)
		DECREMENT_STACK
		DECREMENT_STACK
		u32 path_len;
		u32 fson_length;
		s32 search_res;
		u8 *path = c->stk->s;
		const u8 *fson_start = (c->stk+1)->s;
		const u8 *fson_cursor = (c->stk+1)->s+4;
		Data insert_val = *(c->stk-1);
		
		// handels special case of append at top level
		if(*path == '[')
		{
			if(*(path+1)== '#')
			{
				if(*fson_cursor != ION_ARR_START)
				{
					printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
					return 0;
				}
				fson_length = ION_getLen(fson_start);
				(c->stk-1)->s = ION_appVal(fson_start, insert_val, 2, fson_length);
				goto loop;
			} else {
				goto process_array2;
			}
		}
		
		move_on2:
		if(*path == '.') // we are looking for a key
		{
			if(*fson_cursor != ION_OBJ_START)
			{
				printf("ERROR lex_findPath: not a ION Obj.\n");
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			//printf("lex_searchObj.%s\n", path);
			search_res = lex_searchObj(&fson_cursor, path, path_len);
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on2;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 2);
			} else
			if ( (search_res==1) && (*path!=0) )
			{
				// path not found AND path not complete
				// failure case, no set is possible
				printf("Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				path-=path_len;
				(c->stk-1)->s = ION_newKeyVal(fson_start, fson_cursor, insert_val, 2, path);
			}
		} else if (*path == '[') {
			process_array2:
			if(*fson_cursor != ION_ARR_START)
			{
				printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			search_res = lex_searchArray(&fson_cursor, path, path_len, c);
			// modifies stack pointer
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on2;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 2);
			} else
			if ( (search_res>0) && (*path!=0) )
			{
				// path not found or path is pecial ending AND path not complete
				// failure case, no set is possible
				printf("Error: Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				printf("Error: Index not found, no insert made.\n");
				goto loop;
			} else
			if ( (search_res==2) && (*path==0) )
			{
				// path '#' or '?' AND path is complete
				// handel only '#' for now.
				// we now need to insert key and value pair
				(c->stk-1)->s = ION_newValInsert(fson_start, fson_cursor, insert_val, 2);
			}
		} else {
			printf("ERROR lex_findPath: not a valid path ION Obj.\n");
			return 0;
		}
		goto loop;
	}

	"ion-set-o" { // need to write the path
		// set string function for ion objects
		// if path is object, does not exist, then create it
		// if path is an array, fail if past bounds. Needs append operator.
		// arguments...[index]? str 'path' ionObject
		//                  -2   -1   =     +1
		STACK_CHECK(-3)
		DECREMENT_STACK
		DECREMENT_STACK
		u32 path_len;
		u32 fson_length;
		s32 search_res;
		u8 *path = c->stk->s;
		const u8 *fson_start = (c->stk+1)->s;
		const u8 *fson_cursor = (c->stk+1)->s+4;
		Data insert_val = *(c->stk-1);
		
		// handels special case of append at top level
		if(*path == '[')
		{
			if(*(path+1)== '#')
			{
				if(*fson_cursor != ION_ARR_START)
				{
					printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
					return 0;
				}
				fson_length = ION_getLen(fson_start);
				(c->stk-1)->s = ION_appVal(fson_start, insert_val, 3, fson_length);
				goto loop;
			} else {
				goto process_array3;
			}
		}
		
		move_on3:
		if(*path == '.') // we are looking for a key
		{
			if(*fson_cursor != ION_OBJ_START)
			{
				printf("ERROR lex_findPath: not a ION Obj.\n");
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			//printf("lex_searchObj.%s\n", path);
			search_res = lex_searchObj(&fson_cursor, path, path_len);
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on3;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 3);
			} else
			if ( (search_res==1) && (*path!=0) )
			{
				// path not found AND path not complete
				// failure case, no set is possible
				printf("Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				path-=path_len;
				(c->stk-1)->s = ION_newKeyVal(fson_start, fson_cursor, insert_val, 3, path);
			}
		} else if (*path == '[') {
			process_array3:
			if(*fson_cursor != ION_ARR_START)
			{
				printf("ERROR lex_findPath: not a ION Array, %d.\n",*fson_cursor);
				return 0;
			}
			path++;
			fson_cursor++;
			path_len = getPathLen(path);
			search_res = lex_searchArray(&fson_cursor, path, path_len, c);
			// modifies stack pointer
			path+=path_len;
			if ( (search_res==0) && (*path!=0) )
			{
				// success and more path to go through
				goto move_on3;
			} else
			if ( (search_res==0) && (*path==0) )
			{
				// success and NO more path to go through
				// done fson_cursor points at target value
				(c->stk-1)->s = ION_newValOverWrite(fson_start, fson_cursor, insert_val, 3);
			} else
			if ( (search_res>0) && (*path!=0) )
			{
				// path not found or path is pecial ending AND path not complete
				// failure case, no set is possible
				printf("Error: Key not found in path. More path left unread.\n");
				goto loop;
			} else
			if ( (search_res==1) && (*path==0) )
			{
				// path not found AND path is complete
				// we now need to insert key and value pair
				printf("Error: Index not found, no insert made.\n");
				goto loop;
			} else
			if ( (search_res==2) && (*path==0) )
			{
				// path '#' or '?' AND path is complete
				// handel only '#' for now.
				// we now need to insert key and value pair
				(c->stk-1)->s = ION_newValInsert(fson_start, fson_cursor, insert_val, 3);
			}
		} else {
			printf("ERROR lex_findPath: not a valid path ION Obj.\n");
			return 0;
		}
		goto loop;
	}

	"ion-get" {
		// get function for ion objects
		// arguments... 'path' ionObject
		STACK_CHECK(-2)
		DECREMENT_STACK
		u8 *value = (u8 *)lex_findPath(c->stk->s+4, (c->stk-1)->s, c);
		*(c->stk-1) = lex_returnVal(value);
		//c->stk--;
		goto loop;
	}
	
	//~ "search" {
		//~ u8 *temp = (((c->stk-1)->s)+5);
		//~ (c->stk+1)->s = "type";
		//~ lex_searchObj(&temp, &((c->stk+1)->s));
		//~ (c->stk+3)->i = *temp - 0x92;
		//~ temp++;
		//~ for(u32 i=0; i<(c->stk+3)->i; i++)
		//~ {
			//~ fputc(temp[i], stdout);
		//~ }
		//~ fputc('\n', stdout);
		//~ goto loop;
	//~ }
	
	"praw" {
		DECREMENT_STACK
		for (u8 *a = c->stk->s; (*a); a++)
		{
			if (*a<0x80){
				printf("%c",(*a));
			} else {
				printf("|%hhX|",(*a));
			}
		}
		printf("\n");
		//~ printf((const char *)c->stk->s);
		//~ printf("\n");
		u32 string_len = strlen((const char *)c->stk->s);
		(c->stk+1)->s = malloc(string_len*3);
		lex_FSONToString(c->stk->s, (c->stk+1)->s);
		printf("%s\n",(const char *)(c->stk+1)->s);
		goto loop;
	}

	"or" { // "||"
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i||c->stk->i;
		goto loop;
	}

	"and" { // "&&"
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i&&c->stk->i;
		goto loop;
	}

	"&" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i&c->stk->i;
		goto loop;
	}

	"^" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i^c->stk->i;
		goto loop;
	}

	"|" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i|c->stk->i;
		goto loop;
	}

	"==" {
		DECREMENT_STACK
		(c->stk-1)->i = ((c->stk-1)->i)==(c->stk->i);
		goto loop;
	}

	"!=" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i!=c->stk->i;
		goto loop;
	}

	"<" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i<c->stk->i;
		goto loop;
	}

	">" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i>c->stk->i;
		goto loop;
	}

	"<=" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i<=c->stk->i;
		goto loop;
	}

	">=" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i>=c->stk->i;
		goto loop;
	}

	"<<" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i<<c->stk->i;
		goto loop;
	}

	">>" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i>>c->stk->i;
		goto loop;
	}

	"+" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i+c->stk->i;
		goto loop;
	}

	"-" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i-c->stk->i;
		goto loop;
	}

	"/" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i/c->stk->i;
		goto loop;
	}
	
	"*" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i*c->stk->i;
		goto loop;
	}

	"f+" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d+c->stk->d;
		goto loop;
	}

	"f-" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d-c->stk->d;
		goto loop;
	}

	"f/" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d/c->stk->d;
		goto loop;
	}
	
	"f*" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d*c->stk->d;
		goto loop;
	}
	
	"f==" {
		DECREMENT_STACK
		(c->stk-1)->d = ((c->stk-1)->d)==(c->stk->d);
		goto loop;
	}

	"f!=" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d!=c->stk->d;
		goto loop;
	}

	"f<" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d<c->stk->d;
		goto loop;
	}

	"f>" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d>c->stk->d;
		goto loop;
	}

	"f<=" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d<=c->stk->d;
		goto loop;
	}

	"f>=" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d>=c->stk->d;
		goto loop;
	}

	"%" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i%c->stk->i;
		goto loop;
	}

	"." {
		for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
			printf("(%ld) ",c->stk_start[x].i);
		}
		printf("\n");
		goto loop;
	}
	
	"f." {
		for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
			printf("(%f) ",c->stk_start[x].d);
		}
		printf("\n");
		goto loop;
	}
	
	"~" {
		(c->stk-1)->i = ~((c->stk-1)->i);
		goto loop;
	}
	
	"not" {
		(c->stk-1)->i = !((c->stk-1)->i);
		goto loop;
	}
	
	"dup" {
		c->stk->i = (c->stk-1)->i;
		INCREMENT_STACK
		goto loop;
	}
	
	"over" {
		c->stk->i = (c->stk-2)->i;
		INCREMENT_STACK
		goto loop;
	}
	
	"pick" {
		c->stk->i = (c->stk-3)->i;
		INCREMENT_STACK
		goto loop;
	}
	
	"drop" {
		DECREMENT_STACK
		goto loop;
	}
	
	"clear" {
		c->stk=c->stk_start;
		goto loop;
	}
	
	"dep" {
		c->stk->i = c->stk-c->stk_start;
		INCREMENT_STACK
		goto loop;
	}
	
	"swap" {
		c->stk->i = (c->stk-2)->i;
		(c->stk-2)->i = (c->stk-1)->i;
		(c->stk-1)->i = c->stk->i;
		goto loop;
	}
	
	"rot" {
		c->stk->i = (c->stk-3)->i;
		(c->stk-3)->i = (c->stk-2)->i;
		(c->stk-2)->i = (c->stk-1)->i;
		(c->stk-1)->i = c->stk->i;
		goto loop;
	}

	"if" {
		DECREMENT_STACK
		if(c->stk->i==0){
			YYCURSOR-=lex_if_else(&YYCURSOR, 0);
		}
		goto loop;
	}
	
	"else" {
		YYCURSOR-=lex_if_else(&YYCURSOR, 1);
		goto loop;
	}
	
	"then" {
		goto loop;
	}
	
	"begin" {
		c->cstk->s = YYCURSOR;
		c->cstk++;
		goto loop;
	}
	
	"until" {
		DECREMENT_STACK
		if(c->stk->i==0){
			YYCURSOR = (c->cstk-1)->s;
		} else {
			c->cstk--;
		}
		goto loop;
	}

	// new loop to go over values in an object or an array
	//~ "jeach" { // json is on top of the stack
		//~ // cstack where sql statement will be saved
		//~ (c->cstk+1)->s=0;
		//~ // begin json work
		//~ c->stk->i = fith_json_each((c->stk-1)->s, // json
									//~ (c->stk-1),       // value output
									//~ (sqlite3_stmt **)(c->cstk+1)); // sql
		//~ // check for early exit
		//~ if(c->stk->i==0){
			//~ YYCURSOR-=lex_if_else(&YYCURSOR, 3);
			//~ goto loop;
		//~ }
		//~ // save off data in control stack
		//~ // set up jump back
		//~ c->cstk->s = YYCURSOR;
		//~ c->cstk+=2;
		//~ goto loop;
	//~ }

	//~ "jdone" { // condition not based on data stack
		//~ // begin json work
		//~ (c->stk+1)->i = fith_json_each(0, // json
									//~ c->stk,       // value output
									//~ (sqlite3_stmt **)(c->cstk-1)); // sql
		//~ // 0 if done
		//~ if((c->stk+1)->i==0){
			//~ sqlite3_finalize((sqlite3_stmt *)(c->cstk-1)->s);
			//~ c->cstk-=2;
		//~ } else {
			//~ YYCURSOR = (c->cstk-2)->s;
			//~ INCREMENT_STACK
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "jsort" { // return sorted JSON array

		//~ // begin json work
		//~ c->stk->i = fith_json_sort((c->stk-1)->s, // json
									//~ (c->stk-1));  // value output
		//~ // check for early exit
		//~ if(c->stk->i!=0){
			//~ printf("jsort error!!!\n");
		//~ }
		//~ goto loop;
	//~ }
	
	"sleep" {
		DECREMENT_STACK
		DECREMENT_STACK
		c->time.tv_sec = (c->stk)->i;
		c->time.tv_nsec = (c->stk+1)->i;
		nanosleep(&c->time, NULL);
		goto loop;
	}
	
	"fork" {
		// fork executes a function in another process. The parent process will
		// skip the next function call. The child process need to have the
		// return stack modified to exit, returns pid
		c->stk->i = fork();
		if (c->stk->i < 0){
			printf("Fork error!!!\n");
		} else if (c->stk->i == 0) {
			c->cstk->s = (u8*)"exit";
			c->cstk++;
			// jump to function
			YYCURSOR = (c->stk-1)->s;
			c->stk-=1;
		} else {
			(c->stk-1)->i = c->stk->i;
		}
		goto loop;
	}
	
	"exit" {
		_Exit((c->stk-1)->i);
	}
	
	"malloc" {
		(c->stk-1)->s= malloc((c->stk-1)->i);
		if ((c->stk-1)->s == 0 )
		{
			printf("malloc error!!!\n");
		}
		goto loop;
	}
	
	"realloc" {
		(c->stk-1)->s= realloc ((c->stk-2)->s, (c->stk-1)->i);
		if ((c->stk-1)->s == 0 )
		{
			printf("realloc error!!!\n");
		}
		goto loop;
	}
	
	"free" {
		DECREMENT_STACK
		free(c->stk->s);
		goto loop;
	}
	
	"array" {
		// allocate array with padding for 7 byte header (memory log =1 byte)
		(c->stk-1)->s = malloc(((c->stk-1)->i*8)+7);
		// tag header with invalid utf-8
		*(c->stk-1)->s = 0xFF;
		goto loop;
	}
	
	"sort" {
		DECREMENT_STACK
		if(c->stk-c->stk_start<c->stk->i){
			printf("stack underflow avoided!!!\n");
			goto loop;
		}
		MERGE_SORT_s64((s64 *)(c->stk-c->stk->i), c->stk->i);
		goto loop;
	}
	
	"random" {
		//sqlite3_randomness(8, &c->stk->i);
		c->stk->i = (s64)( ((u64)((*(c->stk->s))+3)) * ((*(c->stk->s+1))+3) * ((*(c->stk->s+2))+3)
		 * ((*(c->stk->s+3))+3) * ((*(c->stk->s+4))+3) * ((*(c->stk->s+5))+3)
		  * ((*(c->stk->s+6))+3) * ((*(c->stk->s+7))+3) );
		INCREMENT_STACK
		goto loop;
	}
	
	"memcmp" {
		DECREMENT_STACK
		DECREMENT_STACK
		(c->stk-1)->i = memcmp((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->i);
		goto loop;
	}
	
	"abs" {
		(c->stk-1)->i=labs((c->stk-1)->i);
		goto loop;
	}

	"fabs" {
		(c->stk-1)->d=fabs((c->stk-1)->d);
		goto loop;
	}
	
	"i2f" {
		(c->stk-1)->d=(c->stk-1)->i;
		goto loop;
	}
	
	"i2s" {
		// save off value
		c->stk->i=(c->stk-1)->i;
		// get string
		(c->stk-1)->s = heap_malloc(24);
		sprintf((char *)(c->stk-1)->s, "%ld", c->stk->i);
		goto loop;
	}
	
	"f2i" {
		(c->stk-1)->i=(c->stk-1)->d;
		goto loop;
	}
	
	"f2s" {
		// save off value
		c->stk->d=(c->stk-1)->d;
		// get string
		(c->stk-1)->s = heap_malloc(24);
		sprintf((char *)(c->stk-1)->s, "%f", c->stk->d);
		goto loop;
	}

	"s2i" {
		(c->stk-1)->i = strtol( (const char *)(c->stk-1)->s, NULL, 0);
		goto loop;
	}
	
	"s2f" {
		(c->stk-1)->d = atof( (const char *)(c->stk-1)->s );
		goto loop;
	}
	
	"s2o" {
		u32 string_len = strlen((const char *)(c->stk-1)->s);
		c->stk->s = malloc(string_len*3);
		lex_stringToFSON((c->stk-1)->s, c->stk->s);
		(c->stk-1)->s = c->stk->s;
		goto loop;
	}
	
	"chan" {
		c->stk->i = socketpair(AF_UNIX, SOCK_STREAM, 0, (c->stk+2)->fd);
		if (c->stk->i < 0)
		{
			printf("socketpair error!!!\n");
		} else {
			c->stk->i = (c->stk+2)->fd[0];
			(c->stk+1)->i = (c->stk+2)->fd[1];
			INCREMENT_STACK
			INCREMENT_STACK
		}
		goto loop;
	}
	
	"reads" { // (0fd, 1pBuf, 2sizeof(pBuf))
		DECREMENT_STACK
		DECREMENT_STACK
		(c->stk+2)->i = read((c->stk-1)->i, c->stk->s, (c->stk+1)->i);
		if ((c->stk+2)->i < 0)
		{
			printf("reads error!!!\n");
		}
		(c->stk-1)->s = c->stk->s;
		goto loop;
	}
	
	"writes" { // (0fd, 1pBuf)
		DECREMENT_STACK
		DECREMENT_STACK
		
		(c->stk+3)->i = write(c->stk->i,
								(c->stk+1)->s,
								strlen((const char *)(c->stk+1)->s)+1);
		
		if ((c->stk+3)->i < 0)
		{
			printf("writes error!!!\n");
		}
		goto loop;
	}
	
	"debug" {
		
		if (c->yycur==0)
		{
			c->yycur=YYCURSOR;
		}
		
		if (c->is_step==3)
		{
			// copy back source
			strncpy((char *)c->stecpot, (const char *)c->file_name_buff, 11);
			c->yycur=c->stecpot;
			c->is_step=0;
		}
		
		// save off pointers
		(c->stk+1)->s=c->yycur;
		(c->stk+2)->s=c->yycur;
		//save off character value
		(c->stk+3)->i = *c->yycur;
		*c->yycur=')';
		
		// find begining and ending of line
		if (*line_num>1) {
			while (*(c->stk+1)->s!='\n'){
				(c->stk+1)->s--;
			}
			(c->stk+1)->s++;
		}
		while (*(c->stk+2)->s!='\n'){
				(c->stk+2)->s++;
		}
		(c->stk+2)->s++;
		//(c->stk+3)->i = *(c->stk+2)->s;
		//*(c->stk+2)->s=0;
		
		//printf("%s", (c->stk+1)->s);
		print_code((c->stk+1)->s, (c->stk+2)->s-(c->stk+1)->s);
		//*(c->stk+2)->s=(c->stk+3)->i;
		*c->yycur=(c->stk+3)->i;
		
		if(c->is_fp)
		{
			for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
				printf("[%f] ",c->stk_start[x].d);
			}
			printf("\n");
		} else {
			for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
				printf("[%ld] ",c->stk_start[x].i);
			}
			printf("\n");
		}
		
		while (1)
		{
			raw_begin();
			(c->stk+4)->i = fith_fgets(c->file_name_buff, 512, c->out);
			raw_end();
			//if (fgets ((char *)c->file_name_buff, 512, stdin) != NULL )
			if ((c->stk+4)->i )
			{
				if (!(strncmp((const char *)c->file_name_buff, ".dump", 5)))
				{
					//*(c->buff) = '\000';
					//printf("%s",c->buff_start);
					print_code(c->buff_start, c->buff-c->buff_start);
					continue;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".exit", 5)))
				{
					return 0;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".fp", 3)))
				{
					c->is_fp=1;
					continue;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".int", 4)))
				{
					c->is_fp=0;
					continue;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".resume", 7)))
				{
					YYCURSOR=c->yycur;
					c->yycur=0;
					goto loop;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".s", 2)))
				{
					YYCURSOR = c->yycur;
					c->is_step=1;
					goto loop;
				}
				c->out = c->buff;
				(c->stk+5)->i=0;
				if((*c->file_name_buff!='\n')&&(*c->file_name_buff!='.')){
					while(c->file_name_buff[(c->stk+5)->i]!=3){
						*c->buff=c->file_name_buff[(c->stk+5)->i];
						(c->stk+5)->i++;
						c->buff++;
					}
				}
				//c->buff = (u8*)stpcpy((char *)c->buff, (const char *)c->file_name_buff);
				(c->stk+5)->s=(u8*)stpcpy((char *)c->buff, " debug");
				*(c->stk+5)->s=3;
				YYCURSOR = c->out;
				goto loop;
			}
		}
	}
	
	var {
		start+=1;
		(c->stk+1)->i = get_variable(start, (YYCURSOR - start), &c->stk->i);
		if ((c->stk+1)->i==0){
			printf("Cannot find variable name!!!");
			print_code(start, (YYCURSOR - start));
			fputc ('\n', stdout);
			goto loop;
		}
		INCREMENT_STACK
		goto loop;
	}

	var_assign {
		start+=2;
		DECREMENT_STACK
		// will try to insert unique name, if fails will update value only
		save_variable(start, (YYCURSOR - start), c->stk->i);
		goto loop;
	}

	function_call {
		//get_function_addr
		c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start));
		if (c->stk->s==0){
			printf("Cannot find function name!!!");
			print_code(start, (YYCURSOR - start));
			fputc ('\n', stdout);
			goto loop;
		}
		// save off return
		c->cstk->s = YYCURSOR;
		c->cstk++;
		// jump to function
		YYCURSOR = c->stk->s;
		goto loop;
	}
	
	function_call_addr {
		c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start-1));
		if (c->stk->s==0){
			printf("Cannot find function name!!!");
			print_code(start, (YYCURSOR - start));
			fputc ('\n', stdout);
			goto loop;
		}
		INCREMENT_STACK
		goto loop;
	}
	
	function_definition {
		save_function_addr(start, (YYCURSOR - start-1), YYCURSOR);
		YYCURSOR-=lex_if_else(&YYCURSOR, 2); // skip definition
		goto loop;
	}
	*/                               // end of re2c block
}  



