
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFInt32.cc,v $
// Revision 1.3  1998/04/21 17:13:53  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:10  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFInt32.cc,v 1.3 1998/04/21 17:13:53 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "FFInt32.h"
#include "util_ff.h"

Int32 *
NewInt32(const String &n)
{
    return new FFInt32(n);
}

FFInt32::FFInt32(const String &n) : Int32(n)
{
}

BaseType *
FFInt32::ptr_duplicate(){

    return new FFInt32(*this);
}

bool
FFInt32::read(const String &dataset, int &)
{
    if (read_p()) // nothing to do
        return true;

    bool status = true;

    char *ds = new char[dataset.length() + 1];
    strcpy(ds, dataset);

    String o_format = make_output_format(name(), type_name(), width());
    char *o_f = new char[o_format.length() + 1];
    strcpy(o_f, o_format);

    String i_format_file = find_ancillary_file(dataset);
    char *if_f = new char[i_format_file.length() + 1];
    strcpy(if_f, i_format_file);

    dods_int32 *i = new dods_int32[width() + 1];
    long bytes = read_ff(ds, if_f, o_f, (char *)i, width()+1);

    if (bytes == -1){
        status = false;
    }
    else {
      set_read_p(true);
      val2buf(i);     
    }

    if (i)
        delete(i);

    delete [] ds;
    delete [] o_f;
    delete [] if_f;

    return status;

}


