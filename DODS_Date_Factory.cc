
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODS_Date_Factory class

// $Log: DODS_Date_Factory.cc,v $
// Revision 1.1  1999/01/22 20:44:34  jimg
// Added
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: DODS_Date_Factory.cc,v 1.1 1999/01/22 20:44:34 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>

#include "String.h"

#include "AttrTable.h"
#include "Error.h"

#include "DODS_Date_Factory.h"

DODS_Date_Factory::DODS_Date_Factory(DDS &dds, DAS &das)
{
    // Read the names of the variables which encode year, month and
    // day from the DAS. These are contained in the DODS_Date attribute
    // container. 
    
    AttrTable *at = das.get_table("DODS_Date");
    if (!at)
	throw Error(unknown_error,
"DODS_Date_Factory requires that the DODS_Date attribute be present.");

    String year_name = at->get_attr("year_variable");
    String year_base = at->get_attr("year_base");
    String month_name = at->get_attr("month_variable");
    String day_name = at->get_attr("day_variable");
    String year_day_name = at->get_attr("year_day_variable");

    // Check to see if these dates are ymd or yd type dates.

    if (year_day_name == "" && day_name != "")
	_format = ymd;
    else if (year_day_name != "" && day_name == "")
	_format = yd;
    else
	throw Error(unknown_error,
"DODS_Date_Factory requires that one, and only one, of the attributes\n\
day_variable or year_day_variable be present.");

    // Extract year_base if its present, else set year base to zero.

    if (year_base == "")
	_year_base = 0;
    else {
	const char *c = year_base.chars();
	char *c2;
	_year_base = strtol(c, &c2, 0);
	if (c == c2 || _year_base == LONG_MAX || _year_base == LONG_MIN)
	    throw Error(unknown_error,
"The year_base attribute value cannot be converted to a valid integer.");
    }

    // Now check that these variables actually exist and that they have
    // sensible types.

    _year = dds.var(year_name);
    if (_year->type() != dods_int32_c)
	throw Error(unknown_error, "DODS_Date_Factory: The variable used for the year must be an integer.");

    switch (_format) {
      case ymd: {
	  _month = dds.var(month_name);
	  if (_month->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_Date_Factory: The variable used for the month must be an integer.");
	  _day = dds.var(day_name);
	  if (_day->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_Date_Factory: The variable used for days must be an integer.");
	  _year_day = 0;
	  break;
      }

      case yd: {
	  _month = 0;
	  _day = 0;
	  _year_day = dds.var(year_day_name);
	  if (_year_day->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_Date_Factory: The variable used for the year-day must be an integer.");
	  break;
      }

      default:
	throw Error(unknown_error,
"DODS_Date_Factory: Not able to figure out the date format.");
	break;
    }
}

DODS_Date
DODS_Date_Factory::get()
{
    int year;
    int *year_p = &year;
    _year->buf2val((void **)&year_p);

    switch (_format) {
      case ymd: {
	  int month;
	  int *month_p = &month;
	  _month->buf2val((void **)&month_p);

	  int day;
	  int *day_p = &day;
	  _day->buf2val((void **)&day_p);

	  return DODS_Date(year + _year_base, month, day);
	  break;
      }

      case yd: {
	  int year_day;
	  int *year_day_p = &year_day;
	  _year_day->buf2val((void **)&year_day_p);

	  return DODS_Date(year + _year_base, year_day);
	  break;
      }

      default:
	throw Error(unknown_error, 
"DODS_Date_Factory: Unknown date format, should never get here!");
    }
}
