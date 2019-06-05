#ifndef PTPV2_MESSAGE_H
#define PTPV2_MESSAGE_H
//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All right reserved.
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
// ptpv2_message.h
//----------------------------------------------------------------------------
#include "eth_ip_udp_tcp_pkt.h"
#include "ptpv2_type.h"
#include "ptpv2_etc.h"
#include "ptpv2_context.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int (*gen_ptpv2_msg[16])( ptpv2_ctx_t *ctx
                               , uint8_t     *msg
                               , ptpv2_msg_hdr_t *hdr
                               , Timestamp_t     *time
                               , PortIdentity_t  *port);

extern int populate_ptpv2_msg_hdr( ptpv2_ctx_t     *ctx
                                 , ptpv2_msg_hdr_t *msg_hdr
                                 , uint8_t          type
                                 , uint16_t         flag
                                 , uint64_t         correction // 8-byte
                                 , uint32_t         oui // 3-byte
                                 , uint64_t         uuid // 5-byte
                                 , uint16_t         ptp_port // 2-byte
                                 , uint16_t         seq_id
                                 , uint8_t          control);

extern int gen_ptpv2_msg_sync( ptpv2_ctx_t *ctx
                             , uint8_t     *msg
                             , ptpv2_msg_hdr_t *hdr
                             , Timestamp_t     *time
                             , PortIdentity_t  *port);

extern int gen_ptpv2_msg_follow_up( ptpv2_ctx_t *ctx
                                  , uint8_t     *msg
                             , ptpv2_msg_hdr_t *hdr
                             , Timestamp_t     *time
                             , PortIdentity_t  *port);

extern int gen_ptpv2_msg_delay_req( ptpv2_ctx_t *ctx
                             , uint8_t     *msg
                             , ptpv2_msg_hdr_t *hdr
                             , Timestamp_t     *time
                             , PortIdentity_t  *port);

extern int gen_ptpv2_msg_pdelay_req( ptpv2_ctx_t *ctx
                                  , uint8_t     *msg
                             , ptpv2_msg_hdr_t *hdr
                             , Timestamp_t     *time
                             , PortIdentity_t  *port);

extern int gen_ptpv2_msg_pdelay_resp( ptpv2_ctx_t *ctx
                                    , uint8_t     *msg
                                    , ptpv2_msg_hdr_t *hdr
                                    , Timestamp_t     *time
                                    , PortIdentity_t  *port);

extern int gen_ptpv2_msg_delay_resp( ptpv2_ctx_t *ctx
                                   , uint8_t     *msg
                                   , ptpv2_msg_hdr_t *hdr
                                   , Timestamp_t     *time
                                   , PortIdentity_t  *port);

extern int gen_ptpv2_msg_pdelay_resp_follow_up( ptpv2_ctx_t *ctx
                                              , uint8_t     *msg
                                              , ptpv2_msg_hdr_t *hdr
                                              , Timestamp_t     *time
                                              , PortIdentity_t  *port);

extern int gen_ptpv2_msg_announce( ptpv2_ctx_t *ctx
                                  , uint8_t     *msg
                                  , ptpv2_msg_hdr_t *hdr
                                  , Timestamp_t     *time
                                  , PortIdentity_t  *port);

extern int gen_ptpv2_msg_signaling( ptpv2_ctx_t *ctx
                                  , uint8_t     *msg
                                  , ptpv2_msg_hdr_t *hdr
                                  , Timestamp_t     *time
                                  , PortIdentity_t  *port);

extern int gen_ptpv2_msg_management( ptpv2_ctx_t *ctx
                                   , uint8_t     *msg
                                   , ptpv2_msg_hdr_t *hdr
                                   , Timestamp_t     *time
                                   , PortIdentity_t  *port);

extern int gen_ptpv2_msg_ethernet( ptpv2_ctx_t *ctx
                                 , uint8_t     *msg  // PTPv2 over Ethernet message to be buit
                                 , uint8_t      mac_src[6]
                                 , ptpv2_msg_hdr_t *hdr
                                 , Timestamp_t     *time
                                 , PortIdentity_t  *port // valid for Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
                                 , int          add_crc // add CRC at the end
                                 , int          add_preamble // add preamble at the beginning
                                 );
extern int gen_ptpv2_msg_udp_ip_ethernet( ptpv2_ctx_t *ctx
                                        , uint8_t     *msg  // PTPv2 over Ethernet message to be buit
                                        , uint8_t      mac_src[6]
                                        , uint32_t     ip_src
                                        , ptpv2_msg_hdr_t *hdr
                                        , Timestamp_t     *time
                                        , PortIdentity_t  *port // only valid when Delay_Req, Pdelay_Resp, Pdelay_Resp_Follow_Up
                                        , int          add_crc // add CRC at the end
                                        , int          add_preamble // add preamble at the beginning
                                        );
ptpv2_ctx_t *gen_ptpv2_context( uint32_t ptp_version   
                              , uint32_t ptp_domain    
                              , uint32_t one_step_clock
                              , uint32_t unicast_port  
                              , uint32_t profile_spec1 
                              , uint32_t profile_spec2 
                              );

ptpv2_ctx_t *get_ptpv2_context( );

extern int parser_ptpv2_message(uint8_t *pkt, int leng);

#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
#endif // PTPV2_MESSAGE_H
