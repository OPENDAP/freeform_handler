// FFModule.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmostpheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#include <iostream>

using std::endl ;

#include "FFModule.h"
#include "BESRequestHandlerList.h"
#include "FFRequestHandler.h"
#include "BESContainerStorageList.h"
#include "BESContainerStorageCatalog.h"
#include "BESCatalogDirectory.h"
#include "BESCatalogList.h"
#include "BESDebug.h"

#define FF_CATALOG "catalog"

void
FFModule::initialize( const string &modname )
{
    BESDEBUG( "Initializing FF module " << modname << endl )

    BESDEBUG( "    adding " << modname << " request handler" << endl )
    BESRequestHandler *handler = new FFRequestHandler( modname ) ;
    BESRequestHandlerList::TheList()->add_handler( modname, handler ) ;

    BESDEBUG( "    adding " << FF_CATALOG << " catalog" << endl )
    BESCatalogList::TheCatalogList()->add_catalog( new BESCatalogDirectory( FF_CATALOG ) ) ;

    BESDEBUG( "    adding catalog container storage" << FF_CATALOG << endl )
    BESContainerStorageCatalog *csc = new BESContainerStorageCatalog( FF_CATALOG ) ;
    BESContainerStorageList::TheList()->add_persistence( csc ) ;

    BESDEBUG( "Done Initializing FF module " << modname << endl )
}

void
FFModule::terminate( const string &modname )
{
    BESDEBUG( "Cleaning FF module " << modname << endl )

    BESDEBUG( "    removing FF Handler" << modname << endl )
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler( modname ) ;
    if( rh ) delete rh ;

    BESDEBUG( "    removing catalog container storage" << FF_CATALOG << endl )
    BESContainerStorageList::TheList()->del_persistence( FF_CATALOG ) ;

    BESDEBUG( "    removing " << FF_CATALOG << " catalog" << endl )
    BESCatalogList::TheCatalogList()->del_catalog( FF_CATALOG ) ;

    BESDEBUG( "Done Cleaning FF module " << modname << endl )
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
FFModule::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "FFModule::dump - ("
			     << (void *)this << ")" << endl ;
}

extern "C"
{
    BESAbstractModule *maker()
    {
        return new FFModule ;
    }
}

