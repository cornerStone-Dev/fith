/* fith compiler */
/* PUBLIC DOMAIN */

/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
	
	mcm = "(" [^)\x03]* ")"; // WILL NOT WORK ON "**/" ending!!!!
	scm = "\\" [^\n\x03]* "\n";
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
	string_lit = ["] ([^"\x00] | ([\\] ["]))* ["];
	string_lit_chain = string_lit (wsp string_lit)+;
	mangled_string_lit = ["] ([^"\x00] | ([\\] ["]))* "\x00";
	char_lit = ['] ([^'\x03] | ([\\] [']))* ['];
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
	* { printf("Invalid Option: %s, Operation aborted\n",start); return 0; } //   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal
	
	"--repl" {
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
	if (p_s->is_step)
	{
		if(p_s->is_step==2)
		{
			p_s->is_step = 3;
			// copy source off
			strncpy((char *)p_s->file_name_buff, (const char *)YYCURSOR, 11);
			// overwrite source with loop back to debug
			//stpcpy((char *)p_s->step_spot, " debug");
			p_s->step_spot=YYCURSOR;
			stpcpy((char *)YYCURSOR, "debug ");
			//printf("YYCURSOR::%s\n",YYCURSOR);
		} else {
			p_s->is_step = 2;
		}
	}

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { goto loop; } //   default rule with its semantic action start =YYCURSOR;
	[\x03] { return 0; }             // EOF rule with 0x03 sentinal
	
	wsp {
		while (start!=YYCURSOR){
			if(*start=='\n'){
				*line_num+=1;
				//printf("wsp, %d\n", *line_num);
			}
			start++;
		}
		if(p_s->is_step==2)
		{
			p_s->is_step = 1;
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
		p_s->stk->s = start+1;
		*(YYCURSOR-1) = 0;
		INCREMENT_STACK
		goto loop;
	}
	
	//~ string_lit_chain { 
		//~ p_s->stk->s = start+1;
		//~ *(YYCURSOR-1) = 0;
		//~ INCREMENT_STACK
		//~ goto loop;
	//~ }
	
	mangled_string_lit {
		p_s->stk->s = start+1;
		INCREMENT_STACK
		goto loop;
	}

	char_lit {
		goto loop;
	}

	";" {
		// pop command stack
		p_s->cstk--;
		// goto to return address
		YYCURSOR = *p_s->cstk;
		goto loop;
	}
	
	"p" {
		DECREMENT_STACK
		printf("%s\n",p_s->stk->s);
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
	
	"clear" {
		p_s->stk=p_s->stk_start;
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
	
	"malloc" {
		(p_s->stk-1)->s= malloc((p_s->stk-1)->i);
		if ((p_s->stk-1)->s == 0 )
		{
			printf("malloc error!!!\n");
		}
		goto loop;
	}
	
	"free" {
		DECREMENT_STACK
		free(p_s->stk->s);
		goto loop;
	}
	
	"random" {
		sqlite3_randomness(8, &p_s->stk->i);
		INCREMENT_STACK
		goto loop;
	}
	
	"abs" {
		(p_s->stk-1)->i=labs((p_s->stk-1)->i);
		goto loop;
	}
	
	"chan" {
		p_s->stk->i = socketpair(AF_UNIX, SOCK_STREAM, 0, (p_s->stk+2)->fd);
		if (p_s->stk->i < 0)
		{
			printf("socketpair error!!!\n");
		} else {
			p_s->stk->i = (p_s->stk+2)->fd[0];
			(p_s->stk+1)->i = (p_s->stk+2)->fd[1];
			INCREMENT_STACK
			INCREMENT_STACK
		}
		goto loop;
	}
	
	"reads" { // (0fd, 1pBuf, 2sizeof(pBuf))
		DECREMENT_STACK
		DECREMENT_STACK
		(p_s->stk+2)->i = read((p_s->stk-1)->i, p_s->stk->s, (p_s->stk+1)->i);
		if ((p_s->stk+2)->i < 0)
		{
			printf("reads error!!!\n");
		}
		(p_s->stk-1)->s = p_s->stk->s;
		goto loop;
	}
	
	"writes" { // (0fd, 1pBuf)
		DECREMENT_STACK
		DECREMENT_STACK
		
		(p_s->stk+3)->i = write(p_s->stk->i,
								(p_s->stk+1)->s,
								strlen((const char *)(p_s->stk+1)->s)+1);
		
		if ((p_s->stk+3)->i < 0)
		{
			printf("writes error!!!\n");
		}
		goto loop;
	}
	
	"debug" {
		u8 * start_of_line;
		u8 * end_of_line;
		u8 tmp;
		
		if (p_s->yycur==0)
		{
			p_s->yycur=YYCURSOR;
		}
		
		if (p_s->is_step==3)
		{
			// copy back source
			strncpy((char *)p_s->step_spot, (const char *)p_s->file_name_buff, 11);
			p_s->yycur=p_s->step_spot;
			p_s->is_step=0;
		}
		
		start_of_line=p_s->yycur;
		end_of_line=p_s->yycur;
		tmp = *p_s->yycur;
		*p_s->yycur=')';
		
		if (*line_num>1) {
			while (*start_of_line!='\n'){
				start_of_line--;
			}
			start_of_line++;
		}
		while (*end_of_line!='\n'){
				end_of_line++;
		}
		end_of_line++;
		//tmp = *end_of_line;
		//*end_of_line=0;
		
		//printf("%s", start_of_line);
		print_code(start_of_line, end_of_line-start_of_line);
		//*end_of_line=tmp;
		*p_s->yycur=tmp;
		
		if(p_s->is_fp)
		{
			for(int x =0 ; &p_s->stk_start[x]!=p_s->stk;x++){
				printf("[%f] ",p_s->stk_start[x].d);
			}
			printf("\n");
		} else {
			for(int x =0 ; &p_s->stk_start[x]!=p_s->stk;x++){
				printf("[%ld] ",p_s->stk_start[x].i);
			}
			printf("\n");
		}
		
		while (1)
		{
			printf("debug:: ");
			if (fgets ((char *)p_s->file_name_buff, 512, stdin) != NULL )
			{
				if (!(strncmp((const char *)p_s->file_name_buff, ".dump", 5)))
				{
					//*(p_s->buff) = '\000';
					//printf("%s",p_s->buff_start);
					print_code(p_s->buff_start, p_s->buff-p_s->buff_start);
					continue;
				}
				if (!(strncmp((const char *)p_s->file_name_buff, ".exit", 5)))
				{
					return 0;
				}
				if (!(strncmp((const char *)p_s->file_name_buff, ".fp", 3)))
				{
					p_s->is_fp=1;
					continue;
				}
				if (!(strncmp((const char *)p_s->file_name_buff, ".int", 4)))
				{
					p_s->is_fp=0;
					continue;
				}
				if (!(strncmp((const char *)p_s->file_name_buff, ".resume", 7)))
				{
					YYCURSOR=p_s->yycur;
					p_s->yycur=0;
					goto loop;
				}
				if (!(strncmp((const char *)p_s->file_name_buff, ".step", 5)))
				{
					YYCURSOR = p_s->yycur;
					p_s->is_step=1;
					goto loop;
				}
				p_s->out = p_s->buff;
				p_s->buff = (u8*)stpcpy((char *)p_s->buff, (const char *)p_s->file_name_buff);
				stpcpy((char *)p_s->buff, " debug");
				YYCURSOR = p_s->out;
				goto loop;
			}
		}
	}
	
	var {
		start+=1;
		p_s->stk->i = is_within(p_s->varList, start, (YYCURSOR - start));
		if (p_s->stk->i==-1){ 
			printf("Cannot find variable name!!!\n");
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
			printf("Cannot find variable name!!!\n");
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
				printf("error out of room for function name!!!\n");
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
			printf("error out of room for function name!!!\n");
			return -1;
		}
		// save off function
		p_s->words[p_s->stk->i] = YYCURSOR;
		YYCURSOR-=lex_if_else(&YYCURSOR, 2); // skip definition
		goto loop;
	}
	*/                               // end of re2c block
}  



