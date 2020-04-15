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
	hex = '0x' [0-9a-fA-F]+;
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
	integer = "-"? oct | dec | hex;
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
	function_call = [a-zA-Z_][a-zA-Z_0-9?-]*;
	function_call_addr = [a-zA-Z_][a-zA-Z_0-9?-]*"@";
	function_definition = [a-zA-Z_][a-zA-Z_0-9?-]* ":";
	var = "$" function_call; // push value on stack, if exists
	var_assign = "=$" function_call; // pop top of stack and assign to value, create variable
	// json get
	var_get_json = "$" function_call ("."|"[")([a-zA-Z_0-9?#.[-]|"]")+; // get JSON string
	// json set
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
	* { printf("lex failure"); goto loop; } //   default rule with its semantic action start =YYCURSOR;
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
		c->stk->i = atol( (const char *)start );
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
	
	"return" {
		// pop command stack
		c->cstk--;
		// goto to return address
		YYCURSOR = c->cstk->s;
		goto loop;
	}
	
	"p" {
		DECREMENT_STACK
		printf("%s\n",(const char *)c->stk->s);
		//~ printf((const char *)c->stk->s);
		//~ printf("\n");
		goto loop;
	}

	"||" {
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i||c->stk->i;
		goto loop;
	}

	"&&" {
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
	
	"!" {
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
	"jeach" { // json is on top of the stack
		// cstack where sql statement will be saved
		(c->cstk+1)->s=0;
		// begin json work
		c->stk->i = fith_json_each((c->stk-1)->s, // json
									(c->stk-1),       // value output
									(sqlite3_stmt **)(c->cstk+1)); // sql
		// check for early exit
		if(c->stk->i==0){
			YYCURSOR-=lex_if_else(&YYCURSOR, 3);
			goto loop;
		}
		// save off data in control stack
		// set up jump back
		c->cstk->s = YYCURSOR;
		c->cstk+=2;
		goto loop;
	}

	"jdone" { // condition not based on data stack
		// begin json work
		(c->stk+1)->i = fith_json_each(0, // json
									c->stk,       // value output
									(sqlite3_stmt **)(c->cstk-1)); // sql
		// 0 if done
		if((c->stk+1)->i==0){
			sqlite3_finalize((sqlite3_stmt *)(c->cstk-1)->s);
			c->cstk-=2;
		} else {
			YYCURSOR = (c->cstk-2)->s;
			INCREMENT_STACK
		}
		goto loop;
	}
	
	"jsort" { // return sorted JSON array

		// begin json work
		c->stk->i = fith_json_sort((c->stk-1)->s, // json
									(c->stk-1));  // value output
		// check for early exit
		if(c->stk->i!=0){
			printf("jsort error!!!\n");
		}
		goto loop;
	}
	
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
			printf("malloc error!!!\n");
		}
		goto loop;
	}
	
	"free" {
		DECREMENT_STACK
		free(c->stk->s);
		goto loop;
	}
	
	"sort" {
		DECREMENT_STACK
		if(c->stk-c->stk_start<c->stk->i){
			printf("stack underflow [?] operator avoided!!!\n");
			goto loop;
		}
		MERGE_SORT_s64((s64 *)(c->stk-c->stk->i), c->stk->i);
		goto loop;
	}
	
	"random" {
		sqlite3_randomness(8, &c->stk->i);
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
		(c->stk-1)->s = logged_malloc(63,0);
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
		(c->stk-1)->s = logged_malloc(63,0);
		sprintf((char *)(c->stk-1)->s, "%f", c->stk->d);
		goto loop;
	}

	"s2i" {
		(c->stk-1)->i = atol( (const char *)(c->stk-1)->s );
		goto loop;
	}
	
	"s2f" {
		(c->stk-1)->d = atof( (const char *)(c->stk-1)->s );
		goto loop;
	}

	"json_extract" {
		DECREMENT_STACK
		(c->stk-1)->i = fith_json_extract((c->stk-1)->s,
											c->stk->s,
											(c->stk+1));
		if ((c->stk-1)->i){
			printf("json_extract error!!!\n");
		}
		(c->stk-1)->i = (c->stk+1)->i;
		
		goto loop;
	}

	"json_set_j" {
		DECREMENT_STACK
		DECREMENT_STACK
		(c->stk-1)->s = fith_json_set_j((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->s);
		goto loop;
	}

	"json_set_s" {
		DECREMENT_STACK
		DECREMENT_STACK
		(c->stk-1)->s = fith_json_set_s((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->s);
		goto loop;
	}
	
	"json_set_i" {
		DECREMENT_STACK
		DECREMENT_STACK
		(c->stk-1)->s = fith_json_set_i((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->i);
		goto loop;
	}
	
	"json_set_d" {
		DECREMENT_STACK
		DECREMENT_STACK
		(c->stk-1)->s = fith_json_set_d((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->d);
		goto loop;
	}
	
	"json_array_len" {
		(c->stk-1)->i = fith_json_array_length((c->stk-1)->s);
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
				if (!(strncmp((const char *)c->file_name_buff, ".step", 5)))
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

	var_get_json {
		start+=1;
		(c->stk+3)->i=1;
		while((start[(c->stk+3)->i]!='.')&&(start[(c->stk+3)->i]!='[')){
			(c->stk+3)->i++;
		}
		// (c->stk+3)->i is now the length
		
		// get variable value
		(c->stk+2)->i = get_variable(start, (c->stk+3)->i, &(c->stk+1)->i);
		if ((c->stk+2)->i==0){
			printf("Cannot find variable name!!!");
			print_code(start, (YYCURSOR - start));
			fputc ('\n', stdout);
			goto loop;
		}
		// need str ptr and length of search path
		(c->stk+4)->s = &start[(c->stk+3)->i];
		(c->stk+2)->i = *YYCURSOR; // save off ending
		*YYCURSOR = 0; // null terminate
		(c->stk+3)->i = fith_json_extract((c->stk+1)->s, // json
											(c->stk+4)->s, // key
											c->stk);   // value
		if ((c->stk+3)->i){
			printf("json_extract returned \"null\"\n");
			c->stk->s = (u8*)"null";
		}
		*YYCURSOR = (c->stk+2)->i;
		INCREMENT_STACK
		goto loop;
	}

	var_assign_json_s {
		start+=2;
		DECREMENT_STACK
		(c->stk+3)->i=1;
		while((start[(c->stk+3)->i]!='.')&&(start[(c->stk+3)->i]!='[')){
			(c->stk+3)->i++;
		}
		// (c->stk+3)->i is now the length
		
		// get variable value
		(c->stk+2)->i = get_variable(start, (c->stk+3)->i, &(c->stk+1)->i);
		if ((c->stk+2)->i==0){
			// no variable exists yet, check start to determine
			// if an array or an object should be created
			if (start[(c->stk+3)->i]=='.'){
				(c->stk+1)->s = (u8*)"\047{}"+1;
			} else {
				(c->stk+1)->s = (u8*)"\047[]"+1;
			}
		}
		// need str ptr and length of search path
		(c->stk+4)->s = &start[(c->stk+3)->i];
		(c->stk+2)->i = *(YYCURSOR-2); // save off ending
		*(YYCURSOR-2) = 0; // null terminate
		// check for "[?]"
		if ((*(YYCURSOR-4)=='?')&&(*(YYCURSOR-3)==']')&&(*(YYCURSOR-5)=='[')){
			if(c->stk-c->stk_start<c->stk->i){
				printf("stack underflow [?] operator avoided!!!\n");
				goto loop;
			}
			*(YYCURSOR-4)='#';
			(c->stk+5)->i=c->stk->i;
			c->stk->s=(c->stk+1)->s;
			for (u32 x = (c->stk+5)->i; x!=0; x--)
			{
				c->stk->s = fith_json_set_s_internal(c->stk->s,   // json
														(c->stk+4)->s, // key
														(c->stk-x)->s);    // value
			}
			// put qmark back
			*(YYCURSOR-4)='?';
			*(YYCURSOR-2) = (c->stk+2)->i;
			// will try to insert unique name, if fails will update value only
			save_variable(start, (c->stk+3)->i, c->stk->i);
			// decrease stack, safety check above
			c->stk-=(c->stk+5)->i;
			goto loop;
		} else {
			
			c->stk->s = fith_json_set_s_internal((c->stk+1)->s,   // json
											(c->stk+4)->s, // key
											c->stk->s);    // value
		}
		*(YYCURSOR-2) = (c->stk+2)->i;
		// will try to insert unique name, if fails will update value only
		save_variable(start, (c->stk+3)->i, c->stk->i);
		goto loop;
	}
	
	var_assign_json_j {
		start+=2;
		DECREMENT_STACK
		(c->stk+3)->i=1;
		while((start[(c->stk+3)->i]!='.')&&(start[(c->stk+3)->i]!='[')){
			(c->stk+3)->i++;
		}
		// (c->stk+3)->i is now the length
		
		// get variable value
		(c->stk+2)->i = get_variable(start, (c->stk+3)->i, &(c->stk+1)->i);
		if ((c->stk+2)->i==0){
			// no variable exists yet, check start to determine
			// if an array or an object should be created
			if (start[(c->stk+3)->i]=='.'){
				(c->stk+1)->s = (u8*)"\047{}"+1;
			} else {
				(c->stk+1)->s = (u8*)"\047[]"+1;
			}
		}
		// need str ptr and length of search path
		(c->stk+4)->s = &start[(c->stk+3)->i];
		(c->stk+2)->i = *(YYCURSOR-2); // save off ending
		*(YYCURSOR-2) = 0; // null terminate
		// check for "[?]"
		if ((*(YYCURSOR-4)=='?')&&(*(YYCURSOR-3)==']')&&(*(YYCURSOR-5)=='[')){
			if(c->stk-c->stk_start<c->stk->i){
				printf("stack underflow [?] operator avoided!!!\n");
				goto loop;
			}
			*(YYCURSOR-4)='#';
			(c->stk+5)->i=c->stk->i;
			c->stk->s=(c->stk+1)->s;
			for (u32 x = (c->stk+5)->i; x!=0; x--)
			{
				c->stk->s = fith_json_set_j_internal(c->stk->s,   // json
														(c->stk+4)->s, // key
														(c->stk-x)->s);    // value
			}
			// put qmark back
			*(YYCURSOR-4)='?';
			*(YYCURSOR-2) = (c->stk+2)->i;
			// will try to insert unique name, if fails will update value only
			save_variable(start, (c->stk+3)->i, c->stk->i);
			// decrease stack, safety check above
			c->stk-=(c->stk+5)->i;
			goto loop;
		} else {
			
			c->stk->s = fith_json_set_j_internal((c->stk+1)->s,   // json
											(c->stk+4)->s, // key
											c->stk->s);    // value
		}
		*(YYCURSOR-2) = (c->stk+2)->i;
		// will try to insert unique name, if fails will update value only
		save_variable(start, (c->stk+3)->i, c->stk->i);
		goto loop;
	}
	
	var_assign_json_i {
		start+=2;
		DECREMENT_STACK
		(c->stk+3)->i=1;
		while((start[(c->stk+3)->i]!='.')&&(start[(c->stk+3)->i]!='[')){
			(c->stk+3)->i++;
		}
		// (c->stk+3)->i is now the length
		
		// get variable value
		(c->stk+2)->i = get_variable(start, (c->stk+3)->i, &(c->stk+1)->i);
		if ((c->stk+2)->i==0){
			// no variable exists yet, check start to determine
			// if an array or an object should be created
			if (start[(c->stk+3)->i]=='.'){
				(c->stk+1)->s = (u8*)"\047{}"+1;
			} else {
				(c->stk+1)->s = (u8*)"\047[]"+1;
			}
		}
		// need str ptr and length of search path
		(c->stk+4)->s = &start[(c->stk+3)->i];
		(c->stk+2)->i = *(YYCURSOR-2); // save off ending
		*(YYCURSOR-2) = 0; // null terminate
		// check for "[?]"
		if ((*(YYCURSOR-4)=='?')&&(*(YYCURSOR-3)==']')&&(*(YYCURSOR-5)=='[')){
			if(c->stk-c->stk_start<c->stk->i){
				printf("stack underflow [?] operator avoided!!!\n");
				goto loop;
			}
			*(YYCURSOR-4)='#';
			(c->stk+5)->i=c->stk->i;
			c->stk->s=(c->stk+1)->s;
			for (u32 x = (c->stk+5)->i; x!=0; x--)
			{
				c->stk->s = fith_json_set_i_internal(c->stk->s,   // json
														(c->stk+4)->s, // key
														(c->stk-x)->i);    // value
			}
			// put qmark back
			*(YYCURSOR-4)='?';
			*(YYCURSOR-2) = (c->stk+2)->i;
			// will try to insert unique name, if fails will update value only
			save_variable(start, (c->stk+3)->i, c->stk->i);
			// decrease stack, safety check above
			c->stk-=(c->stk+5)->i;
			goto loop;
		} else {
			
			c->stk->s = fith_json_set_i_internal((c->stk+1)->s,   // json
											(c->stk+4)->s, // key
											c->stk->i);    // value
		}
		*(YYCURSOR-2) = (c->stk+2)->i;
		// will try to insert unique name, if fails will update value only
		save_variable(start, (c->stk+3)->i, c->stk->i);
		goto loop;
	}
	
	var_assign_json_d {
		start+=2;
		DECREMENT_STACK
		(c->stk+3)->i=1;
		while((start[(c->stk+3)->i]!='.')&&(start[(c->stk+3)->i]!='[')){
			(c->stk+3)->i++;
		}
		// (c->stk+3)->i is now the length
		
		// get variable value
		(c->stk+2)->i = get_variable(start, (c->stk+3)->i, &(c->stk+1)->i);
		if ((c->stk+2)->i==0){
			// no variable exists yet, check start to determine
			// if an array or an object should be created
			if (start[(c->stk+3)->i]=='.'){
				(c->stk+1)->s = (u8*)"\047{}"+1;
			} else {
				(c->stk+1)->s = (u8*)"\047[]"+1;
			}
		}
		// need str ptr and length of search path
		(c->stk+4)->s = &start[(c->stk+3)->i];
		(c->stk+2)->i = *(YYCURSOR-2); // save off ending
		*(YYCURSOR-2) = 0; // null terminate
		// check for "[?]"
		if ((*(YYCURSOR-4)=='?')&&(*(YYCURSOR-3)==']')&&(*(YYCURSOR-5)=='[')){
			if(c->stk-c->stk_start<c->stk->i){
				printf("stack underflow [?] operator avoided!!!\n");
				goto loop;
			}
			*(YYCURSOR-4)='#';
			(c->stk+5)->i=c->stk->i;
			c->stk->s=(c->stk+1)->s;
			for (u32 x = (c->stk+5)->i; x!=0; x--)
			{
				c->stk->s = fith_json_set_d_internal(c->stk->s,   // json
														(c->stk+4)->s, // key
														(c->stk-x)->d);    // value
			}
			// put qmark back
			*(YYCURSOR-4)='?';
			*(YYCURSOR-2) = (c->stk+2)->i;
			// will try to insert unique name, if fails will update value only
			save_variable(start, (c->stk+3)->i, c->stk->i);
			// decrease stack, safety check above
			c->stk-=(c->stk+5)->i;
			goto loop;
		} else {
			
			c->stk->s = fith_json_set_d_internal((c->stk+1)->s,   // json
											(c->stk+4)->s, // key
											c->stk->d);    // value
		}
		*(YYCURSOR-2) = (c->stk+2)->i;
		// will try to insert unique name, if fails will update value only
		save_variable(start, (c->stk+3)->i, c->stk->i);
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



