#ifndef PTPV2_TYPE_H
#define PTPV2_TYPE_H
//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All right reserved.
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
// ptpv2_type.h
//----------------------------------------------------------------------------
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Numeric primitive data types shall be formatted with the most significant
// octet nearest the beginning of the protocol data unit followed in order
// by octets of decreasing significance.
// see IEEE.Std 1588-2008 pp.15
#if defined(HOST_BIGENDIAN)
// for the case of big-endian processor
#	define htons(n) (n)
#	define ntohs(n) (n)
#	define htonl(n) (n)
#	define ntohl(n) (n)
#else
// for the case of little-endian processor
#if !defined(htons)
#define htons(n) (uint16_t)( (((uint16_t) (n)) << 8)\
                    |(((uint16_t) (n)) >> 8))
#endif
#if !defined(ntohs)
#define ntohs(n) htons(n)
#endif
#if !defined(htonl)
#define htonl(n) (uint32_t)( (((uint32_t)(n)&0xFF)<<24)\
                            |(((uint32_t)(n)&0xFF00)<<8)\
                            |(((uint32_t)(n)&0xFF0000)>>8)\
                            |(((uint32_t)(n)&0xFF000000)>>24))
#endif
#if !defined(ntohl)
#define ntohl(n) htonl(n)
#endif
#endif

//----------------------------------------------------------------------------
// Platform specific data types
// see IEEE.Std 1588-2008 pp.12

typedef uint32_t  Boolean      ; // TRUE or FALSE.
typedef uint8_t   Enumeration4 ; // 4-bit enumerated value.
typedef uint8_t   Enumeration8 ; // 8-bit enumerated value.
typedef uint16_t  Enumeration16; // 16-bit enumerated value.
typedef uint8_t   UInteger4    ; // 4-bit signed integer.
typedef int8_t    Integer8     ; // 8-bit signed integer.
typedef uint8_t   UInteger8    ; // 8-bit unsigned integer.
typedef int16_t   Integer16    ; // 16-bit signed integer.
typedef uint16_t  UInteger16   ; // 16-bit unsigned integer.
typedef int32_t   Integer32    ; // 32-bit signed integer.
typedef uint32_t  UInteger32   ; // 32-bit unsigned integer.
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct {  uint16_t  msb; // 16-bit unsigned for upper 16 bits.
                  uint32_t  lsb; // 32-bit unsigned for lower 32 bits.
               }  UInteger48; // 48-bit unsigned integer.
#pragma pack(pop)
#else
typedef struct {  uint16_t  msb; // 16-bit unsigned for upper 16 bits.
                  uint32_t  lsb; // 32-bit unsigned for lower 32 bits.
               }  __attribute__ ((packed)) UInteger48; // 48-bit unsigned integer.
#endif
//typedef int64_t   Integer64 ; // 64-bit signed integer.
typedef struct { int32_t high;
                 int32_t low; // LSB comes firs
               } Integer64;
//typedef uint64_t  UInteger64; // 64-bit unsigned integer.
typedef struct { uint32_t high;
                 uint32_t low;
               } UInteger64;
typedef uint8_t   Nibble    ; // 4-bit field not interpreted as a number.
typedef uint8_t   Octet     ; // 8-bit field not interpreted as a number.

//----------------------------------------------------------------------------
#define PTPV2_TRUE  (1) // TRUE.
#define PTPV2_FALSE (0) // FALSE.
#define PTPV2_NULL   0  // NULL.

//----------------------------------------------------------------------------
typedef Integer64 correctionField_t;
typedef struct { Integer64 scaledNanoseconds; } TimeInterval_t;
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct { UInteger48 secondsField;
                 UInteger32 nanosecondsField;
               } Timestamp_t;
#pragma pack(pop)
#else
typedef struct { UInteger48 secondsField;
                 UInteger32 nanosecondsField;
               } __attribute__ ((packed)) Timestamp_t;
