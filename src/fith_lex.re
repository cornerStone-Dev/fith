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
	word = [a-zA-Z_]([a-zA-Z_0-9?!#.[-]|"]")*;
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

static s64 lex_if_else(/*const*/ u8 ** YYCURSOR_p, u32 is_else) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	/*const*/ //u8 * start;
	u32 num_ifs=0;
	u32 num_funcs=0;
	
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
		DECREMENT_STACK
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
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i||c->stk->i;
		return 0;
	}
	"if" {
		DECREMENT_STACK
		if(c->stk->i==0){
			YYCURSOR-=lex_if_else(&YYCURSOR, 0);
			*YYCURSORout=YYCURSOR;
		}
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
		DECREMENT_STACK
		(c->stk-1)->i = (c->stk-1)->i&&c->stk->i;
		return 0;
	}
	"not" {
		(c->stk-1)->i = !((c->stk-1)->i);
		return 0;
	}
	"dup" {
		c->stk->i = (c->stk-1)->i;
		INCREMENT_STACK
		return 0;
	}
	"dep" {
		c->stk->i = c->stk-c->stk_start;
		INCREMENT_STACK
		return 0;
	}
	"rot" {
		c->stk->i = (c->stk-3)->i;
		(c->stk-3)->i = (c->stk-2)->i;
		(c->stk-2)->i = (c->stk-1)->i;
		(c->stk-1)->i = c->stk->i;
		return 0;
	}
	"abs" {
		(c->stk-1)->i=labs((c->stk-1)->i);
		return 0;
	}
	"i2f" {
		(c->stk-1)->d=(c->stk-1)->i;
		return 0;
	}
	"i2s" {
		// save off value
		c->stk->i=(c->stk-1)->i;
		// get string
		(c->stk-1)->s = heap_malloc(24);
		sprintf((char *)(c->stk-1)->s, "%ld", c->stk->i);
		return 0;
	}
	"f2i" {
		(c->stk-1)->i=(c->stk-1)->d;
		return 0;
	}
	"f2s" {
		// save off value
		c->stk->d=(c->stk-1)->d;
		// get string
		(c->stk-1)->s = heap_malloc(24);
		sprintf((char *)(c->stk-1)->s, "%f", c->stk->d);
		return 0;
	}
	"s2i" {
		(c->stk-1)->i = strtol( (const char *)(c->stk-1)->s, NULL, 0);
		return 0;
	}
	"s2f" {
		(c->stk-1)->d = atof( (const char *)(c->stk-1)->s );
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
		DECREMENT_STACK
		// save off return
		c->cstk->s = YYCURSOR;
		c->cstk++;
		// jump to function
		*YYCURSORout = c->stk->s;
		return 0;
	}
	"load" {
		DECREMENT_STACK
		(c->stk+1)->s = load_file(c->stk->s,1);
		// save off return in command stack
		c->cstk->s = YYCURSOR;
		c->cstk++;
		*YYCURSORout = (c->stk+1)->s;
		return 0;
	}
	"over" {
		c->stk->i = (c->stk-2)->i;
		INCREMENT_STACK
		return 0;
	}
	"pick" {
		c->stk->i = (c->stk-3)->i;
		INCREMENT_STACK
		return 0;
	}
	"drop" {
		DECREMENT_STACK
		return 0;
	}
	"swap" {
		c->stk->i = (c->stk-2)->i;
		(c->stk-2)->i = (c->stk-1)->i;
		(c->stk-1)->i = c->stk->i;
		return 0;
	}
	"else" {
		YYCURSOR-=lex_if_else(&YYCURSOR, 1);
		*YYCURSORout=YYCURSOR;
		return 0;
	}
	"then" {
		return 0;
	}
	"fork" {
		// fork executes a function in another process. The parent process will
		// skip the next function call. The child process need to have the
		// return stack modified to exit, returns pid
		s32 res = fork();
		if (res < 0){
			printf("Fork error!!!\n");
		} else if (res == 0) {
			c->cstk->s = (u8*)"exit";
			c->cstk++;
			// jump to function
			*YYCURSORout = (c->stk-1)->s;
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
		DECREMENT_STACK
		free(c->stk->s);
		return 0;
	}
	"sort" {
		DECREMENT_STACK
		if(c->stk-c->stk_start<c->stk->i){
			printf("stack underflow avoided!!!\n");
			return 0;
		}
		MERGE_SORT_s64((s64 *)(c->stk-c->stk->i), c->stk->i);
		return 0;
	}
	"fabs" {
		(c->stk-1)->d=fabs((c->stk-1)->d);
		return 0;
	}
	"chan" {
		s32 res = socketpair(AF_UNIX, SOCK_STREAM, 0, (c->stk+2)->fd);
		if (res < 0)
		{
			printf("socketpair error!!!\n");
		} else {
			c->stk->i = (c->stk+2)->fd[0];
			(c->stk+1)->i = (c->stk+2)->fd[1];
			INCREMENT_STACK
			INCREMENT_STACK
		}
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
	"begin" {
		c->cstk->s = YYCURSOR;
		c->cstk++;
		return 0;
	}
	"until" {
		DECREMENT_STACK
		if(c->stk->i==0){
			*YYCURSORout = (c->cstk-1)->s;
		} else {
			c->cstk--;
		}
		return 0;
	}
	"sleep" {
		DECREMENT_STACK
		DECREMENT_STACK
		c->time.tv_sec = (c->stk)->i;
		c->time.tv_nsec = (c->stk+1)->i;
		nanosleep(&c->time, NULL);
		return 0;
	}
	"array" {
		// allocate array with padding for 7 byte header (memory log =1 byte)
		(c->stk-1)->s = malloc(((c->stk-1)->i*8)+7);
		// tag header with invalid utf-8
		*(c->stk-1)->s = 0xFF;
		return 0;
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
		// goto to return address
		*YYCURSORout = c->cstk->s;
		return 0;
	}
	"malloc" {
		(c->stk-1)->s= malloc((c->stk-1)->i);
		if ((c->stk-1)->s == 0 )
		{
			printf("malloc error!!!\n");
		}
		return 0;
	}
	"random" {
		//sqlite3_randomness(8, &c->stk->i);
		c->stk->i = (s64)( ((u64)((*(c->stk->s))+3)) * ((*(c->stk->s+1))+3) * ((*(c->stk->s+2))+3)
		 * ((*(c->stk->s+3))+3) * ((*(c->stk->s+4))+3) * ((*(c->stk->s+5))+3)
		  * ((*(c->stk->s+6))+3) * ((*(c->stk->s+7))+3) );
		INCREMENT_STACK
		return 0;
	}
	"memcmp" {
		DECREMENT_STACK
		DECREMENT_STACK
		s32 res = memcmp((c->stk-1)->s,
											c->stk->s,
											(c->stk+1)->i);
		(c->stk-1)->i=res;
		return 0;
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
		(c->stk-1)->s= realloc ((c->stk-2)->s, (c->stk-1)->i);
		if ((c->stk-1)->s == 0 )
		{
			printf("realloc error!!!\n");
		}
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
			printf("YYCURSOR::%s\n",YYCURSOR);
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
	
	//~ "call" {
		//~ DECREMENT_STACK
		//~ // save off return
		//~ c->cstk->s = YYCURSOR;
		//~ c->cstk++;
		//~ // jump to function
		//~ YYCURSOR = c->stk->s;
		//~ goto loop;
	//~ }
	
	//~ "load" {
		//~ DECREMENT_STACK
		//~ (c->stk+1)->s = load_file(c->stk->s,1);
		//~ // save off return in command stack
		//~ c->cstk->s = YYCURSOR;
		//~ c->cstk++;
		//~ YYCURSOR = (c->stk+1)->s;
		//~ goto loop;
	//~ }
	
	//~ "return" {
		//~ // pop command stack
		//~ c->cstk--;
		//~ // goto to return address
		//~ YYCURSOR = c->cstk->s;
		//~ goto loop;
	//~ }
	
	//~ "p" {
		//~ DECREMENT_STACK
		//~ if(((*c->stk->s)&0xC0)==0x80)
		//~ {
			//~ lex_printFSON(c->stk->s);
			
		//~ } else {
			//~ printf("%s",(const char *)c->stk->s);
		//~ }
		//~ printf("%s",(const char *)c->stk->s);
		//~ printf("\n");
		//~ goto loop;
	//~ }

	//~ "or" { // "||"
		//~ DECREMENT_STACK
		//~ (c->stk-1)->i = (c->stk-1)->i||c->stk->i;
		//~ goto loop;
	//~ }

	//~ "and" { // "&&"
		//~ DECREMENT_STACK
		//~ (c->stk-1)->i = (c->stk-1)->i&&c->stk->i;
		//~ goto loop;
	//~ }

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

	"+f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d+c->stk->d;
		goto loop;
	}

	"-f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d-c->stk->d;
		goto loop;
	}

	"/f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d/c->stk->d;
		goto loop;
	}
	
	"*f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d*c->stk->d;
		goto loop;
	}
	
	"==f" {
		DECREMENT_STACK
		(c->stk-1)->d = ((c->stk-1)->d)==(c->stk->d);
		goto loop;
	}

	"!=f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d!=c->stk->d;
		goto loop;
	}

	"<f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d<c->stk->d;
		goto loop;
	}

	">f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d>c->stk->d;
		goto loop;
	}

	"<=f" {
		DECREMENT_STACK
		(c->stk-1)->d = (c->stk-1)->d<=c->stk->d;
		goto loop;
	}

	">=f" {
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
	
	".f" {
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

	"=" {
		c->word_flags = 1;
		goto loop;
	}

	"@" {
		c->word_flags = 2;
		goto loop;
	}

	//~ "not" {
		//~ (c->stk-1)->i = !((c->stk-1)->i);
		//~ goto loop;
	//~ }
	
	//~ "dup" {
		//~ c->stk->i = (c->stk-1)->i;
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	//~ "over" {
		//~ c->stk->i = (c->stk-2)->i;
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	//~ "pick" {
		//~ c->stk->i = (c->stk-3)->i;
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	//~ "drop" {
		//~ DECREMENT_STACK
		//~ goto loop;
	//~ }
	
	//~ "clear" {
		//~ c->stk=c->stk_start;
		//~ goto loop;
	//~ }
	
	//~ "dep" {
		//~ c->stk->i = c->stk-c->stk_start;
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	//~ "swap" {
		//~ c->stk->i = (c->stk-2)->i;
		//~ (c->stk-2)->i = (c->stk-1)->i;
		//~ (c->stk-1)->i = c->stk->i;
		//~ goto loop;
	//~ }
	
	//~ "rot" {
		//~ c->stk->i = (c->stk-3)->i;
		//~ (c->stk-3)->i = (c->stk-2)->i;
		//~ (c->stk-2)->i = (c->stk-1)->i;
		//~ (c->stk-1)->i = c->stk->i;
		//~ goto loop;
	//~ }

	//~ "if" {
		//~ DECREMENT_STACK
		//~ if(c->stk->i==0){
			//~ YYCURSOR-=lex_if_else(&YYCURSOR, 0);
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "else" {
		//~ YYCURSOR-=lex_if_else(&YYCURSOR, 1);
		//~ goto loop;
	//~ }
	
	//~ "then" {
		//~ goto loop;
	//~ }
	
	//~ "begin" {
		//~ c->cstk->s = YYCURSOR;
		//~ c->cstk++;
		//~ goto loop;
	//~ }
	
	//~ "until" {
		//~ DECREMENT_STACK
		//~ if(c->stk->i==0){
			//~ YYCURSOR = (c->cstk-1)->s;
		//~ } else {
			//~ c->cstk--;
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "sleep" {
		//~ DECREMENT_STACK
		//~ DECREMENT_STACK
		//~ c->time.tv_sec = (c->stk)->i;
		//~ c->time.tv_nsec = (c->stk+1)->i;
		//~ nanosleep(&c->time, NULL);
		//~ goto loop;
	//~ }
	
	//~ "fork" {
		//~ // fork executes a function in another process. The parent process will
		//~ // skip the next function call. The child process need to have the
		//~ // return stack modified to exit, returns pid
		//~ s32 res = fork();
		//~ if (res < 0){
			//~ printf("Fork error!!!\n");
		//~ } else if (res == 0) {
			//~ c->cstk->s = (u8*)"exit";
			//~ c->cstk++;
			//~ // jump to function
			//~ YYCURSOR = (c->stk-1)->s;
			//~ c->stk-=1;
		//~ } else {
			//~ (c->stk-1)->i = res;
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "exit" {
		//~ _Exit((c->stk-1)->i);
	//~ }
	
	//~ "malloc" {
		//~ (c->stk-1)->s= malloc((c->stk-1)->i);
		//~ if ((c->stk-1)->s == 0 )
		//~ {
			//~ printf("malloc error!!!\n");
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "realloc" {
		//~ (c->stk-1)->s= realloc ((c->stk-2)->s, (c->stk-1)->i);
		//~ if ((c->stk-1)->s == 0 )
		//~ {
			//~ printf("realloc error!!!\n");
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "free" {
		//~ DECREMENT_STACK
		//~ free(c->stk->s);
		//~ goto loop;
	//~ }
	
	//~ "array" {
		//~ // allocate array with padding for 7 byte header (memory log =1 byte)
		//~ (c->stk-1)->s = malloc(((c->stk-1)->i*8)+7);
		//~ // tag header with invalid utf-8
		//~ *(c->stk-1)->s = 0xFF;
		//~ goto loop;
	//~ }
	
	//~ "sort" {
		//~ DECREMENT_STACK
		//~ if(c->stk-c->stk_start<c->stk->i){
			//~ printf("stack underflow avoided!!!\n");
			//~ goto loop;
		//~ }
		//~ MERGE_SORT_s64((s64 *)(c->stk-c->stk->i), c->stk->i);
		//~ goto loop;
	//~ }
	
	//~ "random" {
		//~ //sqlite3_randomness(8, &c->stk->i);
		//~ c->stk->i = (s64)( ((u64)((*(c->stk->s))+3)) * ((*(c->stk->s+1))+3) * ((*(c->stk->s+2))+3)
		 //~ * ((*(c->stk->s+3))+3) * ((*(c->stk->s+4))+3) * ((*(c->stk->s+5))+3)
		  //~ * ((*(c->stk->s+6))+3) * ((*(c->stk->s+7))+3) );
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	//~ "memcmp" {
		//~ DECREMENT_STACK
		//~ DECREMENT_STACK
		//~ s32 res = memcmp((c->stk-1)->s,
											//~ c->stk->s,
											//~ (c->stk+1)->i);
		//~ (c->stk-1)->i=res;
		//~ goto loop;
	//~ }
	
	//~ "abs" {
		//~ (c->stk-1)->i=labs((c->stk-1)->i);
		//~ goto loop;
	//~ }

	//~ "fabs" {
		//~ (c->stk-1)->d=fabs((c->stk-1)->d);
		//~ goto loop;
	//~ }
	
	//~ "i2f" {
		//~ (c->stk-1)->d=(c->stk-1)->i;
		//~ goto loop;
	//~ }
	
	//~ "i2s" {
		//~ // save off value
		//~ c->stk->i=(c->stk-1)->i;
		//~ // get string
		//~ (c->stk-1)->s = heap_malloc(24);
		//~ sprintf((char *)(c->stk-1)->s, "%ld", c->stk->i);
		//~ goto loop;
	//~ }
	
	//~ "f2i" {
		//~ (c->stk-1)->i=(c->stk-1)->d;
		//~ goto loop;
	//~ }
	
	//~ "f2s" {
		//~ // save off value
		//~ c->stk->d=(c->stk-1)->d;
		//~ // get string
		//~ (c->stk-1)->s = heap_malloc(24);
		//~ sprintf((char *)(c->stk-1)->s, "%f", c->stk->d);
		//~ goto loop;
	//~ }

	//~ "s2i" {
		//~ (c->stk-1)->i = strtol( (const char *)(c->stk-1)->s, NULL, 0);
		//~ goto loop;
	//~ }
	
	//~ "s2f" {
		//~ (c->stk-1)->d = atof( (const char *)(c->stk-1)->s );
		//~ goto loop;
	//~ }
	
	//~ "chan" {
		//~ s32 res = socketpair(AF_UNIX, SOCK_STREAM, 0, (c->stk+2)->fd);
		//~ if (res < 0)
		//~ {
			//~ printf("socketpair error!!!\n");
		//~ } else {
			//~ c->stk->i = (c->stk+2)->fd[0];
			//~ (c->stk+1)->i = (c->stk+2)->fd[1];
			//~ INCREMENT_STACK
			//~ INCREMENT_STACK
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "reads" { // (0fd, 1pBuf, 2sizeof(pBuf))
		//~ DECREMENT_STACK
		//~ DECREMENT_STACK
		//~ (c->stk+2)->i = read((c->stk-1)->i, c->stk->s, (c->stk+1)->i);
		//~ if ((c->stk+2)->i < 0)
		//~ {
			//~ printf("reads error!!!\n");
		//~ }
		//~ (c->stk-1)->s = c->stk->s;
		//~ goto loop;
	//~ }
	
	//~ "writes" { // (0fd, 1pBuf)
		//~ DECREMENT_STACK
		//~ DECREMENT_STACK
		
		//~ (c->stk+3)->i = write(c->stk->i,
								//~ (c->stk+1)->s,
								//~ strlen((const char *)(c->stk+1)->s)+1);
		
		//~ if ((c->stk+3)->i < 0)
		//~ {
			//~ printf("writes error!!!\n");
		//~ }
		//~ goto loop;
	//~ }
	
	//~ "debug" {
		
		//~ if (c->yycur==0)
		//~ {
			//~ c->yycur=YYCURSOR;
		//~ }
		
		//~ if (c->is_step==3)
		//~ {
			//~ // copy back source
			//~ strncpy((char *)c->stecpot, (const char *)c->file_name_buff, 11);
			//~ c->yycur=c->stecpot;
			//~ c->is_step=0;
		//~ }
		
		//~ // save off pointers
		//~ (c->stk+1)->s=c->yycur;
		//~ (c->stk+2)->s=c->yycur;
		//~ //save off character value
		//~ (c->stk+3)->i = *c->yycur;
		//~ *c->yycur=')';
		
		//~ // find begining and ending of line
		//~ //if (*line_num>1) {
		//~ while (*(c->stk+1)->s!='\n'){
			//~ (c->stk+1)->s--;
		//~ }
		//~ (c->stk+1)->s++;
		//~ //}
		//~ while (*(c->stk+2)->s!='\n'){
				//~ (c->stk+2)->s++;
		//~ }
		//~ (c->stk+2)->s++;
		//~ //(c->stk+3)->i = *(c->stk+2)->s;
		//~ //*(c->stk+2)->s=0;
		
		//~ //printf("%s", (c->stk+1)->s);
		//~ print_code((c->stk+1)->s, (c->stk+2)->s-(c->stk+1)->s);
		//~ //*(c->stk+2)->s=(c->stk+3)->i;
		//~ *c->yycur=(c->stk+3)->i;
		
		//~ if(c->is_fp)
		//~ {
			//~ for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
				//~ printf("[%f] ",c->stk_start[x].d);
			//~ }
			//~ printf("\n");
		//~ } else {
			//~ for(int x =0 ; &c->stk_start[x]!=c->stk;x++){
				//~ printf("[%ld] ",c->stk_start[x].i);
			//~ }
			//~ printf("\n");
		//~ }
		
		//~ while (1)
		//~ {
			//~ raw_begin();
			//~ (c->stk+4)->i = fith_fgets(c->file_name_buff, 512, c->out);
			//~ raw_end();
			//~ //if (fgets ((char *)c->file_name_buff, 512, stdin) != NULL )
			//~ if ((c->stk+4)->i )
			//~ {
				//~ if (!(strncmp((const char *)c->file_name_buff, ".dump", 5)))
				//~ {
					//~ //*(c->buff) = '\000';
					//~ //printf("%s",c->buff_start);
					//~ print_code(c->buff_start, c->buff-c->buff_start);
					//~ continue;
				//~ }
				//~ if (!(strncmp((const char *)c->file_name_buff, ".exit", 5)))
				//~ {
					//~ return 0;
				//~ }
				//~ if (!(strncmp((const char *)c->file_name_buff, ".fp", 3)))
				//~ {
					//~ c->is_fp=1;
					//~ continue;
				//~ }
				//~ if (!(strncmp((const char *)c->file_name_buff, ".int", 4)))
				//~ {
					//~ c->is_fp=0;
					//~ continue;
				//~ }
				//~ if (!(strncmp((const char *)c->file_name_buff, ".resume", 7)))
				//~ {
					//~ YYCURSOR=c->yycur;
					//~ c->yycur=0;
					//~ goto loop;
				//~ }
				//~ if (!(strncmp((const char *)c->file_name_buff, ".s", 2)))
				//~ {
					//~ YYCURSOR = c->yycur;
					//~ c->is_step=1;
					//~ goto loop;
				//~ }
				//~ c->out = c->buff;
				//~ (c->stk+5)->i=0;
				//~ if((*c->file_name_buff!='\n')&&(*c->file_name_buff!='.')){
					//~ while(c->file_name_buff[(c->stk+5)->i]!=3){
						//~ *c->buff=c->file_name_buff[(c->stk+5)->i];
						//~ (c->stk+5)->i++;
						//~ c->buff++;
					//~ }
				//~ }
				//~ //c->buff = (u8*)stpcpy((char *)c->buff, (const char *)c->file_name_buff);
				//~ (c->stk+5)->s=(u8*)stpcpy((char *)c->buff, " debug");
				//~ *(c->stk+5)->s=3;
				//~ YYCURSOR = c->out;
				//~ goto loop;
			//~ }
		//~ }
	//~ }
	
	//~ var {
		//~ start+=1;
		//~ (c->stk+1)->i = get_variable(start, (YYCURSOR - start), &c->stk->i);
		//~ if ((c->stk+1)->i==0){
			//~ printf("Cannot find variable name!!!");
			//~ print_code(start, (YYCURSOR - start));
			//~ fputc ('\n', stdout);
			//~ goto loop;
		//~ }
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }

	//~ var_assign {
		//~ start+=2;
		//~ DECREMENT_STACK
		//~ // will try to insert unique name, if fails will update value only
		//~ save_variable(start, (YYCURSOR - start), c->stk->i);
		//~ goto loop;
	//~ }

	// new work flow. Check locals if any. Check functions. Check globals. Nothing found.
	word {
		//get_function_addr
		if (lex_word(start, c, &YYCURSOR, (YYCURSOR - start))){
			switch(c->word_flags){
				case 0: // no flags
				// check locals
				// TODO : STUB_FOR_LOCALS
				// else
				// check if its a function
				c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start));
				if (c->stk->s!=0){
					// IT IS A FUNCTION!!!
					// save off return
					c->cstk->s = YYCURSOR;
					c->cstk++;
					// jump to function
					YYCURSOR = c->stk->s;
					goto loop;
				}
				// else
				// check if its a global
				(c->stk+1)->i = get_variable(start, (YYCURSOR - start), &c->stk->i);
				if ((c->stk+1)->i!=0){
					INCREMENT_STACK
					goto loop;
				}
				// nothing
				printf("Cannot find identifier as a local/function/global name!!!");
				print_code(start, (YYCURSOR - start));
				printf("\n");
				break;
				case 1: // assignment
				c->word_flags=0;
				DECREMENT_STACK
				// will try to insert unique name, if fails will update value only
				save_variable(start, (YYCURSOR - start), c->stk->i);
				break;
				case 2: // get address
				c->word_flags=0;
				// check locals
				// TODO : STUB_FOR_LOCALS
				// else
				// check if its a function
				c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start));
				if (c->stk->s!=0){
					INCREMENT_STACK
					goto loop;
				}
				// else
				// check if its a global
				
				
				// nothing
				printf("Cannot find function name!!!");
				print_code(start, (YYCURSOR - start));
				fputc ('\n', stdout);
				break;
			}
		}
		goto loop;
	}

	//~ function_call_addr {
		//~ start++;
		//~ c->stk->s = (u8 *)get_function_addr(start, (YYCURSOR - start));
		//~ if (c->stk->s==0){
			//~ printf("Cannot find function name!!!");
			//~ print_code(start, (YYCURSOR - start));
			//~ fputc ('\n', stdout);
			//~ goto loop;
		//~ }
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	function_definition {
		save_function_addr(start, (YYCURSOR - start-1), YYCURSOR);
		YYCURSOR-=lex_if_else(&YYCURSOR, 2); // skip definition
		goto loop;
	}
	*/                               // end of re2c block
}  



