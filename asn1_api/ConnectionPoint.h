/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "SEMI"
 * 	found in "SEMI_v2.3.0_070616.asn"
 * 	`asn1c -gen-OER -gen-PER`
 */

#ifndef	_ConnectionPoint_H_
#define	_ConnectionPoint_H_


#include <asn_application.h>

/* Including external dependencies */
#include "PortNumber.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct IpAddress;

/* ConnectionPoint */
typedef struct ConnectionPoint {
	struct IpAddress	*address	/* OPTIONAL */;
	PortNumber_t	 port;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ConnectionPoint_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ConnectionPoint;
extern asn_SEQUENCE_specifics_t asn_SPC_ConnectionPoint_specs_1;
extern asn_TYPE_member_t asn_MBR_ConnectionPoint_1[2];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "IpAddress.h"

#endif	/* _ConnectionPoint_H_ */
#include <asn_internal.h>