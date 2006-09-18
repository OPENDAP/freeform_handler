
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

// (c) COPYRIGHT URI/MIT 1998-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains various functions for use with/in constraint
// expressions. 

#include <iostream>
#include <string>
#include <algorithm>

#include "BaseType.h"
#include "Str.h"
#include "Structure.h"
#include "Sequence.h"
#include "DDS.h"
#include "ConstraintEvaluator.h"
#include "Error.h"
#include "util.h"
#include "cgi_util.h"
#include "debug.h"

#include "date_proc.h"
#include "DODS_Date.h"
#include "DODS_Date_Factory.h"
#include "DODS_StartDate_Factory.h"
#include "DODS_EndDate_Factory.h"
#include "DODS_Time.h"
#include "DODS_Time_Factory.h"
#include "DODS_StartTime_Factory.h"
#include "DODS_EndTime_Factory.h"
#include "DODS_Date_Time.h"
#include "DODS_Date_Time_Factory.h"
#include "DODS_StartDate_Time_Factory.h"
#include "DODS_EndDate_Time_Factory.h"

#include "ff_ce_functions.h"

/** Read an instance of T using a Factory for objects of type T. The Factory
    class for T must read configuration information from the DAS.

    @return An instance of the class T. */

template<class T, class T_Factory>
static T
get_instance(DDS &dds)
{
    static T_Factory *tf = 0;
    if (!tf) {
	// Hack
	string name = find_ancillary_file(dds.filename(), "das", "", "");
	DAS das;
	das.parse(name);
	// end hack
	tf = new T_Factory(dds, das);
    }

    return tf->get();
}

/** Compare an instance of T read from the dataset with the strings in one or
    two DODS Str variables. The Strs are passed into the function using
    BaseType pointers since that is how the constraint expression evaluator
    passes arguments to functions.

    @param argc The number of elements in argv[].
    @param argv[] An array of arguments.
    @param dds The DDS for the dataset.
    @return If one argument is given, return true if the value read from the
    dataset matches the argument value. If two arguments are given, return
    true if the value read from the dataset falls within (inclusive) of the
    two arguments given. */

template<class T, class T_Factory>
static bool
comparison(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 1 || argc > 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a constraint expression function.");

    T t1(argv[0]);
    T t2;
    if (argc == 2)
	t2.set(argv[1]);

    T current = get_instance<T, T_Factory>(dds);
    bool te;

    if (argc == 2) {
      te = ((t1 <= current) && (t2 >= current));
	return ((t1 <= current) && (t2 >= current));
    }  
    else {
      te = (t1 == current);
	return (t1 == current);
    }
}

/** Compare an instance of T read from the dataset with the strings in one or
    two DODS Str variables. The Strs are passed into the function using
    BaseType pointers since that is how the constraint expression evaluator
    passes arguments to functions.

    @param argc The number of elements in argv[].
    @param argv[] An array of arguments.
    @param dds The DDS for the dataset.
    @return If one argument is given, return true if the value read from the
    dataset matches the argument value. If two arguments are given, return
    true if the value read from the dataset falls within (inclusive) of the
    two arguments given. */

template<class T1, class T1_Factory, class T2, class T2_Factory>
static bool
range_comparison(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a constraint expression function.");

    T1 t1(argv[0]);
    T2 t2(argv[1]);

    T1 current_start = get_instance<T1, T1_Factory>(dds);
    T2 current_end = get_instance<T2, T2_Factory>(dds);

    return (((current_start >= t1) && (current_start <= t2)) ||
	    ((current_end >= t1) && (current_end <= t2)) ||
	    ((current_start <= t1) && ( current_end >= t2)));
}

/** Load a new Str variable into the DDS. If position is given, then insert
    the new Str variable into that Structure or Sequence. If position is
    given and is not a Structure or Sequence, throw an exception.

    @param name The name of the new variable.
    @param dds The DDS of the dataset.
    @param position Add the new variable to this Structure or Sequence. */

static void
new_string_variable(const string &name, DDS &dds, BaseType *position = 0)
{
    // Create the new variable

    Str *new_variable = dds.get_factory()->NewStr(name.c_str());
    new_variable->set_read_p(true); // You must call this before ...
    new_variable->set_synthesized_p(true); // this! Look at BaseType.cc.

    // Add it to the DDS in the right place

    if (position) {
	switch (position->type()) {
	  case dods_structure_c: {
	      Structure *sp = (Structure *)position;
	      sp->add_var((BaseType *)new_variable);
	      break;
	  }

	  case dods_sequence_c: {
	      Sequence *sp = (Sequence *)position;
	      sp->add_var((BaseType *)new_variable);
	      break;
	  }

	  default:
	throw Error(malformed_expr,
"You asked me to insert the synthesized variable in \n\
something that did not exist or was not a constructor \n\
type (e.g., a structure, sequence, ...).");
	    break;
	}
    }
    else {
	dds.add_var(new_variable);
    }

    // Mark the variable as part of the current projection.

    dds.mark(name, true); // Don't just call set_send_p()!
}

