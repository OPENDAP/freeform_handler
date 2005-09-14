
#ifndef config_ff_h_
#define config_ff_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* GNU gcc/g++ provides a way to mark variables, etc. as unused */

#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif

#endif /* config_ff_h_ */
