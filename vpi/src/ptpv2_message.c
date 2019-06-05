//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All right reserved.
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ptpv2_etc.h"
#include "ptpv2_context.h"
#include "ptpv2_message.h"
//#include "ptp_api.h"

//-----------------------------------------------------------------------------
static char *ptpv2_msg[] = {
/* 0x0 */   "Sync"                       
/* 0x1 */ , "Delay_Req"
/* 0x2 */ , "Pdelay_Req"
/* 0x3 */ , "Pdelay_Resp"
/* 0x4 */ , "unknown"
/* 0x5 */ , "unknown"
/* 0x6 */ , "unknown"
/* 0x7 */ , "unknown"
/* 0x8 */ , "Follow_Up"
/* 0x9 */ , "Delay_Resp"
/* 0xA */ , "Pdelay_Resp_Follow_Up"
/* 0xB */ , "Announce"
/* 0xC */ , "Signaling"
/* 0xD */ , "Management"
/* 0xE */ , "unknown"
/* 0xF */ , "unknown"
};
//-----------------------------------------------------------------------------
int gen_ptpv2_msg_unknown( ptpv2_ctx_t *ctx
                         , uint8_t     *msg
                         , ptpv2_msg_hdr_t *hdr
                         , Timestamp_t     *time
                         , PortIdentity_t  *port)
{
      PTPV2_ERROR("Un-known PTPv22 message type\n");
      return 0;
}
//-----------------------------------------------------------------------------
int (*gen_ptpv2_msg[16])( ptpv2_ctx_t     *ctx
                        , uint8_t         *msg
                        , ptpv2_msg_hdr_t *hdr
                        , Timestamp_t     *time
                        , PortIdentity_t  *port) //used by Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
= {
/* 0x0 */   gen_ptpv2_msg_sync                        
/* 0x1 */ , gen_ptpv2_msg_delay_req
/* 0x2 */ , gen_ptpv2_msg_pdelay_req
/* 0x3 */ , gen_ptpv2_msg_pdelay_resp
/* 0x4 */ , gen_ptpv2_msg_unknown
/* 0x5 */ , gen_ptpv2_msg_unknown
/* 0x6 */ , gen_ptpv2_msg_unknown
/* 0x7 */ , gen_ptpv2_msg_unknown
/* 0x8 */ , gen_ptpv2_msg_follow_up
/* 0x9 */ , gen_ptpv2_msg_delay_resp
/* 0xA */ , gen_ptpv2_msg_pdelay_resp_follow_up
/* 0xB */ , gen_ptpv2_msg_announce
/* 0xC */ , gen_ptpv2_msg_signaling
/* 0xD */ , gen_ptpv2_msg_management
/* 0xE */ , gen_ptpv2_msg_unknown
/* 0xF */ , gen_ptpv2_msg_unknown
};

