#include <freeform.h>		/* AVLFREE.C */
#include <avltree.h>

static void fa(HEADER * root)
{
    /* Delete the entire tree pointed to by root. Note that unlike
     * tfree(), this routine is passed a pointer to a HEADER rather
     * than to the memory just below the header.
     */

    if (root) {
	fa(root->left);
	fa(root->right);
	memFree(root, "In fa avlfree.c");

    }
}

/*----------------------------------------------------------------------*/

void freeall(HEADER ** root)
{
    fa(*root);
    *root = NULL;
}
