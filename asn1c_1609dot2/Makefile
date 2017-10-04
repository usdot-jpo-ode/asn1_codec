CC=gcc

ASN_SCHEMAS= 1609dot2-base-types.asn 1609dot2-schema.asn

ASN_MODULE_SOURCES=	\
   AesCcmCiphertext.c \
   asn_application.c \
   asn_bit_data.c \
   asn_codecs_prim.c \
   asn_random_fill.c \
   asn_SEQUENCE_OF.c \
   asn_SET_OF.c \
   BasePublicEncryptionKey.c \
   ber_decoder.c \
   ber_tlv_length.c \
   ber_tlv_tag.c \
   BitmapSsp.c \
   BitmapSspRange.c \
   BIT_STRING.c \
   BIT_STRING_oer.c \
   BOOLEAN.c \
   CertificateBase.c \
   Certificate.c \
   CertificateId.c \
   CertificateType.c \
   CircularRegion.c \
   constraints.c \
   constr_CHOICE.c \
   constr_CHOICE_oer.c \
   constr_SEQUENCE.c \
   constr_SEQUENCE_oer.c \
   constr_SEQUENCE_OF.c \
   constr_SET_OF.c \
   constr_SET_OF_oer.c \
   constr_TYPE.c \
   Countersignature.c \
   CountryAndRegions.c \
   CountryAndSubregions.c \
   CountryOnly.c \
   CrlSeries.c \
   der_encoder.c \
   Duration.c \
   EccP256CurvePoint.c \
   EccP384CurvePoint.c \
   EcdsaP256Signature.c \
   EcdsaP384Signature.c \
   EciesP256EncryptedKey.c \
   Elevation.c \
   ElevInt.c \
   EncryptedData.c \
   EncryptedDataEncryptionKey.c \
   EncryptionKey.c \
   EndEntityType.c \
   ExplicitCertificate.c \
   GeographicRegion.c \
   GroupLinkageValue.c \
   HashAlgorithm.c \
   HashedData.c \
   HashedId10.c \
   HashedId3.c \
   HashedId8.c \
   HeaderInfo.c \
   Hostname.c \
   IdentifiedRegion.c \
   Ieee1609Dot2Content.c \
   Ieee1609Dot2Data.c \
   ImplicitCertificate.c \
   INTEGER.c \
   INTEGER_oer.c \
   IssuerIdentifier.c \
   IValue.c \
   KnownLatitude.c \
   KnownLongitude.c \
   LaId.c \
   Latitude.c \
   LinkageData.c \
   LinkageSeed.c \
   LinkageValue.c \
   Longitude.c \
   MissingCrlIdentifier.c \
   NativeEnumerated.c \
   NativeEnumerated_oer.c \
   NativeInteger.c \
   NativeInteger_oer.c \
   NinetyDegreeInt.c \
   NULL.c \
   OCTET_STRING.c \
   OCTET_STRING_oer.c \
   oer_decoder.c \
   oer_encoder.c \
   oer_support.c \
   OneEightyDegreeInt.c \
   Opaque.c \
   OPEN_TYPE.c \
   OPEN_TYPE_oer.c \
   pdu_collection.c \
   per_decoder.c \
   per_encoder.c \
   per_opentype.c \
   per_support.c \
   PKRecipientInfo.c \
   PolygonalRegion.c \
   PreSharedKeyRecipientInfo.c \
   Psid.c \
   PsidGroupPermissions.c \
   PsidSsp.c \
   PsidSspRange.c \
   PublicEncryptionKey.c \
   PublicVerificationKey.c \
   RecipientInfo.c \
   RectangularRegion.c \
   RegionAndSubregions.c \
   SequenceOfCertificate.c \
   SequenceOfHashedId3.c \
   SequenceOfIdentifiedRegion.c \
   SequenceOfOctetString.c \
   SequenceOfPsid.c \
   SequenceOfPsidGroupPermissions.c \
   SequenceOfPsidSsp.c \
   SequenceOfPsidSspRange.c \
   SequenceOfRecipientInfo.c \
   SequenceOfRectangularRegion.c \
   SequenceOfRegionAndSubregions.c \
   SequenceOfUint16.c \
   SequenceOfUint8.c \
   ServiceSpecificPermissions.c \
   Signature.c \
   SignedData.c \
   SignedDataPayload.c \
   SignerIdentifier.c \
   SspRange.c \
   SubjectAssurance.c \
   SubjectPermissions.c \
   SymmAlgorithm.c \
   SymmetricCiphertext.c \
   SymmetricEncryptionKey.c \
   SymmRecipientInfo.c \
   ThreeDLocation.c \
   Time32.c \
   Time64.c \
   ToBeSignedCertificate.c \
   ToBeSignedData.c \
   TwoDLocation.c \
   Uint16.c \
   Uint32.c \
   Uint3.c \
   Uint64.c \
   Uint8.c \
   UnknownLatitude.c \
   UnknownLongitude.c \
   UTF8String.c \
   ValidityPeriod.c \
   VerificationKeyIndicator.c \
   xer_decoder.c \
   xer_encoder.c \
   xer_support.c \



