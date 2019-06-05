//----------------------------------------------------------------------------
// Copyright (c) 2019 by Ando Ki.
// All rights are reserved by Ando Ki.
//----------------------------------------------------------------------------
// network_vpi_lib.c
//----------------------------------------------------------------------------
// VERSION = 2019.05.20.
//----------------------------------------------------------------------------
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
#else
   #if defined(WIN32)  // cygwin use this
   #       include <windows.h>
   #       include <io.h>
   #       include <process.h>
   #else
   #       include <unistd.h>
   #endif
#endif
#include "vpi_user.h"
#include "eth_ip_udp_tcp_pkt.h"
#include "ptpv2_message.h"

//----------------------------------------------------------------------------
static int m_verbose = 0;

//----------------------------------------------------------------------------
void pkt_control(PLI_INT32 operation) {
#if defined(ncsim)||defined(verilogXL)
     switch (operation) {
     case vpiStop: tf_dostop(); break;
     case vpiFinish: tf_dofinish(); break;
     default: vpi_printf("ERROR: %d for vpi_control() is not supported yet!\n",
                          operation);
     }
#else
     vpi_control(operation, 0);
#endif
}

//----------------------------------------------------------------------------
// Build Ethernet packet using payload in the 'payload[]' array.
// $pkt_ethernet( pkt     [ 7:0][0:1024*4-1]
//              , bnum_pkt[15:0] // num of bytes of the whole packet
//              , mac_src [47:0]
//              , mac_dst [47:0]
//              , type_len[15:0]
//              , bnum_payload[15:0] // num of bytes of payload
//              , payload     [ 7:0][0:1024*4-1]
//              , add_crc      //
//              , add_preamble //
//              );
// Note that 'bnum_pkt' depends on 'bnum_payload', 'add_crc', 'add_preamble'.
PLI_INT32 pkt_eth_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 pkt_eth_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build IP packet using payload in the 'payload[]' array.
// $pkt_ip( pkt     [ 7:0][0:1024*4-1]
//        , bnum_pkt[15:0] // num of bytes of the whole packet
//        , ip_src  [31:0]
//        , ip_dst  [31:0]
//        , protocol[ 7:0]
//        , ttl     [ 7:0]
//        , bnum_payload[15:0] // num of bytes of payload
//        , payload     [ 7:0][0:1024*4-1]
//        , tcp_check // update TCP checksum when 1
//        );
PLI_INT32 pkt_ip_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 pkt_ip_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build UDP packet using payload in the 'payload[]' array.
// $pkt_udp( pkt     [ 7:0][0:1024*4-1]
//         , bnum_pkt[15:0] // num of bytes of the whole packet
//         , port_src[15:0]
//         , port_dst[15:0]
//         , bnum_payload[15:0] // num of bytes of payload
//         , payload     [ 7:0][0:1024*4-1]
//         );
PLI_INT32 pkt_udp_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 pkt_udp_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build TCP packet using payload in the 'payload[]' array.
// $pkt_tcp( pkt     [ 7:0][0:1024*4-1]
//         , bnum_pkt[15:0] // num of bytes of the whole packet
//         , port_src[15:0]
//         , port_dst[15:0]
//         , seq_num [31:0]
//         , ack_num [31:0]
//         , bnum_payload[15:0] // num of bytes of payload
//         , payload     [ 7:0][0:1024*4-1]
//         );
PLI_INT32 pkt_tcp_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 pkt_tcp_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build UDP/IP/Ethernet packet using payload in the 'payload[]' array,
// where 'payload[]' contains UDP payload.
PLI_INT32 pkt_udp_ip_eth_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 pkt_udp_ip_eth_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Initialize PTPv2 context
// $msg_ptpv2_set_context( ptp_version 
//                       , ptp_domain
//                       , one_step_clock
//                       , unicast_port
//                       , profile_spec1
//                       , profile_spec2
//                       );
PLI_INT32 msg_ptpv2_set_context_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 msg_ptpv2_set_context_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Read PTPv2 context
// $msg_ptpv2_get_context( ptp_version 
//                       , ptp_domain
//                       , one_step_clock
//                       , unicast_port
//                       , profile_spec1
//                       , profile_spec2
//                       );
PLI_INT32 msg_ptpv2_get_context_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 msg_ptpv2_get_context_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build PTPv2 message
// $msg_ptpv2( pkt             [ 7:0][0:1024*4-1]
//           , bnum_pkt        [15:0] // num of bytes of the whole message
//           , messageType     [ 3:0]
//           , flagField       [15:0]
//           , correctionField [63:0]
//           , sourceClockID   [63:0]
//           , sourcePortID    [15:0]
//           , sequenceID      [15:0]
//           , secondsField    [47:0]
//           , nanosecondsField[31:0]
//           );
PLI_INT32 msg_ptpv2_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 msg_ptpv2_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build PTPv2 message over Ethernet
// $msg_ptpv2_ethernet ( pkt               [ 7:0][0:1024*4-1]
//                     , bnum_pkt          [15:0] // num of bytes of the whole message
//                     , mac_src           [47:0]
//                     , messageType       [ 3:0]
//                     , flagField         [15:0]
//                     , correctionField   [63:0]
//                     , sourceClockID     [63:0]
//                     , sourcePortID      [15:0]
//                     , sequenceID        [15:0]
//                     , secondsField      [47:0]
//                     , nanosecondsField  [31:0]
//                     , reqClockID        [63:0]
//                     , reqPortID         [15:0]
//                     , add_crc
//                     , add_preamble
//                     );
PLI_INT32 msg_ptpv2_eth_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 msg_ptpv2_eth_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// Build PTPv2 message over UDP/IP/Ethernet
// $msg_ptpv2_udp_ip_ethernet ( pkt               [7:0][0:1024*4-1]
//                            , bnum_pkt          [15:0] // num of bytes of the whole message
//                            , mac_src           [47:0]
//                            , ip_src            [31:0]
//                            , messageType       [ 3:0]
//                            , flagField         [15:0]
//                            , correctionField   [63:0]
//                            , sourceClockID     [63:0]
//                            , sourcePortID      [15:0]
//                            , sequenceID        [15:0]
//                            , secondsField      [47:0]
//                            , nanosecondsField  [31:0]
//                            , reqClockID        [63:0]
//                            , reqPortID         [15:0]
//                            , add_crc      //
//                            , add_preamble //
//                            );
PLI_INT32 msg_ptpv2_udp_ip_eth_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 msg_ptpv2_udp_ip_eth_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// $pkt_ethernet_parser( pkt     [ 7:0][0:1024*4-1]
//                     , bnum_pkt[15:0]
//                     , crc   
//                     , preamble
//                     );
PLI_INT32 pkt_eth_parser_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32 pkt_eth_parser_Calltf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
// $pkt_eth_verbose; ==> $pkt_verbose(0);
// $pkt_eth_verbose(); ==> $pkt_verbose(0);
// $pkt_eth_verbose(n);
PLI_INT32   pkt_eth_verbose_Calltf   (PLI_BYTE8 *user_data);
PLI_INT32   pkt_eth_verbose_Compiletf(PLI_BYTE8 *user_data);
PLI_INT32   pkt_eth_verbose_Sizetf   (PLI_BYTE8 *user_data);

//----------------------------------------------------------------------------
void pkt_register() {
    s_vpi_systf_data tf_data;

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$pkt_ethernet";
    tf_data.calltf      = pkt_eth_Calltf;
    tf_data.compiletf   = pkt_eth_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$pkt_ip";
    tf_data.calltf      = pkt_ip_Calltf;
    tf_data.compiletf   = pkt_ip_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$pkt_udp";
    tf_data.calltf      = pkt_udp_Calltf;
    tf_data.compiletf   = pkt_udp_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$pkt_tcp";
    tf_data.calltf      = pkt_tcp_Calltf;
    tf_data.compiletf   = pkt_tcp_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$pkt_udp_ip_ethernet";
    tf_data.calltf      = pkt_udp_ip_eth_Calltf;
    tf_data.compiletf   = pkt_udp_ip_eth_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$msg_ptpv2_set_context";
    tf_data.calltf      = msg_ptpv2_set_context_Calltf;
    tf_data.compiletf   = msg_ptpv2_set_context_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$msg_ptpv2_get_context";
    tf_data.calltf      = msg_ptpv2_get_context_Calltf;
    tf_data.compiletf   = msg_ptpv2_get_context_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$msg_ptpv2";
    tf_data.calltf      = msg_ptpv2_Calltf;
    tf_data.compiletf   = msg_ptpv2_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$msg_ptpv2_ethernet";
    tf_data.calltf      = msg_ptpv2_eth_Calltf;
    tf_data.compiletf   = msg_ptpv2_eth_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$msg_ptpv2_udp_ip_ethernet";
    tf_data.calltf      = msg_ptpv2_udp_ip_eth_Calltf;
    tf_data.compiletf   = msg_ptpv2_udp_ip_eth_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysTask;
    tf_data.sysfunctype = 0;
    tf_data.tfname      = "$pkt_ethernet_parser";
    tf_data.calltf      = pkt_eth_parser_Calltf;
    tf_data.compiletf   = pkt_eth_parser_Compiletf;
    tf_data.sizetf      = NULL;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);

    tf_data.type        = vpiSysFunc;
    tf_data.sysfunctype = vpiSizedFunc; //vpiSysFuncSized;
    tf_data.tfname      = "$pkt_eth_verbose";
    tf_data.calltf      = pkt_eth_verbose_Calltf;
    tf_data.compiletf   = pkt_eth_verbose_Compiletf;
    tf_data.sizetf      = pkt_eth_verbose_Sizetf;
    tf_data.user_data   = NULL;
    vpi_register_systf(&tf_data);
}

