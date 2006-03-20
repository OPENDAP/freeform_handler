
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


// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

#ifndef _dods_date_factory_h
#define _dods_date_factory_h


#include "DODS_Date.h"
#include "DAS.h"
#include "DDS.h"
#include "BaseType.h"

/** Read dates from datasets based on text configuration values. Dates are
    returned using DODS\_Date objects.

    @see DODS_Date
    @author James Gallagher */

class DODS_Date_Factory {
private:
    int _year_base;

    BaseType *_year;
    BaseType *_month;
    BaseType *_day;
    BaseType *_year_day;
    int _month_day;

    enum date_format _format;

protected:
    DODS_Date_Factory() {}	/* Prevent the creation of empty objects. */

public:
    virtual ~DODS_Date_Factory() {}
    
    /** Note that the default constructor is private.
	@name Constructors */

    //@{
    /** Read the configuration information and decide how to build DODS\_Date
	objects. The DODS\_Date\_Factory member function #get_date()# will
	return DODS\_Date objects.

	@see get_date()
	@param dds The DDS of the dataset from which dates are to be read.
	@param das The DAS of the dataset from which dates are to be read. 
	@param attribute_name The name of the attribute container where
	configuration information for the #date# servers-side function can be
	found. */
    
    DODS_Date_Factory(DDS &dds, DAS &das, 
		      const string &attribute_name = "DODS_Date");
    //@}

    /** @name Access */
    //@{
    /** Read a date value from a dataset.

	@return The DODS\_Date object associated with the date. */

    virtual DODS_Date get();
    //@}
};

// $Log: DODS_Date_Factory.h,v $
// Revision 1.4  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.3  2001/09/28 23:19:43  jimg
// Merged with 3.2.3.
//
// Revision 1.2.2.2  2002/01/22 02:19:35  jimg
// Fixed bug 62. Users built fmt files that used types other than int32
// for date and time components (e.g. int16). I fixed the factory classes
// so that DODS_Date and DODS_Time objects will be built correctly when
// any of the integer (or in the case of seconds, float) data types are
// used. In so doing I also refactored the factory classes so that code
// duplication was reduced (by using inhertiance).
// Added two tests for the new capabilities (see date_time.1.exp, the last
// two tests).
//
// Revision 1.2.2.1  2001/05/23 18:26:06  dan
// Modified to support year/month date representations,
// and to support ISO8601 output formats.
//
// Revision 1.2  2000/10/11 19:37:55  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.1  1999/01/08 22:08:18  jimg
// Fixed doc++ comments.
//

#endif // _dods_date_factory_h 
