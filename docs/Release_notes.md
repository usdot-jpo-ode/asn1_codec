asn1_codec Release Notes
----------------------------

Version 1.4.0, released July 5th 2023
----------------------------------------

### **Summary**
The release of asn1_codec version 1.4.0 focuses on making improvements to the logging system. The following changes have been made:

Enhancements in this release:
- Implemented modifications to ensure that log setup errors are flushed immediately.
- Added some files to the `.gitignore` file to exclude them from version control.
- Added an existence check for the build folder before deleting it in `build_local.sh` script.
  
Fixes in this release:
- Minimized the reliance on standard output throughout the code.
- Reviewed the log levels for each log statement and adjusted as needed.

Known Issues:
- The do_kafka_test.sh script in the project's root directory is currently not running successfully. The issue is being investigated and will be addressed in a future update.
- According to Valgrind, a minor memory leak has been detected. The development team is aware of this and is actively working on resolving it.

Version 1.3.0, released Mar 30th 2023
----------------------------------------

### **Summary**
The updates for asn1_codec 1.0.0 include Confluent Cloud integration, some fixes (including a memory leak fix), logging modifications and documentation improvements.

Enhancements in this release:
- Created and utilized the AcmLogger class.
-	Added a method to initialize project submodules to the build_local.sh script.
-	Added asn1 reset to allow build.
-	Simplified logging solution & added log level environment variable to the project.
-	Set proper security protocol.
-	Altered group.id in some properties files.
-	Improved how env vars are retrieved in acm.cpp.
-	Altered acm.error.template property in some files.
-	Updated partition fetch size.
-	Updated descriptor.
-	Modified dockerfiles and cleaned up acm.cpp a bit.
-	Added a note about SASL.
-	Added a section to the README on CC integration.
-	Swapped to using librdkafka package instead of including it as a submodule.
-	Added build script.
-	Added better descriptors and test call.

Fixes in this release:
-	Fixed bug with consumer_ptr.
-	Fixed a memory leak occurring due to a metadata pointer not getting freed.
-	Reorganized some code for readability purposes.
-	Allowed the project to work with an instance of kafka hosted by Confluent Cloud.
-	Commented out asn1.kafka.partition property in some properties files.

Known Issues
-	The do_kafka_test.sh script in the root directory of the project does not run successfully at this time.
-	There is a potential minor memory leak, according to Valgrind.
