

	$Id$

Updated for version 3.8.2

For information about building the FreeForm data handler, see theINSTALL file.

This file provides basic information about the FreeForm handler and itsconfiguration. For more detailed information, see the ServerInstallation Guide at http://www.opendap.org. Also note that there isalso a special manual for the FreeForm server that covers writingformat description files in detail available at the OPeNDAP web site.

For general information about configuration of the base software forthe OPeNDAP server, see the README that comes with that server and/orthe Server Installation Guide that can be found at www.opendap.org.

Test data are also installed, so after installing this handler, Hyraxwill have data to serve providing an easy way to test your newinstallation and to see how a working bes.conf should look. To usethis, make sure that you first install the bes, and that dap-servergets installed too.  Finally, every time you install or reinstallhandlers, make sure to restart the BES and OLFS.

--------------------------------------------------------------------------

dap_ff_handler is a data handler which uses National Geophysical DataCenter's FreeForm/FFND to serve data. To use with the Hyrax, you'llneed BES 3.8.0 or greater.

The handler can read binary, ASCII, and dBASE data files usingFreeForm/FFND format descriptions. It can return both array andsequence data types. The data in the data file is read then servedusing the appropriate type according to the format description in therequired accompanying format file.

All the data and associated blocks (header information) are processed.Basic information about the input file is given as Global Attributes asis any header information. The file's data variables are mapped intothe closest DAP variable type using the FreeForm/FFND external formatfile.

WRITING FORMAT DESCRIPTION FILES

Any data file served must be accompanied by a format description file(<file_name>.fmt). The format description file defines the formats ofheaders and data in the data file. The following shows a few examplesof data description files and their translation into DAP variables, formore information about format descriptions, please see theOPeNDAP-FreeFrom's user guide.

1) Format to read a dBASE sequence file 

       dbase_input_data "ISEE3_data"
       t 1 15 double 6
       Bx 16 30 double 6
       By 31 45 double 6
       Bz 46 60 double 6
       Bx2 61 75 double 6


   The above format file will map into the following DAP data set:

     Dataset {
       Sequence {
        Float64 t;
        Float64 Bx;
        Float64 By;
        Float64 Bz;
        Float64 Bx2;
       } ISEE3_data;
     } test5;




2) Format to read a binary array file

   binary_input_data "array_in"
   x 1 4 ARRAY["lat" 1 to 10]["lon" 1 to 10] of int32 0
   y 5 12 ARRAY["records" 1 to 10] of float64 15


   The above format file will map into the following DAP data set:

   Dataset {
      Int32 x[lat = 10][lon = 10];
      Float64 y[records = 10];
   } test3;




3) Format to read an ASCII sequence with a file header

      ASCII_file_header "header"
      count 1 2 short 0
      description 4 24 char 0

      ASCII_input_data "data"
      value 1 2 short 0


   The above format file will map into the following DAP data set:

       Dataset {
	  Sequence {
	  Int32 value;
	  } data;
	} test0;


4) The FreeForm library contains a useful data processing/transformation library. Here's an example of how to tweak data written on a big-endianmachine so it can be served from a little-endian one - include the followingat the start of the format file:

input_eqv begin constant
    data_byte_order text big_endian
end constant 


Thanks to Denis Nadeau for this information.

For more examples see the test files in the ff-test subdirectory. There isa manual that describes FreeForm and handler on our web page - opendap.org -in the User Documentation section.      

--------------------------------------------------------------------------

Note: The FreeForm library, which is in the subdirectory 'FFND,' carriesthe following notice regarding its copyright protection:

 * CAVEAT:
 * No claims are made as to the suitability of the accompanying
 * source code for any purpose.  Although this source code has been
 * used by the NOAA, no warranty, expressed or implied, is made by
 * NOAA or the United States Government as to the accuracy and
 * functioning of this source code, nor shall the fact of distribution
 * constitute any such endorsement, and no responsibility is assumed
 * by NOAA in connection therewith.  The source code contained
 * within was developed by an agency of the U.S. Government.
 * NOAA's National Geophysical Data Center has no objection to the
 * use of this source code for any purpose since it is not subject to
 * copyright protection in the U.S.  If this source code is incorporated
 * into other software, a statement identifying this source code may be
 * required under 17 U.S.C. 403 to appear with any copyright notice.


