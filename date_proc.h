
// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: date_proc.h,v $
// Revision 1.3  2000/08/31 22:16:55  jimg
// Merged with 3.1.7
//
// Revision 1.2.24.1  2000/08/03 20:16:27  jimg
// The is_leap and days_in_year functions are now externally visible. This
// should be the only place where we calculate leap year stuff.
//
// Revision 1.2  1998/11/10 17:46:56  jimg
// Added this log.
//

int is_leap(int year);
double days_in_year(int year);

long julian_day(int year, int month, int day);
void gregorian_date(double jd, int *year, int *month, int *day, int *hours,
		    int *minutes, double *sec);
int month_day_to_days(int year, int month, int day);
void days_to_month_day(int year, int ddd, int *month, int *day);

int dayofweek(double j);




