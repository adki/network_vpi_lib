#ifndef ETH_IP_UDP_TCP_PKT_H
#define ETH_IP_UDP_TCP_PKT_H
//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All right reserved.
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
#include "eth_ip_udp_tcp_data_type.h"

//----------------------------------------------------------------------------
extern uint32_t compute_eth_crc     ( uint8_t *pkt, int bnum );
extern uint16_t compute_checksum    ( uint8_t *pkt, int bnum );
extern uint16_t compute_ip_checksum ( ip_hdr_t *iphdr );
extern uint16_t compute_udp_checksum( pseudo_ip_hdr_t *ip_hdr, udp_hdr_t *hdr );
extern uint16_t compute_tcp_checksum( pseudo_ip_hdr_t *ip_hdr, tcp_hdr_t *hdr );
extern int      check_eth_crc     ( uint8_t *pkt, int bnum );
extern int      check_checksum    ( uint8_t *pkt, int bnum );
extern int      check_ip_checksum ( ip_hdr_t *iphdr );
extern int      check_udp_checksum( pseudo_ip_hdr_t *ip_hdr, udp_hdr_t *hdr );
extern int      check_tcp_checksum( pseudo_ip_hdr_t *ip_hdr, tcp_hdr_t *hdr );

//----------------------------------------------------------------------------
extern int populate_eth_hdr( eth_hdr_t *ether_hdr
                           , uint8_t    mac_src[6] // network order
                           , uint8_t    mac_dst[6] // network order
                           , uint16_t   type_len); // host order
extern int populate_arp_hdr( arp_hdr_t *hdr
                           , uint16_t   type       // message type 0: ARP req, 2: ARP reply
                           , uint8_t    mac_src[6] // network order
                           , uint8_t    mac_dst[6] // network order
                           , uint32_t   ip_src     // host order
                           , uint32_t   ip_dst);   // host order
extern int populate_ip_hdr( ip_hdr_t *ip_hdr
                          , uint32_t  ip_src // host order
                          , uint32_t  ip_dst // host order
                          , uint8_t   protocol
                          , uint8_t   ttl
                          , uint16_t  payload_size);// pure payload size in host order
extern int populate_pseudo_ip_hdr( pseudo_ip_hdr_t *ip_hdr
                                 , uint32_t         ip_src // host order
                                 , uint32_t         ip_dst // host order
                                 , uint8_t          protocol
                                 , uint16_t         length);// pure payload size in host order
extern int populate_udp_hdr( udp_hdr_t *udp_hdr
                           , uint16_t   port_src // host order
                           , uint16_t   port_dst // host order
                           , uint16_t   payload_size); // pure payload size in host order
extern int populate_tcp_hdr( tcp_hdr_t *tcp_hdr
                           , uint16_t   port_src // host order
                           , uint16_t   port_dst // host order
                           , uint32_t   num_seq  // host order
                           , uint32_t   num_ack);// host order

//----------------------------------------------------------------------------
extern int gen_eth_packet( uint8_t  *packet
                         , uint8_t   mac_src[6] // network order
                         , uint8_t   mac_dst[6] // network order
                         , uint16_t  type_len   // type-length in host order
                         , uint16_t  payload_len // payload length
                         , uint8_t  *payload // payload if not 0
                         , int add_crc
                         , int add_preamble);

// It fills ARP packet and returns length.
#define gen_arp_packet populate_arp_hdr

extern int gen_ip_packet( uint8_t  *packet
                        , uint32_t  ip_src // host order
                        , uint32_t  ip_dst // host order
                        , uint8_t   protocol // type-length in host order
                        , uint8_t   ttl      // time-to-live
                        , uint16_t  payload_len // IP payload length
                        , uint8_t  *payload   // payload if not 0
                        , int       check); // update TCP checksum if 1

extern int gen_udp_packet( uint8_t  *packet
                         , uint16_t  port_src // host order
                         , uint16_t  port_dst // host order
                         , uint16_t  payload_len // UDP payload length
                         , uint8_t  *payload); // payload if not 0

extern int gen_tcp_packet( uint8_t  *packet
                         , uint16_t  port_src // host order
                         , uint16_t  port_dst // host order
                         , uint32_t  num_seq  // host order
                         , uint32_t  num_ack  // host order
                         , uint16_t  payload_len // TCP payload length
                         , uint8_t  *payload); // payload if not 0

// It fills ARP packet and returns length
extern int gen_eth_arp_packet( uint8_t  *packet
                             , uint8_t   mac_src[6] // network order
                             , uint8_t   mac_dst[6] // network order
                             , uint16_t  type       // ARP type
                             , uint32_t  ip_src     // host order
                             , uint32_t  ip_dst     // host order
                             , int add_crc // add CRC when 1
                             , int add_preamble); // add preamble when 1

extern int gen_eth_ip_udp_packet( uint8_t  *packet
                                , uint8_t   mac_src[6] // network order
                                , uint8_t   mac_dst[6] // network order
                                , uint32_t  ip_src     // host order
                                , uint32_t  ip_dst     // host order
                                , uint16_t  port_src   // host order
                                , uint16_t  port_dst   // host order
                                , uint16_t  payload_len// UDP payload length
                                , uint8_t  *payload // payload if not 0
                                , int check // update UDP header checksum
                                , int add_crc // add CRC when 1
                                , int add_preamble); // add preamble when 1

extern int gen_eth_ip_tcp_packet( uint8_t  *packet
                                , uint8_t   mac_src[6] // network order
                                , uint8_t   mac_dst[6] // network order
                                , uint32_t  ip_src     // host order
                                , uint32_t  ip_dst     // host order
                                , uint16_t  port_src   // host order
                                , uint16_t  port_dst   // host order
                                , uint32_t  num_seq    // host order
                                , uint32_t  num_ack    // host order
                                , uint16_t  payload_len// TCP payload length
                                , uint8_t  *payload // payload if not 0
                                , int check // update TCP header checksum
                                , int add_crc // add CRC when 1
                                , int add_preamble); // add preamble when 1

//----------------------------------------------------------------------------
extern int parser_eth_packet   (uint8_t *pkt, int leng);
extern int parser_pseudo_ip_hdr(uint8_t *pkt);
extern int parser_ip_packet    (uint8_t *pkt, int leng);
extern int parser_udp_packet   (uint8_t *pkt, int leng);
extern int parser_tcp_packet   (uint8_t *pkt, int leng);
//----------------------------------------------------------------------------
extern int is_broadcast(uint32_t ip_addr, uint32_t ip_local, uint32_t subnet_mask);
extern int is_multicast(uint32_t ip_addr);
extern int is_outside(uint32_t ip_addr, uint32_t ip_local, uint32_t subnet_mask);

//-----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
#endif /*ETH_IP_UDP_PKT_H*/
