#ifndef ETH_IP_UDP_TCP_DATA_TYPES_H
#define ETH_IP_UDP_TCP_DATA_TYPES_H
//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All right reserved.
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** ETHERNET HEADER STRUCTURE **/
#define ETH_ADDR_LEN 6
#define ETH_HDR_LEN  14
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct eth_hdr
{
    uint8_t  eth_dhost[ETH_ADDR_LEN];    /* destination ethernet address, i.e. mac_dst */
    uint8_t  eth_shost[ETH_ADDR_LEN];    /* source ethernet address, i.e. mac_src */
    uint16_t eth_type;                   /* packet type ID */
} eth_hdr_t;
#pragma pack(pop)
#else
typedef struct eth_hdr
{
    uint8_t  eth_dhost[ETH_ADDR_LEN];    /* destination ethernet address, i.e. mac_dst */
    uint8_t  eth_shost[ETH_ADDR_LEN];    /* source ethernet address, i.e. mac_src */
    uint16_t eth_type;                   /* packet type ID */
} __attribute__ ((packed)) eth_hdr_t;
#endif

/** DEFINES FOR ETHERNET **/
#define ETH_TYPE_ARP  0x0806  /* Addr. resolution protocol */
#define ETH_TYPE_IP   0x0800  /* IP protocol */

/** ARP HEADER STRUCTURE **/
#define ARP_HDR_LEN 28
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct arp_hdr
{
    uint16_t  arp_hrd;             /* format of hardware address   */
    uint16_t  arp_pro;             /* format of protocol address   */
    uint8_t   arp_hln;             /* length of hardware address   */
    uint8_t   arp_pln;             /* length of protocol address   */
    uint16_t  arp_op;              /* ARP opcode (command)         */
    uint8_t   arp_sha[ETH_ADDR_LEN];/* sender hardware address     */
    uint32_t  arp_sip;              /* sender IP address           */
    uint8_t   arp_tha[ETH_ADDR_LEN];/* target hardware address     */
    uint32_t  arp_tip;              /* target IP address           */
} arp_hdr_t;
#pragma pack(pop)
#else
typedef struct arp_hdr
{
    uint16_t  arp_hrd;             /* format of hardware address   */
    uint16_t  arp_pro;             /* format of protocol address   */
    uint8_t   arp_hln;             /* length of hardware address   */
    uint8_t   arp_pln;             /* length of protocol address   */
    uint16_t  arp_op;              /* ARP opcode (command)         */
    uint8_t   arp_sha[ETH_ADDR_LEN];/* sender hardware address     */
    uint32_t  arp_sip;              /* sender IP address           */
    uint8_t   arp_tha[ETH_ADDR_LEN];/* target hardware address     */
    uint32_t  arp_tip;              /* target IP address           */
} __attribute__ ((packed)) arp_hdr_t;
#endif

/** DEFINES FOR ARP **/
#define ARP_HRD_ETHERNET    0x0001
#define ARP_PRO_IP          0x0800
#define ARP_OP_REQUEST      1
#define ARP_OP_REPLY        2
#define RARP_OP_REQUEST     3
#define RARP_OP_REPLY       4

/** IP HEADER STRUCTURE **/
#define IP_ADDR_LEN 4
#define IP_HDR_LEN 20
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct ip_hdr
{
    uint8_t  ip_hdl:4; /* header length (low 4-bit of first byte) */
    uint8_t  ip_ver:4; /* version (high 4-bit of first byte) */
    uint8_t  ip_tos;   /* type of service */
    uint16_t ip_len;   /* total length */
    uint16_t ip_id;    /* identification */
    uint16_t ip_off;   /* fragment offset field */
    uint8_t  ip_ttl;   /* time to live */
    uint8_t  ip_pro;   /* protocol */
    uint16_t ip_sum;   /* checksum */
    uint32_t ip_src;   /* source address */
    uint32_t ip_dst;   /* dest address */
} ip_hdr_t;
#pragma pack(pop)
#else
typedef struct ip_hdr
{
    uint8_t  ip_hdl:4; /* header length (low 4-bit of first byte) */
    uint8_t  ip_ver:4; /* version (high 4-bit of first byte) */
    uint8_t  ip_tos;   /* type of service */
    uint16_t ip_len;   /* total length */
    uint16_t ip_id;    /* identification */
    uint16_t ip_off;   /* fragment offset field */
    uint8_t  ip_ttl;   /* time to live */
    uint8_t  ip_pro;   /* protocol */
    uint16_t ip_sum;   /* checksum */
    uint32_t ip_src;   /* source address */
    uint32_t ip_dst;   /* dest address */
} __attribute__ ((packed)) ip_hdr_t;
#endif

