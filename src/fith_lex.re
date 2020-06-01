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
	hex = "0x" [0-9A-F]+; // a-f removed
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
	u8 *YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ u8 *YYCURSOR;    // YYCURSOR is defined as a local variable
	u8 *start;
	u8 *finish;
	u32 word_len=0;
	u32 num_ifs=0;
	u32 num_funcs=0;
	u32 num_loops=0;
	u32 num_case=0;
	
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer
	start = YYCURSOR;
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

	word {
		word_len = YYCURSOR-start;
		finish = YYCURSOR;
		goto word_processing;
	}

	*/                               // end of re2c block
	
	word_processing:
	switch(word_len){
	case 2: // 2 letter words
	YYCURSOR = start;
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		YYCURSOR = finish;
		goto loop;
	}

	"do" {
		num_loops++;
		goto loop;
	}
	*/                               // end of re2c block
	break;
	case 3: // 3 letter words
	YYCURSOR = start;
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		YYCURSOR = finish;
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
	*/                               // end of re2c block
	break;
	case 4: // 4 letter words
	YYCURSOR = start;
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		YYCURSOR = finish;
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
	"else" {
		if ( (num_ifs==0) && (is_else==0) ){
			*YYCURSOR_p = YYCURSOR;
			return 0;
		}
		goto loop;
	}
	*/                               // end of re2c block
	break;
	default: goto loop;
	}
}

// sub-lexxer for dealing with word.
static Registers lex_word(Context1 *c, Registers r, u8 **YYCURSORout, u64 len) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	//u8 * start;
	
	c->is_customWord =0;
	YYCURSOR = c->source_code;

