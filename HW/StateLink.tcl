set THREAD_LIB_PATH /usr/lib/tcltk/x86_64-linux-gnu/thread2.8.5
lappend auto_path $THREAD_LIB_PATH
package require Thread 

set AXI_LINK 1
set AXIS_LINK 1

set AXI_JTAG_AXI_INSTANCE "*/StateLink_AXI_0/jtag_axi_0"
set AXIS_JTAG_AXI_INSTANCE "*/StateLink_AXIS_0/jtag_axi_1"
set AXIS_JTAG_AXI_LITE_INSTANCE "*/StateLink_AXIS_0/jtag_axi_0"

set AXI_JTAG_AXI ""
set AXIS_JTAG_AXI ""
set AXIS_JTAG_AXI_LITE ""

set AXI_SIM_TO_HW_PIPENAME "/tmp/axi_sim_to_hw_pipe"
set AXI_HW_TO_SIM_PIPENAME "/tmp/axi_hw_to_sim_pipe"
set SIM_TO_HW_PIPE_CLOSED 0

set AXI_WRITE_COMMAND_COUNTER 0
set AXI_READ_COMMAND_COUNTER 0

set AXIS_TX_SIM_TO_HW_PIPENAME "/tmp/axis_tx_sim_to_hw_pipe"
set AXIS_TX_HW_TO_SIM_PIPENAME "/tmp/axis_tx_hw_to_sim_pipe"

set AXIS_WRITE_COMMAND_COUNTER 0

set AXIS_RX_THREAD 0

proc read_axi_sim_to_hw_pipe pipe {
	global SIM_TO_HW_PIPE_CLOSED AXI_HW_TO_SIM_PIPENAME AXI_WRITE_COMMAND_COUNTER AXI_READ_COMMAND_COUNTER AXI_JTAG_AXI

	# TODO: check if multiple lines are read 
	set data [split [read $pipe]]
	set command [lindex $data 0]

	if { $command eq "W"} {
		set waddr [lindex $data 1]
		set wlen [lindex $data 2]
		set wdata [lindex $data 3]
		create_hw_axi_txn wr_txn [get_hw_axis $AXI_JTAG_AXI] -address $waddr -len $wlen -type write -force -data $wdata
		run_hw_axi -verbose wr_txn >> $AXI_HW_TO_SIM_PIPENAME
		incr AXI_WRITE_COMMAND_COUNTER
	} elseif { $command eq "R"} {
		set raddr [lindex $data 1]
		set rlen [lindex $data 2]
		create_hw_axi_txn rd_txn [get_hw_axis $AXI_JTAG_AXI] -address $raddr -len $rlen -type read -force
		run_hw_axi -verbose rd_txn >> $AXI_HW_TO_SIM_PIPENAME
		incr AXI_READ_COMMAND_COUNTER
	}

	if {[eof $pipe]} {
		close $pipe
		puts "Pipe Closed"
		set SIM_TO_HW_PIPE_CLOSED 1
	}
}

proc start_axis_rx_thread {} {
	global AXIS_RX_THREAD

	set AXIS_RX_THREAD [thread::create  -joinable {
		puts " AXIS_RX_THREAD started."
		exec vivado -mode tcl -nolog -nojournal -source /home/sameh/Dropbox/UofT/Research/Work/StateMover/../StateLink/HW/StateLink_AXIS_RX.tcl
		puts " AXIS_RX_THREAD ended."
	}]
}

