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
#include <unordered_map>
#include <vector> 
#include "axis_sniffer.h"
#include "message.h"

using namespace std;

unordered_map<string,axis_interface> axis_interface_map;

queue<string> send_transactions;

#define AXIS_TX_SIM_TO_HW_PIPENAME "/tmp/axis_tx_sim_to_hw_pipe"
#define AXIS_TX_HW_TO_SIM_PIPENAME "/tmp/axis_tx_hw_to_sim_pipe"
#define AXIS_RX_SIM_TO_HW_PIPENAME "/tmp/axis_rx_sim_to_hw_pipe"
#define AXIS_RX_HW_TO_SIM_PIPENAME "/tmp/axis_rx_hw_to_sim_pipe"

int AXIS_TX_SIM_TO_HW_PIPE = -1;
int AXIS_TX_HW_TO_SIM_PIPE = -1;
int AXIS_RX_SIM_TO_HW_PIPE = -1;
int AXIS_RX_HW_TO_SIM_PIPE = -1;

queue<string> rx_transactions; //packet
queue<string> rx_transaction_length; //packet_len

string axis_rx_packet = "";

string axis_rx_hw_packet;
int axis_rx_hw_packet_length = 0;
int axis_rx_flit_counter = 0;
int AXIS_RX_FIRST_PACKET_DELAY = 45000; // minimum number of cycles before feeding the first packet
int AXIS_RX_PACKET_DELAY = 150; // minimum number of cycles between each packet
int axis_rx_packet_delay_counter = AXIS_RX_FIRST_PACKET_DELAY;

mutex axis_mtx;
mutex axis_mtx_2;

int axis_rx_counter = 0;
int axis_tx_counter = 0;

void process_axis_rx_message(string message) {

	if (message[0] == 'L') {
		axis_mtx_2.lock();
		rx_transaction_length.push(message.substr(2));
		axis_mtx_2.unlock();
	}
	// Check if the received message is READ DATA
	else if (message.find("READ DATA") != string::npos) {
		axis_mtx.lock();
		rx_transactions.push(message.substr(1 + message.find_last_of(" ")));
		axis_mtx.unlock();
	}
	else {
		cout << message << endl;
		vpi_printf( (char*)"\tInvalid message from AXIS_RX_HW_TO_SIM_PIPE\n");
	}
}

void axis_rx_thread_fn() {
	while (1) {
		recv_message(AXIS_RX_HW_TO_SIM_PIPE, process_axis_rx_message, true);
	}
}

void process_axis_tx_message(string message) {

	// Check if the received message is READ DATA
	if (message.find("READ DATA") != string::npos) {
		
	}
	else if (message.find("WRITE DATA") != string::npos) {

	}
	else {
		cout << message << endl;
		vpi_printf( (char*)"\tInvalid message from AXIS_TX_HW_TO_SIM_PIPE\n");
	}
}

