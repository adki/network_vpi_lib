//----------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include "eth_ip_udp_tcp_pkt.h"

//----------------------------------------------------------------------------
static uint32_t my_rand(void);
static void my_srand(uint32_t seed);

//----------------------------------------------------------------------------
uint8_t  mac_src[6] = { 0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
uint8_t  mac_dst[6] = { 0xF3, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
uint32_t ip_src = 0xC0123456;
uint32_t ip_dst = 0xCF876543;
uint16_t port_src = 0xABCD;
uint16_t port_dst = 0xEF01;
//----------------------------------------------------------------------------
// Return 0 on success, 1 on failure
int test_checksum(void)
{
    int idx, idy, ret1, ret2, ret3;

    uint8_t packet [1024];
    uint8_t payload[1024];
    pseudo_ip_hdr_t piphdr;
    uint16_t leng, pleng;
    uint16_t ip_checksum;
    uint16_t pkt_checksum;

    for (idx=1; idx<200; idx++) {
         for (idy=0; idy<idx; idy++) {
              payload[idy] = my_rand()&0xFF;
         }
         pleng = idy; // payload length
         //-------------------------------------------------------------------
         leng = gen_eth_ip_udp_packet( packet
                                     , mac_src
                                     , mac_dst
                                     , ip_src
                                     , ip_dst
                                     , port_src
                                     , port_dst
                                     , pleng
                                     , payload
                                     , 1 // checksum add
                                     , 1 // crc add
                                     , 0); // preamble add
         //parser_ip_packet(&packet[ETH_HDR_LEN], IP_HDR_LEN+UDP_HDR_LEN+pleng);
         populate_pseudo_ip_hdr(&piphdr, ip_src, ip_dst, IP_PROTO_UDP
                               , UDP_HDR_LEN+pleng);
         //parser_pseudo_ip_hdr((uint8_t*)&piphdr);
         ret1 = check_ip_checksum((ip_hdr_t*)&packet[ETH_HDR_LEN]);
         ret2 = check_udp_checksum(&piphdr, (udp_hdr_t*)&packet[ETH_HDR_LEN+IP_HDR_LEN]);
         ret3 = check_eth_crc(packet, leng);
         if (ret1) printf("IP header checksum error\n");
         else      printf("IP header checksum OK\n");
         if (ret2) printf("UDP packet checksum error\n");
         else      printf("UDP packet checksum OK\n");
         if (ret3) printf("Ethnet FCS error\n");
         else      printf("Ethnet FCS OK\n");

         //-------------------------------------------------------------------
         leng = gen_eth_ip_tcp_packet( packet
                                     , mac_src
                                     , mac_dst
                                     , ip_src
                                     , ip_dst
                                     , port_src
                                     , port_dst
                                     , idx // num_seq
                                     , ~idx // num_ack
                                     , pleng, payload
                                     , 1 // checksum add
                                     , 1 // crc add
                                     , 0); // preamble add
         //parser_ip_packet((uint8_t*)&packet[ETH_HDR_LEN], IP_HDR_LEN+TCP_HDR_LEN+pleng);
         populate_pseudo_ip_hdr(&piphdr, ip_src, ip_dst, IP_PROTO_TCP
                               , TCP_HDR_LEN+pleng);
         //parser_pseudo_ip_hdr((uint8_t*)&piphdr);
         ret1 = check_ip_checksum((ip_hdr_t*)&packet[ETH_HDR_LEN]);
         ret2 = check_tcp_checksum(&piphdr, (tcp_hdr_t*)&packet[ETH_HDR_LEN+IP_HDR_LEN]);
         ret3 = check_eth_crc(packet, ETH_HDR_LEN+IP_HDR_LEN+TCP_HDR_LEN+pleng+4);
         if (ret1) printf("IP header checksum error\n");
         else      printf("IP header checksum OK\n");
         if (ret2) printf("TCP packet checksum error\n");
         else      printf("TCP packet checksum OK\n");
         if (ret3) printf("Ethnet FCS error\n");
         else      printf("Ethnet FCS OK\n");
    }

    return 0;
}

//----------------------------------------------------------------------------
#define MY_RAND_MAX 0xFFFFFFFF
static uint32_t _Randseed = 1;

static uint32_t my_rand(void)
{
  _Randseed = _Randseed * 1103515245 + 12345;
  return((uint32_t)_Randseed);
}

static void my_srand(uint32_t seed)
{
  _Randseed = seed;
}
//----------------------------------------------------------------------------
