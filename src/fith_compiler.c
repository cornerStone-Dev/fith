/* fith compiler */
/* PUBLIC DOMAIN */


#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <termios.h>

#include "std_types.h"


#include "../sqlite3/sqlite3.h"
#include "../gen/sql3_macros.c"

#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK

#define IS_INT    0x10000000
#define IS_FLOAT  0x20000000
#define IS_STR    0x40000000
#define CTR_OFF   0x80000000
#define FLAG_MASK 0x70000000
typedef struct Xoken Token;

typedef union boken Data;

typedef union boken {
	u8 *   s;
	s64    i;
	f64    d;
	Data * v;
	s32    fd[2];
} Data;

typedef struct Xoken {
	union{
		u8 * s;
		s64  i;
		f64  d;
	};
	u32  l;
	u32  flags;
} Token;

typedef struct ScopeList{
	u8 * end;
	u8 * cursor_stack[16];
	u32  item_count_stack[16];
	u32  scopeIdx;
	u8   table[65536];
} ScopeList;


typedef struct stringLitList{
	u64   cursor;
	u64   bcur;
	u8 ** ar[512];
	u8 * buff[512];
} stringLitList;

typedef struct parser_s{
	ScopeList     *scopeList;
	ScopeList     *varList;
	Data *        stk;
	Data *        stk_start;
	Data *        stk_end;
	Data *        vars;
	stringLitList *strList;
	Data *        cstk;
	u8 **         words;
	u8 *          buff;
	u8 *          buff_start;
	u8 *          out;
	u8 *          yycur;
	u8 *          step_spot;
	struct        timespec time;
	u32           num_dots;
	u32           line_num;
	u32           func_start_line_num;
	u8            file_name_buff[512];
	u8            is_def;
	u8            is_fork_child;
	u8            is_fork_parent;
	u8            is_struct;
	u8            is_union;
	u8            is_fp;
	u8            is_step;
	u8            is_custom_type;
	u8            inside_function;
	u8            printed_error;
	u8            local_macro;
	u8            is_inline;
} ParserState;

static sqlite3 *fdb;

/* function prototypes */
//~ static s32
//~ add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l);
//~ static s32
//~ is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l);
//~ static u8 *
//~ indx_within(ScopeList * restrict str_l, u32  indx, u8 * restrict output);
//~ static void
//~ enter_scope(ScopeList * restrict scope_l);
static inline void
leave_scope(ScopeList * restrict scope_l);
//~ static u32
//~ varintGet(const u8 *s, u64 * pRes);
//~ static u32 
//~ varintWrite(u8 *z, u64 x);

//~ static u32
//~ base64encode(u8 * output, u32 input);
//~ static u32 
//~ safe_atol_64(const u8 ** strp);

static struct termios termios_orig;

void
raw_begin(void)
{
    tcgetattr(STDIN_FILENO, &termios_orig);
    struct termios raw;
    memcpy(&raw, &termios_orig, sizeof(raw));
    raw.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    raw.c_oflag &= ~OPOST;
    raw.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    raw.c_cflag &= ~(CSIZE|PARENB);
    raw.c_cflag |= CS8;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void
raw_end(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_orig);
}

static void
print_code(const u8 *str, u32 len)
{
	//printf("len: %d end\n", len);
	for(u32 x=0; x<len; x++){
		if(str[x]=='\000') {
			fputc ('\'', stdout);
			continue;
		}
		//~ if(str[x]==13) {
			//~ fputc ('[', stdout);
			//~ fputc ('\\', stdout);
			//~ fputc ('n', stdout);
			//~ fputc (']', stdout);
			//~ continue;
		//~ }
		//fputc ('[', stdout);
		fputc (str[x], stdout);
		//fputc (']', stdout);
	}
	//fputc ('\n', stdout);
}

