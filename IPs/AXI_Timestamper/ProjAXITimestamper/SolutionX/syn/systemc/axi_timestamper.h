// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2018.3
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

#ifndef _axi_timestamper_HH_
#define _axi_timestamper_HH_

#include "systemc.h"
#include "AESL_pkg.h"

#include "axi_timestamper_timestamp_s_axi.h"

namespace ap_rtl {

template<unsigned int C_S_AXI_TIMESTAMP_ADDR_WIDTH = 32,
         unsigned int C_S_AXI_TIMESTAMP_DATA_WIDTH = 32>
struct axi_timestamper : public sc_module {
    // Port declarations 28
    sc_in_clk ap_clk;
    sc_in< sc_logic > ap_rst_n;
    sc_in< sc_lv<1> > arvalid_V;
    sc_in< sc_lv<1> > arready_V;
    sc_in< sc_lv<1> > rvalid_V;
    sc_in< sc_lv<1> > rready_V;
    sc_in< sc_lv<1> > wvalid_V;
    sc_in< sc_lv<1> > wready_V;
    sc_in< sc_lv<1> > wlast_V;
    sc_in< sc_lv<1> > bvalid_V;
    sc_in< sc_lv<1> > bready_V;
    sc_in< sc_logic > s_axi_timestamp_AWVALID;
    sc_out< sc_logic > s_axi_timestamp_AWREADY;
    sc_in< sc_uint<C_S_AXI_TIMESTAMP_ADDR_WIDTH> > s_axi_timestamp_AWADDR;
    sc_in< sc_logic > s_axi_timestamp_WVALID;
    sc_out< sc_logic > s_axi_timestamp_WREADY;
    sc_in< sc_uint<C_S_AXI_TIMESTAMP_DATA_WIDTH> > s_axi_timestamp_WDATA;
    sc_in< sc_uint<C_S_AXI_TIMESTAMP_DATA_WIDTH/8> > s_axi_timestamp_WSTRB;
    sc_in< sc_logic > s_axi_timestamp_ARVALID;
    sc_out< sc_logic > s_axi_timestamp_ARREADY;
    sc_in< sc_uint<C_S_AXI_TIMESTAMP_ADDR_WIDTH> > s_axi_timestamp_ARADDR;
    sc_out< sc_logic > s_axi_timestamp_RVALID;
    sc_in< sc_logic > s_axi_timestamp_RREADY;
    sc_out< sc_uint<C_S_AXI_TIMESTAMP_DATA_WIDTH> > s_axi_timestamp_RDATA;
    sc_out< sc_lv<2> > s_axi_timestamp_RRESP;
    sc_out< sc_logic > s_axi_timestamp_BVALID;
    sc_in< sc_logic > s_axi_timestamp_BREADY;
    sc_out< sc_lv<2> > s_axi_timestamp_BRESP;
    sc_signal< sc_logic > ap_var_for_const0;


    // Module declarations
    axi_timestamper(sc_module_name name);
    SC_HAS_PROCESS(axi_timestamper);

    ~axi_timestamper();

    sc_trace_file* mVcdFile;

    ofstream mHdltvinHandle;
    ofstream mHdltvoutHandle;
    axi_timestamper_timestamp_s_axi<C_S_AXI_TIMESTAMP_ADDR_WIDTH,C_S_AXI_TIMESTAMP_DATA_WIDTH>* axi_timestamper_timestamp_s_axi_U;
    sc_signal< sc_logic > ap_rst_n_inv;
    sc_signal< sc_logic > write_timestamp_V_ap_vld;
    sc_signal< sc_logic > read_timestamp_V_ap_vld;
    sc_signal< sc_lv<32> > local_read_timestamp;
    sc_signal< sc_lv<32> > local_write_timestam;
    sc_signal< sc_lv<32> > read_latency_counter;
    sc_signal< sc_lv<1> > read_count_flag_V;
    sc_signal< sc_lv<32> > write_latency_counte;
    sc_signal< sc_lv<1> > write_count_flag_V;
    sc_signal< sc_lv<32> > ap_phi_mux_read_latency_counter_2_phi_fu_143_p6;
    sc_signal< sc_lv<1> > ap_CS_fsm;
    sc_signal< sc_logic > ap_CS_fsm_state1;
    sc_signal< sc_lv<1> > ret_V_fu_174_p2;
    sc_signal< sc_lv<32> > read_latency_counter_1_fu_189_p2;
    sc_signal< sc_lv<1> > read_count_flag_V_lo_load_fu_185_p1;
    sc_signal< sc_lv<32> > ap_phi_mux_write_latency_counte_1_phi_fu_155_p6;
    sc_signal< sc_lv<1> > ret_V_2_fu_238_p2;
    sc_signal< sc_lv<32> > tmp_4_fu_253_p2;
    sc_signal< sc_lv<1> > write_count_flag_V_l_load_fu_249_p1;
    sc_signal< sc_lv<1> > ret_V_1_fu_208_p2;
    sc_signal< sc_lv<1> > ret_V_3_fu_272_p2;
    sc_signal< sc_lv<1> > tmp_fu_232_p2;
    sc_signal< sc_lv<1> > ap_NS_fsm;
    static const sc_logic ap_const_logic_1;
    static const sc_logic ap_const_logic_0;
    static const sc_lv<1> ap_ST_fsm_state1;
    static const sc_lv<32> ap_const_lv32_0;
    static const sc_lv<1> ap_const_lv1_0;
    static const int C_S_AXI_DATA_WIDTH;
    static const sc_lv<1> ap_const_lv1_1;
    static const sc_lv<32> ap_const_lv32_1;
    static const bool ap_const_boolean_1;
    // Thread declarations
    void thread_ap_var_for_const0();
    void thread_ap_clk_no_reset_();
    void thread_ap_CS_fsm_state1();
    void thread_ap_phi_mux_read_latency_counter_2_phi_fu_143_p6();
    void thread_ap_phi_mux_write_latency_counte_1_phi_fu_155_p6();
    void thread_ap_rst_n_inv();
    void thread_read_count_flag_V_lo_load_fu_185_p1();
    void thread_read_latency_counter_1_fu_189_p2();
    void thread_read_timestamp_V_ap_vld();
    void thread_ret_V_1_fu_208_p2();
    void thread_ret_V_2_fu_238_p2();
    void thread_ret_V_3_fu_272_p2();
    void thread_ret_V_fu_174_p2();
    void thread_tmp_4_fu_253_p2();
    void thread_tmp_fu_232_p2();
    void thread_write_count_flag_V_l_load_fu_249_p1();
    void thread_write_timestamp_V_ap_vld();
    void thread_ap_NS_fsm();
    void thread_hdltv_gen();
};

}

using namespace ap_rtl;

#endif
