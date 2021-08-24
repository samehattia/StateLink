set THREAD_LIB_PATH /usr/lib/tcltk/x86_64-linux-gnu/thread2.8.2
lappend auto_path $THREAD_LIB_PATH
package require Thread 

# Default Parameters
set DEVICE_NAME "xcku040_0"

# User-defined Parameters (Overwrite the default parameters)
source SM_Param.tcl

set AXIS_JTAG_AXI_INSTANCE "*/StateLink_AXIS_0/jtag_axi_1"
set AXIS_JTAG_AXI_LITE_INSTANCE "*/StateLink_AXIS_0/jtag_axi_0"

set AXIS_VIO_INSTANCE "*/StateLink_AXIS_0/vio_0"
set AXIS_VIO_SIGNAL "*/StateLink_AXIS_0/axi_fifo_mm_s_0_interrupt"

set AXIS_JTAG_AXI ""
set AXIS_JTAG_AXI_LITE ""
set AXIS_VIO ""

set AXIS_RX_SIM_TO_HW_PIPENAME "/tmp/axis_rx_sim_to_hw_pipe"
set AXIS_RX_HW_TO_SIM_PIPENAME "/tmp/axis_rx_hw_to_sim_pipe"

set AXIS_INTERNAL_PIPENAME "/tmp/axis_internal_pipe"

set AXIS_READ_COMMAND_COUNTER 0

set AXIS_RX_HW_THREAD 0

proc wait_axis_rx_interrupt {} {
	global SIM_TO_HW_PIPE_CLOSED AXIS_RX_HW_THREAD AXIS_JTAG_AXI_LITE AXIS_VIO

	# Wait for the interrupt to go high
	while 1 {
		refresh_hw_vio [get_hw_vios $AXIS_VIO]
		set interrupt [get_property INPUT_VALUE [get_hw_probes -of_objects [get_hw_vios $AXIS_VIO]]]

		if { $interrupt } {
			break
		}

		if { [thread::exists $AXIS_RX_HW_THREAD] eq 0 } {
			break
		}

		after 1
	}

	# Interrupt Status Register (ISR) @0x00 --> Output 04000000
	create_hw_axi_txn rd_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0000 -len 1 -type read -force
	run_hw_axi -verbose rd_txn

	# Interrupt Status Register (ISR) @0x00 <-- Input FFFFFFFF: Reset interrupt bits
	create_hw_axi_txn wr_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0000 -len 1 -type write -force -data FFFF_FFFF
	run_hw_axi wr_txn

}

proc read_axis_rx_hw_packet {axis_internal_pipe axis_rx_hw_to_sim_pipe} {
	global AXIS_INTERNAL_PIPENAME AXIS_RX_HW_TO_SIM_PIPENAME AXIS_READ_COMMAND_COUNTER AXIS_JTAG_AXI AXIS_JTAG_AXI_LITE
	
	# Consume all the recieve FIFO till it is empty
	while { 1 } {
		# Receive FIFO occupancy (RDFO) @0x1C --> OUTPUT 00000202 (FULL)
		create_hw_axi_txn rd_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_001C -len 1 -type read -force
		run_hw_axi -verbose rd_txn >> $AXIS_INTERNAL_PIPENAME
		set data [split [read $axis_internal_pipe]]
		set occupany [lindex $data 6]

		if { $occupany eq "00000000"} {
			break
		}

		# Receive length (RLR) @0x24 --> OUTPUT 0000003C
		create_hw_axi_txn rd_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0024 -len 1 -type read -force
		run_hw_axi -verbose rd_txn >> $AXIS_INTERNAL_PIPENAME

		set data [split [read $axis_internal_pipe]]
		set packet_len_hex [lindex $data 6]
		set packet_len [expr 0x$packet_len_hex]
		set burst_len [expr {$packet_len / 8.0}]
		set burst_len [expr {ceil($burst_len)}]
		set burst_len [expr {int($burst_len)}]
		
		puts $axis_rx_hw_to_sim_pipe "L $packet_len\0"
		flush $axis_rx_hw_to_sim_pipe

		# Receive Data FIFO Data (TDFD) @0x1000 hw_axi_2 --> OUTPUT 0000000000000000000000000000000041612a000000deadbeef0000000000000000000000000a0000000000000801000180220825da924537d0020100350a00
		create_hw_axi_txn rd_txn_2 [get_hw_axis $AXIS_JTAG_AXI] -address 44A0_1000 -len $burst_len -type read -force
		run_hw_axi -verbose rd_txn_2 >> $AXIS_RX_HW_TO_SIM_PIPENAME
		incr AXIS_READ_COMMAND_COUNTER
	}
	
}