#endif
typedef Octet  ClockIdentity_t[8];
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct PortIdentity { ClockIdentity_t clockIdentity;
                              UInteger16      portNumber; //0x0000 and 0xFFFF are reserved.
                            } PortIdentity_t;
#pragma pack(pop)
#else
typedef struct PortIdentity { ClockIdentity_t clockIdentity;
                              UInteger16      portNumber; //0x0000 and 0xFFFF are reserved.
                            } __attribute__ ((packed)) PortIdentity_t;
#endif
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct PortAddress { Enumeration16  networkProtocol;
                             UInteger16     addressLength;
                             Octet         *addressField; // [addressLength]
                           } PortAddress_t;
#pragma pack(pop)
#else
typedef struct PortAddress { Enumeration16  networkProtocol;
                             UInteger16     addressLength;
                             Octet         *addressField; // [addressLength]
                           } __attribute__ ((packed)) PortAddress_t;
#endif
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ClockQuality { UInteger8 clockClass;
                              Enumeration8 clockAccuracy;
                              UInteger16   offsetScaledLogVariance;
                            } ClockQuality_t;
#pragma pack(pop)
#else
typedef struct ClockQuality { UInteger8 clockClass;
                              Enumeration8 clockAccuracy;
                              UInteger16   offsetScaledLogVariance;
                            } __attribute__ ((packed)) ClockQuality_t;
#endif
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct TLV { Enumeration16 tlvType;
                     UInteger16  lengthField;
                     Octet      *valueField; // [lengthField]
                     } TLV_t;
#pragma pack(pop)
#else
typedef struct TLV { Enumeration16 tlvType;
                     UInteger16  lengthField;
                     Octet      *valueField; // [lengthField]
                     } __attribute__ ((packed)) TLV_t;
#endif
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct managementTLV { uint16_t  tlvType;
                               uint16_t  lengthField;
                               uint16_t  managementID;
                               uint8_t  *dataField;
                             } managementTLV_t;
#pragma pack(pop)
#else
typedef struct managementTLV { uint16_t  tlvType;
                               uint16_t  lengthField;
                               uint16_t  managementID;
                               uint8_t  *dataField;
                             } __attribute__ ((packed)) managementTLV_t;
#endif
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct PTPText { UInteger8  lengthField;
                         Octet     *textField; // [lengthField]
                       } PTPText_t;
#pragma pack(pop)
#else
typedef struct PTPText { UInteger8  lengthField;
                         Octet     *textField; // [lengthField]
                       } __attribute__ ((packed)) PTPText_t;
#endif
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct FaultRecord { UInteger16   faultRecordLength;
                             Timestamp_t  faultTime;
                             Enumeration8 severityCode;
                             PTPText_t    faultName;
                             PTPText_t    faultValue;
                             PTPText_t    faultDescription;
                           } FaultRecord_t;
#pragma pack(pop)
#else
typedef struct FaultRecord { UInteger16   faultRecordLength;
                             Timestamp_t  faultTime;
                             Enumeration8 severityCode;
                             PTPText_t    faultName;
                             PTPText_t    faultValue;
                             PTPText_t    faultDescription;
                           } __attribute__ ((packed)) FaultRecord_t;
#endif

//----------------------------------------------------------------------------
// PTP header message types.
// see IEEE.Std 1588-2008 pp.125
#define PTPV2_MSG_Sync                    0x0
#define PTPV2_MSG_Delay_Req               0x1
#define PTPV2_MSG_Pdelay_Req              0x2
#define PTPV2_MSG_Pdelay_Resp             0x3
#define PTPV2_MSG_Follow_Up               0x8
#define PTPV2_MSG_Delay_Resp              0x9
#define PTPV2_MSG_Pdelay_Resp_Follow_Up   0xA
#define PTPV2_MSG_Announce                0xB
#define PTPV2_MSG_Signaling               0xC
#define PTPV2_MSG_Management              0xD

