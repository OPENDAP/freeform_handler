
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// Implementation of the DODS_Time_Factory class

// This file is no longer needed. See DODS_StartTime_Factory.h
// 1/20/2002 jhrg

#if 0
#include "config_ff.h"

static char rcsid[] not_used ="$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include <string>

#include "AttrTable.h"
#include "Error.h"
#include "util.h"

#include "DODS_StartTime_Factory.h"

DODS_StartTime_Factory::DODS_StartTime_Factory(DDS &dds, DAS &das)
{
    // Read the names of the variables which encode hours, minutes and
    // seconds from the DAS. These are contained in the DODS_StartTime attribute
    // container. 
    
    AttrTable *at = das.get_table("DODS_StartTime");
    if (!at)
	throw Error("DODS_StartTime_Factory requires that the DODS_StartTime attribute be present.");

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
	throw Error(unknown_error, "DODS_StartTime_Factory: The variable used for hours must be an integer.");

    _minutes = dds.var(mins_name);
    if ( _minutes && _minutes->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_StartTime_Factory: The variable used for minutes must be an integer.");

    _seconds = dds.var(secs_name);
    if ( _seconds && _seconds->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_StartTime_Factory: The variable used for seconds must be an integer.");

}

DODS_Time
DODS_StartTime_Factory::get()
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




