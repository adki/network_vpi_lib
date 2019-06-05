//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All right reserved.
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "eth_ip_udp_tcp_pkt.h"

//-----------------------------------------------------
#define TEXTIFY(T) #T
#define CHECK_ALIGN(A,B)\
    if (((uintptr_t)(const void*)&((A)))%(B))\
        printf("mis-aligned %s %zd\n", TEXTIFY(A), ((uintptr_t)(const void*)&((A)))%(B))

//-----------------------------------------------------
static uint32_t crc_table[] = {
  0x4DBDF21C, 0x500AE278, 0x76D3D2D4, 0x6B64C2B0,
  0x3B61B38C, 0x26D6A3E8, 0x000F9344, 0x1DB88320,
  0xA005713C, 0xBDB26158, 0x9B6B51F4, 0x86DC4190,
  0xD6D930AC, 0xCB6E20C8, 0xEDB71064, 0xF0000000
};
//-----------------------------------------------------
// 1. calculate Etherent CRC checksum
// 2. return the host order checksum (after inversion)
uint32_t compute_eth_crc_ref(uint8_t *pkt, uint32_t bnum) {
  unsigned int n;
  uint32_t crc=0;
  for (n=0; n<bnum; n++) {
    crc = (crc>>4)^crc_table[(crc^(pkt[n]>>0))&0x0F]; /* lower nibble */
    crc = (crc>>4)^crc_table[(crc^(pkt[n]>>4))&0x0F]; /* upper nibble */
  }
  return ~crc;
}
// It is assumed that bit0 of pkt[0] comes first in
// terms of bit-stream.
// message: pointer to the message that contains 'len' bytes.
// return: 32-bit CRC; LSbyte should sent first with inversion.
uint32_t compute_eth_crc(uint8_t *message, int len) {
   int i, j;
   uint32_t word, crc, mask;
   crc = 0xFFFFFFFF;
   for (i=0; i<len; i++) { //while (message[i] != 0) {
      word = message[i]; // Get next byte.
      crc = crc ^ word;
      for (j = 7; j >= 0; j--) { // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask); // reverse(0x04C11DB7)
      }
   }
   return ~crc; // LSByte should be sent first
}
// len includes 4-byte FCS
// return 0 on success, 1 on failure
int check_eth_crc(uint8_t *message, int len) {
   int i, j;
   uint32_t word, crc, mask;
   crc = 0xFFFFFFFF;
   for (i=0; i<len; i++) {
      if (i<(len-4)) word = message[i]; // Get next byte.
      else         { uint8_t b = ~message[i]; word = b; }
      crc = crc ^ word;
      for (j = 7; j >= 0; j--) { // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask); // reverse(0x04C11DB7)
      }
   }
   return crc;
}

//-----------------------------------------------------
// It is assumed that the checksum field is zero before calling this.
// 1. calculate IP header checksum
// 2. return the host order checksum without inversion
uint16_t compute_checksum(uint8_t* pkt, int bnum) {
    int idx;
    uint32_t sum = 0;
    for (idx=0; idx<(bnum-1); idx+=2) {
         sum += (pkt[idx]<<8)|pkt[idx+1];
    }
    if (idx<bnum) {
         sum += (pkt[idx]<<8);
    }
    sum  = (sum>>16)+(sum&0xFFFF);
    sum += (sum>>16);

    return sum&0xFFFF;
}
//----------------------------------------------------------------------------
// Data in 'pkt[]' are big-endian fashion.
// Return the host order checksum without inversion
//----------------------------------------------------------------------------
uint16_t compute_checksum_d8(uint8_t *pkt, int bnum) {
    int idx;
    uint32_t sum = 0;

    for (idx=0; idx<(bnum-1); idx+=2) {
         sum += (pkt[idx]<<8)|pkt[idx+1];
    }
    if (idx<bnum) {
         sum += (pkt[idx]<<8);
    }
    sum  = (sum>>16)+(sum&0xFFFF);
    sum += (sum>>16);

    return sum&0xFFFF;
}

