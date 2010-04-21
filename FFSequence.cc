// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of ff_handler a FreeForm API handler for the OPeNDAP
// DAP2 data server.

// Copyright (c) 2005 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

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

static char rcsid[]not_used = {
        "$Id$" };

#include <sstream>

using std::endl;
using std::ostringstream;

#include "Error.h"
#include "debug.h"

#include "FFSequence.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;
extern long BufSiz;

int StrLength = 0; // Sets string length befor reading it
int StrLens[MaxStr]; // List of string length in this sequence

// protected

BaseType *
FFSequence::ptr_duplicate()
{
    return new FFSequence(*this);
}

// public

FFSequence::FFSequence(const string &n, const string &d, const string &iff) :
    d_input_format_file(iff), Sequence(n, d)
{
}

FFSequence::~FFSequence()
{
}

static long Records(const string &filename)
{
    int error = 0;
    DATA_BIN_PTR dbin = NULL;
    FF_STD_ARGS_PTR SetUps = NULL;
    PROCESS_INFO_LIST pinfo_list = NULL;
    PROCESS_INFO_PTR pinfo = NULL;
    static char Msgt[255];

    char * FileName = new char[filename.length() + 1];
    (void)filename.copy(FileName, filename.length());
    FileName[filename.length()] = '\0';

    SetUps = ff_create_std_args();
    if (!SetUps) {
    	delete[] FileName;
        return -1;
    }

    /** set the structure values to create the FreeForm DB**/
    SetUps->user.is_stdin_redirected = 0;
    SetUps->input_file = FileName; 

#if 0
    // See my comments in ffdds.cc 10/30/08 jhrg
#ifdef RSS 
    string iff = find_ancillary_rss_formats(filename);
#else
    string iff = find_ancillary_formats(filename);
#endif
    char *if_f = new char[iff.length() + 1];
    iff.copy(if_f, iff.length());
    if_f[iff.length()]='\0';

    SetUps->input_format_file = if_f;
#endif

    SetUps->output_file = NULL;

    error = SetDodsDB(SetUps, &dbin, Msgt);
    if (error && error < ERR_WARNING_ONLY) {
        db_destroy(dbin);
        return -1;
    }

    error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA,
            &pinfo_list);
    if (error)
        return (-1);

    pinfo_list = dll_first(pinfo_list);

    //    pinfo = FF_PI(pinfo_list);
    pinfo = ((PROCESS_INFO_PTR) (pinfo_list)->data.u.pi);

    long num_records = PINFO_SUPER_ARRAY_ELS(pinfo);
    ff_destroy_process_info_list(pinfo_list);

    return num_records;
}

/** Read a row from the Sequence.

 @note Does not use either the \e in_selection or \e send_p properties. If
 this method is called and the \e read_p property is not true, the values
 are read.

 @exception Error if the size of the returned data is zero.
 @return Always returns false. */
bool FFSequence::read()
{
    int StrCnt = 0;

    if (read_p()) // Nothing to do
        return false;

    if ((BufPtr >= BufSiz) && (BufSiz != 0))
        return false; // End of sequence

    if (!BufVal) { // Make the cache (BufVal is global)
        // Create the output Sequence format
        ostringstream str;
        int endbyte = 0;
        int stbyte = 1;
        string ds_str = dataset();

        str << "binary_output_data \"DODS binary output data\"" << endl;
        StrCnt = 0;
        for (Vars_iter p = var_begin(); p != var_end(); ++p) {
            if ((*p)->synthesized_p())
                continue;
            if ((*p)->type() == dods_str_c) {
                endbyte += StrLens[StrCnt];
                StrCnt++;
            }
            else
                endbyte += (*p)->width();

            str << (*p)->name() << " " << stbyte << " " << endbyte << " "
                    << ff_types((*p)->type()) << " " << ff_prec((*p)->type())
                    << endl;
            stbyte = endbyte + 1;
        }

        DBG(cerr << str.str());

        char *o_fmt = new char[str.str().length() + 1];
        (void)str.str().copy(o_fmt, str.str().length());
        o_fmt[str.str().length()] = '\0';

#if 0
	// See my comments in ffdds.cc 10/30/08 jhrg
#ifdef RSS
	string input_format_file = find_ancillary_rss_formats(ds_str);
#else
	string input_format_file = find_ancillary_formats(ds_str);
#endif
	char *if_fmt = new char[input_format_file.length() + 1];
        input_format_file.copy(if_fmt, input_format_file.length());
        if_fmt[input_format_file.length()]='\0';
#endif

	// num_rec could come from DDS if sequence length was known...
	long num_rec = Records(ds_str); 
        if (num_rec == -1) {
            delete[] o_fmt;
            return false;
        }

        BufSiz = num_rec * (stbyte - 1);
        BufVal = new char[BufSiz];
        char *ds = new char[ds_str.length() + 1];
        (void)ds_str.copy(ds, ds_str.length());
        ds[ds_str.length()] = '\0';

        long bytes = read_ff(ds, d_input_format_file.c_str(), o_fmt, BufVal,
                BufSiz);

        // clean up; we should use auto_ptr, but it doesn't work for
        // arrays... 08/29/03 jhrg
        delete[] ds;
        delete[] o_fmt;

        if (bytes == -1)
            throw Error("Could not read requested data from the dataset.");
    }

    StrCnt = 0;
    for (Vars_iter p = var_begin(); p != var_end(); ++p) {
        if ((*p)->type() == dods_str_c) {
            StrLength = StrLens[StrCnt];
            StrCnt++;
        }
        (*p)->read();
    }

    return true;
}

void FFSequence::transfer_attributes(AttrTable *at)
{
    if (at) {
	Vars_iter var = var_begin();
	while (var != var_end()) {
	    (*var)->transfer_attributes(at);
	    var++;
	}
    }
}

