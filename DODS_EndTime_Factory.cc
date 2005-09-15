
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
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// Implementation of the DODS_Time_Factory class

#if 0
// *** This file is no linger needed. See DODS_EndTime_Factory.h
// 1/20/2002 jhrg

#include "config_ff.h"

static char rcsid[] not_used ="$Id$";


#include <string>

#include "AttrTable.h"
#include "Error.h"
#include "util.h"

#include "DODS_EndTime_Factory.h"

DODS_EndTime_Factory::DODS_EndTime_Factory(DDS &dds, DAS &das)
{
    // Read the names of the variables which encode hours, minutes and
    // seconds from the DAS. These are contained in the DODS_EndTime attribute
    // container. 
    
    AttrTable *at = das.get_table("DODS_EndTime");
    if (!at)
	throw Error(unknown_error, "DODS_EndTime_Factory requires that the DODS_Time attribute be present.");

    string hours_name = at->get_attr("hours_variable");
    string mins_name = at->get_attr("minutes_variable");
    string secs_name = at->get_attr("seconds_variable");
    string gmt = at->get_attr("gmt_time");

    // If the gmt attribute is present that meanas that the times are GMT/UTC
    // times. Set the _gmt flag true, otherwise set it false.
    
    downcase(gmt);
    if (gmt == "true")
	_gmt = true;
    else
	_gmt = false;

    // Now check that these variables actually exist and that they have
    // sensible types.

    _hours = dds.var(hours_name);
    if ( _hours && _hours->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_EndTime_Factory: The variable used for hours must be an integer.");

    _minutes = dds.var(mins_name);
    if ( _minutes && _minutes->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_EndTime_Factory: The variable used for minutes must be an integer.");

    _seconds = dds.var(secs_name);
    if ( _seconds && _seconds->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_EndTime_Factory: The variable used for seconds must be an integer.");
}

DODS_Time
DODS_EndTime_Factory::get()
{
    int hour;
    int *hour_p = &hour;
    if ( _hours )
      _hours->buf2val((void **)&hour_p);
    else
      hour = 0;

    int min;
    int *min_p = &min;
    if ( _minutes )
      _minutes->buf2val((void **)&min_p);
    else
      min = 0;

    int sec;
    int *sec_p = &sec;
    if ( _seconds )
      _seconds->buf2val((void **)&sec_p);
    else
      sec = 0;

    return DODS_Time(hour, min, sec, _gmt);
}
#endif



