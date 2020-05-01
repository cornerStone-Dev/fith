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
