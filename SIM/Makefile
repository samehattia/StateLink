
# Modelsim installation path
MODELSIM = /home/sameh/modelsim.10.7a/modeltech

# 32-bit GCC/G++ compilation flags
#CFLAGS = -c -g -I$(MODELSIM)/include -m32

# 32-bit G++ linking flags
#LFLAGS = -shared -Bsymbolic -m32

# 32-bit LD linking flags 
#LFLAGS = -shared -E -melf_i386

# 64-bit GCC/G++ compilation flags
CFLAGS = -c -g -I$(MODELSIM)/include -fPIC -std=c++11

# 64-bit G++ linking flags
LFLAGS = -shared -Bsymbolic

CSR_SIM_PATH = /home/sameh/Research/Work/StateMover/CSR_SIM/
CSR_SIM_OBJECT_FILES = $(CSR_SIM_PATH)/csr_sim.o $(CSR_SIM_PATH)/state_element.o $(CSR_SIM_PATH)/state_access.o

all: $(CSR_SIM_OBJECT_FILES) axi_sniffer.cpp
	g++-5 $(CFLAGS) vlog_startup.cpp axi_sniffer.cpp axi_interface.cpp axis_sniffer.cpp message.cpp
	g++-5 $(LFLAGS) -o StateLink.sl vlog_startup.o axi_sniffer.o axi_interface.o axis_sniffer.o message.o $(CSR_SIM_OBJECT_FILES)

clean: 
	rm -rf StateLink.sl vlog_startup.o axi_sniffer.o axis_sniffer.o message.o



