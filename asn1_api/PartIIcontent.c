/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "J2735_201603DA.asn"
 * 	`asn1c -gen-OER -gen-PER`
 */

#include "PartIIcontent.h"

static int
memb_partII_Id_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 63)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_partII_Value_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	
	if(1 /* No applicable constraints whatsoever */) {
		/* Nothing is here. See below */
	}
	
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

static asn_oer_constraints_t asn_OER_memb_partII_Id_constr_2 GCC_NOTUSED = {
	{ 1, 1 }	/* (0..63) */,
	-1};
static asn_per_constraints_t asn_PER_memb_partII_Id_constr_2 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 6,  6,  0,  63 }	/* (0..63) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_oer_constraints_t asn_OER_memb_partII_Value_constr_3 GCC_NOTUSED = {
	{ 0, 0 },
	-1};
static asn_per_constraints_t asn_PER_memb_partII_Value_constr_3 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_TYPE_member_t asn_MBR_PartIIcontent_151P0_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PartIIcontent_151P0, partII_Id),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PartII_Id,
		memb_partII_Id_constraint_1,
		&asn_OER_memb_partII_Id_constr_2,
		&asn_PER_memb_partII_Id_constr_2,
		0,
		"partII-Id"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PartIIcontent_151P0, partII_Value),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ANY,
		memb_partII_Value_constraint_1,
		&asn_OER_memb_partII_Value_constr_3,
		&asn_PER_memb_partII_Value_constr_3,
		0,
		"partII-Value"
		},
};
static const ber_tlv_tag_t asn_DEF_PartIIcontent_151P0_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_PartIIcontent_151P0_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* partII-Id */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* partII-Value */
};
asn_SEQUENCE_specifics_t asn_SPC_PartIIcontent_151P0_specs_1 = {
	sizeof(struct PartIIcontent_151P0),
	offsetof(struct PartIIcontent_151P0, _asn_ctx),
	asn_MAP_PartIIcontent_151P0_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_PartIIcontent_151P0 = {
	"PartIIcontent",
	"PartIIcontent",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_oer,
	SEQUENCE_encode_oer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PartIIcontent_151P0_tags_1,
	sizeof(asn_DEF_PartIIcontent_151P0_tags_1)
		/sizeof(asn_DEF_PartIIcontent_151P0_tags_1[0]), /* 1 */
	asn_DEF_PartIIcontent_151P0_tags_1,	/* Same as above */
	sizeof(asn_DEF_PartIIcontent_151P0_tags_1)
		/sizeof(asn_DEF_PartIIcontent_151P0_tags_1[0]), /* 1 */
	0,	/* No OER visible constraints */
	0,	/* No PER visible constraints */
	asn_MBR_PartIIcontent_151P0_1,
	2,	/* Elements count */
	&asn_SPC_PartIIcontent_151P0_specs_1	/* Additional specs */
};
