
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// Implementation of the DODS Date/Time class

// $Log: DODS_Date_Time.cc,v $
// Revision 1.1  1998/12/30 06:40:39  jimg
// Initial version
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: DODS_Date_Time.cc,v 1.1 1998/12/30 06:40:39 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <assert.h>

#include <strstream.h>

#include "Error.h"
#include "DODS_Date_Time.h"
#include "debug.h" 

#define seconds_per_day 86400.0

static string
extract_argument(BaseType *arg)
{
#ifndef TEST
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

// Public mfuncs

bool
DODS_Date_Time::OK() const
{
    return _time.OK() && _date.OK();
}

DODS_Date_Time::DODS_Date_Time()
{
}


DODS_Date_Time::DODS_Date_Time(DODS_Date d, DODS_Time t) : _date(d), _time(t)
{
}

DODS_Date_Time::DODS_Date_Time(string date_time)
{
    set_date_time(date_time);
}

DODS_Date_Time::DODS_Date_Time(BaseType *date_time)
{
    set_date_time(date_time);
}

DODS_Date_Time::DODS_Date_Time(int y, int m, int d, int hh, int mm, 
			       double ss, bool gmt)
{
    set_date_time(y, m, d, hh, mm, ss, gmt);
}

DODS_Date_Time::DODS_Date_Time(int y, int yd, int hh, int mm, double ss, 
			       bool gmt)
{
    set_date_time(y, yd, hh, mm, ss, gmt);
}

void
DODS_Date_Time::set_date_time(DODS_Date d, DODS_Time t)
{
    _date = d;
    _time = t;
    
    assert(OK());
}

void
DODS_Date_Time::set_date_time(string date_time)
{
    // The format for the date-time string is <date part>:<time part>.
    int i = date_time.find(":");
    string date_part = date_time.substr(0, i);
    string time_part = date_time.substr(i+1);
 
   _date.set_date(date_part);
    _time.set_time(time_part);

    assert(OK());
}

void
DODS_Date_Time::set_date_time(BaseType *date_time)
{
    set_date_time(extract_argument(date_time));
}

void
DODS_Date_Time::set_date_time(int y, int m, int d, int hh, int mm, double ss, 
			      bool gmt)
{
    _date.set_date(y, m, d);
    _time.set_time(hh, mm, ss, gmt);

    assert(OK());
}

void
DODS_Date_Time::set_date_time(int y, int yd, int hh, int mm, double ss, 
			      bool gmt)
{
    _date.set_date(y, yd);
    _time.set_time(hh, mm, ss, gmt);

    assert(OK());
}

int
DODS_Date_Time::year() const
{
    return _date.year();
}

int
DODS_Date_Time::month() const
{
    return _date.month();
}

int
DODS_Date_Time::day() const
{
    return _date.day();
}

int
DODS_Date_Time::day_number() const
{
    return _date.day_number();
}

int
DODS_Date_Time::hours() const
{
    return _time.hours();
}

int
DODS_Date_Time::minutes() const
{
    return _time.minutes();
}

double
DODS_Date_Time::seconds() const
{
    return _time.seconds();
}

bool
DODS_Date_Time::gmt() const
{
    return _time.gmt();
}

string
DODS_Date_Time::yd_date() const
{
    return _date.yd_date();
}

string
DODS_Date_Time::ymd_date() const
{
    return _date.ymd_date();
}

string
DODS_Date_Time::yd_date_time() const
{
    return _date.yd_date() + ":" + _time.string_rep();
}

string
DODS_Date_Time::ymd_date_time() const
{
    return _date.ymd_date() + ":" + _time.string_rep();
}

double
DODS_Date_Time::julian_day() const
{
    return _date.julian_day() + _time.seconds_since_midnight()/seconds_per_day;
}

time_t 
DODS_Date_Time::unix_time() const
{
    struct tm tm_rec;
    tm_rec.tm_mday = _date.day();
    tm_rec.tm_mon = _date.month() - 1; // zero-based 
    tm_rec.tm_year = _date.year() - 1900; // years since 1900
    tm_rec.tm_hour = _time.hours();
    tm_rec.tm_min = _time.minutes();
    tm_rec.tm_sec = (int)_time.seconds();
    tm_rec.tm_isdst = -1;

    return mktime(&tm_rec);
}

double
DODS_Date_Time::get_epsilon() const
{
    return _time.get_epsilon();
}

void
DODS_Date_Time::set_epsilon(double eps)
{
    _time.set_epsilon(eps);
}

int
operator==(DODS_Date_Time &t1, DODS_Date_Time &t2)
{
    return t1._date == t2._date && t1._time == t2._time;
}

int
operator!=(DODS_Date_Time &t1, DODS_Date_Time &t2)
{
    return t1._date != t2._date || t1._time != t2._time;
}

int
operator<(DODS_Date_Time &t1, DODS_Date_Time &t2)
{
    return t1._date < t2._date || t1._time < t2._time;
}

int
operator>(DODS_Date_Time &t1, DODS_Date_Time &t2)
{
    return t1._date > t2._date || t1._time > t2._time;
}

int
operator<=(DODS_Date_Time &t1, DODS_Date_Time &t2)
{
    return t1._date < t2._date && t1._time <= t2._time;
}

int
operator>=(DODS_Date_Time &t1, DODS_Date_Time &t2)
{
    return t1._date > t2._date && t1._time >= t2._time;
}

#ifdef DATE_TIME_TEST

/* Input args: 1 String,
    2 Two strings, 
    5 y, yd, hh, mm, ss,
    6 y, m, d, hh, mm, ss 
    
    Compile using: g++ -g -I../../include -DHAVE_CONFIG_H -DTEST
    -DDATE_TIME_TEST DODS_Date.cc DODS_Time.cc DODS_Date_Time.cc date_proc.cc
    -lg++ 
*/

int
main(int argc, char *argv[])
{
    DODS_Date_Time dt;
    DODS_Date_Time dt2("1970/1/1:0:0:0");

    argc--;
    switch(argc) {
      case 1:
	dt.set_date_time(argv[1]);
	break;
      case 2:
	dt.set_date_time(argv[1]);
	dt2.set_date_time(argv[2]);
	break;
      case 5:
	dt.set_date_time(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), 
			 atoi(argv[4]), atof(argv[5]));
	break;
      case 6:
	dt.set_date_time(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), 
			 atoi(argv[4]), atoi(argv[5]), atof(argv[6]));
	break;
      default:
	cerr << "Wrong number of arguments!" << endl;
	exit(1);
    }
	
    if (dt < dt2)
	cout << "True: dt < dt2" << endl;
    else
	cout << "False: dt < dt2" << endl;

    if (dt > dt2)
	cout << "True: dt > dt2" << endl;
    else
	cout << "False: dt > dt2" << endl;
    
    if (dt <= dt2)
	cout << "True: dt <= dt2" << endl;
    else
	cout << "False: dt <= dt2" << endl;

    if (dt >= dt2)
	cout << "True: dt >= dt2" << endl;
    else
	cout << "False: dt >= dt2" << endl;

    if (dt == dt2)
	cout << "True: dt == dt2" << endl;
    else
	cout << "False: dt == dt2" << endl;

    if (dt != dt2)
	cout << "True: dt != dt2" << endl;
    else
	cout << "False: dt != dt2" << endl;

    cout << "YMD: " << dt.ymd_date_time() << endl;
    cout << "YD: " << dt.yd_date_time() << endl;
    cout << "Julian day: " << dt.julian_day() << endl;
    cout << "Seconds: " << dt.unix_time() << endl;
}
#endif // TEST_DATE
   

