/* fith compiler */
/* PUBLIC DOMAIN */
/* ION helper functions */

static u32 ION_getLen(const u8 *input)
{
	u32 fson_length;

	// get length of ION
	fson_length= (((*input)&0x3F)<<24);
	fson_length+=((*(input+1))<<16);
	fson_length+=((*(input+2))<<8);
	fson_length+= (*(input+3));
	return fson_length;
}

static f64
ION_toDouble(const u8 *input)
{
	Data ionNumber;
	u64 ionInt;
		
	ionInt = (((u64)(*input))<<56);
	input++;
	ionInt += (((u64)(*input))<<48);
	input++;
	ionInt += (((u64)(*input))<<40);
	input++;
	ionInt += (((u64)(*input))<<32);
	input++;
	ionInt += (((u64)(*input))<<24);
	input++;
	ionInt += (((u64)(*input))<<16);
	input++;
	ionInt += (((u64)(*input))<<8);
	input++;
	ionInt += (*input);
	ionNumber.i = (s64)ionInt;
	
	return ionNumber.d;
}

static s64
ION_Int(const u8 *input, u64 len)
{
	u64 ionInt=0;
	
	switch(8-len)
	{
		case 0:
		ionInt = (((u64)(*input))<<56);
		input++;
		case 1:
		ionInt += (((u64)(*input))<<48);
		input++;
		case 2:
		ionInt += (((u64)(*input))<<40);
		input++;
		case 3:
		ionInt += (((u64)(*input))<<32);
		input++;
		case 4:
		ionInt += (((u64)(*input))<<24);
		input++;
		case 5:
		ionInt += (((u64)(*input))<<16);
		input++;
		case 6:
		ionInt += (((u64)(*input))<<8);
		input++;
		case 7:
		ionInt += (*input);
	}
	
	return (s64)ionInt;
}

// types 0=float 1=int 2=string 3=ion
static u32
ION_getValLen(Data val, u32 type)
{
	u32 val_length;
	switch(type)
	{
		case 0:
		val_length=9;
		break;
		case 1:
		//if(val.i==1){
			//val_length=1;
		//} else {
		val_length= 9-((__builtin_clzl((((u64)val.i)&0xFFFFFFFFFFFFFFFE)))>>3);//}
		break;
		case 2:
		val_length=strlen((const char *)val.s)+1;
		if(val_length>45){val_length++;}
		break;
		case 3:
		val_length = ION_getLen(val.s);
		break;
	}
	return val_length;
}

