#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <utility>
#include <thread>
#include <limits>
#include "axi_interface.h"
#include "axi_sniffer.h"
#include "message.h"

void process_axi_message(std::string message, void* user_data) {

	axi_interface* axi_intf = (axi_interface*) user_data;

	// Check if the received message is READ DATA
	if (message.find("READ DATA") != std::string::npos) {
		axi_intf->mtx.lock();
		axi_intf->data_read_transactions.push(message.substr(1 + message.find_last_of(" ")));
		axi_intf->mtx.unlock();
	}
	else if (message.find("WRITE DATA") != std::string::npos) {
		axi_intf->mtx.lock();
		axi_intf->response_write_transactions.push(true);
		axi_intf->mtx.unlock();
	}
	else {
		vpi_printf( (char*)"\tInvalid message from AXI_HW_TO_SIM_PIPE: %s\n", message.c_str());
	}
}

void axi_interface::response_write_transaction() {

	unsigned int wait_counter = 0;
	unsigned int max_wait = std::numeric_limits<unsigned int>::max();

	while(response_write_transactions.empty()) {

		if (waiting_msg_write_flag) {
			vpi_printf( (char*)"\tWaiting for AXI_HW_TO_SIM_PIPE write response (This message is printed once)\n");
			waiting_msg_write_flag = false;
		}

		wait_counter++;
		if (wait_counter == max_wait) {
			s_vpi_time current_time;
			current_time.type = vpiScaledRealTime;
			vpi_get_time(ports.bvalid, &current_time);
			vpi_printf( (char*)"\tError: Extreme wait time for AXI_HW_TO_SIM_PIPE write response @ time %2.2f. Exiting\n", current_time.real);
			vpi_control(vpiFinish, 1);
		}
	}

	mtx.lock();
	response_write_transactions.pop();
	mtx.unlock();
}

void axi_interface::data_read_transaction() {

	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(ports.rdata, &current_value);
	std::string rdata_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(ports.rlast, &current_value);
	int rlast_value = current_value.value.integer;

	// TODO: Since the queue is not thread safe and since we are alreadg waiting here for the read data
	// the recv_message should be called here with no threads.
	// But in this case, the jtag read in tcl should be done in a separate thread
	// Otherwise, the tcl may block until we read the data here
	
	// If this is the first data beat, read the entire data packet from the board
	if (data_read_counter == 0) {
		unsigned int wait_counter = 0;
		unsigned int max_wait = std::numeric_limits<unsigned int>::max();

		while(data_read_transactions.empty()) {

			if (waiting_msg_read_flag) {
				vpi_printf( (char*)"\tWaiting for AXI_HW_TO_SIM_PIPE read response (This message is printed once)\n");
				waiting_msg_read_flag = false;
			}

			wait_counter++;
			if (wait_counter == max_wait) {
				vpi_printf( (char*)"\tError: Extreme wait time for AXI_HW_TO_SIM_PIPE read response. Exiting\n");
				vpi_control(vpiFinish, 1);
			}
		}

		mtx.lock();
		data_read_packet = data_read_transactions.front();
		data_read_transactions.pop();
		mtx.unlock();
	}

	int arlen_value = address_read_transactions.front().first;
	int arsize_value = address_read_transactions.front().second;

	int data_beat_size = (1 << (arsize_value)) * 2; // number of bytes per beat * number of ASCII characters per data byte
	int data_beat_loc = (arlen_value + 1 - data_read_counter - 1) * data_beat_size;
	
	std::string rdata_hardware_value = data_read_packet.substr(data_beat_loc, data_beat_size);
	
	s_vpi_value hardware_value;
	hardware_value.format = vpiHexStrVal;

	hardware_value.value.str = new char [rdata_hardware_value.length() + 1];
	strcpy(hardware_value.value.str, rdata_hardware_value.c_str());
	vpi_put_value(ports.rdata, &hardware_value, NULL, vpiNoDelay);

	data_read_counter++;

	// If this is the last data beat, then the next beat is the start of a new packet
	if (rlast_value) { 
		data_read_counter = 0;
		address_read_transactions.pop();
	}

	//vpi_printf( (char*)"\tAXI R Transaction on %s: DATA=%s HARDWARE_DATA=%s LAST=%d\n", interface_name.c_str(), rdata_value.c_str(), rdata_hardware_value.c_str(), rlast_value);
}

void axi_interface::address_read_transaction() {
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(ports.araddr, &current_value);
	std::string araddr_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(ports.arlen, &current_value);
	int arlen_value = current_value.value.integer;

	vpi_get_value(ports.arsize, &current_value);
	int arsize_value = current_value.value.integer;

	vpi_get_value(ports.arburst, &current_value);
	int arburst_value = current_value.value.integer;

	address_read_transactions.push(std::make_pair(arlen_value, arsize_value));

	// Message: R Address Len
	std::string message = "R " + araddr_value + " " + std::to_string(int((arlen_value+1) * interface_width / JTAG_AXI_WIDTH)) + " ";
	send_message(sim_to_hw_pipe, message);

	recv_message(hw_to_sim_pipe, process_axi_message, (void *) this);

	//vpi_printf( (char*)"\tAXI AR Transaction on %s: ADDR=%s LEN=%d SIZE=%d BURST=%d\n", interface_name.c_str(), araddr_value.c_str(), arlen_value, arsize_value, arburst_value);
}

void axi_interface::data_write_transaction() {
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(ports.wdata, &current_value);
	std::string wdata_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(ports.wlast, &current_value);
	int wlast_value = current_value.value.integer;

	data_write_transactions.push(wdata_value);

	if (wlast_value == 1) {
		if (address_write_transactions.empty()) {
			vpi_printf( (char*)"\tError: No AXI Address Specified. Exiting\n");
			vpi_control(vpiFinish, 1);
		}

		// Message: W Address Len Data
		std::string message = "W " + address_write_transactions.front().first + " " + std::to_string(int((address_write_transactions.front().second+1) * interface_width / JTAG_AXI_WIDTH)) + " ";
		address_write_transactions.pop();

		std::string wdata_packet = "";
		while (!data_write_transactions.empty()) {
			wdata_packet = data_write_transactions.front() + wdata_packet;
			data_write_transactions.pop();
		}

		message = message + wdata_packet;
		send_message(sim_to_hw_pipe, message);

		recv_message(hw_to_sim_pipe, process_axi_message, (void *) this, true);
	}

	//vpi_printf( (char*)"\tAXI W Transaction on %s: DATA=%s LAST=%d\n", interface_name.c_str(), wdata_value.c_str(), wlast_value);
}

void axi_interface::address_write_transaction() {
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(ports.awaddr, &current_value);
	std::string awaddr_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(ports.awlen, &current_value);
	int awlen_value = current_value.value.integer;

	vpi_get_value(ports.awsize, &current_value);
	int awsize_value = current_value.value.integer;

	vpi_get_value(ports.awburst, &current_value);
	int awburst_value = current_value.value.integer;

	address_write_transactions.push(std::make_pair(awaddr_value, awlen_value));

	//vpi_printf( (char*)"\tAXI AW Transaction on %s: ADDR=%s LEN=%d SIZE=%d BURST=%d\n", interface_name.c_str(), awaddr_value.c_str(), awlen_value, awsize_value, awburst_value);
}