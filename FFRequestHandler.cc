
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of ff_handler, a data handler for the OPeNDAP data
// server. 

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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

// FFRequestHandler.cc

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "FFRequestHandler.h"

#include "config_ff.h"
#include "FFTypeFactory.h"
#include "ff_ce_functions.h"
#include "util_ff.h"

#include "BESDASResponse.h"
#include "BESDDSResponse.h"
#include "BESDataDDSResponse.h"
#include "BESInfo.h"
#include "BESResponseNames.h"
#include "BESContainer.h"
#include "BESResponseHandler.h"
#include "BESVersionInfo.h"
#include "BESDataNames.h"

#include "BESDapError.h"
#include "BESInternalFatalError.h"
#include "InternalErr.h"

#include <DDS.h>
#include <Ancillary.h>
#include <Error.h>
#include <escaping.h>
#include <cgi_util.h>

long BufPtr = 0;                // cache pointer
long BufSiz = 0;                // Cache size
char *BufVal = NULL;            // cache buffer

extern void ff_read_descriptors(DDS & dds, const string & filename);
extern void ff_get_attributes(DAS & das, string filename);

FFRequestHandler::FFRequestHandler(const string &name)
:  BESRequestHandler(name)
{
    add_handler(DAS_RESPONSE, FFRequestHandler::ff_build_das);
    add_handler(DDS_RESPONSE, FFRequestHandler::ff_build_dds);
    add_handler(DATA_RESPONSE, FFRequestHandler::ff_build_data);
    add_handler(HELP_RESPONSE, FFRequestHandler::ff_build_help);
    add_handler(VERS_RESPONSE, FFRequestHandler::ff_build_version);
}

FFRequestHandler::~FFRequestHandler()
{
}

bool FFRequestHandler::ff_build_das(BESDataHandlerInterface & dhi)
{
    BESDASResponse *bdas =
        dynamic_cast <
        BESDASResponse * >(dhi.response_handler->get_response_object());
    DAS *das = bdas->get_das();

    try {
	string accessed = dhi.container->access() ;
        ff_get_attributes(*das, accessed);
#ifdef RSS
	string name = find_ancillary_rss_das(accessed);
#else
        string name = Ancillary::find_ancillary_file(accessed, "das", "", "");
#endif
        if (!name.empty())
            das->parse(name);
    }
    catch(InternalErr & e) {
        BESDapError ex( e.get_error_message(), true, e.get_error_code(),
	                __FILE__, __LINE__ ) ;
        throw ex;
    }
    catch(Error & e) {
        BESDapError ex( e.get_error_message(), false, e.get_error_code(),
	                __FILE__, __LINE__ ) ;
        throw ex;
    }
    catch(...) {
        string s = "unknown exception caught building Freeform DAS";
        BESInternalFatalError ex(s, __FILE__, __LINE__);
        throw ex;
    }

    return true;
}

bool FFRequestHandler::ff_build_dds(BESDataHandlerInterface & dhi)
{
    BESDDSResponse *bdds =
        dynamic_cast <
        BESDDSResponse * >(dhi.response_handler->get_response_object());
    DDS *dds = bdds->get_dds();
    ConstraintEvaluator & ce = bdds->get_ce();

    try {
        FFTypeFactory *factory = new FFTypeFactory;
        dds->set_factory(factory);

        ff_register_functions(ce);
	string accessed = dhi.container->access();
        dds->filename(accessed);
        ff_read_descriptors(*dds, accessed);

        DAS das;
        ff_get_attributes(das, accessed);
        string name = Ancillary::find_ancillary_file(accessed, "das", "", "");
        if (!name.empty())
            das.parse(name);
	Ancillary::read_ancillary_das( das, accessed ) ;
        
        dds->transfer_attributes(&das);

        dhi.data[POST_CONSTRAINT] = dhi.container->get_constraint();
    }
    catch(InternalErr & e) {
        BESDapError ex( e.get_error_message(), true, e.get_error_code(),
			__FILE__, __LINE__ ) ;
        throw ex;
    }
    catch(Error & e) {
        BESDapError ex( e.get_error_message(), false, e.get_error_code(),
			__FILE__, __LINE__ ) ;
        throw ex;
    }
    catch(...) {
        string s = "unknown exception caught building Freeform DDS";
        BESInternalFatalError ex(s, __FILE__, __LINE__);
        throw ex;
    }

    return true;
}

bool FFRequestHandler::ff_build_data(BESDataHandlerInterface & dhi)
{
    BufPtr = 0;                 // cache pointer
    BufSiz = 0;                 // Cache size
    BufVal = NULL;              // cache buffer

    BESDataDDSResponse *bdds =
        dynamic_cast <BESDataDDSResponse *>(dhi.response_handler->get_response_object());
    DataDDS *dds = bdds->get_dds();
    ConstraintEvaluator & ce = bdds->get_ce();

    try {
        FFTypeFactory *factory = new FFTypeFactory;
        dds->set_factory(factory);
        
        ff_register_functions(ce);
        string accessed = dhi.container->access();
        dds->filename(accessed);
        ff_read_descriptors(*dds, accessed);
	Ancillary::read_ancillary_dds( *dds, accessed ) ;

        DAS das;
        ff_get_attributes(das, accessed);
        string name = Ancillary::find_ancillary_file(accessed, "das", "", "");
        if (!name.empty())
            das.parse(name);

	Ancillary::read_ancillary_das( das, accessed ) ;
        
        dds->transfer_attributes(&das);

        dhi.data[POST_CONSTRAINT] = dhi.container->get_constraint();
    }
    catch(InternalErr & e) {
        BESDapError ex( e.get_error_message(), true, e.get_error_code(),
			__FILE__, __LINE__ ) ;
        throw ex;
    }
    catch(Error & e) {
        BESDapError ex( e.get_error_message(), false, e.get_error_code(),
			__FILE__, __LINE__ ) ;
        throw ex;
    }
    catch(...) {
        string s = "unknown exception caught building Freeform DataDDS";
        BESInternalFatalError ex(s, __FILE__, __LINE__);
        throw ex;
    }

    return true;
}

bool FFRequestHandler::ff_build_help(BESDataHandlerInterface & dhi)
{
    BESInfo *info =
        (BESInfo *) dhi.response_handler->get_response_object();
    info->begin_tag("Handler");
    info->add_tag("name", PACKAGE_NAME);
    string handles = (string) DAS_RESPONSE + "," + DDS_RESPONSE
        + "," + DATA_RESPONSE + "," + HELP_RESPONSE + "," + VERS_RESPONSE;
    info->add_tag("handles", handles);
    info->add_tag("version", PACKAGE_STRING);
    info->end_tag("Handler");

    return true;
}

bool FFRequestHandler::ff_build_version(BESDataHandlerInterface & dhi)
{
    BESVersionInfo *info =
        (BESVersionInfo *) dhi.response_handler->get_response_object();
    info->addHandlerVersion(PACKAGE_NAME, PACKAGE_VERSION);
    return true;
}