# 	Uint3.c	\
# 	Uint8.c	\
# 	Uint16.c	\
# 	Uint32.c	\
# 	Uint64.c	\
# 	SequenceOfUint8.c	\
# 	SequenceOfUint16.c	\
# 	Opaque.c	\
# 	HashedId32.c	\
# 	HashedId10.c	\
# 	HashedId8.c	\
# 	HashedId4.c	\
# 	HashedId3.c	\
# 	Time32.c	\
# 	Time64.c	\
# 	ValidityPeriod.c	\
# 	Duration.c	\
# 	GeographicRegion.c	\
# 	CircularRegion.c	\
# 	RectangularRegion.c	\
# 	SequenceOfRectangularRegion.c	\
# 	PolygonalRegion.c	\
# 	TwoDLocation.c	\
# 	IdentifiedRegion.c	\
# 	SequenceOfIdentifiedRegion.c	\
# 	CountryOnly.c	\
# 	CountryAndRegions.c	\
# 	CountryAndSubregions.c	\
# 	RegionAndSubregions.c	\
# 	SequenceOfRegionAndSubregions.c	\
# 	ThreeDLocation.c	\
# 	Latitude.c	\
# 	Longitude.c	\
# 	Elevation.c	\
# 	NinetyDegreeInt.c	\
# 	KnownLatitude.c	\
# 	UnknownLatitude.c	\
# 	OneEightyDegreeInt.c	\
# 	KnownLongitude.c	\
# 	UnknownLongitude.c	\
# 	ElevInt.c	\
# 	Signature.c	\
# 	EcdsaP256Signature.c	\
# 	EccP256CurvePoint.c	\
# 	SymmAlgorithm.c	\
# 	HashAlgorithm.c	\
# 	EciesP256EncryptedKey.c	\
# 	EncryptionKey.c	\
# 	PublicEncryptionKey.c	\
# 	BasePublicEncryptionKey.c	\
# 	PublicVerificationKey.c	\
# 	SymmetricEncryptionKey.c	\
# 	PsidSsp.c	\
# 	SequenceOfPsidSsp.c	\
# 	Psid.c	\
# 	SequenceOfPsid.c	\
# 	ServiceSpecificPermissions.c	\
# 	PsidSspRange.c	\
# 	SequenceOfPsidSspRange.c	\
# 	SspRange.c	\
# 	SequenceOfOctetString.c	\
# 	SubjectAssurance.c	\
# 	CrlSeries.c	\
# 	IValue.c	\
# 	Hostname.c	\
# 	LinkageValue.c	\
# 	GroupLinkageValue.c	\
# 	LaId.c	\
# 	LinkageSeed.c	\
# 	SignedDataPayload.c	\
# 	Ieee1609Dot2Data.c	\
# 	Ieee1609Dot2Content.c	\
# 	SignedData.c	\
# 	SignerIdentifier.c	\
# 	ToBeSignedData.c	\
# 	HashedData.c	\
# 	HeaderInfo.c	\
# 	MissingCrlIdentifier.c	\
# 	Countersignature.c	\
# 	EncryptedData.c	\
# 	RecipientInfo.c	\
# 	SequenceOfRecipientInfo.c	\
# 	PreSharedKeyRecipientInfo.c	\
# 	SymmRecipientInfo.c	\
# 	PKRecipientInfo.c	\
# 	EncryptedDataEncryptionKey.c	\
# 	SymmetricCiphertext.c	\
# 	AesCcmCiphertext.c	\
# 	Certificate.c	\
# 	SequenceOfCertificate.c	\
# 	CertificateBase.c	\
# 	CertificateType.c	\
# 	ImplicitCertificate.c	\
# 	ExplicitCertificate.c	\
# 	IssuerIdentifier.c	\
# 	ToBeSignedCertificate.c	\
# 	CertificateId.c	\
# 	LinkageData.c	\
# 	EndEntityType.c	\
# 	PsidGroupPermissions.c	\
# 	SequenceOfPsidGroupPermissions.c	\
# 	SubjectPermissions.c	\
# 	VerificationKeyIndicator.c
# 
# ASN_MODULE_HEADERS=	\
# 	Uint3.h	\
# 	Uint8.h	\
# 	Uint16.h	\
# 	Uint32.h	\
# 	Uint64.h	\
# 	SequenceOfUint8.h	\
# 	SequenceOfUint16.h	\
# 	Opaque.h	\
# 	HashedId32.h	\
# 	HashedId10.h	\
# 	HashedId8.h	\
# 	HashedId4.h	\
# 	HashedId3.h	\
# 	Time32.h	\
# 	Time64.h	\
# 	ValidityPeriod.h	\
# 	Duration.h	\
# 	GeographicRegion.h	\
# 	CircularRegion.h	\
# 	RectangularRegion.h	\
# 	SequenceOfRectangularRegion.h	\
# 	PolygonalRegion.h	\
# 	TwoDLocation.h	\
# 	IdentifiedRegion.h	\
# 	SequenceOfIdentifiedRegion.h	\
# 	CountryOnly.h	\
# 	CountryAndRegions.h	\
# 	CountryAndSubregions.h	\
# 	RegionAndSubregions.h	\
# 	SequenceOfRegionAndSubregions.h	\
# 	ThreeDLocation.h	\
# 	Latitude.h	\
# 	Longitude.h	\
# 	Elevation.h	\
# 	NinetyDegreeInt.h	\
# 	KnownLatitude.h	\
# 	UnknownLatitude.h	\
# 	OneEightyDegreeInt.h	\
# 	KnownLongitude.h	\
# 	UnknownLongitude.h	\
# 	ElevInt.h	\
# 	Signature.h	\
# 	EcdsaP256Signature.h	\
# 	EccP256CurvePoint.h	\
# 	SymmAlgorithm.h	\
# 	HashAlgorithm.h	\
# 	EciesP256EncryptedKey.h	\
# 	EncryptionKey.h	\
# 	PublicEncryptionKey.h	\
# 	BasePublicEncryptionKey.h	\
# 	PublicVerificationKey.h	\
# 	SymmetricEncryptionKey.h	\
# 	PsidSsp.h	\
# 	SequenceOfPsidSsp.h	\
# 	Psid.h	\
# 	SequenceOfPsid.h	\
# 	ServiceSpecificPermissions.h	\
# 	PsidSspRange.h	\
# 	SequenceOfPsidSspRange.h	\
# 	SspRange.h	\
# 	SequenceOfOctetString.h	\
# 	SubjectAssurance.h	\
# 	CrlSeries.h	\
# 	IValue.h	\
# 	Hostname.h	\
# 	LinkageValue.h	\
# 	GroupLinkageValue.h	\
# 	LaId.h	\
# 	LinkageSeed.h	\
# 	SignedDataPayload.h	\
# 	Ieee1609Dot2Data.h	\
# 	Ieee1609Dot2Content.h	\
# 	SignedData.h	\
# 	SignerIdentifier.h	\
# 	ToBeSignedData.h	\
# 	HashedData.h	\
# 	HeaderInfo.h	\
# 	MissingCrlIdentifier.h	\
# 	Countersignature.h	\
# 	EncryptedData.h	\
# 	RecipientInfo.h	\
# 	SequenceOfRecipientInfo.h	\
# 	PreSharedKeyRecipientInfo.h	\
# 	SymmRecipientInfo.h	\
# 	PKRecipientInfo.h	\
# 	EncryptedDataEncryptionKey.h	\
# 	SymmetricCiphertext.h	\
# 	AesCcmCiphertext.h	\
# 	Certificate.h	\
# 	SequenceOfCertificate.h	\
# 	CertificateBase.h	\
# 	CertificateType.h	\
# 	ImplicitCertificate.h	\
# 	ExplicitCertificate.h	\
# 	IssuerIdentifier.h	\
# 	ToBeSignedCertificate.h	\
# 	CertificateId.h	\
# 	LinkageData.h	\
# 	EndEntityType.h	\
# 	PsidGroupPermissions.h	\
# 	SequenceOfPsidGroupPermissions.h	\
# 	SubjectPermissions.h	\
# 	VerificationKeyIndicator.h
# 
# ASN_MODULE_HEADERS+=OPEN_TYPE.h
# ASN_MODULE_SOURCES+=OPEN_TYPE.c
# ASN_MODULE_HEADERS+=constr_CHOICE.h
# ASN_MODULE_HEADERS+=UTF8String.h
# ASN_MODULE_HEADERS+=BOOLEAN.h
# ASN_MODULE_SOURCES+=BOOLEAN.c
# ASN_MODULE_HEADERS+=INTEGER.h
# ASN_MODULE_HEADERS+=NativeEnumerated.h
# ASN_MODULE_SOURCES+=INTEGER.c
# ASN_MODULE_HEADERS+=NULL.h
# ASN_MODULE_SOURCES+=NULL.c
# ASN_MODULE_SOURCES+=NativeEnumerated.c
# ASN_MODULE_HEADERS+=NativeInteger.h
# ASN_MODULE_SOURCES+=NativeInteger.c
# ASN_MODULE_SOURCES+=UTF8String.c
# ASN_MODULE_HEADERS+=asn_SEQUENCE_OF.h
# ASN_MODULE_SOURCES+=asn_SEQUENCE_OF.c
# ASN_MODULE_HEADERS+=asn_SET_OF.h
# ASN_MODULE_SOURCES+=asn_SET_OF.c
# ASN_MODULE_SOURCES+=constr_CHOICE.c
# ASN_MODULE_HEADERS+=constr_SEQUENCE.h
# ASN_MODULE_SOURCES+=constr_SEQUENCE.c
# ASN_MODULE_HEADERS+=constr_SEQUENCE_OF.h
# ASN_MODULE_SOURCES+=constr_SEQUENCE_OF.c
# ASN_MODULE_HEADERS+=constr_SET_OF.h
# ASN_MODULE_SOURCES+=constr_SET_OF.c
# ASN_MODULE_HEADERS+=asn_application.h
# ASN_MODULE_HEADERS+=asn_ioc.h
# ASN_MODULE_HEADERS+=asn_system.h
# ASN_MODULE_HEADERS+=asn_codecs.h
# ASN_MODULE_HEADERS+=asn_internal.h
# ASN_MODULE_HEADERS+=asn_bit_data.h
# ASN_MODULE_SOURCES+=asn_bit_data.c
# ASN_MODULE_HEADERS+=OCTET_STRING.h
# ASN_MODULE_SOURCES+=OCTET_STRING.c
# ASN_MODULE_HEADERS+=BIT_STRING.h
# ASN_MODULE_SOURCES+=BIT_STRING.c
# ASN_MODULE_SOURCES+=asn_codecs_prim.c
# ASN_MODULE_HEADERS+=asn_codecs_prim.h
# ASN_MODULE_HEADERS+=ber_tlv_length.h
# ASN_MODULE_SOURCES+=ber_tlv_length.c
# ASN_MODULE_HEADERS+=ber_tlv_tag.h
# ASN_MODULE_SOURCES+=ber_tlv_tag.c
# ASN_MODULE_HEADERS+=ber_decoder.h
# ASN_MODULE_SOURCES+=ber_decoder.c
# ASN_MODULE_HEADERS+=der_encoder.h
# ASN_MODULE_SOURCES+=der_encoder.c
# ASN_MODULE_HEADERS+=constr_TYPE.h
# ASN_MODULE_SOURCES+=constr_TYPE.c
# ASN_MODULE_HEADERS+=constraints.h
# ASN_MODULE_SOURCES+=constraints.c
# ASN_MODULE_HEADERS+=xer_support.h
# ASN_MODULE_SOURCES+=xer_support.c
# ASN_MODULE_HEADERS+=xer_decoder.h
# ASN_MODULE_SOURCES+=xer_decoder.c
# ASN_MODULE_HEADERS+=xer_encoder.h
# ASN_MODULE_SOURCES+=xer_encoder.c
# ASN_MODULE_HEADERS+=per_support.h
# ASN_MODULE_SOURCES+=per_support.c
# ASN_MODULE_HEADERS+=per_decoder.h
# ASN_MODULE_SOURCES+=per_decoder.c
# ASN_MODULE_HEADERS+=per_encoder.h
# ASN_MODULE_SOURCES+=per_encoder.c
# ASN_MODULE_HEADERS+=per_opentype.h
# ASN_MODULE_SOURCES+=per_opentype.c
# ASN_MODULE_HEADERS+=oer_decoder.h
# ASN_MODULE_SOURCES+=oer_decoder.c
# ASN_MODULE_HEADERS+=oer_encoder.h
# ASN_MODULE_SOURCES+=oer_encoder.c
# ASN_MODULE_HEADERS+=oer_support.h
# ASN_MODULE_SOURCES+=oer_support.c
# ASN_MODULE_SOURCES+=OPEN_TYPE_oer.c
# ASN_MODULE_SOURCES+=INTEGER_oer.c
# ASN_MODULE_SOURCES+=OCTET_STRING_oer.c
# ASN_MODULE_SOURCES+=NativeInteger_oer.c
# ASN_MODULE_SOURCES+=NativeEnumerated_oer.c
# ASN_MODULE_SOURCES+=constr_SEQUENCE_oer.c
# ASN_MODULE_SOURCES+=constr_CHOICE_oer.c
# ASN_MODULE_SOURCES+=constr_SET_OF_oer.c
# ASN_MODULE_SOURCES+=BIT_STRING_oer.c