//loop: // label for looping within the lexxer
	//start = YYCURSOR;
	
	switch(len-1){
	case 0: // 1 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		c->is_customWord =1;
		return r;
	} //   default rule with its semantic action start =YYCURSOR;

	"p" {
		STACK_CHECK_DOWN_R(-1)
		return print_string(c,r);
	}
	*/                               // end of re2c block
	break;
	case 1: // 2 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		c->is_customWord =1;
		return r;
	} //   default rule with its semantic action start =YYCURSOR;

	"or" { // "||"
		STACK_CHECK_DOWN_R(-2)
		return logical_or(c,r);
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
		return r;
	}
	*/                               // end of re2c block
	break;
	case 2: // 3 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		c->is_customWord =1;
		return r;
	} //   default rule with its semantic action start =YYCURSOR;

	"and" { // "&&"
		STACK_CHECK_DOWN_R(-2)
		return logical_and(c,r);
	}
	"not" {
		STACK_CHECK_DOWN_R(-1)
		return logical_not(c,r);
	}
	"dup" {
		STACK_CHECK_DOWN_R(-1)
		STACK_CHECK_UP_R(1)
		//~ r.sp->i = r.tos.i;
		//~ r.sp++;
		return _fith_dup(c,r);
	}
	"dep" {
		STACK_CHECK_UP_R(1)
		return _fith_dep(c,r);
	}
	"rot" {
		STACK_CHECK_DOWN_R(-3)
		return _fith_rot(c,r);
	}
	"abs" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_abs(c,r);
	}
	"i2f" {
		STACK_CHECK_DOWN_R(-1)
		r.tos.d=r.tos.i;
		return r;
	}
	"f2i" {
		STACK_CHECK_DOWN_R(-1)
		r.tos.i=r.tos.d;
		return r;
	}
	"i2s" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_i2s(c,r);
	}
	"f2s" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_f2s(c,r);
	}
	"s2i" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_s2i(c,r);
	}
	"s2f" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_s2f(c,r);
	}
	"rev" {
		STACK_CHECK_DOWN_R(-1)
		if( (r.sp - c->stk_start-1) < r.tos.i ){
			printf("rev : stack underflow avoided!!!\n");
			return r;
		}
		return _fith_rev(c,r);
	}
	"if{" {
		STACK_CHECK_DOWN_R(-1)
		r.sp--;
		s64 tmp = r.tos.i;
		r.tos.i = r.sp->i;
		if(tmp==0){ // false
			if(c->in_case){
				// "duplicate" value for next test
				// pushes value into range of stack
				r.sp++;
			}
			YYCURSOR-=lex_if_else(&YYCURSOR, 0, 0);
			*YYCURSORout=YYCURSOR;
		} else if(c->in_case){ // true
			// clear flag
			c->in_case=0;
			// re-write ending for case statement
			lex_if_else(&YYCURSOR, 0, 1);
			// drop auxilary test value
			r.sp--;
			r.tos.i = r.sp->i;
		}
		return r;
	}
	"end" {
		// clear flag
		c->in_case=0;
		c->cstk-=2;
		return r;
	}
	*/                               // end of re2c block
	break;
	case 3: // 4 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		c->is_customWord =1;
		return r;
	} //   default rule with its semantic action start =YYCURSOR;

	"call" {
		STACK_CHECK_DOWN_R(-1)
		r.sp--;
		// save off return
		c->cstk->s = (u8 *)(((u64)YYCURSOR)|0x8000000000000000);
		c->cstk++;
		// jump to function
		*YYCURSORout = r.tos.s;
		r.tos.i = r.sp->i;
		// enter scope
		enter_scope();
		return r;
	}
	"load" {
		STACK_CHECK_DOWN_R(-1)
		r.sp--;
		u8 *tmp = load_file(r.tos.s,1);
		r.tos.i = r.sp->i;
		// save off return in command stack
		c->cstk->s = YYCURSOR;
		c->cstk++;
		*YYCURSORout = tmp;
		return r;
	}
	"over" {
		STACK_CHECK_DOWN_R(-2)
		STACK_CHECK_UP_R(1)
		return _fith_over(c,r);
	}
	"pick" {
		STACK_CHECK_DOWN_R(-3)
		STACK_CHECK_UP_R(1)
		return _fith_pick(c,r);
	}
	"drop" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_drop(c,r);
	}
	"swap" {
		STACK_CHECK_DOWN_R(-2)
		return _fith_swap(c,r);
	}
	"else" {
		YYCURSOR-=lex_if_else(&YYCURSOR, 1, 0);
		*YYCURSORout=YYCURSOR;
		return r;
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
			*YYCURSORout = r.tos.s;
			r.sp-=1;
			r.tos.i = r.sp->i;
			// enter scope
			enter_scope();
		} else {
			r.tos.i = res;
		}
		return r;
	}
	"exit" {
		_Exit((r.sp-1)->i);
	}
	"free" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_free(c,r);
	}
	"sort" {
		STACK_CHECK_DOWN_R(-1)
		if( (r.sp - c->stk_start) < r.tos.i ){
			printf("stack underflow avoided!!!\n");
			return r;
		}
		return _fith_drop(c,r);
	}
	"fabs" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_fabs(c,r);
	}
	"chan" {
		STACK_CHECK_UP_R(2)
		r = _fith_chan(c,r);
		return r;
	}
	"loop" {
		*YYCURSORout = (c->cstk-2)->s;
		return r;
	}
	"case" {
		// duplicate test value if there
		STACK_CHECK_DOWN_R(-1)
		STACK_CHECK_UP_R(1)
		r.sp->i = r.tos.i;
		r.sp++;
		// save top of case
		c->cstk->s = YYCURSOR;
		c->cstk++;
		// find end of case
		YYCURSOR-=lex_if_else(&YYCURSOR, 4, 0);
		// save end of loop
		c->cstk->s = YYCURSOR;
		c->cstk++;
		c->in_case=1;
		return r;
	}
	*/                               // end of re2c block
	break;
	case 4: // 5 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		c->is_customWord =1;
		return r;
	} //   default rule with its semantic action start =YYCURSOR;

	"clear" {
		return _fith_clear(c,r);
	}
	"sleep" {
		STACK_CHECK_DOWN_R(-2)
		return _fith_sleep(c,r);
	}
	//~ "array" {
		//~ STACK_CHECK_DOWN_R(-1)
		//~ return _fith_array(c,r);
	//~ }
	"reads" { // (0fd, 1pBuf, TOS=sizeof(pBuf))
		STACK_CHECK_DOWN_R(-3)
		r.sp-=2;
		s32 tmp = read(r.sp->i, (r.sp+1)->s, r.tos.i);
		if (tmp < 0)
		{
			printf("reads error!!!\n");
		}
		r.tos.s = (r.sp+1)->s;
		return r;
	}
	"leave" {
		// clear flag
		c->in_case=0;
		// leave current loop
		*YYCURSORout = (c->cstk-1)->s;
		// clear top two cntrl stacks entries
		c->cstk-=2;
		return r;
	}
	"again" {
		*YYCURSORout = (c->cstk-2)->s;
		return r;
	}
	"strdup" { // might move to a library
		STACK_CHECK_DOWN_R(-1)
		u8 *buff;
		u64 size;
		size = strlen((const char *)r.tos.s)+1;
		buff = malloc(size);
		memmove(buff, r.tos.s, size);
		r.tos.s = buff;
		return r;
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
		(r.sp+1)->s=c->yycur;
		(r.sp+2)->s=c->yycur;
		//save off character value
		(r.sp+3)->i = *c->yycur;
		*c->yycur=')';
		
		// find begining and ending of line
		//if (*line_num>1) {
		while (*(r.sp+1)->s!='\n'){
			(r.sp+1)->s--;
		}
		(r.sp+1)->s++;
		//}
		while (*(r.sp+2)->s!='\n'){
				(r.sp+2)->s++;
		}
		(r.sp+2)->s++;
		//(r.sp+3)->i = *(r.sp+2)->s;
		//*(r.sp+2)->s=0;
		
		//printf("%s", (r.sp+1)->s);
		print_code((r.sp+1)->s, (r.sp+2)->s-(r.sp+1)->s);
		//*(r.sp+2)->s=(r.sp+3)->i;
		*c->yycur=(r.sp+3)->i;
		
		if(c->is_fp)
		{
			for(int x =0 ; &c->stk_start[x]!=r.sp;x++){
				printf("[%f] ",c->stk_start[x].d);
			}
			printf("\n");
		} else {
			for(int x =0 ; &c->stk_start[x]!=r.sp;x++){
				printf("[%ld] ",c->stk_start[x].i);
			}
			printf("\n");
		}
		
		while (1)
		{
			raw_begin();
			(r.sp+4)->i = fith_fgets(c->file_name_buff, 512, c->out);
			raw_end();
			//if (fgets ((char *)c->file_name_buff, 512, stdin) != NULL )
			if ((r.sp+4)->i )
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
					return r;
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
					return r;
				}
				if (!(strncmp((const char *)c->file_name_buff, ".s", 2)))
				{
					*YYCURSORout = c->yycur;
					c->is_step=1;
					return r;
				}
				c->out = c->buff;
				(r.sp+5)->i=0;
				if((*c->file_name_buff!='\n')&&(*c->file_name_buff!='.')){
					while(c->file_name_buff[(r.sp+5)->i]!=3){
						*c->buff=c->file_name_buff[(r.sp+5)->i];
						(r.sp+5)->i++;
						c->buff++;
					}
				}
				//c->buff = (u8*)stpcpy((char *)c->buff, (const char *)c->file_name_buff);
				(r.sp+5)->s=(u8*)stpcpy((char *)c->buff, " debug");
				*(r.sp+5)->s=3;
				*YYCURSORout = c->out;
				return r;
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
		c->is_customWord =1;
		return r;
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
		return r;
	}
	"malloc" {
		STACK_CHECK_DOWN_R(-1)
		return _fith_malloc(c,r);
	}
	"random" {
		STACK_CHECK_UP_R(1)
		return _fith_random(c,r);
	}
	"memcmp" {
		STACK_CHECK_DOWN_R(-3)
		r.sp-=2;
		s32 res = memcmp(r.sp->s,
						(r.sp+1)->s,
						r.tos.i);
		r.tos.i=res;
		return r;
	}
	"writes" { // (0fd, 1pBuf)
		STACK_CHECK_DOWN_R(-2)
		r.sp-=2;
		s32 res;
		res = write((r.sp+1)->i,
					r.tos.s,
					strlen((const char *)r.tos.s)+1);
		r.tos.i = r.sp->i;
		if (res < 0)
		{
			printf("writes error!!!\n");
		}
		return r;
	}
	*/                               // end of re2c block
	break;
	case 6: // 7 letter words
	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { // not a predefined token
		c->is_customWord =1;
		return r;
	} //   default rule with its semantic action start =YYCURSOR;

	"realloc" {
		STACK_CHECK_DOWN_R(-2)
		return _fith_realloc(c,r);
	}
	*/                               // end of re2c block
	break;
	
	default:
	c->is_customWord =1;
	return r;
	}
	
	//~ stack_up_print:
	//~ printf("stack overflow!!!\n"); return r;
	//~ stack_down_print:
	//~ printf("stack underflow!!!\n");return r;
	
}

