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
#include <math.h>
#include <termios.h>
#include <sys/random.h>

#include "std_types.h"
//#include "fith_ION.h"
//#include "fith_util.h"

#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK

typedef struct Xoken Token;

typedef union data_s Data;

typedef union data_s {
	u8     *s;
	s64    i;
	f64    d;
	Data   *v;
	s32    fd[2];
} Data;

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

typedef struct context1_s{
	Data *        stk;
	Data *        stk_start;
	Data *        cstk;
	u8 *          buff;
	u8 *          buff_start;
	u8 *          out;
	u8 *          yycur;
	u8 *          source_code;
	u8 *          stecpot;
	struct        timespec time;
	u32           line_num;
	u8            is_def;
	u8            is_customWord;
	u8            is_fp;
	u8            is_step;
	u8            printed_error;
	u8            word_flags;
	u8            in_case;
	u8            file_name_buff[512];
} Context1;

typedef struct context2_s{
	Data *        Stub;
} Context2;

typedef struct context3_s{
	Data *        Stub;
} Context3;

typedef struct{
	Data *sp;
	Data tos;
} Registers;

/* function prototypes */
static void
save_function_addr(u8 *start, u64 len, u8 *addr);
static void
save_variable(u8 *start, u64 len, s64 val, u64 flags);
static void
garbage_collect(u64 last_requested_size);
//~ static void lex_skipVal(const u8 **YYCURSORx);
//~ static u32 ION_getLen(const u8 *input);
static void *
heap_malloc(size_t bytes) __attribute__((malloc,alloc_size(1)));


//~ static s32
//~ add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l);
//~ static s32
//~ is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l);
//~ static u8 *
//~ indx_within(ScopeList * restrict str_l, u32  indx, u8 * restrict output);
//~ static void
//~ enter_scope(ScopeList * restrict scope_l);
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
    memmove(&raw, &termios_orig, sizeof(raw));
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
	for(u32 x=0; x<len; x++){
		if(str[x]=='\000') {
			fputc ('\'', stdout);
			continue;
		}
		if(str[x]==0x04) {
			fputc ('}', stdout);
			continue;
		}
		fputc (str[x], stdout);
	}
}

static u8 *
load_file(u8 *file_name, u8 as_function)
{
	FILE *pFile;
	u8 * buffer;
	size_t fileSize, result;
	
	pFile = fopen ( (char *)file_name, "rb" );
	if (pFile==NULL) {fputs ("File error, cannot open source file\n",stderr); exit (1);}
	// obtain file size:
	fseek(pFile , 0 , SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);
	// allocate memory to contain the whole file:
	buffer = malloc(fileSize+2);
	if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}
	// copy the file into the buffer:
	result = fread (buffer,1,fileSize,pFile);
	if (result != fileSize) {fputs ("Reading error\n",stderr); exit (3);}
	/* 0x03 terminate buffer, leave return in sub file */
	if (as_function){
		buffer[fileSize]=';';
		buffer[fileSize+1]=3;
	} else {
		buffer[fileSize]=3;
	}
	fclose (pFile);
	// TODO update to maybe not use garbage collector
	//save_variable(file_name, strlen((const char *)file_name), (s64)buffer);
	return (u8*)buffer;
}

