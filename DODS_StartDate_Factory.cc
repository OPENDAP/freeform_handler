
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// Implementation of the DODS_StartDate_Factory class

// $Log: DODS_StartDate_Factory.cc,v $
// Revision 1.2  2000/08/31 22:16:54  jimg
// Merged with 3.1.7
//
// Revision 1.1.2.2  2000/08/03 20:18:57  jimg
// Removed config_dap.h and replaced it with config_ff.h (in *.cc files;
// neither should be included in a header file).
// Changed code that calculated leap year information so that it uses the
// functions in date_proc.c/h.
//
// Revision 1.1.2.1  2000/05/01 20:11:47  dan
// New server-side function to support date-range usage in time fields.
//
//

#include "config_ff.h"

static char rcsid[] not_used = "$Id: DODS_StartDate_Factory.cc,v 1.2 2000/08/31 22:16:54 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include <string>

#include "AttrTable.h"
#include "Error.h"

#include "DODS_StartDate_Factory.h"

DODS_StartDate_Factory::DODS_StartDate_Factory(DDS &dds, DAS &das)
{
    // Read the names of the variables which encode year, month and
    // day from the DAS. These are contained in the DODS_Date attribute
    // container. 
    
    AttrTable *at = das.get_table("DODS_StartDate");
    if (!at)
	throw Error(unknown_error,
"DODS_StartDate_Factory requires that the DODS_StartDate attribute be present.");

    string year_name = at->get_attr("year_variable");
    string year_base = at->get_attr("year_base");
    string month_name = at->get_attr("month_variable");
    string day_name = at->get_attr("day_variable");
    string year_day_name = at->get_attr("year_day_variable");

    // Check to see if these dates are ymd or yd type dates.

    if (year_day_name == "" && day_name != "")
	_format = ymd;
    else if (year_day_name != "" && day_name == "")
	_format = yd;
    else
	throw Error(unknown_error,
"DODS_StartDate_Factory requires that one, and only one, of the attributes\n\
day_variable or year_day_variable be present.");

    // Extract year_base if its present, else set year base to zero.

    if (year_base == "")
	_year_base = 0;
    else {
	const char *c = year_base.c_str();
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
	throw Error(unknown_error, "DODS_StartDate_Factory: The variable used for the year must be an integer.");

    switch (_format) {
      case ymd: {
	  _month = dds.var(month_name);
	  if (_month->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_StartDate_Factory: The variable used for the month must be an integer.");
	  _day = dds.var(day_name);
	  if (_day->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_StartDate_Factory: The variable used for days must be an integer.");
	  _year_day = 0;
	  break;
      }

      case yd: {
	  _month = 0;
	  _day = 0;
	  _year_day = dds.var(year_day_name);
	  if (_year_day->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_StartDate_Factory: The variable used for the year-day must be an integer.");
	  break;
      }

      default:
	throw Error(unknown_error,
"DODS_StartDate_Factory: Not able to figure out the date format.");
	break;
    }
}

DODS_Date
DODS_StartDate_Factory::get()
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
"DODS_StartDate_Factory: Unknown date format, should never get here!");
    }
}