// new strat: manage buffer only, then regen current line every time
static s32
fith_fgets(u8 *string, u32 limit, u8 *history)
{
	//u8 * histEnd;
	s32 curChar;
	u32 cur=0;
	u32 top=1;
	string[cur] = 3;
	while (1)
	{
		fputc ( '\r', stdout);
		printf("fith-> ");
		//~ for (u32 i=0; i<cur; i++)
		//~ {
			//~ fputc ( string[i], stdout);
		//~ }
		//printf("%s", string);
		print_code(string, cur);
		fputc ( 32, stdout);
		for (u32 i=0; i<top-cur; i++)
		{
			fputc ( '\010', stdout); // backspace
		}
		curChar = fgetc ( stdin );
		//printf("%d ", curChar);
		if (curChar==-1){ // EOF
			return 0;
		}
		if (curChar==3){ // cntrl + c
			raw_end();
			exit(1);
		}
		// right arrow 27 91 67
		// left arrow 27 91 68
		// del key 27 91 51 126
		if (curChar==27){ // up arrow 27 91 65 down arrow 27 91 66
			curChar = fgetc ( stdin );
			if (curChar==91){ // up arrow 27 91 65 down arrow 27 91 66
				curChar = fgetc ( stdin );
				if (curChar==51){ // up arrow 27 91 65 down arrow 27 91 66
					curChar = fgetc ( stdin );
					if (curChar==126){ // up arrow 27 91 65 down arrow 27 91 66
						// handle delete key 27 91 51 126
						if(cur==(top-1)){ // check for overrun
							continue;
						}
						top--;
						// shift buff backward
						for (u32 i=cur; i<top; i++)
						{
							string[i]=string[i+1];
						}
						continue;
					}
				} else if (curChar==65){
					// handle up arrow
					if (history==0){ // null pointer, no history
						continue;
					}
					if ((*history==3)){ // at top of history
						if (*(history-1)==3){ // bottom
							continue;
						} else {
							history-=2; // skip past null and newline
							// go back for previous newline
							while((*history!='\n')&&(*history!=3)){
								history--;
							}
							history++;
						}
					}
					fputc ( '\r', stdout);
					for (u32 i=0; i<cur+9; i++)
					{
						
						fputc ( 32, stdout);
					}
					cur=0;
					top=1;
					string[cur] = 0;
					while((*history!='\n')&&(*history!=3))
					{
						// shift buff forward
						for (u32 i=top; i>cur; i--)
						{
							string[i]=string[i-1];
						}
						string[cur] = *history; // insert char
						top++;
						cur++;
						history++;
					}
					history--;
					while((*history!='\n')&&(*history!=3)){
						history--;
					}
					// one below last start
					if ((*history==3)){
						history++;
					} else { //newline, search again
						history--;
						while((*history!='\n')&&(*history!=3)){
							history--;
						}
						history++;
					}
				} else if (curChar==66){
					// handle down
				} else if (curChar==67){
					// handle right arrow
					if(cur==(top-1)){ // check for overrun
						continue;
					}
					cur++;
				} else if (curChar==68){
					// handle left arrow
					if(cur==0){ // check for overrun
						continue;
					}
					cur--;
				} else { // skip anything else
					continue;
				}
				continue;
			}
		}
		if (curChar==13){ // carriage return character WHEN YOU PRESS ENTER
			fputc ( '\r', stdout);
			fputc ( '\n', stdout);
			// write into local buff
			if(top==limit){ // check for overrun
				printf("fith_fgets:Input greater than limit.\n");
				return 0;
			}
			string[top-1] = 10; // CHANGE TO A NEWLINE!!!!
			string[top] = 3;
			break;
		}
		if (curChar==127){ //
			if(cur==0){ // check for overrun
				continue;
			}
			cur--;
			top--;
			// shift buff backward
			for (u32 i=cur; i<top; i++)
			{
				string[i]=string[i+1];
			}
			continue;
		}
		// write into local buff
		if(top==limit){ // check for overrun
			printf("fith_fgets:Input greater than limit.\n");
			return 0;
		}
		// shift buff forward
		for (u32 i=top; i>cur; i--)
		{
			string[i]=string[i-1];
		}
		string[cur] = curChar; // insert char
		top++;
		cur++;
		// always null terminate
		//~ if(cur==limit){ // check for overrun
			//~ printf("fith_fgets:Input greater than limit.\n");
			//~ return 0;
		//~ }
		//~ string[cur] = 0;
		
		//~ //fputc ( '{', stdout);
		//printf("%d", curChar);//backspace=127, del=51 126
		//~ //fputc ( curChar, stdout); //fgetc ( stdin );
		//~ //fputc ( '}', stdout);
		//~ fputc ( '\r', stdout);
		//~ fputc ( '\n', stdout);
		//~ //fflush (stdout);
	}
	return top;
}

