
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

#ifndef _dods_time_factory_h
#define _dods_time_factory_h

#ifdef __GNUG__
#pragma interface
#endif

#include "DODS_Time.h"
#include "DAS.h"
#include "DDS.h"
#include "BaseType.h"

/** Read times from datasets based on text configuration values. Times are
    returned using DODS\_Time objects.

    @see DODS_Time
    @author James Gallagher */

class DODS_Time_Factory {
private:
    BaseType *_hours;
    BaseType * _minutes;
    BaseType *_seconds;
    bool _gmt;

    DODS_Time_Factory() {}	/* Prevent the creation of empty objects. */

public:

    /** Note that the default constructor is private.
	@name Constructors */

    //@{
    /** Read the configuration information and decide how to build DODS\_Time
	objects. The DODS\_Time\_Factory member function #get_time()# will
	return DODS\_Time objects.

	@see get_time()
	@param dds The DDS of the dataset from which times are to be read.
	@param das The DAS of the dataset from which times are to be read. */
    
    DODS_Time_Factory(DDS &dds, DAS &das);
    //@}

    /** @name Access */

    //@{
    /** Read a time value from a dataset.

	@return The DODS\_Time object associated with the time. */
    DODS_Time get();
    //@}
};

// $Log: DODS_Time_Factory.h,v $
// Revision 1.4  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.3  1999/01/08 22:08:19  jimg
// Fixed doc++ comments.
//
// Revision 1.2  1999/01/05 00:42:42  jimg
// Switched to simpler method names.
// Added _gmt field.
//
// Revision 1.1  1998/12/28 19:08:05  jimg
// Initial version of the DODS_Time factory object. This is a test
// implementation. 
//

#endif // _dods_time_factory_h 
