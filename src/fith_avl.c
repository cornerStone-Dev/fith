
#define INTT


#ifdef STRING
typedef char* value_t;
#define COMP(x, y)  (strcmp((x), (y)))
#define EQUAL(x, y) (strcmp((x), (y)))
#endif


#define COMP_I(x, y)  ((x)-(y))

#define LEFT 0
#define RIGHT 1
#define NEITHER -1
typedef int s32;
typedef struct node_i_s {
   s64        value; //8
   struct node_i_s *next[2]; //24
   s32            longer; //28
} *node_i;

typedef struct node_info_s {
   struct node_i_s *n;
   int comeFrom;
} NodeInfo_i;

typedef struct trav_state_s {
   NodeInfo_i stack[48];
   int idx;
   int state;
} TravState_i;


#define	STARTED 0
#define	ENDED 1
#define	FROM_LEFT 2
#define	FROM_RIGHT 3
#define	FROM_SELF 4


#define Balanced(n) (n->longer < 0)


node_i int_avl_traverse(node_i tree, TravState_i *s)
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

node_i int_avl_find(node_i tree, s64 target)
{
	int result;
	while (tree && (result=(COMP_I(target, tree->value)!=0)) ) {
		s32 next_step = (result>0);
		tree = tree->next[next_step];
	}
	return tree;
}


static node_i int_avl_rotate_2(node_i *path_top, s32 dir)
{
	node_i B, C, D, E;
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


static node_i int_avl_rotate_3(node_i *path_top, s32 dir, s32 third)
{
	node_i B, F, D, C, E;
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
static inline void int_avl_rebalance_path(node_i path, s64 target)
{
	/* Each node_i in path is currently balanced.
	 * Until we find target, mark each node_i as longer
	 * in the s32 of target because we know we have
	 * inserted target there
	 */
	int result;
	while (path && (result=(COMP_I(target, path->value)!=0)) ) {
		s32 next_step = (result>0);
		path->longer = next_step;
		path = path->next[next_step];
	}
}

static inline void int_avl_rebalance_insert(node_i *path_top, s64 target)
{
	node_i path = *path_top;
	s32 first, second, third;
	if (Balanced(path)) 
		;
	else if (path->longer != (first = (COMP_I(target, path->value)>0)) ) {
		/* took the shorter path */
		path->longer = NEITHER;
		path = path->next[first];
	} else if (first == (second = (COMP_I(target, path->next[first]->value)>0) )) {
		/* just a two-point rotate */
		path = int_avl_rotate_2(path_top, first);
	} else {
		/* fine details of the 3 point rotate depend on the third step.
		 * However there may not be a third step, if the third point of the
		 * rotation is the newly inserted point.  In that case we record
		 * the third step as NEITHER
		 */
		path = path->next[first]->next[second];
		if ((COMP_I(target, path->value)==0)) third = NEITHER;
		else third = ((COMP_I(target, path->value)>0));
		path = int_avl_rotate_3(path_top, first, third);
	}
	int_avl_rebalance_path(path, target);
}


int int_avl_insert(node_i *treep, s64 target)
{
	/* insert the target into the tree, returning 1 on success or 0 if it
	 * already existed
	 */
	node_i tree = *treep;
	node_i *path_top = treep;
	while (tree && (COMP_I(target, tree->value)!=0)) {
		s32 next_step = (COMP_I(target, tree->value)>0);
		if (!Balanced(tree)) path_top = treep;
		treep = &tree->next[next_step];
		tree = *treep;
	}
	if (tree) return 0;
	tree = malloc(sizeof(*tree));
	tree->next[0] = tree->next[1] = NULL;
	tree->longer = NEITHER;
	tree->value = target;
	*treep = tree;
	int_avl_rebalance_insert(path_top, target);
	return 1;
}

/******************************************************
 * DELETION                                           *
 *****************************************************/

static inline void int_avl_swap_del(node_i *targetp, node_i *treep, s32 dir)
{
	node_i targetn = *targetp;
	node_i tree = *treep;

	*targetp = tree;
	*treep = tree->next[1-dir];
	tree->next[LEFT] = targetn->next[LEFT];
	tree->next[RIGHT]= targetn->next[RIGHT];
	tree->longer = targetn->longer;

	free(targetn);
}

static inline node_i *int_avl_rebalance_del(node_i *treep, s64 target, node_i *targetp)
{
	/* each node_i from treep down towards target, but
	 * excluding the last, will have a subtree grow
	 * and need rebalancing
	 */
	node_i targetn = *targetp;

	while (1) {
		node_i tree = *treep;
		s32 dir = (COMP_I(target, tree->value)>0);

		if (tree->next[dir]==NULL)
			break;

		if (Balanced(tree))
			tree->longer = 1-dir;
		else if (tree->longer == dir)
			tree->longer = NEITHER;
		else {
			int second = tree->next[1-dir]->longer;
			if (second == dir)
				int_avl_rotate_3(treep, 1-dir, 
					     tree->next[1-dir]->next[dir]->longer);
			else if (second == NEITHER) {
				int_avl_rotate_2(treep, 1-dir);
				tree->longer = 1-dir;
				(*treep)->longer = dir;
			} else
				int_avl_rotate_2(treep, 1-dir);
			if (tree == targetn)
				targetp = &(*treep)->next[dir];
		}

		treep = &tree->next[dir];
	}
	return targetp;
}

int int_avl_delete(node_i *treep, s64 target)
{
	/* delete the target from the tree, returning 1 on success or 0 if
	 * it wasn't found
	 */
	node_i tree = *treep;
	node_i *path_top = treep;
	node_i *targetp = NULL;
	s32 dir;
	
	while (tree) {
		dir = (COMP_I(target, tree->value)>0);
		if ((COMP_I(target, tree->value)==0))
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
	targetp = int_avl_rebalance_del(path_top, target, targetp);

	/* We have re-balanced everything, it remains only to 
	 * swap the end of the path (*treep) with the deleted item
	 * (*targetp)
	 */
	int_avl_swap_del(targetp, treep, dir);

	return 1;
}
