set SIM_TO_HW_PIPENAME "/tmp/sim_to_hw_pipe"
set HW_TO_SIM_PIPENAME "/tmp/hw_to_sim_pipe"
set SIM_TO_HW_PIPE_CLOSED 0
set WRITE_COMMAND_COUNTER 0
set READ_COMMAND_COUNTER 0

proc read_pipe pipe {
	global SIM_TO_HW_PIPE_CLOSED HW_TO_SIM_PIPENAME WRITE_COMMAND_COUNTER READ_COMMAND_COUNTER

	# TODO: check if multiple lines are read 
	set data [split [read $pipe]]
	set command [lindex $data 0]

	if { $command eq "W"} {
		set waddr [lindex $data 1]
		set wlen [lindex $data 2]
		set wdata [lindex $data 3]
		create_hw_axi_txn wr_txn [get_hw_axis hw_axi_1] -address $waddr -len $wlen -type write -force -data $wdata
		run_hw_axi -verbose wr_txn >> $HW_TO_SIM_PIPENAME
		incr WRITE_COMMAND_COUNTER
	} elseif { $command eq "R"} {
		set raddr [lindex $data 1]
		set rlen [lindex $data 2]
		create_hw_axi_txn rd_txn [get_hw_axis hw_axi_1] -address $raddr -len $rlen -type read -force
		run_hw_axi -verbose rd_txn >> $HW_TO_SIM_PIPENAME
		incr READ_COMMAND_COUNTER
	}

	if {[eof $pipe]} {
		close $pipe
		puts "Pipe Closed"
		set SIM_TO_HW_PIPE_CLOSED 1
	}
}

### Open the fifo, set up event-driven input from it.
proc open_pipe {} {
	global SIM_TO_HW_PIPENAME

	set pipe [open $SIM_TO_HW_PIPENAME r]
	fconfigure $pipe -blocking 0
	fileevent $pipe readable [list read_pipe $pipe]
}

proc StateLink {} {
	global SIM_TO_HW_PIPE_CLOSED HW_TO_SIM_PIPENAME WRITE_COMMAND_COUNTER READ_COMMAND_COUNTER
	
	set SIM_TO_HW_PIPE_CLOSED 0
	set WRITE_COMMAND_COUNTER 0
	set READ_COMMAND_COUNTER 0

	puts "Opening SIM_TO_HW_PIPE"
	open_pipe

	puts "Opening HW_TO_SIM_PIPE"
	set send_pipe [open $HW_TO_SIM_PIPENAME w]

	puts "Entering Event Loop"
	vwait SIM_TO_HW_PIPE_CLOSED

	close $send_pipe

	puts -nonewline $WRITE_COMMAND_COUNTER
	puts " Write Transactions"
	puts -nonewline $READ_COMMAND_COUNTER
	puts " Read Transactions"
}