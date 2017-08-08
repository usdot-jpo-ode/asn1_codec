/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "IEEE1609dot2BaseTypes"
 * 	found in "1609dot2-base-types.asn"
 * 	`asn1c -gen-OER`
 */

#ifndef	_PublicVerificationKey_H_
#define	_PublicVerificationKey_H_


#include <asn_application.h>

/* Including external dependencies */
#include "EccP256CurvePoint.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PublicVerificationKey_PR {
	PublicVerificationKey_PR_NOTHING,	/* No components present */
	PublicVerificationKey_PR_ecdsaNistP256,
	PublicVerificationKey_PR_ecdsaBrainpoolP256r1
	/* Extensions may appear below */
	
} PublicVerificationKey_PR;

/* PublicVerificationKey */
typedef struct PublicVerificationKey {
	PublicVerificationKey_PR present;
	union PublicVerificationKey_u {
		EccP256CurvePoint_t	 ecdsaNistP256;
		EccP256CurvePoint_t	 ecdsaBrainpoolP256r1;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PublicVerificationKey_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PublicVerificationKey;
extern asn_CHOICE_specifics_t asn_SPC_PublicVerificationKey_specs_1;
extern asn_TYPE_member_t asn_MBR_PublicVerificationKey_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _PublicVerificationKey_H_ */
#include <asn_internal.h>