void axis_rx_transaction(string axis_interface_name) {

	axis_interface axis_interface_ports = axis_interface_map[axis_interface_name];
	
	s_vpi_value current_value;
	current_value.format = vpiIntVal;

	vpi_get_value(axis_interface_ports.tlast, &current_value);
	int tlast_value = current_value.value.integer;

	current_value.format = vpiHexStrVal;

	vpi_get_value(axis_interface_ports.tdata, &current_value);
	string tdata_value = current_value.value.str;

	current_value.format = vpiBinStrVal;

	vpi_get_value(axis_interface_ports.tkeep, &current_value);
	string tkeep_value = current_value.value.str;

	axis_rx_packet = tdata_value + axis_rx_packet;

	if (axis_rx_flit_counter == 0) {
		unsigned int wait_counter = 0;
		unsigned int max_wait = numeric_limits<unsigned int>::max();

		while(rx_transactions.empty()) {
			wait_counter++;

			if (wait_counter == max_wait) {
				vpi_printf( (char*)"\tError: Extreme wait time for AXIS_RX_HW_TO_SIM_PIPE.\n");
				vpi_control(vpiFinish, 1);
			}
		}

		axis_mtx.lock();
		axis_rx_hw_packet = rx_transactions.front();
		rx_transactions.pop();
		axis_mtx.unlock();

		if (rx_transaction_length.empty()) {
			vpi_printf( (char*)"\tError: No packet length from AXIS_RX_HW_TO_SIM_PIPE.\n");
			vpi_control(vpiFinish, 1);
		}

		axis_mtx_2.lock();
		axis_rx_hw_packet_length = stoi(rx_transaction_length.front());
		rx_transaction_length.pop();
		axis_mtx_2.unlock();

		axis_rx_hw_packet = axis_rx_hw_packet.substr(0, axis_rx_hw_packet.find_first_of("\n"));
	}

	if (axis_rx_hw_packet_length > 0) {
		int flit_width = 8; // in bytes
		int flit_loc = axis_rx_hw_packet.length() - ((axis_rx_flit_counter + 1) * flit_width * 2); // in bytes
		
		string tdata_hardware_value = axis_rx_hw_packet.substr(flit_loc, flit_width * 2);

		string tkeep_hardware_value = "";
		for (int i = 0; i < flit_width; i++) {
			if (i < axis_rx_hw_packet_length) 
				tkeep_hardware_value = '1' + tkeep_hardware_value;
			else
				tkeep_hardware_value = '0' + tkeep_hardware_value;
		}

		int tlast_hardware_value = 0;
		if (axis_rx_hw_packet_length <= flit_width)
			tlast_hardware_value = 1;


		axis_rx_hw_packet_length -= flit_width;
		axis_rx_flit_counter++;

		vpi_printf( (char*)"\t\t Hardware TDATA=%s TKEEP=%s TLAST=%d\n", tdata_hardware_value.c_str(), tkeep_hardware_value.c_str(), tlast_hardware_value);
		vpi_printf( (char*)"\t\t Software TDATA=%s TKEEP=%s TLAST=%d\n", tdata_value.c_str(), tkeep_value.c_str(), tlast_value);
	}

	if (tlast_value) {
		vpi_printf( (char*)"\tAXIS Receive Transaction on %s DATA=%s\n", axis_interface_name.c_str(), axis_rx_packet.c_str());
		vpi_printf( (char*)"\tAXIS Receive Transaction on Hardware %d DATA=%s\n", axis_rx_counter, axis_rx_hw_packet.c_str());
		axis_rx_flit_counter = 0;
		axis_rx_packet = "";
	}
}

void set_signal_value(vpiHandle signal, string singal_value, bool binary_string=false) {

	s_vpi_value new_value;
	if (binary_string)
		new_value.format = vpiBinStrVal;
	else
		new_value.format = vpiHexStrVal;

	new_value.value.str = new char [singal_value.length() + 1];
	strcpy(new_value.value.str, singal_value.c_str());
	vpi_put_value(signal, &new_value, NULL, vpiNoDelay);
}

string get_signal_value(vpiHandle signal) {

	string signal_value;
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(signal, &current_value);
	signal_value = current_value.value.str;

	return signal_value;
}

bool get_binary_signal_value(vpiHandle signal) {

	string signal_value;
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(signal, &current_value);
	signal_value = current_value.value.str;

	if (signal_value == "1")
		return true;

	return false;
}