proc read_axis_tx_sim_to_hw_pipe pipe {
	global SIM_TO_HW_PIPE_CLOSED AXIS_TX_HW_TO_SIM_PIPENAME AXIS_WRITE_COMMAND_COUNTER AXIS_JTAG_AXI AXIS_JTAG_AXI_LITE

	# TODO: check if multiple lines are read 
	set data [split [read $pipe]]
	set command [lindex $data 0]

	if { $command eq "W"} {
		set packet_len [lindex $data 1]
		set packet_data [lindex $data 2]

		set burst_len [expr {$packet_len / 8.0}]
		set burst_len [expr {ceil($burst_len)}]
		set burst_len [expr {int($burst_len)}]
		set packet_len_hex [format "%08x" $packet_len]

		# Transmit FIFO vacancy (TDFV) @0xC --> OUTPUT should be 000001FC
		# create_hw_axi_txn rd_txn [get_hw_axis hw_axi_1] -address 44A0_000C -len 1 -type read -force
		# run_hw_axi -verbose rd_txn

		# Transmit Data FIFO Data (TDFD) @0x0 hw_axi_2
		create_hw_axi_txn wr_txn_2 [get_hw_axis $AXIS_JTAG_AXI] -address 44A0_0000 -len $burst_len -type write -force -data $packet_data
		run_hw_axi -quiet wr_txn_2

		# Transmit Length Register (TLR) @0x14 <-- Input 0000003C
		create_hw_axi_txn wr_txn [get_hw_axis $AXIS_JTAG_AXI_LITE] -address 44A0_0014 -len 1 -type write -force -data $packet_len_hex
		run_hw_axi -verbose wr_txn >> $AXIS_TX_HW_TO_SIM_PIPENAME

		incr AXIS_WRITE_COMMAND_COUNTER
	}

	if {[eof $pipe]} {
		close $pipe
		puts "Pipe Closed"
		set SIM_TO_HW_PIPE_CLOSED 1
	}
}

### Open the fifo, set up event-driven input from it.
proc open_axi_sim_to_hw_pipe {} {
	global AXI_SIM_TO_HW_PIPENAME

	while 1 {
		if {[file exists $AXI_SIM_TO_HW_PIPENAME]} {
			break
		}
		after 1000
	}
	set axi_sim_to_hw_pipe [open $AXI_SIM_TO_HW_PIPENAME r]
	fconfigure $axi_sim_to_hw_pipe -blocking 0
	fileevent $axi_sim_to_hw_pipe readable [list read_axi_sim_to_hw_pipe $axi_sim_to_hw_pipe]
}

proc open_axis_tx_sim_to_hw_pipe {} {
	global AXIS_TX_SIM_TO_HW_PIPENAME

	while 1 {
		if {[file exists $AXIS_TX_SIM_TO_HW_PIPENAME]} {
			break
		}
		after 1000
	}
	set axis_tx_sim_to_hw_pipe [open $AXIS_TX_SIM_TO_HW_PIPENAME r]
	fconfigure $axis_tx_sim_to_hw_pipe -blocking 0
	fileevent $axis_tx_sim_to_hw_pipe readable [list read_axis_tx_sim_to_hw_pipe $axis_tx_sim_to_hw_pipe]
}

proc StateLink {} {
	global DEVICE_NAME AXI_LINK AXIS_LINK SIM_TO_HW_PIPE_CLOSED AXI_HW_TO_SIM_PIPENAME AXI_WRITE_COMMAND_COUNTER AXI_READ_COMMAND_COUNTER AXIS_TX_HW_TO_SIM_PIPENAME AXIS_WRITE_COMMAND_COUNTER AXIS_RX_THREAD AXI_JTAG_AXI AXIS_JTAG_AXI AXIS_JTAG_AXI_LITE AXI_JTAG_AXI_INSTANCE AXIS_JTAG_AXI_INSTANCE AXIS_JTAG_AXI_LITE_INSTANCE
	
	set SIM_TO_HW_PIPE_CLOSED 0
	set AXI_WRITE_COMMAND_COUNTER 0
	set AXI_READ_COMMAND_COUNTER 0
	set AXIS_WRITE_COMMAND_COUNTER 0

	if { $AXI_LINK ne 0 } {
		puts "Opening AXI_SIM_TO_HW_PIPE"
		open_axi_sim_to_hw_pipe

		puts "Opening AXI_HW_TO_SIM_PIPE"
		set axi_hw_to_sim_pipe [open $AXI_HW_TO_SIM_PIPENAME w]

		set AXI_JTAG_AXI [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXI_JTAG_AXI_INSTANCE"]
	}

	if { $AXIS_LINK ne 0 } {
		puts "Opening AXIS_TX_SIM_TO_HW_PIPE"
		open_axis_tx_sim_to_hw_pipe

		puts "Opening AXIS_TX_HW_TO_SIM_PIPE"
		set axis_tx_hw_to_sim_pipe [open $AXIS_TX_HW_TO_SIM_PIPENAME w]

		puts "Starting AXIS_RX_THREAD"
		start_axis_rx_thread

		set AXIS_JTAG_AXI [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_JTAG_AXI_INSTANCE"]
		set AXIS_JTAG_AXI_LITE [get_hw_axis -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~$AXIS_JTAG_AXI_LITE_INSTANCE"]
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