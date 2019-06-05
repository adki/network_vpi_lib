`ifndef TOP_TASKS_UDP_IP_ETHERNET_V
`define TOP_TASKS_UDP_IP_ETHERNET_V
task test_udp_ip_ethernet;
    reg [ 7:0] pkt_eth[0:4095];
    reg [15:0] bnum_pkt;
    reg [47:0] mac_src;
    reg [47:0] mac_dst;
    reg [31:0] ip_src  ;
    reg [31:0] ip_dst  ;
    reg [ 7:0] ttl     ;
    reg [15:0] port_src;
    reg [15:0] port_dst;
    reg [15:0] bnum_payload;
    reg [ 7:0] payload[0:4095];
    reg [31:0] add_crc;
    integer    add_preamble;
    integer idx;
begin
        for (idx=0; idx<1024; idx=idx+1) pkt_eth[idx] = 8'hFF;
        mac_src=48'h02_12_34_56_78_9A;
        mac_dst=48'h02_11_22_33_44_55;
        ip_src  =32'hC0ABCDEF;
        ip_dst  =32'hC1234567;
        ttl     =1;
        port_src=16'h2112;
        port_dst=16'h1221;
        bnum_payload=10;
        for (idx=0; idx<1024; idx=idx+1) payload[idx] = idx+1;
        add_crc=0;
        add_preamble=0;
//--------------------
        $pkt_udp_ip_ethernet( pkt_eth
                            , bnum_pkt
                            , port_src
                            , port_dst
                            , ip_src  
                            , ip_dst  
                            , ttl     
                            , mac_src
                            , mac_dst
                            , bnum_payload
                            , payload
                            , add_crc
                            , add_preamble
                            );
        $display("%m bnum_pkt=%d %s", bnum_pkt, (bnum_pkt==(14+20+8+10)) ? "OK" : "ERROR");
        $pkt_ethernet_parser( pkt_eth
                    , bnum_pkt
                    , add_crc
                    , add_preamble
                    );
        #10;
    end
endtask
`endif
