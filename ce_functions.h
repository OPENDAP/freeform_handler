
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


bool func_date(int argc, BaseType *argv[], DDS &);
bool func_date_range(int argc, BaseType *argv[], DDS &);

// Selection and projection (synthesis) for yyyy/ddd dates.
void proj_dods_jdate(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_jdate(int argc, BaseType *argv[], DDS &dds);

// Selection and projection for yyyy/mm/dd dates.
void proj_dods_date(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_date(int argc, BaseType *argv[], DDS &dds);

bool func_time(int argc, BaseType *argv[], DDS &);

void proj_dods_time(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_time(int argc, BaseType *argv[], DDS &dds);

bool func_date_time(int argc, BaseType *argv[], DDS &);

void proj_dods_date_time(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_date_time(int argc, BaseType *argv[], DDS &dds);

bool func_startdate(int argc, BaseType *argv[], DDS &);

void proj_dods_startdate(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_startdate(int argc, BaseType *argv[], DDS &dds);

bool func_starttime(int argc, BaseType *argv[], DDS &);

void proj_dods_starttime(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_starttime(int argc, BaseType *argv[], DDS &dds);

bool func_startdate_time(int argc, BaseType *argv[], DDS &);

void proj_dods_startdate_time(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_startdate_time(int argc, BaseType *argv[], DDS &dds);

bool func_enddate(int argc, BaseType *argv[], DDS &);

void proj_dods_enddate(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_enddate(int argc, BaseType *argv[], DDS &dds);

bool func_endtime(int argc, BaseType *argv[], DDS &);

void proj_dods_endtime(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_endtime(int argc, BaseType *argv[], DDS &dds);

bool func_enddate_time(int argc, BaseType *argv[], DDS &);

void proj_dods_enddate_time(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_enddate_time(int argc, BaseType *argv[], DDS &dds);

#if 0
bool func_decimal_year(int argc, BaseType *argv[], DDS &);

void proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);
#endif

void proj_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_decimal_year(int argc, BaseType *argv[], DDS &dds);

void proj_dods_startdecimal_year(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_startdecimal_year(int argc, BaseType *argv[], DDS &dds);

void proj_dods_enddecimal_year(int argc, BaseType *argv[], DDS &dds);
bool sel_dods_enddecimal_year(int argc, BaseType *argv[], DDS &dds);