ASN_MODULE_HEADERS=	${ASN_MODULE_SOURCES:.c=.h}

# ASN_CONVERTER_SOURCES+=converter-sample.c
# ASN_CONVERTER_SOURCES+=pdu_collection.c

1609DOT2SRCDIR=../1609dot2/src
1609DOT2LIBDIR=../1609dot2/lib
1609DOT2INCDIR=../1609dot2/include

# ASN1C_CONVERTER_CLI_SOURCE=${1609DOT2SRCDIR}/asn1-module-converter.c
# ASN1C_CONVERTER_LIB_SOURCE=${1609DOT2SRCDIR}/asn1-1609dot2-lib.c
ASN1C_CONVERTER_CLI_SOURCE=converter-example.c

# This file may be used as an input for make(3)
# Remove the lines below to convert it into a pure .am file
# TARGET = 1609-2-dump
TARGET = 1609dot2
TARGET_1609DOT2_CLI = ${TARGET}-parse
# TARGET_1609DOT2_LIB = ${1609DOT2LIBDIR}/lib${TARGET}.a

CFLAGS +=

CPPFLAGS += -DASN_CONVERTER_TITLE="IEEE 1609.2-2016 decoder" 
CPPFLAGS += -DJUNKTEST 
CPPFLAGS += -D_DEFAULT_SOURCE  
CPPFLAGS += -DASN_DISABLE_PER_SUPPORT 
CPPFLAGS += -DPDU=Ieee1609Dot2Data 
CPPFLAGS += -DASN_PDU_COLLECTION
CPPFLAGS += -I.
CPPFLAGS += -I${1609DOT2INCDIR}

