
// (c) COPYRIGHT URI/MIT 1998-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains various functions for use with/in constraint
// expressions. 

// $Log: ce_functions.cc,v $
// Revision 1.12  1999/07/22 21:28:09  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.11  1999/07/22 20:50:40  jimg
// Resolved conflicts from merge. I've kept the old (I think) versions of the
// decimal year functions but wrapped them in #if 0 #endif.
//
// Revision 1.10  1999/05/28 17:17:34  dan
// Added DODS_Decimal_Year server-side definitions.
//
// Revision 1.9.2.5  1999/06/04 19:38:34  dan
// Changed name.data() to name.c_str() in new_string_variable, this fixed
// projection problem for synthesized variables.
//
// Revision 1.9.2.4  1999/06/04 15:19:25  dan
// Changed DODS_DYear to DODS_Decimal_Year in sel_dods_decimal_year.  Should
// have been done with the change to proj_dods_decimal_year, oops.
//
// Revision 1.9.2.3  1999/06/04 14:50:01  dan
// Changed syntehsized variable DODS_DYear back to DODS_Decimal_Year
//
// Revision 1.9.2.2  1999/06/02 15:36:18  jimg
// removed errant #if 0 ... #endif
//
// Revision 1.9.2.1  1999/06/01 15:35:10  jimg
// Changed the proj and sel functions for decimal dates (DODS_DYear) so that
// they access DODS_Date and DODS_Date_Time objects using the get member
// function with the `decimal' token. This works because I also changed those
// classes to parse floating point date-time representations (using code from
// Dan's Decimal_Year class).
//
// Revision 1.9  1999/05/28 16:33:10  jimg
// Added/fixed comments.
//
// Revision 1.8  1999/05/04 02:55:37  jimg
// Merge with no-gnu
//
// Revision 1.7  1999/03/17 23:26:11  jimg
// get_instance() now uses the find_ancillary_file() function. This should help
// to standardize things, although it is still a hack to read the DAS at this
// point.
//
// Revision 1.6.8.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.6  1999/01/05 00:46:13  jimg
// Switched to a template design for some of the CE selection functions.
// There's more to do here, but its OK...
//
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

#include "BaseType.h"
#include "Str.h"
#include "Structure.h"
#include "Sequence.h"
#include "DDS.h"
#include "Error.h"
#include "util.h"
#include "cgi_util.h"
#include "debug.h"

#include "date_proc.h"
#include "DODS_Date.h"
#include "DODS_Date_Factory.h"
#include "DODS_Time.h"
#include "DODS_Time_Factory.h"
#include "DODS_Date_Time.h"
#include "DODS_Date_Time_Factory.h"
#if 0
#include "DODS_Decimal_Year.h"
#include "DODS_Decimal_Year_Factory.h"
#endif

/** Read an instance of T using a Factory for objects of type T. The Factory
    class for T must read configuration information from the DAS.

    @return An instance of the class T. */

template<class T, class T_Factory>
T
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
bool
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

    if (argc == 2)
	return (t1 <= current) && (t2 >= current);
    else
	return (t1 == current);
}

/** Load a new Str variable into the DDS. If position is given, then insert
    the new Str variable into that Structure or Sequence. If position is
    given and is not a Structure or Sequence, throw an exception.

    @param name The name of the new variable.
    @param dds The DDS of the dataset.
    @param position Add the new variable to this Structure or Sequence. */

void
new_string_variable(const string &name, DDS &dds, BaseType *position = 0)
{
    // Create the new variable

    Str *new_variable = NewStr(name.c_str());
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

bool
func_date(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date, DODS_Date_Factory>(argc, argv, dds);
}

bool
func_time(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Time, DODS_Time_Factory>(argc, argv, dds);
}

// This comparision function should be used for decimal dates. 5/29/99 jhrg

bool
func_date_time(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Date_Time, DODS_Date_Time_Factory>(argc, argv, dds);
}

#if 0
bool
func_decimal_year(int argc, BaseType *argv[], DDS &dds)
{
    return comparison<DODS_Decimal_Year, DODS_Decimal_Year_Factory>(argc, argv, dds);
}
#endif

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

// The date and date_time functions should now recognize decimal format years
// and date-times. 5/30/99 jhrg

bool
sel_dods_jdate(int argc, BaseType *argv[], DDS &dds)
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

void
proj_dods_jdate(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    new_string_variable("DODS_JDate", dds, (argc == 1) ? argv[0] : 0);

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
  
    DODS_Date current = get_instance<DODS_Date, DODS_Date_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_Date.
    Str *dods_date = (Str*)dds.var("DODS_Date");
    // Calling the regular form of DODS_Date::get() returns the data in y/m/d
    // format. 5/27/99 jhrg
    string s = current.get().c_str();
    dods_date->val2buf(&s);

    return true;
}

void
proj_dods_date(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    new_string_variable("DODS_Date", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    dds.append_clause(sel_dods_date, 0); // 0 == no BaseType args
}

/************************ DODS_Time functions *************************/


bool
sel_dods_time(int argc, BaseType *argv[], DDS &dds)
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

void
proj_dods_time(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
		  "Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Time", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    dds.append_clause(sel_dods_time, 0); // 0 == no BaseType args
}

/*************************** Date/Time functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

bool
sel_dods_date_time(int argc, BaseType *argv[], DDS &dds)
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

void
proj_dods_date_time(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Date_Time", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    dds.append_clause(sel_dods_date_time, 0); // 0 == no BaseType args
}



/*************************** Decimal/Year functions *************************/

#if 0

// NB: I'm not abosoutely sure which version of these we should be using.
// I'll leave these here just in case my guess is wrong. 7/22/99 jhrg

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

bool
sel_dods_decimal_year(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 0)
	throw Error(malformed_expr,
		  "Wrong number of arguments to internal selection function.\n\
Please report this error.");
  
    DODS_Decimal_Year current 
	= get_instance<DODS_Decimal_Year, DODS_Decimal_Year_Factory>(dds);

    // Stuff the yyyy/ddd string into DODS_JDate.
    Str *dods_decimal_year = (Str*)dds.var("DODS_Decimal_Year");
    string s = current.get().c_str();
    dods_decimal_year->val2buf(&s);

    return true;
}

// A projection function: This adds a new variable to the DDS and arranges
// for the matching selection function (above) to be called.

void
proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Decimal_Year", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    dds.append_clause(sel_dods_decimal_year, 0); // 0 == no BaseType args
}
#endif

/*************************** Decimal/Year functions *************************/

// This function is added to the selection part of the CE when the matching
// `projection function' is run. 

bool
sel_dods_decimal_year(int argc, BaseType *argv[], DDS &dds)
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

void
proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 0 || argc > 1)
	throw Error(malformed_expr,
"Wrong number of arguments to projection function.\n\
Expected zero or one arguments.");

    // Create the new variable

    new_string_variable("DODS_Decimal_Year", dds, (argc == 1) ? argv[0] : 0);

    // Add the selection function to the CE

    dds.append_clause(sel_dods_decimal_year, 0); // 0 == no BaseType args
}
