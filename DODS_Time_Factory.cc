
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODS_Time_Factory class

#include "config_ff.h"

static char rcsid[] not_used ="$Id: DODS_Time_Factory.cc,v 1.9 2003/02/10 23:01:52 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <string>

#include "AttrTable.h"
#include "Error.h"
#include "InternalErr.h"
#include "dods-datatypes.h"
#include "util.h"
#include "util_ff.h"

#include "DODS_Time_Factory.h"

// attribute_name defaults to "DODS_TIME".
DODS_Time_Factory::DODS_Time_Factory(DDS &dds, DAS &das, 
				     const string &attribute_name)
{
    // Read the names of the variables which encode hours, minutes and
    // seconds from the DAS. These are contained in the DODS_Time attribute
    // container. 
    
    AttrTable *at = das.get_table(attribute_name);
    if (!at)
	throw Error(string("DODS_Time_Factory requires that the ")
		    + attribute_name + string(" attribute be present."));

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
    if (_hours && !is_integer_type(_hours))
	throw Error("DODS_Time_Factory: The variable used for hours must be an integer.");

    _minutes = dds.var(mins_name);
    if (_minutes && !is_integer_type(_minutes))
	throw Error("DODS_Time_Factory: The variable used for minutes must be an integer.");

    _seconds = dds.var(secs_name);
    if (_seconds && !(is_integer_type(_seconds) || is_float_type(_seconds)))
	throw Error("DODS_Time_Factory: The variable used for seconds must be an integer.");
}

DODS_Time
DODS_Time_Factory::get()
{
#if 0
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
#endif

    return DODS_Time(get_integer_value(_hours), get_integer_value(_minutes),
		     get_float_value(_seconds), _gmt);
}

// $Log: DODS_Time_Factory.cc,v $
// Revision 1.9  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.8  2001/10/14 01:36:17  jimg
// Merged with release-3-2-4.
//
// Revision 1.7.2.2  2002/01/22 02:19:35  jimg
// Fixed bug 62. Users built fmt files that used types other than int32
// for date and time components (e.g. int16). I fixed the factory classes
// so that DODS_Date and DODS_Time objects will be built correctly when
// any of the integer (or in the case of seconds, float) data types are
// used. In so doing I also refactored the factory classes so that code
// duplication was reduced (by using inhertiance).
// Added two tests for the new capabilities (see date_time.1.exp, the last
// two tests).
//
// Revision 1.7.2.1  2001/10/11 17:42:09  jimg
// Fixed a bug in the Time, StartTime and EndTime factory calsses. A local
// variable _gmt shadowed the class member _gmt.
//
// Revision 1.7  2000/10/11 19:37:55  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.6  2000/08/31 22:16:55  jimg
// Merged with 3.1.7
//
// Revision 1.5.2.1  2000/08/03 20:18:57  jimg
// Removed config_dap.h and replaced it with config_ff.h (in *.cc files;
// neither should be included in a header file).
// Changed code that calculated leap year information so that it uses the
// functions in date_proc.c/h.
//
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