//----------------------------------------------------------------------------
// PTP header message flags.
// see IEEE.Std 1588-2008 pp.126
#define PTPV2_MSG_FLAG_alternateMasterFlag    0x0001
#define PTPV2_MSG_FLAG_twoStepFlag            0x0002
#define PTPV2_MSG_FLAG_unicastFlag            0x0004
#define PTPV2_MSG_FLAG_profile1               0x0020
#define PTPV2_MSG_FLAG_profile2               0x0040
#define PTPV2_MSG_FLAG_leap61                 0x0100
#define PTPV2_MSG_FLAG_leap59                 0x0200
#define PTPV2_MSG_FLAG_currentUtcOffsetValid  0x0400
#define PTPV2_MSG_FLAG_ptpTimescale           0x0800
#define PTPV2_MSG_FLAG_timeTraceable          0x1000
#define PTPV2_MSG_FLAG_freqTraceable          0x1000

//----------------------------------------------------------------------------
// PTP header control field values (for backward combatibility).
// see IEEE.Std 1588-2008 pp.128
#define PTPV2_MSG_CTRL_Sync        0x00
#define PTPV2_MSG_CTRL_Delay_Req   0x01
#define PTPV2_MSG_CTRL_Follow_Up   0x02
#define PTPV2_MSG_CTRL_Delay_Resp  0x03
#define PTPV2_MSG_CTRL_Management  0x04
#define PTPV2_MSG_CTRL_All_others  0x05

//----------------------------------------------------------------------------
// Default value for logMessageInterval 
// (for Delay_Req, Signaling, Management, Pdelay_Req, 
// Pdelay_Resp, Pdelay_Resp_Follow_Up)
// see IEEE.Std 1588-2008 pp.128
#define PTPV2_MSG_DEFAULT_INTERVAL 0x7F

