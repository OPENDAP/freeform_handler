
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of ff_handler a FreeForm API handler for the OPeNDAP
// DAP2 data server.

// Copyright (c) 2005 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
// 
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      dan             Daniel Holloway (dholloway@gso.uri.edu)

// Implementation of the DODS_Date_Time_Factory class

#include "config_ff.h"

static char rcsid[] not_used ="$Id$";


#include <stdio.h>
#include <stdlib.h>

#include "Error.h"
#include "DODS_EndDate_Time_Factory.h"

// Build DODS_Date_Factory and DODS_Time_Factory objects using the DAS
// information. 

DODS_EndDate_Time_Factory::DODS_EndDate_Time_Factory(DDS &dds, DAS &das) :
    _ddf(dds, das), _dtf(dds, das)
{
}

DODS_Date_Time
DODS_EndDate_Time_Factory::get()
{
    DODS_Date d = _ddf.get();
    DODS_Time t = _dtf.get();

    return DODS_Date_Time(d, t);
}


