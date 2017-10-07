# Building Test Data Files

- You can construct test data files by working with the BSM MessageFrame and working up.

j2735.MessageFrame.Bsm.xml : apply converter-example -ixer -oper -p MessageFrame
j2735.MessageFrame.Bsm.per : apply xxd -p
j2735.MessageFrame.Bsm.hex : cut and paste into Ieee1609Dot2Data xml structure.

Ieee1609Dot2Data.unsecuredData.xml : apply converter-example -ixer -oper -p Ieee1609Dot2Data
Ieee1609Dot2Data.unsecuredData.per : apply xxd -p
Ieee1609Dot2Data.unsecuredData.hex : cut and paste into the BAH packet that goes into the input Kafka stream.

BAH.Input.xml : the data that is received on the Kafka input stream.

# Notes on using "xxd" for conversion from bytes to hex strings and hex strings to bytes.

- Be careful to follow these directions or you will produce errors.  The hex dump is normally annotated and that is not
  needed in the hex strings input into the payload sections of these XML files.

- xxd -p <file>.bin > <file>.hex      // converts a binary file into a straight hex string.
- xxd -r -p <file>.hex > <file>.bin   // converts the binary string above into a binary file.






