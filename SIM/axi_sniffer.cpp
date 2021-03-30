#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <queue>
#include <stack>
#include <utility>
#include <thread>
#include <mutex>
#include <limits>
#include <chrono>
#include "axi_sniffer.h"

#include <stdio.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <unistd.h> 

using namespace std;

unordered_map<string,axi_interface> axi_interface_map;

queue<pair<string,int>> address_write_transactions; // awaddr and awlen
queue<pair<int,int>> address_read_transactions; // arlen and arsize
queue<string> data_write_transactions; // wdata
queue<string> data_read_transactions; // rdata_packet
queue<bool> response_write_transactions;

#define SIM_TO_HW_PIPENAME "/tmp/sim_to_hw_pipe"
#define HW_TO_SIM_PIPENAME "/tmp/hw_to_sim_pipe"

#define MAX_RECV_MSG_LEN 1000
#define JTAG_AXI_TO_TASK_AXI_WIDTH_RATIO 2 

int SIM_TO_HW_PIPE = -1;
int HW_TO_SIM_PIPE = -1;

int AXI_DATA_READ_COUNTER = 0;
string AXI_DATA_READ_PACKET;

bool HW_TO_SIM_PIPE_WAITING_MSG_READ_FLAG = true;
bool HW_TO_SIM_PIPE_WAITING_MSG_WRITE_FLAG = true;

mutex mtx;

void setup_recv_pipe() {
	// Create FIFO
	mkfifo(HW_TO_SIM_PIPENAME, 0666);

	// Open FIFO for read only
	vpi_printf( (char*)"  Opening HW_TO_SIM_PIPE\n");
	HW_TO_SIM_PIPE = open(HW_TO_SIM_PIPENAME, O_RDONLY);
}

void setup_send_pipe() {
	// Create FIFO
	mkfifo(SIM_TO_HW_PIPENAME, 0666); 

	// Open FIFO for write only
	vpi_printf( (char*)"  Opening SIM_TO_HW_PIPE\n");
	SIM_TO_HW_PIPE = open(SIM_TO_HW_PIPENAME, O_WRONLY);
}

void recv_message_thread() {

	char message_c_str[MAX_RECV_MSG_LEN + 1];

	int bytes_read = read(HW_TO_SIM_PIPE, message_c_str, MAX_RECV_MSG_LEN);

	if (bytes_read == 0) {
		vpi_printf( (char*)"\tHW_TO_SIM_PIPE is not open\n");
	} 
	else {
		// Check if the received message is READ DATA
		string message = message_c_str;
		if (message.find("READ DATA") != string::npos) {
			mtx.lock();
			data_read_transactions.push(message.substr(1 + message.find_last_of(" ")));
			mtx.unlock();
		}
		else if (message.find("WRITE DATA") != string::npos) {
			mtx.lock();
			response_write_transactions.push(true);
			mtx.unlock();
		}
		else {
			cout << message << endl;
			vpi_printf( (char*)"\tInvalid message from HW_TO_SIM_PIPE\n");
		}
	}
}

void recv_message() {
	if (HW_TO_SIM_PIPE == -1)
		return;

	// Reading from the pipe is a blocking call,
	// Run it in a thread in order not to block the simulation, 
	thread recv_thread = thread(recv_message_thread);
	recv_thread.detach();
}

void send_message(string message) {
	if (SIM_TO_HW_PIPE == -1)
		return;

	write(SIM_TO_HW_PIPE, message.c_str(), message.length()+1);
}

void response_write_transaction(string axi_interface_name) {
	unsigned int wait_counter = 0;
	unsigned int max_wait = numeric_limits<unsigned int>::max();

	while(response_write_transactions.empty()) {

		if (HW_TO_SIM_PIPE_WAITING_MSG_WRITE_FLAG) {
			vpi_printf( (char*)"\tWaiting for HW_TO_SIM_PIPE write response (This message is printed once)\n");
			HW_TO_SIM_PIPE_WAITING_MSG_WRITE_FLAG = false;
		}

		wait_counter++;
		if (wait_counter == max_wait) {
			vpi_printf( (char*)"\tError: Extreme wait time for HW_TO_SIM_PIPE write response. Exiting\n");
			vpi_control(vpiFinish, 1);
		}
	}

	mtx.lock();
	response_write_transactions.pop();
	mtx.unlock();
}