//-----------------------------------------------------------------------------
// Return message length,
// Return 0 when error.
inline static uint16_t get_msg_length(uint8_t type)
{
      switch (type) {
      case PTPV2_MSG_Sync                 : return sizeof(ptpv2_msg_sync_t); break;
      case PTPV2_MSG_Delay_Req            : return sizeof(ptpv2_msg_delay_req_t); break;
      case PTPV2_MSG_Pdelay_Req           : return sizeof(ptpv2_msg_pdelay_req_t); break;
      case PTPV2_MSG_Pdelay_Resp          : return sizeof(ptpv2_msg_pdelay_resp_t); break;
      case PTPV2_MSG_Follow_Up            : return sizeof(ptpv2_msg_follow_up_t); break;
      case PTPV2_MSG_Delay_Resp           : return sizeof(ptpv2_msg_delay_resp_t); break;
      case PTPV2_MSG_Pdelay_Resp_Follow_Up: return sizeof(ptpv2_msg_pdelay_resp_follow_up_t); break;
      case PTPV2_MSG_Announce             : return sizeof(ptpv2_msg_announce_t); break;
      case PTPV2_MSG_Signaling            : return sizeof(ptpv2_msg_signaling_t); break;
      case PTPV2_MSG_Management           : return sizeof(ptpv2_msg_management_t); break;
      default: PTPV2_ERROR("Un-known PTPv22 message type: %u\n", type); return 0;
      }
}
//-----------------------------------------------------------------------------
// see IEEE.Std 1588-2008 pp.126
// Return message flags
inline static uint16_t get_msg_flags(ptpv2_ctx_t *ctx, uint8_t type)
{
      uint16_t flags=0x0000;
      switch (type) {
      case PTPV2_MSG_Sync: 
           if (!ctx->one_step_clock) flags |= PTPV2_MSG_FLAG_twoStepFlag;
           break;
      case PTPV2_MSG_Delay_Req: 
           break;
      case PTPV2_MSG_Pdelay_Req: 
           break;
      case PTPV2_MSG_Pdelay_Resp: 
           if (!ctx->one_step_clock) flags |= PTPV2_MSG_FLAG_twoStepFlag;
           break;
      case PTPV2_MSG_Follow_Up: 
           break;
      case PTPV2_MSG_Delay_Resp: 
           break;
      case PTPV2_MSG_Pdelay_Resp_Follow_Up: 
           break;
      case PTPV2_MSG_Announce:
           // need attention
           break;
      case PTPV2_MSG_Signaling: 
           break;
      case PTPV2_MSG_Management: 
           break;
      default: PTPV2_ERROR("Un-known PTPv22 message type: %u\n", type);
      }
      if (ctx->unicast_port ) flags |= PTPV2_MSG_FLAG_unicastFlag;
      if (ctx->profile_spec1) flags |= PTPV2_MSG_FLAG_profile1;
      if (ctx->profile_spec2) flags |= PTPV2_MSG_FLAG_profile2;

      return flags;
}
//-----------------------------------------------------------------------------
// Return controlField of message
inline static uint8_t get_msg_control(uint8_t type)
{
      switch (type) {
      case PTPV2_MSG_Sync      : return PTPV2_MSG_CTRL_Sync      ; break;
      case PTPV2_MSG_Delay_Req : return PTPV2_MSG_CTRL_Delay_Req ; break;
      case PTPV2_MSG_Follow_Up : return PTPV2_MSG_CTRL_Follow_Up ; break;
      case PTPV2_MSG_Delay_Resp: return PTPV2_MSG_CTRL_Delay_Resp; break;
      case PTPV2_MSG_Management: return PTPV2_MSG_CTRL_Management; break;
      default:                   return PTPV2_MSG_CTRL_All_others; break;
      }
}

//-----------------------------------------------------------------------------
int populate_ptpv2_msg_hdr( ptpv2_ctx_t     *ctx
                          , ptpv2_msg_hdr_t *msg_hdr
                          , uint8_t          type
                          , uint16_t         flag
                          , uint64_t         correction // 8-byte
                          , uint32_t         oui // 3-byte
                          , uint64_t         uuid // 5-byte
                          , uint16_t         ptp_port // 2-byte
                          , uint16_t         seq_id
                          , uint8_t          control
                          )
{
    memset((void*)msg_hdr, 0, sizeof(ptpv2_msg_hdr_t));
    msg_hdr->transportSpecific   = 0x0;
    msg_hdr->messageType         = type&0xF; // lower 4-bit
    msg_hdr->versionPTP          = ctx->ptp_version&0xF; // it should be 2
    msg_hdr->messageLength       = htons(get_msg_length(type));
    msg_hdr->domainNumber        = ctx->ptp_domain;
    msg_hdr->flagField           = htons(get_msg_flags(ctx,type));
    msg_hdr->correctionField.low = 0x0;
    msg_hdr->correctionField.high= 0x0;
    memset((void*)&msg_hdr->sourcePortIdentity,0,sizeof(PortIdentity_t)); // need attension
    msg_hdr->sequenceID         = htons(seq_id);
    msg_hdr->controlField       = get_msg_control(type);
    msg_hdr->logMessageInterval = PTPV2_MSG_DEFAULT_INTERVAL; // 0x7F

    return PTPV2_HDR_LEN; // 34
}

