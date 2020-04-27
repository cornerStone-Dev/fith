/* fith compiler */
/* PUBLIC DOMAIN */


// first thought strategy is to search for end of object
// however this binary pattern can show up in integers and doubles
// therefore we must lex past them if they come up
// also must count objects to avoid nesting trap
// we dont need to deal with strings because they are utf-8 WRONG
// utf-8 has these tokens in them, crap we do have to deal with strings
static void lex_skipObj(const u8 **YYCURSORx)
{
	const u8 *YYCURSOR = *YYCURSORx;
	u32 obj_count=0;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		goto loop;
	}//   default rule with its semantic action skip garbage

	[\x86] { // ION Object
		obj_count++;
		goto loop;
	}

	[\x87] { // ION Object End
		if(obj_count==0){
			*YYCURSORx = YYCURSOR;
			return;
		}
		obj_count--;
		goto loop;
	}

	[\x8a] { // ION float
		YYCURSOR+=8;
		goto loop;
	}

	[\x8b-\x92] { // ION Ints
		YYCURSOR+=((*(YYCURSOR-1))-0x8A);
		goto loop;
	}

	[\x93-\xbe] { // ION Strings
		YYCURSOR+=((*(YYCURSOR-1))-0x92);
		goto loop;
	}

	[\xbf] { // ION unknown string length
		YYCURSOR+=strlen((const char *)YYCURSOR)+1;
		goto loop;
	}

	*/                               // end of re2c block
}

// same strat as objects explained above
static void lex_skipArray(const u8 **YYCURSORx)
{
	const u8 *YYCURSOR = *YYCURSORx;
	u32 array_count=0;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		goto loop;
	}//   default rule with its semantic action skip garbage

	[\x88] { // ION Array
		array_count++;
		goto loop;
	}

	[\x89] { // ION Array End
		if(array_count==0){
			*YYCURSORx = YYCURSOR;
			return;
		}
		array_count--;
		goto loop;
	}

	[\x8a] { // ION float
		YYCURSOR+=8;
		goto loop;
	}

	[\x8b-\x92] { // ION Ints
		YYCURSOR+=((*(YYCURSOR-1))-0x8A);
		goto loop;
	}
	
	[\x93-\xbe] { // ION Strings
		YYCURSOR+=((*(YYCURSOR-1))-0x92);
		goto loop;
	}

	[\xbf] { // ION unknown string length
		YYCURSOR+=strlen((const char *)YYCURSOR)+1;
		goto loop;
	}

	*/                               // end of re2c block
}

static void lex_skipVal(const u8 **YYCURSORx)
{
	const u8 *YYCURSOR = *YYCURSORx;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		*YYCURSORx=0;
		printf("invalid ION inside lex_skipVal\n");
		return;
	}//   default rule with its semantic action skip garbage

	[\x80] { // ION 0
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x81] { // ION 1
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x82] { // ION negative sign
		goto loop;
	}

	[\x83] { // ION true
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x84] { // ION false
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x85] { // ION null
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x86] { // ION Object
		lex_skipObj(&YYCURSOR);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x88] { // ION Array
		lex_skipArray(&YYCURSOR);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x8a] { // ION float
		YYCURSOR+=8;
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x8b-\x92] { // ION Ints
		YYCURSOR+=((*(YYCURSOR-1))-0x8A);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x93-\xbe] { // ION Strings
		YYCURSOR+=((*(YYCURSOR-1))-0x92);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\xbf] { // ION unknown string length
		YYCURSOR+=strlen((const char *)YYCURSOR)+1;
		*YYCURSORx = YYCURSOR;
		return;
	}

	*/                               // end of re2c block
}

// given an ION object, search for given path, ending with '.' '[' or '\0'
static const u8 * lex_searchObj(const u8 *inputx, u8 *pathx)
{                                    //
	const u8 *YYCURSOR = inputx;
	u8 *path = pathx;
	u32 key_len;
	u32 path_len=0, i=0, isNullTerminated=0;
	
	// find length of this part of the path
	while ( (path[path_len]!=0) && (path[path_len]!='[') && (path[path_len]!='.') )
	{
		path_len++;
	}
	
	while(1){
		key_len = (*YYCURSOR) - ION_STRING01+1;
		if (key_len>45)
		{
			printf("Error: lex_searchObj Keys must be strings\n");
			return 0;
		}
		YYCURSOR++; // point at key string
		if ((path_len>44)&&(key_len==ION_STRING_N))
		{
			key_len = strlen((const char *)YYCURSOR);
			isNullTerminated=1;
		}
		
		if(key_len != path_len){
			goto next;
		}
		for (; i<key_len; i++)
		{
			if (YYCURSOR[i] != path[i])
			{
				break;
			}
		}
		if(i<key_len)
		{
			return &YYCURSOR[i]; // success
		}
		i=0;
		
		next:
		YYCURSOR+= key_len+isNullTerminated; // skip past key
		isNullTerminated=0;
		lex_skipVal(&YYCURSOR); // skip value
		if(*YYCURSOR==ION_OBJ_END) // end of object found
		{
			return 0; // TODO MAKE return NULL
		}
	}

}

// given an ION element, search for given path
static int lex_findPath(const u8 *inputx, u8 *pathx)
{                                    //
	const u8 *YYCURSOR = inputx;
	u8 *path = pathx;
	
	if(*path == '.') // we are looking for a key
	{
		if(*YYCURSOR != ION_OBJ_START)
		{
			return -1;
		}
		path++;
		YYCURSOR++;
		return lex_searchObj(YYCURSOR, path);
	} else if (*pathx == '[') {
		
	} else {
		return -1;
	}
}

