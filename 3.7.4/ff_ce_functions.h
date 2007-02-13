
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

#ifndef _ff_ce_functions_h
#define _ff_ce_functions_h 1

class DDS ;
class ConstraintEvaluator ;

void ff_register_functions(ConstraintEvaluator &ce) ;

#if 0

// These are not defined as static functions to keep them out of the global
// namespace. This was done to prevent name collisions when this code is used
// to build a BES module. jhrg 8/30/06

bool func_date(int argc, BaseType *argv[], DDS &);
bool func_date_range(int argc, BaseType *argv[], DDS &);

// Selection and projection (synthesis) for yyyy/ddd dates.
void proj_dods_jdate(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_jdate(int argc, BaseType *argv[], DDS &dds);

// Selection and projection for yyyy/mm/dd dates.
void proj_dods_date(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_date(int argc, BaseType *argv[], DDS &dds);

bool func_time(int argc, BaseType *argv[], DDS &);

void proj_dods_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_time(int argc, BaseType *argv[], DDS &dds);

bool func_date_time(int argc, BaseType *argv[], DDS &);

void proj_dods_date_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_date_time(int argc, BaseType *argv[], DDS &dds);

bool func_startdate(int argc, BaseType *argv[], DDS &);

void proj_dods_startdate(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_startdate(int argc, BaseType *argv[], DDS &dds);

bool func_starttime(int argc, BaseType *argv[], DDS &);

void proj_dods_starttime(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_starttime(int argc, BaseType *argv[], DDS &dds);

bool func_startdate_time(int argc, BaseType *argv[], DDS &);

void proj_dods_startdate_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_startdate_time(int argc, BaseType *argv[], DDS &dds);

bool func_enddate(int argc, BaseType *argv[], DDS &);

void proj_dods_enddate(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_enddate(int argc, BaseType *argv[], DDS &dds);

bool func_endtime(int argc, BaseType *argv[], DDS &);

void proj_dods_endtime(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_endtime(int argc, BaseType *argv[], DDS &dds);

bool func_enddate_time(int argc, BaseType *argv[], DDS &);

void proj_dods_enddate_time(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_enddate_time(int argc, BaseType *argv[], DDS &dds);

#if 0
bool func_decimal_year(int argc, BaseType *argv[], DDS &);

void proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);
#endif

void proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);

void proj_dods_startdecimal_year(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_startdecimal_year(int argc, BaseType *argv[], DDS &dds);

void proj_dods_enddecimal_year(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);
bool sel_dods_enddecimal_year(int argc, BaseType *argv[], DDS &dds);
#endif

#endif // _ff_ce_functions_h