//----------------------------------------------------------------------------
// PTP V2 HEADER STRUCTURE
#define PTPV2_HDR_LEN  34
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_hdr {
    uint8_t               messageType:4; // lower 4-bit
    uint8_t               transportSpecific:4; // higher 4-bit
    uint8_t               versionPTP:4; // lower 4-bit
    uint8_t               reserved0:4; // higher 4-bit
    uint16_t              messageLength;
    uint8_t               domainNumber;
    uint8_t               reserved1;
    uint16_t              flagField;
    correctionField_t     correctionField;
    uint32_t              reserved2;
    PortIdentity_t        sourcePortIdentity;
    uint16_t              sequenceID;
    uint8_t               controlField;
    uint8_t               logMessageInterval;
} ptpv2_msg_hdr_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_hdr {
    uint8_t               messageType:4; // lower 4-bit
    uint8_t               transportSpecific:4; // higher 4-bit
    uint8_t               versionPTP:4; // lower 4-bit
    uint8_t               reserved0:4; // higher 4-bit
    uint16_t              messageLength;
    uint8_t               domainNumber;
    uint8_t               reserved1;
    uint16_t              flagField;
    correctionField_t     correctionField;
    uint32_t              reserved2;
    PortIdentity_t        sourcePortIdentity;
    uint16_t              sequenceID;
    uint8_t               controlField;
    uint8_t               logMessageInterval;
} __attribute__ ((packed)) ptpv2_msg_hdr_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_announce {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp ;
    uint16_t        currentUTCOffset;
    uint8_t         res;
    uint8_t         grandmasterPriority1;
    ClockQuality_t  grandmasterClockQuality;
    uint8_t         grandmasterPriority2;    
    ClockIdentity_t grandmasterIdentity;
    uint16_t        stepsRemoved;
    uint8_t         timeSource;
} ptpv2_msg_announce_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_announce {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp ;
    uint16_t        currentUTCOffset;
    uint8_t         res;
    uint8_t         grandmasterPriority1;
    ClockQuality_t  grandmasterClockQuality;
    uint8_t         grandmasterPriority2;    
    ClockIdentity_t grandmasterIdentity;
    uint16_t        stepsRemoved;
    uint8_t         timeSource;
} __attribute__ ((packed)) ptpv2_msg_announce_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_sync {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp;
} ptpv2_msg_sync_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_sync {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp;
} __attribute__ ((packed)) ptpv2_msg_sync_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_follow_up {
    ptpv2_msg_hdr_t header;
    Timestamp_t     preciseOriginTimestamp;
} ptpv2_msg_follow_up_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_follow_up {
    ptpv2_msg_hdr_t header;
    Timestamp_t     preciseOriginTimestamp;
} __attribute__ ((packed)) ptpv2_msg_follow_up_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_delay_req {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp;
} ptpv2_msg_delay_req_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_delay_req {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp;
} __attribute__ ((packed)) ptpv2_msg_delay_req_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_delay_resp {
    ptpv2_msg_hdr_t header;
    Timestamp_t     receiveTimestamp;
    PortIdentity_t  requestingPortIdentity;
} ptpv2_msg_delay_resp_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_delay_resp {
    ptpv2_msg_hdr_t header;
    Timestamp_t     receiveTimestamp;
    PortIdentity_t  requestingPortIdentity;
} __attribute__ ((packed)) ptpv2_msg_delay_resp_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_pdelay_req {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp;
    uint8_t         res[10];
} ptpv2_msg_pdelay_req_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_pdelay_req {
    ptpv2_msg_hdr_t header;
    Timestamp_t     originTimestamp;
    uint8_t         res[10];
} __attribute__ ((packed)) ptpv2_msg_pdelay_req_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_pdelay_resp {
    ptpv2_msg_hdr_t header;
    Timestamp_t     requestReceiptTimestamp;
    PortIdentity_t  requestingPortIdentity;
} ptpv2_msg_pdelay_resp_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_pdelay_resp {
    ptpv2_msg_hdr_t header;
    Timestamp_t     requestReceiptTimestamp;
    PortIdentity_t  requestingPortIdentity;
} __attribute__ ((packed)) ptpv2_msg_pdelay_resp_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_pdelay_resp_follow_up {
    ptpv2_msg_hdr_t header;
    Timestamp_t     responseOriginTimestamp;
    PortIdentity_t  requestingPortIdentity;
} ptpv2_msg_pdelay_resp_follow_up_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_pdelay_resp_follow_up {
    ptpv2_msg_hdr_t header;
    Timestamp_t     responseOriginTimestamp;
    PortIdentity_t  requestingPortIdentity;
} __attribute__ ((packed)) ptpv2_msg_pdelay_resp_follow_up_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_signaling {
    ptpv2_msg_hdr_t header;
    PortIdentity_t  targetPortIdentity;
    TLV_t           TLVs;
} ptpv2_msg_signaling_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_signaling {
    ptpv2_msg_hdr_t header;
    PortIdentity_t  targetPortIdentity;
    TLV_t           TLVs;
} __attribute__ ((packed)) ptpv2_msg_signaling_t;
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ptpv2_msg_management {
    ptpv2_msg_hdr_t header;
    PortIdentity_t  targetPortIdentity;
    uint8_t         startingBoundaryHops;
    uint8_t         BoundaryHops;
    uint8_t         rese:4;
    uint8_t         actionField:4;
    uint8_t         reserve;
    managementTLV_t managementTLV;
} ptpv2_msg_management_t;
#pragma pack(pop)
#else
typedef struct ptpv2_msg_management {
    ptpv2_msg_hdr_t header;
    PortIdentity_t  targetPortIdentity;
    uint8_t         startingBoundaryHops;
    uint8_t         BoundaryHops;
    uint8_t         rese:4;
    uint8_t         actionField:4;
    uint8_t         reserve;
    managementTLV_t managementTLV;
} __attribute__ ((packed)) ptpv2_msg_management_t;
#endif

//----------------------------------------------------------------------------
#define PTPV2_ETHERNET_TYPE_LENGTH   0x88F7
#define PTPV2_ETHERNET_MAC_DST_DELAY 0x01_1B_19_00_00_00
#define PTPV2_ETHERNET_MAC_DST_PEER  0x01_80_C2_00_00_0E
//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Started by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
#endif // PTPV2_TYPE_H