static int lex_stringToFSON(const u8 *YYCURSORx, u8 *outx)
{                                    //
	const u8 *YYCURSOR = YYCURSORx;
	u8 *out = outx;
	const u8 *YYMARKER;
	const u8 *start;
	//u8 *startMangledString;
	//YYCURSOR = *YYCURSOR_p;
	//startMangledString = YYCURSOR;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { goto loop; }//   default rule with its semantic action skip garbage
	[\x00] { 
		// write in metadata in begining
		return 1; 
	}             // EOF rule with null sentinal
	
	[ \n\t\r] { // JSON Whitespace
		goto loop;
	}

	"{" { // JSON Object start
		*out = ION_OBJ_START;
		out++;
		goto loop;
	}

	"}" { //  JSON Object end
		*out = ION_OBJ_END;
		out++;
		goto loop;
	}

	"[" { // JSON Whitespace
		*out = ION_ARR_START;
		out++;
		goto loop;
	}

	"]" { // JSON Whitespace
		*out = ION_ARR_END;
		out++;
		goto loop;
	}
	
	"," { // JSON Whitespace
		goto loop;
	}
	
	":" { // JSON Whitespace
		goto loop;
	}

	"true" { // JSON Whitespace
		*out = ION_TRUE;
		out++;
		goto loop;
	}

	"false" { // JSON Whitespace
		*out = ION_FALSE;
		out++;
		goto loop;
	}

	"null" { // JSON Whitespace
		*out = ION_NULL;
		out++;
		goto loop;
	}

	"-"?( "0"|([1-9][0-9]*) )  { // JSON integer
		u64 ionInt;
		s32 lz;
		ionInt = (u64)atol( (const char *)start );
		lz = __builtin_clzl(ionInt);
		if (lz==0)
		{
			// negative number
			*out = ION_NEG;
			out++;
			// get absolute value
			ionInt = (u64)labs((s64)ionInt);
			// retake leading zeros
			lz = __builtin_clzl(ionInt);
		}
		lz/=8;
		switch (lz){
			case 0:
			// 8 bytes of int
			*out = ION_INT8;
			out++;
			*out = ionInt&0xFF00000000000000;
			out++;
			*out = ionInt&0x00FF000000000000;
			out++;
			*out = ionInt&0x0000FF0000000000;
			out++;
			*out = ionInt&0x000000FF00000000;
			out++;
			*out = ionInt&0x00000000FF000000;
			out++;
			*out = ionInt&0x0000000000FF0000;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 1:
			// 7 bytes of int
			*out = ION_INT7;
			out++;
			*out = ionInt&0x00FF000000000000;
			out++;
			*out = ionInt&0x0000FF0000000000;
			out++;
			*out = ionInt&0x000000FF00000000;
			out++;
			*out = ionInt&0x00000000FF000000;
			out++;
			*out = ionInt&0x0000000000FF0000;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 2:
			// 6 bytes of int
			*out = ION_INT6;
			out++;
			*out = ionInt&0x0000FF0000000000;
			out++;
			*out = ionInt&0x000000FF00000000;
			out++;
			*out = ionInt&0x00000000FF000000;
			out++;
			*out = ionInt&0x0000000000FF0000;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 3:
			// 5 bytes of int
			*out = ION_INT5;
			out++;
			*out = ionInt&0x000000FF00000000;
			out++;
			*out = ionInt&0x00000000FF000000;
			out++;
			*out = ionInt&0x0000000000FF0000;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 4:
			// 4 bytes of int
			*out = ION_INT4;
			out++;
			*out = ionInt&0x00000000FF000000;
			out++;
			*out = ionInt&0x0000000000FF0000;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 5:
			// 3 bytes of int
			*out = ION_INT3;
			out++;
			*out = ionInt&0x0000000000FF0000;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 6:
			// 2 bytes of int
			*out = ION_INT2;
			out++;
			*out = ionInt&0x000000000000FF00;
			out++;
			*out = ionInt&0xFF;
			out++;
			break;
			case 7:
			if (ionInt == 1)
			{
				// 1
				*out = ION_0;
				out++;
			} else {
				// byte 1 of int
				*out = ION_INT1;
				out++;
				*out = ionInt&0xFF;
				out++;
			}
			break;
			case 8:
			// 0
			*out = ION_0;
			out++;
			break;
			default:
			break;
		}
		goto loop;
	}

	"-"?( "0"|([1-9][0-9]*) ) ( ("." [0-9]+) |  (("e"|"E") [+-] [0-9]+) | ( ("." [0-9]+) (("e"|"E") [+-] [0-9]+) ) )  { // JSON floating
		Data ionNumber;
		u64 ionInt;
		
		ionNumber.d = atof( (const char *)start );
		ionInt = (u64)ionNumber.i;
		*out = ION_FLOAT;
		out++;
		*out = ionInt&0xFF00000000000000;
		out++;
		*out = ionInt&0x00FF000000000000;
		out++;
		*out = ionInt&0x0000FF0000000000;
		out++;
		*out = ionInt&0x000000FF00000000;
		out++;
		*out = ionInt&0x00000000FF000000;
		out++;
		*out = ionInt&0x0000000000FF0000;
		out++;
		*out = ionInt&0x000000000000FF00;
		out++;
		*out = ionInt&0xFF;
		out++;
		goto loop;
	}

	 ["] ([^"] | ([\\] ["]))* ["] { // JSON string
		u64 length;
		length = YYCURSOR-start-2;
		start++;
		if (length < 45)
		{
			*out = ION_STRING01+(length-1);
		} else {
			*out = ION_STRING_N;
		}
		out++;
		memcpy(out, start, length);
		goto loop;
	}

	*/                               // end of re2c block
}
