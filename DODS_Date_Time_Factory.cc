
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODS_Date_Time_Factory class

// $Log: DODS_Date_Time_Factory.cc,v $
// Revision 1.1  1999/01/22 20:44:35  jimg
// Added
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: DODS_Date_Time_Factory.cc,v 1.1 1999/01/22 20:44:35 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>

#include "Error.h"
#include "DODS_Date_Time_Factory.h"

// Build DODS_Date_Factory and DODS_Time_Factory objects using the DAS
// information. 

DODS_Date_Time_Factory::DODS_Date_Time_Factory(DDS &dds, DAS &das) :
    _ddf(dds, das), _dtf(dds, das)
{
}

DODS_Date_Time
DODS_Date_Time_Factory::get()
{
    DODS_Date d = _ddf.get();
    DODS_Time t = _dtf.get();

    return DODS_Date_Time(d, t);
}