//----------------------------------------------------------------------------
#define CHECK_ARRAY_ARG(A,B,C,D)\
        arg_handle = vpi_scan(arg_iterator);\
        if (arg_handle==NULL) {\
            vpi_printf("ERROR: %s must have %s argument.\n", TASK_NAME, (B));\
            vpi_free_object(arg_iterator);\
            pkt_control(vpiFinish);\
        } else  {\
          if (vpi_get(vpiArray, arg_handle)) {\
              (C) = vpi_get(vpiSize, arg_handle);\
              ele_handle = vpi_handle_by_index(arg_handle, 0);\
              (D) = vpi_get(vpiSize, ele_handle);\
          } else {\
              vpi_printf("ERROR: %s %s argument must be array\n", TASK_NAME, (A));\
              vpi_free_object(arg_iterator);\
              pkt_control(vpiFinish);\
          }\
        }
#define CHECK_WIDE_ARG(A,B,C)\
        arg_handle = vpi_scan(arg_iterator);\
        if (arg_handle==NULL) {\
            vpi_printf("ERROR: %s must have %s arguments.\n", TASK_NAME, (B));\
            vpi_free_object(arg_iterator);\
            pkt_control(vpiFinish);\
        } else {\
           width = vpi_get(vpiSize, arg_handle);\
           if (width<(C)) {\
               vpi_printf("ERROR: %s %s argument must be %d-bit\n", TASK_NAME, (A), (C));\
               vpi_free_object(arg_iterator);\
               pkt_control(vpiFinish);\
           }\
        }
#define CHECK_INT_ARG(A,B)\
        arg_handle = vpi_scan(arg_iterator);\
        if (arg_handle==NULL) {\
            vpi_printf("ERROR: %s must have %s arguments.\n", TASK_NAME, (B));\
            vpi_free_object(arg_iterator);\
            pkt_control(vpiFinish);\
        } else {\
           arg_type = vpi_get(vpiType, arg_handle);\
           if ((arg_type!=vpiReg)&&(arg_type!=vpiIntegerVar)&&\
               (arg_type!=vpiConstant)&&(arg_type!=vpiNet)&&\
               (arg_type!=vpiParameter)&&(arg_type!=vpiSpecParam)) {\
               vpi_printf("ERROR: %s %s argument must be integer, but %d\n", TASK_NAME, (A), arg_type);\
               vpi_free_object(arg_iterator);\
               pkt_control(vpiFinish);\
           }\
        }

