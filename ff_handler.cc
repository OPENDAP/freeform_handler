
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of ff_handler a FreeForm API handler for the OPeNDAP
// DAP2 data server.

// Copyright (c) 2002,2003,2005 OPeNDAP, Inc.
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

#include "config_ff.h"

static char not_used rcsid[]={"$Id$"};

#include <string>

#include "DODSFilter.h"
#include "DDS.h"
#include "DAS.h"
#include "DataDDS.h"
#include "ConstraintEvaluator.h"

#include "FFTypeFactory.h"
#include "ObjectType.h"
#include "cgi_util.h"
#include "ff_ce_functions.h"

long BufPtr = 0; // cache pointer
long BufSiz =0; // Cache size
char *BufVal = NULL; // cache buffer
const string cgi_version = PACKAGE_VERSION;

extern void ff_read_descriptors(DDS &dds, const string &filename) throw(Error);
extern void ff_get_attributes(DAS &das, string filename) throw(Error);

int 
main(int argc, char *argv[])
{
    try { 
	DODSFilter df(argc, argv);

	switch (df.get_response()) {
	  case DODSFilter::DAS_Response: {
	    DAS das;

	    ff_get_attributes(das, df.get_dataset_name());
	    df.read_ancillary_das(das);
	    df.send_das(das);
	    break;
	  }

	  case DODSFilter::DDS_Response: {
	    FFTypeFactory ff_factory;
	    DDS dds(&ff_factory);
            ConstraintEvaluator ce;

	    ff_read_descriptors(dds, df.get_dataset_name());
	    df.read_ancillary_dds(dds);
	    df.send_dds(dds, ce, true);
	    break;
	  }

	  case DODSFilter::DataDDS_Response: {
            FFTypeFactory ff_factory;
	    DDS dds(&ff_factory);
            ConstraintEvaluator ce;

	    register_functions(ce);
	    dds.filename(df.get_dataset_name());
	    ff_read_descriptors(dds, df.get_dataset_name()); 
	    df.read_ancillary_dds(dds);
	    df.send_data(dds, ce, stdout);
	    break;
	  }

          case DODSFilter::DDX_Response: {
            FFTypeFactory ff_factory;
            DDS dds(&ff_factory);
            DAS das;
            ConstraintEvaluator ce;

            register_functions(ce);
            dds.filename(df.get_dataset_name());
            ff_read_descriptors(dds, df.get_dataset_name()); 

            ff_get_attributes(das, df.get_dataset_name());
            df.read_ancillary_das(das);

            dds.transfer_attributes(&das);

            df.send_ddx(dds, ce, stdout);
            break;
          }

	  case DODSFilter::Version_Response: {
	    if (df.get_cgi_version() == "")
		df.set_cgi_version(cgi_version);
	    df.send_version_info();

	    break;
	  }

	  default:
	    df.print_usage();	// Throws Error
	}
    }
    catch (Error &e) {
	set_mime_text(stdout, dods_error, cgi_version);
	e.print(stdout);
	return 1;
    }

    return 0;
}

// $Log: ff_handler.cc,v $
// Revision 1.1  2003/05/14 19:26:44  jimg
// Added
//