/* globals */

//~ static FILE * outputFile,* typeProtoFile, * typesFile,
	//~ * funcProtoFile, * globalsFile, * interfaceFile, * includesFile;
#define DECREMENT_STACK \
if (p_s->stk>p_s->stk_start) \
{ \
	p_s->stk--; \
} else \
{ printf("stack underflow!!!\n"); }

#define INCREMENT_STACK \
if (p_s->stk<p_s->stk_end) \
{ \
	p_s->stk++; \
} else \
{ printf("stack overflow!!!\n"); }

#include "fith_data.c"
#include "../tool_output/fith_lex.c"
//#include "../tool_output/fl_c_gram.c"
//~ #include "../tool_output/fcompile_gram.c"
//~ #include "../tool_output/ignore_gram.c"

//#define INTPUT_FILE "input/weeklystat.c"
#define DEFAULT_DIR     "c_src/"
#define OUTPUT_FILE     "source.c"
#define TYPE_PROTO      "type_proto.h"
#define TYPES           "types.h"
#define FUNC_PROTO      "func_proto.h"
#define FL_STD_FILE     "fl_std.h"
#define FL_GLOBALS_FILE "globals.h"
#define INTERFACE_FILE  "interface.h"
#define INCLUDES_FILE   "includes.h"

int main(int argc, char **argv)
{
	
	unsigned char * data=0, *output_string_base;
	//void *pEngine;     /* The LEMON-generated LALR(1) parser */
	//yyParser sEngine;  /* Space to hold the Lemon-generated Parser object */
	unsigned char output_string[65536] = {0};
	unsigned char stringBuffer[4096] = {0};
	unsigned char *strBuff;
	Data stack[384]={0};
	Data vars[512]={0};
	//unsigned char file_name_buff[512] = {0};
	unsigned char * output = output_string;
	u8 dirName[512];
	Data cstack[128]={0};
	u8 * words[128]={0};
	u8 * dirName_p;
	Token token = {0};
	ScopeList scopeList={0};
	ScopeList varList={0};
	stringLitList strList={0};
	int tmp_token;
	u32 x, inputLen,z;
	ParserState p_s = {0};
	FILE * pFile;
	size_t lSize;
	unsigned char * buffer;
	size_t result;
	DIR *d=0;
	struct dirent *dir;
	output_string[0]=3;
	output_string_base=&output_string[1];
	p_s.scopeList = &scopeList;
	p_s.varList = &varList;
	p_s.out = output;
	p_s.buff_start = output_string_base;
	p_s.buff = output_string_base;
	p_s.stk = stack;
	p_s.stk_start = stack;
	p_s.stk_end = &stack[374];
	p_s.vars = vars;
	p_s.cstk = cstack;
	p_s.words = words;
	p_s.strList = &strList;
	scopeList.cursor_stack[0]=scopeList.table;
	scopeList.end=&scopeList.table[65535];
	varList.cursor_stack[0]=varList.table;
	varList.end=&varList.table[65535];
	//strBuff = stpcpy((char *)stringBuffer, "fith-> ");
	strBuff = stringBuffer;
	
	sqlite3_initialize();
	sqlite3_open(":memory:", &fdb);
	
	fdb_SETUP();
	fdb_PREPARE();
	
	SQL3_SETUP(fdb, "PRAGMA journal_mode=OFF;");
	SQL3_SETUP(fdb, "CREATE TABLE fns(name TEXT PRIMARY KEY, addr INTEGER)WITHOUT ROWID;");
	SQL3_SETUP(fdb, "CREATE TABLE vars(name TEXT PRIMARY KEY, val INTEGER)WITHOUT ROWID;");
	SQL3_SETUP(fdb, "CREATE TABLE ptrs(addr INTEGER PRIMARY KEY);");
	
	
	// end output fl_std file
	//~ printf("argc: %d\n", argc);
	//~ for (s32 x=0; x<argc; x++){
		//~ printf("%d:%s\n", x, argv[x]);
	//~ }
	
	if ( (argc > 1)
	  && (argv[1][0]=='-')
	  && (argv[1][1]=='d') 
	  && (strlen(&argv[1][2])<498) ) 
	{
		dirName_p = (u8*)stpcpy((char *)dirName, &argv[1][2]);
		dirName_p = (u8*)stpcpy((char *)dirName_p, "/");
	} else {
		dirName_p = (u8*)stpcpy((char *)dirName, DEFAULT_DIR);
	}
	u32 i=1;
	if (argc > 1)
	{
		for (; i<argc; i++)
		{
			x = lex_options((u8 *)argv[i]);
			
			switch (x){
				case 0:
				return 0;
				break;
				case 1:
				
				while (1)
				{
					//printf("fith-> ");
					//~ printf("%d", fgetc(stdin));
					//~ //fputc ( curChar, stdout); //fgetc ( stdin );
					//~ //fputc ( '}', stdout);
					//~ fputc ( '\r', stdout);
					//~ fputc ( '\n', stdout);
					x++;
					raw_begin();
					inputLen = fith_fgets(strBuff, 4096, data);
					raw_end();
					//~ for(u32 y=0;strBuff[y];y++)
					//~ {
							//~ //fputc ( strBuff[y], stdout);
							//~ printf("%d ", strBuff[y]);
							
					//~ }
					//~ fputc ( '\n', stdout);
					//printf("strBuff: %s", strBuff);
					if (inputLen!= 0 )
					{
						if (!(strncmp((const char *)strBuff, ".exit", 5)))
						{
							return 0;
						}
						if (!(strncmp((const char *)strBuff, ".fp", 3)))
						{
							p_s.is_fp=1;
							continue;
						}
						if (!(strncmp((const char *)strBuff, ".int", 4)))
						{
							p_s.is_fp=0;
							continue;
						}
						//printf("strBuff: %s\n", strBuff);
						if (!(strncmp((const char *)strBuff, ".dump", 5)))
						{
							//*(p_s.buff_start) = '\000';
							//printf("xxx: %s end\n", output_string_base);
							//printf("%s",output_string_base);
							print_code(output_string_base, p_s.buff_start-output_string_base);
							continue;
						}
						if (!(strncmp((const char *)strBuff, ".save", 5)))
						{
							*(p_s.buff_start) = '\000';
							pFile = fopen ( "session.fith", "w" );
							if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
							fwrite (output_string_base,
								sizeof(char),
								p_s.buff_start-output_string_base,
								pFile);
							fflush (pFile);
							fclose (pFile);
							printf("session.fith saved\n");
							continue;
						}
						
						data = p_s.buff_start;
						z=0;
						if((*strBuff!='\n')&&(*strBuff!='.')){
							while(strBuff[z]!=3){
								*p_s.buff_start=strBuff[z];
								z++;
								p_s.buff_start++;
							}
							//p_s.buff_start = (u8*)stpcpy((char *)p_s.buff_start, (const char *)strBuff);
						}
						//printf("data: %s\n", strBuff);
						if(p_s.is_fp)
						{
							*(p_s.buff_start) = 'f';
							*(p_s.buff_start+1) = '.';
							*(p_s.buff_start+2) = '\003';
						} else {
							*(p_s.buff_start) = '.';
							*(p_s.buff_start+1) = '\003';
						}
						do {
							tmp_token = lex(data, &token, &p_s.line_num, &p_s);
						} while (tmp_token != 0);
						*(p_s.buff_start) = 3;
					}
				}
				break;
				case 2:
				printf("target file: %s\n",argv[i]);
				p_s.is_def=1;
				sprintf((char *)p_s.file_name_buff, "%s", argv[i]);
				goto one_file;
				one_file_return:
				break;
			}
			p_s.buff_start = output_string_base;
		}
	}
	
	if (p_s.is_def)
	{
		return 0;
	}
	
	/** Set up parser **/
	//pEngine = &sEngine;
	//ParseInit(pEngine, &p_s);
	
	/* open current directory */
	d = opendir("fith_src");
	if (d==0)
	{
		//printf("NO DIR!!!\n");
		return -1;
	}
	
	
	//printf("about to start\n");
	while ( ((dir = readdir(d)) != NULL) /*&& (strstr(dir->d_name, ".c")!=0)*/ )
	{
		//printf("Got in\n");
		if ( (strstr(dir->d_name, ".fith")!=0) ) {
		//printf("%s\n", dir->d_name);
		p_s.line_num=1;
		//~ output = (uint8_t *)stpcpy((char *)output, "/* src/");
		//~ output = (uint8_t *)stpcpy((char *)output, dir->d_name);
		//~ output = (uint8_t *)stpcpy((char *)output, " */\n");
		//~ fwrite (output_string,
				//~ sizeof(char),
				//~ output-output_string,
				//~ outputFile);
		//~ output = output_string;
		
		p_s.out = output;
		
		sprintf((char *)p_s.file_name_buff, "fith_src/%s", dir->d_name);

		one_file:
		
		pFile = fopen ( (char *)p_s.file_name_buff, "rb" );
		if (pFile==NULL) {fputs ("File error, cannot open source file\n",stderr); exit (1);}
		
		
		
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		// allocate memory to contain the whole file:
		buffer = (unsigned char*) malloc (sizeof(char)*lSize+1);
		if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}
		data = buffer;

		// copy the file into the buffer:
		result = fread (buffer,1,lSize,pFile);
		if (result != lSize) {fputs ("Reading error\n",stderr); exit (3);}
		
		/* 0x03 terminate buffer */
		buffer[lSize]=3;
		
		do {
			tmp_token = lex(data, &token, &p_s.line_num, &p_s);

			//Parse(pEngine, tmp_token, token);
			
		} while (tmp_token != 0);
		
		

		fclose (pFile);
		/* free memory that stored copy of file */
		free (buffer);
		}
		if (p_s.is_def)
		{
			goto one_file_return;
		}
	}
	
	/* close directory */
	closedir(d);
	
	/* flush file out of cache and close all files */
	//~ fflush (outputFile); 
	//~ fclose (outputFile);
	
	
	/*** De-comission parser * */
	//ParseFinalize(pEngine);
	s32 rcode;
	if(p_s.printed_error){
		rcode = 1;
	} else {
		rcode = 0;
	}
	return rcode;
}