//----------------------------------------------------------------------------
// $pkt_ethernet( pkt     [ 7:0][0:1024*4-1]
//              , bnum_pkt[15:0]
//              , mac_dst [47:0]        // [47:40]=msb
//              , mac_src [47:0]
//              , type_len[15:0]
//              , bnum_payload[15:0]
//              , payload     [ 7:0][0:1024*4-1]
//              , add_crc      //
//              , add_preamble //
//              );
//----------------------------------------------------------------------------
#define TASK_NAME "$pkt_ethernet"
PLI_INT32 pkt_eth_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;
  int numB, widthB;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have nine arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "nine", numA, widthA) // ethernet pkt
  CHECK_WIDE_ARG ("2nd", "nine", 16)  // bnum pkt
  CHECK_WIDE_ARG ("3rd", "nine", 48)  // SRC MAC
  CHECK_WIDE_ARG ("4th", "nine", 48)  // DST MAC
  CHECK_WIDE_ARG ("5th", "nine", 16)  // TYPE_LENG
  CHECK_WIDE_ARG ("6th", "nine", 16)  // bnum payload
  CHECK_ARRAY_ARG("7th", "nine", numB, widthB) // payload
  CHECK_INT_ARG  ("8th", "nine") // add crc
  CHECK_INT_ARG  ("9th", "nine") // add preamble

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have nine arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (widthB!=8) {
      vpi_printf("ERROR: %s seventh argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (numA<numB) {
      vpi_printf("ERROR: %s first argument must be largeer than sixth argument.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME

//----------------------------------------------------------------------------
#define GET_WIDE_ARG(A)\
        value.format = vpiVectorVal;\
        vpi_get_value((A), &value);
#define GET_INT_ARG(A,B,C)\
        value.format = vpiIntVal;\
        vpi_get_value((A), &value);\
        (C) = (B)value.value.integer;
#define GET_ARRAY_ARG(A,B,C,D)\
        value.format = vpiIntVal;\
        for (idy=(B), idz=0; idy<(C); idy++, idz++) {\
             vpiHandle ele = vpi_handle_by_index((A), idy);\
             vpi_get_value(ele, &value);\
             (D)[idz] = value.value.integer;\
        }
//----------------------------------------------------------------------------
#define PUT_INT_ARG(h,t,v)\
        value.format = vpiIntVal;\
        value.value.integer = (v);\
        vpi_put_value((h), &value, NULL, vpiNoDelay);

//----------------------------------------------------------------------------
PLI_INT32 pkt_eth_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt     ;
  vpiHandle H_bnum_pkt;
  vpiHandle H_mac_dst ;
  vpiHandle H_mac_src ;
  vpiHandle H_type_len;
  vpiHandle H_bnum_payload;
  vpiHandle H_payload ;
  vpiHandle H_crc     ;
  vpiHandle H_preamble;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 bnum_pkt;
  PLI_UBYTE8 mac_dst[6];
  PLI_UBYTE8 mac_src[6];
  PLI_UINT16 type_len;
  PLI_UINT16 bnum_payload;
  PLI_UINT32 add_crc;
  PLI_UINT32 add_preamble;
  int idx, idy, idz;
  uint8_t *eth_pkt; // buffer to hold whole Ethenet packet including pre & payload & crc
  uint8_t *payload; // buffer to hold payload data
  uint32_t crc;
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_mac_src      = vpi_scan(arg_iterator);
  H_mac_dst      = vpi_scan(arg_iterator);
  H_type_len     = vpi_scan(arg_iterator);
  H_bnum_payload = vpi_scan(arg_iterator);
  H_payload      = vpi_scan(arg_iterator);
  H_crc          = vpi_scan(arg_iterator);
  H_preamble     = vpi_scan(arg_iterator);

  //--------------------MAC DST
  GET_WIDE_ARG(H_mac_dst)
  val32 = value.value.vector[0].aval;
  mac_dst[5] =  val32     &0xFF;
  mac_dst[4] = (val32>> 8)&0xFF;
  mac_dst[3] = (val32>>16)&0xFF;
  mac_dst[2] = (val32>>24)&0xFF;
  val32 = value.value.vector[1].aval;
  mac_dst[1] =  val32     &0xFF;
  mac_dst[0] = (val32>> 8)&0xFF; // msb

  //--------------------MAC SRC
  GET_WIDE_ARG(H_mac_src)
  val32 = value.value.vector[0].aval;
  mac_src[5] =  val32     &0xFF;
  mac_src[4] = (val32>> 8)&0xFF;
  mac_src[3] = (val32>>16)&0xFF;
  mac_src[2] = (val32>>24)&0xFF;
  val32 = value.value.vector[1].aval;
  mac_src[1] =  val32     &0xFF;
  mac_src[0] = (val32>> 8)&0xFF; // msb

  //--------------------TYPE LENGTH
  GET_INT_ARG(H_type_len,PLI_UINT16,type_len)

  //--------------------num of bytes of payload
  GET_INT_ARG(H_bnum_payload,PLI_UINT16,bnum_payload)

  //--------------------crc
  GET_INT_ARG(H_crc,PLI_UINT32,add_crc)

  //--------------------preamble
  GET_INT_ARG(H_preamble,PLI_UINT32,add_preamble)

  //-------------------- use num of payload when type_len is 0
  if (type_len==0) {
      type_len = bnum_payload;
  }

  //--------------------build Ethernet packet
  tmp = (add_preamble) ? 8 : 0;
  tmp += ETH_HDR_LEN;
  tmp += (bnum_payload<46) ? 46 : bnum_payload;
  tmp += (add_crc) ? 4 : 0; // num of bytes from preamble (if any) to crc (if any).
  eth_pkt = (uint8_t*)calloc(tmp, 1);
  if (eth_pkt==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  payload = (uint8_t*)calloc(bnum_payload, 1);
  if (payload==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  GET_ARRAY_ARG(H_payload,0,bnum_payload,payload)

  tmp = gen_eth_packet( eth_pkt
                      , mac_src
                      , mac_dst
                      , type_len
                      , bnum_payload
                      , payload
                      , add_crc
                      , add_preamble
                      );

#if defined(RIGOR)
  int xxy = (add_preamble) ? 8 : 0;
  xxy += ETH_HDR_LEN;
  if (add_crc) {
      xxy += (bnum_payload<46) ? 46 : bnum_payload;
      xxy += (add_crc) ? 4 : 0;
  } else {
      xxy += bnum_payload;
  }
  if (tmp!=xxy) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, xxy);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = eth_pkt[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(eth_pkt);
  free(payload);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $pkt_ip( pkt     [ 7:0][0:1024*4-1]
//        , bnum_pkt[15:0] // num of bytes of the whole packet
//        , ip_src  [31:0]
//        , ip_dst  [31:0]
//        , protocol[ 7:0]
//        , ttl     [ 7:0]
//        , bnum_payload[15:0] // num of bytes of payload
//        , payload     [ 7:0][0:1024*4-1]
//        , tcp_check // update TCP checksum when 1
//        );
//----------------------------------------------------------------------------
#define TASK_NAME "$pkt_ip"
PLI_INT32 pkt_ip_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;
  int numB, widthB;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have eight arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1", "nine", numA, widthA) // ethernet pkt
  CHECK_INT_ARG  ("2", "nine") // bnum pkt
  CHECK_INT_ARG  ("3", "nine") // SRC IP
  CHECK_INT_ARG  ("4", "nine") // DST IP
  CHECK_INT_ARG  ("5", "nine") // Protocol
  CHECK_INT_ARG  ("6", "nine") // TTL
  CHECK_INT_ARG  ("7", "nine") // bnum payload
  CHECK_ARRAY_ARG("8", "nine", numB, widthB) // PAYLOAD
  CHECK_INT_ARG  ("9", "nine") // tcp_checksum

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have nine arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (widthB!=8) {
      vpi_printf("ERROR: %s seventh argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (numA<numB) {
      vpi_printf("ERROR: %s first argument must be largeer than seventh argument.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 pkt_ip_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt      ;
  vpiHandle H_bnum_pkt ;
  vpiHandle H_ip_dst   ;
  vpiHandle H_ip_src   ;
  vpiHandle H_protocol ;
  vpiHandle H_ttl      ;
  vpiHandle H_bnum_payload;
  vpiHandle H_payload  ;
  vpiHandle H_tcp_check;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 bnum_pkt;
  PLI_UINT32 ip_dst  ;
  PLI_UINT32 ip_src  ;
  PLI_UBYTE8 protocol;
  PLI_UBYTE8 ttl     ;
  PLI_UINT16 bnum_payload;
  PLI_UINT32 tcp_check;
  int idx, idy, idz;
  uint8_t *ip_pkt; // buffer to hold whole IP packet
  uint8_t *payload; // buffer to hold payload data
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_ip_src       = vpi_scan(arg_iterator);
  H_ip_dst       = vpi_scan(arg_iterator);
  H_protocol     = vpi_scan(arg_iterator);
  H_ttl          = vpi_scan(arg_iterator);
  H_bnum_payload = vpi_scan(arg_iterator);
  H_payload      = vpi_scan(arg_iterator);
  H_tcp_check    = vpi_scan(arg_iterator);

  GET_INT_ARG(H_ip_src  ,PLI_UINT32,ip_src)
  GET_INT_ARG(H_ip_dst  ,PLI_UINT32,ip_dst)
  GET_INT_ARG(H_protocol,PLI_UBYTE8,protocol)
  GET_INT_ARG(H_ttl     ,PLI_UBYTE8,ttl)
  GET_INT_ARG(H_bnum_payload,PLI_UINT16,bnum_payload)
  GET_INT_ARG(H_tcp_check,PLI_UINT32,tcp_check)

  //--------------------build Ethernet packet
  tmp = IP_HDR_LEN + bnum_payload;
  ip_pkt = (uint8_t*)calloc(tmp, 1);
  if (ip_pkt==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  payload = (uint8_t*)calloc(bnum_payload, 1);
  if (payload==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  GET_ARRAY_ARG(H_payload,0,bnum_payload,payload)

  tmp = gen_ip_packet( ip_pkt
                     , ip_src
                     , ip_dst
                     , protocol
                     , ttl
                     , bnum_payload
                     , payload
                     , tcp_check
                     );

#if defined(RIGOR)
  int xxy = IP_HDR_LEN + bnum_payload;
  if (tmp!=xxy) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, xxy);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = ip_pkt[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(ip_pkt);
  free(payload);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $pkt_udp( pkt     [ 7:0][0:1024*4-1]
//         , bnum_pkt[15:0] // num of bytes of the whole packet
//         , port_src[15:0]
//         , port_dst[15:0]
//         , bnum_payload[15:0] // num of bytes of payload
//         , payload     [ 7:0][0:1024*4-1]
//         );
//----------------------------------------------------------------------------
#define TASK_NAME "$pkt_udp"
PLI_INT32 pkt_udp_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;
  int numB, widthB;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "six", numA, widthA) // ethernet pkt
  CHECK_INT_ARG  ("2nd", "six") // bnum pkt
  CHECK_INT_ARG  ("3rd", "six") // SRC port
  CHECK_INT_ARG  ("4th", "six") // DST port
  CHECK_INT_ARG  ("5th", "six") // bnum payload
  CHECK_ARRAY_ARG("6th", "six", numB, widthB) // PAYLOAD

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (widthB!=8) {
      vpi_printf("ERROR: %s seventh argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (numA<numB) {
      vpi_printf("ERROR: %s first argument must be largeer than seventh argument.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 pkt_udp_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt     ;
  vpiHandle H_bnum_pkt;
  vpiHandle H_port_dst;
  vpiHandle H_port_src;
  vpiHandle H_bnum_payload;
  vpiHandle H_payload ;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 bnum_pkt;
  PLI_UINT16 port_dst  ;
  PLI_UINT16 port_src  ;
  PLI_UINT16 bnum_payload;
  int idx, idy, idz;
  uint8_t *udp_pkt; // buffer to hold whole UDP packet
  uint8_t *payload; // buffer to hold payload data
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_port_src     = vpi_scan(arg_iterator);
  H_port_dst     = vpi_scan(arg_iterator);
  H_bnum_payload = vpi_scan(arg_iterator);
  H_payload      = vpi_scan(arg_iterator);

  GET_INT_ARG(H_port_src,PLI_UINT16,port_src)
  GET_INT_ARG(H_port_dst,PLI_UINT16,port_dst)
  GET_INT_ARG(H_bnum_payload,PLI_UINT16,bnum_payload)

  //--------------------build Ethernet packet
  tmp = UDP_HDR_LEN + bnum_payload;
  udp_pkt = (uint8_t*)calloc(tmp, 1);
  if (udp_pkt==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  payload = (uint8_t*)calloc(bnum_payload, 1);
  if (payload==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  GET_ARRAY_ARG(H_payload,0,bnum_payload,payload)

  tmp = gen_udp_packet( udp_pkt
                      , port_src
                      , port_dst
                      , bnum_payload
                      , payload
                      );

#if defined(RIGOR)
  int xxy = UDP_HDR_LEN + bnum_payload;
  if (tmp!=xxy) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, xxy);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = udp_pkt[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(udp_pkt);
  free(payload);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $pkt_tcp( pkt     [ 7:0][0:1024*4-1]
//         , bnum_pkt[15:0] // num of bytes of the whole packet
//         , port_src[15:0]
//         , port_dst[15:0]
//         , seq_num [31:0]
//         , ack_num [31:0]
//         , bnum_payload[15:0] // num of bytes of payload
//         , payload     [ 7:0][0:1024*4-1]
//         );
//----------------------------------------------------------------------------
#define TASK_NAME "$pkt_tcp"
PLI_INT32 pkt_tcp_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;
  int numB, widthB;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "eight", numA, widthA) // ethernet pkt
  CHECK_INT_ARG  ("2nd", "eight") // bnum pkt
  CHECK_INT_ARG  ("3rd", "eight") // SRC port
  CHECK_INT_ARG  ("4th", "eight") // DST port
  CHECK_INT_ARG  ("5th", "eight") // seq num
  CHECK_INT_ARG  ("6th", "eight") // ack num
  CHECK_INT_ARG  ("7th", "eight") // bnum payload
  CHECK_ARRAY_ARG("8th", "eight", numB, widthB) // PAYLOAD

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have eight arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (widthB!=8) {
      vpi_printf("ERROR: %s seventh argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (numA<numB) {
      vpi_printf("ERROR: %s first argument must be largeer than seventh argument.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 pkt_tcp_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt     ;
  vpiHandle H_bnum_pkt;
  vpiHandle H_port_dst;
  vpiHandle H_port_src;
  vpiHandle H_num_seq;
  vpiHandle H_num_ack;
  vpiHandle H_bnum_payload;
  vpiHandle H_payload ;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 bnum_pkt;
  PLI_UINT16 port_dst  ;
  PLI_UINT16 port_src  ;
  PLI_UINT32 num_seq;
  PLI_UINT32 num_ack;
  PLI_UINT16 bnum_payload;
  int idx, idy, idz;
  uint8_t *tcp_pkt; // buffer to hold whole UDP packet
  uint8_t *payload; // buffer to hold payload data
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_port_src     = vpi_scan(arg_iterator);
  H_port_dst     = vpi_scan(arg_iterator);
  H_num_seq      = vpi_scan(arg_iterator);
  H_num_ack      = vpi_scan(arg_iterator);
  H_bnum_payload = vpi_scan(arg_iterator);
  H_payload      = vpi_scan(arg_iterator);

  GET_INT_ARG(H_port_src,PLI_UINT16,port_src)
  GET_INT_ARG(H_port_dst,PLI_UINT16,port_dst)
  GET_INT_ARG(H_num_seq,PLI_UINT32,num_seq)
  GET_INT_ARG(H_num_ack,PLI_UINT32,num_ack)
  GET_INT_ARG(H_bnum_payload,PLI_UINT16,bnum_payload)

  //--------------------build Ethernet packet
  tmp = TCP_HDR_LEN + bnum_payload;
  tcp_pkt = (uint8_t*)calloc(tmp, 1);
  if (tcp_pkt==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  payload = (uint8_t*)calloc(bnum_payload, 1);
  if (payload==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  GET_ARRAY_ARG(H_payload,0,bnum_payload,payload)

  tmp = gen_tcp_packet( tcp_pkt
                      , port_src
                      , port_dst
                      , num_seq
                      , num_ack
                      , bnum_payload
                      , payload
                      );

#if defined(RIGOR)
  int xxy = UDP_HDR_LEN + bnum_payload;
  if (tmp!=xxy) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, xxy);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = tcp_pkt[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(tcp_pkt);
  free(payload);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $pkt_udp_ip_ethernet( pkt     [7:0][0:4095]
//                     , bnum_pkt[15:0] // num of bytes of the whole packet
//                     , port_src[15:0]
//                     , port_dst[15:0]
//                     , ip_src  [31:0]
//                     , ip_dst  [31:0]
//                     , ttl     [ 7:0]
//                     , mac_src [47:0]
//                     , mac_dst [47:0]
//                     , bnum_payload[15:0] // num of bytes of payload
//                     , payload [7:0][0:4095]
//                     , add_crc      //
//                     , add_preamble //
//                     );
//----------------------------------------------------------------------------
#define TASK_NAME "$pkt_udp_ip_eth"
PLI_INT32 pkt_udp_ip_eth_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;
  int numB, widthB;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "13-th", numA, widthA) // ethernet pkt
  CHECK_INT_ARG  ("2nd", "13-th"              ) // bnum pkt
  CHECK_INT_ARG  ("3rd", "13-th"              ) // SRC port
  CHECK_INT_ARG  ("4th", "13-th"              ) // DST port
  CHECK_INT_ARG  ("5th", "13-th"              ) // SRC IP
  CHECK_INT_ARG  ("6th", "13-th"              ) // DST IP
  CHECK_INT_ARG  ("7th", "13-th"              ) // TTL
  CHECK_WIDE_ARG ("8th", "13-th", 48          ) // SRC MAC
  CHECK_WIDE_ARG ("9th", "13-th", 48          ) // DST MAC
  CHECK_WIDE_ARG ("10th","13-th", 16          ) // bnum payload
  CHECK_ARRAY_ARG("11th","13-th", numB, widthB) // payload
  CHECK_INT_ARG  ("12th","13-th"              ) // add crc
  CHECK_INT_ARG  ("13th","13-th"              ) // add preamble

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have 13 arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (widthB!=8) {
      vpi_printf("ERROR: %s seventh argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }
  if (numA<numB) {
      vpi_printf("ERROR: %s first argument must be largeer than seventh argument.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 pkt_udp_ip_eth_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt     ;
  vpiHandle H_bnum_pkt;
  vpiHandle H_port_dst;
  vpiHandle H_port_src;
  vpiHandle H_ip_dst   ;
  vpiHandle H_ip_src   ;
//vpiHandle H_protocol ;
  vpiHandle H_ttl      ;
  vpiHandle H_mac_dst ;
  vpiHandle H_mac_src ;
//vpiHandle H_type_len;
  vpiHandle H_bnum_payload;
  vpiHandle H_payload ;
  vpiHandle H_crc     ;
  vpiHandle H_preamble;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 bnum_pkt;
  PLI_UINT16 port_dst  ;
  PLI_UINT16 port_src  ;
  PLI_UINT32 ip_dst  ;
  PLI_UINT32 ip_src  ;
  PLI_UBYTE8 protocol=IP_PROTO_UDP;
  PLI_UBYTE8 ttl     ;
  PLI_UBYTE8 mac_dst[6];
  PLI_UBYTE8 mac_src[6];
  PLI_UINT16 type_len=ETH_TYPE_IP;
  PLI_UINT16 bnum_payload;
  PLI_UINT32 add_crc;
  PLI_UINT32 add_preamble;
  int idx, idy, idz;
  uint8_t *eth_pkt; // buffer to hold whole UDP packet
  uint8_t *payload; // buffer to hold payload data
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_port_src     = vpi_scan(arg_iterator);
  H_port_dst     = vpi_scan(arg_iterator);
  H_ip_src       = vpi_scan(arg_iterator);
  H_ip_dst       = vpi_scan(arg_iterator);
  H_ttl          = vpi_scan(arg_iterator);
  H_mac_src      = vpi_scan(arg_iterator);
  H_mac_dst      = vpi_scan(arg_iterator);
  H_bnum_payload = vpi_scan(arg_iterator);
  H_payload      = vpi_scan(arg_iterator);
  H_crc          = vpi_scan(arg_iterator);
  H_preamble     = vpi_scan(arg_iterator);

  GET_INT_ARG(H_port_src,PLI_UINT16,port_src)
  GET_INT_ARG(H_port_dst,PLI_UINT16,port_dst)
  GET_INT_ARG(H_bnum_payload,PLI_UINT16,bnum_payload)
  GET_INT_ARG(H_ip_src  ,PLI_UINT32,ip_src)
  GET_INT_ARG(H_ip_dst  ,PLI_UINT32,ip_dst)
  GET_INT_ARG(H_ttl     ,PLI_UBYTE8,ttl)
  //--------------------MAC DST
  GET_WIDE_ARG(H_mac_dst)
  val32 = value.value.vector[0].aval;
  mac_dst[5] =  val32     &0xFF;
  mac_dst[4] = (val32>> 8)&0xFF;
  mac_dst[3] = (val32>>16)&0xFF;
  mac_dst[2] = (val32>>24)&0xFF;
  val32 = value.value.vector[1].aval;
  mac_dst[1] =  val32     &0xFF;
  mac_dst[0] = (val32>> 8)&0xFF; // msb
  //--------------------MAC SRC
  GET_WIDE_ARG(H_mac_src)
  val32 = value.value.vector[0].aval;
  mac_src[5] =  val32     &0xFF;
  mac_src[4] = (val32>> 8)&0xFF;
  mac_src[3] = (val32>>16)&0xFF;
  mac_src[2] = (val32>>24)&0xFF;
  val32 = value.value.vector[1].aval;
  mac_src[1] =  val32     &0xFF;
  mac_src[0] = (val32>> 8)&0xFF; // msb
  //--------------------num of bytes of payload
  GET_INT_ARG(H_bnum_payload,PLI_UINT16,bnum_payload)
  //--------------------crc
  GET_INT_ARG(H_crc,PLI_UINT32,add_crc)
  //--------------------preamble
  GET_INT_ARG(H_preamble,PLI_UINT32,add_preamble)

  //--------------------build Ethernet packet
  tmp = ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN + bnum_payload;
  eth_pkt = (uint8_t*)calloc(tmp, 1);
  if (eth_pkt==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  payload = (uint8_t*)calloc(bnum_payload, 1);
  if (payload==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  GET_ARRAY_ARG(H_payload,0,bnum_payload,payload)

  tmp = gen_eth_ip_udp_packet( eth_pkt //uint8_t  *packet
                             , mac_src
                             , mac_dst
                             , ip_src
                             , ip_dst
                             , port_src
                             , port_dst
                             , bnum_payload // Pure UDP payload
                             , payload
                             , 1 // update UDP checksum
                             , add_crc
                             , add_preamble);

#if defined(RIGOR)
  int xxy = ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN + bnum_payload;
  if (tmp!=xxy) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, xxy);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = eth_pkt[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(eth_pkt);
  free(payload);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// Initialize PTPv2 context
// $msg_ptpv2_set_context( ptp_version 
//                       , ptp_domain
//                       , one_step_clock
//                       , unicast_port
//                       , profile_spec1
//                       , profile_spec2
//                       );
#define TASK_NAME "$msg_ptpv2_set_context"
PLI_INT32 msg_ptpv2_set_context_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle;
  PLI_INT32 arg_type;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_INT_ARG  ("1st", "six")
  CHECK_INT_ARG  ("2nd", "six")
  CHECK_INT_ARG  ("3rd", "six")
  CHECK_INT_ARG  ("4th", "six")
  CHECK_INT_ARG  ("5th", "six")
  CHECK_INT_ARG  ("6th", "six")

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 msg_ptpv2_set_context_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_ptp_version   ;
  vpiHandle H_ptp_domain    ;
  vpiHandle H_one_step_clock;
  vpiHandle H_unicast_port  ;
  vpiHandle H_profile_spec1 ;
  vpiHandle H_profile_spec2 ;
  s_vpi_value value;
  PLI_UINT32 ptp_version   ;
  PLI_UINT32 ptp_domain    ;
  PLI_UINT32 one_step_clock;
  PLI_UINT32 unicast_port  ;
  PLI_UINT32 profile_spec1 ;
  PLI_UINT32 profile_spec2 ;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_ptp_version    = vpi_scan(arg_iterator);
  H_ptp_domain     = vpi_scan(arg_iterator);
  H_one_step_clock = vpi_scan(arg_iterator);
  H_unicast_port   = vpi_scan(arg_iterator);
  H_profile_spec1  = vpi_scan(arg_iterator);
  H_profile_spec2  = vpi_scan(arg_iterator);

  GET_INT_ARG (H_ptp_version   , PLI_UINT32, ptp_version   );
  GET_INT_ARG (H_ptp_domain    , PLI_UINT32, ptp_domain    );
  GET_INT_ARG (H_one_step_clock, PLI_UINT32, one_step_clock);
  GET_INT_ARG (H_unicast_port  , PLI_UINT32, unicast_port  );
  GET_INT_ARG (H_profile_spec1 , PLI_UINT32, profile_spec1 );
  GET_INT_ARG (H_profile_spec2 , PLI_UINT32, profile_spec2 );

  gen_ptpv2_context( ptp_version   
                   , ptp_domain    
                   , one_step_clock
                   , unicast_port  
                   , profile_spec1 
                   , profile_spec2 
                   );
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// Get PTPv2 context
// $msg_ptpv2_get_context( ptp_version 
//                       , ptp_domain
//                       , one_step_clock
//                       , unicast_port
//                       , profile_spec1
//                       , profile_spec2
//                       );
#define TASK_NAME "$msg_ptpv2_get_context"
PLI_INT32 msg_ptpv2_get_context_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle;
  PLI_INT32 arg_type;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_INT_ARG  ("first" , "six")
  CHECK_INT_ARG  ("second", "six")
  CHECK_INT_ARG  ("third" , "six")
  CHECK_INT_ARG  ("fourth", "six")
  CHECK_INT_ARG  ("fifth" , "six")
  CHECK_INT_ARG  ("sixth" , "six")

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have six arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 msg_ptpv2_get_context_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_ptp_version   ;
  vpiHandle H_ptp_domain    ;
  vpiHandle H_one_step_clock;
  vpiHandle H_unicast_port  ;
  vpiHandle H_profile_spec1 ;
  vpiHandle H_profile_spec2 ;
  s_vpi_value value;
  PLI_UINT32 ptp_version   ;
  PLI_UINT32 ptp_domain    ;
  PLI_UINT32 one_step_clock;
  PLI_UINT32 unicast_port  ;
  PLI_UINT32 profile_spec1 ;
  PLI_UINT32 profile_spec2 ;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_ptp_version    = vpi_scan(arg_iterator);
  H_ptp_domain     = vpi_scan(arg_iterator);
  H_one_step_clock = vpi_scan(arg_iterator);
  H_unicast_port   = vpi_scan(arg_iterator);
  H_profile_spec1  = vpi_scan(arg_iterator);
  H_profile_spec2  = vpi_scan(arg_iterator);

  ptpv2_ctx_t *ptpv2_ctx = get_ptpv2_context();
  ptp_version    = ptpv2_ctx->ptp_version   ;
  ptp_domain     = ptpv2_ctx->ptp_domain    ;
  one_step_clock = ptpv2_ctx->one_step_clock;
  unicast_port   = ptpv2_ctx->unicast_port  ;
  profile_spec1  = ptpv2_ctx->profile_spec1 ;
  profile_spec2  = ptpv2_ctx->profile_spec2 ;

  PUT_INT_ARG (H_ptp_version   , PLI_UINT32, ptp_version   );
  PUT_INT_ARG (H_ptp_domain    , PLI_UINT32, ptp_domain    );
  PUT_INT_ARG (H_one_step_clock, PLI_UINT32, one_step_clock);
  PUT_INT_ARG (H_unicast_port  , PLI_UINT32, unicast_port  );
  PUT_INT_ARG (H_profile_spec1 , PLI_UINT32, profile_spec1 );
  PUT_INT_ARG (H_profile_spec2 , PLI_UINT32, profile_spec2 );

  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// returns PTPv2 message length
//
// 'sourcePortIdentity'
// [9:8] = port identity
// [7:5] = oui
// [4:0] = uuid
int msg_ptpv2_header(ptpv2_msg_hdr_t *ptpv2_msg_hdr
                    ,vpiHandle H_messageType
                    ,vpiHandle H_flagField
                    ,vpiHandle H_correctionField
                    ,vpiHandle H_sourceClockID
                    ,vpiHandle H_sourcePortID
                    ,vpiHandle H_sequenceID)
{
  s_vpi_value value;
  PLI_UINT32 val32;
  PLI_UINT16 val16;
  PLI_UBYTE8  val8;
  memset((void*)ptpv2_msg_hdr, 0, sizeof(ptpv2_msg_hdr_t));

  GET_INT_ARG (H_messageType,PLI_UBYTE8,val8)
  ptpv2_msg_hdr->messageType = val8&0xF;
  ptpv2_msg_hdr->versionPTP  = 0x2;

  GET_INT_ARG(H_flagField,PLI_UINT16,val16)
  ptpv2_msg_hdr->flagField = htons(val16);

  GET_WIDE_ARG(H_correctionField) //64-bit
  val32 = value.value.vector[0].aval;
  ptpv2_msg_hdr->correctionField.low = htonl(val32);
  val32 = value.value.vector[1].aval;
  ptpv2_msg_hdr->correctionField.high = htonl(val32);

  GET_WIDE_ARG(H_sourceClockID) //64-bit
  val32 = value.value.vector[0].aval;

  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[7] = val32&0x000000FF;
  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[6] =(val32&0x0000FF00)>>8;
  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[5] =(val32&0x00FF0000)>>16;
  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[4] =(val32&0xFF000000)>>24;
  val32 = value.value.vector[1].aval;

  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[3] = val32&0x000000FF;
  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[2] =(val32&0x0000FF00)>>8;
  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[1] =(val32&0x00FF0000)>>16;
  ptpv2_msg_hdr->sourcePortIdentity.clockIdentity[0] =(val32&0xFF000000)>>24;

  GET_INT_ARG(H_sourcePortID,PLI_UINT16,val16)
  ptpv2_msg_hdr->sourcePortIdentity.portNumber = htons(val16);

  GET_INT_ARG(H_sequenceID,PLI_UINT16,val16)
  ptpv2_msg_hdr->sequenceID = htons(val16);

  uint16_t msg_len;
  switch (ptpv2_msg_hdr->messageType) {
  case PTPV2_MSG_Sync                 : msg_len=sizeof(ptpv2_msg_sync_t                 ); break;
  case PTPV2_MSG_Delay_Req            : msg_len=sizeof(ptpv2_msg_delay_req_t            ); break;
  case PTPV2_MSG_Pdelay_Req           : msg_len=sizeof(ptpv2_msg_pdelay_req_t           ); break;
  case PTPV2_MSG_Pdelay_Resp          : msg_len=sizeof(ptpv2_msg_pdelay_resp_t          ); break;
  case PTPV2_MSG_Follow_Up            : msg_len=sizeof(ptpv2_msg_follow_up_t            ); break;
  case PTPV2_MSG_Delay_Resp           : msg_len=sizeof(ptpv2_msg_delay_resp_t           ); break;
  case PTPV2_MSG_Pdelay_Resp_Follow_Up: msg_len=sizeof(ptpv2_msg_pdelay_resp_follow_up_t); break;
  case PTPV2_MSG_Announce             : msg_len=sizeof(ptpv2_msg_announce_t             ); break;
  case PTPV2_MSG_Signaling            : msg_len=sizeof(ptpv2_msg_signaling_t            ); break;
  case PTPV2_MSG_Management           : msg_len=sizeof(ptpv2_msg_management_t           ); break;
  default: vpi_printf("Unknown PTPv22 message type: %u\n", ptpv2_msg_hdr->messageType);
           return 0;
  }
  ptpv2_msg_hdr->messageLength = htons(msg_len);
  uint8_t logInterval;
  uint8_t unicast=0; // 0: multicst, 1: unicast [NEED attention...]
  switch (ptpv2_msg_hdr->messageType) {
  case PTPV2_MSG_Sync                 : logInterval = (unicast) ? 0x7F: 0x03; break;
  case PTPV2_MSG_Delay_Req            : logInterval = (unicast) ? 0x7F: 0x03; break;
  case PTPV2_MSG_Pdelay_Req           : logInterval=0x7F; break;
  case PTPV2_MSG_Pdelay_Resp          : logInterval=0x7F; break;
  case PTPV2_MSG_Follow_Up            : logInterval = (unicast) ? 0x7F: 0x03; break;
  case PTPV2_MSG_Delay_Resp           : logInterval = (unicast) ? 0x7F: 0x03; break;
  case PTPV2_MSG_Pdelay_Resp_Follow_Up: logInterval=0x7F; break;
  case PTPV2_MSG_Announce             : logInterval=0x7F; break;
  case PTPV2_MSG_Signaling            : logInterval=0x7F; break;
  case PTPV2_MSG_Management           : logInterval=0x7F; break;
  }
  ptpv2_msg_hdr->logMessageInterval = logInterval;

  return msg_len;
}

//----------------------------------------------------------------------------
// $msg_ptpv2( pkt             [ 7:0][0:1024*4-1]
//           , bnum_pkt        [15:0] // num of bytes of the whole message
//           , messageType     [ 3:0]
//           , flagField       [15:0]
//           , correctionField [63:0]
//           , sourceClockID   [63:0]
//           , sourcePortID    [15:0]
//           , sequenceID      [15:0]
//           , secondsField    [47:0]
//           , nanosecondsField[31:0]
//           );
//----------------------------------------------------------------------------
#define TASK_NAME "$msg_ptpv2"
PLI_INT32 msg_ptpv2_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have ten arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "ten", numA, widthA)
  CHECK_INT_ARG  ("2nd", "ten") // bnum pkt
  CHECK_INT_ARG  ("3rd", "ten") // message type
  CHECK_INT_ARG  ("4th", "ten") // flagField
  CHECK_WIDE_ARG ("5th", "ten", 64)  // correctionField
  CHECK_WIDE_ARG ("6th", "ten", 64)  // sourceClockID
  CHECK_INT_ARG  ("7th", "ten")  // sourcePortID
  CHECK_INT_ARG  ("8th", "ten") // sequence ID
  CHECK_WIDE_ARG ("9th", "ten", 48)  // seconds
  CHECK_INT_ARG  ("10th","ten") // nano

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have 10 arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 msg_ptpv2_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt         ;
  vpiHandle H_bnum_pkt    ;
  vpiHandle H_messageType ;
  vpiHandle H_secondsField;
  vpiHandle H_nanoField   ;
  vpiHandle H_sequenceID  ;
  vpiHandle H_sourceClockID;
  vpiHandle H_sourcePortID ;
  vpiHandle H_correctionField   ;
  vpiHandle H_flagField         ;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 bnum_pkt;
  //PLI_UBYTE8 messageType;
  PLI_UINT16 secondsMsb ;
  PLI_UINT32 secondsLsb ;
  PLI_UINT32 nanoseconds;
  //PLI_UINT16 sequenceID ;
  PLI_UINT16 flagField  ;
  int idx, idy, idz;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);

  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_messageType  = vpi_scan(arg_iterator);
  H_flagField       = vpi_scan(arg_iterator);
  H_correctionField = vpi_scan(arg_iterator);
  H_sourceClockID = vpi_scan(arg_iterator);
  H_sourcePortID  = vpi_scan(arg_iterator);
  H_sequenceID   = vpi_scan(arg_iterator);
  H_secondsField = vpi_scan(arg_iterator);
  H_nanoField    = vpi_scan(arg_iterator);

  GET_WIDE_ARG(H_secondsField)
  val32 = value.value.vector[0].aval;
  secondsLsb = val32;
  val32 = value.value.vector[1].aval;
  secondsMsb = val32&0xFFFF;
  GET_INT_ARG(H_nanoField,PLI_UINT32,nanoseconds)

  //--------------------build Ethernet packet
  ptpv2_msg_hdr_t ptpv2_msg_hdr;
  uint16_t msg_len;
  msg_len = msg_ptpv2_header(&ptpv2_msg_hdr
                            ,H_messageType
                            ,H_flagField
                            ,H_correctionField
                            ,H_sourceClockID
                            ,H_sourcePortID
                            ,H_sequenceID);
  if (msg_len==0) {
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  uint8_t *ptpv2_msg; // buffer to hold whole PTPV2 message
  ptpv2_msg = (uint8_t*)calloc(msg_len, 1);
  if (ptpv2_msg==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  ptpv2_ctx_t *ctx = get_ptpv2_context();

  Timestamp_t time;
  time.secondsField.msb = secondsMsb;
  time.secondsField.lsb = secondsLsb;
  time.nanosecondsField = nanoseconds;

  msg_len = gen_ptpv2_msg[ptpv2_msg_hdr.messageType]( ctx
                                                    , ptpv2_msg
                                                    ,&ptpv2_msg_hdr
                                                    ,&time
                                                    , NULL);
  if (msg_len==0) {
      vpi_printf("something wrong while building PTPv2 header\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  //--------------------copy all generated contents
  int tmp=msg_len; // [Need attension]
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = ptpv2_msg[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(ptpv2_msg);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $msg_ptpv2_ethernet ( pkt               [ 7:0][0:1024*4-1]
//                     , bnum_pkt          [15:0] // num of bytes of the whole message
//                     , mac_src           [47:0]
//                     , messageType       [ 3:0]
//                     , flagField         [15:0]
//                     , correctionField   [63:0]
//                     , sourceClockID     [63:0]
//                     , sourcePortID      [15:0]
//                     , sequenceID        [15:0]
//                     , secondsField      [47:0]
//                     , nanosecondsField  [31:0]
//                     , reqClockID        [64:0] // PortId(2, MSBytes)+ClockId(8)
//                     , reqPortID         [15:0] // PortId(2, MSBytes)+ClockId(8)
//                     , add_crc
//                     , add_preamble
//                     )
//----------------------------------------------------------------------------
#define TASK_NAME "$msg_ptpv2_ethernet"
PLI_INT32 msg_ptpv2_eth_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have 15 arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "15", numA, widthA)
  CHECK_INT_ARG  ("2nd", "15") // bnum pkt
  CHECK_WIDE_ARG ("3rd", "15", 48) // MAC SRC
  CHECK_INT_ARG  ("4rd", "15") // message type
  CHECK_INT_ARG  ("5th", "15") // flagField
  CHECK_WIDE_ARG ("6th", "15", 64)  // correctionField
  CHECK_WIDE_ARG ("7th", "15", 64)  // sourcePortIdentify
  CHECK_INT_ARG  ("8th", "15")  // sourcePortIdentify
  CHECK_INT_ARG  ("9th", "15") // sequence ID
  CHECK_WIDE_ARG ("10th","15", 48)  // seconds
  CHECK_INT_ARG  ("11th","15") // nano
  CHECK_WIDE_ARG ("12th","15", 64) // reqClockID
  CHECK_INT_ARG  ("13th","15") // reqClockID
  CHECK_INT_ARG  ("14th","15") // crc
  CHECK_INT_ARG  ("15th","15") // preamble

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have 15 arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 msg_ptpv2_eth_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt         ;
  vpiHandle H_bnum_pkt    ;
  vpiHandle H_messageType ;
  vpiHandle H_secondsField;
  vpiHandle H_nanoField   ;
  vpiHandle H_sequenceID  ;
  vpiHandle H_sourceClockID;
  vpiHandle H_sourcePortID;
  vpiHandle H_correctionField   ;
  vpiHandle H_flagField ;
  vpiHandle H_reqClockID;
  vpiHandle H_reqPortID ;
  vpiHandle H_mac_src   ;
  vpiHandle H_crc     ;
  vpiHandle H_preamble;
  s_vpi_value value;
  PLI_UINT32 val32, val16, val8;
  PLI_UINT16 bnum_pkt   ;
  //PLI_UBYTE8 messageType;
  PLI_UINT16 secondsMsb ;
  PLI_UINT32 secondsLsb ;
  PLI_UINT32 nanoseconds;
  //PLI_UINT16 sequenceID ;
  PLI_UBYTE8 mac_src[6] ;
  PLI_UINT32 add_crc     ;
  PLI_UINT32 add_preamble;
  int idx, idy, idz;
  uint8_t *ptpv2_msg; // buffer to hold whole Ethernet packet
  int tmp;

//printf("%s()@%s 0\n", __FUNCTION__, __FILE__); fflush(stdout); vpi_flush();
  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);

  H_pkt             = vpi_scan(arg_iterator);
  H_bnum_pkt        = vpi_scan(arg_iterator);
  H_mac_src         = vpi_scan(arg_iterator);
  H_messageType     = vpi_scan(arg_iterator);
  H_flagField       = vpi_scan(arg_iterator);
  H_correctionField = vpi_scan(arg_iterator);
  H_sourceClockID   = vpi_scan(arg_iterator);
  H_sourcePortID    = vpi_scan(arg_iterator);
  H_sequenceID      = vpi_scan(arg_iterator);
  H_secondsField    = vpi_scan(arg_iterator);
  H_nanoField       = vpi_scan(arg_iterator);
  H_reqClockID      = vpi_scan(arg_iterator);
  H_reqPortID       = vpi_scan(arg_iterator);
  H_crc             = vpi_scan(arg_iterator);
  H_preamble        = vpi_scan(arg_iterator);
  
//printf("%s()@%s 1\n", __FUNCTION__, __FILE__); fflush(stdout); vpi_flush();
  GET_WIDE_ARG(H_secondsField)
  val32 = value.value.vector[0].aval;
  secondsLsb = val32;
  val32 = value.value.vector[1].aval;
  secondsMsb = val32&0xFFFF;
  GET_INT_ARG(H_nanoField,PLI_UINT32,nanoseconds)
//printf("%s()@%s secondMSB=0x%08X LSB=0x%08X NS=0x%08X\n", __FUNCTION__, __FILE__, secondsMsb, secondsLsb, nanoseconds);

//printf("%s()@%s 2\n", __FUNCTION__, __FILE__); fflush(stdout); vpi_flush();
  GET_WIDE_ARG(H_mac_src)
  val32 = value.value.vector[0].aval;
  mac_src[5] =  val32     &0xFF;
  mac_src[4] = (val32>> 8)&0xFF;
  mac_src[3] = (val32>>16)&0xFF;
  mac_src[2] = (val32>>24)&0xFF;
  val32 = value.value.vector[1].aval;
  mac_src[1] =  val32     &0xFF;
  mac_src[0] = (val32>> 8)&0xFF; // msb

//printf("%s()@%s 3\n", __FUNCTION__, __FILE__); fflush(stdout); vpi_flush();
  PortIdentity_t reqClockID;
  GET_WIDE_ARG(H_reqClockID)
  val32 = value.value.vector[0].aval;
//printf("%s val32 0x%08X\n", __FUNCTION__, val32);
  reqClockID.clockIdentity[7] = (val32&0x000000FF);
  reqClockID.clockIdentity[6] = (val32&0x0000FF00)>>8;
  reqClockID.clockIdentity[5] = (val32&0x00FF0000)>>16;
  reqClockID.clockIdentity[4] = (val32&0xFF000000)>>24;
  val32 = value.value.vector[1].aval;
//printf("%s val32 0x%08X\n", __FUNCTION__, val32);
  reqClockID.clockIdentity[3] = (val32&0x000000FF);
  reqClockID.clockIdentity[2] = (val32&0x0000FF00)>>8;
  reqClockID.clockIdentity[1] = (val32&0x00FF0000)>>16;
  reqClockID.clockIdentity[0] = (val32&0xFF000000)>>24;

  GET_INT_ARG(H_reqPortID,PLI_UINT16,val16)
  reqClockID.portNumber = htons(val16);

  GET_INT_ARG(H_crc,PLI_UINT32,add_crc)
  GET_INT_ARG(H_preamble,PLI_UINT16,add_preamble)

  //--------------------build Ethernet packet
  ptpv2_msg_hdr_t ptpv2_msg_hdr;
  uint16_t msg_len;
  msg_len = msg_ptpv2_header(&ptpv2_msg_hdr
                            ,H_messageType
                            ,H_flagField
                            ,H_correctionField
                            ,H_sourceClockID
                            ,H_sourcePortID
                            ,H_sequenceID);
  if (msg_len==0) {
      vpi_printf("something wrong while building PTPv2 header\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  msg_len = (add_crc&&(msg_len<46)) ? 46 : msg_len;
  msg_len += ETH_HDR_LEN;
  msg_len += (add_preamble) ? 8 : 0;
  msg_len += (add_crc     ) ? 4 : 0;

  ptpv2_msg = (uint8_t*)calloc(msg_len, 1);
  if (ptpv2_msg==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  ptpv2_ctx_t *ctx = get_ptpv2_context();

  Timestamp_t time;
  time.secondsField.msb = secondsMsb;
  time.secondsField.lsb = secondsLsb;
  time.nanosecondsField = nanoseconds;

  tmp = gen_ptpv2_msg_ethernet( ctx
                              , ptpv2_msg
                              , mac_src
                              ,&ptpv2_msg_hdr
                              ,&time
                              ,&reqClockID
                              , add_crc
                              , add_preamble
                              );

#if defined(RIGOR)
  if (tmp!=msg_len) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, msg_len);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = ptpv2_msg[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(ptpv2_msg);
  vpi_free_object(arg_iterator);

  return(0);
}
//----------------------------------------------------------------------------
// $msg_ptpv2_udp_ip_ethernet ( pkt               [7:0][0:1024*4-1]
//                            , bnum_pkt          [15:0] // num of bytes of the whole message
//                            , mac_src           [47:0]
//                            , ip_src            [31:0]
//                            , messageType       [ 3:0]
//                            , flagField         [15:0]
//                            , correctionField   [63:0]
//                            , sourceClockID     [63:0]
//                            , sourcePortID      [15:0]
//                            , sequenceID        [15:0]
//                            , secondsField      [47:0]
//                            , nanosecondsField  [31:0]
//                            , reqClockID        [63:0]
//                            , reqPortID         [16:0]
//                            , add_crc      //
//                            , add_preamble //
//                            );
//----------------------------------------------------------------------------
#define TASK_NAME "$msg_ptpv2_udp_ip_ethernet"
PLI_INT32 msg_ptpv2_udp_ip_eth_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have 16 arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "16", numA, widthA)
  CHECK_INT_ARG  ("2nd", "16") // bnum pkt
  CHECK_WIDE_ARG ("3rd", "16", 48) // MAC SRC
  CHECK_INT_ARG  ("4th", "16") // IP SRC
  CHECK_INT_ARG  ("5th", "16") // message type
  CHECK_INT_ARG  ("6th", "16") // flagField
  CHECK_WIDE_ARG ("7th", "16", 64)  // correctionField
  CHECK_WIDE_ARG ("8th", "16", 64)  // sourcePortIdentify
  CHECK_INT_ARG  ("9th", "16")  // sourcePortIdentify
  CHECK_INT_ARG  ("10th","16") // sequence ID
  CHECK_WIDE_ARG ("11th","16", 48)  // seconds
  CHECK_INT_ARG  ("12th","16") // nano
  CHECK_WIDE_ARG ("13th","16", 64) // 
  CHECK_INT_ARG  ("14th","16") // 
  CHECK_INT_ARG  ("15th","16") // crc
  CHECK_INT_ARG  ("16th","16") // preamble

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have 16 arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 msg_ptpv2_udp_ip_eth_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt         ;
  vpiHandle H_bnum_pkt    ;
  vpiHandle H_messageType ;
  vpiHandle H_secondsField;
  vpiHandle H_nanoField   ;
  vpiHandle H_sequenceID  ;
  vpiHandle H_sourceClockID;
  vpiHandle H_sourcePortID;
  vpiHandle H_correctionField   ;
  vpiHandle H_flagField         ;
  vpiHandle H_reqClockID        ;
  vpiHandle H_reqPortID        ;
  vpiHandle H_ip_src      ;
  vpiHandle H_mac_src     ;
  vpiHandle H_crc     ;
  vpiHandle H_preamble;
  s_vpi_value value;
  PLI_UINT32 val32, val16, val8;
  PLI_UINT16 bnum_pkt   ;
  PLI_UBYTE8 messageType;
  PLI_UINT16 secondsMsb ;
  PLI_UINT32 secondsLsb ;
  PLI_UINT32 nanoseconds;
  PLI_UINT16 sequenceID ;
  PLI_UINT32 ip_src     ;
  PLI_UBYTE8 mac_src[6] ;
  PLI_UINT32 add_crc     ;
  PLI_UINT32 add_preamble;
  int idx, idy, idz;
  uint8_t *ptpv2_msg; // buffer to hold whole Ethernet packet
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);

  H_pkt          = vpi_scan(arg_iterator);
  H_bnum_pkt     = vpi_scan(arg_iterator);
  H_mac_src      = vpi_scan(arg_iterator);
  H_ip_src       = vpi_scan(arg_iterator);
  H_messageType  = vpi_scan(arg_iterator);
  H_flagField          = vpi_scan(arg_iterator);
  H_correctionField    = vpi_scan(arg_iterator);
  H_sourceClockID = vpi_scan(arg_iterator);
  H_sourcePortID = vpi_scan(arg_iterator);
  H_sequenceID   = vpi_scan(arg_iterator);
  H_secondsField = vpi_scan(arg_iterator);
  H_nanoField    = vpi_scan(arg_iterator);
  H_reqClockID   = vpi_scan(arg_iterator);
  H_reqPortID   = vpi_scan(arg_iterator);
  H_crc          = vpi_scan(arg_iterator);
  H_preamble     = vpi_scan(arg_iterator);
  
  GET_WIDE_ARG(H_secondsField)
  val32 = value.value.vector[0].aval;
  secondsLsb = val32;
  val32 = value.value.vector[1].aval;
  secondsMsb = val32&0xFFFF;
  GET_INT_ARG(H_nanoField,PLI_UINT32,nanoseconds)
  GET_INT_ARG(H_sequenceID,PLI_UINT16,sequenceID)

  GET_INT_ARG(H_ip_src,PLI_UINT32,ip_src)
  GET_WIDE_ARG(H_mac_src)
  val32 = value.value.vector[0].aval;
  mac_src[5] =  val32     &0xFF;
  mac_src[4] = (val32>> 8)&0xFF;
  mac_src[3] = (val32>>16)&0xFF;
  mac_src[2] = (val32>>24)&0xFF;
  val32 = value.value.vector[1].aval;
  mac_src[1] =  val32     &0xFF;
  mac_src[0] = (val32>> 8)&0xFF; // msb

  PortIdentity_t reqClockID;
  GET_WIDE_ARG(H_reqClockID)

  val32 = value.value.vector[0].aval;
  reqClockID.clockIdentity[7] = (val32&0x000000FF);
  reqClockID.clockIdentity[6] = (val32&0x0000FF00)>>8;
  reqClockID.clockIdentity[5] = (val32&0x00FF0000)>>16;
  reqClockID.clockIdentity[4] = (val32&0xFF000000)>>24;

  val32 = value.value.vector[1].aval;
  reqClockID.clockIdentity[3] = (val32&0x000000FF);
  reqClockID.clockIdentity[2] = (val32&0x0000FF00)>>8;
  reqClockID.clockIdentity[1] = (val32&0x00FF0000)>>16;
  reqClockID.clockIdentity[0] = (val32&0xFF000000)>>24;

  GET_INT_ARG(H_reqPortID,PLI_UINT16,val16)
  reqClockID.portNumber = htons(val16);

  GET_INT_ARG(H_crc,PLI_UINT32,add_crc)
  GET_INT_ARG(H_preamble,PLI_UINT16,add_preamble)

  //--------------------build Ethernet packet
  ptpv2_msg_hdr_t ptpv2_msg_hdr;
  uint16_t msg_len;
  msg_len = msg_ptpv2_header(&ptpv2_msg_hdr
                            ,H_messageType
                            ,H_flagField
                            ,H_correctionField
                            ,H_sourceClockID
                            ,H_sourcePortID
                            ,H_sequenceID);
  if (msg_len==0) {
      vpi_printf("something wrong while building PTPv2 header\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  msg_len = ((UDP_HDR_LEN+IP_HDR_LEN+msg_len)<46) ? 46 : (UDP_HDR_LEN+IP_HDR_LEN+msg_len);
  msg_len += ETH_HDR_LEN;
  msg_len += (add_preamble) ? 8 : 0;
  msg_len += (add_crc     ) ? 4 : 0;

  ptpv2_msg = (uint8_t*)calloc(msg_len, 1);
  if (ptpv2_msg==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  ptpv2_ctx_t *ctx = get_ptpv2_context();

  Timestamp_t time;
  time.secondsField.msb = secondsMsb;
  time.secondsField.lsb = secondsLsb;
  time.nanosecondsField = nanoseconds;

  tmp = gen_ptpv2_msg_udp_ip_ethernet( ctx
                                     , ptpv2_msg
                                     , mac_src
                                     , ip_src
                                     ,&ptpv2_msg_hdr
                                     ,&time
                                     ,&reqClockID
                                     , add_crc
                                     , add_preamble
                                     );
#if defined(RIGOR)
  if (tmp!=msg_len) {
       vpi_printf("ERROR: %s()@%s whole packet length error %d %d\n", __FUNCTION__, __FILE__, tmp, msg_len);
  }
#endif

  //--------------------copy all generated contents
  for (idy=0; idy<tmp; idy++) {
       vpiHandle ele = vpi_handle_by_index(H_pkt, idy);
       value.value.integer = ptpv2_msg[idy];
       vpi_put_value(ele, &value, NULL, vpiNoDelay);
  }

  //--------------------put num of bytes of Ethernet packet
  value.format = vpiIntVal;
  value.value.integer = tmp;
  vpi_put_value(H_bnum_pkt, &value, NULL, vpiNoDelay);

  free(ptpv2_msg);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $pkt_ethernet_parser( pkt     [ 7:0][0:1024*4-1]
//                     , bnum_pkt[15:0]
//                     , crc   
//                     , preamble
//                     );
//----------------------------------------------------------------------------
#define TASK_NAME "$pkt_ethernet_parser"
PLI_INT32 pkt_eth_parser_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle, ele_handle;
  PLI_INT32 tfarg_type, arg_type;
  int width;
  int numA, widthA;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) {
      vpi_printf("ERROR: %s must have four arguments.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  CHECK_ARRAY_ARG("1st", "four", numA, widthA)
  CHECK_INT_ARG  ("2nd", "four") // bnum_pkt
  CHECK_INT_ARG  ("3rd", "four") // crc
  CHECK_INT_ARG  ("4th", "four") // preamble

  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
      vpi_printf("ERROR: %s must have four arguments.\n", TASK_NAME);
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }

  if (widthA!=8) {
      vpi_printf("ERROR: %s first argument must be 8-bit array.\n", TASK_NAME);
      pkt_control(vpiFinish);
  }

  return(0);
}
#undef TASK_NAME
//----------------------------------------------------------------------------
PLI_INT32 pkt_eth_parser_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator;
  vpiHandle H_pkt     ;
  vpiHandle H_bnum_pkt;
  vpiHandle H_crc     ;
  vpiHandle H_preamble;
  s_vpi_value value;
  PLI_UINT32 val32, val8;
  PLI_UINT16 leng;
  PLI_UINT32 crc ;
  PLI_UINT32 preamble ;
  int idx, idy, idz;
  uint8_t *eth_pkt; // buffer to hold whole Ethernet packet
  uint16_t type_leng;
  int tmp;

  //--------------------Get all handlers
  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  H_pkt        = vpi_scan(arg_iterator);
  H_bnum_pkt   = vpi_scan(arg_iterator);
  H_crc        = vpi_scan(arg_iterator);
  H_preamble   = vpi_scan(arg_iterator);

  //--------------------Get all values
  GET_INT_ARG(H_bnum_pkt,PLI_UBYTE8,leng    )
  GET_INT_ARG(H_crc     ,PLI_UINT32,crc     )
  GET_INT_ARG(H_preamble,PLI_UINT32,preamble)
  //--------------------parsing
  eth_pkt = (uint8_t*)calloc(leng, 1);
  if (eth_pkt==NULL) {
      vpi_printf("ERROR: calloc error.\n");
      vpi_free_object(arg_iterator);
      pkt_control(vpiFinish);
  }
  GET_ARRAY_ARG(H_pkt,0,leng,eth_pkt)
  //--------------------parsing
  idx = 0;
  if (preamble&(leng>=8)) {
      //vpi_printf("preamble: 0x");
      //for (idy=0; idy<8; idy++) vpi_printf("%02X",eth_pkt[idy]);
      //vpi_printf("\n");
      idx = 8;
  }
  parser_eth_packet(&eth_pkt[idx], leng-idx);
  type_leng  = eth_pkt[idx+12]<<8;
  type_leng |= eth_pkt[idx+13];
  //if (leng>=(idx+6)) {
  //    vpi_printf("mac dst  : 0x");
  //    for (idy=0; idy<6; idy++) vpi_printf("%02X",eth_pkt[idx++]);
  //    vpi_printf("\n");
  //} else goto end;
  //if (leng>=(idx+6)) {
  //    vpi_printf("mac src  : 0x");
  //    for (idy=0; idy<6; idy++) vpi_printf("%02X",eth_pkt[idx++]);
  //    vpi_printf("\n");
  //} else goto end;
  //if (leng>=(idx+2)) {
  //    type_leng  = eth_pkt[idx++]<<8;
  //    type_leng |= eth_pkt[idx++];
  //    vpi_printf("type leng: 0x%04X", type_leng);
  //    switch (type_leng) {
  //    case 0x0800: vpi_printf(" IPv4  packet\n"); break;
  //    case 0x0806: vpi_printf(" ARP   packet\n"); break;
  //    case 0x08DD: vpi_printf(" IPv6  packet\n"); break;
  //    case 0x8100: vpi_printf(" VLAN  packet\n"); break;
  //    case 0x88F7: vpi_printf(" PTPv2 raw packet\n"); break;
  //    default:     vpi_printf("\n"); break;
  //    }
  //} else goto end;
  if (type_leng==0x0800) { // IP packet
      uint8_t protocol = eth_pkt[idx+14+9];
      if (protocol==0x11) { // UDP
          uint16_t port_src  = eth_pkt[idx+14+20]<<8;
                   port_src |= eth_pkt[idx+14+21];
          uint16_t port_dst  = eth_pkt[idx+14+22]<<8;
                   port_dst |= eth_pkt[idx+14+23];
          if ((port_src==319)||(port_src==320)||
              (port_dst==319)||(port_dst==320)) {
             vpi_flush(); vpi_printf("\n");
             fflush(stderr); fflush(stdout);
             parser_ptpv2_message(&eth_pkt[idx+14+28],leng-idx-14-28);
             fflush(stderr); fflush(stdout);
          }
      }
  }
  if (type_leng==0x88F7) { // PTPv2 raw packet
      vpi_flush(); vpi_printf("\n");
      fflush(stderr); fflush(stdout);
      parser_ptpv2_message(&eth_pkt[idx+14],leng-idx-14);
      fflush(stderr); fflush(stdout);
  }

  //--------------------return
end:
  free(eth_pkt);
  vpi_free_object(arg_iterator);

  return(0);
}

//----------------------------------------------------------------------------
// $pkt_eth_verbose; ==> $pkt_verbose(0);
// $pkt_eth_verbose(); ==> $pkt_verbose(0);
// $pkt_eth_verbose(n);
//----------------------------------------------------------------------------
PLI_INT32 pkt_eth_verbose_Calltf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_itr, arg1, arg2;
  s_vpi_value value;
  int level;
  PLI_INT32 result;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_itr      = vpi_iterate(vpiArgument, systf_handle);
  if (arg_itr==NULL) {
      level = 0;
  } else {
     arg1 = vpi_scan(arg_itr);
     if (vpiOperation==vpi_get(vpiType,arg1)) {
         level = 0;
     } else {
         value.format = vpiIntVal;
         vpi_get_value(arg1, &value);
         level = value.value.integer;
     }
     vpi_free_object(arg_itr);
  }
  m_verbose = level; 
  value.format = vpiIntVal;
  value.value.integer = 0;
  vpi_put_value(systf_handle, &value, NULL, vpiNoDelay);
  return(0);
}

//----------------------------------------------------------------------------
PLI_INT32 pkt_eth_verbose_Compiletf(PLI_BYTE8 *user_data) {
  vpiHandle systf_handle, arg_iterator, arg_handle;
  PLI_INT32 tfarg_type;

  systf_handle = vpi_handle(vpiSysTfCall, NULL);
  arg_iterator = vpi_iterate(vpiArgument, systf_handle);
  if (arg_iterator==NULL) { /* no argument */
     return(0);
  }
  arg_handle   = vpi_scan(arg_iterator);
  if (arg_handle==NULL) {
       vpi_printf("ERROR: $pkt_verbose must have only one argument.\n");
       pkt_control(vpiFinish);
  }
  tfarg_type = vpi_get(vpiType, arg_handle);
  if (tfarg_type==vpiOperation) {
     if (vpiNullOp!=vpi_get(vpiOpType,arg_handle)) {
        // deal with empty ()
        vpi_printf("ERROR: $pkt_verbose must have only one argument if any.\n");
        pkt_control(vpiFinish);
     }
  } else {
     if ((tfarg_type!=vpiIntegerVar)&&(tfarg_type!=vpiParameter)&&(tfarg_type!=vpiConstant)) {
          vpi_printf("ERROR: $pkt_verbose must have integer argument, but %d.\n",
                      tfarg_type);
          vpi_free_object(arg_iterator);
          pkt_control(vpiFinish);
     }
  }
  arg_handle = vpi_scan(arg_iterator);
  if (arg_handle!=NULL) {
       vpi_printf("ERROR: $pkt_verbose must have one arguments.\n");
       vpi_free_object(arg_iterator);
       pkt_control(vpiFinish);
  }
  return(0);
}
//----------------------------------------------------------------------------
PLI_INT32 pkt_eth_verbose_Sizetf(PLI_BYTE8 *user_data) {
  return(32); /* $pkt_verbose() returns 32-bit */
}

//----------------------------------------------------------------------------
void (*vlog_startup_routines[])() = {
      pkt_register,
      0
};

//----------------------------------------------------------------------------
// Revision history:
//
// 2019.05.20: Rewritten by Ando Ki (andoki@gmail.com)
//----------------------------------------------------------------------------