void data_read_transaction(string axi_interface_name) {

	axi_interface axi_interface_ports = axi_interface_map[axi_interface_name];
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(axi_interface_ports.rdata, &current_value);
	string rdata_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(axi_interface_ports.rlast, &current_value);
	int rlast_value = current_value.value.integer;

	// TODO: Since the queue is not thread safe and since we are alreadg waiting here for the read data
	// the recv_message should be called here with no threads.
	// But in this case, the jtag read in tcl should be done in a separate thread
	// Otherwise, the tcl may block until we read the data here
	
	// If this is the first data beat, read the entire data packet from the board
	if (AXI_DATA_READ_COUNTER == 0) {
		unsigned int wait_counter = 0;
		unsigned int max_wait = numeric_limits<unsigned int>::max();

		while(data_read_transactions.empty()) {

			if (HW_TO_SIM_PIPE_WAITING_MSG_READ_FLAG) {
				vpi_printf( (char*)"\tWaiting for HW_TO_SIM_PIPE read response (This message is printed once)\n");
				HW_TO_SIM_PIPE_WAITING_MSG_READ_FLAG = false;
			}

			wait_counter++;
			if (wait_counter == max_wait) {
				vpi_printf( (char*)"\tError: Extreme wait time for HW_TO_SIM_PIPE read response. Exiting\n");
				vpi_control(vpiFinish, 1);
			}
		}

		mtx.lock();
		AXI_DATA_READ_PACKET = data_read_transactions.front();
		data_read_transactions.pop();
		mtx.unlock();
	}

	int arlen_value = address_read_transactions.front().first;
	int arsize_value = address_read_transactions.front().second;

	int data_beat_size = (1 << (arsize_value)) * 2; // number of bytes per beat * number of ASCII characters per data byte
	int data_beat_loc = (arlen_value + 1 - AXI_DATA_READ_COUNTER - 1) * data_beat_size;
	
	string rdata_hardware_value = AXI_DATA_READ_PACKET.substr(data_beat_loc, data_beat_size);
	
	s_vpi_value hardware_value;
	hardware_value.format = vpiHexStrVal;

	hardware_value.value.str = new char [rdata_hardware_value.length() + 1];
	strcpy(hardware_value.value.str, rdata_hardware_value.c_str());
	vpi_put_value(axi_interface_ports.rdata, &hardware_value, NULL, vpiNoDelay);

	AXI_DATA_READ_COUNTER++;

	// If this is the last data beat, then the next beat is the start of a new packet
	if (rlast_value) { 
		AXI_DATA_READ_COUNTER = 0;
		address_read_transactions.pop();
	}

	//vpi_printf( (char*)"\tAXI R Transaction on %s: DATA=%s HARDWARE_DATA=%s LAST=%d\n", axi_interface_name.c_str(), rdata_value.c_str(), rdata_hardware_value.c_str(), rlast_value);
}

void address_read_transaction(string axi_interface_name) {

	axi_interface axi_interface_ports = axi_interface_map[axi_interface_name];
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(axi_interface_ports.araddr, &current_value);
	string araddr_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(axi_interface_ports.arlen, &current_value);
	int arlen_value = current_value.value.integer;

	vpi_get_value(axi_interface_ports.arsize, &current_value);
	int arsize_value = current_value.value.integer;

	vpi_get_value(axi_interface_ports.arburst, &current_value);
	int arburst_value = current_value.value.integer;

	address_read_transactions.push(make_pair(arlen_value, arsize_value));

	// Message: R Address Len
	string message = "R " + araddr_value + " " + to_string((arlen_value+1)/JTAG_AXI_TO_TASK_AXI_WIDTH_RATIO) + " ";
	send_message(message);

	recv_message();

	//vpi_printf( (char*)"\tAXI AR Transaction on %s: ADDR=%s LEN=%d SIZE=%d BURST=%d\n", axi_interface_name.c_str(), araddr_value.c_str(), arlen_value, arsize_value, arburst_value);
}

