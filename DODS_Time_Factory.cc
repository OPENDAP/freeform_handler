
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODS_Time_Factory class

// $Log: DODS_Time_Factory.cc,v $
// Revision 1.1  1998/12/28 19:08:05  jimg
// Initial version of the DODS_Time factory object. This is a test implementation.
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: DODS_Time_Factory.cc,v 1.1 1998/12/28 19:08:05 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include "String.h"

#include "AttrTable.h"
#include "Error.h"

#include "DODS_Time_Factory.h"

DODS_Time_Factory::DODS_Time_Factory(DDS &dds, DAS &das)
{
    // Read the names of the variables which encode hours, minutes and
    // seconds from the DAS. These are contained in the DODS_Time attribute
    // container. 
    
    AttrTable *at = das.get_table("DODS_Time");
    if (!at)
	throw Error(unknown_error, "DODS_Time_Factory requires that the DODS_Time attribute be present.");

    String _hours_name = at->get_attr("hours_variable");
    String _mins_name = at->get_attr("minutes_variable");
    String _secs_name = at->get_attr("seconds_variable");

    // Now check that these variables actually exist and that they have
    // sensible types.

    _hours = dds.var(_hours_name);
    if (_hours->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Time_Factory: The variable used for hours must be an integer.");

    _minutes = dds.var(_mins_name);
    if (_minutes->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Time_Factory: The variable used for minutes must be an integer.");

    _seconds = dds.var(_secs_name);
    if (_seconds->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Time_Factory: The variable used for seconds must be an integer.");
}

DODS_Time
DODS_Time_Factory::get_time()
{
    int hour;
    int *hour_p = &hour;
    _hours->buf2val((void **)&hour_p);

    int min;
    int *min_p = &min;
    _minutes->buf2val((void **)&min_p);

    int sec;
    int *sec_p = &sec;
    _seconds->buf2val((void **)&sec_p);

    DODS_Time t(hour, min, sec, true);

    DBG(cerr << "hh: " << hour << " mm: " << min << " ss: " << sec << endl);

    return t;
}