//-----------------------------------------------------------------------------
int gen_ptpv2_msg_sync( ptpv2_ctx_t     *ctx
                      , uint8_t         *msg
                      , ptpv2_msg_hdr_t *hdr
                      , Timestamp_t     *time
                      , PortIdentity_t  *port) //used by Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
{
    ptpv2_msg_sync_t *msg_sync = (ptpv2_msg_sync_t*)msg;
    if (hdr==NULL) {
       return 0;
    } else {
       memcpy((void*)msg_sync, (void*)hdr, PTPV2_HDR_LEN);
    }
    //ret = populate_ptpv2_msg_hdr( ctx
    //                            , (ptpv2_msg_hdr_t*)msg
    //                            , PTPV2_MSG_Sync
    //                            , seq_id
    //                            );
    if (ctx->one_step_clock) {
      memset((void*)&msg_sync->originTimestamp, 0, sizeof(Timestamp_t));
    } else {
      msg_sync->originTimestamp.secondsField.msb = htons(time->secondsField.msb);
      msg_sync->originTimestamp.secondsField.lsb = htonl(time->secondsField.lsb);
      msg_sync->originTimestamp.nanosecondsField = htonl(time->nanosecondsField);
    }
    return sizeof(ptpv2_msg_sync_t);
}

//-----------------------------------------------------------------------------
// 'time': pointer to Timestamp of previous Sync message.
int gen_ptpv2_msg_follow_up( ptpv2_ctx_t *ctx
                           , uint8_t     *msg
                           , ptpv2_msg_hdr_t *hdr
                           , Timestamp_t     *time
                           , PortIdentity_t  *port) //used by Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
{
    ptpv2_msg_follow_up_t *msg_fol = (ptpv2_msg_follow_up_t*)msg;
    if (hdr==NULL) {
        return 0;
    } else {
       memcpy((void*)msg_fol, (void*)hdr, PTPV2_HDR_LEN);
    }
    //ret = populate_ptpv2_msg_hdr( ctx
    //                            , (ptpv2_msg_hdr_t*)msg
    //                            , PTPV2_MSG_Follow_Up
    //                            , seq_id
    //                            );
    msg_fol->preciseOriginTimestamp.secondsField.msb = htons(time->secondsField.msb);
    msg_fol->preciseOriginTimestamp.secondsField.lsb = htonl(time->secondsField.lsb);
    msg_fol->preciseOriginTimestamp.nanosecondsField = htonl(time->nanosecondsField);

    return sizeof(ptpv2_msg_follow_up_t);
}
int gen_ptpv2_msg_delay_req( ptpv2_ctx_t *ctx
                      , uint8_t          *msg
                      , ptpv2_msg_hdr_t  *hdr
                      , Timestamp_t      *time
                      , PortIdentity_t   *port) //used by Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
{
    ptpv2_msg_delay_req_t *msg_delay_req = (ptpv2_msg_delay_req_t*)msg;
    if (hdr==NULL) {
        return 0;
    } else {
        memcpy((void*)msg_delay_req, (void*)hdr, PTPV2_HDR_LEN);
    }
    //ret = populate_ptpv2_msg_hdr( ctx
    //                            , (ptpv2_msg_hdr_t*)msg
    //                            , PTPV2_MSG_Sync
    //                            , seq_id
    //                            );
    if (ctx->one_step_clock) {
      memset((void*)&(msg_delay_req->originTimestamp), 0, sizeof(Timestamp_t));
    } else {
      //ret = ptp_get_time_rtc(&sec_msb, &sec_lsb, &nano);
      //if (ret) {
      //    PTPV2_ERROR("something wrong while getting ptp_get_time\n");
      //}
      msg_delay_req->originTimestamp.secondsField.msb = htons(time->secondsField.msb);
      msg_delay_req->originTimestamp.secondsField.lsb = htonl(time->secondsField.lsb);
      msg_delay_req->originTimestamp.nanosecondsField = htonl(time->nanosecondsField);
    }
    return sizeof(ptpv2_msg_delay_req_t);
}
int gen_ptpv2_msg_delay_resp( ptpv2_ctx_t *ctx
                           , uint8_t     *msg
                           , ptpv2_msg_hdr_t *hdr
                           , Timestamp_t     *time
                           , PortIdentity_t  *port) //used by Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
{ 
    ptpv2_msg_delay_resp_t *msg_delay_resp = (ptpv2_msg_delay_resp_t*)msg;
    if (hdr==NULL) {
        return 0;
    } else {
        memcpy((void*)msg_delay_resp, (void*)hdr, PTPV2_HDR_LEN);
    }
    //ret = populate_ptpv2_msg_hdr( ctx
    //                            , (ptpv2_msg_hdr_t*)msg
    //                            , PTPV2_MSG_Delay_Resp
    //                            , seq_id
    //                            );
    if (ctx->one_step_clock) {
      memset((void*)&(msg_delay_resp->receiveTimestamp), 0, sizeof(Timestamp_t));
    } else {
      //ret = ptp_get_time_rtc(&sec_msb, &sec_lsb, &nano);
      //if (ret) {
      //    PTPV2_ERROR("something wrong while getting ptp_get_time\n");
      //}
      msg_delay_resp->receiveTimestamp.secondsField.msb = htons(time->secondsField.msb);
      msg_delay_resp->receiveTimestamp.secondsField.lsb = htonl(time->secondsField.lsb);
      msg_delay_resp->receiveTimestamp.nanosecondsField = htonl(time->nanosecondsField);
    }
    if (port==NULL) {
        memset((void*)&(msg_delay_resp->requestingPortIdentity), 0, sizeof(PortIdentity_t));
    } else {
        memcpy((void*)&(msg_delay_resp->requestingPortIdentity), (void*)port, sizeof(PortIdentity_t));
//printf("PTPv2 oui   =+=*=0x%02X%02X%02X\n",       port->clockIdentity[0]
//                                          ,       port->clockIdentity[1]
//                                          ,       port->clockIdentity[2]);
//printf("PTPv2 uuid  =+=*=0x%02X%02X%02X%02X%02X\n", port->clockIdentity[3]
//                                                  , port->clockIdentity[4]
//                                                  , port->clockIdentity[5]
//                                                  , port->clockIdentity[6]
//                                                  , port->clockIdentity[7]);
//printf("PTPv2 portId=+=*=0x%02X\n", ntohs(port->portNumber));
    }
    return sizeof(ptpv2_msg_delay_resp_t);
}
int gen_ptpv2_msg_pdelay_req( ptpv2_ctx_t *ctx
                            , uint8_t     *msg
                            , ptpv2_msg_hdr_t *hdr
                            , Timestamp_t     *time
                            , PortIdentity_t  *port)
{ return 0; }
int gen_ptpv2_msg_pdelay_resp( ptpv2_ctx_t *ctx
                             , uint8_t     *msg
                             , ptpv2_msg_hdr_t *hdr
                             , Timestamp_t     *time
                             , PortIdentity_t  *port)
{ return 0; }
int gen_ptpv2_msg_pdelay_resp_follow_up( ptpv2_ctx_t *ctx
                                       , uint8_t     *msg
                                       , ptpv2_msg_hdr_t *hdr
                                       , Timestamp_t     *time
                                       , PortIdentity_t  *port)
{ return 0; }
int gen_ptpv2_msg_announce( ptpv2_ctx_t *ctx
                          , uint8_t     *msg
                          , ptpv2_msg_hdr_t *hdr
                          , Timestamp_t     *time
                          , PortIdentity_t  *port)
{ return 0; }
int gen_ptpv2_msg_signaling( ptpv2_ctx_t *ctx
                           , uint8_t     *msg
                           , ptpv2_msg_hdr_t *hdr
                           , Timestamp_t     *time
                           , PortIdentity_t  *port)
{ return 0; }
int gen_ptpv2_msg_management( ptpv2_ctx_t *ctx
                            , uint8_t     *msg
                            , ptpv2_msg_hdr_t *hdr
                            , Timestamp_t     *time
                            , PortIdentity_t  *port)
{ return 0; }

