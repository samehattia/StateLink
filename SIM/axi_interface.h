#ifndef AXI_INTERFACE_H
#define AXI_INTERFACE_H

#include <queue>
#include <mutex>
#include "vpi_user.h"

struct axi_interface_ports {	
	// AR Channel
	vpiHandle arvalid;
	vpiHandle arready;

	vpiHandle araddr;
	vpiHandle arlen;
	vpiHandle arsize;
	vpiHandle arburst;

	// R Channel
	vpiHandle rvalid;
	vpiHandle rready;

	vpiHandle rdata;
	vpiHandle rlast;
	vpiHandle rresp;

	// AW Channel
	vpiHandle awvalid;
	vpiHandle awready;

	vpiHandle awaddr;
	vpiHandle awlen;
	vpiHandle awsize;
	vpiHandle awburst;

	// W Channel
	vpiHandle wvalid;
	vpiHandle wready;

	vpiHandle wdata;
	vpiHandle wlast;

	// B Channel
	vpiHandle bvalid;
	vpiHandle bready;

	vpiHandle bresp;
};

class axi_interface
{
public:
	std::string interface_name;
	int interface_width = 64;

	axi_interface_ports ports = {0,0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0, 0,0,0};

	std::queue<std::pair<std::string,int>> address_write_transactions; // awaddr and awlen
	std::queue<std::pair<int,int>> address_read_transactions; // arlen and arsize
	std::queue<std::string> data_write_transactions; // wdata
	std::queue<std::string> data_read_transactions; // rdata_packet
	std::queue<bool> response_write_transactions;

	int sim_to_hw_pipe = -1;
	int hw_to_sim_pipe = -1;

	int data_read_counter = 0;
	std::string data_read_packet;

	bool waiting_msg_read_flag = true;
	bool waiting_msg_write_flag = true;

	std::mutex mtx;

	void response_write_transaction();
	void data_read_transaction();
	void address_read_transaction();
	void data_write_transaction();
	void address_write_transaction();

	axi_interface() = default;
	axi_interface(const axi_interface&) = delete;

	axi_interface(std::string axi_interface_name) {
		interface_name = axi_interface_name;
	}
};

#endif