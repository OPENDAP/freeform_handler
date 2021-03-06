/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of ff_handler a FreeForm API handler for the OPeNDAP
 DAP2 data server.

 Copyright (c) 2005 OPeNDAP, Inc.
 Author: James Gallagher <jgallagher@opendap.org>

 This is free software; you can redistribute it and/or modify it under the
 terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2.1 of the License, or (at your
 option) any later version.
 
 This software is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
*/

#ifndef config_ff_h_
#define config_ff_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* GNU gcc/g++ provides a way to mark variables, etc. as unused */

#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif

#endif /* config_ff_h_ */