static void
ION_writeInt(u8 **outx, s64 val)
{
	u64 ionInt = (u64) val;
	u8 *out = *outx;
	s32 lz;
	lz = __builtin_clzl(ionInt);
	if (lz==0)
	{
		// negative number
		*out = ION_NEG;
		out++;
		// get absolute value
		ionInt = (u64)labs(val);
		// retake leading zeros
		lz = __builtin_clzl(ionInt);
	}
	lz/=8;
	switch (lz){
		case 0:
		// 8 bytes of int
		*out = ION_INT8;
		out++;
		*out = (ionInt&0xFF00000000000000)>>56;
		out++;
		*out = (ionInt&0x00FF000000000000)>>48;
		out++;
		*out = (ionInt&0x0000FF0000000000)>>40;
		out++;
		*out = (ionInt&0x000000FF00000000)>>32;
		out++;
		*out = (ionInt&0x00000000FF000000)>>24;
		out++;
		*out = (ionInt&0x0000000000FF0000)>>16;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 1:
		// 7 bytes of int
		*out = ION_INT7;
		out++;
		*out = (ionInt&0x00FF000000000000)>>48;
		out++;
		*out = (ionInt&0x0000FF0000000000)>>40;
		out++;
		*out = (ionInt&0x000000FF00000000)>>32;
		out++;
		*out = (ionInt&0x00000000FF000000)>>24;
		out++;
		*out = (ionInt&0x0000000000FF0000)>>16;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 2:
		// 6 bytes of int
		*out = ION_INT6;
		out++;
		*out = (ionInt&0x0000FF0000000000)>>40;
		out++;
		*out = (ionInt&0x000000FF00000000)>>32;
		out++;
		*out = (ionInt&0x00000000FF000000)>>24;
		out++;
		*out = (ionInt&0x0000000000FF0000)>>16;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 3:
		// 5 bytes of int
		*out = ION_INT5;
		out++;
		*out = (ionInt&0x000000FF00000000)>>32;
		out++;
		*out = (ionInt&0x00000000FF000000)>>24;
		out++;
		*out = (ionInt&0x0000000000FF0000)>>16;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 4:
		// 4 bytes of int
		*out = ION_INT4;
		out++;
		*out = (ionInt&0x00000000FF000000)>>24;
		out++;
		*out = (ionInt&0x0000000000FF0000)>>16;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 5:
		// 3 bytes of int
		*out = ION_INT3;
		out++;
		*out = (ionInt&0x0000000000FF0000)>>16;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 6:
		// 2 bytes of int
		*out = ION_INT2;
		out++;
		*out = (ionInt&0x000000000000FF00)>>8;
		out++;
		*out = ionInt&0xFF;
		out++;
		break;
		case 7:
		if (ionInt == 1)
		{
			// 1
			*out = ION_1;
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
	*outx = out;
}

static void
ION_writeFloat(u8 **outx, f64 val)
{
	Data data;
	data.d = val;
	u64 ionInt = (u64) data.i;
	u8 *out = *outx;
	*out = ION_FLOAT;
	out++;
	*out = (ionInt&0xFF00000000000000)>>56;
	out++;
	*out = (ionInt&0x00FF000000000000)>>48;
	out++;
	*out = (ionInt&0x0000FF0000000000)>>40;
	out++;
	*out = (ionInt&0x000000FF00000000)>>32;
	out++;
	*out = (ionInt&0x00000000FF000000)>>24;
	out++;
	*out = (ionInt&0x0000000000FF0000)>>16;
	out++;
	*out = (ionInt&0x000000000000FF00)>>8;
	out++;
	*out = ionInt&0xFF;
	out++;
	*outx = out;
}

static void
ION_writeString(u8 **outx, const u8 *string, u32 len)
{
	u8 *out = *outx;
	if (len < 46)
	{
		*out = ION_STRING01+len-2;
	} else {
		*out = ION_STRING_N;
		out++;
		memcpy(out, string, len-2);
		out+=len-2;
		*out =0; //null terminate long strings
		out++;
		*outx = out;
		return;
	}
	out++;
	memcpy(out, string, len-1);
	out+=len-1;
	*outx = out;
	return;
}

static void
ION_writeION(u8 **outx, const u8 *ion, u32 len)
{
	u8 *out = *outx;
	len-=4;
	memcpy(out, (ion+4), len);
	out+=len;
	*outx = out;
	return;
}

// types 0=float 1=int 2=string 3=ion
static void
ION_writeVal(u8 **outx, Data val, u32 type, u32 len)
{
	
	switch(type)
	{
		case 0:
		ION_writeFloat(outx, val.d);
		break;
		case 1:
		ION_writeInt(outx, val.i);
		break;
		case 2:
		ION_writeString(outx, val.s, len);
		break;
		case 3:
		ION_writeION(outx, val.s, len);
		break;
	}
}

// types 0=float 1=int 2=string 3=ion
static u8 *
ION_appVal(const u8 *input, Data val, u32 type, u32 fson_length)
{
	const u8 *cursor = input;
	const u8 *insert_location = input+fson_length-1;
	u8 *buff;
	u8 *buffp;
	u8 *endp;
	//u32 fson_length;
	u32 val_length;
	u32 insert_offset=insert_location-input;
	//printf("Array append!\n" );
	//fson_length = ION_getLen(input);
	cursor+=4;
	// get length of val
	val_length = ION_getValLen(val, type);
	// allocate new buffer
	//buff = malloc(fson_length+val_length);
	buff = realloc((u8*)input, fson_length+val_length);
	buffp = buff+4;
	if(buff!=input)
	{
		
		//printf("attempting to add to an Object NEW BUFF!!!\n" );
		//memcpy(buffp, cursor, insert_location-cursor); //copy over begining
		insert_location = buff+insert_offset;
	}
	buffp+=(insert_offset-4);
	// end first
	endp=buffp+val_length;
	memcpy(endp, insert_location, fson_length-(insert_offset)); //copy ending
	// done with end
	ION_writeVal(&buffp, val, type, val_length);
	//copy ending done above
	buffp+=(fson_length-(insert_offset));
	fson_length = buffp - buff; // get length
	//length +=3;
	//length /=4;
	buff[0] = ((fson_length&0x00000000FF000000)>>24)|0x80;
	//printf("header,%d\n", buff[0]);
	buff[1] = (fson_length&0x0000000000FF0000)>>16;
	buff[2] = (fson_length&0x000000000000FF00)>>8;
	buff[3] = fson_length&0xFF;
	return buff;
}

// types 0=float 1=int 2=string 3=ion
static u8 *
ION_newValInsert(const u8 *input, const u8 *insert_location, Data val, u32 type)
{
	const u8 *cursor = input;
	u8 *buff;
	u8 *buffp;
	u32 fson_length;
	u32 val_length;
	u32 key_length;
	
	fson_length = ION_getLen(input);
	cursor+=4;
	// get length of val
	val_length = ION_getValLen(val, type);
	// allocate new buffer
	buff = malloc(fson_length+val_length);
	buffp = buff+4;
	
	//printf("attempting to add a value to an Object!!!\n");
	memcpy(buffp, cursor, insert_location-cursor); //copy over begining
	buffp+=(insert_location-cursor);
	// make insert
	ION_writeVal(&buffp, val, type, val_length);
	//copy ending
	memcpy(buffp, insert_location, fson_length-(insert_location-input)); 
	buffp+=(fson_length-(insert_location-input));
	fson_length = buffp - buff; // get length
	//length +=3;
	//length /=4;
	buff[0] = ((fson_length&0x00000000FF000000)>>24)|0x80;
	buff[1] = (fson_length&0x0000000000FF0000)>>16;
	buff[2] = (fson_length&0x000000000000FF00)>>8;
	buff[3] = fson_length&0xFF;
	
	return buff;
}


// need to skip over old value for copy
static u8 *
ION_newValOverWrite(const u8 *input, const u8 *insert_location, Data val, u32 type)
{
	const u8 *cursor = input;
	u8 *buff;
	u8 *buffp;
	u32 fson_length;
	u32 val_length;
	u32 key_length;
	
	fson_length = ION_getLen(input);
	cursor+=4;
	// get length of val
	val_length = ION_getValLen(val, type);
	// allocate new buffer
	buff = malloc(fson_length+val_length);
	buffp = buff+4;
	
	//printf("attempting to add a value to an Object!!!\n");
	memcpy(buffp, cursor, insert_location-cursor); //copy over begining
	buffp+=(insert_location-cursor);
	ION_writeVal(&buffp, val, type, val_length);
	// skip over value we are replacing
	lex_skipVal(&insert_location);
	memcpy(buffp, insert_location, fson_length-(insert_location-input)); //copy ending
	buffp+=(fson_length-(insert_location-input));
	fson_length = buffp - buff; // get length
	//length +=3;
	//length /=4;
	buff[0] = ((fson_length&0x00000000FF000000)>>24)|0x80;
	buff[1] = (fson_length&0x0000000000FF0000)>>16;
	buff[2] = (fson_length&0x000000000000FF00)>>8;
	buff[3] = fson_length&0xFF;
	
	return buff;
}

// types 0=float 1=int 2=string 3=ion
static u8 *
ION_newKeyVal(const u8 *input, const u8 *insert_location, Data val, u32 type, const u8 *path_ending)
{
	const u8 *cursor = input;
	u8 *buff;
	u8 *buffp;
	Data pathv;
	u32 fson_length;
	u32 val_length;
	u32 key_length;
	
	pathv.s=(u8*)path_ending;
	key_length= ION_getValLen(pathv, 2);
	fson_length = ION_getLen(input);
	cursor+=4;
	// get length of val
	val_length = ION_getValLen(val, type);
	// allocate new buffer
	buff = malloc(fson_length+val_length+key_length);
	buffp = buff+4;
	
	printf("attempting to add to an Object!!!, %s\n",path_ending );
	memcpy(buffp, cursor, insert_location-cursor); //copy over begining
	buffp+=(insert_location-cursor);
	ION_writeVal(&buffp, pathv, 2, key_length);
	ION_writeVal(&buffp, val, type, val_length);
	memcpy(buffp, insert_location, fson_length-(insert_location-input)); //copy ending
	buffp+=(fson_length-(insert_location-input));
	fson_length = buffp - buff; // get length
	//length +=3;
	//length /=4;
	buff[0] = ((fson_length&0x00000000FF000000)>>24)|0x80;
	buff[1] = (fson_length&0x0000000000FF0000)>>16;
	buff[2] = (fson_length&0x000000000000FF00)>>8;
	buff[3] = fson_length&0xFF;
	
	return buff;
}