//-----------------------------------------------------------------------------
int gen_ptpv2_msg_ethernet ( ptpv2_ctx_t *ctx
                           , uint8_t     *msg  // PTPv2 over Ethernet message to be built
                           , uint8_t      mac_src[6]
                           , ptpv2_msg_hdr_t *hdr
                           , Timestamp_t     *time // timestamp
                           , PortIdentity_t  *port // requesting PTPv2 port for Delay_Resp, Pdelay_Resp, Pdelay_Resp_Follow_Up
                           , int          add_crc // add CRC at the end
                           , int          add_preamble // add preamble at the beginning
                           )
{
     uint16_t msg_leng;
     uint8_t  loc = (add_preamble) ? ETH_HDR_LEN+8 : ETH_HDR_LEN;
     uint8_t  mac_dst[6];
     switch (hdr->messageType) {
     case 0x0: // Event:Sync
     case 0x1: // Event:Delay_Req
     case 0x8: // General:Follow_Up
     case 0x9: // General:Delay_Resp
     case 0xB: // General:Announce
     case 0xC: // General:Signaling
     case 0xD: // General:Management
               mac_dst[0] = 0x01;
               mac_dst[1] = 0x1B;
               mac_dst[2] = 0x19;
               mac_dst[3] = 0x00;
               mac_dst[4] = 0x00;
               mac_dst[5] = 0x00; break;
     case 0x2: // Event:Pdelay_Req
     case 0x3: // Event:Pdelay_Resp
     case 0xA: // General:Pdelay_Resp_Follow_Up
               mac_dst[0] = 0x01;
               mac_dst[1] = 0x80;
               mac_dst[2] = 0xC2;
               mac_dst[3] = 0x00;
               mac_dst[4] = 0x00;
               mac_dst[5] = 0x0E; break;
     default: PTPV2_ERROR("undefined PTPv2 message type: 0x%1X", hdr->messageType);
     }
//printf("%s PTPv2 oui   =+=*=0x%02X%02X%02X\n", __FUNCTION__, port->clockIdentity[0]
//                                             ,       port->clockIdentity[1]
//                                             ,       port->clockIdentity[2]);
//printf("%s PTPv2 uuid  =+=*=0x%02X%02X%02X%02X%02X\n", __FUNCTION__, port->clockIdentity[3]
//                                                     , port->clockIdentity[4]
//                                                     , port->clockIdentity[5]
//                                                     , port->clockIdentity[6]
//                                                     , port->clockIdentity[7]);
//printf("%s PTPv2 portId=+=*=0x%02X\n", __FUNCTION__, ntohs(port->portNumber));
     msg_leng = gen_ptpv2_msg[hdr->messageType]( ctx
                                   , &msg[loc]
                                   , hdr
                                   , time
                                   , port);

     msg_leng = gen_eth_packet( msg
                              , mac_src
                              , mac_dst
                              , PTPV2_ETHERNET_TYPE_LENGTH // 0x88F7
                              , msg_leng // payload length, i.e., PTPv2 message length
                              , 0 // since already copied
                              , add_crc
                              , add_preamble);
     return msg_leng;
}

