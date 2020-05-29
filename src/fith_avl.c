/* fith_avl.c */
/* PUBLIC DOMAIN */


#define CMP(x,y)  (strcmp(((const char *)x),(const char *)(y)))

#define LEFT 0
#define RIGHT 1
#define NEITHER -1

typedef struct StringTos64Node_s {
	struct StringTos64Node_s *next[2];
	s64                       val;
	s8                        longer;
	s8                        taken;
	u8                        len;
	u8                        key[5];
} *StringTos64Node;

static StringTos64Node fns, vars[64];
static u32 scope_index;

typedef struct StringTos64Nodenfo_s {
   struct StringTos64Node_s *n;
   s32 comeFrom;
} NodeInfo_i;

typedef struct trav_state_s {
   NodeInfo_i stack[48];
   s32 idx;
   s32 state;
} TravState_i;


#define	STARTED 0
#define	ENDED 1
#define	FROM_LEFT 2
#define	FROM_RIGHT 3
#define	FROM_SELF 4


#define Balanced(n) (n->longer < 0)


static void
StringTos64Tree_destroy(StringTos64Node *treep)
{
	StringTos64Node tree = *treep;
	
	if (tree==0){
		return;
	}
	StringTos64Tree_destroy(&tree->next[LEFT]);
	StringTos64Tree_destroy(&tree->next[RIGHT]);
	free(tree);
	*treep = 0;
}

static StringTos64Node
StringTos64Tree_traverse(StringTos64Node tree, TravState_i *s)
{
	if(s->state == STARTED){
		s->stack[s->idx].n=tree;
		// find leftmost child
		while (tree) {
			s->stack[s->idx].comeFrom= FROM_LEFT;
			s->idx++;
			s->stack[s->idx].n= tree->next[LEFT];
			tree = tree->next[LEFT];
		}
		// stack is ... (leftmost) (0) (null)
		//                         *
		s->idx--;
		// stack is ... (leftmost) (0) (null)
		//              *
		s->state=15;
		s->stack[s->idx].comeFrom=FROM_SELF;
		return s->stack[s->idx].n;
	}
	if(s->state == ENDED){return 0;}
	// re-entrant, just sent data
	if(s->stack[s->idx].comeFrom== FROM_SELF){
		tree = s->stack[s->idx].n;
		if(tree->next[RIGHT]){
			s->stack[s->idx].comeFrom= FROM_RIGHT;
			s->idx++;
			s->stack[s->idx].n= tree->next[RIGHT];
			tree = tree->next[RIGHT];
			if(tree->next[LEFT]){
				// find leftmost child
				while (tree) {
					s->stack[s->idx].comeFrom= FROM_LEFT;
					s->idx++;
					s->stack[s->idx].n= tree->next[LEFT];
					tree = tree->next[LEFT];
				}
				s->idx--;
				s->stack[s->idx].comeFrom=FROM_SELF;
				return s->stack[s->idx].n;
			} else { // right child
				s->stack[s->idx].comeFrom=FROM_SELF;
				return s->stack[s->idx].n;
			}
		}
		// no right child
		
		s->idx--;
		while((s->idx>=0)&&(s->stack[s->idx].comeFrom==FROM_RIGHT)){
			s->idx--;
		}
		if(s->idx<0){
			s->state = ENDED;
			return 0;
		}
		if(s->stack[s->idx].comeFrom==FROM_LEFT){
			s->stack[s->idx].comeFrom=FROM_SELF;
			return s->stack[s->idx].n;
		}
	}
	return tree;
}

static StringTos64Node
StringTos64Tree_find(StringTos64Node tree, u8 *target)
{
	s32 res;
	while (tree && ((res=CMP(target, tree->key))!=0) ) {
		s8 next_step = (res>0);
		tree = tree->next[next_step];
	}
	return tree;
}


static StringTos64Node
StringTos64Tree_rotate_2(StringTos64Node *path_top, s8 dir)
{
	StringTos64Node B, C, D, E;
	B = *path_top;
	D = B->next[dir];
	C = D->next[1-dir];
	E = D->next[dir];

	*path_top = D;
	D->next[1-dir] = B;
	B->next[dir] = C;
	B->longer = NEITHER;
	D->longer = NEITHER;
	return E;
}


static StringTos64Node
StringTos64Tree_rotate_3(StringTos64Node *path_top, s8 dir, s8 third)
{
	StringTos64Node B, F, D, C, E;
	B = *path_top;
	F = B->next[dir];
	D = F->next[1-dir];
	/* note: C and E can be NULL */
	C = D->next[1-dir];
	E = D->next[dir];
	*path_top = D;
	D->next[1-dir] = B;
	D->next[dir] = F;
	B->next[dir] = C;
	F->next[1-dir] = E;
	D->longer = NEITHER;

	/* assume both trees are balanced */
	B->longer = F->longer = NEITHER;

	if (third == NEITHER)
		return NULL;
	else if (third == dir) {
		/* E holds the insertion so B is unbalanced */ 
		B->longer = 1-dir;
		return E;
	} else {
		/* C holds the insertion so F is unbalanced */
		F->longer = dir;
		return C;
	}
}


/***************************************************
 * INSERTION                                       *
 ***************************************************/
