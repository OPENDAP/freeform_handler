
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/16/97

// $Log: FFSequence.cc,v $
// Revision 1.3  1998/04/21 17:13:57  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:14  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id"};

#ifdef _GNUG_
#pragma implementation
#endif

#include <strstream.h>

#include "FFSequence.h"
#include "util_ff.h"

Sequence *
NewSequence(const String &n)
{
    return new FFSequence(n);
}

// protected

BaseType *
FFSequence::ptr_duplicate()
{
    return new FFSequence(*this);
}

// public

FFSequence::FFSequence(const String &n) : Sequence(n)
{
}

FFSequence::~FFSequence()
{
}

bool 
FFSequence::read(const String &dataset, int &error)
{
    if (read_p())		// Nothing to do
        return true;

    bool status = true;

    // make char * variables to hold String data for read_ff

    char *ds = new char[dataset.length() + 1];
    strcpy(ds, dataset);

    // Create the output Sequence format
    ostrstream str;
    int endbyte = 0;
    int stbyte = 1;

    str << "binary_output_data \"DODS binary output data\"" << endl;
    for(Pix p = first_var(); p; next_var(p)) {
	endbyte += var(p)->width();
	str << var(p)->name() << " " << stbyte << " " << endbyte 
	    << " " << ff_types(var(p)->type_name()) 
	    << " " << ff_prec(var(p)->type_name()) << endl;
	stbyte = endbyte + 1;
    }

    String output_format = str.str();
    char *o_fmt = new char[output_format.length() + 1];
    strcpy(o_fmt, output_format);

    String input_format_file = find_ancillary_file(dataset);
    char *if_fmt = new char[input_format_file.length() + 1];
    strcpy(if_fmt, input_format_file);

    dods_byte *b = (dods_byte *)new char[stbyte]; // `+1' hack for FreeForm
    long bytes = read_ff(ds, if_fmt, o_fmt, (char *)b, stbyte);

    if (bytes == -1)
	status = false;
    else { 
	set_read_p(true);
	val2buf((void *) b); //I think this part is different for sequence?
    }

    if (b)
	delete(b);

    // clean up
    delete[] ds;		// delete temporary char * arrays
    delete[] o_fmt;
    delete[] if_fmt;

    return status;
}
