
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

#ifndef _dods_date_time_factory_h
#define _dods_date_time_factory_h

#ifdef __GNUG__
#pragma interface
#endif

#include "DODS_Date_Time.h"
#include "DODS_Date_Factory.h"
#include "DODS_Time_Factory.h"

/** Read dates from datasets based on text configuration values. Dates are
    returned using DODS\_Date objects.

    @see DODS_Date_Time
    @author James Gallagher */

class DODS_Date_Time_Factory {
private:
    DODS_Date_Factory _ddf;
    DODS_Time_Factory _dtf;

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
    
    DODS_Date_Time_Factory(DDS &dds, DAS &das);
    //@}

    /** @name Access */
    //@{
    /** Read a date/time value from a dataset.

	@return The DODS\_Date\_Time object associated with the date/time. */
    DODS_Date_Time get();
    //@}
};

// $Log: DODS_Date_Time_Factory.h,v $
// Revision 1.2  2000/10/11 19:37:55  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.1  1999/01/08 22:08:19  jimg
// Fixed doc++ comments.
//

#endif // _dods_date_time_factory_h
