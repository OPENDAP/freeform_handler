
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: DODS_Date.h,v $
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

#if 0
#include <string>
#endif

#include <String.h>
#define string String
#define data() chars()
#define find_first_of(x) index((x))
#define find_last_of(x) index((x), -1)

#include "BaseType.h"
#include "date_proc.h"

/** The DODS Date object. This provides a way to translate between local
    representations of dates and the DODS standard representation(s). The
    DODS\_Date object provides ctors, accessors and comparision operations;
    DODS servers must implement CE functions that make use of this object.
    
    @author James Gallagher */

class DODS_Date {
private:
    long _julian_day;
    int _year;
    int _month;
    int _day;
    int _day_number;

    /// Class invariant.
    bool OK() const;

public:
    /** @name Constructors */

    //@{
    /** Create an empty date. Set to a date using one of the #set_date#
	mfuncs.
	@see set_date() */
    DODS_Date();
    
    /** Build a DODS\_Date by parsing the string #date_str#. If #date_str#
	is of the form `yyyy/ddd' assume that it is a year and day-number
	format date. If it is of the form `yyyy/mm/dd' assume it is a year,
	month and day format. Note that if `yyyy' is `98' that means year 98
	A.D., not 1998. If you want 1998, you need to say so.

	Throws an Error for dates that don't parse and for dates that parse
	to invalid dates.  
    
	@param date_str A string containing the date. */
    DODS_Date(string date_str);

    /** @param arg A DODS String containing the date.
	@see DODS_Date(string). */
    DODS_Date(BaseType *arg);

    /** Build a DODS\_Date using year and day-numbers values. This ctor
	assumes that the two integers are the year and day-number,
	respectively. Throws an Error for dates that cannot exist (e.g.,
	1998/400). 
 
	@param year The year. `98' is 98 A.D., \emph{not} 1998. 
	@param day_num The day-number, 1 Jan is day 1. */
    DODS_Date(int year, int day_num);

    /** Build a DODS\_Date using year, month and day numbers. Throws an Error
	object for dates that cannot exist (e.g., the 30th of February).
	

	@param year The year. As with the other ctors, does not prefix 1900
	to two digit years.
	@param month The month of the year; 1 == January, ..., 12 == December.
	@param day THe day of the month; 1, ..., {31, 30, 29, 28}. */
    DODS_Date(int year, int month, int day);
    //@}

    /** @name Assignment */
    //@{
    /** Parse the string and assign the value to this object. 
	@see DODS_Date(string) */
    void set_date(string date);

    /** Parse the DODS String and assign the value to this object. 
	@see DODS_Date(BaseType *arg) */
    void set_date(BaseType *arg);

    /** Assign the date using the two integers.
	@see DODS_Date(int year, int day_number) */
    void set_date(int year, int day_number);

    /** Assign the date usign three integers.
	@see DODS_Date(int year, int month, int day) */
    void set_date(int year, int month, int day);
    //@}

    /** @name Relational operators. */
    //@{
    /// Equality
    friend int operator==(DODS_Date &d1, DODS_Date &d2);
    /// !Equality
    friend int operator!=(DODS_Date &d1, DODS_Date &d2);
    /// Less than
    friend int operator<(DODS_Date &d1, DODS_Date &d2);
    /// Greater than
    friend int operator>(DODS_Date &d1, DODS_Date &d2);
    /// Greater than or equal
    friend int operator<=(DODS_Date &d1, DODS_Date &d2);
    /// Less than or equal
    friend int operator>=(DODS_Date &d1, DODS_Date &d2);
    //@}

    /** @name Access. */
    //@{
    /** @return The year in years A.D. */
    int year() const;

    /** @return The month of the year (1 == January, ..., 12 == December). */
    int month() const;

    /** @return The day of the month (1, ... {28, 29, 30, 31}. */
    int day() const;

    /** @return The day-number of the year (1 = 1 Jan). */
    int day_number() const;

    /** @return The date using the yyyy/ddd format. */
    string yd_date() const;

    /** @return The date using the yyyy/mm/dd format. */
    string ymd_date() const;

    /** @return The Julian day number for this date. */
    long julian_day() const;
    //@}

    /** For the date calculate the time as the number of seconds since
	00:00:00 UTC 1 Jan 1970. If the date is before 1 Jan 1970, return
	DODS\_UINT\_MAX. If the date is too late to represent as seconds
	since 1 Jan 1970, return DODS\_UINT\_MAX. Each day starts at 00:00:00
	UTC.

	@return The date in seconds since 1 Jan 1970.
        @see dods-limits.h
	@see time.h
 	@see mktime(3) */
    time_t unix_time() const;
};

#endif // _dods_date_h
