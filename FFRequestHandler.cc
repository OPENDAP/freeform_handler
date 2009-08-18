
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

#include "FFRequestHandler.h"

#include "config_ff.h"
#include "ff_ce_functions.h"
#include "util_ff.h"

#include <BESDASResponse.h>
#include <BESDDSResponse.h>
#include <BESDataDDSResponse.h>
#include <BESInfo.h>
#include <BESDapNames.h>
#include <BESResponseNames.h>
#include <BESContainer.h>
#include <BESResponseHandler.h>
#include <BESVersionInfo.h>
#include <BESServiceRegistry.h>
#include <BESUtil.h>

#include <BESDapError.h>
#include <BESInternalFatalError.h>
#include <InternalErr.h>

#include <DDS.h>
#include <Ancillary.h>
#include <Error.h>
#include <escaping.h>
#include <cgi_util.h>

#define FF_NAME "ff"

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
    BESResponseObject *response = dhi.response_handler->get_response_object() ;
    BESDASResponse *bdas = dynamic_cast < BESDASResponse * >(response) ;
    if( !bdas )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;

    try {
	bdas->set_container( dhi.container->get_symbolic_name() ) ;
	DAS *das = bdas->get_das();

	string accessed = dhi.container->access() ;
        ff_get_attributes(*das, accessed);
#ifdef RSS
	string name = find_ancillary_rss_das(accessed);
#else
        string name = Ancillary::find_ancillary_file(accessed, "das", "", "");
#endif
        if (!name.empty())
            das->parse(name);

	bdas->clear_container() ;
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
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESDDSResponse *bdds = dynamic_cast < BESDDSResponse * >(response);
    if( !bdds )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;
  
    try {
	bdds->set_container( dhi.container->get_symbolic_name() ) ;
	DDS *dds = bdds->get_dds();
	ConstraintEvaluator & ce = bdds->get_ce();

        ff_register_functions(ce);
	string accessed = dhi.container->access();
        dds->filename(accessed);
        ff_read_descriptors(*dds, accessed);

        DAS *das = new DAS ;
	BESDASResponse bdas( das ) ;
	bdas.set_container( dhi.container->get_symbolic_name() ) ;
        ff_get_attributes( *das, accessed ) ;
	Ancillary::read_ancillary_das( *das, accessed ) ;
        
        dds->transfer_attributes( das ) ;

	bdds->set_constraint( dhi ) ;

	bdds->clear_container() ;
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

    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESDataDDSResponse *bdds = dynamic_cast < BESDataDDSResponse * >(response);
    if( !bdds )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;
  
    try {
	bdds->set_container( dhi.container->get_symbolic_name() ) ;
	DataDDS *dds = bdds->get_dds();
	ConstraintEvaluator & ce = bdds->get_ce();

        ff_register_functions(ce);
        string accessed = dhi.container->access();
        dds->filename(accessed);
        ff_read_descriptors(*dds, accessed);
	Ancillary::read_ancillary_dds( *dds, accessed ) ;

        DAS *das = new DAS ;
	BESDASResponse bdas( das ) ;
	bdas.set_container( dhi.container->get_symbolic_name() ) ;
        ff_get_attributes( *das, accessed ) ;
	Ancillary::read_ancillary_das( *das, accessed ) ;
        
        dds->transfer_attributes( das ) ;

	bdds->set_constraint( dhi ) ;

	bdds->clear_container() ;
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
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESInfo *info = dynamic_cast<BESInfo *>(response);
    if( !info )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;

    map<string,string> attrs ;
    attrs["name"] = PACKAGE_NAME ;
    attrs["version"] = PACKAGE_VERSION ;
    list<string> services ;
    BESServiceRegistry::TheRegistry()->services_handled( FF_NAME, services );
    if( services.size() > 0 )
    {
	string handles = BESUtil::implode( services, ',' ) ;
	attrs["handles"] = handles ;
    }
    info->begin_tag( "module", &attrs ) ;
    info->end_tag( "module" ) ;

    return true;
}

bool FFRequestHandler::ff_build_version(BESDataHandlerInterface & dhi)
{
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESVersionInfo *info = dynamic_cast < BESVersionInfo * >(response);
    if( !info )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;
  
    info->add_module( PACKAGE_NAME, PACKAGE_VERSION ) ;

    return true ;
}
