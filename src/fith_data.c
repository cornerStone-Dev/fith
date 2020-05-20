/* fith compiler */
/* PUBLIC DOMAIN */

typedef struct heap_data_s {
	u8 *h;
	u8 *cache;
	u64 i;
	u64 b;
	u64 t;
	u64 generation_size;
	u32 generation_count;
} Heap;
static Heap heap_data;

typedef struct var_data_s {
	s64 *v;
	u32 i;
	u32 hw;
} Vars;
static Vars var_data;


// returns a pointer to one
static void *
heap_malloc(size_t bytes)
{
	void *p;
	u64 next_index;
	
	next_index = heap_data.i+bytes;
	while (next_index>heap_data.t) // garbage collection time
	{
		garbage_collect(bytes);
		next_index = heap_data.i+bytes;
	}
	p = &heap_data.h[heap_data.i];
	heap_data.cache = p;
	heap_data.i = next_index;
	return p;
}

// returns a pointer to one
static void *
heap_realloc(u8 *ptr, size_t bytes)
{
	u8 *p;
	u64 next_index, cache_offset;
	u32 len;
	
	if ((ptr) == heap_data.cache)
	{
		cache_offset = (u64)(heap_data.cache - heap_data.h);
		next_index = cache_offset+bytes;
		if ((next_index)<=heap_data.t)
		{
			//printf("expanded allocation!\n");
			heap_data.i = next_index;
			return ptr;
		}
		
	}
	// fresh allocation
	p = heap_malloc(bytes);
	if(((*ptr)&0xC0)==0x80)
	{
		len = ION_getLen(ptr);
	} else {
		len = strlen((const char *)ptr)+1;//copy the null
	}
	if(len>bytes)
	{
		len = bytes;
	}
	fith_memcpy(p, ptr, len);
	return p;
	
}

#define JEACH_TEXT "SELECT json_each.type,json_each.value FROM json_each(?);"
//~ static s64
//~ fith_json_each(u8 *json, Data *val, sqlite3_stmt **sql)
//~ {
	//~ const u8 *res;
	//~ u8 *ret;
	//~ s64 step;
	//~ sqlite3_stmt *s;
	
	//~ if (*sql)
	//~ {
		//~ s=*sql;
	//~ } else {
		//~ sqlite3_prepare_v2(fdb,
						//~ JEACH_TEXT,
						//~ sizeof JEACH_TEXT-1,
						//~ &s,
						//~ 0);
		//~ sqlite3_bind_text(s, 1, (const char *)json, -1, SQLITE_TRANSIENT);
		//~ *sql=s;
	//~ }
	
	//~ /* prepare SQL query */
	//~ step = sqlite3_step(s);
	//~ if ((step = (step == SQLITE_ROW))) {
		//~ res=sqlite3_column_text(s, 0);
		//~ switch(res[0]) {
			//~ case 0:   // NULL
			//~ res = (const u8 *)"NULL";
			//~ goto copy_exit;
			//~ case 'a': // array
			//~ goto text_exit;
			//~ case 'f': // false
			//~ goto copy_exit;
			//~ case 'i': // integer
			//~ val->i=sqlite3_column_int64(s, 1);
			//~ goto reset_exit;
			//~ case 'n': // null
			//~ goto copy_exit;
			//~ case 'o': // object
			//~ goto text_exit;
			//~ case 'r': // real
			//~ val->d=sqlite3_column_double(s, 1);
			//~ goto reset_exit;
			//~ case 't': // true or text
			//~ if (res[1]=='r') { // true
				//~ goto copy_exit;
			//~ }
			//~ goto text_exit;
			//~ default: return -1;
		//~ }
		//~ text_exit:
		//~ res=sqlite3_column_text(s, 1);
		//~ copy_exit:
		//~ ret = logged_malloc_block(sqlite3_column_bytes(s,1));
		//~ strcpy((char *)ret, (const char *)res);
		//~ val->s = ret;
	//~ }
	//~ reset_exit:
	//~ return step;
