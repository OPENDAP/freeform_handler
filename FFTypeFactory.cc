
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


#include <string>

#include "FFByte.h"
#include "FFInt16.h"
#include "FFUInt16.h"
#include "FFInt32.h"
#include "FFUInt32.h"
#include "FFFloat32.h"
#include "FFFloat64.h"
#include "FFStr.h"
#include "FFUrl.h"
#include "FFArray.h"
#include "FFStructure.h"
#include "FFSequence.h"
#include "FFGrid.h"

#include "FFTypeFactory.h"
#include "debug.h"

Byte *
FFTypeFactory::NewByte(const string &n ) const 
{ 
    return new FFByte(n);
}

Int16 *
FFTypeFactory::NewInt16(const string &n ) const 
{ 
    return new FFInt16(n); 
}

UInt16 *
FFTypeFactory::NewUInt16(const string &n ) const 
{ 
    return new FFUInt16(n);
}

Int32 *
FFTypeFactory::NewInt32(const string &n ) const 
{ 
    DBG(cerr << "Inside FFTypeFactory::NewInt32" << endl);
    return new FFInt32(n);
}

UInt32 *
FFTypeFactory::NewUInt32(const string &n ) const 
{ 
    return new FFUInt32(n);
}

Float32 *
FFTypeFactory::NewFloat32(const string &n ) const 
{ 
    return new FFFloat32(n);
}

Float64 *
FFTypeFactory::NewFloat64(const string &n ) const 
{ 
    return new FFFloat64(n);
}

Str *
FFTypeFactory::NewStr(const string &n ) const 
{ 
    return new FFStr(n);
}

Url *
FFTypeFactory::NewUrl(const string &n ) const 
{ 
    return new FFUrl(n);
}

Array *
FFTypeFactory::NewArray(const string &n , BaseType *v) const 
{ 
    return new FFArray(n, v);
}

Structure *
FFTypeFactory::NewStructure(const string &n ) const 
{ 
    return new FFStructure(n);
}

Sequence *
FFTypeFactory::NewSequence(const string &n ) const 
{ 
    DBG(cerr << "Inside FFTypeFactory::NewSequence" << endl);
    return new FFSequence(n);
}

Grid *
FFTypeFactory::NewGrid(const string &n ) const 
{ 
    return new FFGrid(n);
}