//----------------------------------------------------------------------------
// Data in 'pkt[]' are big-endian fashion.
// return the host order checksum without inversion
//----------------------------------------------------------------------------
uint16_t compute_checksum_d16(uint8_t *pkt, int bnum)
{
    uint32_t sum = 0;

    /* Main loop - 2 bytes at a time */
    while (bnum>=sizeof(uint16_t)) {
        sum  += (pkt[0]<<8)|pkt[1];
        bnum -= 2;
        pkt  += 2;
    }

    /* Handle tail less than 2-bytes long */
    if (bnum&0x1) {
        sum += (pkt[0]<<8);
    }

    sum  = (sum>>16)+(sum&0xFFFF);
    sum += (sum>>16);

    return sum&0xFFFF;
}
//----------------------------------------------------------------------------
// return the host order checksum without inversion
//----------------------------------------------------------------------------
uint16_t compute_checksum_d32(uint8_t *pkt, int bnum)
{
    uint32_t sum = 0;

    /* Main loop - 4 bytes at a time */
    while (bnum>=sizeof(uint32_t)) {
        uint32_t s = (pkt[0]<<24)|(pkt[1]<<16)|(pkt[2]<<8)|pkt[3];
        sum += s;
        if (sum<s) sum++;
        bnum -= 4;
        pkt  += 4;
    }

    /* Handle tail less than 4-bytes long */
    if (bnum&0x2) {
        uint16_t s = (pkt[0]<<8)|pkt[1];
        sum += s;
        if (sum<s) sum++;
        bnum -= 2;
        pkt  += 2;
    }

    if (bnum&0x1) {
        uint16_t s = (pkt[0]<<8);
        sum += s;
        if (sum<s) sum++;
    }

    sum  = (sum>>16)+(sum&0xFFFF);
    sum += (sum>>16);

    return sum&0xFFFF;
}
//----------------------------------------------------------------------------
//RFC-1071 Computing the Internet checksum. R.T. Braden, D.A. Borman, C.
//     Partridge. Sep-01-1988. (Format: TXT=54941 bytes) (Updated by
//     RFC1141) (Status: UNKNOWN)
//
//RFC-1141 Incremental updating of the Internet checksum. T. Mallory, A.
//     Kullberg. Jan-01-1990. (Format: TXT=3587 bytes) (Updates RFC1071)
//     (Updated by RFC1624) (Status: INFORMATIONAL)
//
//RFC-1624 Computation of the Internet Checksum via Incremental Update. A.
//     Rijsinghani, Ed.. May 1994. (Format: TXT=9836 bytes) (Updates
//     RFC1141) (Status: INFORMATIONAL)
//----------------------------------------------------------------------------
// Incrementaly update a checksum, given old and new 16bit words
// old_check: host order not-inverted
// old: old value
// new: new value
// return the host order checksum without inversion
uint16_t checksum_incremental_d16(uint16_t old_check, uint16_t old, uint16_t new)
{
    uint32_t w, s;
    old = ~old; // note that this means the inverted checksum-old can be used directly.
    w = (uint32_t)old_check + old + new;
    s = ((uint16_t)(w>>16)+(w&0xffff));
    return ((uint16_t)(s>>16)+(s&0xffff));
}
//----------------------------------------------------------------------------
// Incrementaly update a checksum, given old and new 32bit words
// old_check: host order not-inverted
// old: old value
// new: new value
// return the host order checksum without inversion
uint16_t checksum_incremental_d32(uint16_t old_check, uint32_t old, uint32_t new)
{
    uint32_t w, s;
    old = ~old; // note that this means the inverted checksum-old can be used directly.
    w = (uint32_t)old_check
      + (old>>16) + (old&0xffff)
      + (new>>16) + (new&0xffff);
    s = ((uint16_t)(w>>16)+(w&0xffff));
    return ((uint16_t)(s>>16)+(s&0xffff));
}
//----------------------------------------------------------------------------
// Incrementaly update a checksum, given old and new 64bit words
// old_check: host order not-inverted
// old: old value
// new: new value
// return the host order checksum without inversion
uint16_t checksum_incremental_d64(uint16_t old_check, uint64_t old, uint64_t new)
{
    uint32_t w, s;
    old = ~old; // note that this means the inverted checksum-old can be used directly.
    w = (uint32_t)old_check
      + ((old>>48)&0xFFFF) + ((old>>32)&0xFFFF)
      + ((old>>16)&0xFFFF) +  (old&0xFFFF)
      + ((new>>48)&0xFFFF) + ((new>>32)&0xFFFF)
      + ((new>>16)&0xFFFF) + (new&0xFFFF);
    s = ((uint16_t)(w>>16)+(w&0xffff));
    return ((uint16_t)(s>>16)+(s&0xFFFF));
}
//----------------------------------------------------------------------------
// return 0 on success, 1 on failure
int check_checksum(uint8_t* pkt, int bnum) {
    int idx;
    uint32_t sum = 0;
    for (idx=0; idx<(bnum-1); idx+=2) {
         sum += (pkt[idx]<<8)|pkt[idx+1];
    }
    if (idx<bnum) {
         sum += (pkt[idx]<<8);
    }
    sum  = (sum>>16)+(sum&0xFFFF);
    sum += (sum>>16);

    return (sum==0xFFFF) ? 0 : 1;
}
//-----------------------------------------------------
// 1. calculate IP header checksum
// 2. return the host order checksum after inversion
uint16_t compute_ip_checksum(ip_hdr_t* iphdr) {
    uint32_t sum;
    int numShorts;
    int i;
    uint16_t* s_ptr;

    sum = 0;
    numShorts = iphdr->ip_hdl * 2; // ip_hdl in words
    s_ptr = (uint16_t*)iphdr;
    for (i=0; i<numShorts; ++i) {
         if (i!=5) sum += (uint32_t)ntohs(s_ptr[i]);
    }

    sum  = (sum >> 16) + (sum & 0xFFFF); /* sum carries */
    sum += (sum >> 16);

    return (~sum)&0xFFFF;
}
// return 0 on sucessful, 1 on failure
int check_ip_checksum(ip_hdr_t* iphdr) {
    uint32_t sum;
    int numShorts;
    int i;
    uint16_t* s_ptr;

    sum = 0;
    numShorts = iphdr->ip_hdl * 2; // ip_hdl in words
    s_ptr = (uint16_t*)iphdr;
    for (i=0; i<numShorts; ++i) {
         sum += (uint32_t)ntohs(s_ptr[i]);
    }

    sum  = (sum >> 16) + (sum & 0xFFFF); /* sum carries */
    sum += (sum >> 16);

    return (sum==0xFFFF) ? 0 : 1;
}

//-----------------------------------------------------
// 1. calculate UDP header checksum
//    [Pseudo Header] SrcIP(32-bit),DstIP(32-bit),
//                    Rsv(8-bit; all zero),Proto(8-bit, should be 6),
//                    UdpLen(16-bit; UDP hder + UDP payload)
//    [UDP Segment  ] Udp Header + UDP Data
// 2. return the host order checksum with inversion
uint16_t compute_udp_checksum(pseudo_ip_hdr_t* ip_hdr, udp_hdr_t* udp_hdr) {
    uint32_t  sum;
    uint16_t  udp_len;
    uint16_t *spt;
    int idx;
    //-----------------------------------------------------
    // calculate checksum of pseudo-header
    sum = 0;
    spt = (uint16_t*)ip_hdr;
    for (idx=0; idx<6; idx++) sum += (uint32_t)ntohs(spt[idx]);
    //-----------------------------------------------------
    // calculate checksum of udp header and datagram
    spt = (uint16_t*)udp_hdr;
    udp_len = ntohs(udp_hdr->udp_len); //udp_len = ntohs(ip_hdr->ip_len);
    for (idx=0; idx<(udp_len/2); idx++) {
         if (idx!=3) sum += (uint32_t)ntohs(spt[idx]);
    }
    if (udp_len&0x1) sum += (uint32_t)(ntohs(spt[idx])&0xFF00); // when not multiple of 16-bit
    //-----------------------------------------------------
    // add carries
    sum  = (sum>>16) + (sum&0xFFFF); //sum += (sum>>16) + (sum&0xFFFF);
    sum += (sum>>16);
    //-----------------------------------------------------
    // invert and return
    return (~sum)&0xFFFF;
}
// return 0 on success, 1 on failture
int check_udp_checksum(pseudo_ip_hdr_t* ip_hdr, udp_hdr_t* udp_hdr) {
    uint32_t  sum;
    uint16_t  udp_len;
    uint16_t *spt;
    int idx;
    //-----------------------------------------------------
    // calculate checksum of pseudo-header
    sum = 0;
    spt = (uint16_t*)ip_hdr;
    for (idx=0; idx<6; idx++) sum += (uint32_t)ntohs(spt[idx]);
    //-----------------------------------------------------
    // calculate checksum of udp header and datagram
    spt = (uint16_t*)udp_hdr;
    udp_len = ntohs(ip_hdr->ip_len);
    for (idx=0; idx<(udp_len/2); idx++) {
         sum += (uint32_t)ntohs(spt[idx]);
    }
    if (udp_len&0x1) sum += (uint32_t)(ntohs(spt[idx])&0xFF00); // when not multiple of 16-bit
    //-----------------------------------------------------
    // add carries
    sum  = (sum>>16) + (sum&0xFFFF); //sum += (sum>>16) + (sum&0xFFFF);
    sum += (sum>>16);
    //-----------------------------------------------------
    // invert and return
    return (sum==0xFFFF) ? 0 : 1;
}

