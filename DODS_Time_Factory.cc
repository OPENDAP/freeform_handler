
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODS_Time_Factory class

// $Log: DODS_Time_Factory.cc,v $
// Revision 1.5  1999/07/22 21:28:09  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.4.2.1  1999/06/04 15:01:35  dan
// Fixed problem caused by testing '_hours', prior to setting '_hours'
// with an assignment to dds.var(). Same for '_minutes', '_seconds'.
//
// Revision 1.4  1999/05/25 18:37:50  dan
// Check for empty hour/minute/second variables in a DODS_Time
// variable, if these variables do not exist set a default value
// for them to 0.
//
// Revision 1.3  1999/05/04 02:55:35  jimg
// Merge with no-gnu
//
// Revision 1.2.8.1  1999/05/01 04:40:28  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.2  1999/01/05 00:40:44  jimg
// Switched to simpler method names.
//
// Revision 1.1  1998/12/28 19:08:05  jimg
// Initial version of the DODS_Time factory object. This is a test implementation.
//

#include <string>

#include "config_dap.h"

static char rcsid[] not_used ="$Id: DODS_Time_Factory.cc,v 1.5 1999/07/22 21:28:09 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include "AttrTable.h"
#include "Error.h"
#include "util.h"

#include "DODS_Time_Factory.h"

DODS_Time_Factory::DODS_Time_Factory(DDS &dds, DAS &das)
{
    // Read the names of the variables which encode hours, minutes and
    // seconds from the DAS. These are contained in the DODS_Time attribute
    // container. 
    
    AttrTable *at = das.get_table("DODS_Time");
    if (!at)
	throw Error(unknown_error, "DODS_Time_Factory requires that the DODS_Time attribute be present.");

    string _hours_name = at->get_attr("hours_variable");
    string _mins_name = at->get_attr("minutes_variable");
    string _secs_name = at->get_attr("seconds_variable");
    string _gmt = at->get_attr("gmt_time");

    // If the gmt attribute is present that meanas that the times are GMT/UTC
    // times. Set the _gmt flag true, otherwise set it false.
    
    downcase(_gmt);
    if (_gmt == "true")
	_gmt = true;
    else
	_gmt = false;

    // Now check that these variables actually exist and that they have
    // sensible types.

    _hours = dds.var(_hours_name);
    if ( _hours ) {
      if (_hours->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Time_Factory: The variable used for hours must be an integer.");
    }

    _minutes = dds.var(_mins_name);
    if ( _minutes ) {
      if (_minutes->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Time_Factory: The variable used for minutes must be an integer.");
    }

    _seconds = dds.var(_secs_name);
    if ( _seconds ) {
      if (_seconds->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Time_Factory: The variable used for seconds must be an integer.");
    }
}

DODS_Time
DODS_Time_Factory::get()
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



