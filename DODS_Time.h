
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: DODS_Time.h,v $
// Revision 1.3  1999/01/08 22:08:19  jimg
// Fixed doc++ comments.
//
// Revision 1.2  1999/01/05 00:37:28  jimg
// Removed string class; replaced with the GNU String class. It seems those
// don't mix well.
// Switched to simpler method names.
// Added DOC++ Comments.
//
// Revision 1.1  1998/12/28 19:07:33  jimg
// Initial version of the DODS_Time object
//

#ifndef _dods_time_h
#define _dods_time_h

#ifdef __GNUG__
#pragma interface
#endif

#include <String.h>

#include "BaseType.h"

/** The DODS Time object. This provides a way to translate between various
    representations of time. This class is designed to be compatible with the
    DODS\_Date class so that dates and times may be used together in relational
    expressions. 

    NB: The seconds part of a DODS\_Time may contain fractional components.
    To test for equality of times, this class tests to see if the second time
    falls within a neighborhood around the first time the size of which is
    determined by an epsilon value (1.0e-6 by default). Thus seconds may have
    precision down to the micro-second (depending on the underlying
    hardware). See the #set_epsilon()# and #get_epsilon()# mfuncs. The
    epsilon value is a static class member; the same value is used for all
    instances of the class, and when changed, is changed at that time for all
    instances.

    @see DODS_Date
    @see DODS_Date_Time()
    @author James Gallagher */

class DODS_Time {
private:
    int _hours;			// 0--23; regular wall time
    int _minutes;		// 0--59
    double _seconds;		// 0--59.9...
    double _sec_since_midnight;
    bool _gmt;

    static double _eps;		// defined as 1.0e-6 in DODS_Time.cc

public:
    /** @name Constructors */

    //@{
    /** Build a DODS\_Time by parsing the String #time#. The string may be
	either of the form `hh:mm:ss' or `hh:mm'. In the later case the
	seconds are assumed to be zero. In addition, the string may have the
	suffix `GMT' or `UTC' indicating that the time is in Greenwich Mean
	Time.

	@param time The time string. */
    DODS_Time(String time);

    /** Build a DODS\_Time by parsing the DODS Str #arg#.

	@see DODS_Time(String).
	@param arg A DODS Str variable, passed as a BaseType pointer. */
    DODS_Time(BaseType *arg);

    /** Build a DODS\_Time.

	@param hh The hours, 0-23.
	@param mm The minutes, 0-59.
	@param gmt True if the time is a GMT time, false otherwise. */
    DODS_Time(int hh, int mm, bool gmt = false);

    /** Build a DODS\_Time.

	@param hh The hours, 0--23.
	@param mm The minutes, 0--59.
	@param ss The seconds, 0--59. May contain a fractional component.
	@param gmt True if the time is a GMT time, false otherwise. */
    DODS_Time(int hh, int mm, double ss, bool gmt = false);

    /** Build an empty DODS\_Time.

	NB: This won't pass the class invariant. */
    DODS_Time();
    //@}

    /** @name Assignment */

    //@{
    /** Set the value by parsing the String #time#.

	@param time The time string.
	@see DODS_Time(String). */
    void set(String time);

    /** Set the value by parsing the DODS Str #arg#.

	@param arg The time string wrapped in a DODS String.
	@see DODS_Time(BaseType *). */
    void set(BaseType *arg);

    /** Set the value using the given numeric values.

	@param hh The hours, 0-23.
	@param mm The minutes, 0-59.
	@param gmt True if the time is a GMT time, false otherwise.
	@see DODS_Time(int, int, bool). */
    void set(int hh, int mm, bool gmt = false);

    /** Set the value using the given numeric values.

	@param hh The hours, 0--23.
	@param mm The minutes, 0--59.
	@param ss The seconds, 0--59. May contain a fractional component.
	@param gmt True if the time is a GMT time, false otherwise. 
	@see DODS_Time(int, int, double, bool). */
    void set(int hh, int mm, double ss, bool gmt = false);
    //@}

    /** @name Access */

    //@{
    /** Get the string representation of time.

	@param gmt If true append the suffix `GMT' to the time if it a GMT
	time. If false, ignore gmt. True by default.
	@return The string representation for this time. */
    String get(bool gmt = true) const;

    /** @return The number of hours. */
    int hours() const;

    /** @return The number of minutes. */
    int minutes() const;

    /** @return The number of seconds. */
    double seconds() const;

    /** @return True if the time is a GMT time, false otherwise. */
    bool gmt() const;

    /** Get the number of seconds since midnight.
	@return The number of seconds since midnight. */
    double seconds_since_midnight() const;
    //@}

    /** @name Relational operators */
    //@{
    /// Equality
    friend int operator==(DODS_Time &t1, DODS_Time &t2);

    /// Inequality
    friend int operator!=(DODS_Time &t1, DODS_Time &t2);

    /// Less-than
    friend int operator<(DODS_Time &t1, DODS_Time &t2);

    /// Greater-than
    friend int operator>(DODS_Time &t1, DODS_Time &t2);

    /// Less-than or Equal-to
    friend int operator<=(DODS_Time &t1, DODS_Time &t2);

    /// Greater-than or Equal-to
    friend int operator>=(DODS_Time &t1, DODS_Time &t2);
    //@}

    /** Class invariant.

	@return True for a valid instance, otherwise false. */
    bool OK() const;

    /** Get the value of epsilon used for equality tests. */
    double get_epsilon() const;

    /** Set the value of epsilon used for equality tests. By default the
	value is 0.000001 (10e-6). */
    void set_epsilon(double eps);

};

#endif // _dods_time_h