//-----------------------------------------------------
// Note that this should be called with IP packet, not TCP packet alone,
// since it requires Pseudo Header, which includes IP information.
//
// 1. calculate TCP header checksum
//    [Pseudo Header] SrcIP(32-bit),DstIP(32-bit),
//                    Rsv(8-bit; all zero),Proto(8-bit, should be 6),
//                    TcpLen(16-bit; TCP hder + TCP payload)
//    [TCP Segment  ] TCP Header + TCP Data
// 2. return the host order checksum with inversion
// TCP Checksum is a 16-bit field in TCP header used for error detection.
// It is computed over the TCP segment (might plus some padding) and
// a 12-byte TCP pseudo header created on the fly.
// Same as IP checksum, TCP checksum is also one¡¯s complement of
// the one¡¯s complement sum of all 16 bit words in the computation data.
//
// Note that 'pseudo_ip_hdr_t' is not the same as 'ip_hdr_t'.
uint16_t compute_tcp_checksum(pseudo_ip_hdr_t* ip_hdr, tcp_hdr_t* tcp_hdr) {
    uint32_t  sum;
    uint16_t  tcp_len;
    uint16_t *spt;
    int idx;
    //-----------------------------------------------------
    // calculate checksum of pseudo-header
    sum = 0;
    spt = (uint16_t*)ip_hdr;
    for (idx=0; idx<6; idx++) sum += (uint32_t)ntohs(spt[idx]);
    //-----------------------------------------------------
    // calculate checksum of tcp header and datagram
    spt = (uint16_t*)tcp_hdr;
    tcp_len = ntohs(ip_hdr->ip_len);
    for (idx=0; idx<(tcp_len/2); idx++) {
         if (idx!=8) sum += (uint32_t)ntohs(spt[idx]);
    }
    if (tcp_len&0x1) sum += (uint32_t)(ntohs(spt[idx])&0xFF00); // when not multiple of 16-bit
    //-----------------------------------------------------
    // add carries
    sum  = (sum>>16) + (sum&0xFFFF); //sum += (sum>>16) + (sum&0xFFFF);
    sum += (sum>>16);
    //-----------------------------------------------------
    // invert and return
    return (~sum)&0xFFFF;
}
// return 0 on success, 1 on failure
int check_tcp_checksum(pseudo_ip_hdr_t* ip_hdr, tcp_hdr_t* tcp_hdr) {
    uint32_t  sum;
    uint16_t  tcp_len;
    uint16_t *spt;
    int idx;
    //-----------------------------------------------------
    // calculate checksum of pseudo-header
    sum = 0;
    spt = (uint16_t*)ip_hdr;
    for (idx=0; idx<6; idx++) sum += (uint32_t)ntohs(spt[idx]);
    //-----------------------------------------------------
    // calculate checksum of tcp header and datagram
    spt = (uint16_t*)tcp_hdr;
    tcp_len = ntohs(ip_hdr->ip_len);
    for (idx=0; idx<(tcp_len/2); idx++) {
         sum += (uint32_t)ntohs(spt[idx]);
    }
    if (tcp_len&0x1) sum += (uint32_t)(ntohs(spt[idx])&0xFF00); // when not multiple of 16-bit
    //-----------------------------------------------------
    // add carries
    sum  = (sum>>16) + (sum&0xFFFF); //sum += (sum>>16) + (sum&0xFFFF);
    sum += (sum>>16);
    //-----------------------------------------------------
    // invert and return
    return (sum==0xFFFF) ? 0 : 1;
}

//-----------------------------------------------------
// Populates an Ethernet header with the usual data.
// Return the number of bytes
int populate_eth_hdr( eth_hdr_t *eth_hdr
                    , uint8_t    mac_src[6]// src MAC (network order)
                    , uint8_t    mac_dst[6]// dest MAC (network order)
                    , uint16_t   type_len  // leng or type (host order)
                    )
{
    if (mac_dst) {
        memcpy((void*)(eth_hdr->eth_dhost), (void*)mac_dst, ETH_ADDR_LEN);
    }
    if (mac_src) {
        memcpy((void*)(eth_hdr->eth_shost), (void*)mac_src, ETH_ADDR_LEN);
    }
    #if defined(RIGOR)
    #if 0
    if (((unsigned)&(eth_hdr->eth_type))%2) {
        printf("mis-aligned eth_hdr->eth_type %d\n", ((unsigned)&(eth_hdr->eth_type))%2);
    }
    #else
    CHECK_ALIGN(eth_hdr->eth_type, 2);
    #endif
    #endif
    eth_hdr->eth_type = htons(type_len); // what if mis-aligned

    return ETH_HDR_LEN;
}

//-----------------------------------------------------
// Populates an ARP header
int populate_arp_hdr( arp_hdr_t *hdr
                    , uint16_t   type       // message type 0: ARP req, 2: ARP reply
                    , uint8_t    mac_src[6] // network order
                    , uint8_t    mac_dst[6] // network order
                    , uint32_t   ip_src     // host order
                    , uint32_t   ip_dst)
{
    hdr->arp_hrd = htons(ARP_HRD_ETHERNET); // 0x0001
    hdr->arp_pro = htons(ARP_PRO_IP); // 0x0800
    hdr->arp_hln = 0x06; // Ethernet
    hdr->arp_pln = 0x04; // IPv4
    hdr->arp_op  = htons(type); // 0:ARP_REQ,1:ARP_RPY,2:RARP_REQ,4:RARP_RPL
    if (mac_src) { memcpy(hdr->arp_sha, mac_src, ETH_ADDR_LEN); }
    if (mac_dst) { memcpy(hdr->arp_tha, mac_dst, ETH_ADDR_LEN); }
    hdr->arp_sip = htonl(ip_src);
    hdr->arp_tip = htonl(ip_dst);
    return ARP_HDR_LEN; // 28
}

