

	$Id$

For information about building the FreeForm data handler, see the INSTALLfile.

This file provides basic information about the FreeForm handler and itsconfiguration. For more detailed information, see the Server InstallationGuide at http://www.opendap.org. Also note that there is also a specialmanual for the FreeForm server that covers writing format description filesin detail available at the OPeNDAP web site.

For general information about configuration of the base software for theOPeNDAP server, see the README that comes with that server and/or the ServerInstallation Guide that can be found at www.opendap.org.

A configuration edition helper script, `bes-ff-data.sh' is provided in this package for easy configuration of the Hyrax BES server, designed to editbes.conf. The script is called using:

   bes-ff-data.sh  <bes.conf file to modify>  <bes modules dir>  

The `bes-conf' make target runs the script while trying to select paths cleverly, and should be called using:

   make bes-conf

Test data are also installed, so after installing this handler, Hyrax will have data to serve providing an easy way to test your new installationand to see how a working bes.conf should look. To use this, make surethat you first install the bes, and that dap-server gets installed too.Finally, every time you install or reinstall handlers, make sure to restartthe BES and OLFS.

----------------------------------------------------------------------------

dap_ff_handler is a data handler which uses National Geophysical Data Center'sFreeForm/FFND to serve data. The handler requires the OPeNDAP dap-serversoftware above version 3.7.5. To use with the BES, you'll need BES 3.4.1 orgreater. 

The handler can read binary, ASCII, and dBASE data files using FreeForm/FFNDformat descriptions. It can return both array and sequence data types. Thedata in the data file is read then served using the appropriate typeaccording to the format description in the required accompanying format file.

All the data and associated blocks (header information) areprocessed. Basic information about the input file is given as GlobalAttributes as is any header information. The file's data variables are mappedinto the closest DAP variable type using the FreeForm/FFND external formatfile. 

WRITING FORMAT DESCRIPTION FILES

Any data file served must be accompanied by a format description file(<file_name>.fmt). The format description file defines the formats of headersand data in the data file. The following shows a few examples of datadescription files and their translation into DAP variables, for moreinformation about format descriptions, please see the OPeNDAP-FreeFrom's userguide.

1) Format to read a dBASE sequence file 

       dbase_input_data "ISEE3_data"       t 1 15 double 6       Bx 16 30 double 6       By 31 45 double 6       Bz 46 60 double 6       Bx2 61 75 double 6

   The above format file will map into the following DAP data set:

     Dataset {       Sequence {        Float64 t;        Float64 Bx;        Float64 By;        Float64 Bz;        Float64 Bx2;       } ISEE3_data;     } test5;



2) Format to read a binary array file

   binary_input_data "array_in"   x 1 4 ARRAY "lat" 1 to 10  "lon" 1 to 10  of int32 0   y 5 12 ARRAY "records" 1 to 10  of float64 15

   The above format file will map into the following DAP data set:

   Dataset {      Int32 x lat = 10  lon = 10 ;      Float64 y records = 10 ;   } test3;



3) Format to read an ASCII sequence with a file header

      ASCII_file_header "header"      count 1 2 short 0      description 4 24 char 0

      ASCII_input_data "data"      value 1 2 short 0

   The above format file will map into the following DAP data set:

       Dataset {	  Sequence {	  Int32 value;	  } data;	} test0;

For more examples see the test files in the ff-test subdirectory.         

----------------------------------------------------------------------------

Note: The FreeForm library, which is in the subdirectory 'FFND,' carriesthe following notice regarding its copyright protection:

 * CAVEAT: * No claims are made as to the suitability of the accompanying * source code for any purpose.  Although this source code has been * used by the NOAA, no warranty, expressed or implied, is made by * NOAA or the United States Government as to the accuracy and * functioning of this source code, nor shall the fact of distribution * constitute any such endorsement, and no responsibility is assumed * by NOAA in connection therewith.  The source code contained * within was developed by an agency of the U.S. Government. * NOAA's National Geophysical Data Center has no objection to the * use of this source code for any purpose since it is not subject to * copyright protection in the U.S.  If this source code is incorporated * into other software, a statement identifying this source code may be * required under 17 U.S.C. 403 to appear with any copyright notice.