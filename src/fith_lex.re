/* fith compiler */
/* PUBLIC DOMAIN */


/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
	
	mcm = "(" [^)\x03]* ")"; 
	scm = "\\" [^\n\x03]* "\n";
	wsp = ([ \n\t\r] | scm | mcm)+; // removed \v
	//macro = "#" ([^\n] | "\\\n")* "\n";
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
	word = [a-zA-Z_]([a-zA-Z_0-9?!#.{}[-]|"]")*;
	function_call_addr = "@" word ;
	function_definition = word ":";
	var = "$" word; // push value on stack, if exists
	var_assign = "=$" word; // pop top of stack and assign to value, create variable
	var_addr = "@$" word; // push address on stack

	
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
	u64 len;
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
		len = YYCURSOR-start-1;
		memmove(startMangledString, start, len);
		startMangledString+=len;
		// skip starting tabs to allow formatting
		while(*YYCURSOR=='\t'){YYCURSOR++;}
		goto loop;
	}
	
	string_lit_end {
		if(startMangledString==start)
		{
			*(YYCURSOR-1) = 0;
			return 0;
		}
		len = YYCURSOR-start-1;
		memmove(startMangledString, start, len);
		startMangledString+=len;
		*startMangledString=0;
		*YYCURSOR_p = startMangledString;
		return 1;
	}

	*/                               // end of re2c block
}