//-----------------------------------------------------
// Populates an IP header with the usual data.
int populate_ip_hdr( ip_hdr_t *ip_hdr
                   , uint32_t  ip_src // host order
                   , uint32_t  ip_dst // host order
                   , uint8_t   protocol
                   , uint8_t   ttl     
                   , uint16_t  payload_size // pure payload size (not including header)
                   )
{
    ip_hdr->ip_hdl = 5;
    ip_hdr->ip_ver = 4;
    ip_hdr->ip_tos = 0;     /* type of service */
    ip_hdr->ip_len = 0;     /* total length */
    ip_hdr->ip_id  = 0;      /* identification */
#if defined(RIGOR)
    #if 0
    if (((unsigned)&ip_hdr->ip_off)%2) {
        printf("mis-aligned ip_hdr->ip_off %d\n", ((unsigned)&ip_hdr->ip_off)%2);
    }
    #else
    CHECK_ALIGN(ip_hdr->ip_off, 2);
    #endif
#endif
    ip_hdr->ip_off = htons(IP_FRAG_DF); // what if mis-aligned
    ip_hdr->ip_len = htons(IP_HDR_LEN + payload_size); // what if mis-aligned
    ip_hdr->ip_ttl = ttl;
    ip_hdr->ip_pro = protocol;
#if defined(UNSUPPORT_MISALIGN)
    uint8_t *pt = (uint8_t*)&ip_hdr->ip_src;
    uint32_t val = htonl(ip_src);
    pt[0] = (val&0x000000FF)    ;
    pt[1] = (val&0x0000FF00)>> 8;
    pt[2] = (val&0x00FF0000)>>16;
    pt[3] = (val&0xFF000000)>>24;
    pt = (uint8_t*)&ip_hdr->ip_dst;
    val = htonl(ip_dst);
    pt[0] = (val&0x000000FF)    ;
    pt[1] = (val&0x0000FF00)>> 8;
    pt[2] = (val&0x00FF0000)>>16;
    pt[3] = (val&0xFF000000)>>24;
#else
    #if defined(RIGOR)
    #if 0
    if (((unsigned)&ip_hdr->ip_src)%4) {
        printf("mis-aligned ip_hdr->ip_src %d\n", ((unsigned)&ip_hdr->ip_src)%4);
    }
    if (((unsigned)&ip_hdr->ip_dst)%4) {
        printf("mis-aligned ip_hdr->ip_dst %d\n", ((unsigned)&ip_hdr->ip_dst)%4);
    }
    #else
    CHECK_ALIGN(ip_hdr->ip_src, 4);
    CHECK_ALIGN(ip_hdr->ip_dst, 4);
    #endif
    #endif
    ip_hdr->ip_src = htonl(ip_src); // what if mis-aligned
    ip_hdr->ip_dst = htonl(ip_dst); // what if mis-aligned
#endif
    ip_hdr->ip_sum = htons(compute_ip_checksum(ip_hdr));

    return IP_HDR_LEN;
}

//-----------------------------------------------------
// Populates an IP header with the usual data.
int populate_pseudo_ip_hdr( pseudo_ip_hdr_t *ip_hdr
                          , uint32_t         ip_src // host order
                          , uint32_t         ip_dst // host order
                          , uint8_t          protocol
                          , uint16_t         length // pure payload size (not including header)
                          )
{
    ip_hdr->ip_src = htonl(ip_src);
    ip_hdr->ip_dst = htonl(ip_dst);
    ip_hdr->ip_zro = 0;
    ip_hdr->ip_pro = protocol;
    ip_hdr->ip_len = htons(length);
    return IP_HDR_LEN;
}

//-----------------------------------------------------
// Populates an UDP header with the usual data.
// It zeros checksum.
int populate_udp_hdr( udp_hdr_t *udp_hdr
                    , uint16_t   port_src // host order
                    , uint16_t   port_dst // host order
                    , uint16_t   payload_size // pure payload size (not including header)
                    ) 
{
#if defined(RIGOR)
    #if 0
    if (((unsigned)&udp_hdr->udp_src)%2) {
        printf("mis-aligned udp_hdr->port_src %d\n", ((unsigned)&udp_hdr->udp_src)%2);
    }
    #else
    CHECK_ALIGN(udp_hdr->udp_src,2);
    #endif
#endif
     udp_hdr->udp_src = htons(port_src); // what if mis-aligned
     udp_hdr->udp_dst = htons(port_dst); // what if mis-aligned
     udp_hdr->udp_len = htons(UDP_HDR_LEN+payload_size); // what if mis-aligned
     udp_hdr->udp_sum = 0; // should be zero (to be used to calculate check sum with IP header)

     return UDP_HDR_LEN;
}

//-----------------------------------------------------
// Populates an TCP header with the usual data.
// It does not use packet or payload size.
// It zeros checksum.
int populate_tcp_hdr( tcp_hdr_t *tcp_hdr
                    , uint16_t   port_src // host order
                    , uint16_t   port_dst // host order
                    , uint32_t   num_seq  // host order
                    , uint32_t   num_ack  // host order
                    ) 
{
#if defined(RIGOR)
    if (sizeof(tcp_hdr_t)!=TCP_HDR_LEN) {
        printf("TCP header length error %zu (should be 20)\n", sizeof(tcp_hdr_t));
    }
    #if 0
    if (((unsigned)&tcp_hdr->port_src)%2) {
        printf("mis-aligned tcp_hdr->port_src %d\n", ((unsigned)&tcp_hdr->port_src)%2);
    }
    if (((unsigned)&tcp_hdr->tcp_seq)%4) {
        printf("mis-aligned tcp_hdr->tcp_seq %d\n", ((unsigned)&tcp_hdr->tcp_seq)%4);
    }
    #else
    CHECK_ALIGN(tcp_hdr->port_src, 2);
    CHECK_ALIGN(tcp_hdr->tcp_seq, 4);
    #endif
#endif
     tcp_hdr->port_src = htons(port_src); // what if mis-aligned
     tcp_hdr->port_dst = htons(port_dst); // what if mis-aligned
     tcp_hdr->tcp_seq  = htonl(num_seq ); // what if mis-aligned
     tcp_hdr->tcp_ack  = htonl(num_ack ); // what if mis-aligned
     tcp_hdr->tcp_hl   = (TCP_HDR_LEN+3)/4; // it should be 5; higher 4-bit is valid
     tcp_hdr->tcp_rsv  = 0; // reserved
     tcp_hdr->tcp_ctl  = 0; // lower 6-bit is valid
     tcp_hdr->tcp_win  = 0;
     tcp_hdr->tcp_sum  = 0; // should be zero (to be used to calculate check sum with IP header)
     tcp_hdr->tcp_pnt  = 0;

     return TCP_HDR_LEN;
}

