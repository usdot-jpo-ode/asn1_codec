/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "IEEE1609dot2BaseTypes"
 * 	found in "1609dot2-base-types.asn"
 * 	`asn1c -gen-OER -gen-PER`
 */

#include "IEEE1609dot2BaseTypes_Psid.h"

int
IEEE1609dot2BaseTypes_Psid_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using NativeInteger,
 * so here we adjust the DEF accordingly.
 */
static asn_oer_constraints_t asn_OER_type_IEEE1609dot2BaseTypes_Psid_constr_1 GCC_NOTUSED = {
	{ 0, 0 }	/* (0..MAX) */,
	-1};
asn_per_constraints_t asn_PER_type_IEEE1609dot2BaseTypes_Psid_constr_1 GCC_NOTUSED = {
	{ APC_SEMI_CONSTRAINED,	-1, -1,  0,  0 }	/* (0..MAX) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static const ber_tlv_tag_t asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_IEEE1609dot2BaseTypes_Psid = {
	"Psid",
	"Psid",
	NativeInteger_free,
	NativeInteger_print,
	IEEE1609dot2BaseTypes_Psid_constraint,
	NativeInteger_decode_ber,
	NativeInteger_encode_der,
	NativeInteger_decode_xer,
	NativeInteger_encode_xer,
	NativeInteger_decode_oer,
	NativeInteger_encode_oer,
	NativeInteger_decode_uper,
	NativeInteger_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1,
	sizeof(asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1)
		/sizeof(asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1[0]), /* 1 */
	asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1,	/* Same as above */
	sizeof(asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1)
		/sizeof(asn_DEF_IEEE1609dot2BaseTypes_Psid_tags_1[0]), /* 1 */
	&asn_OER_type_IEEE1609dot2BaseTypes_Psid_constr_1,
	&asn_PER_type_IEEE1609dot2BaseTypes_Psid_constr_1,
	0, 0,	/* No members */
	0	/* No specifics */
};
