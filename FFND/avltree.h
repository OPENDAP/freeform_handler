
/* TREE.H */

#ifndef AVLTREE_H__
#define AVLTREE_H__

typedef int *TREE;		/* Dummy typedef for a tree. */

typedef struct _leaf {
    struct _leaf *left;
    struct _leaf *right;
    unsigned size:14;
    unsigned bal:2;
} HEADER;

			/* Possible values of bal field. Can be */
			/* any three consecutive numbers but    */
			/* L < B < R must hold.                 */
#define L	 0		/*        Left  subtree is larger         */
#define	B	 1		/*        Balanced subtree                */
#define R	 2		/*        Right subtree is larger         */

HEADER *insert(HEADER **, HEADER *, int (*)(void *, void *));

void tprint(TREE *, void (*)(), FILE *);

HEADER *talloc(int);
void freeall(HEADER **);

#endif				/* AVLTREE_H__ */