# LDFLAGS += -L${1609DOT2LIBDIR}

# LIBS += -l${TARGET}

OBJS=${ASN_MODULE_SOURCES:.c=.o} ${ASN1C_CONVERTER_CLI_SOURCE:.c=.o}

all: SETUP Ieee1609Dot2Data.c $(TARGET_1609DOT2_CLI) # $(TARGET_1609DOT2_LIB)

# Build the objects using new wildcard syntax.
%.o: %.c %.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

SETUP: 
	mkdir -p $(1609DOT2LIBDIR)

$(TARGET_1609DOT2_CLI): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TARGET_1609DOT2_CLI) ${OBJS} $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET_1609DOT2_CLI)
	rm -f $(TARGET_1609DOT2_LIB)
	rm -f $(OBJS)

regen: regenerate-from-asn1-source

regenerate-from-asn1-source: $(ASN_SCHEMAS)
	asn1c -pdu=Ieee1609Dot2Data -pdu=auto -fcompound-names -gen-OER $(ASN_SCHEMAS)

Ieee1609Dot2Data.c: $(ASN_SCHEMAS)
	make regen
	@touch Ieee1609Dot2Data.c
	make

# Build the library for the J2735 functions we will use in the Kafka module.
$(TARGET_1609DOT2_LIB): $(OBJS)
	ar -rcs $@ $(OBJS)

