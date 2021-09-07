set THREAD_LIB_PATH /usr/lib/tcltk/x86_64-linux-gnu/thread2.8.5
lappend auto_path $THREAD_LIB_PATH
package require Thread 

set AXI_LINK 2
set AXIS_LINK 1

set AXI_JTAG_AXI_INSTANCE {"*/StateLink_AXI_0/jtag_axi_0" "*/StateLink_AXI_1/jtag_axi_0"}
set AXIS_JTAG_AXI_INSTANCE "*/StateLink_AXIS_0/jtag_axi_1"
set AXIS_JTAG_AXI_LITE_INSTANCE "*/StateLink_AXIS_0/jtag_axi_0"

set AXI_SIM_TO_HW_PIPENAME "/tmp/axi_sim_to_hw_pipe"
set AXI_HW_TO_SIM_PIPENAME "/tmp/axi_hw_to_sim_pipe"

set AXIS_TX_SIM_TO_HW_PIPENAME "/tmp/axis_tx_sim_to_hw_pipe"
set AXIS_TX_HW_TO_SIM_PIPENAME "/tmp/axis_tx_hw_to_sim_pipe"

set SIM_TO_HW_PIPE_CLOSED 0

set AXI_WRITE_COMMAND_COUNTER 0
set AXI_READ_COMMAND_COUNTER 0
set AXIS_WRITE_COMMAND_COUNTER 0

set AXIS_RX_THREAD 0

# StateLink Scripts
set STATELINK_PATH [file dirname [file normalize [info script]]]
source $STATELINK_PATH/StateLink_AXI.tcl
source $STATELINK_PATH/StateLink_AXIS.tcl

proc StateLink {} {
	global DEVICE_NAME AXI_LINK AXIS_LINK SIM_TO_HW_PIPE_CLOSED AXI_SIM_TO_HW_PIPENAME AXI_HW_TO_SIM_PIPENAME AXI_WRITE_COMMAND_COUNTER AXI_READ_COMMAND_COUNTER AXIS_TX_SIM_TO_HW_PIPENAME AXIS_TX_HW_TO_SIM_PIPENAME AXIS_WRITE_COMMAND_COUNTER AXIS_RX_THREAD AXI_JTAG_AXI_INSTANCE AXIS_JTAG_AXI_INSTANCE AXIS_JTAG_AXI_LITE_INSTANCE
	
	set SIM_TO_HW_PIPE_CLOSED 0
	set AXI_WRITE_COMMAND_COUNTER 0
	set AXI_READ_COMMAND_COUNTER 0
	set AXIS_WRITE_COMMAND_COUNTER 0

	for {set i 0} {$i < $AXI_LINK} {incr i} {
		set axi_jtag_axi [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~[lindex $AXI_JTAG_AXI_INSTANCE $i]"]

		set axi_sim_to_hw_pipename ${AXI_SIM_TO_HW_PIPENAME}_$i
		set axi_hw_to_sim_pipename ${AXI_HW_TO_SIM_PIPENAME}_$i

		puts "Opening AXI_SIM_TO_HW_PIPE"
		open_axi_sim_to_hw_pipe $axi_sim_to_hw_pipename $axi_hw_to_sim_pipename $axi_jtag_axi

		puts "Opening AXI_HW_TO_SIM_PIPE"
		set axi_hw_to_sim_pipe [open $axi_hw_to_sim_pipename w]
	}

	if { $AXIS_LINK ne 0 } {
		set axis_jtag_axi [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_JTAG_AXI_INSTANCE"]
		set axis_jtag_axi_lite [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_JTAG_AXI_LITE_INSTANCE"]

		puts "Opening AXIS_TX_SIM_TO_HW_PIPE"
		open_axis_tx_sim_to_hw_pipe $AXIS_TX_SIM_TO_HW_PIPENAME $AXIS_TX_HW_TO_SIM_PIPENAME $axis_jtag_axi $axis_jtag_axi_lite

		puts "Opening AXIS_TX_HW_TO_SIM_PIPE"
		set axis_tx_hw_to_sim_pipe [open $AXIS_TX_HW_TO_SIM_PIPENAME w]

		puts "Starting AXIS_RX_THREAD"
		start_axis_rx_thread
	}

	puts "Entering Event Loop"
	vwait SIM_TO_HW_PIPE_CLOSED

	if { $AXI_LINK ne 0 } {
		close $axi_hw_to_sim_pipe

		puts "$AXI_WRITE_COMMAND_COUNTER AXI Write Transactions"
		puts "$AXI_READ_COMMAND_COUNTER AXI Read Transactions"	
	}

	if { $AXIS_LINK ne 0 } {
		close $axis_tx_hw_to_sim_pipe
		thread::join $AXIS_RX_THREAD
		puts [exec cat /tmp/statelink.log]

		puts "$AXIS_WRITE_COMMAND_COUNTER AXIS Send Transactions"	
	}
}