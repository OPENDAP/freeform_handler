
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// $Log: DODS_StartDate_Factory.h,v $
// Revision 1.2  2000/08/31 22:16:54  jimg
// Merged with 3.1.7
//
// Revision 1.1.2.1  2000/05/01 20:11:17  dan
// New server-side function to support date-range usage in time fields.
//
//

#ifndef _dods_startdate_factory_h
#define _dods_startdate_factory_h

#ifdef __GNUG__
#pragma interface
#endif

#include "DODS_Date.h"
#include "DAS.h"
#include "DDS.h"
#include "BaseType.h"

/** Read dates from datasets based on text configuration values. Dates are
    returned using DODS\_Date objects.

    @see DODS_Date
    @author Daniel Holloway
    @author James Gallagher */

class DODS_StartDate_Factory {
private:
    int _year_base;

    BaseType *_year;
    BaseType *_month;
    BaseType *_day;
    BaseType *_year_day;

    enum date_format _format;

    DODS_StartDate_Factory() {}	/* Prevent the creation of empty objects. */

public:

    /** Note that the default constructor is private.
	@name Constructors */

    //@{
    /** Read the configuration information and decide how to build DODS\_Date
	objects. The DODS\_StartDate\_Factory member function #get_date()# will
	return DODS\_Date objects.

	@see get_date()
	@param dds The DDS of the dataset from which dates are to be read.
	@param das The DAS of the dataset from which dates are to be read. */
    
    DODS_StartDate_Factory(DDS &dds, DAS &das);
    //@}

    /** @name Access */
    //@{
    /** Read a date value from a dataset.

	@return The DODS\_Date object associated with the date. */

    DODS_Date get();
    //@}
};

#endif // _dods_startdate_factory_h 









