
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains various functions for use with/in constraint
// expressions. 

// $Log: ce_functions.cc,v $
// Revision 1.5  1998/12/31 17:59:18  jimg
// Changes to match the new (shorter) names in the DODS_Date, _Time and
// _Date_Time classes along with the associated _Factory classes.
// Added a template function for comparison.
//
// Revision 1.4  1998/12/15 19:38:25  jimg
// *** empty log message ***
//
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
#include "DDS.h"
#include "Error.h"
#include "util.h"
#include "debug.h"

#include "date_proc.h"
#include "DODS_Date.h"
#include "DODS_Date_Factory.h"
#include "DODS_Time.h"
#include "DODS_Time_Factory.h"

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

// Select sequence elements based on date and time. The sequence is assumed
// to have year, month, day and time information accessible.
//
// Date uses the following notation: yyyy/ddd:hh:mm:ss, yyyy/mm/dd:hh:mm:ss
//
// Semantics: One argument: return true for date/times that match exactly.
// Two arguments: return true for date/times that fall within the (inclusive)
// interval. 
//
// Algorithm: Use DODS_Date_Time objects.
//
// 12/30/98 jhrg

template<class T, class T_Factory>
bool
comparison(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 1 || argc > 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a constraint expression function.");

    T t1(argv[0]);
    T t2;
    if (argc == 2) {
	t2.set(argv[1]);
    }

    static T_Factory *tf = 0;
    if (!tf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	tf = new T_Factory(dds, das);
    }

    T current = tf->get();

    bool res;
    if (argc == 2)
	res = t2 >= current && t1 <= current;
    else
	res = t1 == current;

    return res;
}

bool
func_date(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date, DODS_Date_Factory>(argc, argv, dds);
}

#if 0
bool
func_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 1 || argc > 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a constraint expression function.");

    DODS_Date d1(argv[0]);
    DODS_Date d2;
    if (argc == 2) {
	d2.set_date(argv[1]);
    }

    static DODS_Date_Factory *ddf = 0;
    if (!ddf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	ddf = new DODS_Date_Factory(dds, das);
    }

    DODS_Date current = ddf->get_date();

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
#endif

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

bool
sel_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    static DODS_Date_Factory *ddf = 0;
    if (!ddf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	ddf = new DODS_Date_Factory(dds, das);
    }

    DODS_Date current = ddf->get();

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_jdate = (Str*)dds.var("DODS_JDate");
    String s = current.get(yd).chars();
    dods_jdate->val2buf(&s);

    DBG2(dods_jdate->print_val(cerr, "    ", true));
    DBG2(cerr << "Exiting (" << dods_jdate->name() << " [" \
	<< dods_jdate << "])" << endl);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

void
proj_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    Str *dods_jdate = NewStr("DODS_JDate");
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

// Same as the above function, but for ymd dates.

bool
sel_dods_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    static DODS_Date_Factory *ddf = 0;
    if (!ddf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	ddf = new DODS_Date_Factory(dds, das);
    }

    DODS_Date current = ddf->get();

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_date = (Str*)dds.var("DODS_Date");
    String s = current.get().chars();
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

    Str *dods_date = NewStr("DODS_Date");
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

/************************ DODS_Time functions *************************/

bool
func_time(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 1 || argc > 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a constraint expression function.");

    DODS_Time t1(argv[0]);
    DODS_Time t2;
    if (argc == 2) {
	t2.set(argv[1]);
    }

    static DODS_Time_Factory *dtf = 0;
    if (!dtf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	dtf = new DODS_Time_Factory(dds, das);
    }

    DODS_Time current = dtf->get();

    DBG(cerr << "Comp times: " << t1.get() << " " \
	<< current.get() << " " << d2.get() << endl);

    bool res;
    if (argc == 2)
	res = t2 >= current && t1 <= current;
    else
	res = t1 == current;

    DBG(cerr << "Result: " << res << endl);
    return res;
}

