#include <iostream>
#include <cstring>
#include "utils.h"

void set_signal_value(vpiHandle signal, std::string singal_value, bool binary_string) {

	s_vpi_value new_value;
	if (binary_string)
		new_value.format = vpiBinStrVal;
	else
		new_value.format = vpiHexStrVal;

	new_value.value.str = new char [singal_value.length() + 1];
	strcpy(new_value.value.str, singal_value.c_str());
	vpi_put_value(signal, &new_value, NULL, vpiNoDelay);
}

std::string get_signal_value(vpiHandle signal) {

	std::string signal_value;
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(signal, &current_value);
	signal_value = current_value.value.str;

	return signal_value;
}

bool get_binary_signal_value(vpiHandle signal) {

	std::string signal_value;
	s_vpi_value current_value;
	current_value.format = vpiHexStrVal;

	vpi_get_value(signal, &current_value);
	signal_value = current_value.value.str;

	if (signal_value == "1")
		return true;

	return false;
}

bool check_active_channel(vpiHandle valid_signal, vpiHandle ready_signal) {

	std::string valid_value, ready_value;
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