void axis_rx_hw_transaction(string axis_interface_name) {

	int flit_width = 8; // in bytes

	// new_flit is true, if the last flit is received by the DUT or new packet is received from the board
	// once true, the new flit will be put on the interface with a valid signal, and the valid signal should not be lowered until the flit is received by the DUT
	bool new_flit = false;

	axis_rx_packet_delay_counter--;
	if (axis_rx_packet_delay_counter < 0)
		axis_rx_packet_delay_counter = 0;

	axis_interface axis_interface_ports = axis_interface_map[axis_interface_name];

	// If the previous flit is received by the DUT, update the global counters
	if (get_binary_signal_value(axis_interface_ports.tvalid) && get_binary_signal_value(axis_interface_ports.tready)) {
		
		axis_rx_hw_packet_length -= flit_width;
		axis_rx_flit_counter++;

		if (get_binary_signal_value(axis_interface_ports.tlast)) {
			axis_rx_flit_counter = 0;
			axis_rx_packet_delay_counter = AXIS_RX_PACKET_DELAY;

			axis_rx_counter++;
			//vpi_printf( (char*)"\tAXIS RX Transaction %d\n", axis_rx_counter);
		}

		new_flit = true;

		//vpi_printf( (char*)"\tAXIS RX Transaction on %s TDATA=%s TKEEP=%s TLAST=%s\n", axis_interface_name.c_str(), get_signal_value(axis_interface_ports.tdata).c_str(), get_signal_value(axis_interface_ports.tkeep).c_str(), get_signal_value(axis_interface_ports.tlast).c_str());
	}

	// If this the first flit in a packet and we don't have an unconsumed packet, get a new packet from the board
	if (axis_rx_flit_counter == 0 and axis_rx_hw_packet_length <= 0) {

		// If there is no received packets, turn off the valid signal and return
		if(rx_transactions.empty() ||  axis_rx_packet_delay_counter > 0) {
			set_signal_value(axis_interface_ports.tvalid, "0");
			set_signal_value(axis_interface_ports.tlast, "0");
			return;
		}		

		axis_mtx.lock();
		axis_rx_hw_packet = rx_transactions.front();
		rx_transactions.pop();
		axis_mtx.unlock();

		if (rx_transaction_length.empty()) {
			vpi_printf( (char*)"\tError: No packet length from AXIS_RX_HW_TO_SIM_PIPE.\n");
			vpi_control(vpiFinish, 1);
		}

		axis_mtx_2.lock();
		axis_rx_hw_packet_length = stoi(rx_transaction_length.front());
		rx_transaction_length.pop();
		axis_mtx_2.unlock();

		if (axis_rx_hw_packet.find_first_of("\n") == string::npos) {
			vpi_printf( (char*)"\tError: No endline in the message received from AXIS_RX_HW_TO_SIM_PIPE. %s\n", axis_rx_hw_packet.c_str());
			vpi_control(vpiFinish, 1);
		}

		axis_rx_hw_packet = axis_rx_hw_packet.substr(0, axis_rx_hw_packet.find_first_of("\n"));

		if (axis_rx_hw_packet_length > axis_rx_hw_packet.length()/2) {
			vpi_printf( (char*)"\tError: Received Packet Length exceeds the actual length of the receieved packet from AXIS_RX_HW_TO_SIM_PIPE. Packet=%s axis_rx_hw_packet_length=%d axis_rx_hw_packet.length()=%d\n", axis_rx_hw_packet.c_str(), axis_rx_hw_packet_length, axis_rx_hw_packet.length());
			vpi_control(vpiFinish, 1);
		}

		new_flit = true;
	}

	if (new_flit) {
		string tdata_hardware_value = "";
		string tkeep_hardware_value = "";
		string tlast_hardware_value = "";

		// Get a flit (TDATA, TKEEP, TLAST) from the received packet
		if (axis_rx_hw_packet_length > 0) {
			int flit_loc = axis_rx_hw_packet.length() - ((axis_rx_flit_counter + 1) * flit_width * 2); // in bytes
			if (flit_loc > axis_rx_hw_packet.length()) {
				vpi_printf( (char*)"\tError: Incorrect flit in the message received from AXIS_RX_HW_TO_SIM_PIPE. Packet=%s axis_rx_hw_packet_length=%d axis_rx_flit_counter=%d flit_loc=%d axis_rx_hw_packet.length()=%d\n", axis_rx_hw_packet.c_str(), axis_rx_hw_packet_length, axis_rx_flit_counter, flit_loc, axis_rx_hw_packet.length());
				vpi_control(vpiFinish, 1);
			}
			tdata_hardware_value = axis_rx_hw_packet.substr(flit_loc, flit_width * 2);

			for (int i = 0; i < flit_width; i++) {
				if (i < axis_rx_hw_packet_length) 
					tkeep_hardware_value = '1' + tkeep_hardware_value;
				else
					tkeep_hardware_value = '0' + tkeep_hardware_value;
			}

			if (axis_rx_hw_packet_length <= flit_width)
				tlast_hardware_value = "1";
			else
				tlast_hardware_value = "0";
		}

		// Place a flit on the interface
		set_signal_value(axis_interface_ports.tdata, tdata_hardware_value);
		set_signal_value(axis_interface_ports.tkeep, tkeep_hardware_value, true);
		set_signal_value(axis_interface_ports.tlast, tlast_hardware_value);
		set_signal_value(axis_interface_ports.tvalid, "1");	
	}
	
}