// new strat: manage buffer only, then regen current line every time
static s64
fith_fgets(u8 *string, u32 limit, u8 *history)
{
	u8 * histEnd;
	s32 curChar;
	u32 cur=0;
	u32 top=1;
	string[cur] = 3;
	while (1)
	{
		fputc ( '\r', stdout);
		printf("fith-> ");
		print_code(string, (top-1));
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
					for (u32 i=0; i<top+9; i++)
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
						history++; // next history start
					}
				} else if (curChar==66){
					// handle down
					if (history==0){ // null pointer, no history
						continue;
					}
					if ((*history==3)){ // at top of history
						continue;
					}
					// we have valid history, but may be just last command
					histEnd = history;
					// find end
					while((*histEnd!='\n')&&(*histEnd!=3)){
						histEnd++;
					}
					if ((*histEnd==3)){ // at top of history
						continue;
					}
					histEnd++;
					history = histEnd;
					fputc ( '\r', stdout);
					for (u32 i=0; i<top+9; i++)
					{
						
						fputc ( 32, stdout);
					}
					cur=0;
					top=1;
					string[cur] = 0;
					while((*histEnd!='\n')&&(*histEnd!=3))
					{
						// shift buff forward
						for (u32 i=top; i>cur; i--)
						{
							string[i]=string[i-1];
						}
						string[cur] = *histEnd; // insert char
						top++;
						cur++;
						histEnd++;
					}
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

#define FITH_STACK_MAX 135

/* globals */
#define DECREMENT_STACK \
if (c->stk>c->stk_start) \
{ \
	c->stk--; \
} else \
{ printf("stack underflow!!!\n"); }

#define INCREMENT_STACK \
if (c->stk<c->stk_end) \
{ \
	c->stk++; \
} else \
{ printf("stack overflow!!!\n"); }

//~ static s8
//~ check_up(Data *stk, Data *start,s32 x)
//~ {
	//~ s8 tmp = ((((s64)(stk - start))+(x))>FITH_STACK_MAX);
	//~ if(tmp){printf("stack overflow!!!\n");}
	//~ return tmp;
//~ }

//~ static s8
//~ check_down(Data *stk, Data *start,s32 x)
//~ {
	//~ s8 tmp = ((((s64)(stk - start))+(x))<0);
	//~ if(tmp){printf("stack underflow!!!\n");}
	//~ return tmp;
//~ }


#define STACK_CHECK(x) \
if ( (((r.sp - c->stk_start)+(x))<0)){printf("stack underflow!!!\n"); goto loop;} \
else if ((((r.sp - c->stk_start)+(x))>374)){printf("stack overflow!!!\n"); goto loop;}

//~ #define STACK_CHECK_DOWN(x) 
//~ if ( check_down(r.sp,c->stk_start, (x) ) ){ goto loop;}

//~ #define STACK_CHECK_UP(x) 
//~ if ( check_up(r.sp,c->stk_start, (x) ) ){ goto loop;}

#define STACK_CHECK_DOWN(x) \
if ( __builtin_expect( ((((s64)(r.sp - c->stk_start))+(x))<0), 0) ){/*printf("stack underflow!!!\n");*/ goto stack_down_print;}

#define STACK_CHECK_UP(x) \
if ( __builtin_expect( ((((s64)(r.sp - c->stk_start))+(x))>FITH_STACK_MAX), 0) ){/*printf("stack overflow!!!\n");*/ goto stack_up_print;}

//~ #define STACK_CHECK_DOWN_R(x) 
//~ if ( check_down(r.sp,c->stk_start, (x) ) ){  return tos;}

//~ #define STACK_CHECK_UP_R(x) 
//~ if ( check_up(r.sp,c->stk_start, (x) ) ){  return tos;}

//~ #define STACK_CHECK_DOWN_R(x) 
//~ if ( __builtin_expect( ((((s64)(r.sp - c->stk_start))+(x))<0), 0) ){/*printf("stack underflow!!!\n");*/ goto stack_down_print;}

//~ #define STACK_CHECK_UP_R(x) 
//~ if ( __builtin_expect( ((((s64)(r.sp - c->stk_start))+(x))>FITH_STACK_MAX), 0) ){/*printf("stack overflow!!!\n");*/ goto stack_up_print;}

#define STACK_CHECK_DOWN_R(x) \
if ( __builtin_expect( ((((s64)(r.sp - c->stk_start))+(x))<0), 0) ){printf("stack underflow!!!\n"); return r;}

#define STACK_CHECK_UP_R(x) \
if ( __builtin_expect( ((((s64)(r.sp - c->stk_start))+(x))>FITH_STACK_MAX), 0) ){printf("stack overflow!!!\n"); return r;}


#include "fith_avl.c"
#include "fith_data.c"
#include "fith_words.c"
//#include "fith_ION.c"
//#include "../tool_output/fith_ION_lex.c"
#include "../tool_output/fith_lex.c"

int main(int argc, char **argv)
{
	
	unsigned char * data=0, *output_string_base;
	unsigned char output_string[65536] = {0};
	unsigned char stringBuffer[4096] = {0};
	unsigned char *strBuff;
	Data stack[FITH_STACK_MAX+1]={0};
	Data cstack[128]={0};
	unsigned char * output = output_string;
	//u8 dirName[512];
	
	u32 x, inputLen,z;
	Context1 c = {0};
	Context2 c2 = {0};
	Registers r = {0};
	FILE * pFile;
	DIR *d=0;
	struct dirent *dir;
	output_string[0]=3;
	output_string_base=&output_string[1];
	c.out = output;
	c.buff_start = output_string_base;
	c.buff = output_string_base;
	r.sp = stack;
	c.stk = stack;
	c.stk_start = stack;
	c.cstk = cstack;
	strBuff = stringBuffer;
	
	var_data.v = malloc(4096);
	var_data.hw=511;
	
	heap_data.c = &c;
	heap_data.h = malloc(128*1024*1);
	heap_data.t=(128*1024*1)-1;
	heap_data.cache=(u8*)5; // set to garbage
	
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

					raw_begin();
					inputLen = fith_fgets(strBuff, 4096, data);
					raw_end();

					if (inputLen!= 0 )
					{
						if (!(strncmp((const char *)strBuff, ".exit", 5)))
						{
							return 0;
						}
						if (!(strncmp((const char *)strBuff, ".fp", 3)))
						{
							c.is_fp=1;
							continue;
						}
						if (!(strncmp((const char *)strBuff, ".int", 4)))
						{
							c.is_fp=0;
							continue;
						}
						//printf("strBuff: %s\n", strBuff);
						if (!(strncmp((const char *)strBuff, ".dump", 5)))
						{
							//*(c.buff) = '\000';
							//printf("xxx: %s end\n", c.buff_start);
							//printf("%s",c.buff_start);
							print_code(c.buff_start, c.buff-c.buff_start);
							continue;
						}
						if (!(strncmp((const char *)strBuff, ".save", 5)))
						{
							*(c.buff) = '\000';
							pFile = fopen ( "session.fith", "w" );
							if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
							fwrite (c.buff_start,
								sizeof(char),
								c.buff-c.buff_start,
								pFile);
							fflush (pFile);
							fclose (pFile);
							printf("session.fith saved\n");
							continue;
						}
						
						data = c.buff;
						z=0;
						if((*strBuff!='\n')&&(*strBuff!='.')){
							while(strBuff[z]!=3){
								*c.buff=strBuff[z];
								z++;
								c.buff++;
							}
						}
						//printf("data: %s\n", strBuff);
						if(c.is_fp)
						{
							*(c.buff) = '.';
							*(c.buff+1) = 'f';
							*(c.buff+2) = '\003';
						} else {
							*(c.buff) = '.';
							*(c.buff+1) = '\003';
						}
						c.source_code = data;
						r = lex(&c, r, &c2);

						*(c.buff) = 3;
					}
				}
				break;
				case 2:
				printf("target file: %s\n",argv[i]);
				c.is_def=1;
				sprintf((char *)c.file_name_buff, "%s", argv[i]);
				goto one_file;
				one_file_return:
				break;
			}
			c.buff = c.buff_start;
		}
	}
	
	if (c.is_def)
	{
		return 0;
	}
	
	/** Set up parser **/
	//pEngine = &sEngine;
	//ParseInit(pEngine, &c);
	
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
		c.line_num=1;
		//~ output = (uint8_t *)stpcpy((char *)output, "/* src/");
		//~ output = (uint8_t *)stpcpy((char *)output, dir->d_name);
		//~ output = (uint8_t *)stpcpy((char *)output, " */\n");
		//~ fwrite (output_string,
				//~ sizeof(char),
				//~ output-output_string,
				//~ outputFile);
		//~ output = output_string;
		
		c.out = output;
		
		sprintf((char *)c.file_name_buff, "fith_src/%s", dir->d_name);

		one_file:
		
		data = load_file(c.file_name_buff, 0);
		
		c.source_code = data;
		
		r = lex(&c, r, &c2);

		}
		if (c.is_def)
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
	if(c.printed_error){
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

//~ static inline void
//~ leave_scope(ScopeList * restrict scope_l)
//~ {
	//~ scope_l->scopeIdx--;
	//~ //printf("leaving scope!%d\n",scope_l->scopeIdx);
//~ }


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
