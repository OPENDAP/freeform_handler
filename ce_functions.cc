
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains various functions for use with/in constraint
// expressions. 

// $Log: ce_functions.cc,v $
// Revision 1.3  1998/11/13 05:40:02  jimg
// Added many DBG() lines.
//
// Revision 1.2  1998/11/10 17:53:45  jimg
// Recoded the projection and selection date functions for jdate. Added new
// functions for ymd dates and changed the overall organization a bit; it now
// tries to isolate the information specific to a particular dataset and it now
// uses the new DODS_Date objects for date conversions.
//
// Revision 1.1  1998/09/17 17:44:14  jimg
// Created
//

#include <iostream>
#include <strstream>
#include <string>
#include <algo.h>

#include <String.h>

#include "BaseType.h"
#include "Str.h"
#include "Structure.h"
#include "Sequence.h"
#include "FFStr.h"
#include "DDS.h"
#include "Error.h"

#include "date_proc.h"
#include "DODS_Date.h"
#include "debug.h"

// Special for the JPL Pathfinder dataset. Read the sequence variables and
// build a DODS_Date object.

DODS_Date
DODS_Date_jpl_pathfinder(DDS &dds)
{
    BaseType *day_var = dds.var("JPL_Pathfinder.day");
    int day_number;
    int *day_number_p = &day_number;
    day_var->buf2val((void **)&day_number_p);

    BaseType *year_var = dds.var("JPL_Pathfinder.year");
    int year;
    int *year_p = &year;
    year_var->buf2val((void **)&year_p);

    DODS_Date d(year, day_number);

    DBG(cerr << "year: " << year << " day number: " << day_number << endl);

    return d;
}

// Select sequence elements based on date. The sequence is assumed to have
// year, month and day information accessible.
//
// Date uses the following notation: yyyy/ddd, yyyy/mm/dd.
//
// Semantics: One argument: return true for dates that match exactly.
// Two arguments: return true for dates that fall within the (inclusive)
// interval. 
//
// Algorithm: Convert dates (arguments) to days since 1 AD and compare to
// dates from the sequence similarly converted.
//
// 9/3/98 jhrg

bool
func_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 1 || argc > 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a projection function.");

    DODS_Date d1(argv[0]);
    DODS_Date d2;
    if (argc == 2) {
	d2.set_date(argv[1]);
    }

    DODS_Date current = DODS_Date_jpl_pathfinder(dds);

    DBG(cerr << "Comp dates: " << d1.yd_date() << " " << current.yd_date()\
	<< " " << d2.yd_date() << endl);

    bool res;
    if (argc == 2)
	res = d2 >= current && d1 <= current;
    else
	res = d1 == current;

    DBG(cerr << "Result: " << res << endl);
    return res;
}

bool
sel_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    DODS_Date new_date = DODS_Date_jpl_pathfinder(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_jdate = (Str*)dds.var("DODS_JDate");
    String s = new_date.yd_date().data();
    dods_jdate->val2buf(&s);

    DBG2(dods_jdate->print_val(cerr, "    ", true));
    DBG2(cerr << "Exiting (" << dods_jdate->name() << " [" \
	<< dods_jdate << "])" << endl);

    return true;
}

void
proj_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    Str *dods_jdate = new FFStr("DODS_JDate");
    dods_jdate->set_read_p(true); // You must call this before ...
    dods_jdate->set_synthesized_p(true); // this! Look at BaseType.cc.

    // Add it to the DDS in the right place

    if (argc == 1 && argv[0]) {
	switch (argv[0]->type()) {
	  case dods_structure_c: {
	      Structure *sp = (Structure *)argv[0];
	      sp->add_var((BaseType *)dods_jdate);
	      break;
	  }

	  case dods_sequence_c: {
	      Sequence *sp = (Sequence *)argv[0];
	      sp->add_var((BaseType *)dods_jdate);
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

    // Mark the variable as part of the current projection.

    dds.mark("DODS_JDate", true); // Don't just call set_send_p()!

    // Add the selection function to the CE

    dds.append_clause(sel_dods_jdate, 0); // 0 == no BaseType args
}

bool
sel_dods_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    DODS_Date new_date = DODS_Date_jpl_pathfinder(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_date = (Str*)dds.var("DODS_Date");
    String s = new_date.ymd_date().data();
    dods_date->val2buf(&s);

    DBG2(dods_date->print_val(cerr, "    ", true));
    DBG2(cerr << "Exiting (" << dods_date->name() << " [" \
	<< dods_date << "])" << endl);

    return true;
}

void
proj_dods_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    Str *dods_date = new FFStr("DODS_Date");
    dods_date->set_read_p(true); // You must call this before ...
    dods_date->set_synthesized_p(true); // this! Look at BaseType.cc.

    // Add it to the DDS in the right place

    if (argc == 1 && argv[0]) {
	switch (argv[0]->type()) {
	  case dods_structure_c: {
	      Structure *sp = (Structure *)argv[0];
	      sp->add_var((BaseType *)dods_date);
	      break;
	  }

	  case dods_sequence_c: {
	      Sequence *sp = (Sequence *)argv[0];
	      sp->add_var((BaseType *)dods_date);
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

    // Mark the variable as part of the current projection.

    dds.mark("DODS_Date", true); // Don't just call set_send_p()!

    // Add the selection function to the CE

    dds.append_clause(sel_dods_date, 0); // 0 == no BaseType args
}