void axis_tx_transaction(string axis_interface_name) {

	axis_interface axis_interface_ports = axis_interface_map[axis_interface_name];
	
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(axis_interface_ports.tdata, &current_value);
	string tdata_value = current_value.value.str;

	vpi_get_value(axis_interface_ports.tkeep, &current_value);
	string tkeep_value = current_value.value.str;

	current_value.format = vpiIntVal;

	vpi_get_value(axis_interface_ports.tlast, &current_value);
	int tlast_value = current_value.value.integer;

	send_transactions.push(tdata_value);	

	if (tlast_value) {
		string tdata_packet = "";
		while (!send_transactions.empty()) {
			tdata_packet = send_transactions.front() + tdata_packet;
			send_transactions.pop();
		}

		// Packet length in bytes
		int packet_length = tdata_packet.length()/2;

		for (int i = 0; i < tkeep_value.length(); i++) {
			if (tkeep_value[i] == '0') {
				packet_length -= 4;
			} 
			else if (tkeep_value[i] == '1') {
				packet_length -= 3;
			}
			else if (tkeep_value[i] == '3') {
				packet_length -= 2;
			}
			else if (tkeep_value[i] == '7') {
				packet_length -= 1;
			}
			else {
				break;
			}
		}

		// Message: W Len Data
		string message = "W " + to_string(packet_length) + " " + tdata_packet;
		send_message(AXIS_TX_SIM_TO_HW_PIPE, message);

		recv_message(AXIS_TX_HW_TO_SIM_PIPE, process_axis_tx_message, true);
		//vpi_printf( (char*)"\tAXIS TX Transaction on %s: %s\n", axis_interface_name.c_str(), message.c_str());
		axis_tx_counter++;
		//vpi_printf( (char*)"\tAXIS TX Transaction %d\n", axis_tx_counter);
	}
}