//-----------------------------------------------------
// It generates raw Ethernet packet.
// 1. add preamble if 'add_preamble' is 1
// 2. build Ethernet header
// 3. copy payload data from 'payload' to 'packet'
// 4. add padding if required
// 5. add crc if 'add_crc' is 1
int gen_eth_packet( uint8_t  *packet
                  , uint8_t   mac_src[6] // network order
                  , uint8_t   mac_dst[6] // network order
                  , uint16_t  type_len   // type-length host order
                  , uint16_t  payload_len // Ethernet payload length
                  , uint8_t  *payload // pure payload
                  , int add_crc
                  , int add_preamble
                  )
{
    int pkt_len=0;

    //----------------------------------------------------------------------------
    #if defined(RIGOR)
    if (type_len==0) printf("%s()@%s type-len should be positive number, but %d\n", __FUNCTION__, __FILE__, type_len);
    if (payload_len==0) printf("%s()@%s payload-len should be positive number, but %d\n", __FUNCTION__, __FILE__, payload_len);
    #endif
    //----------------------------------------------------------------------------
    // add preamble
    if (add_preamble) {
        int idx;
        for (idx=0; idx<7; idx++) packet[idx] = 0x55;
        packet[7] = 0xD5;
        pkt_len += 8;
    }
    //----------------------------------------------------------------------------
    // fill Ethernet header
    eth_hdr_t* eth_hdr = (eth_hdr_t*)&packet[pkt_len];
    pkt_len += populate_eth_hdr( eth_hdr
                               , mac_src
                               , mac_dst
                               , type_len);

    //----------------------------------------------------------------------------
    // copy payload
    uint8_t *pld = (uint8_t*)(((uint8_t*)eth_hdr)+ETH_HDR_LEN);
    if (payload!=0) {
        memcpy((void*)pld, (void*)payload, payload_len);
    }

    //----------------------------------------------------------------------------
    // add crc if any
    if (add_crc) {
        int idx;
        for (idx=payload_len; idx<46; idx++) {
             // fill padding if packe is less than 46.
             // Ethernet requires minimum 46-byte of its payload,
             // which is pure Ethernet payload, i.e., excluding MAC SRC/DST/TypeLen.
             pld[idx] = 0x00;
        }
        pkt_len += idx;
        uint32_t crc = compute_eth_crc((uint8_t*)eth_hdr, ETH_HDR_LEN+idx);
        memcpy((void*)&pld[idx], (void*)&crc, 4);
        pkt_len += 4;
    } else {
      pkt_len += payload_len;
    }

    //----------------------------------------------------------------------------

    return pkt_len;
}

//-----------------------------------------------------
// It generates IP packet.
// 1. build IP header
// 2. copy payload data from 'payload' to 'packet'
// 3. update checksum for TCP or UDP
//    (to do this, 'payload' should be proper packet)
int gen_ip_packet( uint8_t  *packet
                 , uint32_t  ip_src // host order order
                 , uint32_t  ip_dst // host order order
                 , uint8_t   protocol
                 , uint8_t   ttl
                 , uint16_t  payload_len // IP payload length
                 , uint8_t  *payload // pure payload
                 , int       check // update UDP or TCP header checksum when 1
                 )
{
    int pkt_len=0;

    //----------------------------------------------------------------------------
    // fill Ethernet header
    ip_hdr_t* ip_hdr = (ip_hdr_t*)packet;
    pkt_len += populate_ip_hdr( ip_hdr
                              , ip_src
                              , ip_dst
                              , protocol
                              , ttl
                              , payload_len);

    //----------------------------------------------------------------------------
    // copy payload
    uint8_t *pld = (uint8_t*)(((uint8_t*)ip_hdr)+IP_HDR_LEN);
    if (payload!=0) {
        memcpy((void*)pld, (void*)payload, payload_len);
    }
    pkt_len += payload_len;

    //----------------------------------------------------------------------------
    // calculate TCP/UDP packet checksum
    if (check&&payload_len) {
        pseudo_ip_hdr_t pseudo_ip_hdr;
        pseudo_ip_hdr.ip_src = htonl(ip_src);
        pseudo_ip_hdr.ip_dst = htonl(ip_dst);
        pseudo_ip_hdr.ip_zro = 0x00;
        pseudo_ip_hdr.ip_pro = protocol;
        if (protocol==IP_PROTO_TCP) {
            tcp_hdr_t* tcp_hdr = (tcp_hdr_t*)(((uint8_t*)ip_hdr)+IP_HDR_LEN);
            pseudo_ip_hdr.ip_len = htons(TCP_HDR_LEN+payload_len);
            tcp_hdr->tcp_sum = htons(compute_tcp_checksum(&pseudo_ip_hdr, tcp_hdr));
        } else 
        if (protocol==IP_PROTO_UDP) {
            udp_hdr_t* udp_hdr = (udp_hdr_t*)(((uint8_t*)ip_hdr)+IP_HDR_LEN);
            pseudo_ip_hdr.ip_len = htons(UDP_HDR_LEN+payload_len);
            udp_hdr->udp_sum = htons(compute_udp_checksum(&pseudo_ip_hdr, udp_hdr));
        }
    }

    //----------------------------------------------------------------------------
    return pkt_len;
}

//-----------------------------------------------------
// It generates UDP packet.
// 1. build IP header
// 2. copy payload data from 'payload' to 'packet'
int gen_udp_packet( uint8_t  *packet
                  , uint16_t  port_src // host order order
                  , uint16_t  port_dst // host order order
                  , uint16_t  payload_len // IP payload length
                  , uint8_t  *payload // pure payload
                  )
{
    int pkt_len=0;

    //----------------------------------------------------------------------------
    // fill Ethernet header
    udp_hdr_t* udp_hdr = (udp_hdr_t*)packet;
    pkt_len += populate_udp_hdr( udp_hdr
                               , port_src
                               , port_dst
                               , payload_len);

    //----------------------------------------------------------------------------
    // copy payload
    uint8_t *pld = (uint8_t*)(((uint8_t*)udp_hdr)+UDP_HDR_LEN);
    if (payload!=0) {
        memcpy((void*)pld, (void*)payload, payload_len);
    }
    pkt_len += payload_len;

    //----------------------------------------------------------------------------
    return pkt_len;
}

