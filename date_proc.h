// (c) COPYRIGHT URI/MIT 1998
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

long julian_day(int year, int month, int day);
void gregorian_date(double jd, int *year, int *month, int *day, int *hours,
		    int *minutes, double *sec);
int month_day_to_days(int year, int month, int day);
void days_to_month_day(int year, int ddd, int *month, int *day);

int dayofweek(double j);