//-----------------------------------------------------------------------------
int gen_ptpv2_msg_udp_ip_ethernet( ptpv2_ctx_t *ctx
                                 , uint8_t     *msg  // PTPv2 over Ethernet message to be built
                                 , uint8_t      mac_src[6]
                                 , uint32_t     ip_src
                                 , ptpv2_msg_hdr_t *hdr
                                 , Timestamp_t     *time
                                 , PortIdentity_t  *port
                                 , int          add_crc // add CRC at the end
                                 , int          add_preamble // add preamble at the beginning
                                 )
{
     uint16_t msg_leng;
     uint8_t  loc;
     uint8_t  mac_dst[6];
     uint16_t port_src, port_dst;
     uint32_t ip_dst;
     switch (hdr->messageType) {
     case 0x0: // Event:Sync
     case 0x1: // Event:Delay_Req
     case 0x2: // Event:Pdelay_Req
     case 0x3: // Event:Pdelay_Resp
               port_src   = 319;
               port_dst   = 319;
               ip_dst     = 0xE0000181;
               mac_dst[0] = 0x01;
               mac_dst[1] = 0x00;
               mac_dst[2] = 0x5E;
               mac_dst[3] = 0x00;
               mac_dst[4] = 0x01;
               mac_dst[5] = 0x81; break;
     case 0x8: // General:Follow_Up
     case 0x9: // General:Delay_Resp
     case 0xA: // General:Pdelay_Resp_Follow_Up
     case 0xB: // General:Announce
     case 0xC: // General:Signaling
     case 0xD: // General:Management
               port_src   = 320;
               port_dst   = 320;
               ip_dst     = 0xE000006B;
               mac_dst[0] = 0x01;
               mac_dst[1] = 0x00;
               mac_dst[2] = 0x5E;
               mac_dst[3] = 0x00;
               mac_dst[4] = 0x00;
               mac_dst[5] = 0x6B; break;
     default: PTPV2_ERROR("undefined PTPv2 message type: 0x%1X", hdr->messageType);
     }
     loc = ETH_HDR_LEN+IP_HDR_LEN+UDP_HDR_LEN;
     if (add_preamble) loc += 8;
     msg_leng = (gen_ptpv2_msg[hdr->messageType])(ctx, &msg[loc], hdr, time, port);
     loc -= UDP_HDR_LEN;
     msg_leng = gen_udp_packet( &msg[loc]
                              , port_src // host order
                              , port_dst // host order
                              , msg_leng // UDP payload length
                              , 0); // since already copied
     loc -= IP_HDR_LEN;
     msg_leng = gen_ip_packet( &msg[loc]
                             , ip_src // host order
                             , ip_dst // host order
                             , IP_PROTO_UDP // 0x11
                             , 0 // ttl      // type-length in host order
                             , msg_leng
                             , 0   // since already copied
                             , 0); // since it is UDP, not TCP
     loc -= ETH_HDR_LEN;
     if (add_preamble) loc -= 8;
     msg_leng = gen_eth_packet( &msg[loc]
                              , mac_src
                              , mac_dst
                              , ETH_TYPE_IP // 0x0800
                              , msg_leng
                              , 0 // since already copied
                              , add_crc
                              , add_preamble);
     return msg_leng;
}