static Registers lex(Context1 *c, Registers r, Context2 *c2) // YYCURSOR is defined as a function parameter
{                                    //
	u8 *YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	u8 *YYCURSOR;    // YYCURSOR is defined as a local variable
	u8 *start;
	
	YYCURSOR = c->source_code;

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
	[\x03] { return r; }             // EOF rule with 0x03 sentinal
	
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
		STACK_CHECK_UP(1)
		//printf("TOS: %ld \n",tos.i);
		//printf("STACK: %ld \n",r.sp->i);
		r = push_i(c, r, strtol((const char *)start, NULL, 0));
		//printf("TOS: %ld \n",r.tos.i);
		goto loop;
	}
	
	flt {
		STACK_CHECK_UP(1)
		r = push_d(c, r, atof((const char *)start));
		goto loop;
	}
	
	string_lit { 
		start++;
		u8 *string_start = start;
		// concatenate all multiline strings
		if(lex_string_lit_chain(&start))
		{
			// start is final null of created mangled_string_lit
			start++;
			// fill in with spaces until end
			while (start!=YYCURSOR){*start=' ';start++;}
		}
		STACK_CHECK_UP(1)
		r = push_s(c, r, string_start);
		goto loop;
	}
	
	mangled_string_lit {
		STACK_CHECK_UP(1)
		start++;
		r = push_s(c, r, start);
		goto loop;
	}

	char_lit {
		STACK_CHECK_UP(1)
		r = push_i(c, r, *start);
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
		r = push_s(c, r, YYCURSOR);
		YYCURSOR-=lex_if_else(&YYCURSOR, 2, 0); // skip definition
		goto loop;
	}

	"&" {
		STACK_CHECK_DOWN(-2)
		r=bitwise_and(c,r);
		goto loop;
	}

	"^" {
		STACK_CHECK_DOWN(-2)
		r=bitwise_xor(c,r);
		goto loop;
	}

	"|" {
		STACK_CHECK_DOWN(-2)
		r=bitwise_or(c,r);
		goto loop;
	}

	"==" {
		STACK_CHECK_DOWN(-2)
		r=equals(c,r);
		goto loop;
	}

	"!=" {
		STACK_CHECK_DOWN(-2)
		r=not_equals(c,r);
		goto loop;
	}

	"<" {
		STACK_CHECK_DOWN(-2)
		r=less_than(c,r);
		goto loop;
	}

	">" {
		STACK_CHECK_DOWN(-2)
		r=greater_than(c,r);
		goto loop;
	}

	"<=" {
		STACK_CHECK_DOWN(-2)
		r=less_than_orEquals(c,r);
		goto loop;
	}

	">=" {
		STACK_CHECK_DOWN(-2)
		r=greater_than_orEquals(c,r);
		goto loop;
	}

	"<<" {
		STACK_CHECK_DOWN(-2)
		r=shift_left(c,r);
		goto loop;
	}

	">>" {
		STACK_CHECK_DOWN(-2)
		r=shift_left(c,r);
		goto loop;
	}

	"+" {
		STACK_CHECK_DOWN(-2)
		r=add(c,r);
		goto loop;
	}

	"+s" { // string concatenation
		stringCat:
		STACK_CHECK_DOWN(-2)
		r.sp--;
		u8 *ret;
		//u8 *readthis = (r.sp-1);
		u64 string1Len = strlen((const char *)(r.sp-1)->s);
		u64 string2Len = strlen((const char *)r.sp->s);
		//printf("Dead here1?\n");
		ret = heap_realloc((r.sp-1)->s,
						              string1Len+string2Len+1 );
		if (ret==0){/*YYCURSOR=start;*/r.sp++;goto stringCat;}
		
		//printf("Dead here2?\n");
		//printf("Dead here3?\n");
		(r.sp-1)->s = ret;
		//printf("Dead here3?\n");
		memmove(((r.sp-1)->s+string1Len), r.sp->s, string2Len+1);
		//printf("Dead here4?\n");
		goto loop;
	}

	"-" {
		STACK_CHECK_DOWN(-2)
		r=sub(c,r);
		goto loop;
	}

	"/" {
		STACK_CHECK_DOWN(-2)
		r=divi(c,r);
		goto loop;
	}
	
	"*" {
		STACK_CHECK_DOWN(-2)
		r=mul(c,r);
		goto loop;
	}

	"+f" {
		STACK_CHECK_DOWN(-2)
		r=addf(c,r);
		goto loop;
	}

	"-f" {
		STACK_CHECK_DOWN(-2)
		r=subf(c,r);
		goto loop;
	}

	"/f" {
		STACK_CHECK_DOWN(-2)
		r=divif(c,r);
		goto loop;
	}
	
	"*f" {
		STACK_CHECK_DOWN(-2)
		r=mulf(c,r);
		goto loop;
	}
	
	"==f" {
		STACK_CHECK_DOWN(-2)
		r=equalsf(c,r);
		goto loop;
	}

	"!=f" {
		STACK_CHECK_DOWN(-2)
		r=not_equalsf(c,r);
		goto loop;
	}

	"<f" {
		STACK_CHECK_DOWN(-2)
		r=less_thanf(c,r);
		goto loop;
	}

	">f" {
		STACK_CHECK_DOWN(-2)
		r=greater_thanf(c,r);
		goto loop;
	}

	"<=f" {
		STACK_CHECK_DOWN(-2)
		r=less_than_orEqualsf(c,r);
		goto loop;
	}

	">=f" {
		STACK_CHECK_DOWN(-2)
		r=greater_than_orEqualsf(c,r);
		goto loop;
	}

	"%f" {
		STACK_CHECK_DOWN(-2)
		r=modulof(c,r);
		goto loop;
	}

	"%" {
		STACK_CHECK_DOWN(-2)
		r=modulo(c,r);
		goto loop;
	}

	"." {
		r=print_stack(c,r);
		goto loop;
	}
	
	".f" {
		r=print_stackf(c,r);
		goto loop;
	}
	
	"~" {
		STACK_CHECK_DOWN(-1)
		r=bitwise_not(c,r);
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
	
	"[]" {
		STACK_CHECK_DOWN(-2)
		r=_fith_array_get(c,r);
		goto loop;
	}

	"=[]" {
		STACK_CHECK_DOWN(-3)
		r=_fith_array_set(c,r);
		goto loop;
	}

	"[h]" {
		STACK_CHECK_DOWN_R(-1)
		r=_fith_array(c,r);
		goto loop;
	}

	"[].len" {
		STACK_CHECK_DOWN_R(-1)
		r=_fith_array_len(c,r);
		goto loop;
	}

	// new work flow. Check locals if any. Check functions. Check globals. Nothing found.
	word {
		//get_function_addr
		s64 *varP, tmp, val;
		u8 *ptr;
		s32 res;
		u8 flags;
		c->source_code = start;
		r = lex_word(c, r, &YYCURSOR, (YYCURSOR - start));
		if ( c->is_customWord ){
			switch(c->word_flags&0x0F){
				case 0: // no flags
				// check locals
				if(scope_index>0&&(vars[scope_index]!=0)){
					res = get_variable(start, (YYCURSOR - start), &val, scope_index);
					if (res!=0){
						STACK_CHECK_UP(1)
						r.sp->i = r.tos.i;
						r.tos.i = val;
						r.sp++;
						goto loop;
					}
				}
				// else
				// check if its a function
				ptr = (u8 *)get_function_addr(start, (YYCURSOR - start));
				if (ptr!=0){
					// IT IS A FUNCTION!!!
					// save off return
					c->cstk->s = (u8 *)(((u64)YYCURSOR)|0x8000000000000000);
					c->cstk++;
					// jump to function
					YYCURSOR = ptr;
					// enter scope
					enter_scope();
					goto loop;
				}
				// else
				// check if its a global
				res = get_variable(start, (YYCURSOR - start), &val, 0);
				if (res!=0){
					STACK_CHECK_UP(1)
					r.sp->i = r.tos.i;
					r.tos.i = val;
					r.sp++;
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
				r.sp--;
				// will try to insert unique name, if fails will update value only
				save_variable(start, (YYCURSOR - start), r.tos.i, 0);
				r.tos.i = r.sp->i;
				break;
				case 2: // get address
				c->word_flags=0;
				// check locals
				if(scope_index>0&&(vars[scope_index]!=0)){
					ptr = (u8 *)get_variable_addr(start, (YYCURSOR - start), scope_index, &tmp);
					if (ptr!=0){
						STACK_CHECK_UP(1)
						r.sp->i = r.tos.i;
						r.tos.s = ptr;
						r.sp++;
						goto loop;
					}
				}
				// else
				// check if its a function
				ptr = (u8 *)get_function_addr(start, (YYCURSOR - start));
				if (ptr!=0){
					STACK_CHECK_UP(1)
					r.sp->i = r.tos.i;
					r.tos.s = ptr;
					r.sp++;
					goto loop;
				}
				// else
				// check if its a global
				ptr = (u8 *)get_variable_addr(start, (YYCURSOR - start), 0, &tmp);
				if (ptr!=0){
					STACK_CHECK_UP(1)
					r.sp->i = r.tos.i;
					r.tos.s = ptr;
					r.sp++;
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
				r.sp--;
				// will try to insert unique name, if fails will update value only
				save_variable(start, (YYCURSOR - start), r.tos.i, ((u64)0x80)<<56);
				r.tos.i = r.sp->i;
				break;
				case 4: // plus equals operator
				flags=c->word_flags;
				c->word_flags=0;
				STACK_CHECK_DOWN(-1)
				r.sp--;
				val = r.tos.i;
				r.tos.i = r.sp->i;
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
					tmp = tmp+val;
					break;
					case 1:
					tmp = tmp-val;
					break;
					case 2:
					tmp = tmp*val;
					break;
					case 3:
					tmp = tmp/val;
					break;
					case 4:
					tmp = tmp%val;
					break;
					case 5:
					tmp = tmp&val;
					break;
					case 6:
					tmp = tmp|val;
					break;
					case 7:
					tmp = tmp^val;
					break;
					case 8:
					tmp = tmp>>val;
					break;
					case 9:
					tmp = tmp<<val;
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

	stack_up_print:
	printf("stack overflow!!!\n"); goto loop;
	stack_down_print:
	printf("stack underflow!!!\n"); goto loop;
}