//-----------------------------------------------------
// It generates TCP packet.
// 1. build IP header
// 2. copy payload data from 'payload' to 'packet'
//
// Note that it zeros checksum. So that checksum calcluation
// should be called explicitly with pseudo IP header.
int gen_tcp_packet( uint8_t  *packet
                  , uint16_t  port_src // host order order
                  , uint16_t  port_dst // host order order
                  , uint32_t  num_seq  // host order order
                  , uint32_t  num_ack  // host order order
                  , uint16_t  payload_len // IP payload length
                  , uint8_t  *payload // pure payload
                  )
{
    int pkt_len=0;

    //----------------------------------------------------------------------------
    // fill Ethernet header
    tcp_hdr_t* tcp_hdr = (tcp_hdr_t*)packet;
    pkt_len += populate_tcp_hdr( tcp_hdr
                               , port_src
                               , port_dst
                               , num_seq 
                               , num_ack);

    //----------------------------------------------------------------------------
    // copy payload
    uint8_t *pld = (uint8_t*)(((uint8_t*)tcp_hdr)+TCP_HDR_LEN);
    if (payload!=0) {
        memcpy((void*)pld, (void*)payload, payload_len);
    }
    pkt_len += payload_len;

    //----------------------------------------------------------------------------
    return pkt_len;
}

//-----------------------------------------------------
int gen_eth_arp_packet( uint8_t  *packet
                      , uint8_t   mac_src[6] // network order
                      , uint8_t   mac_dst[6] // network order
                      , uint16_t  type       // ARP type
                      , uint32_t  ip_src     // host order
                      , uint32_t  ip_dst     // host order
                      , int add_crc
                      , int add_preamble
                      )
{
    int pkt_len=0;
    //----------------------------------------------------------------------------
    // add preamble
    if (add_preamble) {
        int idx;
        for (idx=0; idx<7; idx++) packet[idx] = 0x55;
        packet[7] = 0xD5;
        pkt_len = 8;
    }
    //----------------------------------------------------------------------------
    // fill Ethernet header
    eth_hdr_t* eth_hdr = (add_preamble) ? (eth_hdr_t*)&packet[8]
                                        : (eth_hdr_t*)packet;
    pkt_len += populate_eth_hdr( eth_hdr
                               , mac_src
                               , mac_dst
                               , ETH_TYPE_ARP);
    //----------------------------------------------------------------------------
    // fill ARP
    arp_hdr_t* arp_hdr = (arp_hdr_t*)(((uint8_t*)eth_hdr)+ETH_HDR_LEN);
    pkt_len += populate_arp_hdr( arp_hdr
                               , type, mac_src, mac_dst, ip_src, ip_dst);
    //----------------------------------------------------------------------------
    // add crc if any
    if (add_crc) {
        int idx;
        uint8_t *pld = (uint8_t*)(((uint8_t*)arp_hdr)+ARP_HDR_LEN);
        for (idx=0; idx<(46-ARP_HDR_LEN); idx++) {
             // fill padding if packe is less than 46.
             // Ethernet requires minimum 46-byte of its payload,
             // which is pure Ethernet payload, i.e., excluding MAC SRC/DST/TypeLen.
             pld[idx] = 0x00;
        }
        pkt_len += idx;
        uint32_t crc = compute_eth_crc((uint8_t*)eth_hdr, ETH_HDR_LEN+ARP_HDR_LEN+idx);
        memcpy((void*)&arp_hdr[ARP_HDR_LEN], (void*)&crc, 4);
        pkt_len += 4;
    }
    //----------------------------------------------------------------------------
    return pkt_len;
}

//-----------------------------------------------------
// It generates Ethernet packet containing UDP over IP.
// 1. add preamble if 'add_preamble' is 1
// 2. build Ethernet header
// 3. build IP header
// 4. build UDP header
// 5. copy payload data from 'payload' to 'packet', when 'payload' is not 0
// 6. add padding if required
// 7. add crc if 'add_crc' is 1
//
// Note that 'payload_len' can be non-zero whiel 'payload' is 0,
//           when UDP payload has been built before being called this.
int gen_eth_ip_udp_packet( uint8_t  *packet
                         , uint8_t   mac_src[6] // network order
                         , uint8_t   mac_dst[6] // network order
                         , uint32_t  ip_src     // host order
                         , uint32_t  ip_dst     // host order
                         , uint16_t  port_src   // 0x0001; host order
                         , uint16_t  port_dst   // 0x0002; host order
                         , uint16_t  payload_len // UDP payload length
                         , uint8_t  *payload // udp payload (pure)
                         , int check           // update UDP header checksum when 1
                         , int add_crc         // add CRC at the end of packet when 1
                         , int add_preamble    // add preamble at the beginnin of packet when 1
                         )
{
    int pkt_len=0;
    //----------------------------------------------------------------------------
    // add preamble
    if (add_preamble) {
        int idx;
        for (idx=0; idx<7; idx++) packet[idx] = 0x55;
        packet[7] = 0xD5;
        pkt_len = 8;
    }
    //----------------------------------------------------------------------------
    // fill Ethernet header
    eth_hdr_t* eth_hdr = (add_preamble) ? (eth_hdr_t*)&packet[8]
                                        : (eth_hdr_t*)packet;
    pkt_len += populate_eth_hdr( eth_hdr
                               , mac_src
                               , mac_dst
                               , ETH_TYPE_IP);

    //----------------------------------------------------------------------------
    // fill IP header
    ip_hdr_t* ip_hdr = (ip_hdr_t*)(((uint8_t*)eth_hdr)+ETH_HDR_LEN);
    pkt_len += populate_ip_hdr( ip_hdr
                              , ip_src
                              , ip_dst
                              , IP_PROTO_UDP
                              , 0x01
                              , UDP_HDR_LEN + payload_len);

    //----------------------------------------------------------------------------
    // fill UDP header
    udp_hdr_t* udp_hdr = (udp_hdr_t*)(((uint8_t*)ip_hdr)+IP_HDR_LEN);
    pkt_len += populate_udp_hdr( udp_hdr
                               , port_src
                               , port_dst
                               , payload_len);

    //----------------------------------------------------------------------------
    // copy UDP payload
    uint8_t *pld = (uint8_t*)(((uint8_t*)udp_hdr)+UDP_HDR_LEN);
    if (payload!=0) {
        memcpy((void*)pld, (void*)payload, payload_len);
    }

    //----------------------------------------------------------------------------
    // calculate UDP checksum
    if (check&&payload_len) {
        pseudo_ip_hdr_t pseudo_ip_hdr;
        pseudo_ip_hdr.ip_src = htonl(ip_src);
        pseudo_ip_hdr.ip_dst = htonl(ip_dst);
        pseudo_ip_hdr.ip_zro = 0x00;
        pseudo_ip_hdr.ip_pro = IP_PROTO_UDP;
        pseudo_ip_hdr.ip_len = htons(UDP_HDR_LEN+payload_len);
        udp_hdr->udp_sum = htons(compute_udp_checksum(&pseudo_ip_hdr, udp_hdr));
    }

    //----------------------------------------------------------------------------
    // add crc if any
    if (add_crc) {
        int idx;
        for (idx=payload_len; idx<(46-IP_HDR_LEN-UDP_HDR_LEN); idx++) {
             // fill padding if packe is less than 46.
             // Ethernet requires minimum 46-byte of its payload,
             // which is pure Ethernet payload, i.e., excluding MAC SRC/DST/TypeLen.
             pld[idx] = 0x00;
        }
        pkt_len += idx;
        uint32_t crc = compute_eth_crc((uint8_t*)eth_hdr, ETH_HDR_LEN+IP_HDR_LEN+UDP_HDR_LEN+idx);
        memcpy((void*)&pld[idx], (void*)&crc, 4);
        pkt_len += 4;
    } else {
      pkt_len += payload_len;
    }

    //----------------------------------------------------------------------------
    return pkt_len;
}

