/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "IEEE1609dot2"
 * 	found in "1609dot2-schema.asn"
 * 	`asn1c -gen-OER -gen-PER`
 */

#ifndef	_SymmRecipientInfo_H_
#define	_SymmRecipientInfo_H_


#include <asn_application.h>

/* Including external dependencies */
#include "HashedId8.h"
#include "SymmetricCiphertext.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SymmRecipientInfo */
typedef struct SymmRecipientInfo {
	HashedId8_t	 recipientId;
	SymmetricCiphertext_t	 encKey;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SymmRecipientInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SymmRecipientInfo;
extern asn_SEQUENCE_specifics_t asn_SPC_SymmRecipientInfo_specs_1;
extern asn_TYPE_member_t asn_MBR_SymmRecipientInfo_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _SymmRecipientInfo_H_ */
#include <asn_internal.h>