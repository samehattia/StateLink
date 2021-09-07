
proc read_axi_sim_to_hw_pipe {sim_to_hw_pipe hw_to_sim_pipename jtag_axi} {
	global SIM_TO_HW_PIPE_CLOSED AXI_WRITE_COMMAND_COUNTER AXI_READ_COMMAND_COUNTER

	# TODO: check if multiple lines are read 
	set data [split [read $sim_to_hw_pipe]]
	set command [lindex $data 0]

	if { $command eq "W"} {
		set waddr [lindex $data 1]
		set wlen [lindex $data 2]
		set wdata [lindex $data 3]
		create_hw_axi_txn wr_txn [get_hw_axis $jtag_axi] -address $waddr -len $wlen -type write -force -data $wdata
		run_hw_axi -verbose wr_txn >> $hw_to_sim_pipename
		incr AXI_WRITE_COMMAND_COUNTER
	} elseif { $command eq "R"} {
		set raddr [lindex $data 1]
		set rlen [lindex $data 2]
		create_hw_axi_txn rd_txn [get_hw_axis $jtag_axi] -address $raddr -len $rlen -type read -force
		run_hw_axi -verbose rd_txn >> $hw_to_sim_pipename
		incr AXI_READ_COMMAND_COUNTER
	}

	if {[eof $sim_to_hw_pipe]} {
		close $sim_to_hw_pipe
		puts "Pipe Closed"
		set SIM_TO_HW_PIPE_CLOSED 1
	}
}

### Open the fifo, set up event-driven input from it.
proc open_axi_sim_to_hw_pipe {sim_to_hw_pipename hw_to_sim_pipename jtag_axi} {

	while 1 {
		if {[file exists $sim_to_hw_pipename]} {
			break
		}
		after 1000
	}
	set axi_sim_to_hw_pipe [open $sim_to_hw_pipename r]
	fconfigure $axi_sim_to_hw_pipe -blocking 0
	fileevent $axi_sim_to_hw_pipe readable [list read_axi_sim_to_hw_pipe $axi_sim_to_hw_pipe $hw_to_sim_pipename $jtag_axi]
}