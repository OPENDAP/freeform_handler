
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: DODS_Time_Factory.h,v $
// Revision 1.2  1999/01/05 00:42:42  jimg
// Switched to simpler method names.
// Added _gmt field.
//
// Revision 1.1  1998/12/28 19:08:05  jimg
// Initial version of the DODS_Time factory object. This is a test implementation.
//

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
    returned using DODS_Time objects.

    @see DODS\_Time
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
    /** Read the configuration information and decide how to build DODS_Time
	objects. The DODS_Time_Factory mfunc get\_time() will return
	DODS\_Time objects.

	@see get\_time()
	@param dds The DDS of the dataset from which times are to be read.
	@param das The DAS of the dataset from which times are to be read. */
    
    DODS_Time_Factory(DDS &dds, DAS &das);
    //@}

    /** Read a time value from a dataset.

	@return The DODS_Time object associated with the time. */

    DODS_Time get();
};

#endif // _dods_time_factory_h 
