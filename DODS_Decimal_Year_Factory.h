
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


#ifndef _dods_Decimal_Year_factory_h
#define _dods_Decimal_Year_factory_h

#ifdef __GNUG__
#pragma interface
#endif

#include "DODS_Decimal_Year.h"
#include "DODS_Date_Factory.h"
#include "DODS_Time_Factory.h"

/** Read dates from datasets based on text configuration values. Dates are
    returned using DODS\_Date objects.

    @see DODS_Decimal_Year
    @author James Gallagher */

class DODS_Decimal_Year_Factory {
private:
    DODS_Date_Factory _ddf;
    DODS_Time_Factory _dtf;

public:

    /** Note that the default constructor is private.
	@name Constructors */

    //@{
    /** Read the configuration information and decide how to build
	DODS\_Date\_Time objects. The DODS\_Date\_Time\_Factory member
	function #get_Decimal_Year()# will return DODS\_Date objects.

	@see get_Decimal_Year()
	@param dds The DDS of the dataset from which dates are to be read.
	@param das The DAS of the dataset from which dates are to be read. */
    
    DODS_Decimal_Year_Factory(DDS &dds, DAS &das);
    //@}

    /** @name Access */
    //@{
    /** Read a date/time value from a dataset.

	@return The DODS\_Date\_Time object associated with the date/time. */
    DODS_Decimal_Year get();
    //@}
};

#endif // _dods_Decimal_Year_factory_h

