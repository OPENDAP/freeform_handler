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

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id"};

#ifdef _GNUG_
#pragma implementation
#endif

#include <strstream.h>

#include "FFSequence.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;
extern long BufSiz;

int StrLength = 0; // Sets string length befor reading it
extern int StrLens[MaxStr]; // List of string length in this sequence 

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

long
Records(const String &filename)
{
    int error = 0;
    DATA_BIN_PTR dbin = NULL;
    FF_STD_ARGS_PTR SetUps = NULL;  
    PROCESS_INFO_LIST pinfo_list = NULL;
    PROCESS_INFO_PTR  pinfo = NULL;
    static char Msgt[255];

    char * FileName = new char [filename.length()+1];
    (void) strcpy(FileName, (const char *)filename);

    SetUps = ff_create_std_args();
    if (!SetUps)
	return -1;
    
    /** set the structure values to create the FreeForm DB**/
    SetUps->user.is_stdin_redirected = 0;
    SetUps->input_file = FileName; 
    String iff = find_ancillary_file(filename);
    char *if_f = new char[iff.length() + 1];
    strcpy(if_f, iff);
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

bool 
FFSequence::read(const String &dataset, int &error)
{

  int StrCnt = 0;

    if (read_p())  // Nothing to do
      return false;

    if((BufPtr >= BufSiz) && (BufSiz != 0))
      return false; // End of sequence

    if (!BufVal) { // Make the cache
	char *ds = new char[dataset.length() + 1];
	strcpy(ds, (const char*)dataset);
   
	// Create the output Sequence format
	ostrstream str;
	int endbyte = 0;
	int stbyte = 1;

	str << "binary_output_data \"DODS binary output data\"" << endl;
	StrCnt = 0;
	for(Pix p = first_var(); p; next_var(p)) {
	  if (var(p)->synthesized_p())
	      continue;
	  if (var(p)->type() == dods_str_c) {
	    endbyte +=StrLens[StrCnt];
	    StrCnt++;
	  }
	  else
	    endbyte += var(p)->width();
	  
	  str << var(p)->name() << " " << stbyte << " " << endbyte 
	      << " " << ff_types(var(p)->type_name()) 
	      << " " << ff_prec(var(p)->type_name()) << endl;
	  stbyte = endbyte + 1;
	}

#ifdef TEST 
	cout << str.str();
#endif 
      
	String output_format = str.str();
      
	char *o_fmt = new char[output_format.length() + 1];
	strcpy(o_fmt, (const char*)output_format);

	String input_format_file = find_ancillary_file(dataset);
	char *if_fmt = new char[input_format_file.length() + 1];
	strcpy(if_fmt, input_format_file);
     
	long num_rec = Records(dataset); // It could come from DDS if sequence length was known

	if (num_rec == -1)
	    return false;

	BufSiz = num_rec * (stbyte - 1);

	BufVal = (char *)new char[BufSiz];
	long bytes = read_ff(ds, if_fmt, o_fmt, BufVal, BufSiz);
	if (bytes == -1) {
	    error = 1;
	    return false;
	}
	    
	// clean up
	delete[] ds;	       
	delete[] o_fmt;
	delete[] if_fmt;
    }
    StrCnt = 0;
    for (Pix p = first_var(); p; next_var(p)) {
      if(var(p)->type()== dods_str_c){
	StrLength = StrLens[StrCnt];
	StrCnt++;
      }
         var(p)->read(dataset, error);
	 if (error)
	     return false;
    }

    return true;
}