proc open_axis_rx_sim_to_hw_pipe {} {
	global AXIS_RX_HW_THREAD

	set AXIS_RX_HW_THREAD [thread::create  -joinable {
		proc read_axis_rx_sim_to_hw_pipe pipe {
			global SIM_TO_HW_PIPE_CLOSED

			set data [split [read $pipe]]

			if {[eof $pipe]} {
				close $pipe
				puts "Pipe Closed"
				set SIM_TO_HW_PIPE_CLOSED 1
			}
		}

		set AXIS_RX_SIM_TO_HW_PIPENAME "/tmp/axis_rx_sim_to_hw_pipe"
		set SIM_TO_HW_PIPE_CLOSED 0 

		set axis_rx_sim_to_hw_pipe [open $AXIS_RX_SIM_TO_HW_PIPENAME r]
		fconfigure $axis_rx_sim_to_hw_pipe -blocking 0
		fileevent $axis_rx_sim_to_hw_pipe readable [list read_axis_rx_sim_to_hw_pipe $axis_rx_sim_to_hw_pipe]

		vwait SIM_TO_HW_PIPE_CLOSED
	}]
}

proc StateLink_AXIS_RX {} {
	global DEVICE_NAME FULL_PROBES AXIS_RX_SIM_TO_HW_PIPENAME AXIS_RX_HW_TO_SIM_PIPENAME AXIS_INTERNAL_PIPENAME AXIS_READ_COMMAND_COUNTER AXIS_RX_HW_THREAD AXIS_JTAG_AXI AXIS_JTAG_AXI_LITE AXIS_VIO AXIS_JTAG_AXI_INSTANCE AXIS_JTAG_AXI_LITE_INSTANCE AXIS_VIO_INSTANCE

	open_hw
	connect_hw_server
	open_hw_target
	set_property PROBES.FILE $FULL_PROBES [get_hw_devices $DEVICE_NAME]
	set_property FULL_PROBES.FILE $FULL_PROBES [get_hw_devices $DEVICE_NAME]
	current_hw_device [get_hw_devices $DEVICE_NAME]
	refresh_hw_device [lindex [get_hw_devices $DEVICE_NAME] 0]

	puts "Opening AXIS_RX_SIM_TO_HW_PIPE"
	open_axis_rx_sim_to_hw_pipe

	puts "Opening AXIS_RX_HW_TO_SIM_PIPE"
	set axis_rx_hw_to_sim_pipe [open $AXIS_RX_HW_TO_SIM_PIPENAME w]

	if {![file exists $AXIS_INTERNAL_PIPENAME]} {
		exec mkfifo $AXIS_INTERNAL_PIPENAME
	}
	set axis_internal_pipe [open $AXIS_INTERNAL_PIPENAME {RDONLY NONBLOCK}]

	set AXIS_JTAG_AXI [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_JTAG_AXI_INSTANCE"]
	set AXIS_JTAG_AXI_LITE [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_JTAG_AXI_LITE_INSTANCE"]
	set AXIS_VIO [get_hw_vios -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_VIO_INSTANCE"]

	# Direct the packets to the AXIS FIFO # Currently, packets are directed by the decouple signal
	# set_property OUTPUT_VALUE 1 [get_hw_probes "memcached_i/StateLink_AXIS_0/S00_AXIS_tdest_1" -of_objects [get_hw_vios -of_objects [get_hw_devices "xcku040_0"] -filter "CELL_NAME=~memcached_i/StateLink_AXIS_0/vio_1"]]
	# commit_hw_vio [get_hw_probes "memcached_i/StateLink_AXIS_0/S00_AXIS_tdest_1" -of_objects [get_hw_vios -of_objects [get_hw_devices "xcku040_0"] -filter "CELL_NAME=~memcached_i/StateLink_AXIS_0/vio_1"]]

	# Receive Data FIFO Reset Register (RDFR) @0x18 <-- Input 0000_00A5: Reset Receive FIFO
	create_hw_axi_txn wr_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0018 -len 1 -type write -force -data 0000_00A5
	run_hw_axi wr_txn

	# Interrupt Status Register (ISR) @0x00 <-- Input FFFFFFFF: Reset interrupt bits
	create_hw_axi_txn wr_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0000 -len 1 -type write -force -data FFFF_FFFF
	run_hw_axi wr_txn

	# Interrupt Enable Register (IER) @0x04 <-- Input 04000000: Enable receive complete interrupt
	create_hw_axi_txn wr_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0004 -len 1 -type write -force -data 0400_0000
	run_hw_axi wr_txn

	# Start RX Loop
	while { 1 } {
		read_axis_rx_hw_packet $axis_internal_pipe $axis_rx_hw_to_sim_pipe

		wait_axis_rx_interrupt

		if { [thread::exists $AXIS_RX_HW_THREAD] eq 0 } {
			break
		}
	}

	close $axis_internal_pipe
	close $axis_rx_hw_to_sim_pipe

	puts "$AXIS_READ_COMMAND_COUNTER AXIS Receive Transactions"
	exec echo "$AXIS_READ_COMMAND_COUNTER AXIS Receive Transactions" > /tmp/statelink.log
}

StateLink_AXIS_RX
exit