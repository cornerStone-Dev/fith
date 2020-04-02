/* florida_c compiler */
/* PUBLIC DOMAIN */

/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
	
	mcm = "(" [^)]* ")"; // WILL NOT WORK ON "**/" ending!!!!
	scm = "\\" [^\n]* "\n";
	wsp = ([ \n\t\v\r] | scm | mcm)+;
	//macro = "#" ([^\n] | "\\\n")* "\n";
	//local_macro = "#@" ([^\n] | "\\\n")* "\n";
	// integer literals
	oct = "0" [0-7]*;
	dec = [1-9][0-9]*;
	hex = '0x' [0-9a-fA-F]+;
	// floating literals
	frc = [0-9]* "." [0-9]+ | [0-9]+ ".";
	exp = 'e' [+-]? [0-9]+;
	flt = (frc exp? | [0-9]+ exp) [fFlL]?;
	string_lit = ["] ([^"] | ([\\] ["]))* ["];
	mangled_string_lit = "`" [A-Za-z0-9]* " ";
	char_lit = ['] ([^'] | ([\\] [']))* ['];
	integer = oct | dec | hex;
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
	function_call = [a-zA-Z_][a-zA-Z_0-9?]*;
	function_call_addr = [a-zA-Z_][a-zA-Z_0-9?]*"@";
	function_definition = [a-zA-Z_][a-zA-Z_0-9?]* ":";
	//query =  "SQL3_QUERY_" [a-zA-Z_0-9]*;
	var = "$" function_call; // push value on stack, if exists
	var_assign = "=$" function_call; // pop top of stack and assign to value, create variable 
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
	* { printf("Invalid Option: %s\n",start); return 0; } //   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal
	
	"-repl" {
		return 1;
	}
	
	[a-zA-Z_/0-9-]+ ".fith" {
		return 2;
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
	
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer
	//start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { /*start =YYCURSOR;*/ goto loop; }//   default rule with its semantic action
	[\x00] { return 1; }             // EOF rule with null sentinal
	
	
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

	*/                               // end of re2c block
}

static int lex(/*const*/ u8 * YYCURSOR, Token * t, u32 * line_num, ParserState * p_s) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ //u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	/*const*/ u8 * start;
	
	//YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { start =YYCURSOR; goto loop; }//   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal
	
	wsp {
		while (start!=YYCURSOR){
			if(*start=='\n'){
				*line_num+=1;
				//printf("wsp, %d\n", *line_num);
			}
			start++;
		}
		goto loop;
	}
 
	integer {
		p_s->stk->i = atol( (const char *)start );
		INCREMENT_STACK
		//t->flags|=IS_INT;
		goto loop;
	}
	
	flt {
		p_s->stk->d = atof( (const char *)start );
		INCREMENT_STACK
		//t->flags|=IS_FLOAT;
		goto loop;
	}
	
	string_lit { 
		// rewrite _"ccc"_ ->
		//         `XXccc" where XX is a var int
		//         `Xccc" where XX is a var int
		// get length of string
		//u8 * tmp = start;
		//printf("PREmangled_string_lit:%s\n", start);
		(p_s->stk+1)->i=((YYCURSOR - start)-2);
		// write in accent
		*start='`';
		start++;
		// get index for string literal after saving it off
		(p_s->stk+2)->i= enter_string_literal((p_s->stk+1)->i, start, p_s->strList, &p_s->stk->s);
		//printf("length %ld\n",(p_s->stk+1)->i);
		//printf("index %ld\n",(p_s->stk+2)->i);
		
		// move cursor forward when writting index
		start+=base64encode(start, (p_s->stk+2)->i);
		
		// clean up rest of string with whitespace
		while(start<YYCURSOR)
		{
			*start=' ';
			start++;
		}
		//printf("mangled_string_lit:%s\n", tmp);
		INCREMENT_STACK
		goto loop;
	}
	
	mangled_string_lit { // 64 bit pointers, special STRING CLASS POINTERS
		//*(YYCURSOR-1) = 0; // null terminate
		start++;
		(p_s->stk+1)->i=safe_atol_64((const u8 **)&start);
		
		p_s->stk->s =p_s->strList->ar[(p_s->stk+1)->i/512][(p_s->stk+1)->i%512];
		INCREMENT_STACK
		//t->l = (YYCURSOR - start)-1;//maybe wrong
		//t->flags|=IS_STR;
		goto loop;
	}

	char_lit {
		goto loop;
	}

	semi {
		// pop command stack
		p_s->cstk--;
		// goto to return address
		YYCURSOR = *p_s->cstk;
		goto loop;
	}

	//~ ":" {
		//~ p_s->is_def=1;
		//~ goto loop;
	//~ }
	
	"p" {
		DECREMENT_STACK
		p_s->stk->s+=varintGet(p_s->stk->s, (u64*)&(p_s->stk+1)->i);
		for(u32 x=0; x<(p_s->stk+1)->i; x++){
			if(p_s->stk->s[x]=='\\') {
				x++;
				switch(p_s->stk->s[x]){
					case '\\':
					break;
					case 'n':
					p_s->stk->s[x]='\n';
					break;
					case 't':
					p_s->stk->s[x]='\t';
					break;
					case '\"':
					p_s->stk->s[x]='\n';
					break;
					case '\'':
					p_s->stk->s[x]='\'';
					break;
					case 'r':
					p_s->stk->s[x]='\r';
					break;
				}
			}
			fputc ( p_s->stk->s[x] , stdout );
		}
		//printf("%s",p_s->stk->s);
		fputc ( '\n' , stdout );
		goto loop;
	}

	"||" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i||p_s->stk->i;
		goto loop;
	}

	"&&" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i&&p_s->stk->i;
		goto loop;
	}

	"&" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i&p_s->stk->i;
		goto loop;
	}

	"^" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i^p_s->stk->i;
		goto loop;
	}

	"|" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i|p_s->stk->i;
		goto loop;
	}

	"==" {
		DECREMENT_STACK
		(p_s->stk-1)->i = ((p_s->stk-1)->i)==(p_s->stk->i);
		goto loop;
	}

	"!=" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i!=p_s->stk->i;
		goto loop;
	}

	"<" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i<p_s->stk->i;
		goto loop;
	}

	">" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i>p_s->stk->i;
		goto loop;
	}

	"<=" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i<=p_s->stk->i;
		goto loop;
	}

	">=" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i>=p_s->stk->i;
		goto loop;
	}

	"<<" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i<<p_s->stk->i;
		goto loop;
	}

	">>" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i>>p_s->stk->i;
		goto loop;
	}

	"+" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i+p_s->stk->i;
		goto loop;
	}

	"-" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i-p_s->stk->i;
		goto loop;
	}

	"/" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i/p_s->stk->i;
		goto loop;
	}
	
	"*" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i*p_s->stk->i;
		goto loop;
	}

	"f+" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d+p_s->stk->d;
		goto loop;
	}

	"f-" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d-p_s->stk->d;
		goto loop;
	}

	"f/" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d/p_s->stk->d;
		goto loop;
	}
	
	"f*" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d*p_s->stk->d;
		goto loop;
	}
	
	"f==" {
		DECREMENT_STACK
		(p_s->stk-1)->d = ((p_s->stk-1)->d)==(p_s->stk->d);
		goto loop;
	}

	"f!=" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d!=p_s->stk->d;
		goto loop;
	}

	"f<" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d<p_s->stk->d;
		goto loop;
	}

	"f>" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d>p_s->stk->d;
		goto loop;
	}

	"f<=" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d<=p_s->stk->d;
		goto loop;
	}

	"f>=" {
		DECREMENT_STACK
		(p_s->stk-1)->d = (p_s->stk-1)->d>=p_s->stk->d;
		goto loop;
	}

	"%" {
		DECREMENT_STACK
		(p_s->stk-1)->i = (p_s->stk-1)->i%p_s->stk->i;
		goto loop;
	}

	"." {
		for(int x =0 ; &p_s->stk_start[x]!=p_s->stk;x++){
			printf("[%ld] ",p_s->stk_start[x].i);
		}
		printf("\n");
		goto loop;
	}
	
	"f." {
		for(int x =0 ; &p_s->stk_start[x]!=p_s->stk;x++){
			printf("[%f] ",p_s->stk_start[x].d);
		}
		printf("\n");
		goto loop;
	}
	
	"~" {
		(p_s->stk-1)->i = ~((p_s->stk-1)->i);
		goto loop;
	}
	
	"!" {
		(p_s->stk-1)->i = !((p_s->stk-1)->i);
		goto loop;
	}
	
	"dup" {
		p_s->stk->i = (p_s->stk-1)->i;
		INCREMENT_STACK
		goto loop;
	}
	
	"over" {
		p_s->stk->i = (p_s->stk-2)->i;
		INCREMENT_STACK
		goto loop;
	}
	
	"drop" {
		DECREMENT_STACK
		goto loop;
	}
	
	"swap" {
		p_s->stk->i = (p_s->stk-2)->i;
		(p_s->stk-2)->i = (p_s->stk-1)->i;
		(p_s->stk-1)->i = p_s->stk->i;
		goto loop;
	}
	
	"rot" {
		p_s->stk->i = (p_s->stk-3)->i;
		(p_s->stk-3)->i = (p_s->stk-2)->i;
		(p_s->stk-2)->i = (p_s->stk-1)->i;
		(p_s->stk-1)->i = p_s->stk->i;
		goto loop;
	}

	"if" {
		DECREMENT_STACK
		if(p_s->stk->i==0){
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
		*p_s->cstk = YYCURSOR;
		p_s->cstk++;
		goto loop;
	}
	
	"until" {
		DECREMENT_STACK
		if(p_s->stk->i==0){
			YYCURSOR = *(p_s->cstk-1);
		} else {
			p_s->cstk--;
		}
		goto loop;
	}
	
	"sleep" {
		p_s->time.tv_sec = (p_s->stk-2)->i;
		p_s->time.tv_nsec = (p_s->stk-1)->i;
		p_s->stk-=2;
		nanosleep(&p_s->time, NULL);
		goto loop;
	}
	
	"fork" {
		// fork executes a function in another process. The parent process will
		// skip the next function call. The child process need to have the
		// return stack modified to exit, returns pid
		p_s->stk->i = fork();
		if (p_s->stk->i < 0){
			printf("Fork error!!!");
		} else if (p_s->stk->i == 0) {
			*p_s->cstk = (u8*)"exit";
			p_s->cstk++;
			// jump to function
			YYCURSOR = (p_s->stk-1)->s;
			p_s->stk-=1;
		} else {
			(p_s->stk-1)->i = p_s->stk->i;
		}
		goto loop;
	}
	
	"exit" {
		_Exit((p_s->stk-1)->i);
	}
	
	var {
		start+=1;
		p_s->stk->i = is_within(p_s->varList, start, (YYCURSOR - start));
		if (p_s->stk->i==-1){ 
			printf("Cannot find variable name!!!");
			return -1;
		}
		p_s->stk->i = p_s->vars[p_s->stk->i].i;
		INCREMENT_STACK

		goto loop;
	}
	
	var_addr {
		start+=1;
		p_s->stk->i = is_within(p_s->varList, start, (YYCURSOR - start));
		if (p_s->stk->i==-1){ 
			printf("Cannot find variable name!!!");
			return -1;
		}
		p_s->stk->v = &p_s->vars[p_s->stk->i];
		INCREMENT_STACK
		goto loop;
	}
	
	var_assign {
		start+=2;
		p_s->stk->i = is_within(p_s->varList, start, (YYCURSOR - start));
		if (p_s->stk->i!=-1){ // var is found
			// save value into variable
			p_s->vars[p_s->stk->i].i = (p_s->stk-1)->i;
			// pop stack
			DECREMENT_STACK
		} else { // var does not exist
			// create variable
			p_s->stk->i = add_to(p_s->varList, start, (YYCURSOR - start));
			if (p_s->stk->i==-1){ 
				printf("error out of room for function name!!!");
				return -1;
			}
			// save value into variable
			p_s->vars[p_s->stk->i].i = (p_s->stk-1)->i;
			// pop stack
			DECREMENT_STACK
		}
		goto loop;
	}
	
	function_call { //TODO make correct data structure and overwrite previous entry if exists
		p_s->stk->i = is_within(p_s->scopeList, start, (YYCURSOR - start));
		if (p_s->stk->i==-1){
			printf("Cannot find function name!!!:%s , len:%ld\n", start, (YYCURSOR - start));
			//return -1;
			goto loop;
		}
		// save off return
		*p_s->cstk = YYCURSOR;
		p_s->cstk++;
		// jump to function
		YYCURSOR = p_s->words[p_s->stk->i]; 
		goto loop;
	}
	
	function_call_addr { // push function address on stack
		(p_s->stk+1)->i = is_within(p_s->scopeList, start, (YYCURSOR - start-1));
		if ((p_s->stk+1)->i==-1){ 
			printf("Cannot find function name!!!:%s , len:%ld\n", start, (YYCURSOR - start));
			//return -1;
			goto loop;
		}
		// save off return
		p_s->stk->s = p_s->words[(p_s->stk+1)->i];
		INCREMENT_STACK
		goto loop;
	}
	
	function_definition { //TODO make correct data structure and overwrite previous entry if exists
		p_s->stk->i = add_to(p_s->scopeList, start, (YYCURSOR - start-1));
		if (p_s->stk->i==-1){ 
			printf("error out of room for function name!!!");
			return -1;
		}
		// save off function
		p_s->words[p_s->stk->i] = YYCURSOR;
		YYCURSOR-=lex_if_else(&YYCURSOR, 2); // skip definition
		goto loop;
	}
	*/                               // end of re2c block
}  