static bool
func_date(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date, DODS_Date_Factory>(argc, argv, dds);
}

static bool
func_startdate(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date, DODS_StartDate_Factory>(argc, argv, dds);
}

static bool
func_enddate(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date, DODS_EndDate_Factory>(argc, argv, dds);
}

static bool
func_date_range(int argc, BaseType *argv[], DDS &dds)
{
    return range_comparison<DODS_Date, DODS_StartDate_Factory, DODS_Date, DODS_EndDate_Factory>(argc, argv, dds);
}

static bool
func_time(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Time, DODS_Time_Factory>(argc, argv, dds);
}

static bool
func_starttime(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Time, DODS_StartTime_Factory>(argc, argv, dds);
}

static bool
func_endtime(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Time, DODS_EndTime_Factory>(argc, argv, dds);
}

// This comparision function should be used for decimal dates. 5/29/99 jhrg

static bool
func_date_time(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date_Time, DODS_Date_Time_Factory>(argc, argv, dds);
}

static bool
func_startdate_time(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date_Time, DODS_StartDate_Time_Factory>(argc, argv, dds);
}

static bool
func_enddate_time(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date_Time, DODS_EndDate_Time_Factory>(argc, argv, dds);
}

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

// The date and date_time functions should now recognize decimal format years
// and date-times. 5/30/99 jhrg

static bool
sel_dods_jdate(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
"Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date current = get_instance<DODS_Date, DODS_Date_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_jdate = (Str*)dds.var("DODS_JDate");
    // By calling DODS_Date::get with the token `yd' I'm explicitly asking
    // for the year/day (pseudo juilian) date format. 5/27/99 jhrg
    string s = current.get(yd).c_str();
    dods_jdate->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_jdate(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    new_string_variable("DODS_JDate", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_jdate, 0); // 0 == no BaseType args
}

// Same as the above function, but for ymd dates.

static bool
sel_dods_date(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date current = get_instance<DODS_Date, DODS_Date_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_Date.
    Str *dods_date = (Str*)dds.var("DODS_Date");
    // Calling the regular form of DODS_Date::get() returns the data in y/m/d
    // format. 5/27/99 jhrg
    string s = current.get().c_str();
    dods_date->val2buf(&s);

    return true;
}

static void
proj_dods_date(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    new_string_variable("DODS_Date", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_date, 0); // 0 == no BaseType args
}

/************************ DODS_Time functions *************************/


static bool
sel_dods_time(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Time current = get_instance<DODS_Time, DODS_Time_Factory>(dds);

    // Stuff the "hh:mm:ss" string into `DODS_Time'
    Str *dods_time = (Str*)dds.var("DODS_Time");
    string s = current.get().c_str();
    dods_time->val2buf(&s);

    return true;
}

static void
proj_dods_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Time", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_time, 0); // 0 == no BaseType args
}

/*************************** Date/Time functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

static bool
sel_dods_date_time(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date_Time current 
	= get_instance<DODS_Date_Time, DODS_Date_Time_Factory>(dds);

    Str *dods_date_time = (Str*)dds.var("DODS_Date_Time");
    string s = current.get().c_str();
    dods_date_time->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_date_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Date_Time", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_date_time, 0); // 0 == no BaseType args
}

/*************************** Decimal/Year functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

static bool
sel_dods_decimal_year(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date_Time current 
	= get_instance<DODS_Date_Time, DODS_Date_Time_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_decimal_year = (Str*)dds.var("DODS_Decimal_Year");
    string s = current.get(decimal);
    dods_decimal_year->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Decimal_Year", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_decimal_year, 0); // 0 == no BaseType args
}

/*************************** Decimal/Year functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

static bool
sel_dods_startdecimal_year(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date_Time current 
	= get_instance<DODS_Date_Time, DODS_StartDate_Time_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_decimal_year = (Str*)dds.var("DODS_StartDecimal_Year");
    string s = current.get(decimal);
    dods_decimal_year->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_startdecimal_year(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_StartDecimal_Year", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_startdecimal_year, 0); // 0 == no BaseType args
}

/*************************** Decimal/Year functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

static bool
sel_dods_enddecimal_year(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date_Time current 
	= get_instance<DODS_Date_Time, DODS_EndDate_Time_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_decimal_year = (Str*)dds.var("DODS_EndDecimal_Year");
    string s = current.get(decimal);
    dods_decimal_year->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_enddecimal_year(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_EndDecimal_Year", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_enddecimal_year, 0); // 0 == no BaseType args
}

/************************ DODS_StartDate functions *************************/