check-per:
	@if test -f sample-Ieee1609Dot2Data-1-nopad.per ; then \
	for f in sample-Ieee1609Dot2Data-[1-9]-nopad.per; do \
	for b in 1 17 33 980 8192; do \
	echo "Recoding non-padded $$f ($$pdu) into DER into XER and back ($$b)..."; \
	./${TARGET} -p $$pdu -b $$b -per-nopad -iper -oder $$f > ./.tmp.1.$$$$ || exit 2; \
	./${TARGET} -p $$pdu -b $$b -iber -oxer ./.tmp.1.$$$$ > ./.tmp.2.$$$$ || exit 3; \
	./${TARGET} -p $$pdu -b $$b -ixer -oder ./.tmp.2.$$$$ > ./.tmp.3.$$$$ || exit 4; \
	diff ./.tmp.1.$$$$ ./.tmp.3.$$$$ || exit 5; \
	rm -f ./.tmp.[123].$$$$; \
	echo "Test junking $$f (please wait)..."; \
	./${TARGET} -J0.0001 -n 1000 -p $$pdu -b $$b -per-nopad -iper -onull $$f || exit 6; \
	./${TARGET} -J0.001 -n 1000 -p $$pdu -b $$b -per-nopad -iper -onull $$f || exit 7; \
	done; done; fi
	@if test -f sample-Ieee1609Dot2Data-1.per ; then \
	for f in sample-*-[1-9].per; do \
	pdu=`echo $$f | sed -E -e "s/sample-([A-Za-z-]+)-[0-9].*/\1/"`; \
	for b in 1 17 33 980 8192; do \
	echo "Recoding $$f ($$pdu) into DER into XER and back ($$b)..."; \
	./${TARGET} -p $$pdu -b $$b -iper -oder $$f > ./.tmp.1.$$$$ || exit 3; \
	./${TARGET} -p $$pdu -b $$b -iber -oxer ./.tmp.1.$$$$ > ./.tmp.2.$$$$ || exit 4; \
	./${TARGET} -p $$pdu -b $$b -ixer -oper ./.tmp.2.$$$$ > ./.tmp.1.$$$$ || exit 5; \
	diff $$f ./.tmp.1.$$$$ || exit 6; \
	rm -f ./.tmp.[12].$$$$; \
	echo "Test junking $$f (please wait)..."; \
	./${TARGET} -J0.0001 -n 1000 -p $$pdu -b $$b -iper -onull $$f || exit 7; \
	./${TARGET} -J0.001 -n 1000 -p $$pdu -b $$b -iper -onull $$f || exit 8; \
	done; done; fi

maybe-wip-pause:
	@if [ -f WIP ]; then cat WIP; sleep 2; fi

distclean: clean
	rm -f $(ASN_MODULE_SOURCES)
	rm -f $(ASN_MODULE_HEADERS)
	rm -f $(ASN1C_CONVERTER_CLI_SOURCE)
	rm -f Makefile.am.sample
