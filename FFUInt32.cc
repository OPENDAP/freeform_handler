
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      ReZa       Reza Nekovei (reza@intcomm.net)

// $Log: FFUInt32.cc,v $
// Revision 1.3  1998/04/21 17:14:04  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:20  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFUInt32.cc,v 1.3 1998/04/21 17:14:04 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "FFUInt32.h"
#include "util_ff.h"

UInt32 *
NewUInt32(const String &n)
{
    return new FFUInt32(n);
}

FFUInt32::FFUInt32(const String &n) : UInt32(n)
{
}

BaseType *
FFUInt32::ptr_duplicate(){

    return new FFUInt32(*this);
}

bool
FFUInt32::read(const String &dataset, int &)
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

    dods_uint32 *i = new dods_uint32[width() + 1];
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


