# Interfacing with the ASN.1 Codec Module

## Terminology:

- ASN.1 Codec Module (ACM).  The general term for this module. The specific instantiations of the module are named based on their handling of ASN.1 not other data formats.
    - Decoding Module (ADM): ASN.1 to XML (or other). Handles data from the CV environment (OBU/RSU) into ODE into ACM back to ODE. Examples: Received TIM, BSMs.
    - Encoding Module (AEM): XML (or other) to ASN.1. Handles data from the TMC environment into the ODE into ACM back to ODE to CV environment. Example: Transmitted TIM (outgoing).
- Node-to-Encode: The top-level node within the `<payload><data>` section that will act as the root of XML to encode.
- Containing Node: The parent node of the Node-to-Encode.

## Current ACM Data Types

- Currently, the ACM handles inbound (from CV environment to ODE) Basic Safety Messages (BSMs) and Traveler Information
  Messages (TIMs) as SAE J2735 MessageFrames. It will also handle the MessageFrames if wrapped in a IEEE 1609.2 Data
  frame.
- In the future, the ACM will handle message types that are structures (i.e., have more than one element).
- 
    - IEEE 1609.2 frames are usually encoded using COER rules, but the
      module will use metadata provided by the ODE to determine the decoder to use.
    - SAE J2735 frames are usually encoded using UPER rules, but the
      module will use metadata provided by the ODE to determine the decoder to use.
- The ACM handles outbound (from ODE to CV environment) TIMs as SAE J2735 MessageFrames.
    - Currently, the SAE J2735 MessageFrame is encoded by the ODE using XER
      rules. UPER rules are used to encode these payload for transmission to the CV environment.

# Instructions to the ACM
- The ACM does not maintain any state with regard to a particular message; each message is handled based on the information provided in the XML input.
- The ACM handles XML messages whose root node is named `<OdeAsn1Data>`.
- The root node has two child element nodes: `<metadata>` and `<payload>`.
- The `<payload>` node contains the data to encode or decode.
- The `<metadata>` node provides instructions to the ACM.
    - In the `<metadata>` node, the child element `<encodings>` specifies how encodings/decodings will be performed and in which order.
      The following example should explain the heirarchy:
        ```xml
        <encodings>
        <encodings>
            <elementName>data/level1/level2 </elementName>
            <elementType>level2 </elementType>
            <encodingRule>UPER</encodingRule>
        </encodings>
        <encodings>
            <elementName>data/level1</elementName>
            <elementType>level1</elementType>
            <encodingRule>COER</encodingRule>
        </encodings>
        </encodings>
        ```
    - The `<encodings>` nodes included in the top level node will be processed in order.
    - The AEM will apply all specified encodings instructions during that "call."
        - The AEM will NOT perform a portion of encodings and then return for a signature, etc.
        - The only exception to this rule is encoding/decoding error.
    - `<elementName>` is the path in the XML message to find the Node-to-Encode relative to the `<payload>` node.  The last name in the path 
      will be the ASN.1 data type name (discussed below). This name is also specified in the `<elementType>` node.
    - If the path is not found or another error occurs, an XML document will be returned that describes the error and where it occured.
    - `<encodingRule>` will indicate the encoding/decoding rule to use.

# ASN.1 Specification Type Names
- [asn1c](https://github.com/vlm/asn1c) generates a `pdu_collection.c` file when you compile your ASN.1 specification files.
  It defines the various types of encodings that can be made when using the compiled ASN.1 specification files. This file 
  also contains an array of pointers to these structures. The structures contain two useful fields for the purpose of discovering
  based on their ASN.1 specification name.
    - The type of these structures is `asn_TYPE_descriptor_t`. The type definition is in the file, `constr_TYPE.h`, in the directory 
      where you compiled the ASN.1 specification
    - The `name` field is the name used in the ASN.1 specification. If you compile several ASN.1 specification names can collide; 
      the asn1c type name is name mangled to avoid collision.
    - The `elements_count` field contains the number of elements in the ASN.1 type. This is used to deconflict some name collisions. 
      The AEM (encoder) will only process types that have more than 1 element.
- The AEM builds an associative array mapping ASN.1 specification type names to the structures needed for encoding.  The names must be used
  in the provided XML file. This is the way the AEM will discern HOW to encode the information provided by the producer of the XML document message.
    - It would be nice for the name of the mangled name of the structure to be included in the structure, possibly the name string, but this
      may create other unknown problems in asn1c.

# Produced Encodings and Decodings
- When the ACM completes its task, it will produce the result as an XML document containing the same elements as it was sent with the following exceptions:
    - When a message is encoded, the XML will be encoded into binary, transformed into a hex string and written to the `<payload>` `<data>` child element.
    - When a message is encoded, the `<payload>` `<dataType>` element will be changed to: `us.dot.its.jpo.ode.model.OdeHexByteArray`
    - When a message is decoded, the hex string will be converted into binary, decoded, and the XER (XML) written to the `<payload>` `<data>` child element.
    - When a message is decoded, the `<payload>` `<dataType>` element will be changed to the name of the decoded data element, e.g., `MessageFrame`