void data_write_transaction(string axi_interface_name) {

	axi_interface axi_interface_ports = axi_interface_map[axi_interface_name];
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(axi_interface_ports.wdata, &current_value);
	string wdata_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(axi_interface_ports.wlast, &current_value);
	int wlast_value = current_value.value.integer;

	data_write_transactions.push(wdata_value);

	if (wlast_value == 1) {
		// Message: W Address Len Data
		string message = "W " + address_write_transactions.front().first + " " + to_string((address_write_transactions.front().second+1)/JTAG_AXI_TO_TASK_AXI_WIDTH_RATIO) + " ";
		address_write_transactions.pop();

		string wdata_packet = "";
		while (!data_write_transactions.empty()) {
			wdata_packet = data_write_transactions.front() + wdata_packet;
			data_write_transactions.pop();
		}

		message = message + wdata_packet;
		send_message(message);

		recv_message();
	}

	//vpi_printf( (char*)"\tAXI W Transaction on %s: DATA=%s LAST=%d\n", axi_interface_name.c_str(), wdata_value.c_str(), wlast_value);
}

void address_write_transaction(string axi_interface_name) {

	axi_interface axi_interface_ports = axi_interface_map[axi_interface_name];
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(axi_interface_ports.awaddr, &current_value);
	string awaddr_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(axi_interface_ports.awlen, &current_value);
	int awlen_value = current_value.value.integer;

	vpi_get_value(axi_interface_ports.awsize, &current_value);
	int awsize_value = current_value.value.integer;

	vpi_get_value(axi_interface_ports.awburst, &current_value);
	int awburst_value = current_value.value.integer;

	address_write_transactions.push(make_pair(awaddr_value, awlen_value));

	//vpi_printf( (char*)"\tAXI AW Transaction on %s: ADDR=%s LEN=%d SIZE=%d BURST=%d\n", axi_interface_name.c_str(), awaddr_value.c_str(), awlen_value, awsize_value, awburst_value);
}

bool check_active_channel(vpiHandle valid_signal, vpiHandle ready_signal) {

	string valid_value, ready_value;
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(valid_signal, &current_value);
	valid_value = current_value.value.str;

	vpi_get_value(ready_signal, &current_value);
	ready_value = current_value.value.str;

	if (valid_value == "1" && ready_value == "1")
		return true;

	return false;
}

PLI_INT32 axi_sniffer(p_cb_data cb_data) {

	string axi_interface_name = (char*)(cb_data->user_data);
	axi_interface axi_interface_ports = axi_interface_map[axi_interface_name];

	// if not a positive edge, we should abort
	if (!cb_data->value->value.integer) {
		// In order to have the read response ready by the next positive clock edge
		// we are going to process the read response here
		if (check_active_channel(axi_interface_ports.rvalid, axi_interface_ports.rready)) {
			data_read_transaction(axi_interface_name);
		} 
		else if (check_active_channel(axi_interface_ports.bvalid, axi_interface_ports.bready)) {
			response_write_transaction(axi_interface_name);
		}
		return 0;
	}

	// Check transactions on AW channel
	if (check_active_channel(axi_interface_ports.awvalid, axi_interface_ports.awready)) {
		address_write_transaction(axi_interface_name);
	}
	else if (check_active_channel(axi_interface_ports.arvalid, axi_interface_ports.arready)) {
		address_read_transaction(axi_interface_name);
	} 
	else if (check_active_channel(axi_interface_ports.wvalid, axi_interface_ports.wready)) {
		data_write_transaction(axi_interface_name);
	}

	return 0;
}