//~ }


static void
save_function_addr(u8 *start, u64 len, u8 *addr)
{
	s64 a=(s64)addr;
	u8 tmp;
	
	tmp=start[len];
	start[len]=0;
	StringTos64Tree_insert(&fns, start, len, a);
	start[len]=tmp;
}

static s64
get_function_addr(u8 *start, u64 len)
{
	StringTos64Node node;
	u8 tmp;

	tmp=start[len];
	start[len]=0;
	node=StringTos64Tree_find(fns, start);
	if (node==0)
	{
		start[len]=tmp;
		return 0;
	}
	start[len]=tmp;
	return node->val;
}

static void
save_variable(u8 *start, u64 len, s64 val)
{
	StringTos64Node node;
	u8 *p;
	u32 index;
	u8 tmp;
	
	tmp=start[len];
	start[len]=0;
	node=StringTos64Tree_find(vars, start);
	if (node==0) // no variable saved
	{
		var_data.v[var_data.i] = val;
		index = var_data.i;
		var_data.i++;
		if(var_data.i>var_data.hw){
			var_data.hw+=512;
			p = realloc(var_data.v, (var_data.hw/512+1)*4096);
			if (p==0){
				printf("OUT OF MEMORY!!!\n"); exit(1);
			}
			var_data.v=(s64 *)p;
		}
		StringTos64Tree_insert(&vars, start, len, index);
		start[len]=tmp;
	} else {
		var_data.v[node->val] = val;
		start[len]=tmp;
	}
}

static s32
get_variable(u8 *start, u64 len, s64 *val)
{
	StringTos64Node node;
	u8 tmp;

	tmp=start[len];
	start[len]=0;
	node=StringTos64Tree_find(vars, start);
	if (node==0)
	{
		start[len]=tmp;
		return 0;
	}
	start[len]=tmp;
	*val=var_data.v[node->val];
	return 1;
}

// binary search of a sorted array of s64
static s64
fith_binary_search(s64 *array, s64 size, s64 target)
{
	s64 diff;
	s64 min, max, search_index;
	
	min = 0;
	max = size - 1;
	while(1){
		if (max < min)
		{
			return -1;
		}
		search_index = (min + max) / 2;
		diff = target - array[search_index];
		if (diff==0)
		{
			return search_index;
		}
		if(diff>0)
		{
			min = search_index + 1;
		} else {
			max = search_index - 1;
		}
	}
}


typedef struct tuple_s {
	s64 x;
	s64 y;
} tuple;

static __inline s64 CMP_TUPLE(tuple a,tuple b)
{
	return ((a.x) - (b.x));
}

/*  Used in mergesort. */
static __inline void INSERTION_SORT_tuple(tuple *dsti, const size_t size) 
{
	size_t i=1;
	tuple *dst=dsti;
	
	for (; i < size; i++) 
	{
		size_t j;
		tuple x;

		/* If this entry is already correct, just move along */
		if (CMP_TUPLE(dst[i - 1], dst[i]) <= 0)
		{
			continue;
		}

		/* Else we need to find the right place while shifting everything over */
		x = dst[i];
		j = i;

		for (;;) 
		{
			dst[j] = dst[j-1];
			j--;
			if ( ((j == 0)||(CMP_TUPLE(dst[j-1], x) <= 0)) )
			{
				break;
			}
		}
		dst[j] = x;
	}
}