//-----------------------------------------------------
// It generates Ethernet packet containing TCP over IP.
// 1. add preamble if 'add_preamble' is 1
// 2. build Ethernet header
// 3. build IP header
// 4. build TCP header
// 5. copy payload data from 'payload' to 'packet', when 'payload' is not 0
// 6. add padding if required
// 7. add crc if 'add_crc' is 1
//
// Note that 'payload_len' can be non-zero whiel 'payload' is 0,
//           when TCP payload has been built before being called this.
int gen_eth_ip_tcp_packet( uint8_t  *packet
                         , uint8_t   mac_src[6] // network order
                         , uint8_t   mac_dst[6] // network order
                         , uint32_t  ip_src     // host order
                         , uint32_t  ip_dst     // host order
                         , uint16_t  port_src   // 0x0001; host order
                         , uint16_t  port_dst   // 0x0001; host order
                         , uint32_t  num_seq    // host order
                         , uint32_t  num_ack    // host order
                         , uint16_t  payload_len // TCP payload length
                         , uint8_t  *payload   // tcp payload (pure)
                         , int check           // update TCP header checksum when 1
                         , int add_crc         // add CRC at the end of packet when 1
                         , int add_preamble    // add preamble at the beginnin of packet when 1
                         )
{
    int pkt_len=0;
    //----------------------------------------------------------------------------
    // add preamble
    if (add_preamble) {
        int idx;
        for (idx=0; idx<7; idx++) packet[idx] = 0x55;
        packet[7] = 0xD5;
        pkt_len = 8;
    }
    //----------------------------------------------------------------------------
    // fill Ethernet header
    eth_hdr_t* eth_hdr = (add_preamble) ? (eth_hdr_t*)&packet[8]
                                        : (eth_hdr_t*)packet;
    pkt_len += populate_eth_hdr( eth_hdr
                               , mac_src
                               , mac_dst
                               , ETH_TYPE_IP);

    //----------------------------------------------------------------------------
    // fill IP header
    ip_hdr_t* ip_hdr = (ip_hdr_t*)(((uint8_t*)eth_hdr)+ETH_HDR_LEN);
    pkt_len += populate_ip_hdr( ip_hdr
                              , ip_src
                              , ip_dst
                              , IP_PROTO_TCP
                              , 0x01 // TTL
                              , TCP_HDR_LEN + payload_len);

    //----------------------------------------------------------------------------
    // fill TCP header
    tcp_hdr_t* tcp_hdr = (tcp_hdr_t*)(((uint8_t*)ip_hdr)+IP_HDR_LEN);
    pkt_len += populate_tcp_hdr( tcp_hdr
                               , port_src
                               , port_dst
                               , num_seq 
                               , num_ack);

    //----------------------------------------------------------------------------
    // copy TCP payload
    uint8_t *pld = (uint8_t*)(((uint8_t*)tcp_hdr)+TCP_HDR_LEN);
    if (payload!=0) {
        memcpy((void*)pld, (void*)payload, payload_len);
    }

    //----------------------------------------------------------------------------
    // calculate TCP checksum
    if (check&&payload_len) {
        // payload_len should be positive
        pseudo_ip_hdr_t pseudo_ip_hdr;
        pseudo_ip_hdr.ip_src = htonl(ip_src);
        pseudo_ip_hdr.ip_dst = htonl(ip_dst);
        pseudo_ip_hdr.ip_zro = 0x00;
        pseudo_ip_hdr.ip_pro = IP_PROTO_TCP;
        pseudo_ip_hdr.ip_len = htons(TCP_HDR_LEN+payload_len);
        tcp_hdr->tcp_sum = htons(compute_tcp_checksum(&pseudo_ip_hdr, tcp_hdr));
    }

    //----------------------------------------------------------------------------
    // add crc if any
    if (add_crc&&payload_len) {
        // payload_len should be positive
        int idx;
        for (idx=payload_len; idx<(46-IP_HDR_LEN-TCP_HDR_LEN); idx++) {
             // fill padding if packe is less than 46.
             // Ethernet requires minimum 46-byte of its payload,
             // which is pure Ethernet payload, i.e., excluding MAC SRC/DST/TypeLen.
             pld[idx] = 0x00;
        }
        pkt_len += idx;
        uint32_t crc = compute_eth_crc((uint8_t*)eth_hdr, ETH_HDR_LEN+IP_HDR_LEN+TCP_HDR_LEN+idx);
        memcpy((void*)&pld[idx], (void*)&crc, 4);
        pkt_len += 4;
    } else {
      pkt_len += payload_len;
    }

    //----------------------------------------------------------------------------
    return pkt_len;
}