//-----------------------------------------------------------------------------
static ptpv2_ctx_t ptpv2_ctx = {
       2, // ptpv2_ctx.ptp_version   
       0, // ptpv2_ctx.ptp_domain    
       0, // ptpv2_ctx.one_step_clock (0=Two Step Clock, which means Follow_Up is required)
       0, // ptpv2_ctx.unicast_port  
       0, // ptpv2_ctx.profile_spec1 
       0  // ptpv2_ctx.profile_spec2 
       };

//-----------------------------------------------------------------------------
ptpv2_ctx_t *gen_ptpv2_context( uint32_t ptp_version   
                              , uint32_t ptp_domain    
                              , uint32_t one_step_clock
                              , uint32_t unicast_port  
                              , uint32_t profile_spec1 
                              , uint32_t profile_spec2 
                              )
{
    ptpv2_ctx.ptp_version    = ptp_version   ;
    ptpv2_ctx.ptp_domain     = ptp_domain    ;
    ptpv2_ctx.one_step_clock = one_step_clock;
    ptpv2_ctx.unicast_port   = unicast_port  ;
    ptpv2_ctx.profile_spec1  = profile_spec1 ;
    ptpv2_ctx.profile_spec2  = profile_spec2 ;
    return &ptpv2_ctx;
}

//-----------------------------------------------------------------------------
ptpv2_ctx_t *get_ptpv2_context( ) {
    return &ptpv2_ctx;
}

