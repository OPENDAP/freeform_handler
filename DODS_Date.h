
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: DODS_Date.h,v $
// Revision 1.4  1999/01/08 22:08:18  jimg
// Fixed doc++ comments.
//
// Revision 1.3  1999/01/05 00:34:45  jimg
// Removed string class; replaced with the GNU String class. It seems those
// don't mix well.
// Switched to simpler method names.
// Added the date_format enumerated type.
//
// Revision 1.2  1998/12/30 02:01:12  jimg
// Added class invariant.
//
// Revision 1.1  1998/12/28 19:08:26  jimg
// Initial version of the DODS_Date object
//

#ifndef _dods_date_h
#define _dods_date_h

#ifdef __GNUG__
#pragma interface
#endif

#include <time.h>

#include <String.h>

#include "BaseType.h"
#include "date_proc.h"

/** Constants used to denote different supported date formats. #ymd# is a
    year---month---day format, #yd# is a year---year-day format. */

enum date_format {
    unknown_format,
    ymd,
    yd
};

/** The DODS Date object. This provides a way to translate between local
    representations of dates and the DODS standard representation(s). The
    DODS\_Date object provides constructors, accessors and comparison
    operations; DODS servers which support the DODS standard representation
    of dates must implement CE functions that make use of this object.
    
    @author James Gallagher */

class DODS_Date {
private:
    long _julian_day;
    int _year;
    int _month;
    int _day;
    int _day_number;

public:
    /** @name Constructors */

    //@{
    /** Create an empty date. Set the date using one of the #set_date#
	mfuncs.

	@see set_date() */
    DODS_Date();
    
    /** Build a DODS\_Date by parsing the string #date_str#. If #date_str# is
	of the form `yyyy/ddd' assume that it is in year and day-number
	format. If it is of the form `yyyy/mm/dd' assume it is in year, month
	and day format. Note that if `yyyy' is `98' that means year 98 A.D.,
	not 1998.

	@param date_str A string containing the date. */
    DODS_Date(String date_str);

    /** Build a DODS\_Date by parsing the DODS String contained in #arg#.

	Throws Error if #arg# is not a DODS Str object.

	@param arg A DODS String containing the date.
	@see DODS_Date(String). */
    DODS_Date(BaseType *arg);

    /** Build a DODS\_Date using year and day-number values. This constructor
	assumes that the two integers are the year and day-number,
	respectively.

	@param year The year. `98' is 98 A.D., not 1998. 
	@param day_num The day-number, 1 Jan is day 1. */
    DODS_Date(int year, int day_num);

    /** Build a DODS\_Date using year, month and day values. 

	@param year The year. As with the other constructors, does not prefix
	1900 to two digit years.
	@param month The month of the year; 1 == January, ..., 12 == December.
	@param day The day of the month; 1, ..., \{31, 30, 29, 28\}. */
    DODS_Date(int year, int month, int day);
    //@}

    /** @name Assignment */
    //@{
    /** Parse the string and assign the value to this object. 
	@see DODS_Date(string) */
    void set(String date);

    /** Parse the DODS String and assign the value to this object. 
	@see DODS_Date(BaseType *arg) */
    void set(BaseType *arg);

    /** Assign the date using the two integers.
	@see DODS_Date(int year, int day_number) */
    void set(int year, int day_number);

    /** Assign the date using three integers.
	@see DODS_Date(int year, int month, int day) */
    void set(int year, int month, int day);
    //@}

    /** @name Access */
    //@{
    /** Get the string representation for this date. By default the y/m/d
	format is used. To get the year/year-day format use #yd# for the
	value of #format#.
	
	Throws Error if #format# is not #ymd# or #yd#.

	@param format The format of the date.
	@see date_format.
	@return The date's string representation. */
    String get(date_format format = ymd) const;

    /** @return The year in years A.D. */
    int year() const;

    /** @return The month of the year (1 == January, ..., 12 == December). */
    int month() const;

    /** @return The day of the month (1, ... \{28, 29, 30, 31\}). */
    int day() const;

    /** @return The day-number of the year (1 == 1 Jan). */
    int day_number() const;

    /** @return The Julian day number for this date. */
    long julian_day() const;

    /** Return the number of seconds since 00:00:00 UTC 1 Jan 1970. If the
	date is before 1 Jan 1970, return DODS\_UINT\_MAX. If the date is too
	late to represent as seconds since 1 Jan 1970, return
	DODS\_UINT\_MAX. Each day starts at 00:00:00 UTC.

	@return The date in seconds since 1 Jan 1970.
        @see dods-limits.h
	@see time.h
 	@see mktime(3) */
    time_t unix_time() const;
    //@}

    /** @name Relational operators */
    //@{
    /// Equality
    friend int operator==(DODS_Date &d1, DODS_Date &d2);

    /// Inequality
    friend int operator!=(DODS_Date &d1, DODS_Date &d2);

    /// Less than
    friend int operator<(DODS_Date &d1, DODS_Date &d2);

    /// Greater than
    friend int operator>(DODS_Date &d1, DODS_Date &d2);

    /// Less than or equal
    friend int operator<=(DODS_Date &d1, DODS_Date &d2);

    /// Greater than or equal
    friend int operator>=(DODS_Date &d1, DODS_Date &d2);
    //@}

    /** Class invariant.

	@return True for a valid instance, otherwise false. */
    bool OK() const;
};

#endif // _dods_date_h