//-----------------------------------------------------------------------------
int parser_eth_packet(uint8_t *pkt, int leng)
{
  int idx;
  uint16_t type_leng=0;
  //eth_hdr_t *eth_hdr = (eth_hdr_t*)pkt;
  idx = 0;
  if (leng>=12) {
      int idy;
      printf("ETH mac dst  : 0x");
      for (idy=0; idy<6; idy++) printf("%02X",pkt[idx++]);
      printf("\n");
      printf("ETH mac src  : 0x");
      for (idy=0; idy<6; idy++) printf("%02X",pkt[idx++]);
      printf("\n");
  }
  if (leng>=(idx+2)) {
      type_leng  = pkt[idx++]<<8;
      type_leng |= pkt[idx++];
      printf("ETH type leng: 0x%04X", type_leng);
      switch (type_leng) {
      case 0x0800: printf(" (IPv4  packet)\n"); break;
      case 0x0806: printf(" (ARP   packet)\n"); break;
      case 0x08DD: printf(" (IPv6  packet)\n"); break;
      case 0x8100: printf(" (VLAN  packet)\n"); break;
      case 0x88F7: printf(" (PTPv2 raw packet)\n"); break;
      default:     printf("\n"); break;
      }
   }
   switch (type_leng) {
   case 0x0800: parser_ip_packet(pkt+ETH_HDR_LEN, leng-ETH_HDR_LEN); break;
   }
   return 0;
}

//-----------------------------------------------------------------------------
int parser_pseudo_ip_hdr(uint8_t *pkt)
{
    pseudo_ip_hdr_t *ip_hdr = (pseudo_ip_hdr_t*)pkt;
    printf("Pseudo IP source address        0x%08X\n", ntohl(ip_hdr->ip_src));
    printf("Pseudo IP dest address          0x%08X\n", ntohl(ip_hdr->ip_dst));
    printf("Pseudo IP zero                  0x%02X\n",       ip_hdr->ip_zro );
    printf("Pseudo IP protocol              0x%02X  ",       ip_hdr->ip_pro );
    switch (ip_hdr->ip_pro) {
    case 0x11: printf("(UDP)\n"); break;
    case 0x06: printf("(TCP)\n"); break;
    default:   printf("\n"); break;
    }
    printf("Pseudo IP total length          0x%04X\n", ntohs(ip_hdr->ip_len));
    return 0;
}

//-----------------------------------------------------------------------------
int parser_ip_packet(uint8_t *pkt, int leng)
{
    ip_hdr_t *ip_hdr = (ip_hdr_t*)pkt;
    printf("IP header length         0x%01X\n",       ip_hdr->ip_hdl );
    printf("IP version               0x%01X\n",       ip_hdr->ip_ver );
    printf("IP type of service       0x%02X\n",       ip_hdr->ip_tos );
    printf("IP total length          0x%04X\n", ntohs(ip_hdr->ip_len));
    printf("IP identification        0x%04X\n", ntohs(ip_hdr->ip_id ));
    printf("IP fragment offset field 0x%04X\n", ntohs(ip_hdr->ip_off));
    printf("IP time to live          0x%02X\n",       ip_hdr->ip_ttl );
    printf("IP protocol              0x%02X  ",       ip_hdr->ip_pro );
    switch (ip_hdr->ip_pro) {
    case 0x11: printf("(UDP)\n"); break;
    case 0x06: printf("(TCP)\n"); break;
    case 0x01: printf("(ICMP)\n"); break;
    case 0x02: printf("(IGMP)\n"); break;
    case 0x5E: printf("(ICMP)\n"); break;
    default:   printf("\n"); break;
    }
    printf("IP checksum              0x%04X\n", ntohs(ip_hdr->ip_sum));
    printf("IP source address        0x%08X\n", ntohl(ip_hdr->ip_src));
    printf("IP dest address          0x%08X\n", ntohl(ip_hdr->ip_dst));

    switch (ip_hdr->ip_pro) {
    case 0x11: // UDP
         parser_udp_packet(pkt+(ip_hdr->ip_hdl*4), leng-(ip_hdr->ip_hdl*4));
         break;
    case 0x06: // TCP
         parser_tcp_packet(pkt+(ip_hdr->ip_hdl*4), leng-(ip_hdr->ip_hdl*4));
         break;
    case 0x01: // ICMP
    case 0x02: // IGMP
    case 0x5E: // ICMP
    default: printf("not implemented yet\n");
    }
    return 0;
}

//-----------------------------------------------------------------------------
int parser_udp_packet(uint8_t *pkt, int leng)
{
    udp_hdr_t *udp_hdr = (udp_hdr_t*)pkt;
    printf("UDP source port           0x%04X\n", ntohs(udp_hdr->udp_src));
    printf("UDP destination port      0x%04X\n", ntohs(udp_hdr->udp_dst));
    printf("UDP total length in bytes 0x%04X\n", ntohs(udp_hdr->udp_len));
    printf("UDP checksum              0x%04X\n", ntohs(udp_hdr->udp_sum));
    return 0;
}

//-----------------------------------------------------------------------------
int parser_tcp_packet(uint8_t *pkt, int leng)
{
    tcp_hdr_t *tcp_hdr = (tcp_hdr_t*)pkt;
    printf("TCP source port            0x%04X\n", ntohs(tcp_hdr->port_src)); // source port
    printf("TCP destination port       0x%04X\n", ntohs(tcp_hdr->port_dst)); // destination port
    printf("TCP sequence number        0x%08X\n", ntohl(tcp_hdr->tcp_seq)); // sequence number
    printf("TCP acknowledgement number 0x%08X\n", ntohl(tcp_hdr->tcp_ack)); // acknowledgement number
    printf("TCP header length          0x%01X\n",       tcp_hdr->tcp_hl); // header length (only higher 4-bit)
    printf("TCP control                0x%02X\n",       tcp_hdr->tcp_ctl); // control (only lower 6-bit)
    printf("TCP window                 0x%04X\n", ntohs(tcp_hdr->tcp_win)); // window
    printf("TCP checksum               0x%04X\n", ntohs(tcp_hdr->tcp_sum)); // checksum
    printf("TCP urgent point           0x%04X\n", ntohs(tcp_hdr->tcp_pnt)); // urgent pointer
    return 0;
}

//-----------------------------------------------------------------------------
int is_broadcast(uint32_t ip_addr, uint32_t ip_local, uint32_t subnet_mask)
{
    if (ip_addr==0xFFFFFFFF) return 1;
    else if (ip_addr==(ip_local|~subnet_mask)) return 1;
    else return 0;
}

int is_multicast(uint32_t ip_addr)
{
    if ((ip_addr>=0xE0000000)&&(ip_addr<=0xEFFFFFFF)) return 1;
    else return 0;
}

int is_outside(uint32_t ip_addr, uint32_t ip_local, uint32_t subnet_mask)
{
    if ((ip_addr&subnet_mask)==(ip_local&subnet_mask)) return 0;
    else return 1;
}

//-----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
