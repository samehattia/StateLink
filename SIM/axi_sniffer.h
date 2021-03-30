#ifndef AXI_SNIFFER_H
#define AXI_SNIFFER_H

#include <unordered_map>
#include <vector> 
#include "vpi_user.h"

#define PARAM_FILE_NAME "param.txt"

// Define AXI_SNIFFER if you want to use the AXI sniffer
#define AXI_SNIFFER

struct axi_interface {	
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

#endif
