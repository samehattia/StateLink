
set DEVICE_NAME "xcku040_0"

set DESIGN_PATH "."
set FULL_PROBES "$DESIGN_PATH/top.ltx"

open_hw
connect_hw_server
open_hw_target
set_property FULL_PROBES.FILE $FULL_PROBES [get_hw_devices $DEVICE_NAME]
current_hw_device [get_hw_devices $DEVICE_NAME]
refresh_hw_device [lindex [get_hw_devices $DEVICE_NAME] 0]

# Base Address 0x44A0_0000
# Page 19 and 37

# Transmit FIFO vacancy (TDFV) @0xC --> OUTPUT 000001FC
create_hw_axi_txn rd_txn [get_hw_axis hw_axi_1] -address 44A0_000C -len 1 -type read -force
run_hw_axi -verbose rd_txn

# Transmit Destination address (TDR) @0x2C 
create_hw_axi_txn wr_txn [get_hw_axis hw_axi_1] -address 44A0_002C -len 1 -type write -force -data "00000000"
run_hw_axi -verbose wr_txn

# Transmit Data FIFO Reset (TDFR) @0x8 <-- Input 000000A5
create_hw_axi_txn wr_txn [get_hw_axis hw_axi_1] -address 44A0_0008 -len 1 -type write -force -data "000000A5"
run_hw_axi -verbose wr_txn

# Transmit Data FIFO Data (TDFD) @0x0 hw_axi_2
create_hw_axi_txn wr_txn_2 [get_hw_axis hw_axi_2] -address 44A0_0000 -len 8 -type write -force -data "0000000000000000000000000000000000000000000000000000deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef0008020100350a0025da924537d0"
run_hw_axi -verbose wr_txn_2

# Transmit Length Register (TLR) @0x14 <-- Input 0000003C
create_hw_axi_txn wr_txn [get_hw_axis hw_axi_1] -address 44A0_0014 -len 1 -type write -force -data "0000003C"
run_hw_axi -verbose wr_txn

########################################################################################################
set_property OUTPUT_VALUE 1 [get_hw_probes memcached_i/vio_0_probe_out0 -of_objects [get_hw_vios -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~memcached_i/vio_0"]]
commit_hw_vio [get_hw_probes memcached_i/vio_0_probe_out0 -of_objects [get_hw_vios -of_objects [get_hw_devices $DEVICE_NAME] -filter "CELL_NAME=~memcached_i/vio_0"]]
########################################################################################################

# Receive FIFO occupancy (RDFO) @0x1C --> OUTPUT 00000202 (FULL)
create_hw_axi_txn rd_txn [get_hw_axis hw_axi_1] -address 44A0_001C -len 1 -type read -force
run_hw_axi -verbose rd_txn

# Receive length (RLR) @0x24 --> OUTPUT 0000003C
create_hw_axi_txn rd_txn [get_hw_axis hw_axi_1] -address 44A0_0024 -len 1 -type read -force
run_hw_axi -verbose rd_txn

# Receive Data FIFO Data (TDFD) @0x1000 hw_axi_2 --> OUTPUT 0000000000000000000000000000000041612a000000deadbeef0000000000000000000000000a0000000000000801000180220825da924537d0020100350a00
create_hw_axi_txn rd_txn_2 [get_hw_axis hw_axi_2] -address 44A0_1000 -len 8 -type read -force
run_hw_axi -verbose rd_txn_2