//~ static s32
//~ add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l)
//~ {
	//~ u8 * cursor = str_l->cursor_stack[str_l->scopeIdx];
	//~ u8 * end =    str_l->end;
	
	//~ if ( (end - cursor) < l )
	//~ {
		//~ printf ("ScopeList: out of room\n");
		//~ return -1;
	//~ }
	
	//~ /* copy string to type_table */
	//~ for (s32 i=0;i<l;i++)
	//~ {
		//~ *cursor = *s;
		//~ cursor++;
		//~ s++;
	//~ }
	
	//~ /* store length of string */
	//~ *cursor = l;
	//~ cursor++;
	
	
	//~ //printf("*str_l->count =%d\n", str_l->item_count_stack[str_l->scopeIdx]);
	//~ str_l->cursor_stack[str_l->scopeIdx] = cursor;
	//~ str_l->item_count_stack[str_l->scopeIdx]+=1;
	
	//~ return str_l->item_count_stack[str_l->scopeIdx]-1;
//~ }

//~ /* search type table for IDENT */
//~ static s32
//~ is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l)
//~ {
	//~ u8 * cursor = str_l->cursor_stack[str_l->scopeIdx]-1;
	//~ u8 * start =  str_l->table;
	//~ u8 * string = s;
	//~ u8 * len_cursor;
	//~ s32  idx = str_l->item_count_stack[str_l->scopeIdx];
	//~ u8   len;
	
	
	//~ /* search backwards, view as a stack */
	//~ while ( cursor >= start )
	//~ {
		//~ len = (*cursor);
		//~ len_cursor = cursor;
		//~ idx--;
		//~ if ( l != len )
		//~ {
			//~ /* skip to next one */
			//~ cursor = len_cursor-len-1;
			//~ continue;
		//~ }
		//~ cursor-=len;
		//~ /* matching length */
		//~ for (s32 i=0;i<l;i++)
		//~ {
			//~ if ( (*cursor)!= (*s) )
			//~ {
				//~ /* skip to next string */
				//~ cursor = len_cursor-len-1;
				//~ /* reset string */
				//~ s = string;
				//~ break;
			//~ }
			//~ cursor++;
			//~ s++;
		//~ }
		//~ /* see if success */
		//~ if ( s != string )
		//~ {
			//~ //printf(" found something!!\n");
			//~ return idx;
		//~ }
	//~ }
	//~ /* we have found nothing */
	//~ //printf(" found nothing!!\n");
	//~ return -1;
