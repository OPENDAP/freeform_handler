
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// $Log: DODS_StartDate_Time_Factory.h,v $
// Revision 1.2  2000/08/31 22:16:54  jimg
// Merged with 3.1.7
//
// Revision 1.1.2.1  2000/05/01 20:18:56  dan
// New server-side function to support date-range usage in time fields.
//
// Revision 1.1  1999/01/08 22:08:19  jimg
// Fixed doc++ comments.
//

#ifndef _dods_startdate_time_factory_h
#define _dods_startdate_time_factory_h


#include "DODS_Date_Time.h"
#include "DODS_StartDate_Factory.h"
#include "DODS_StartTime_Factory.h"

/** Read dates from datasets based on text configuration values. Dates are
    returned using DODS\_Date objects.

    @see DODS_Date_Time
    @author Daniel Holloway
    @author James Gallagher */

class DODS_StartDate_Time_Factory {
private:
    DODS_StartDate_Factory _ddf;
    DODS_StartTime_Factory _dtf;

public:

    /** Note that the default constructor is private.
	@name Constructors */

    //@{
    /** Read the configuration information and decide how to build
	DODS\_Date\_Time objects. The DODS\_Date\_Time\_Factory member
	function #get_date_time()# will return DODS\_Date objects.

	@see get_date_time()
	@param dds The DDS of the dataset from which dates are to be read.
	@param das The DAS of the dataset from which dates are to be read. */
    
    DODS_StartDate_Time_Factory(DDS &dds, DAS &das);
    //@}

    /** @name Access */
    //@{
    /** Read a date/time value from a dataset.

	@return The DODS\_Date\_Time object associated with the date/time. */
    DODS_Date_Time get();
    //@}
};

#endif // _dods_startdate_time_factory_h
