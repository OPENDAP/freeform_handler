
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// Implementation of the DODS Date class

// $Log: DODS_Date.cc,v $
// Revision 1.1  1998/12/28 19:08:25  jimg
// Initial version of the DODS_Date object
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: DODS_Date.cc,v 1.1 1998/12/28 19:08:25 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <assert.h>

#include <strstream.h>

#include "DODS_Date.h"
#include "debug.h" 

// The Error class is defined in the core software. For testing we don't need
// this function and can supply a dummy version. That simplifies building the
// test code. 11/12/98 jhrg

#include "Error.h"

static string
extract_argument(BaseType *arg)
{
#ifndef TEST_DATE
    if (arg->type() != dods_str_c)
	throw Error(malformed_expr, 
	      "The Projection function requires a DODS string-type argument.");
    
    // Use String until conversion of String to string is complete. 9/3/98
    // jhrg
    String *sp = 0;
    arg->buf2val((void **)&sp);
    string s = sp->chars();
    delete sp;

    DBG(cerr << "s: " << s << endl);

    return s;
#else
    return "";
#endif
}

// Public member functions.

DODS_Date::DODS_Date(): _julian_day(0), _year(0), _month(0), _day(0),
    _day_number(0)
{
}

DODS_Date::DODS_Date(BaseType *arg)
{
    string s = extract_argument(arg);
    set_date(s);
}

DODS_Date::DODS_Date(string date_str)
{
    set_date(date_str);
}

DODS_Date::DODS_Date(int year, int day_num)
{
    set_date(year, day_num);
}

DODS_Date::DODS_Date(int year, int month, int day)
{
    set_date(year, month, day);
}    

void
DODS_Date::set_date(BaseType *arg)
{
    string s = extract_argument(arg);
    set_date(s);
}

void
DODS_Date::set_date(string date) 
{
    // Parse the date_str.
    istrstream iss(date.data());
    char c;
    iss >> _year;
    iss >> c;
    iss >> _month;

    // If there are two slashes, assume a yyyy/mm/dd date.
    if (date.find_first_of("/") != date.find_last_of("/")) {
	iss >> c;
	iss >> _day;
	// Convert to julian day number and record year, month, ...
	_julian_day = ::julian_day(_year, _month, _day);
	_day_number = month_day_to_days(_year, _month, _day);
    }
    else {
	// Note that when a `yyyy/ddd' date is read in, the day-number winds
	// up in the `_month' member.
	_day_number = _month;
	days_to_month_day(_year, _day_number, &_month, &_day);
	_julian_day = ::julian_day(_year, _month, _day);
    }
}

void
DODS_Date::set_date(int year, int day_num)
{
    _year = year;
    _day_number = day_num;
    days_to_month_day(_year, _day_number, &_month, &_day);
    _julian_day = ::julian_day(_year, _month, _day);
}

void
DODS_Date::set_date(int year, int month, int day)
{
    _year = year;
    _month = month;
    _day = day;
    _day_number = month_day_to_days(_year, _month, _day);
    _julian_day = ::julian_day(_year, _month, _day);
}

int 
operator==(DODS_Date &d1, DODS_Date &d2)
{
    return d1._julian_day == d2._julian_day ? 1: 0;
}

int
operator!=(DODS_Date &d1, DODS_Date &d2)
{
    return d1._julian_day != d2._julian_day ? 1: 0;
}

int
operator<(DODS_Date &d1, DODS_Date &d2)
{
    return d1._julian_day < d2._julian_day ? 1: 0;
}

int
operator>(DODS_Date &d1, DODS_Date &d2)
{
    return d1._julian_day > d2._julian_day ? 1: 0;
}

int
operator<=(DODS_Date &d1, DODS_Date &d2)
{
    return d1._julian_day <= d2._julian_day ? 1: 0;
}

int
operator>=(DODS_Date &d1, DODS_Date &d2)
{
    return d1._julian_day >= d2._julian_day ? 1: 0;
}

int 
DODS_Date::year() const
{
    return _year;
}

int 
DODS_Date::month() const
{
    return _month;
}

int 
DODS_Date::day() const
{
    return _day;
}

int 
DODS_Date::day_number() const
{
    return _day_number;
}

long 
DODS_Date::julian_day() const
{
    return _julian_day;
}

string 
DODS_Date::yd_date() const
{
    ostrstream oss;
    oss << _year << "/" << _day_number << ends;
    string yd = oss.str();
    oss.freeze(0);
    return yd;
}

string 
DODS_Date::ymd_date() const
{
    ostrstream oss;
    oss << _year << "/" << _month << "/" << _day << ends;
    string yd = oss.str();
    oss.freeze(0);
    return yd;
}


time_t
DODS_Date::unix_time() const
{
    struct tm tm_rec;
    tm_rec.tm_mday = _day;
    tm_rec.tm_mon = _month - 1; // zero-based 
    tm_rec.tm_year = _year - 1900; // years since 1900
    tm_rec.tm_hour = 0;
    tm_rec.tm_min = 0;
    tm_rec.tm_sec = 1;		// smallest time into the day
    tm_rec.tm_isdst = -1;

    return mktime(&tm_rec);
}

#ifdef TEST_DATE

// Call this with one, two or three args. If one arg, call the string ctor.
// If two or three args, use the yd or ymd ctor. Once built, compare to 1 Jan
// 1970 and then call the yd_date() and ymd_date() mfuncs. 11/4/98 jhrg

// Build with: `g++ -g -I../../include -DHAVE_CONFIG_H -DTEST_DATE
// DODS_Date.cc date_proc.o'. Add: `-ftest-coverage -fprofile-arcs' for test
// coverage. 

int main(int argc, char *argv[])
{
    DODS_Date epoc((string)"1970/1/1");
    DODS_Date d1;

    switch (--argc) {
      case 1:
	d1.set_date((string)argv[1]);
	break;
      case 2:
	d1.set_date(atoi(argv[1]), atoi(argv[2]));
	break;
      case 3:
	d1.set_date(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
	break;
      default:
	cerr << "Wrong number of args!" << endl;
	abort();
    }

    if (d1 < epoc)
	cout << "True: d1 < epoc" << endl;
    else
	cout << "False: d1 < epoc" << endl;

    if (d1 > epoc)
	cout << "True: d1 > epoc" << endl;
    else
	cout << "False: d1 > epoc" << endl;
    
    if (d1 <= epoc)
	cout << "True: d1 <= epoc" << endl;
    else
	cout << "False: d1 <= epoc" << endl;

    if (d1 >= epoc)
	cout << "True: d1 >= epoc" << endl;
    else
	cout << "False: d1 >= epoc" << endl;

    if (d1 == epoc)
	cout << "True: d1 == epoc" << endl;
    else
	cout << "False: d1 == epoc" << endl;

    if (d1 != epoc)
	cout << "True: d1 != epoc" << endl;
    else
	cout << "False: d1 != epoc" << endl;

    cout << "YMD: " << d1.ymd_date() << endl;
    cout << "YD: " << d1.yd_date() << endl;
    cout << "Julian day: " << d1.julian_day() << endl;
    cout << "Seconds: " << d1.unix_time() << endl;
}
#endif // TEST_DATE