//~ }

/* search type table at index */
//~ static u8 *
//~ indx_within(ScopeList * restrict str_l, u32 indx, u8 * restrict output)
//~ {
	//~ u8 * cursor = str_l->cursor_stack[str_l->scopeIdx]-1;
	//~ s32  idx = str_l->item_count_stack[str_l->scopeIdx];
	//~ u8   len=0;
	
	//~ if (indx > (idx-1) ) {
		//~ return output;
	//~ }
	
	//~ /* one comparision */
	//~ while ( idx > 0 )
	//~ {
		//~ len = (*cursor);
		//~ idx--;
		//~ if ( idx != indx )
		//~ {
			//~ /* skip to next one */
			//~ cursor = cursor-len-1;
			//~ continue;
		//~ }
		//~ break;
	//~ }
	//~ cursor-=len;
	//~ /* copy out string */
	//~ for (s32 i=0;i<len;i++)
	//~ {
		//~ *output = *cursor;
		//~ cursor++;
		//~ output++;
	//~ }
	
	//~ return output;
//~ }

//~ static void
//~ enter_scope(ScopeList * restrict scope_l)
//~ {
	//~ u32 idx;
	//~ scope_l->scopeIdx++;
	//~ idx = scope_l->scopeIdx;
	//~ /* copy end of previous scope to new scope */
	//~ scope_l->item_count_stack[idx] = scope_l->item_count_stack[idx-1];
	//~ scope_l->cursor_stack[idx] = scope_l->cursor_stack[idx-1];
	//~ //printf("entering scope!%d\n",scope_l->scopeIdx);