static inline void
StringTos64Tree_rebalance_path(StringTos64Node path, u8 *target)
{
	/* Each StringTos64Node in path is currently balanced.
	 * Until we find target, mark each StringTos64Node as longer
	 * in the s32 of target because we know we have
	 * inserted target there
	 */
	if(path!=0){
		while ( path->taken > NEITHER ) {
			s8 next_step = (path->taken);
			path->longer = next_step;
			path = path->next[next_step];
		}
	}
}

static inline void
StringTos64Tree_rebalance_insert(StringTos64Node *path_top, u8 *target)
{
	StringTos64Node path = *path_top;
	s8 first, second, third;
	if (Balanced(path)) 
		;
	else if (path->longer != (first = (path->taken)) ) {
		/* took the shorter path */
		path->longer = NEITHER;
		path = path->next[first];
	} else if (first == (second = (path->next[first]->taken))) {
		/* just a two-point rotate */
		path = StringTos64Tree_rotate_2(path_top, first);
	} else {
		/* fine details of the 3 point rotate depend on the third step.
		 * However there may not be a third step, if the third point of the
		 * rotation is the newly inserted point.  In that case we record
		 * the third step as NEITHER
		 */
		path = path->next[first]->next[second];
		if (path->taken==NEITHER) {third = NEITHER;}
		else {third = (path->taken);}
		path = StringTos64Tree_rotate_3(path_top, first, third);
	}
	StringTos64Tree_rebalance_path(path, target);
}


static s32
StringTos64Tree_insert(StringTos64Node *treep, u8 *target, u32 len, s64 val)
{
	/* insert the target into the tree, returning 1 on success or 0 if it
	 * already existed
	 */
	StringTos64Node tree = *treep;
	StringTos64Node *path_top = treep;
	s32 res;
	u32 i=0;
	u8 node_len;
	while (tree && ((res=CMP(target, tree->key))!=0) ) {
		s8 next_step = tree->taken = (res>0);
		if (!Balanced(tree)) path_top = treep;
		treep = &tree->next[next_step];
		tree = *treep;
	}
	if (tree){ // tree already exists
		tree->val = val; // update value
		return 0;
	}
	node_len = ((27+len+1)+7)/8*8; // round up to 8 bytes
	tree = malloc(node_len); 
	tree->next[0] = tree->next[1] = NULL;
	tree->val = val;
	tree->longer = NEITHER;
	tree->taken = NEITHER;
	tree->len = node_len;
	do{tree->key[i]=target[i];i++;}while(i<len);
	tree->key[len] = 0; // null terminate
	*treep = tree;
	StringTos64Tree_rebalance_insert(path_top, target);
	return 1;
}

/******************************************************
 * DELETION                                           *
 *****************************************************/

static inline void 
StringTos64Tree_swap_del(StringTos64Node *targetp, StringTos64Node *treep, s8 dir)
{
	StringTos64Node targetn = *targetp;
	StringTos64Node tree = *treep;

	*targetp = tree;
	*treep = tree->next[1-dir];
	tree->next[LEFT] = targetn->next[LEFT];
	tree->next[RIGHT]= targetn->next[RIGHT];
	tree->longer = targetn->longer;

	free(targetn);
}

static inline StringTos64Node *
StringTos64Tree_rebalance_del(StringTos64Node *treep, u8 *target, StringTos64Node *targetp)
{
	/* each StringTos64Node from treep down towards target, but
	 * excluding the last, will have a subtree grow
	 * and need rebalancing
	 */
	StringTos64Node targetn = *targetp;

	while (1) {
		StringTos64Node tree = *treep;
		s8 dir = (tree->taken);

		if (tree->next[dir]==NULL)
			break;

		if (Balanced(tree))
			tree->longer = 1-dir;
		else if (tree->longer == dir)
			tree->longer = NEITHER;
		else {
			int second = tree->next[1-dir]->longer;
			if (second == dir)
				StringTos64Tree_rotate_3(treep, 1-dir, 
					     tree->next[1-dir]->next[dir]->longer);
			else if (second == NEITHER) {
				StringTos64Tree_rotate_2(treep, 1-dir);
				tree->longer = 1-dir;
				(*treep)->longer = dir;
			} else
				StringTos64Tree_rotate_2(treep, 1-dir);
			if (tree == targetn)
				targetp = &(*treep)->next[dir];
		}

		treep = &tree->next[dir];
	}
	return targetp;
}

static s32
StringTos64Tree_delete(StringTos64Node *treep, u8 *target)
{
	/* delete the target from the tree, returning 1 on success or 0 if
	 * it wasn't found
	 */
	StringTos64Node tree = *treep;
	StringTos64Node *path_top = treep;
	StringTos64Node *targetp = NULL;
	s32 res;
	s8 dir;
	
	while (tree) {
		res = CMP(target, tree->key);
		dir = tree->taken = (res>0);
		if (res==0)
			targetp = treep;
		if (tree->next[dir] == NULL)
			break;
		if (Balanced(tree)
		    || (tree->longer == (1-dir) && Balanced(tree->next[1-dir]))
			) path_top = treep;
		treep = &tree->next[dir];
		tree = *treep;
	}
	if (!targetp)
		return 0;

	/* adjust balance, but don't lose 'targetp' */
	targetp = StringTos64Tree_rebalance_del(path_top, target, targetp);

	/* We have re-balanced everything, it remains only to 
	 * swap the end of the path (*treep) with the deleted item
	 * (*targetp)
	 */
	StringTos64Tree_swap_del(targetp, treep, dir);

	return 1;
}
