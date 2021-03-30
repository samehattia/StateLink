
extern void csr_sim_register(), axi_sniffer_register();

/**
 * Required structure for initializing VPI routines.
 */
void (*vlog_startup_routines[])() = {
	csr_sim_register,
	axi_sniffer_register,
	0
};