bool check_axis_active_channel(vpiHandle valid_signal, vpiHandle ready_signal) {

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

std::chrono::duration<double, milli> axis_sniffer_duration;
int clock_counter = 0;

PLI_INT32 axis_sniffer(p_cb_data cb_data) {

	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

	string axis_interface_name = (char*)(cb_data->user_data);
	axis_interface axis_interface_ports = axis_interface_map[axis_interface_name];

	// if not a positive edge, we should abort
	if (!cb_data->value->value.integer) {
		// For RX interfaces, get packets from the board and feed them to the interface
		if (!axis_interface_ports.master) {
			axis_rx_hw_transaction(axis_interface_name);
		}
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		std::chrono::duration<double, milli> fp_ms = t2 - t1;
		axis_sniffer_duration += fp_ms;
		return 0;
	}

	// Check transactions
	if (check_axis_active_channel(axis_interface_ports.tvalid, axis_interface_ports.tready)) {
		if (axis_interface_ports.master) {
			axis_tx_transaction(axis_interface_name);
		}
		else {
			//axis_rx_transaction(axis_interface_name);
		}
	}

	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	std::chrono::duration<double, milli> fp_ms = t2 - t1;
	axis_sniffer_duration += fp_ms;

	clock_counter++;
	if (clock_counter == 100000) {
		vpi_printf( (char*)"AXIS  %f ms\n", axis_sniffer_duration.count());
		clock_counter = 0;
	}

	return 0;
}

PLI_INT32 setup_axis_sniffer(p_cb_data cb_data) {

	string module_name;
	fstream fs;
	vpiHandle mod_handle, port_handle, portbit_handle, net_handle, clk_handle;
	int error_code;
	s_vpi_error_info error_info;
	axis_interface empty_interface = {false, 0,0,0,0,0};

	vpi_printf( (char*)"\n======================================\n" );
	vpi_printf( (char*)"AXIS Sniffer" );
	vpi_printf( (char*)"\n======================================\n" );

	// open parameters file for read
	fs.open(PARAM_FILE_NAME, fstream::in);
	if (!fs.is_open()) 
		vpi_printf( (char*)"File not found\n");
	
	// read module name
	fs >> module_name;

	// read StateLink parameters which are preceded by "#StateLink"
	// current parameters: AXIS_RX_FIRST_PACKET_DELAY, AXIS_RX_PACKET_DELAY
	/*string input_string;
	int input_integer;
	while(fs.peek()!= EOF) {
		fs >> input_string;
		if (input_string == "#StateLink") {
			fs >> input_integer;
			AXIS_RX_FIRST_PACKET_DELAY = input_integer;
			axis_rx_packet_delay_counter = AXIS_RX_FIRST_PACKET_DELAY;
			fs >> input_integer;
			AXIS_RX_PACKET_DELAY = input_integer;
		}
	}*/

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
			if (port_name.find("axis_clk") != string::npos || port_name.find("AXIS_CLK") != string::npos) {
				// Get the internal net connected to that port
				clk_handle = vpi_handle(vpiLowConn, port_handle);

				vpi_printf( (char*)"  Clock Found: %s\n", port_name.c_str());
			}

			// Check if the port is part of an AXIS interface
			else if (port_name.find("_T") != string::npos || port_name.find("_t") != string::npos) {

				int pos = port_name.find_last_of('_');
				string axis_interface_name = port_name.substr(0,pos);
				string axis_interface_port = port_name.substr(pos+1);				
				std::transform(axis_interface_port.begin(), axis_interface_port.end(), axis_interface_port.begin(), [](unsigned char c){ return std::tolower(c); });

				// Get the internal net connected to that port
				net_handle = vpi_handle(vpiLowConn, port_handle);

				// For multi-bit ports, we cannot get the net connected to te port directly
				if (!net_handle) {
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
				if (axis_interface_map.find(axis_interface_name) == axis_interface_map.end()) {
					axis_interface_map.emplace(axis_interface_name, empty_interface);
					vpi_printf( (char*)"  AXIS Interface Found: %s\n", axis_interface_name.c_str());
				}

				if (axis_interface_port == "tdata") {
					axis_interface_map[axis_interface_name].tdata = net_handle;
					// Check if it is an input or output port to set master flag
					if (vpi_get(vpiDirection, port_handle) == vpiOutput)
						axis_interface_map[axis_interface_name].master = true;
					else
						axis_interface_map[axis_interface_name].master = false;
				}
				else if (axis_interface_port == "tvalid") 
					axis_interface_map[axis_interface_name].tvalid = net_handle;
				else if (axis_interface_port == "tready") 
					axis_interface_map[axis_interface_name].tready = net_handle;
				else if (axis_interface_port == "tlast") 
					axis_interface_map[axis_interface_name].tlast = net_handle;
				else if (axis_interface_port == "tkeep") 
					axis_interface_map[axis_interface_name].tkeep = net_handle;
			}

			// get the next port
			port_handle = vpi_scan(port_itr_handle);
			if ((error_code = vpi_chk_error(&error_info)) && error_info.message)
				vpi_printf( (char*)"  %s\n", error_info.message);
		}

		// We can either register a callback using vpi_register_cb()
		// or a system task using vpi_register_systf() that can be called from Verilog (has to be called after all events are processed (using cbReadWriteSynch??))
		// The callback can be called due to a specific signal value change or every clock cycle using cbValueChange (Do we need cbReadWriteSynch ??)
		for (auto& axis_interface_entry: axis_interface_map) {
			s_cb_data cb_clk;
			s_vpi_value cb_value_s;
			s_vpi_time cb_time_s;

			// get a handle for save signal, and set the callback function save_state 
			cb_clk.reason = cbValueChange;
			cb_clk.cb_rtn = axis_sniffer;
			cb_clk.obj = clk_handle; //axis_interface_entry.second.awvalid;
			cb_clk.value = &cb_value_s;
			cb_clk.time = &cb_time_s;
			cb_time_s.type = vpiSuppressTime;
			cb_value_s.format = vpiIntVal;
			cb_clk.user_data = (PLI_BYTE8*)(axis_interface_entry.first.c_str());
			vpi_register_cb(&cb_clk);
		}
	}

	AXIS_TX_SIM_TO_HW_PIPE = setup_send_channel(AXIS_TX_SIM_TO_HW_PIPENAME);
	AXIS_TX_HW_TO_SIM_PIPE = setup_recv_channel(AXIS_TX_HW_TO_SIM_PIPENAME);

	AXIS_RX_SIM_TO_HW_PIPE = setup_send_channel(AXIS_RX_SIM_TO_HW_PIPENAME);
	AXIS_RX_HW_TO_SIM_PIPE = setup_recv_channel(AXIS_RX_HW_TO_SIM_PIPENAME);

	thread axis_rx_thread = thread(axis_rx_thread_fn);
	axis_rx_thread.detach();
	vpi_printf( (char*)"\n======================================\n" );

	return 0;
}

void axis_sniffer_register(void) {
	s_cb_data cb_data_s;
	vpiHandle callback_handle;

	cb_data_s.reason = cbEndOfCompile;
	cb_data_s.cb_rtn = setup_axis_sniffer;
	cb_data_s.obj = NULL;
	cb_data_s.time = NULL;
	cb_data_s.value = NULL;
	cb_data_s.user_data = NULL;
	callback_handle = vpi_register_cb(&cb_data_s);
	vpi_free_object(callback_handle);
}