/**
 * Start building a header for library functions we need in the Kafka module.
 * This should be the only include needed and then link with the library.
 *
 */

#ifndef _ASN1_J2735_LIB_H
#define _ASN1_J2735_LIB_H

#include <asn_internal.h>    // Includes asn_application.h 
                             // for ASN__DEFAULT_STACK_MAX

#ifdef __cplusplus
extern "C" {
#endif

/* Convert "Type" defined by -DPDU=<type> into "asn_DEF_<type>" */
#define    ASN_DEF_PDU(t)    asn_DEF_ ## t
#define    DEF_PDU_Type(t)    ASN_DEF_PDU(t)
#define    PDU_Type    DEF_PDU_Type(PDU)

// Generic name to a preprocessor defined specific ASN.1 type.
// Example: -DPDU=MessageFrame for J2735. The specific type name (the transformation of PDU_Type) becomes asn_DEF_MessageFrame using the macros above.
// See MessageFrame.c for the actual type in this case.
// the asn_TYPE_descriptor_s structure (and asn_TYPE_descriptor_t typedef) are found in constr_TYPE.h
extern asn_TYPE_descriptor_t PDU_Type;    /* ASN.1 type to be decoded */



// For reference. In the specific case, this is what the above is declaring has been created.
// asn_TYPE_descriptor_t asn_DEF_MessageFrame = {
// 	"MessageFrame",
// 	"MessageFrame",
// 	&asn_OP_SEQUENCE,       // ===> see MessageFrame.c
// 	SEQUENCE_constraint,
// 	asn_DEF_MessageFrame_tags_1,
// 	sizeof(asn_DEF_MessageFrame_tags_1)
// 		/sizeof(asn_DEF_MessageFrame_tags_1[0]), /* 1 */
// 	asn_DEF_MessageFrame_tags_1,	/* Same as above */
// 	sizeof(asn_DEF_MessageFrame_tags_1)
// 		/sizeof(asn_DEF_MessageFrame_tags_1[0]), /* 1 */
// 	0,	/* No OER visible constraints */
// 	0,	/* No PER visible constraints */
// 	asn_MBR_MessageFrame_1,
// 	2,	/* Elements count */
// 	&asn_SPC_MessageFrame_specs_1	/* Additional specs */
// };

extern int opt_debug;    /* -d (or -dd) */

// Input data format selector.
// NOTE: Moved these from the converter file with main to move more toward a lib.
enum input_format {
    INP_BER,    /* -iber: BER (Basic Encoding Rules) input */
    INP_XER,    /* -ixer: XER (XML Encoding Rules) input */
    INP_OER,    /* -ioer: OER (Octet Encoding Rules) input */
    INP_PER     /* -iper: UPER (Unaligned Packed Encoding Rules) input */
} iform;    /* -i<format> */

// Output data format selector.
// NOTE: Moved these from the converter file with main to move more toward a lib.
enum output_format {
    OUT_XER,    /* -oxer: XER (XML) output */
    OUT_DER,    /* -oder: DER (BER) output */
    OUT_OER,    /* -ooer: Canonical OER output */
    OUT_PER,    /* -oper: Unaligned PER output */
    OUT_TEXT,   /* -otext: semi-structured text */
    OUT_NULL    /* -onull: No pretty-printing */
} oform;    /* -o<format> */

/**
 * In xer_encoder.h just didn't include the header.
 * The variant of the above function which dumps the BASIC-XER (XER_F_BASIC)
 * output into the chosen file pointer.
 * RETURN VALUES:
 * 	 0: The structure is printed.
 * 	-1: Problem printing the structure.
 * WARNING: No sensible errno value is returned.
 */
extern int xer_fprint(FILE *stream, struct asn_TYPE_descriptor_s *td, void *sptr);

/**
 * Building a buffer decoder, build buffer from kafka and decode.
 */
void *data_decode_from_buffer(asn_TYPE_descriptor_t *pduType, const uint8_t *buffer, ssize_t bufsize, int on_first_pdu);

/**
 * Original decode a file function. Removed static so we can make this a library.
 */
void *data_decode_from_file(asn_TYPE_descriptor_t *pduType, FILE *file, const char *name, ssize_t suggested_bufsize, int on_first_pdu);

/**
 * Shadows the structure in xer_encoder.c
 */
struct xer_buffer {
    char *buffer;
    size_t buffer_size;      // this is really where we will write next.
    size_t allocated_size;   // this is the total size of the buffer.
};

/**
 * This is somewhat tightly coupled with xer_encoder.h/c.  Putting it in lib so we don't have to put it in the asn1
 * repository; these functions get overwritten when you recompile the ASN.1 specification.
 */
int xer_buf(void *buf, struct asn_TYPE_descriptor_s *td, void *sptr);
int xer_buffer_append(const void *buffer, size_t size, void *app_key);

#ifdef __cplusplus
}
#endif

#endif
