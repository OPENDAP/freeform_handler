
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// Implementation of the DODS Time class

// $Log: DODS_Time.cc,v $
// Revision 1.2  1998/12/30 06:38:26  jimg
// Define TEST to use this without the dap++ library (e.g., when testing
// DODS_Date_Time).
//
// Revision 1.1  1998/12/28 19:07:33  jimg
// Initial version of the DODS_Time object
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: DODS_Time.cc,v 1.2 1998/12/30 06:38:26 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <assert.h>

#include <string>
#include <String.h>
#include <strstream.h>
#include <iomanip.h>

#include "BaseType.h"
#include "DODS_Time.h"
#include "debug.h" 
#include "Error.h"

double DODS_Time::_eps = 1.0e-6;

static String time_syntax_string = \
"Invalid time: times must be given as hh:mm or hh:mm:ss with an optional\n\
suffix of GMT or UTC. In addition, 0 <= hh <=23, 0 <= mm <= 59 and\n\
0 <= ss <= 59.999999";

static inline double
compute_ssm(int hh, int mm, double ss)
{
    return ((hh * 60 + mm) * 60) + ss;
}

static string
extract_argument(BaseType *arg)
{
#ifndef TEST
    if (arg->type() != dods_str_c)
	throw Error(malformed_expr, "A DODS string argument is required.");
    
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

bool
DODS_Time::OK() const
{
    return _hours >= 0 && _hours <= 23
	&& _minutes >= 0 && _minutes <= 59
	&& _seconds >= 0.0 && _seconds < 60.0;
}

// Public mfincs.

// Don't test this ctor with OK since a null Time is not OK for use but we
// want to be able to make one and then call set_time(). 12/16/98 jhrg

DODS_Time::DODS_Time(): _hours(-1), _minutes(-1), _seconds(-1),
    _sec_since_midnight(-1), _gmt(false)
{
}

DODS_Time::DODS_Time(string time_str)
{
    set_time(time_str);
}

DODS_Time::DODS_Time(BaseType *arg)
{
    set_time(extract_argument(arg));
}

DODS_Time::DODS_Time(int hh, int mm, bool gmt = false):
    _hours(hh), _minutes(mm), _seconds(0), _gmt(gmt)
{
    _sec_since_midnight = compute_ssm(hh, mm, 0);
#ifndef TEST
    if (!OK())
	throw Error(malformed_expr, time_syntax_string);
#endif
}

DODS_Time::DODS_Time(int hh, int mm, double ss, bool gmt = false):
    _hours(hh), _minutes(mm), _seconds(ss), _gmt(gmt)
{
    _sec_since_midnight = compute_ssm(hh, mm, ss);
#ifndef TEST
    if (!OK())
	throw Error(malformed_expr, time_syntax_string);
#endif
}

void
DODS_Time::set_time(string time)
{
        // Parse the date_str.
    istrstream iss(time.data());
    char c;
    iss >> _hours;
    iss >> c;
    iss >> _minutes;

    // If there are two colons, assume hours:minutes:seconds.
    if (time.find(":") != time.rfind(":")) {
	iss >> c;
	iss >> _seconds;
    }
    
    _sec_since_midnight = compute_ssm(_hours, _minutes, _seconds);

    string gmt;
    iss >> gmt;
    if (gmt == "GMT" || gmt == "gmt" || gmt == "UTC" || gmt == "utc")
	_gmt = true;
    else
	_gmt = false;

#ifndef TEST
    if (!OK())
	throw Error(malformed_expr, time_syntax_string);
#endif
}    

void
DODS_Time::set_time(BaseType *arg)
{
    set_time(extract_argument(arg));
}    

void 
DODS_Time::set_time(int hh, int mm, bool gmt = false)
{
    set_time(hh, mm, 0, gmt);
}

void 
DODS_Time::set_time(int hh, int mm, double ss, bool gmt = false)
{
   _hours = hh;
   _minutes = mm;
   _seconds = ss;
   _gmt = gmt;
   _sec_since_midnight = compute_ssm(hh, mm, ss);

#ifndef TEST
    if (!OK())
	throw Error(malformed_expr, time_syntax_string);
#endif
}

double
DODS_Time::get_epsilon() const
{
    return _eps;
}

void
DODS_Time::set_epsilon(double eps)
{
    _eps = eps;
}

int
operator==(DODS_Time &t1, DODS_Time &t2)
{
    return t1.seconds_since_midnight() + t1._eps >= t2.seconds_since_midnight()
    && t1.seconds_since_midnight() - t2._eps <= t2.seconds_since_midnight();
}

int
operator!=(DODS_Time &t1, DODS_Time &t2)
{
    return !(t1 == t2);
}

// The relational ops > and < are possibly flaky. Note that the Intel machines
// and the Sparc machines *do* represent floating point numbers slightly
// differently. 

int
operator>(DODS_Time &t1, DODS_Time &t2)
{
    return t1.seconds_since_midnight() > t2.seconds_since_midnight();
}

int
operator>=(DODS_Time &t1, DODS_Time &t2)
{
    return t1 > t2 || t1 == t2;
}

int
operator<(DODS_Time &t1, DODS_Time &t2)
{
    return t1.seconds_since_midnight() < t2.seconds_since_midnight();
}

int
operator<=(DODS_Time &t1, DODS_Time &t2)
{
    return t1 < t2 || t1 == t2;
}

double
DODS_Time::seconds_since_midnight() const
{
    return _sec_since_midnight;
}

int
DODS_Time::hours() const
{
    return _hours;
}

int
DODS_Time::minutes() const
{
    return _minutes;
}

double
DODS_Time::seconds() const
{
    return _seconds;
}

bool
DODS_Time::gmt() const
{
    return _gmt;
}

string
DODS_Time::string_rep(bool gmt) const
{
    ostrstream oss;
    // Pad with leading zeros and use fixed fields of two chars for hours and
    // minutes. Make sure that seconds < 10 have a leading zero but don't
    // require their filed to have `precision' digits if they are all zero.
    oss << setfill('0') << setw(2) << _hours << ":" 
	<< setfill('0') << setw(2) << _minutes << ":" 
	<< setfill('0') << setw(2) << setprecision(6) << _seconds << " ";

    if (_gmt)
	oss << "GMT";
    
    oss << ends;

    string time_str = oss.str();
    oss.freeze(0);

    return time_str;
}
