
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

// Implementation of the DODS_EndDate_Factory class

// No longer used. 1/21/2002 jhrg

#if 0
#include "config_ff.h"

static char rcsid[] not_used = "$Id$";


#include <stdlib.h>

#include <string>

#include "AttrTable.h"
#include "Error.h"

#include "DODS_EndDate_Factory.h"

DODS_EndDate_Factory::DODS_EndDate_Factory(DDS &dds)
{
    // Read the names of the variables which encode year, month and
    // day from the DAS. These are contained in the DODS_Date attribute
    // container. 
    
    AttrTable *at = dds.get_attr_table().find_container(attribute_name);
    if (!at)
	throw Error(unknown_error,
"DODS_EndDate_Factory requires that the DODS_EndDate attribute be present.");

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
"DODS_EndDate_Factory requires that one, and only one, of the attributes\n\
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
	throw Error(unknown_error, "DODS_EndDate_Factory: The variable used for the year must be an integer.");

    switch (_format) {
      case ymd: {
	  _month = dds.var(month_name);
	  if (_month->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_EndDate_Factory: The variable used for the month must be an integer.");
	  _day = dds.var(day_name);
	  if (_day->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_EndDate_Factory: The variable used for days must be an integer.");
	  _year_day = 0;
	  break;
      }

      case yd: {
	  _month = 0;
	  _day = 0;
	  _year_day = dds.var(year_day_name);
	  if (_year_day->type() != dods_int32_c)
	      throw Error(unknown_error, 
"DODS_EndDate_Factory: The variable used for the year-day must be an integer.");
	  break;
      }

      default:
	throw Error(unknown_error,
"DODS_EndDate_Factory: Not able to figure out the date format.");
	break;
    }
}

DODS_Date
DODS_EndDate_Factory::get()
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
"DODS_EndDate_Factory: Unknown date format, should never get here!");
    }
}
#endif

// $Log: DODS_EndDate_Factory.cc,v $
// Revision 1.4  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.3.2.1  2002/01/22 02:19:35  jimg
// Fixed bug 62. Users built fmt files that used types other than int32
// for date and time components (e.g. int16). I fixed the factory classes
// so that DODS_Date and DODS_Time objects will be built correctly when
// any of the integer (or in the case of seconds, float) data types are
// used. In so doing I also refactored the factory classes so that code
// duplication was reduced (by using inhertiance).
// Added two tests for the new capabilities (see date_time.1.exp, the last
// two tests).
//
// Revision 1.3  2000/10/11 19:37:55  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.2  2000/08/31 22:16:53  jimg
// Merged with 3.1.7
//
// Revision 1.1.2.2  2000/08/03 20:18:57  jimg
// Removed config_dap.h and replaced it with config_ff.h (in *.cc files;
// neither should be included in a header file).
// Changed code that calculated leap year information so that it uses the
// functions in date_proc.c/h.
//
// Revision 1.1.2.1  2000/05/01 21:25:45  dan
// New server-side function to support date-range usage in time fields.
//
//
