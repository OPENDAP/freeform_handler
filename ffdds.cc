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

// This file contains functions which read the variables and their description
// from a freeform API and build the in-memeory DDS. These functions form the
// core of the server-side software necessary to extract the DDS from a
// FreeForm data file.
//
// It also contains test code which will print the in-memeory DDS to
// stdout.
//
// ReZa 6/20/97

#include "config_ff.h"

static char rcsid[]not_used = {
        "$Id$" };

#include <cstdio>
#include <cstring>

#include <iostream>
#include <string>

#include <Error.h>
#include <InternalErr.h>
#include <cgi_util.h>
#include <util.h>

#include "FreeFormCPP.h"

#include <DDS.h>

#include "FFInt16.h"
#include "FFUInt16.h"
#include "FFInt32.h"
#include "FFUInt32.h"
#include "FFFloat32.h"
#include "FFFloat64.h"
#include "FFByte.h"
#include "FFArray.h"
#include "FFSequence.h"
#include "FFGrid.h"
#include "FFStr.h"
#include "util_ff.h"

extern int StrLens[MaxStr]; // List of string lengths

void ff_read_descriptors(DDS &dds_table, const string &filename)
{
    int error = 0;
    int i = 0;
    int num_names = 0;
    int StrNum = 0;
    char **var_names_vector = NULL;
    DATA_BIN_PTR dbin = NULL;
    FF_STD_ARGS_PTR SetUps = NULL;
    VARIABLE_PTR var = NULL;
    FORMAT_PTR iformat = NULL;
    PROCESS_INFO_LIST pinfo_list = NULL;
    PROCESS_INFO_PTR pinfo = NULL;
    bool newseq = true;
    bool is_array = true;
    Array *ar = NULL;
    Sequence *seq = NULL;

    if (!file_exist(filename.c_str()))
        throw Error((string) "Could not open file " + path_to_filename(
                filename) + string("."));

    // Set dataset name
    dds_table.set_dataset_name(name_path(filename));

    SetUps = ff_create_std_args();
    if (!SetUps) {
        string msg = (string) "Insufficient memory";
        throw Error(msg);
    }

    // Set the structure values to create the FreeForm DB
    SetUps->user.is_stdin_redirected = 0;

    SetUps->input_file = new char[filename.length() + 1];
    filename.copy(SetUps->input_file, filename.length());
    SetUps->input_file[filename.length()] = '\0';

    // Setting the input format file here causes db_set (called by SetDodsDB)
    // to not set that field of SetUps. In the original modification for the
    // RSS-hosted data, the server also called this code in FFArray.cc and
    // FFSequence. However, in a later version of the handler I moved the 
    // format finding code here and recorded the results in the various 
    // objects (including FFArray, ...). So I think the RSS format-specific
    // code in those classes is not needed anymore. I'm going to #if 0 #endif
    // them out and check in the result. 10/30/08 jhrg 
#ifdef RSS
    string iff = find_ancillary_rss_formats(filename);
    SetUps->input_format_file = new char[iff.length() + 1];
#if 0
    // Huh??
    iff.copy(SetUps->input_format_file, iff.length);
    SetUps->input_format_file[iff.length] = '\0';
#endif
    strcpy(SetUps->input_format_file, iff.c_str()); // strcpy needs the /0
#endif

    SetUps->output_file = NULL;

    char Msgt[Msgt_size];
    error = SetDodsDB(SetUps, &dbin, Msgt);
    if (error && error < ERR_WARNING_ONLY) {
        db_destroy(dbin);
        string msg = (string) Msgt + " FreeForm error code: ";
        append_long_to_string((long) error, 10, msg);
        throw Error(msg);
    }

    error = db_ask(dbin, DBASK_VAR_NAMES, FFF_INPUT | FFF_DATA, &num_names,
            &var_names_vector);
    if (error) {
        string msg =
                (string) "Could not get varible list from the input file."
                        + " FreeForm error code: ";
        append_long_to_string((long) error, 10, msg);
        throw Error(msg);
    }

    error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA,
            &pinfo_list);
    if (error) {
        string msg =
                (string) "Could not get process info for the input file."
                        + " FreeForm error code: ";
        append_long_to_string((long) error, 10, msg);
        throw Error(msg);
    }

    for (i = 0; i < num_names; i++) {
        int num_dim_names = 0;
        char **dim_names_vector = NULL;
        char *cp = NULL;
        FF_ARRAY_DIM_INFO_PTR array_dim_info = NULL;
        int j = 0;

        error = db_ask(dbin, DBASK_ARRAY_DIM_NAMES, var_names_vector[i],
                &num_dim_names, &dim_names_vector);
        if (error) {
            string msg = "Could not get array dimension names for variable: ";
            msg += (string) var_names_vector[i] + ", FreeForm error code: ";
            append_long_to_string((long) error, 10, msg);
            throw Error(msg);
        }

        var = NULL;

        if (num_dim_names == 0) // sequence names
            cp = var_names_vector[i];
        else {
            cp = strstr(var_names_vector[i], "::");
            // If cp is not null, advance past the "::"
            if (cp)
                cp += 2;
        }

        pinfo_list = dll_first(pinfo_list);
        // pinfo = FF_PI(pinfo_list);
        pinfo = ((PROCESS_INFO_PTR) (pinfo_list)->data.u.pi);
        iformat = PINFO_FORMAT(pinfo);

        var = ff_find_variable(cp, iformat);

        // For some formats: Freefrom sends an extra EOL variable at the end of
        // the list.
        if (IS_EOL(var))
            break;

        while (!var) { // search formats in the format list for the variable
            pinfo_list = (pinfo_list)->next;
            pinfo = ((PROCESS_INFO_PTR) (pinfo_list)->data.u.pi);

            if (!pinfo) {
                string msg = "Variable ";
                msg += (string) cp + " was not found in the format file.";
                throw Error(msg);
            }

            iformat = PINFO_FORMAT(pinfo);
            var = ff_find_variable(cp, iformat);
        }

        string input_format_file = PINFO_ORIGIN(pinfo);

        BaseType *bt = NULL;
        switch (FFV_DATA_TYPE(var)) {
            case FFV_TEXT:
                if (StrNum > MaxStr - 1)
                    throw InternalErr(__FILE__, __LINE__,
                            "String variable length.");
                StrLens[StrNum] = var->end_pos - var->start_pos + 1;
                StrNum++;
                bt = new FFStr(cp, filename);
                break;

            case FFV_INT8:
                bt = new FFByte(cp, filename);
                break;

            case FFV_UINT8:
                bt = new FFByte(cp, filename); // Byte is unsigned.
                break;

            case FFV_INT16:
                bt = new FFInt16(cp, filename);
                break;

            case FFV_UINT16:
                bt = new FFUInt16(cp, filename);
                break;

            case FFV_INT32:
                bt = new FFInt32(cp, filename);
                break;

            case FFV_UINT32:
                bt = new FFUInt32(cp, filename);
                break;

            case FFV_INT64:
                bt = new FFInt32(cp, filename); // Ouch!
                break;

            case FFV_UINT64:
                bt = new FFUInt32(cp, filename);
                break;

            case FFV_FLOAT32:
                bt = new FFFloat32(cp, filename);
                break;

            case FFV_FLOAT64:
                bt = new FFFloat64(cp, filename);
                break;

            case FFV_ENOTE:
                bt = new FFFloat64(cp, filename);
                break;

            default:
                throw InternalErr(__FILE__, __LINE__,
                        "Unknown FreeForm type!");
        }

        if (num_dim_names == 0) {
            if (newseq) {
                newseq = false;
                // The format name cannot contain spaces! 8/12/98 jhrg
                seq
                        = new FFSequence(iformat->name, filename, input_format_file);
            }
            seq->add_var(bt);
            is_array = false;
        }
        else {
            ar = new FFArray(cp, filename, bt, input_format_file);
            newseq = true; // An array terminates the old sequence
            is_array = true;
        }

        for (j = 0; j < num_dim_names; j++) {
            error = db_ask(dbin, DBASK_ARRAY_DIM_INFO, var_names_vector[i],
                    dim_names_vector[j], &array_dim_info);
            if (error) {
                string msg =
                        "Could not get array dimension info for variable ";
                msg += (string) var_names_vector[i]
                        + ", FreeForm error code: ";
                append_long_to_string((long) error, 10, msg);
                throw Error(msg);
            }

            int DimSiz = (array_dim_info->end_index
                    - array_dim_info->start_index + 1)
                    / array_dim_info->granularity;
            ar->append_dim(DimSiz, (string) dim_names_vector[j]);

#ifdef TEST
            printf("Array %s, dimension %s:\n", var_names_vector[i],
                    dim_names_vector[j]);
            printf("Start index is %ld\n", array_dim_info->start_index);
            printf("End index is %ld\n", array_dim_info->end_index);
            printf("Granularity is %ld\n", array_dim_info->granularity);
            printf("Separation is %ld\n", array_dim_info->separation);
            printf("Grouping is %ld\n\n", array_dim_info->grouping);
#endif
            memFree(array_dim_info, "");
            array_dim_info = NULL;
        }

        memFree(dim_names_vector, "");
        dim_names_vector = NULL;

        if (is_array)
            dds_table.add_var(ar);
        else if (newseq)
            dds_table.add_var(seq);
    } // End of the for num_names.

    if (!is_array)
        dds_table.add_var(seq);

    memFree(var_names_vector, "");
    var_names_vector = NULL;
}

#ifdef TEST
int
main(int argc, char *argv[])
{
    DDS dds;
    string err;

    try {
        ff_read_descriptors(dds, (string)argv[1]);
        dds.print();
    }
    catch (Error &e) {
        e.display_message();
        return 1;
    }

    return 0;
}
#endif

