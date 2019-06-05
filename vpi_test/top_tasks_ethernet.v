`ifndef TOP_TASKS_ETHERNET_V
`define TOP_TASKS_ETHERNET_V
task test_ethernet;
    reg [ 7:0] pkt_eth[0:1023];
    reg [15:0] bnum_pkt;
    reg [47:0] mac_src;
    reg [47:0] mac_dst;
    reg [15:0] type_len;
    reg [15:0] bnum_payload;
    reg [ 7:0] payload[0:1023];
    reg [31:0] add_crc;
    integer    add_preamble;
    integer idx;
begin
        for (idx=0; idx<1024; idx=idx+1) pkt_eth[idx] = 8'hFF;
        mac_src=48'h02_12_34_56_78_9A;
        mac_dst=48'h02_11_22_33_44_55;
        for (idx=0; idx<1024; idx=idx+1) payload[idx] = idx;
        type_len=0; // 0 menas use 'bnum_payload'
$display("%05d %m 1", $time);
//--------------------
if (1) begin
        bnum_payload = 10;
        type_len = bnum_payload;
        add_crc = 0;
        add_preamble = 0;
        bnum_pkt=0;
$display("%05d %m 2", $time);
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkt=%d %s", bnum_pkt, (bnum_pkt==(14+10)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
$display("%05d %m 3", $time);
       //--------------------
if (0) begin
        bnum_payload = 10;
        type_len = bnum_payload;
        add_crc = 1;
        add_preamble = 0;
        bnum_pkt=0;
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkt=%d %s", bnum_pkt, (bnum_pkt==(14+46+4)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
       //--------------------
if (0) begin
        bnum_payload = 10;
        type_len = bnum_payload;
        add_crc = 1;
        add_preamble = 1;
        bnum_pkt=0;
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkt=%d %s", bnum_pkt, (bnum_pkt==(8+14+46+4)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
//--------------------
if (0) begin
        bnum_payload = 46;
        type_len = bnum_payload;
        add_crc = 0;
        add_preamble = 0;
        bnum_pkt=0;
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkt=%d %s", bnum_pkt, (bnum_pkt==(14+46)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
       //--------------------
if (0) begin
        bnum_payload = 46;
        type_len = bnum_payload;
        add_crc = 1;
        add_preamble = 0;
        bnum_pkt=0;
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkts%d %s", bnum_pkt, (bnum_pkt==(14+46+4)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
       //--------------------
if (0) begin
        bnum_payload = 46;
        type_len = bnum_payload;
        add_crc = 0;
        add_preamble = 1;
        bnum_pkt=0;
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkts%d %s", bnum_pkt, (bnum_pkt==(8+14+46)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
       //--------------------
if (0) begin
        bnum_payload = 46;
        type_len = bnum_payload;
        add_crc = 1;
        add_preamble = 1;
        bnum_pkt=0;
        $pkt_ethernet(pkt_eth
                     ,bnum_pkt
                     ,mac_src
                     ,mac_dst
                     ,type_len
                     ,bnum_payload
                     ,payload
                     ,add_crc
                     ,add_preamble
                     );
        $display("%m bnum_pkts%d %s", bnum_pkt, (bnum_pkt==(8+14+46+4)) ? "OK" : "ERROR");
        //for (idx=0; (idx<bnum_pkt)&&(idx<100); idx=idx+1) begin
        //     $display("%m pkt_eth[%02d]=0x%02X", idx, pkt_eth[idx]);
        //end
        #10;
end
    end
endtask
`endif