/* Standard merge sort */
static void
MERGE_SORT_RECURSIVE_tuple(tuple *newdsti, tuple *dsti, const size_t size) {
	tuple *newdst=newdsti;
	tuple *dst=dsti;
	const size_t middle = size / 2;
	size_t out = 0;
	size_t i = 0;
	size_t j = middle;

	if (size <= 16) {
		INSERTION_SORT_tuple(dst, size);
		return;
	}

	MERGE_SORT_RECURSIVE_tuple(newdst, dst, middle);
	MERGE_SORT_RECURSIVE_tuple(newdst, &dst[middle], size - middle);

	if (CMP_TUPLE(dst[middle - 1], dst[middle]) <= 0)
		{ return; }

	/* copy off left side */
	for (; out < middle; out++) {
		newdst[out]=dst[out];
	}
	out=0;

	for (; i < middle && j < size; ++out) {
		if (CMP_TUPLE(newdst[i], dst[j]) <= 0) {
			dst[out] = newdst[i++];
		} else {
			dst[out] = dst[j++];
		}
	}

	for (; i < middle; ++out) {
		dst[out] = newdst[i++];
	}
}

/* Standard merge sort */
static void
MERGE_SORT_tuple(tuple *dst, const size_t size) {
	tuple *newdst;

	/* don't bother sorting an array of size <= 1 */
	if (size <= 1) {
		return;
	}

	newdst = malloc((size/2)*sizeof(tuple));

	MERGE_SORT_RECURSIVE_tuple(newdst, dst, size);
	free(newdst);
}


static __inline s64 CMP_INT(s64 x,s64 y)
{
	return ((x) - (y));
}

/*  Used in mergesort. */
static void INSERTION_SORT_s64(s64 *dsti, const size_t size) 
{
	size_t i=1;
	s64 *dst=dsti;
	
	for (; i < size; i++) 
	{
		size_t j;
		s64 x;

		/* If this entry is already correct, just move along */
		if (CMP_INT(dst[i - 1], dst[i]) <= 0)
		{
			continue;
		}

		/* Else we need to find the right place while shifting everything over */
		x = dst[i];
		j = i;

		for (;;) 
		{
			dst[j] = dst[j-1];
			j--;
			if ( ((j == 0)||(CMP_INT(dst[j-1], x) <= 0)) )
			{
				break;
			}
		}
		dst[j] = x;
	}
}

/* Standard merge sort */
static void
MERGE_SORT_RECURSIVE_s64(s64 *newdsti, s64 *dsti, const size_t size) {
	s64 *newdst=newdsti;
	s64 *dst=dsti;
	const size_t middle = size / 2;
	size_t out = 0;
	size_t i = 0;
	size_t j = middle;

	if (size <= 16) {
		INSERTION_SORT_s64(dst, size);
		return;
	}

	MERGE_SORT_RECURSIVE_s64(newdst, dst, middle);
	MERGE_SORT_RECURSIVE_s64(newdst, &dst[middle], size - middle);

	if (CMP_INT(dst[middle - 1], dst[middle]) <= 0)
		{ return; }

	/* copy off left side */
	for (; out < middle; out++) {
		newdst[out]=dst[out];
	}
	out=0;

	for (; i < middle && j < size; ++out) {
		if (CMP_INT(newdst[i], dst[j]) <= 0) {
			dst[out] = newdst[i++];
		} else {
			dst[out] = dst[j++];
		}
	}

	for (; i < middle; ++out) {
		dst[out] = newdst[i++];
	}
}

