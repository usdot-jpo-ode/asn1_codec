asn1_codec Release Notes
----------------------------

Version 3.1.0, released May 2025
----------------------------------------
### **Summary**
This release introduces an HTTP server to the asn1_codec module, enabling decoding of J2735 ASN.1 messages via a request-response interface. This enhancement adds batch decoding support for the Conflict Visualizer GUI (CVManager Intersection API), allowing it to process and display PCAP files without relying on embedded Kafka infrastructure. Additionally, it fixes the encoding for the eventJackKnife extension in the VehicleEventFlags BIT STRING, per the 2024 J2735 update. Instead of removing the extension as previously done, the generated C code was corrected to properly handle UPER encoding, with compatibility verified against multiple ASN.1 compilers.

Enhancements in this release:
- [CDOT PR 41](https://github.com/CDOT-CV/asn1_codec/pull/41): Http server
- [CDOT PR 42](https://github.com/CDOT-CV/asn1_codec/pull/42): Fix VehicleEventFlags Jackknife Event Extension
- [CDOT PR 43](https://github.com/CDOT-CV/asn1_codec/pull/43): Set up CI with Azure Pipelines


Version 3.0.0, released January 2025
----------------------------------------
### **Summary**
The main updates include:
- Changing the default configurations to use ZStandard (zstd) compression.
- Updating GitHub Actions workflows with the latest versions of third-party actions from external repositories to eliminate Node.js deprecation warnings.
- Introducing partial compatibility with J2735 2024.

However, there is a limitation. Due to [a bug in the ASN.1 Compiler](https://github.com/usdot-fhwa-stol/usdot-asn1c/issues/2), the VehicleEventFlags bitstring has been reverted to its 2020 version, omitting the 
eventJackKnife bit introduced in the 2024 revision. As a result, the eventJackKnife bit will not appear in output BSMs until the issue is resolved.

- [CDOT PR 34](https://github.com/CDOT-CV/asn1_codec/pull/34): Compression
- [CDOT PR 35](https://github.com/CDOT-CV/asn1_codec/pull/35): Update j2735 version to 2024.
- [CDOT PR 37](https://github.com/CDOT-CV/asn1_codec/pull/37): Workaround bsm vehicle event flags issue
- [CDOT PR 39](https://github.com/CDOT-CV/asn1_codec/pull/39): Renamed PartIIcontent to BSMpartIIExtension in test data
- [USDOT PR 62](https://github.com/usdot-jpo-ode/asn1_codec/pull/62): Update GitHub Actions Third-Party Action Versions

### Breaking Changes
In the 2024 TIM message specification, the following fields in the `TravelerDataFrame` sequence have been renamed. This is a breaking change to the XML 
encoding, requiring an update to the ODE POJOs. However, the UPER encoding remains independent of these names and remains backward compatible.

notUsed -> doNotUse1
notUsed1 -> doNotUse2
notUsed2 -> doNotUse3
notUsed3 -> doNotUse4

As a result of these breaking changes, the major version number has been updated to 3.


Version 2.2.0, released September 2024
----------------------------------------
### **Summary**
The changes for the asn1_codec 2.2.0 release include the addition of encoder topics to KAFKA_CREATE_TOPICS and updates to the `do_kafka_test.sh` script.

Enhancements in this release:
- CDOT PR 29: Added encoder topics to KAFKA_CREATE_TOPICS
- CDOT PR 30: Updated `do_kafka_test.sh` script


Version 2.1.0, released June 2024
----------------------------------------
### **Summary**
The changes for the asn1_codec 2.1.0 release include revised documentation for accuracy and improved clarity and readability. Additionally, there has been a transition to using `usdot-fhwa-stol/usdot-asn1c` instead of `mouse07410/asn1c` for the ASN.1 Compiler.

Enhancements in this release:
- CDOT PR 25: Revised documentation for accuracy & improved clarity/readability
- CDOT PR 26: Transitioned to using `usdot-fhwa-stol/usdot-asn1c` instead of `mouse07410/asn1c` for ASN.1 Compiler

Known Issues:
- The do_kafka_test.sh script in the project's root directory is currently not running successfully. The issue is being investigated and will be addressed in a future update.
- According to Valgrind, a minor memory leak has been detected. The development team is aware of this and is actively working on resolving it.


Version 2.0.0, released February 2024
----------------------------------------

### **Summary**
The changes for the asn1_codec 2.0.0 release include dockerfile optimizations, a transition to J2735 2020, dockerhub image documentation, a move to including the generated header/implementation files in tracked files as tarballs and updated SCMS files.

Enhancements in this release:
- CDOT PR 14: Optimized dockerfiles for smaller & faster deployments by switching over to using alpine as the base image
- CDOT PR 18: Allowed 'doIt.sh' to target directories instead of a single file
- CDOT PR 19: Transitioned to using the ASN files for J2735 2020.
- CDOT PR 17: Added dockerhub image documentation
- CDOT PR 21: Included generated header/implementation files in tracked files as tarballs
- CDOT PR 20: Updated SCMS files

Known Issues:
- The do_kafka_test.sh script in the project's root directory is currently not running successfully. The issue is being investigated and will be addressed in a future update.
- According to Valgrind, a minor memory leak has been detected. The development team is aware of this and is actively working on resolving it.

Breaking Changes
- Users should note that due to the switch to J2735 2020, TIMs conforming to J2735 2016 will no longer be processed successfully. This is due to the fact that the J2735 2020 standard has renamed some fields for TIMs. The corresponding updates in the 2.0.0 release for the ODE handles these changes for incoming 2016 TIMs, but any outside applications will need to update their systems to handle these changes prior to forwarding 2016 TIMs to the ACM. The TIM changes in J2735 2020 include the following field renamings:
    - `sspTimRights` -> `notUsed`
    - `sspLocationRights` -> `notUsed1`
    - `sspMsgRights1` -> `notUsed2`
    - `sspMsgRights2` -> `notUsed3`
    - `duratonTime` -> `durationTime`


Version 1.5.0, released November 2023
----------------------------------------

### **Summary**
The changes for the asn1_codec 1.5.0 include improved compatibility, a new build script, enhanced code analysis, uniform logging levels, auto-restart for Docker-compose, a default logging level change, and graceful shutdown for ACM in response to errors.
- The catch dependency has been updated to a more recent version to ensure compatibility and reliability.
- Added `build.sh script`
- A new sonar configuration has been included to enhance code analysis capabilities.
- The logging level strings have been converted to uppercase to match those in other submodules.
- Docker-compose.yml files have been modified to automatically restart in case of failure.
- The default logging level has been changed from TRACE to ERROR.
- ACM will now gracefully shut down when it encounters a transport error or an unrecognized Kafka error.

Known Issues:
1.	 The do_kafka_test.sh script in the project's root directory is currently not running successfully. The issue is being investigated and will be addressed in a future update.
2.	According to Valgrind, a minor memory leak has been detected. The development team is aware of this and is actively working on resolving it.


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
