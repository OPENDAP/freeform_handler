
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// Implementation of the DODS_Decimal_Year_Factory class

#include "config_ff.h"

static char rcsid[]#include "config_ff.h"
="$Id: DODS_EndDecimal_Year_Factory.cc,v 1.2 2000/08/31 22:16:53 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>

#include "Error.h"
#include "DODS_EndDecimal_Year_Factory.h"

// Build DODS_EndDate_Factory and DODS_EndTime_Factory objects using the DAS
// information. 

DODS_EndDecimal_Year_Factory::DODS_EndDecimal_Year_Factory(DDS &dds, DAS &das) :
  _ddf(dds, das), _dtf(dds, das)
{
}

DODS_Decimal_Year
DODS_EndDecimal_Year_Factory::get()
{
    DODS_Date d = _ddf.get();
    DODS_Time t = _dtf.get();

    return DODS_Decimal_Year(d, t);
}



