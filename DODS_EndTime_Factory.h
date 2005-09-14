
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

#ifndef _dods_endtime_factory_h
#define _dods_endtime_factory_h

#include "DODS_Time.h"
#include "DODS_Time_Factory.h"
#include "DAS.h"
#include "DDS.h"
#include "BaseType.h"

/** Read times from datasets based on text configuration values. Times are
    returned using DODS\_Time objects.

    @see DODS_Time
    @author Daniel Holloway
    @author James Gallagher */

class DODS_EndTime_Factory :public DODS_Time_Factory {
private:
    DODS_EndTime_Factory() {}	/* Prevent the creation of empty objects. */

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
    
    DODS_EndTime_Factory(DDS &dds, DAS &das) 
	: DODS_Time_Factory(dds, das, "DODS_EndTime") {}
    //@}
};

#endif // _dods_endtime_factory_h 