// Read a time from the dataset. This uses the customized function
// DODS_Time_gso_avhrr() which knows how to build a time object from
// information in a particular instance of the gso_avhrr file server
// database. The time information is then stuffed into the new variable
// that was created by the proj_dods_time() function (which must have been
// invoked by DODS_Time(...) in the projection part of a CE. That function
// created the new variable and arranged for sel_dods_time to be called when
// evaluating the selection part of the CE. Messy. 12/16/98 jhrg

bool
sel_dods_time(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    static DODS_Time_Factory *dtf = 0;
    if (!dtf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	dtf = new DODS_Time_Factory(dds, das);
    }

    DODS_Time new_time = dtf->get();

    // Stuff the "hh:mm:ss" string into `DODS_Time'
    Str *dods_time = (Str*)dds.var("DODS_Time");
    String s = new_time.get().chars();
    dods_time->val2buf(&s);

    DBG2(dods_time->print_val(cerr, "    ", true));
    DBG2(cerr << "Exiting (" << dods_time->name() << " [" \
	<< dods_time << "])" << endl);

    return true;
}

void
proj_dods_time(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    Str *dods_time = NewStr("DODS_Time");
    dods_time->set_read_p(true); // You must call this before ...
    dods_time->set_synthesized_p(true); // this! Look at BaseType.cc.

    // Add it to the DDS in the right place

    if (argc == 1 && argv[0]) {
	switch (argv[0]->type()) {
	  case dods_structure_c: {
	      Structure *sp = (Structure *)argv[0];
	      sp->add_var((BaseType *)dods_time);
	      break;
	  }

	  case dods_sequence_c: {
	      Sequence *sp = (Sequence *)argv[0];
	      sp->add_var((BaseType *)dods_time);
	      break;
	  }

	  default:
	throw Error(malformed_expr,
"You asked me to insert the synthesized variable in \n\
something that did not exist or was not a constructor \n\
type (i.e., a structure or sequence.");
	    break;
	}
    }

    // Mark the variable as part of the current projection.

    dds.mark("DODS_Time", true); // Don't just call set_send_p()!

    // Add the selection function to the CE

    dds.append_clause(sel_dods_time, 0); // 0 == no BaseType args
}

/*************************** Date/Time functions *************************/

#if 0
bool
func_date_time(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 1 || argc > 2)
	throw Error(malformed_expr,
		    "Wrong number of arguments to a constraint expression function.");

    DODS_Date_Time dt1(argv[0]);
    DODS_Date_Time dt2;
    if (argc == 2) {
	dt2.set_date_time(argv[1]);
    }

    static DODS_Date_Time_Factory *ddtf = 0;
    if (!ddtf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	ddtf = new DODS_Date_Time_Factory(dds, das);
    }

    DODS_Date_Time current = ddtf->get_date_time();

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

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

bool
sel_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    static DODS_Date_Factory *ddf = 0;
    if (!ddf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	ddf = new DODS_Date_Factory(dds, das);
    }

    DODS_Date current = ddf->get_date();

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_jdate = (Str*)dds.var("DODS_JDate");
    String s = current.yd_date().chars();
    dods_jdate->val2buf(&s);

    DBG2(dods_jdate->print_val(cerr, "    ", true));
    DBG2(cerr << "Exiting (" << dods_jdate->name() << " [" \
	<< dods_jdate << "])" << endl);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

void
proj_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    Str *dods_jdate = NewStr("DODS_JDate");
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

// Same as the above function, but for ymd dates.

bool
sel_dods_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DBG2(cerr << "Entering" << endl);

    static DODS_Date_Factory *ddf = 0;
    if (!ddf) {
	// Hack
	String name = dds.filename();
	DAS das;
	das.parse(name + ".das-fs");
	// end hack
	ddf = new DODS_Date_Factory(dds, das);
    }

    DODS_Date current = ddf->get_date();

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_date = (Str*)dds.var("DODS_Date");
    String s = current.ymd_date().chars();
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

    Str *dods_date = NewStr("DODS_Date");
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

#endif
