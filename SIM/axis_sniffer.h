#ifndef AXIS_SNIFFER_H
#define AXIS_SNIFFER_H

#include "vpi_user.h"

#define PARAM_FILE_NAME "param.txt"

#define AXIS_TX_SIM_TO_HW_PIPENAME "/tmp/axis_tx_sim_to_hw_pipe"
#define AXIS_TX_HW_TO_SIM_PIPENAME "/tmp/axis_tx_hw_to_sim_pipe"
#define AXIS_RX_SIM_TO_HW_PIPENAME "/tmp/axis_rx_sim_to_hw_pipe"
#define AXIS_RX_HW_TO_SIM_PIPENAME "/tmp/axis_rx_hw_to_sim_pipe"

#define AXIS_RX_FIRST_PACKET_DELAY 45000 // minimum number of cycles before feeding the first packet
#define AXIS_RX_PACKET_DELAY 150 // minimum number of cycles between each packet

// Timestamp mode in which the the received AXIS packets from hardware are timestamped
const bool axis_timestamp_mode = true;
#define MAX_IDLE_CYCLES 150

#define MAX_AXIS_QUEUE_SIZE 5000

#endif