#define PSEUDO_IP_HDR_LEN 12
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct pseudo_ip_hdr
{
    uint32_t ip_src;  /* source address */
    uint32_t ip_dst;  /* dest address */
    uint8_t  ip_zro;  /* header length (low 4-bit of first byte) */
    uint8_t  ip_pro;  /* version (high 4-bit of first byte) */
    uint16_t ip_len;  /* total length */
} pseudo_ip_hdr_t;
#pragma pack(pop)
#else
typedef struct pseudo_ip_hdr
{
    uint32_t ip_src;  /* source address */
    uint32_t ip_dst;  /* dest address */
    uint8_t  ip_zro;  /* header length (low 4-bit of first byte) */
    uint8_t  ip_pro;  /* version (high 4-bit of first byte) */
    uint16_t ip_len;  /* total length */
} __attribute__ ((packed)) pseudo_ip_hdr_t;
#endif

/** UDP HEADER STRUCTURE **/
#define UDP_HDR_LEN 8
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct udp_hdr
{
    uint16_t udp_src; // source port
    uint16_t udp_dst; // destination port
    uint16_t udp_len; // total length in bytes
    uint16_t udp_sum; // checksum
} udp_hdr_t;
#pragma pack(pop)
#else
typedef struct udp_hdr
{
    uint16_t udp_src; // source port
    uint16_t udp_dst; // destination port
    uint16_t udp_len; // total length in bytes
    uint16_t udp_sum; // checksum
} __attribute__ ((packed)) udp_hdr_t;
#endif

/** TCP HEADER STRUCTURE **/
#define TCP_HDR_LEN 20
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct tcp_hdr
{
    uint16_t port_src; // source port
    uint16_t port_dst; // destination port
    uint32_t tcp_seq; // sequence number
    uint32_t tcp_ack; // acknowledgement number
    uint16_t tcp_hl:4; // header length (only higher 4-bit)
    uint16_t tcp_rsv:6; // reserved
    uint16_t tcp_ctl:6; // control (only lower 6-bit)
    uint16_t tcp_win; // window
    uint16_t tcp_sum; // checksum
    uint16_t tcp_pnt; // urgent pointer
} tcp_hdr_t;
#pragma pack(pop)
#else
typedef struct tcp_hdr
{
    uint16_t port_src; // source port
    uint16_t port_dst; // destination port
    uint32_t tcp_seq; // sequence number
    uint32_t tcp_ack; // acknowledgement number
    uint16_t tcp_hl:4; // header length (only higher 4-bit)
    uint16_t tcp_rsv:6; // reserved
    uint16_t tcp_ctl:6; // control (only lower 6-bit)
    uint16_t tcp_win; // window
    uint16_t tcp_sum; // checksum
    uint16_t tcp_pnt; // urgent pointer
} __attribute__ ((packed)) tcp_hdr_t;
#endif

/** DEFINES FOR IP **/
#define IP_PROTO_ICMP      0x01  // ICMP protocol
#define IP_PROTO_TCP       0x06  // TCP protocol
#define IP_PROTO_UDP       0x11  // UDP protocol
#define IP_PROTO_PWOSPF    0x59  // PWOSPF protocol
#define IP_FRAG_RF         0x8000  // reserved fragment flag
#define IP_FRAG_DF         0x4000  // dont fragment flag
#define IP_FRAG_MF         0x2000  // more fragments flag
#define IP_FRAG_OFFMASK    0x1fff  // mask for fragmenting bits

/** ICMP HEADER STRUCTURE **/
#if defined(_MSC_VER)
#pragma pack(push, 1)
typedef struct icmp_hdr
{
    uint8_t icmp_type;
    uint8_t icmp_code;
    uint16_t icmp_sum;
} icmp_hdr_t;
#pragma pack(pop)
#else
typedef struct icmp_hdr
{
    uint8_t icmp_type;
    uint8_t icmp_code;
    uint16_t icmp_sum;
} __attribute__ ((packed)) icmp_hdr_t;
#endif

/** DEFINES FOR ICMP **/
#define ICMP_TYPE_DESTINATION_UNREACHABLE  0x3
#define ICMP_CODE_NET_UNREACHABLE          0x0
#define ICMP_CODE_HOST_UNREACHABLE         0x1
#define ICMP_CODE_PROTOCOL_UNREACHABLE     0x2
#define ICMP_CODE_PORT_UNREACHABLE         0x3
#define ICMP_CODE_NET_UNKNOWN              0x6

#define ICMP_TYPE_TIME_EXCEEDED            0xB
#define ICMP_CODE_TTL_EXCEEDED             0x0

#define ICMP_TYPE_ECHO_REQUEST             0x8
#define ICMP_TYPE_ECHO_REPLY               0x0
#define ICMP_CODE_ECHO                     0x0

#ifdef __cplusplus
}
#endif

//-----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
#endif
