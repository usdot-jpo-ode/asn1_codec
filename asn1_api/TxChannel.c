/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "SEMI"
 * 	found in "SEMI_v2.3.0_070616.asn"
 * 	`asn1c -gen-OER -gen-PER`
 */

#include "TxChannel.h"

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
static asn_oer_constraints_t asn_OER_type_TxChannel_constr_1 GCC_NOTUSED = {
	{ 0, 0 },
	-1};
asn_per_constraints_t asn_PER_type_TxChannel_constr_1 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 4,  4,  0,  8 }	/* (0..8) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static const asn_INTEGER_enum_map_t asn_MAP_TxChannel_value2enum_1[] = {
	{ 0,	9,	"chControl" },
	{ 1,	9,	"chService" },
	{ 2,	5,	"ch172" },
	{ 3,	5,	"ch174" },
	{ 4,	5,	"ch176" },
	{ 5,	5,	"ch178" },
	{ 6,	5,	"ch180" },
	{ 7,	5,	"ch182" },
	{ 8,	5,	"ch184" }
};
static const unsigned int asn_MAP_TxChannel_enum2value_1[] = {
	2,	/* ch172(2) */
	3,	/* ch174(3) */
	4,	/* ch176(4) */
	5,	/* ch178(5) */
	6,	/* ch180(6) */
	7,	/* ch182(7) */
	8,	/* ch184(8) */
	0,	/* chControl(0) */
	1	/* chService(1) */
};
const asn_INTEGER_specifics_t asn_SPC_TxChannel_specs_1 = {
	asn_MAP_TxChannel_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_TxChannel_enum2value_1,	/* N => "tag"; sorted by N */
	9,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static const ber_tlv_tag_t asn_DEF_TxChannel_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_TxChannel = {
	"TxChannel",
	"TxChannel",
	NativeEnumerated_free,
	NativeEnumerated_print,
	NativeEnumerated_constraint,
	NativeEnumerated_decode_ber,
	NativeEnumerated_encode_der,
	NativeEnumerated_decode_xer,
	NativeEnumerated_encode_xer,
	NativeEnumerated_decode_oer,
	NativeEnumerated_encode_oer,
	NativeEnumerated_decode_uper,
	NativeEnumerated_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_TxChannel_tags_1,
	sizeof(asn_DEF_TxChannel_tags_1)
		/sizeof(asn_DEF_TxChannel_tags_1[0]), /* 1 */
	asn_DEF_TxChannel_tags_1,	/* Same as above */
	sizeof(asn_DEF_TxChannel_tags_1)
		/sizeof(asn_DEF_TxChannel_tags_1[0]), /* 1 */
	&asn_OER_type_TxChannel_constr_1,
	&asn_PER_type_TxChannel_constr_1,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_TxChannel_specs_1	/* Additional specs */
};
