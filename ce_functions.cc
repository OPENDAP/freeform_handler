
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains various functions for use with/in constraint
// expressions. 

// $Log: ce_functions.cc,v $
// Revision 1.1  1998/09/17 17:44:14  jimg
// Created
//

#include <iostream>
#include <strstream>
#include <string>
#include <algo.h>

#include <String.h>

#include "BaseType.h"
#include "DDS.h"
#include "date_proc.h"

static string
extract_argument(BaseType *arg)
{
#if 0
    if (arg->type() != dods_string_c)
	throw wrong_argument_type;
#endif
    
    // Use String until conversion of String to string is complete. 9/3/98
    // jhrg
    String *sp = 0;
    arg->buf2val((void **)&sp);
    string s = (const char *)(*sp);
    delete sp;

    cerr << "Argument value: `" << s << "'" << endl;
    
    return s;
}

// Parse the following notations: yyyy/ddd, yyyy/mm/dd, yyyymmdd, yyyyddd.
//
// Return the Julian day.

static long
get_julian_day(string date)
{
    int year, month, day;

    // Does the date have slashes?
    if (date.find("/") != string::npos) {
	istrstream iss(date.c_str());
	char slash;

	iss >> year;
	iss >> slash;
	iss >> month;		// might read month or day, check below
	iss >> slash;
	if (slash && iss) {	// must have month and days
	    iss >> day;
	}
	else {			// must have only year and day
	    day = month;
	    days_to_month_day(year, day, &month, &day);
	}
	
	return julian_day(year, month, day);
    }
    else {			// no slashes
	assert("Non-slash version of date not implemented" && false);
    }
}

// This function reads variables from the current instance of a sequence and,
// from them, computes the Julian day. That value is then used in date_func as
// part of the comparison with the user-supplied date or date range.

static long
read_julian_day(DDS &dds)
{
    BaseType *day_var = dds.var("JPL_Pathfinder.day");
    int day_number;
    int *day_number_p = &day_number;
    day_var->buf2val((void **)&day_number_p);

    BaseType *year_var = dds.var("JPL_Pathfinder.year");
    int year;
    int *year_p = &year;
    year_var->buf2val((void **)&year_p);

    int month;			// The jpl pathfinder catalog uses year/day.

    days_to_month_day(year, day_number, &month, &day_number);

    return julian_day(year, month, day_number);
}

// Select sequence elements based on date. The sequence is assumed to have
// year, month and day information accessible.
//
// Date uses the following notation: yyyyddd, yyyy/ddd, yyddd, yy/ddd, 
// yyyymmdd, yyyy/mm/dd, yymmdd, yy/mm/dd.
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
    string date1, date2 = "";
    long days1, days2 = 0;

#if 0
    if (argc < 1 || argc > 2)
	throw wrong_number_of_arguments;
#endif

    date1 = extract_argument(argv[0]);
    days1 = get_julian_day(date1);
    if (argc == 2) {
	date2 = extract_argument(argv[1]);
	days2 = get_julian_day(date2);
    }

    cerr << date1 << ": " << days1 << endl;
    if (argc == 2) 
	cerr << date2 << ": " << days2 << endl;

    long jday = read_julian_day(dds);
    cerr << "jday: " << jday << endl;

    bool res;
    if (argc == 2)
	res = days2 >= jday && days1 <= jday;
    else
	res = days1 == jday;

    cerr << "Result: " << res << endl;
    return res;
}