PLI_INT32 setup_axi_sniffer(p_cb_data cb_data) {

	string module_name;
	fstream fs;
	vpiHandle mod_handle, port_handle, portbit_handle, net_handle, clk_handle;
	int error_code;
	s_vpi_error_info error_info;
	axi_interface empty_interface = {0,0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0, 0,0,0};

	vpi_printf( (char*)"\n======================================\n" );
	vpi_printf( (char*)"AXI Sniffer" );
	vpi_printf( (char*)"\n======================================\n" );

	// open parameters file for read
	fs.open(PARAM_FILE_NAME, fstream::in);
	if (!fs.is_open()) 
		vpi_printf( (char*)"File not found\n");
	
	// read module name
	fs >> module_name;

	// close parameters file
	fs.close();

	mod_handle = vpi_handle_by_name((char*)module_name.c_str(), 0);
	if ((error_code = vpi_chk_error(&error_info)) && error_info.message)
		vpi_printf( (char*)"  %s\n", error_info.message);

	// get an iterator on ports of this module
	vpiHandle port_itr_handle = vpi_iterate(vpiPort, mod_handle);
	if ((error_code = vpi_chk_error(&error_info)) && error_info.message)
		vpi_printf( (char*)"  %s\n", error_info.message);

	if (!error_code && port_itr_handle) {
		// get a port handle
		port_handle = vpi_scan(port_itr_handle);
		if ((error_code = vpi_chk_error(&error_info)) && error_info.message)
			vpi_printf( (char*)"  %s\n", error_info.message);

		while (port_handle) {
			string port_name = vpi_get_str(vpiName, port_handle);

			// Check if it is a clock port
			if (port_name.find("clk") != string::npos || port_name.find("CLK") != string::npos) {
				// Get the internal net connected to that port
				clk_handle = vpi_handle(vpiLowConn, port_handle);

				vpi_printf( (char*)"  Clock Found: %s\n", port_name.c_str());
			}

			// Check if the port is part of an AXI interface
			else if (port_name.find("AXI") != string::npos || port_name.find("axi") != string::npos) {

				int pos = port_name.find_last_of('_');
				string axi_interface_name = port_name.substr(0,pos);
				string axi_interface_port = port_name.substr(pos+1);				
				std::transform(axi_interface_port.begin(), axi_interface_port.end(), axi_interface_port.begin(), [](unsigned char c){ return std::tolower(c); });

				if (vpi_get(vpiSize, port_handle) == 1) {
					// Get the internal net connected to that port
					net_handle = vpi_handle(vpiLowConn, port_handle);
				} 
				else {
					// For multi-bit ports, we cannot get the net connected to te port directly
					// Get an iterator of bits of the port
					vpiHandle portbit_itr_handle = vpi_iterate(vpiBit, port_handle);

					// Get the first portbit in that port
					portbit_handle = vpi_scan(portbit_itr_handle);

					// Get the internal netbit connected to that portbit
					net_handle = vpi_handle(vpiLowConn, portbit_handle);

					// Get the parent of this netbit (i.e. the multi-bit net connected to the port)
					net_handle = vpi_handle(vpiParent, net_handle);
				}

				// First time
				if (axi_interface_map.find(axi_interface_name) == axi_interface_map.end()) {
					axi_interface_map.emplace(axi_interface_name, empty_interface);
					vpi_printf( (char*)"  AXI Interface Found: %s\n", axi_interface_name.c_str());
				}

				if (axi_interface_port == "araddr") 
					axi_interface_map[axi_interface_name].araddr = net_handle;
				else if (axi_interface_port == "arvalid") 
					axi_interface_map[axi_interface_name].arvalid = net_handle;
				else if (axi_interface_port == "arready") 
					axi_interface_map[axi_interface_name].arready = net_handle;
				else if (axi_interface_port == "arlen") 
					axi_interface_map[axi_interface_name].arlen = net_handle;
				else if (axi_interface_port == "arsize") 
					axi_interface_map[axi_interface_name].arsize = net_handle;
				else if (axi_interface_port == "arburst") 
					axi_interface_map[axi_interface_name].arburst = net_handle;

				else if (axi_interface_port == "rdata") 
					axi_interface_map[axi_interface_name].rdata = net_handle;
				else if (axi_interface_port == "rvalid") 
					axi_interface_map[axi_interface_name].rvalid = net_handle;
				else if (axi_interface_port == "rready") 
					axi_interface_map[axi_interface_name].rready = net_handle;
				else if (axi_interface_port == "rlast") 
					axi_interface_map[axi_interface_name].rlast = net_handle;
				else if (axi_interface_port == "rresp") 
					axi_interface_map[axi_interface_name].rresp = net_handle;

				else if (axi_interface_port == "awaddr")
					axi_interface_map[axi_interface_name].awaddr = net_handle;
				else if (axi_interface_port == "awvalid") 
					axi_interface_map[axi_interface_name].awvalid = net_handle;
				else if (axi_interface_port == "awready") 
					axi_interface_map[axi_interface_name].awready = net_handle;
				else if (axi_interface_port == "awlen") 
					axi_interface_map[axi_interface_name].awlen = net_handle;
				else if (axi_interface_port == "awsize") 
					axi_interface_map[axi_interface_name].awsize = net_handle;
				else if (axi_interface_port == "awburst") 
					axi_interface_map[axi_interface_name].awburst = net_handle;

				else if (axi_interface_port == "wdata") 
					axi_interface_map[axi_interface_name].wdata = net_handle;
				else if (axi_interface_port == "wvalid") 
					axi_interface_map[axi_interface_name].wvalid = net_handle;
				else if (axi_interface_port == "wready") 
					axi_interface_map[axi_interface_name].wready = net_handle;
				else if (axi_interface_port == "wlast") 
					axi_interface_map[axi_interface_name].wlast = net_handle;

				else if (axi_interface_port == "bresp") 
					axi_interface_map[axi_interface_name].bresp = net_handle;
				else if (axi_interface_port == "bvalid") 
					axi_interface_map[axi_interface_name].bvalid = net_handle;
				else if (axi_interface_port == "bready") 
					axi_interface_map[axi_interface_name].bready = net_handle;
			}

			// get the next port
			port_handle = vpi_scan(port_itr_handle);
			if ((error_code = vpi_chk_error(&error_info)) && error_info.message)
				vpi_printf( (char*)"  %s\n", error_info.message);
		}

		// We can either register a callback using vpi_register_cb()
		// or a system task using vpi_register_systf() that can be called from Verilog (has to be called after all events are processed (using cbReadWriteSynch??))
		// The callback can be called due to a specific signal value change or every clock cycle using cbValueChange (Do we need cbReadWriteSynch ??)
		for (auto& axi_interface_entry: axi_interface_map) {
			s_cb_data cb_clk;
			s_vpi_value cb_value_s;
			s_vpi_time cb_time_s;

			// get a handle for save signal, and set the callback function save_state 
			cb_clk.reason = cbValueChange;
			cb_clk.cb_rtn = axi_sniffer;
			cb_clk.obj = clk_handle; //axi_interface_entry.second.awvalid;
			cb_clk.value = &cb_value_s;
			cb_clk.time = &cb_time_s;
			cb_time_s.type = vpiSuppressTime;
			cb_value_s.format = vpiIntVal;
			cb_clk.user_data = (PLI_BYTE8*)(axi_interface_entry.first.c_str());
			vpi_register_cb(&cb_clk);
		}
	}

	setup_send_pipe();
	setup_recv_pipe();
	vpi_printf( (char*)"\n======================================\n" );

	return 0;
}

void axi_sniffer_register(void) {
	s_cb_data cb_data_s;
	vpiHandle callback_handle;

	cb_data_s.reason = cbEndOfCompile;
	cb_data_s.cb_rtn = setup_axi_sniffer;
	cb_data_s.obj = NULL;
	cb_data_s.time = NULL;
	cb_data_s.value = NULL;
	cb_data_s.user_data = NULL;
	callback_handle = vpi_register_cb(&cb_data_s);
	vpi_free_object(callback_handle);
}