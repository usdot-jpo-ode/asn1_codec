/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "SEMI"
 * 	found in "SEMI_v2.3.0_070616.asn"
 * 	`asn1c -gen-OER -gen-PER`
 */

#ifndef	_ObjectRegistrationData_H_
#define	_ObjectRegistrationData_H_


#include <asn_application.h>

/* Including external dependencies */
#include "SemiDialogID.h"
#include "SemiSequenceID.h"
#include "GroupID.h"
#include "TemporaryID.h"
#include "ServiceID.h"
#include "ServiceRecord.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ObjectRegistrationData */
typedef struct ObjectRegistrationData {
	SemiDialogID_t	 dialogID;
	SemiSequenceID_t	 seqID;
	GroupID_t	 groupID;
	TemporaryID_t	 requestID;
	ServiceID_t	 serviceID;
	ServiceRecord_t	 serviceRecord;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ObjectRegistrationData_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ObjectRegistrationData;

#ifdef __cplusplus
}
#endif

#endif	/* _ObjectRegistrationData_H_ */
#include <asn_internal.h>