static u64 lex_if_else(/*const*/ u8 ** YYCURSOR_p, u32 is_else, u32 in_case) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	u32 num_ifs=0;
	u32 num_funcs=0;
	u32 num_loops=0;
	u32 num_case=0;
	
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer


	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { goto loop; }//   default rule with its semantic action
	[\x03] { return 1; }             // EOF rule with null sentinal
	
	
	(scm | mcm)+ {
		goto loop;
	}
	
	string_lit {
		goto loop;
	}
	
	mangled_string_lit {
		goto loop;
	}
	
	":" {
		num_funcs++;
		goto loop;
	}
	
	";" {
		if ( (is_else==2) && (num_funcs==0) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		num_funcs--;
		goto loop;
	}
	
	"do" {
		num_loops++;
		goto loop;
	}
	
	"loop" {
		if ( (is_else==3) && (num_loops==0) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		num_loops--;
		goto loop;
	}

	"case" {
		num_case++;
		goto loop;
	}

	"end" {
		if ( (is_else==4) && (num_case==0) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		num_case--;
		goto loop;
	}

	"if{" {
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

	"}" {
		if (num_ifs==0 && ((is_else==0)||(is_else==1)) ){
			if(in_case){
				*(YYCURSOR-1)=0x04;
			}
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		num_ifs--;
		goto loop;
	}

	*/                               // end of re2c block
}

// sub-lexxer for dealing with word.
static int lex_word(u8 * YYCURSOR, Context * c, u8 **YYCURSORout, u64 len) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ //u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	//u8 * start;
	
	//YYCURSOR = *YYCURSOR_p;

//loop: // label for looping within the lexxer
	//start = YYCURSOR;
	
	switch(len-1){
	case 0: // 1 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"p" {
		STACK_CHECK_DOWN_R(-1)
		c->stk--;
		printf("%s",(const char *)c->stk->s);
		printf("\n");
		return 0;
	}
	*/                               // end of re2c block
	break;
	case 1: // 2 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"or" { // "||"
		STACK_CHECK_DOWN_R(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i||c->stk->i;
		return 0;
	}
	"do" {
		// save top of loop
		c->cstk->s = YYCURSOR;
		c->cstk++;
		// find end of loop
		YYCURSOR-=lex_if_else(&YYCURSOR, 3, 0);
		// save end of loop
		c->cstk->s = YYCURSOR;
		c->cstk++;
		return 0;
	}
	*/                               // end of re2c block
	break;
	case 2: // 3 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"and" { // "&&"
		STACK_CHECK_DOWN_R(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i&&c->stk->i;
		return 0;
	}
	"not" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->i = !((c->stk-1)->i);
		return 0;
	}
	"dup" {
		STACK_CHECK_DOWN_R(-1)
		STACK_CHECK_UP_R(1)
		c->stk->i = (c->stk-1)->i;
		c->stk++;
		return 0;
	}
	"dep" {
		STACK_CHECK_UP_R(1)
		c->stk->i = c->stk-c->stk_start;
		c->stk++;
		return 0;
	}
	"rot" {
		s64 tmp;
		STACK_CHECK_DOWN_R(-3)
		tmp = (c->stk-3)->i;
		(c->stk-3)->i = (c->stk-2)->i;
		(c->stk-2)->i = (c->stk-1)->i;
		(c->stk-1)->i = tmp;
		return 0;
	}
	"abs" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->i=labs((c->stk-1)->i);
		return 0;
	}
	"i2f" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->d=(c->stk-1)->i;
		return 0;
	}
	"i2s" {
		STACK_CHECK_DOWN_R(-1)
		// save off value
		c->stk->i=(c->stk-1)->i;
		// get string
		(c->stk-1)->s = heap_malloc(24);
		sprintf((char *)(c->stk-1)->s, "%ld", c->stk->i);
		return 0;
	}
	"f2i" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->i=(c->stk-1)->d;
		return 0;
	}
	"f2s" {
		STACK_CHECK_DOWN_R(-1)
		// save off value
		c->stk->d=(c->stk-1)->d;
		// get string
		(c->stk-1)->s = heap_malloc(24);
		sprintf((char *)(c->stk-1)->s, "%f", c->stk->d);
		return 0;
	}
	"s2i" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->i = strtol( (const char *)(c->stk-1)->s, NULL, 0);
		return 0;
	}
	"s2f" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->d = atof( (const char *)(c->stk-1)->s );
		return 0;
	}
	"rev" {
		s64 tmp, num;
		s64 *start, *end;
		STACK_CHECK_DOWN_R(-1)
		num = (c->stk-1)->i;
		if( (c->stk - c->stk_start-1) < num ){
			printf("rev : stack underflow avoided!!!\n");
			return 0;
		}
		c->stk-=1;
		start = (s64 *)c->stk - num;
		end = (s64 *)c->stk - 1;
		// reverse items
		while(start<end){
			tmp = *start;
			*start = *end;
			*end = tmp;
			start++;
			end--;
		}
		return 0;
	}
	"if{" {
		STACK_CHECK_DOWN_R(-1)
		c->stk--;
		if(c->stk->i==0){ // false
			if(c->in_case){
				// duplicate value for next test
				c->stk->i = (c->stk-1)->i;
				c->stk++;
			}
			YYCURSOR-=lex_if_else(&YYCURSOR, 0, 0);
			*YYCURSORout=YYCURSOR;
		} else if(c->in_case){ // true
			// clear flag
			c->in_case=0;
			// re-write ending for case statement
			lex_if_else(&YYCURSOR, 0, 1);
			// drop auxilary test value
			c->stk--;
		}
		return 0;
	}
	"end" {
		// clear flag
		c->in_case=0;
		c->cstk-=2;
		return 0;
	}
	*/                               // end of re2c block
	break;
	case 3: // 4 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"call" {
		STACK_CHECK_DOWN_R(-1)
		c->stk--;
		// save off return
		c->cstk->s = (u8 *)(((u64)YYCURSOR)|0x8000000000000000);
		c->cstk++;
		// jump to function
		*YYCURSORout = c->stk->s;
		// enter scope
		enter_scope();
		return 0;
	}
	"load" {
		STACK_CHECK_DOWN_R(-1)
		c->stk--;
		(c->stk+1)->s = load_file(c->stk->s,1);
		// save off return in command stack
		c->cstk->s = YYCURSOR;
		c->cstk++;
		*YYCURSORout = (c->stk+1)->s;
		return 0;
	}
	"over" {
		STACK_CHECK_DOWN_R(-2)
		STACK_CHECK_UP_R(1)
		c->stk->i = (c->stk-2)->i;
		c->stk++;
		return 0;
	}
	"pick" {
		STACK_CHECK_DOWN_R(-3)
		STACK_CHECK_UP_R(1)
		c->stk->i = (c->stk-3)->i;
		c->stk++;
		return 0;
	}
	"drop" {
		if(c->stk>c->stk_start)
		{
			c->stk--;
		} else {
		printf("stack underflow!!!\n");}
		return 0;
	}
	"swap" {
		s64 tmp;
		STACK_CHECK_DOWN_R(-2)
		tmp = (c->stk-2)->i;
		(c->stk-2)->i = (c->stk-1)->i;
		(c->stk-1)->i = tmp;
		return 0;
	}
	"else" {
		YYCURSOR-=lex_if_else(&YYCURSOR, 1, 0);
		*YYCURSORout=YYCURSOR;
		return 0;
	}
	"fork" {
		// fork executes a function in another process. The parent process will
		// skip the next function call. The child process need to have the
		// return stack modified to exit, returns pid
		STACK_CHECK_DOWN_R(-1)
		s32 res = fork();
		if (res < 0){
			printf("Fork error!!!\n");
		} else if (res == 0) {
			c->cstk->s = (u8 *)(((u64)"exit")|0x8000000000000000);
			c->cstk++;
			// jump to function
			*YYCURSORout = (c->stk-1)->s;
			// enter scope
			enter_scope();
			c->stk-=1;
		} else {
			(c->stk-1)->i = res;
		}
		return 0;
	}
	"exit" {
		_Exit((c->stk-1)->i);
	}
	"free" {
		STACK_CHECK_DOWN_R(-1)
		c->stk-=1;
		free(c->stk->s);
		return 0;
	}
	"sort" {
		STACK_CHECK_DOWN_R(-1)
		s64 num;
		num = (c->stk-1)->i;
		if( (c->stk - c->stk_start) < num ){
			printf("stack underflow avoided!!!\n");
			return 0;
		}
		c->stk-=1;
		if(num>1){
			INSERTION_SORT_s64((s64 *)(c->stk-num), num);
		}
		return 0;
	}
	"fabs" {
		STACK_CHECK_DOWN_R(-1)
		(c->stk-1)->d=fabs((c->stk-1)->d);
		return 0;
	}
	"chan" {
		STACK_CHECK_UP_R(2)
		s32 res = socketpair(AF_UNIX, SOCK_STREAM, 0, (c->stk+2)->fd);
		if (res < 0)
		{
			printf("socketpair error!!!\n");
		} else {
			c->stk->i = (c->stk+2)->fd[0];
			(c->stk+1)->i = (c->stk+2)->fd[1];
			c->stk+=2;
		}
		return 0;
	}
	"loop" {
		*YYCURSORout = (c->cstk-2)->s;
		return 0;
	}
	"case" {
		// duplicate test value if there
		STACK_CHECK_DOWN_R(-1)
		STACK_CHECK_UP_R(1)
		c->stk->i = (c->stk-1)->i;
		c->stk++;
		// save top of case
		c->cstk->s = YYCURSOR;
		c->cstk++;
		// find end of case
		YYCURSOR-=lex_if_else(&YYCURSOR, 4, 0);
		// save end of loop
		c->cstk->s = YYCURSOR;
		c->cstk++;
		c->in_case=1;
		return 0;
	}
	*/                               // end of re2c block
	break;
	case 4: // 5 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"clear" {
		c->stk=c->stk_start;
		return 0;
	}
	"sleep" {
		STACK_CHECK_DOWN_R(-2)
		c->stk-=2;
		c->time.tv_sec = (c->stk)->i;
		c->time.tv_nsec = (c->stk+1)->i;
		nanosleep(&c->time, NULL);
		return 0;
	}
	"array" {
		STACK_CHECK_DOWN_R(-1)
		// allocate array, 8 byte header
		(c->stk-1)->i = (c->stk-1)->i*8+8;
		goto malloc;
		return 0;
	}
	"reads" { // (0fd, 1pBuf, 2sizeof(pBuf))
		STACK_CHECK_DOWN_R(-3)
		c->stk-=2;
		(c->stk+2)->i = read((c->stk-1)->i, c->stk->s, (c->stk+1)->i);
		if ((c->stk+2)->i < 0)
		{
			printf("reads error!!!\n");
		}
		(c->stk-1)->s = c->stk->s;
		return 0;
	}
	"leave" {
		// clear flag
		c->in_case=0;
		// leave current loop
		*YYCURSORout = (c->cstk-1)->s;
		// clear top two cntrl stacks entries
		c->cstk-=2;
		return 0;
	}
	"again" {
		*YYCURSORout = (c->cstk-2)->s;
		return 0;
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
		//if (*line_num>1) {
		while (*(c->stk+1)->s!='\n'){
			(c->stk+1)->s--;
		}
		(c->stk+1)->s++;
		//}
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
					*YYCURSORout=c->yycur;
					c->yycur=0;
					return 0;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".s", 2)))
				{
					*YYCURSORout = c->yycur;
					c->is_step=1;
					return 0;
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
				*YYCURSORout = c->out;
				return 0;
			}
		}
	}
	*/                               // end of re2c block
	break;
	case 5: // 6 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"return" {
		// pop command stack
		c->cstk--;
		// skip past any looping data
		while( (((u64)c->cstk->s)&0x8000000000000000)==0 ){c->cstk--;}
		// goto to return address
		*YYCURSORout = (u8 *)(((u64)c->cstk->s)&0x7FFFFFFFFFFFFFFF);
		// leave scope
		leave_scope();
		return 0;
	}
	"malloc" {
		STACK_CHECK_DOWN_R(-1)
		malloc:
		(c->stk-1)->s= malloc((c->stk-1)->i);
		if ((c->stk-1)->s == 0 )
		{
			printf("malloc error!!!\n");
		}
		return 0;
	}
	"random" {
		s32 res;
		STACK_CHECK_UP_R(1)
		do{
			res = getrandom(&c->stk->i, 8, 0);
		} while(res!=8);
		c->stk++;
		return 0;
	}
	"memcmp" {
		STACK_CHECK_DOWN_R(-3)
		c->stk-=2;
		s32 res = memcmp((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->i);
		(c->stk-1)->i=res;
		return 0;
	}
	"writes" { // (0fd, 1pBuf)
		STACK_CHECK_DOWN_R(-2)
		c->stk-=2;
		s32 res;
		res = write(c->stk->i,
					(c->stk+1)->s,
					strlen((const char *)(c->stk+1)->s)+1);

		if (res < 0)
		{
			printf("writes error!!!\n");
		}
		return 0;
	}
	*/                               // end of re2c block
	break;
	case 6: // 7 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		return 1;
	} //   default rule with its semantic action start =YYCURSOR;

	"realloc" {
		STACK_CHECK_DOWN_R(-2)
		c->stk--;
		void *ptr;
		ptr= realloc((c->stk-1)->s, c->stk->i);
		if (ptr == 0 )
		{
			printf("realloc error, allocation unchanged!\n");
			return 0;
		}
		(c->stk-1)->s = ptr;
		return 0;
	}
	*/                               // end of re2c block
	break;
	
	default:
	return 1;
	}
}