//-----------------------------------------------------------------------------
int parser_ptpv2_message(uint8_t *pkt, int leng)
{
    ptpv2_msg_hdr_t *hdr = (ptpv2_msg_hdr_t*)pkt;
    printf("PTPv2 transportSpecific  0x%01X\n",       hdr->transportSpecific );
    printf("PTPv2 messageType        0x%01X (%s)\n",  hdr->messageType       , ptpv2_msg[hdr->messageType]);
    printf("PTPv2 reserved0          0x%01X\n",       hdr->reserved0         );
    printf("PTPv2 versionPTP         0x%01X\n",       hdr->versionPTP        );
    printf("PTPv2 messageLength      0x%04X\n", ntohs(hdr->messageLength     ));
    printf("PTPv2 domainNumber       0x%02X\n",       hdr->domainNumber      );
    printf("PTPv2 reserved1          0x%02X\n",       hdr->reserved1         );
    printf("PTPv2 flagField          0x%04X\n", ntohs(hdr->flagField         ));
    printf("PTPv2 correctionField    0x%04X\n", ntohl(hdr->correctionField.low ));
    printf("PTPv2 correctionField    0x%04X\n", ntohl(hdr->correctionField.high));
    printf("PTPv2 reserved2          0x%08X\n", ntohl(hdr->reserved2         ));
  //printf("PTPv2 oui                0x%02X%02X%02X\n",       hdr->sourcePortIdentity.clockIdentity[0]
  //                                                  ,       hdr->sourcePortIdentity.clockIdentity[1]
  //                                                  ,       hdr->sourcePortIdentity.clockIdentity[2]);
  //printf("PTPv2 uuid               0x%02X%02X%02X%02X%02X\n",       hdr->sourcePortIdentity.clockIdentity[3]
  //                                                          ,       hdr->sourcePortIdentity.clockIdentity[4]
  //                                                          ,       hdr->sourcePortIdentity.clockIdentity[5]
  //                                                          ,       hdr->sourcePortIdentity.clockIdentity[6]
  //                                                          ,       hdr->sourcePortIdentity.clockIdentity[7]);
    printf("PTPv2 clockId            0x%02X%02X%02X%02X%02X%02X%02X%02X\n"
                                              , hdr->sourcePortIdentity.clockIdentity[0]
                                              , hdr->sourcePortIdentity.clockIdentity[1]
                                              , hdr->sourcePortIdentity.clockIdentity[2]
                                              , hdr->sourcePortIdentity.clockIdentity[3]
                                              , hdr->sourcePortIdentity.clockIdentity[4]
                                              , hdr->sourcePortIdentity.clockIdentity[5]
                                              , hdr->sourcePortIdentity.clockIdentity[6]
                                              , hdr->sourcePortIdentity.clockIdentity[7]);
    printf("PTPv2 portId             0x%04X\n", ntohs(hdr->sourcePortIdentity.portNumber));
    printf("PTPv2 sequenceID         0x%04X\n", ntohs(hdr->sequenceID        ));
    printf("PTPv2 controlField       0x%02X\n", hdr->controlField      );
    printf("PTPv2 logMessageInterval 0x%02X\n", hdr->logMessageInterval);

    uint8_t *tpt = (uint8_t*)(pkt+PTPV2_HDR_LEN); // timestamp
    uint8_t *ppt = (uint8_t*)(pkt+PTPV2_HDR_LEN+sizeof(Timestamp_t)); // requesting portId
    switch (hdr->messageType) {
    case PTPV2_MSG_Sync                 ://  0x0
    case PTPV2_MSG_Follow_Up            ://  0x8
    case PTPV2_MSG_Delay_Req            ://  0x1
    case PTPV2_MSG_Pdelay_Req           ://  0x2
         // haeder(34)+timeStamp(10)
         printf("PTPv2 second             0x%02X%02X%02X%02X%02X%02X\n",tpt[0],tpt[1],tpt[2],tpt[3],tpt[4],tpt[5]);
         printf("PTPv2 nanosecond         0x%02X%02X%02X%02X\n",tpt[6],tpt[7],tpt[8],tpt[9]);
         break;
    case PTPV2_MSG_Delay_Resp           ://  0x9
    case PTPV2_MSG_Pdelay_Resp          ://  0x3
    case PTPV2_MSG_Pdelay_Resp_Follow_Up://  0xA
         // haeder(34)+timeStamp(10)+ReqPort(10)
         printf("PTPv2 second             0x%02X%02X%02X%02X%02X%02X\n",tpt[0],tpt[1],tpt[2],tpt[3],tpt[4],tpt[5]);
         printf("PTPv2 nanosecond         0x%02X%02X%02X%02X\n",tpt[6],tpt[7],tpt[8],tpt[9]);
         printf("PTPv2 clockId            0x%02X%02X%02X%02X%02X%02X%02X%02X\n",ppt[0],ppt[1],ppt[2],ppt[3],ppt[4],ppt[5],ppt[6],ppt[7]);
         printf("PTPv2 portId             0x%02X%02X\n",ppt[8],ppt[9]);
         break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
