/* fith words */

static inline Registers
push_i(Context1 *c, Registers r, s64 val) __attribute__((always_inline));
static inline Registers
push_d(Context1 *c, Registers r, f64 d) __attribute__((always_inline));
static inline Registers
push_s(Context1 *c, Registers r, u8 *s) __attribute__((always_inline));
static inline Registers
bitwise_and(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
bitwise_or(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
equals(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
not_equals(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
equalsf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
not_equalsf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
less_than(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
greater_than(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
less_than_orEquals(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
greater_than_orEquals(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
less_thanf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
greater_thanf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
less_than_orEqualsf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
greater_than_orEqualsf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
shift_left(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
shift_right(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
add(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
sub(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
divi(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
mul(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
addf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
subf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
divif(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
mulf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
modulo(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
modulof(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
bitwise_not(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
print_stack(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
print_stackf(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
print_string(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
logical_or(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
logical_and(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
logical_not(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
pick_number(Context1 *c, Registers r, u64 num) __attribute__((always_inline));
static inline Registers
_fith_dup(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_dep(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_rot(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_abs(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_i2s(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_f2s(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_s2i(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_s2f(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_rev(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_over(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_pick(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_drop(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_swap(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_sort(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_fabs(Context1 *c, Registers r) __attribute__((always_inline));
static Registers
_fith_chan(Context1 *c, Registers r);
static inline Registers
_fith_clear(Context1 *c, Registers r) __attribute__((always_inline));
static Registers
_fith_sleep(Context1 *c, Registers r);
static Registers
_fith_array(Context1 *c, Registers r);
static Registers
_fith_malloc(Context1 *c, Registers r);
static inline Registers
_fith_free(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_random(Context1 *c, Registers r) __attribute__((always_inline));
static Registers
_fith_realloc(Context1 *c, Registers r);
static inline Registers
_fith_array_get(Context1 *c, Registers r) __attribute__((always_inline));
static inline Registers
_fith_array_set(Context1 *c, Registers r) __attribute__((always_inline));

static inline Registers
push_i(Context1 *c, Registers r, s64 i)
{
	r.sp->i = r.tos.i;
	r.tos.i = i;
	r.sp++;
	return r;
}

static inline Registers
push_d(Context1 *c, Registers r, f64 d)
{
	r.sp->i = r.tos.i;
	r.tos.d = d;
	r.sp++;
	return r;
}

static inline Registers
push_s(Context1 *c, Registers r, u8 *s)
{
	r.sp->i = r.tos.i;
	r.tos.s = s;
	r.sp++;
	return r;
}

static inline Registers
bitwise_and(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i&r.tos.i;
	return r;
}

static inline Registers
bitwise_xor(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i^r.tos.i;
	return r;
}

static inline Registers
bitwise_or(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i|r.tos.i;
	return r;
}

static inline Registers
equals(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i==r.tos.i;
	return r;
}

static inline Registers
not_equals(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i!=r.tos.i;
	return r;
}

static inline Registers
equalsf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d==r.tos.d;
	return r;
}

static inline Registers
not_equalsf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d!=r.tos.d;
	return r;
}

static inline Registers
less_than(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i<r.tos.i;
	return r;
}

static inline Registers
greater_than(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i>r.tos.i;
	return r;
}

static inline Registers
less_than_orEquals(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i<=r.tos.i;
	return r;
}

static inline Registers
greater_than_orEquals(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i>=r.tos.i;
	return r;
}

static inline Registers
less_thanf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d<r.tos.d;
	return r;
}

static inline Registers
greater_thanf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d>r.tos.d;
	return r;
}

static inline Registers
less_than_orEqualsf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d<=r.tos.d;
	return r;
}

static inline Registers
greater_than_orEqualsf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d>=r.tos.d;
	return r;
}

static inline Registers
shift_left(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i<<r.tos.i;
	return r;
}

static inline Registers
shift_right(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i>>r.tos.i;
	return r;
}

static inline Registers
add(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i+r.tos.i;
	return r;
}

static inline Registers
sub(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i-r.tos.i;
	return r;
}

static inline Registers
divi(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i/r.tos.i;
	return r;
}

static inline Registers
mul(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i*r.tos.i;
	return r;
}

static inline Registers
addf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d+r.tos.d;
	return r;
}

static inline Registers
subf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d-r.tos.d;
	return r;
}

static inline Registers
divif(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d/r.tos.d;
	return r;
}

static inline Registers
mulf(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.d = r.sp->d*r.tos.d;
	return r;
}

static inline Registers
modulo(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i%r.tos.i;
	return r;
}

static inline Registers
modulof(Context1 *c, Registers r)
{
	f64 var;	
	r.sp--;
	var = r.sp->d/r.tos.d;
	var = modf(var, &r.sp->d);
	r.tos.d = var * r.tos.d;
	return r;
}

static inline Registers
print_stack(Context1 *c, Registers r)
{
	u32 x=1;
	if(&c->stk_start[0]!=r.sp){
		for(; &c->stk_start[x]!=r.sp;x++){
			printf("(%ld) ",c->stk_start[x].i);
		}
		printf("(%ld) ",r.tos.i);
	}
	printf("\n");
	return r;
}

static inline Registers
print_stackf(Context1 *c, Registers r)
{
	u32 x=1;
	if(&c->stk_start[0]!=r.sp){
		for(; &c->stk_start[x]!=r.sp;x++){
			printf("(%f) ",c->stk_start[x].d);
		}
		printf("(%f) ",r.tos.d);
	}
	printf("\n");
	return r;
}

static inline Registers
bitwise_not(Context1 *c, Registers r)
{
	r.tos.i = ~r.tos.i;
	return r;
}

static inline Registers
print_string(Context1 *c, Registers r)
{
	r.sp--;
	printf("%s",(const char *)r.tos.s);
	printf("\n");
	r.tos.i = r.sp->i;
	return r;
}

static inline Registers
logical_or(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i||r.tos.i;
	return r;
}

static inline Registers
logical_and(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i&&r.tos.i;
	return r;
}

static inline Registers
logical_not(Context1 *c, Registers r)
{
	r.tos.i = !r.tos.i;
	return r;
}

static inline Registers
pick_number(Context1 *c, Registers r, u64 num)
{
	r.sp->i = r.tos.i;
	r.tos.i = (r.sp-num)->i;
	r.sp++;
	return r;
}

static inline Registers
_fith_dup(Context1 *c, Registers r)
{
	//pick_number(c,r,0); compiler would barf on this
	r.sp->i = r.tos.i;
	r.sp++;
	return r;
}

static inline Registers
_fith_dep(Context1 *c, Registers r)
{
	r.sp->i = r.tos.i;
	r.tos.i = r.sp - c->stk_start;
	r.sp++;
	return r;
}

static inline Registers
_fith_rot(Context1 *c, Registers r)
{
	s64 tmp;	
	tmp = (r.sp-2)->i;
	(r.sp-2)->i = (r.sp-1)->i;
	(r.sp-1)->i = r.tos.i;
	r.tos.i = tmp;
	return r;
}

static inline Registers
_fith_abs(Context1 *c, Registers r)
{
	r.tos.i=labs(r.tos.i);
	return r;
}

static inline Registers
_fith_i2s(Context1 *c, Registers r)
{
	// save off value
	s64 tmp=r.tos.i;
	// get string
	r.tos.s = heap_malloc(24);
	sprintf((char *)r.tos.s, "%ld", tmp);
	return r;
}

static inline Registers
_fith_f2s(Context1 *c, Registers r)
{
	// save off value
	f64 tmp=r.tos.d;
	// get string
	r.tos.s = heap_malloc(24);
	sprintf((char *)r.tos.s, "%f", tmp);
	return r;
}

static inline Registers
_fith_s2i(Context1 *c, Registers r)
{
	r.tos.i = strtol( (const char *)r.tos.s, NULL, 0);
	return r;
}

static inline Registers
_fith_s2f(Context1 *c, Registers r)
{
	r.tos.d = atof( (const char *)r.tos.s );
	return r;
}

static inline Registers
_fith_rev(Context1 *c, Registers r)
{
	s64 tmp, num;
	s64 *start, *end;	
	num = r.tos.i;
	start = (s64 *)r.sp - num;
	end = (s64 *)r.sp - 1;
	// reverse items
	while(start<end){
		tmp = *start;
		*start = *end;
		*end = tmp;
		start++;
		end--;
	}
	r.sp-=1;
	r.tos.i = r.sp->i;
	return r;
}

static inline Registers
_fith_over(Context1 *c, Registers r)
{
	r.sp->i = r.tos.i;
	r.tos.i = (r.sp-1)->i;
	r.sp++;
	return r;
}

static inline Registers
_fith_pick(Context1 *c, Registers r)
{
	r.sp->i = r.tos.i;
	r.tos.i = (r.sp-2)->i;
	r.sp++;
	return r;
}

static inline Registers
_fith_drop(Context1 *c, Registers r)
{
	r.sp--;
	r.tos.i = r.sp->i;
	return r;
}

static inline Registers
_fith_swap(Context1 *c, Registers r)
{
	s64 tmp;	
	tmp = (r.sp-1)->i;
	(r.sp-1)->i = r.tos.i;
	r.tos.i = tmp;
	return r;
}

static inline Registers
_fith_sort(Context1 *c, Registers r)
{
	s64 num;	
	// TODO TEST
	num = r.tos.i;
	if(num>1){
		INSERTION_SORT_s64((s64 *)(r.sp-num), num);
	}
	r.sp-=1;
	r.tos.i = r.sp->i;
	return r;
}

static inline Registers
_fith_fabs(Context1 *c, Registers r)
{
	r.tos.d=fabs(r.tos.d);
	return r;
}

static Registers
_fith_chan(Context1 *c, Registers r)
{
	s32 res = socketpair(AF_UNIX, SOCK_STREAM, 0, (r.sp+2)->fd);
	if (res < 0)
	{
		printf("socketpair error!!!\n");
	} else {
		r.sp->i = r.tos.i;
		(r.sp+1)->i = (r.sp+2)->fd[0];
		r.tos.i = (r.sp+2)->fd[1];
		r.sp+=2;
	}
	return r;
}

static inline Registers
_fith_clear(Context1 *c, Registers r)
{
	r.sp=c->stk_start;
	return r;
}

static Registers
_fith_sleep(Context1 *c, Registers r)
{
	r.sp-=2;
	c->time.tv_sec = (r.sp+1)->i;
	c->time.tv_nsec = r.tos.i;
	r.tos.i = r.sp->i;
	nanosleep(&c->time, NULL);
	return r;
}

static Registers
_fith_array(Context1 *c, Registers r)
{
	// allocate array, 8 byte header
	
	u64 tmp = r.tos.i*8+8;
	r.tos.i = r.tos.i*8+8;
	r.tos.s= malloc(tmp);
	if (r.tos.s == 0 )
	{
		printf("malloc error!!!\n");
	}
	r.tos.v->i = tmp;
	return r;
}

static inline Registers
_fith_array_get(Context1 *c, Registers r)
{
	r.sp-=1;
	r.tos = r.tos.v[r.sp->i+1];
	return r;
}

static inline Registers
_fith_array_set(Context1 *c, Registers r)
{
	r.sp-=3;
	r.tos.v[(r.sp+2)->i+1] = *(r.sp+1);
	r.tos = *r.sp;
	return r;
}

static inline Registers
_fith_array_len(Context1 *c, Registers r)
{
	r.tos.i = (r.tos.v->i>>3)-1;
	return r;
}

static Registers
_fith_malloc(Context1 *c, Registers r)
{
	r.tos.s= malloc(r.tos.i);
	if (r.tos.s == 0 )
	{
		printf("malloc error!!!\n");
	}
	return r;
}

static inline Registers
_fith_free(Context1 *c, Registers r)
{
	free(r.tos.s);
	return _fith_drop(c,r);
}

static inline Registers
_fith_random(Context1 *c, Registers r)
{
	r.sp->i = r.tos.i;
	randomness(8, &r.tos.i);
	r.sp++;
	return r;
}

static Registers
_fith_realloc(Context1 *c, Registers r)
{
	void *ptr;
	r.sp--;
	ptr= realloc(r.sp->s, r.tos.i);
	if (ptr == 0)
	{
		printf("realloc error, allocation unchanged!\n");
		r.tos.s = r.sp->s;
		return r;
	}
	r.tos.s = ptr;
	return r;
}

