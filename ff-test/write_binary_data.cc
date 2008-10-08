
/* Little program to write out binary data used to test the FreeForm server.
   5/18/2001 jhrg */

#include <iostream>
#include <fstream>

int
main(int argc, char *argv[])
{
    int data;
    double fdata;
    ofstream ofs2("dbl_data3.bin");

	for (int i = 0; i<10; ++i) {
	    for (int j = 0; j<10; ++j) {
		data = (i+1) * (j+1);
		ofs2.write(&data, sizeof(int));
	    }
	}
	for (int i = 0; i<10; ++i) {
	    for (int j = 0; j<20; ++j) {
		fdata = static_cast<double>((i+1) * (j+1)) * 3.1415;
		ofs2.write(&fdata, sizeof(double));
	    }
	}
}