/*  Used in mergesort. */
static int REVERSE_SORT_s64(s64 *dsti, const size_t size) 
{
	s64 *dst=dsti;
	s64 x;
	size_t i;
	size_t j;
	size_t n;
	size_t p;
	size_t num_reverse;
	size_t off;

	/* filter for reverse runs */
	for (i = 1; i < size; i++)
	{
		/* If this entry is already correct, just move along */
		if (CMP_INT(dst[i - 1], dst[i]) <= 0) 
		{
			if(i>33)
			{
				return 0;
			}
			continue;
		} else 
		{
			off=i-1;
			num_reverse=0;
			do{
				num_reverse++;
				i++;
			} while((i < size)&&(CMP_INT(dst[i - 1], dst[i]) > 0));
			n=num_reverse;
			num_reverse++;

			j=num_reverse/2; /* num items to cpy */
			for (p = 0; p < j; p++) /* top part */
			{
				x=dst[p+off];
				dst[p+off]=dst[n+off];
				dst[n+off]=x;
				n--;
			}
			if(num_reverse==size)
			{
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

/* Standard merge sort */
static void
MERGE_SORT_s64(s64 *dst, const size_t size) {
	s64 *newdst;

	/* don't bother sorting an array of size <= 1 */
	if (size <= 1) {
		return;
	}

	if (size <= 16) {
		INSERTION_SORT_s64(dst, size);
		return;
	}

	if(REVERSE_SORT_s64(dst, size))
	{
		return;
	}

	newdst = malloc((size/2)*sizeof(s64));

	MERGE_SORT_RECURSIVE_s64(newdst, dst, size);
	free(newdst);
}

#define HEAP_PTR(a) (u8 *)((a)+heap_data.h)
static void
garbage_collect(u64 last_requested_size)
{
	u8  *pBottom, *pTmp;
	u64 bottom, top, tmp, bucket;
	u32 x=0, size;
	u8 copy_collect=0;
	tuple variables[2048];
	
	top = (u64)(heap_data.i + heap_data.h);
	// increment and check generation count
	heap_data.generation_count++;
	
	// check if heap needs upgraded
	bucket = (heap_data.generation_size+last_requested_size)*8;
	// check if current partition is undersized
	if (bucket > (heap_data.t+1))
	{
		copy_collect = 1;
		heap_data.generation_count = 0;
	}
	
	if (heap_data.generation_count%8)
	{
		bottom = (u64)(heap_data.b + heap_data.h);
	} else {
		bottom = (u64)heap_data.h;
		heap_data.b = 0;
	}
	// copy filtered variables into array
	for(u32 y=0;y<var_data.i;y++)
	{
		tmp = var_data.v[y];
		if ( (tmp>=bottom)&&(tmp<top) )
		{
			variables[x].x = (s64)tmp-(u64)heap_data.h;
			variables[x].y = y;
			x++;
		}
	}
	
	// TODO record stack vars (maybe locals) by pointer
	
	// sort vars
	MERGE_SORT_tuple(variables, x);
	
	if (copy_collect)
	{
		bucket = ((bucket/(128*1024))+1) * (128*1024);
		pTmp = realloc(heap_data.h, bucket);
		if(pTmp==0)
		{
			printf("CANNOT EXPAND HEAP!!!\n");
			
		} else {
			//~ if (heap_data.h==pTmp){
				//~ printf("REALLOC WORKED!!!\n");
			//~ }
			heap_data.h=pTmp;
			heap_data.t = bucket-1;
			//printf("LARGEST ADDR %ld, top %ld\n", (s64)heap_data.h+heap_data.t, heap_data.t);
		}
	}

	// starting at bottom, copy down all valid pointers
	pBottom = (u8 *)(heap_data.b + heap_data.h);
	
	for(u32 y=0;y<x;y++)
	{
		// get size
		if(((*(HEAP_PTR(variables[y].x)))&0xC0)==0x80)
		{
			size = ION_getLen(HEAP_PTR(variables[y].x));
		} else {
			size = strlen((const char *)HEAP_PTR(variables[y].x))+1;//copy the null
		}
		
		// copy into bottom
		fith_memcpy(pBottom, HEAP_PTR(variables[y].x), size);
		// overwrite pointer to new location
		var_data.v[variables[y].y] = (s64)(pBottom);
		// move pointer forward
		pBottom+=size;
	}
	// record size of young generation survivors
	heap_data.generation_size = (pBottom-(u8 *)(heap_data.b + heap_data.h));
	// move bottom up to new bottom
	heap_data.b += heap_data.generation_size;
	// set index to new bottom
	heap_data.i = heap_data.b;

}