static int lex(u8 * YYCURSOR, Context * c) // YYCURSOR is defined as a function parameter
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
	//printf("start of loop\n");
	//print_code((YYCURSOR-0), 5);
	//printf("Hello\n");

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		u8 *s=start;
		u8 *f=YYCURSOR;
		//printf("char is %d, length %d \n", *s, YYCURSOR-start);
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
		printf("\n");
		goto loop; 
	} //   default rule with its semantic action start =YYCURSOR;
	[\x03] { return 0; }             // EOF rule with 0x03 sentinal
	
	[\x04] { // inside case statement, end of if
		// restore normal ending
		*(YYCURSOR-1)='}';
		// leave current case
		YYCURSOR = (c->cstk-1)->s;
		// clear top two cntrl stacks entries
		c->cstk-=2;
		goto loop;
	}            
	
	wsp {
		//~ while (start!=YYCURSOR){
			//~ if(*start=='\n'){
				//~ *line_num+=1;
				//~ //printf("wsp, %d\n", *line_num);
			//~ }
			//~ start++;
		//~ }
		if(c->is_step==2)
		{
			c->is_step = 1;
		}
		goto loop;
	}
 
	integer {
		c->stk->i = strtol( (const char *)start , NULL, 0);
		STACK_CHECK_UP(1)
		c->stk++;
		goto loop;
	}
	
	flt {
		c->stk->d = atof( (const char *)start );
		STACK_CHECK_UP(1)
		c->stk++;
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
		STACK_CHECK_UP(1)
		c->stk++;
		goto loop;
	}
	
	mangled_string_lit {
		STACK_CHECK_UP(1)
		c->stk->s = start+1;
		c->stk++;
		goto loop;
	}

	char_lit {
		goto loop;
	}

	";" { // end of function default return
		// pop command stack
		c->cstk--;
		// goto to return address
		YYCURSOR = (u8 *)(((u64)c->cstk->s)&0x7FFFFFFFFFFFFFFF);
		// leave scope
		leave_scope();
		goto loop;
	}
	
	"}" {
		goto loop;
	}
	
	":" {
		STACK_CHECK_UP(1)
		c->stk->s = YYCURSOR;
		c->stk++;
		YYCURSOR-=lex_if_else(&YYCURSOR, 2, 0); // skip definition
		goto loop;
	}

	"&" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i&c->stk->i;
		goto loop;
	}

	"^" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i^c->stk->i;
		goto loop;
	}

	"|" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i|c->stk->i;
		goto loop;
	}

	"==" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = ((c->stk-1)->i)==(c->stk->i);
		goto loop;
	}

	"!=" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i!=c->stk->i;
		goto loop;
	}

	"<" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i<c->stk->i;
		goto loop;
	}

	">" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i>c->stk->i;
		goto loop;
	}

	"<=" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i<=c->stk->i;
		goto loop;
	}

	">=" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i>=c->stk->i;
		goto loop;
	}

	"<<" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i<<c->stk->i;
		goto loop;
	}

	">>" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i>>c->stk->i;
		goto loop;
	}

	"+" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i+c->stk->i;
		goto loop;
	}

	"+s" { // string concatenation
		STACK_CHECK_DOWN(-2)
		c->stk--;
		u64 string1Len = strlen((const char *)(c->stk-1)->s);
		u64 string2Len = strlen((const char *)c->stk->s);
		(c->stk-1)->s = heap_realloc((c->stk-1)->s,
						              string1Len+string2Len+1 );
		memmove(((c->stk-1)->s+string1Len), c->stk->s, string2Len+1);
		goto loop;
	}

	"-" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i-c->stk->i;
		goto loop;
	}

	"/" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i/c->stk->i;
		goto loop;
	}
	
	"*" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->i = (c->stk-1)->i*c->stk->i;
		goto loop;
	}

	"+f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d+c->stk->d;
		goto loop;
	}

	"-f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d-c->stk->d;
		goto loop;
	}

	"/f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d/c->stk->d;
		goto loop;
	}
	
	"*f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d*c->stk->d;
		goto loop;
	}
	
	"==f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = ((c->stk-1)->d)==(c->stk->d);
		goto loop;
	}

	"!=f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d!=c->stk->d;
		goto loop;
	}

	"<f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d<c->stk->d;
		goto loop;
	}

	">f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d>c->stk->d;
		goto loop;
	}

	"<=f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d<=c->stk->d;
		goto loop;
	}

	">=f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		(c->stk-1)->d = (c->stk-1)->d>=c->stk->d;
		goto loop;
	}

	"%f" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
		f64 var;
		var = (c->stk-1)->d/c->stk->d;
		var = modf(var, &(c->stk-1)->d);
		(c->stk-1)->d = var * c->stk->d;
		goto loop;
	}

	"%" {
		STACK_CHECK_DOWN(-2)
		c->stk--;
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
	
	".f" {
		for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
			printf("(%f) ",c->stk_start[x].d);
		}
		printf("\n");
		goto loop;
	}
	
	"~" {
		STACK_CHECK_DOWN(-1)
		(c->stk-1)->i = ~((c->stk-1)->i);
		goto loop;
	}

	"=" { // assignment
		c->word_flags = 1;
		goto loop;
	}

	"=:" { // assignment of constant
		c->word_flags = 3;
		goto loop;
	}

	"+=" { // assignment
		c->word_flags = 4;
		goto loop;
	}
	
	"-=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x10;
		goto loop;
	}

	"*=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x20;
		goto loop;
	}

	"/=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x30;
		goto loop;
	}

	"%=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x40;
		goto loop;
	}

	"&=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x50;
		goto loop;
	}

	"|=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x60;
		goto loop;
	}

	"^=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x70;
		goto loop;
	}

	">>=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x80;
		goto loop;
	}

	"<<=" { // assignment
		c->word_flags = 4;
		c->word_flags |= 0x90;
		goto loop;
	}

	"@" {
		c->word_flags = 2;
		goto loop;
	}

	// new work flow. Check locals if any. Check functions. Check globals. Nothing found.
	word {
		//get_function_addr
		s64 *varP, tmp;
		s32 res;
		u8 flags;
		if (lex_word(start, c, &YYCURSOR, (YYCURSOR - start))){
			switch(c->word_flags&0x0F){
				case 0: // no flags
				// check locals
				if(scope_index>0&&(vars[scope_index]!=0)){
					res = get_variable(start, (YYCURSOR - start), &c->stk->i, scope_index);
					if (res!=0){
						STACK_CHECK_UP(1)
						c->stk++;
						goto loop;
					}
				}
				// else
				// check if its a function
				c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start));
				if (c->stk->s!=0){
					// IT IS A FUNCTION!!!
					// save off return
					c->cstk->s = (u8 *)(((u64)YYCURSOR)|0x8000000000000000);
					c->cstk++;
					// jump to function
					YYCURSOR = c->stk->s;
					// enter scope
					enter_scope();
					goto loop;
				}
				// else
				// check if its a global
				res = get_variable(start, (YYCURSOR - start), &c->stk->i, 0);
				if (res!=0){
					STACK_CHECK_UP(1)
					c->stk++;
					goto loop;
				}
				// nothing
				printf("Cannot find identifier as a local/function/global name!!!");
				print_code(start, (YYCURSOR - start));
				printf("\n");
				break;
				case 1: // assignment
				c->word_flags=0;
				STACK_CHECK_DOWN(-1)
				c->stk--;
				// will try to insert unique name, if fails will update value only
				save_variable(start, (YYCURSOR - start), c->stk->i, 0);
				break;
				case 2: // get address
				c->word_flags=0;
				// check locals
				if(scope_index>0&&(vars[scope_index]!=0)){
					c->stk->s = (u8 *)get_variable_addr(start, (YYCURSOR - start), scope_index, &tmp);
					if (c->stk->s!=0){
						STACK_CHECK_UP(1)
						c->stk++;
						goto loop;
					}
				}
				// else
				// check if its a function
				c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start));
				if (c->stk->s!=0){
					STACK_CHECK_UP(1)
					c->stk++;
					goto loop;
				}
				// else
				// check if its a global
				c->stk->s = (u8 *)get_variable_addr(start, (YYCURSOR - start), 0, &tmp);
				if (c->stk->s!=0){
					STACK_CHECK_UP(1)
					c->stk++;
					goto loop;
				}
				// nothing
				printf("Cannot find identifier as a local/function/global name!!!");
				print_code(start, (YYCURSOR - start));
				printf("\n");
				break;
				case 3: // assignment of constant
				c->word_flags=0;
				STACK_CHECK_DOWN(-1)
				c->stk--;
				// will try to insert unique name, if fails will update value only
				save_variable(start, (YYCURSOR - start), c->stk->i, ((u64)0x80)<<56);
				break;
				case 4: // plus equals operator
				flags=c->word_flags;
				c->word_flags=0;
				STACK_CHECK_DOWN(-1)
				c->stk--;
				varP=0;
				// check locals
				if(scope_index>0&&(vars[scope_index]!=0)){
					varP = get_variable_addr(start, (YYCURSOR - start), scope_index, &tmp);
				}
				// else
				// check if its a global
				if(varP==0){
					varP = get_variable_addr(start, (YYCURSOR - start), 0, &tmp);
				}
				// nothing
				if (varP==0){
					printf("Cannot find identifier as a local/global name!!!");
					print_code(start, (YYCURSOR - start));
					printf("\n");
					goto loop;
				}
				// constant
				if((tmp&0x8000000000000000)!=0){
					printf("Cannot change constant: ");
					print_code(start, (YYCURSOR - start));
					printf("\n");
					goto loop;
				}
				// else
				// load -> modify -> store
				tmp = *varP; // load
				// modify
				switch(flags>>4){
					case 0:
					tmp = tmp+c->stk->i;
					break;
					case 1:
					tmp = tmp-c->stk->i;
					break;
					case 2:
					tmp = tmp*c->stk->i;
					break;
					case 3:
					tmp = tmp/c->stk->i;
					break;
					case 4:
					tmp = tmp%c->stk->i;
					break;
					case 5:
					tmp = tmp&c->stk->i;
					break;
					case 6:
					tmp = tmp|c->stk->i;
					break;
					case 7:
					tmp = tmp^c->stk->i;
					break;
					case 8:
					tmp = tmp>>c->stk->i;
					break;
					case 9:
					tmp = tmp<<c->stk->i;
					break;
				}
				// store
				*varP = tmp;
				break;
			}
		}
		goto loop;
	}
	
	function_definition {
		save_function_addr(start, (YYCURSOR - start-1), YYCURSOR);
		YYCURSOR-=lex_if_else(&YYCURSOR, 2, 0); // skip definition
		goto loop;
	}
	*/                               // end of re2c block
}  



