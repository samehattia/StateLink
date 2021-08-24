#ifndef AXIS_SNIFFER_H
#define AXIS_SNIFFER_H

#include "vpi_user.h"

#define PARAM_FILE_NAME "param.txt"

struct axis_interface {	
	// Master (sender) or Slave (receiver) AXIS interface
	bool master;

	vpiHandle tdata;

	vpiHandle tkeep;
	vpiHandle tlast;

	vpiHandle tready;
	vpiHandle tvalid;
};

#endif