//~ }

static inline void
leave_scope(ScopeList * restrict scope_l)
{
	scope_l->scopeIdx--;
	//printf("leaving scope!%d\n",scope_l->scopeIdx);
}


//~ const u8 base64EncodeLookup[64]= 
	//~ { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	  //~ 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	  //~ 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	  //~ 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_' };
//~ static u32 //TODO reword this function. It works just not pretty
//~ base64encode(u8 * output, u32 input)
//~ {
	//~ u8 temp_output[16];
	//~ u8 * output_p = output;
	//~ s32 count = 1;
	//~ s32 tmp_value;
	
	//~ temp_output[count] = '\0';
	//~ do{
		//~ tmp_value = input%64;
		//~ input = input / 64;
		//~ count+=1;
		//~ temp_output[count] = (base64EncodeLookup[tmp_value]);
	//~ }while(input);
	
	//~ while (count){
		//~ *output_p = temp_output[count];
		//~ output_p+=1;
		//~ count-=1;
	//~ }
	//~ return (output_p - output-1);
//~ }

//~ static u32 
//~ safe_atol_64(const u8 ** strp) /* 18 decimal digits only */
//~ {
	//~ static const u8 base64intLookup[78]= 
	//~ {  62,   0,   0,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,
	    //~ 0,   0,   0,   0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11, 
	   //~ 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,
	    //~ 0,   0,  63,   0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,
	   //~ 38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51 };
	
	//~ const u8 * str = *strp;
	//~ u32 cnt=0;
	//~ u32 val=0;
	
	//~ while ( (*str >= '-') && (*str <= 'z') && (cnt<10) ) {
		//~ val = (val * 64) + (base64intLookup[(*str - '-')]);
		//~ ++str;
		//~ ++cnt;
	//~ }
	//~ *strp=str;
	//~ return val;
//~ }
