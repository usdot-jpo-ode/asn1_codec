GENERAL INFORMATION
===================

This folder should contain the SAE J2735-201603 Dedicated Short Range Communications (DSRC)
Message Set Dictionary codec.

This folder should include the ASN.1 schema file for SAE J2735-201603 which isnot provided here.
The SAE J2735-201603 schema file is provided by SAE to purchasers 
as SAE J2735ASN-201603 (tm) as a companion product to 
assist them in deployment of SAE Standard J2735-201603. 
This document, like SAE J2735(tm) itself contains 
material copyrighted by SAE International.

Save your copy of SAE J2735-201603 schema file in this directory before building the ASN.1 compiler. 
Make sure the extension is *.asn in order to be picked up by the compiler build script (build_codec.sh).
Once the J2735 API code is generated by the compiler (via build_coded.sh script), 
invoking `make -f Makefile.am.sample` will build the API object files.

OBTAINING THE J2735 SPECIFICATION
=================================

Go to http://standards.sae.org/j2735_201603/ and download the files.

You should extract the ASN.1 module from there and save it
as the ../J2735_201603.asn1 file. This file should start with
the following line

	DSRC DEFINITIONS AUTOMATIC TAGS ::=

and end with the "END" token.
Be careful not to copy any non-ASN.1 preambles from that .DOC file.

After obtaining the J2735_201603.asn1, type `make` in the directory
containing this README file.
