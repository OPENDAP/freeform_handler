
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/16/97

#include "config_ff.h"

static char rcsid[] not_used = {"$Id$"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <sstream>

using std::endl;
using std::ostringstream;

#include "Error.h"
#include "FFSequence.h"
#include "util_ff.h"
#include "debug.h"

extern long BufPtr;
extern char *BufVal;
extern long BufSiz;

int StrLength = 0; // Sets string length befor reading it
int StrLens[MaxStr]; // List of string length in this sequence 

#if 0
Sequence *
NewSequence(const string &n)
{
    return new FFSequence(n);
}
#endif

// protected

BaseType *
FFSequence::ptr_duplicate()
{
    return new FFSequence(*this);
}

// public

FFSequence::FFSequence(const string &n) : Sequence(n)
{
}

FFSequence::~FFSequence()
{
}

static long
Records(const string &filename)
{
    int error = 0;
    DATA_BIN_PTR dbin = NULL;
    FF_STD_ARGS_PTR SetUps = NULL;  
    PROCESS_INFO_LIST pinfo_list = NULL;
    PROCESS_INFO_PTR  pinfo = NULL;
    static char Msgt[255];

    char * FileName = new char [filename.length()+1];
    (void) strcpy(FileName, filename.c_str());

    SetUps = ff_create_std_args();
    if (!SetUps)
	return -1;
    
    /** set the structure values to create the FreeForm DB**/
    SetUps->user.is_stdin_redirected = 0;
    SetUps->input_file = FileName; 
    string iff = find_ancillary_file(filename);
    char *if_f = new char[iff.length() + 1];
    strcpy(if_f, iff.c_str());
    SetUps->input_format_file = if_f;
    SetUps->output_file = NULL;

    error = SetDodsDB(SetUps, &dbin, Msgt);
    if (error && error < ERR_WARNING_ONLY) {
	db_destroy(dbin);
	return -1;
    }

    error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, &pinfo_list);
    if (error)
	return(-1);

    pinfo_list = dll_first(pinfo_list);
 
    //    pinfo = FF_PI(pinfo_list);
    pinfo = ((PROCESS_INFO_PTR)(pinfo_list)->data.u.pi);

    long num_records = PINFO_SUPER_ARRAY_ELS(pinfo);
    ff_destroy_process_info_list(pinfo_list);

    return num_records;
}

/** Read a row from the Sequence.

    @note Does not use either the \e in_selection or \e send_p properties. If
    this method is called and the \e read_p property is not true, the values
    are read.
    
    @exception Error if the size of the returned data is zero.
    @param dataset The name of the data file. Must have a matching FreeForm
    format file.
    @return Always returns false. */
bool 
FFSequence::read(const string &dataset)
{
    int StrCnt = 0;

    if (read_p())		// Nothing to do
	return false;

    if ((BufPtr >= BufSiz) && (BufSiz != 0))
	return false;		// End of sequence

    if (!BufVal) {		// Make the cache (BufVal is global)
	// Create the output Sequence format
	ostringstream str;
	int endbyte = 0;
	int stbyte = 1;

	str << "binary_output_data \"DODS binary output data\"" << endl;
	StrCnt = 0;
	for(Vars_iter p = var_begin(); p != var_end(); ++p) {
	    if ((*p)->synthesized_p())
		continue;
	    if ((*p)->type() == dods_str_c) {
		endbyte +=StrLens[StrCnt];
		StrCnt++;
	    }
	    else
		endbyte += (*p)->width();
	  
	    str << (*p)->name() << " " << stbyte << " " << endbyte 
		<< " " << ff_types((*p)->type()) 
		<< " " << ff_prec((*p)->type()) << endl;
	    stbyte = endbyte + 1;
	}

	DBG(cerr << str.str());
      
	char *o_fmt = new char[str.str().length() + 1];
	strcpy(o_fmt, str.str().c_str());

	string input_format_file = find_ancillary_file(dataset);
	char *if_fmt = new char[input_format_file.length() + 1];
	strcpy(if_fmt, input_format_file.c_str());
     
	// num_rec could come from DDS if sequence length was known...
	long num_rec = Records(dataset); 

	if (num_rec == -1)
	    return false;

	BufSiz = num_rec * (stbyte - 1);
	BufVal = new char[BufSiz];
	char *ds = new char[dataset.length() + 1];
	strcpy(ds, dataset.c_str());
   
	long bytes = read_ff(ds, if_fmt, o_fmt, BufVal, BufSiz);
	    
	// clean up; we should use auto_ptr, but it doesn't work for
	// arrays... 08/29/03 jhrg
	delete[] ds;	       
	delete[] o_fmt;
	delete[] if_fmt;

	if (bytes == -1)
	    throw Error("Could not read requested data from the dataset.");
    }

    StrCnt = 0;
    for(Vars_iter p = var_begin(); p != var_end(); ++p) {
	if((*p)->type()== dods_str_c){
	    StrLength = StrLens[StrCnt];
	    StrCnt++;
	}
	(*p)->read(dataset);
    }

    return true;
}

// $Log: FFSequence.cc,v $
// Revision 1.17  2004/02/04 20:50:08  jimg
// Build fixes. No longer uses Pix.
//
// Revision 1.16  2003/12/08 21:55:52  edavis
// Merge release-3-4 into trunk
//
// Revision 1.14.4.2  2003/09/06 23:33:14  jimg
// I modified the read() method implementations so that they test the new
// in_selection property. If it is true, the methods will read values
// even if the send_p property is not true. This is so that variables used
// in the selection part of the CE, or as function arguments, will be read.
// See bug 657.
//
// Revision 1.14.4.1  2003/06/29 05:30:36  rmorris
// Proper Use of template library headers and missing usage statements.
//
// Revision 1.15  2003/05/14 19:28:02  jimg
// Replaced strstream with sstream
//
// Revision 1.14  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.13.2.1  2002/12/18 23:30:42  pwest
// gcc3.2 compile corrections, mainly regarding the using statement
//
// Revision 1.13  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.12  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.11  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.10.8.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.10  1998/11/13 05:39:15  jimg
// Minor changes, formatting sanity.
// Added ends to ostrstream object using to builf output format string.
// Added debug.h
//
// Revision 1.9  1998/11/10 19:22:15  jimg
// Added calls to the new BaseType accessor synthesized_p(). This means that
// the projection functions now work.
//
// Revision 1.8  1998/08/31 04:06:03  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.7  1998/08/18 16:58:23  reza
// Files with headers are now handled correctly
//
// Revision 1.6  1998/08/14 18:20:14  reza
// Fixed extra records read for sequences.
//
// Revision 1.5  1998/08/13 20:24:32  jimg
// Fixed read mfunc semantics
//
// Revision 1.4  1998/08/12 21:20:56  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:57  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:14  jimg
// Sequence support added by Reza
