
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


bool func_date(int argc, BaseType *argv[], DDS &);

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