static bool
sel_dods_startdate(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date current = get_instance<DODS_Date, DODS_StartDate_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_StartDate.
    Str *dods_date = (Str*)dds.var("DODS_StartDate");
    // Calling the regular form of DODS_Date::get() returns the data in y/m/d
    // format. 5/27/99 jhrg
    string s = current.get().c_str();
    dods_date->val2buf(&s);

    return true;
}

static void
proj_dods_startdate(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    new_string_variable("DODS_StartDate", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_startdate, 0); // 0 == no BaseType args
}

/************************ DODS_StartTime functions *************************/

static bool
sel_dods_starttime(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Time current = get_instance<DODS_Time, DODS_StartTime_Factory>(dds);

    // Stuff the "hh:mm:ss" string into `DODS_Time'
    Str *dods_time = (Str*)dds.var("DODS_StartTime");
    string s = current.get().c_str();
    dods_time->val2buf(&s);

    return true;
}

static void
proj_dods_starttime(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_StartTime", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_starttime, 0); // 0 == no BaseType args
}

/*************************** StartDate/Time functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

static bool
sel_dods_startdate_time(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date_Time current 
	= get_instance<DODS_Date_Time, DODS_StartDate_Time_Factory>(dds);

    Str *dods_date_time = (Str*)dds.var("DODS_StartDate_Time");
    string s = current.get().c_str();
    dods_date_time->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_startdate_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_StartDate_Time", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_startdate_time, 0); // 0 == no BaseType args
}

/************************ DODS_EndDate functions *************************/

static bool
sel_dods_enddate(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date current = get_instance<DODS_Date, DODS_EndDate_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_EndDate.
    Str *dods_date = (Str*)dds.var("DODS_EndDate");
    // Calling the regular form of DODS_Date::get() returns the data in y/m/d
    // format. 5/27/99 jhrg
    string s = current.get().c_str();
    dods_date->val2buf(&s);

    return true;
}

static void
proj_dods_enddate(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    new_string_variable("DODS_EndDate", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_enddate, 0); // 0 == no BaseType args
}

/************************ DODS_EndTime functions *************************/

static bool
sel_dods_endtime(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Time current = get_instance<DODS_Time, DODS_EndTime_Factory>(dds);

    // Stuff the "hh:mm:ss" string into `DODS_Time'
    Str *dods_time = (Str*)dds.var("DODS_EndTime");
    string s = current.get().c_str();
    dods_time->val2buf(&s);

    return true;
}

static void
proj_dods_endtime(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_EndTime", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_endtime, 0); // 0 == no BaseType args
}

/*************************** EndDate/Time functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

static bool
sel_dods_enddate_time(int argc, BaseType *[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Date_Time current 
	= get_instance<DODS_Date_Time, DODS_EndDate_Time_Factory>(dds);

    Str *dods_date_time = (Str*)dds.var("DODS_EndDate_Time");
    string s = current.get().c_str();
    dods_date_time->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

static void
proj_dods_enddate_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_EndDate_Time", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    ce.append_clause(sel_dods_enddate_time, 0); // 0 == no BaseType args
}

void
ff_register_functions(ConstraintEvaluator &ce)
{
    ce.add_function("date", func_date);
    ce.add_function("date_range", func_date_range);
    ce.add_function("start_date", func_startdate);
    ce.add_function("end_date", func_enddate);
    ce.add_function("DODS_JDate", proj_dods_jdate);
    ce.add_function("DODS_Date", proj_dods_date);
    ce.add_function("DODS_StartDate", proj_dods_startdate);
    ce.add_function("DODS_EndDate", proj_dods_enddate);

    ce.add_function("time", func_time);
    ce.add_function("start_time", func_starttime);
    ce.add_function("end_time", func_endtime);
    ce.add_function("DODS_Time", proj_dods_time);
    ce.add_function("DODS_StartTime", proj_dods_starttime);
    ce.add_function("DODS_EndTime", proj_dods_endtime);

    ce.add_function("date_time", func_date_time);
    ce.add_function("start_date_time", func_startdate_time);
    ce.add_function("end_date_time", func_enddate_time);
    ce.add_function("DODS_Date_Time", proj_dods_date_time);
    ce.add_function("DODS_StartDate_Time", proj_dods_startdate_time);
    ce.add_function("DODS_EndDate_Time", proj_dods_enddate_time);

    // Use date() and date_time() comparison functions.
    ce.add_function("DODS_Decimal_Year", proj_dods_decimal_year);
    ce.add_function("DODS_StartDecimal_Year", proj_dods_startdecimal_year);
    ce.add_function("DODS_EndDecimal_Year", proj_dods_enddecimal_year);
}
