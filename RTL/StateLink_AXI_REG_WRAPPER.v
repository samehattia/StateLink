module StateLink_AXI_REG_WRAPPER (
	input wire sys_clk,
	input wire sys_rst_n
);
	parameter AXI_DATA_WIDTH = 512;
	parameter AXI_STRB_WIDTH = AXI_DATA_WIDTH / 8;
		
	wire [0:0] M00_AXI_0_arready;
	wire [0:0] M00_AXI_0_awready;
	wire [1:0] M00_AXI_0_bresp;
	wire [0:0] M00_AXI_0_bvalid;
	wire [AXI_DATA_WIDTH-1:0] M00_AXI_0_rdata;
	wire [0:0] M00_AXI_0_rlast;
	wire [1:0] M00_AXI_0_rresp;
	wire [0:0] M00_AXI_0_rvalid;
	wire [0:0] M00_AXI_0_wready;
	wire [31:0] S00_AXI_araddr;
	wire [1:0] S00_AXI_arburst;
	wire [3:0] S00_AXI_arcache;
	wire [5:0] S00_AXI_arid;
	wire [7:0] S00_AXI_arlen;
	wire S00_AXI_arlock;
	wire [2:0] S00_AXI_arprot;
	wire [3:0] S00_AXI_arqos;
	wire [3:0] S00_AXI_arregion;
	wire [2:0] S00_AXI_arsize;
	wire [3:0] S00_AXI_aruser;
	wire S00_AXI_arvalid;
	wire [31:0] S00_AXI_awaddr;
	wire [1:0] S00_AXI_awburst;
	wire [3:0] S00_AXI_awcache;
	wire [5:0] S00_AXI_awid;
	wire [7:0] S00_AXI_awlen;
	wire S00_AXI_awlock;
	wire [2:0] S00_AXI_awprot;
	wire [3:0] S00_AXI_awqos;
	wire [3:0] S00_AXI_awregion;
	wire [2:0] S00_AXI_awsize;
	wire [3:0] S00_AXI_awuser;
	wire S00_AXI_awvalid;
	wire S00_AXI_bready;
	wire S00_AXI_rready;
	wire [AXI_DATA_WIDTH-1:0] S00_AXI_wdata;
	wire S00_AXI_wlast;
	wire [AXI_STRB_WIDTH-1:0] S00_AXI_wstrb;
	wire S00_AXI_wvalid;
	wire decouple_0;
	wire [1:0] stop_req_0;

	(* dont_touch = "true" *) reg [31:0] M00_AXI_0_araddr;
	(* dont_touch = "true" *) reg [1:0] M00_AXI_0_arburst;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_arcache;
	(* dont_touch = "true" *) reg [7:0] M00_AXI_0_arlen;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_arlock;
	(* dont_touch = "true" *) reg [2:0] M00_AXI_0_arprot;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_arqos;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_arregion;
	(* dont_touch = "true" *) reg [2:0] M00_AXI_0_arsize;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_aruser;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_arvalid;
	(* dont_touch = "true" *) reg [31:0] M00_AXI_0_awaddr;
	(* dont_touch = "true" *) reg [1:0] M00_AXI_0_awburst;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_awcache;
	(* dont_touch = "true" *) reg [7:0] M00_AXI_0_awlen;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_awlock;
	(* dont_touch = "true" *) reg [2:0] M00_AXI_0_awprot;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_awqos;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_awregion;
	(* dont_touch = "true" *) reg [2:0] M00_AXI_0_awsize;
	(* dont_touch = "true" *) reg [3:0] M00_AXI_0_awuser;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_awvalid;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_bready;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_rready;
	(* dont_touch = "true" *) reg [AXI_DATA_WIDTH-1:0] M00_AXI_0_wdata;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_wlast;
	(* dont_touch = "true" *) reg [AXI_STRB_WIDTH-1:0] M00_AXI_0_wstrb;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_wvalid;
	(* dont_touch = "true" *) reg S00_AXI_arready;
	(* dont_touch = "true" *) reg S00_AXI_awready;
	(* dont_touch = "true" *) reg [5:0] S00_AXI_bid;
	(* dont_touch = "true" *) reg [1:0] S00_AXI_bresp;
	(* dont_touch = "true" *) reg S00_AXI_bvalid;
	(* dont_touch = "true" *) reg [AXI_DATA_WIDTH-1:0] S00_AXI_rdata;
	(* dont_touch = "true" *) reg [5:0] S00_AXI_rid;
	(* dont_touch = "true" *) reg S00_AXI_rlast;
	(* dont_touch = "true" *) reg [1:0] S00_AXI_rresp;
	(* dont_touch = "true" *) reg S00_AXI_rvalid;
	(* dont_touch = "true" *) reg S00_AXI_wready;
	(* dont_touch = "true" *) reg [1:0] stop_ack_0;

	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_arready_r;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_awready_r;
	(* dont_touch = "true" *) reg [1:0] M00_AXI_0_bresp_r;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_bvalid_r;
	(* dont_touch = "true" *) reg [AXI_DATA_WIDTH-1:0] M00_AXI_0_rdata_r;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_rlast_r;
	(* dont_touch = "true" *) reg [1:0] M00_AXI_0_rresp_r;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_rvalid_r;
	(* dont_touch = "true" *) reg [0:0] M00_AXI_0_wready_r;
	(* dont_touch = "true" *) reg [31:0] S00_AXI_araddr_r;
	(* dont_touch = "true" *) reg [1:0] S00_AXI_arburst_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_arcache_r;
	(* dont_touch = "true" *) reg [5:0] S00_AXI_arid_r;
	(* dont_touch = "true" *) reg [7:0] S00_AXI_arlen_r;
	(* dont_touch = "true" *) reg S00_AXI_arlock_r;
	(* dont_touch = "true" *) reg [2:0] S00_AXI_arprot_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_arqos_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_arregion_r;
	(* dont_touch = "true" *) reg [2:0] S00_AXI_arsize_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_aruser_r;
	(* dont_touch = "true" *) reg S00_AXI_arvalid_r;
	(* dont_touch = "true" *) reg [31:0] S00_AXI_awaddr_r;
	(* dont_touch = "true" *) reg [1:0] S00_AXI_awburst_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_awcache_r;
	(* dont_touch = "true" *) reg [5:0] S00_AXI_awid_r;
	(* dont_touch = "true" *) reg [7:0] S00_AXI_awlen_r;
	(* dont_touch = "true" *) reg S00_AXI_awlock_r;
	(* dont_touch = "true" *) reg [2:0] S00_AXI_awprot_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_awqos_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_awregion_r;
	(* dont_touch = "true" *) reg [2:0] S00_AXI_awsize_r;
	(* dont_touch = "true" *) reg [3:0] S00_AXI_awuser_r;
	(* dont_touch = "true" *) reg S00_AXI_awvalid_r;
	(* dont_touch = "true" *) reg S00_AXI_bready_r;
	(* dont_touch = "true" *) reg S00_AXI_rready_r;
	(* dont_touch = "true" *) reg [AXI_DATA_WIDTH-1:0] S00_AXI_wdata_r;
	(* dont_touch = "true" *) reg S00_AXI_wlast_r;
	(* dont_touch = "true" *) reg [AXI_STRB_WIDTH-1:0] S00_AXI_wstrb_r;
	(* dont_touch = "true" *) reg S00_AXI_wvalid_r;
	(* dont_touch = "true" *) reg decouple_0_r;
	(* dont_touch = "true" *) reg [1:0] stop_req_0_r;

	wire [31:0] M00_AXI_0_araddr_s;
	wire [1:0] M00_AXI_0_arburst_s;
	wire [3:0] M00_AXI_0_arcache_s;
	wire [7:0] M00_AXI_0_arlen_s;
	wire [0:0] M00_AXI_0_arlock_s;
	wire [2:0] M00_AXI_0_arprot_s;
	wire [3:0] M00_AXI_0_arqos_s;
	wire [3:0] M00_AXI_0_arregion_s;
	wire [2:0] M00_AXI_0_arsize_s;
	wire [3:0] M00_AXI_0_aruser_s;
	wire [0:0] M00_AXI_0_arvalid_s;
	wire [31:0] M00_AXI_0_awaddr_s;
	wire [1:0] M00_AXI_0_awburst_s;
	wire [3:0] M00_AXI_0_awcache_s;
	wire [7:0] M00_AXI_0_awlen_s;
	wire [0:0] M00_AXI_0_awlock_s;
	wire [2:0] M00_AXI_0_awprot_s;
	wire [3:0] M00_AXI_0_awqos_s;
	wire [3:0] M00_AXI_0_awregion_s;
	wire [2:0] M00_AXI_0_awsize_s;
	wire [3:0] M00_AXI_0_awuser_s;
	wire [0:0] M00_AXI_0_awvalid_s;
	wire [0:0] M00_AXI_0_bready_s;
	wire [0:0] M00_AXI_0_rready_s;
	wire [AXI_DATA_WIDTH-1:0] M00_AXI_0_wdata_s;
	wire [0:0] M00_AXI_0_wlast_s;
	wire [AXI_STRB_WIDTH-1:0] M00_AXI_0_wstrb_s;
	wire [0:0] M00_AXI_0_wvalid_s;
	wire S00_AXI_arready_s;
	wire S00_AXI_awready_s;
	wire [5:0] S00_AXI_bid_s;
	wire [1:0] S00_AXI_bresp_s;
	wire S00_AXI_bvalid_s;
	wire [AXI_DATA_WIDTH-1:0] S00_AXI_rdata_s;
	wire [5:0] S00_AXI_rid_s;
	wire S00_AXI_rlast_s;
	wire [1:0] S00_AXI_rresp_s;
	wire S00_AXI_rvalid_s;
	wire S00_AXI_wready_s;
	wire [1:0] stop_ack_0_s;

	wire sys_clk_bufg;
	wire sys_rst_n_bufg;
	assign sys_clk_bufg = sys_clk;
	assign sys_rst_n_bufg = sys_rst_n;

	StateLink_AXI_wrapper StateLink_AXI_WRAPPER_ (
		.sys_clk_0(sys_clk),
		.sys_rst_n_0(sys_rst_n),
		.M00_AXI_0_arready(M00_AXI_0_arready_r),
		.M00_AXI_0_awready(M00_AXI_0_awready_r),
		.M00_AXI_0_bresp(M00_AXI_0_bresp_r),
		.M00_AXI_0_bvalid(M00_AXI_0_bvalid_r),
		.M00_AXI_0_rdata(M00_AXI_0_rdata_r),
		.M00_AXI_0_rlast(M00_AXI_0_rlast_r),
		.M00_AXI_0_rresp(M00_AXI_0_rresp_r),
		.M00_AXI_0_rvalid(M00_AXI_0_rvalid_r),
		.M00_AXI_0_wready(M00_AXI_0_wready_r),
		.S00_AXI_0_araddr(S00_AXI_araddr_r),
		.S00_AXI_0_arburst(S00_AXI_arburst_r),
		.S00_AXI_0_arcache(S00_AXI_arcache_r),
		.S00_AXI_0_arid(S00_AXI_arid_r),
		.S00_AXI_0_arlen(S00_AXI_arlen_r),
		.S00_AXI_0_arlock(S00_AXI_arlock_r),
		.S00_AXI_0_arprot(S00_AXI_arprot_r),
		.S00_AXI_0_arqos(S00_AXI_arqos_r),
		.S00_AXI_0_arregion(S00_AXI_arregion_r),
		.S00_AXI_0_arsize(S00_AXI_arsize_r),
		.S00_AXI_0_aruser(S00_AXI_aruser_r),
		.S00_AXI_0_arvalid(S00_AXI_arvalid_r),
		.S00_AXI_0_awaddr(S00_AXI_awaddr_r),
		.S00_AXI_0_awburst(S00_AXI_awburst_r),
		.S00_AXI_0_awcache(S00_AXI_awcache_r),
		.S00_AXI_0_awid(S00_AXI_awid_r),
		.S00_AXI_0_awlen(S00_AXI_awlen_r),
		.S00_AXI_0_awlock(S00_AXI_awlock_r),
		.S00_AXI_0_awprot(S00_AXI_awprot_r),
		.S00_AXI_0_awqos(S00_AXI_awqos_r),
		.S00_AXI_0_awregion(S00_AXI_awregion_r),
		.S00_AXI_0_awsize(S00_AXI_awsize_r),
		.S00_AXI_0_awuser(S00_AXI_awuser_r),
		.S00_AXI_0_awvalid(S00_AXI_awvalid_r),
		.S00_AXI_0_bready(S00_AXI_bready_r),
		.S00_AXI_0_rready(S00_AXI_rready_r),
		.S00_AXI_0_wdata(S00_AXI_wdata_r),
		.S00_AXI_0_wlast(S00_AXI_wlast_r),
		.S00_AXI_0_wstrb(S00_AXI_wstrb_r),
		.S00_AXI_0_wvalid(S00_AXI_wvalid_r),
		.decouple_0_0(decouple_0_r),
		.stop_req_0_0(stop_req_0_r),
		.sys_clk_bufg_0(sys_clk_bufg),
		.sys_rst_n_bufg_0(sys_rst_n_bufg),

		.M00_AXI_0_araddr(M00_AXI_0_araddr_s),
		.M00_AXI_0_arburst(M00_AXI_0_arburst_s),
		.M00_AXI_0_arcache(M00_AXI_0_arcache_s),
		.M00_AXI_0_arlen(M00_AXI_0_arlen_s),
		.M00_AXI_0_arlock(M00_AXI_0_arlock_s),
		.M00_AXI_0_arprot(M00_AXI_0_arprot_s),
		.M00_AXI_0_arqos(M00_AXI_0_arqos_s),
		.M00_AXI_0_arregion(M00_AXI_0_arregion_s),
		.M00_AXI_0_arsize(M00_AXI_0_arsize_s),
		//.M00_AXI_0_aruser(M00_AXI_0_aruser_s),
		.M00_AXI_0_arvalid(M00_AXI_0_arvalid_s),
		.M00_AXI_0_awaddr(M00_AXI_0_awaddr_s),
		.M00_AXI_0_awburst(M00_AXI_0_awburst_s),
		.M00_AXI_0_awcache(M00_AXI_0_awcache_s),
		.M00_AXI_0_awlen(M00_AXI_0_awlen_s),
		.M00_AXI_0_awlock(M00_AXI_0_awlock_s),
		.M00_AXI_0_awprot(M00_AXI_0_awprot_s),
		.M00_AXI_0_awqos(M00_AXI_0_awqos_s),
		.M00_AXI_0_awregion(M00_AXI_0_awregion_s),
		.M00_AXI_0_awsize(M00_AXI_0_awsize_s),
		//.M00_AXI_0_awuser(M00_AXI_0_awuser_s),
		.M00_AXI_0_awvalid(M00_AXI_0_awvalid_s),
		.M00_AXI_0_bready(M00_AXI_0_bready_s),
		.M00_AXI_0_rready(M00_AXI_0_rready_s),
		.M00_AXI_0_wdata(M00_AXI_0_wdata_s),
		.M00_AXI_0_wlast(M00_AXI_0_wlast_s),
		.M00_AXI_0_wstrb(M00_AXI_0_wstrb_s),
		.M00_AXI_0_wvalid(M00_AXI_0_wvalid_s),
		.S00_AXI_0_arready(S00_AXI_arready_s),
		.S00_AXI_0_awready(S00_AXI_awready_s),
		.S00_AXI_0_bid(S00_AXI_bid_s),
		.S00_AXI_0_bresp(S00_AXI_bresp_s),
		.S00_AXI_0_bvalid(S00_AXI_bvalid_s),
		.S00_AXI_0_rdata(S00_AXI_rdata_s),
		.S00_AXI_0_rid(S00_AXI_rid_s),
		.S00_AXI_0_rlast(S00_AXI_rlast_s),
		.S00_AXI_0_rresp(S00_AXI_rresp_s),
		.S00_AXI_0_rvalid(S00_AXI_rvalid_s),
		.S00_AXI_0_wready(S00_AXI_wready_s),
		.stop_ack_0_0(stop_ack_0_s)
	);

	always @(posedge sys_clk)
	begin
		M00_AXI_0_arready_r <= M00_AXI_0_arready;
		M00_AXI_0_awready_r <= M00_AXI_0_awready;
		M00_AXI_0_bresp_r <= M00_AXI_0_bresp;
		M00_AXI_0_bvalid_r <= M00_AXI_0_bvalid;
		M00_AXI_0_rdata_r <= M00_AXI_0_rdata;
		M00_AXI_0_rlast_r <= M00_AXI_0_rlast;
		M00_AXI_0_rresp_r <= M00_AXI_0_rresp;
		M00_AXI_0_rvalid_r <= M00_AXI_0_rvalid;
		M00_AXI_0_wready_r <= M00_AXI_0_wready;
		S00_AXI_araddr_r <= S00_AXI_araddr;
		S00_AXI_arburst_r <= S00_AXI_arburst;
		S00_AXI_arcache_r <= S00_AXI_arcache;
		S00_AXI_arid_r <= S00_AXI_arid;
		S00_AXI_arlen_r <= S00_AXI_arlen;
		S00_AXI_arlock_r <= S00_AXI_arlock;
		S00_AXI_arprot_r <= S00_AXI_arprot;
		S00_AXI_arqos_r <= S00_AXI_arqos;
		S00_AXI_arregion_r <= S00_AXI_arregion;
		S00_AXI_arsize_r <= S00_AXI_arsize;
		S00_AXI_aruser_r <= S00_AXI_aruser;
		S00_AXI_arvalid_r <= S00_AXI_arvalid;
		S00_AXI_awaddr_r <= S00_AXI_awaddr;
		S00_AXI_awburst_r <= S00_AXI_awburst;
		S00_AXI_awcache_r <= S00_AXI_awcache;
		S00_AXI_awid_r <= S00_AXI_awid;
		S00_AXI_awlen_r <= S00_AXI_awlen;
		S00_AXI_awlock_r <= S00_AXI_awlock;
		S00_AXI_awprot_r <= S00_AXI_awprot;
		S00_AXI_awqos_r <= S00_AXI_awqos;
		S00_AXI_awregion_r <= S00_AXI_awregion;
		S00_AXI_awsize_r <= S00_AXI_awsize;
		S00_AXI_awuser_r <= S00_AXI_awuser;
		S00_AXI_awvalid_r <= S00_AXI_awvalid;
		S00_AXI_bready_r <= S00_AXI_bready;
		S00_AXI_rready_r <= S00_AXI_rready;
		S00_AXI_wdata_r <= S00_AXI_wdata;
		S00_AXI_wlast_r <= S00_AXI_wlast;
		S00_AXI_wstrb_r <= S00_AXI_wstrb;
		S00_AXI_wvalid_r <= S00_AXI_wvalid;
		decouple_0_r <= decouple_0;
		stop_req_0_r <= stop_req_0;

		M00_AXI_0_araddr <= M00_AXI_0_araddr_s;
		M00_AXI_0_arburst <= M00_AXI_0_arburst_s;
		M00_AXI_0_arcache <= M00_AXI_0_arcache_s;
		M00_AXI_0_arlen <= M00_AXI_0_arlen_s;
		M00_AXI_0_arlock <= M00_AXI_0_arlock_s;
		M00_AXI_0_arprot <= M00_AXI_0_arprot_s;
		M00_AXI_0_arqos <= M00_AXI_0_arqos_s;
		M00_AXI_0_arregion <= M00_AXI_0_arregion_s;
		M00_AXI_0_arsize <= M00_AXI_0_arsize_s;
		M00_AXI_0_aruser <= M00_AXI_0_aruser_s;
		M00_AXI_0_arvalid <= M00_AXI_0_arvalid_s;
		M00_AXI_0_awaddr <= M00_AXI_0_awaddr_s;
		M00_AXI_0_awburst <= M00_AXI_0_awburst_s;
		M00_AXI_0_awcache <= M00_AXI_0_awcache_s;
		M00_AXI_0_awlen <= M00_AXI_0_awlen_s;
		M00_AXI_0_awlock <= M00_AXI_0_awlock_s;
		M00_AXI_0_awprot <= M00_AXI_0_awprot_s;
		M00_AXI_0_awqos <= M00_AXI_0_awqos_s;
		M00_AXI_0_awregion <= M00_AXI_0_awregion_s;
		M00_AXI_0_awsize <= M00_AXI_0_awsize_s;
		M00_AXI_0_awuser <= M00_AXI_0_awuser_s;
		M00_AXI_0_awvalid <= M00_AXI_0_awvalid_s;
		M00_AXI_0_bready <= M00_AXI_0_bready_s;
		M00_AXI_0_rready <= M00_AXI_0_rready_s;
		M00_AXI_0_wdata <= M00_AXI_0_wdata_s;
		M00_AXI_0_wlast <= M00_AXI_0_wlast_s;
		M00_AXI_0_wstrb <= M00_AXI_0_wstrb_s;
		M00_AXI_0_wvalid <= M00_AXI_0_wvalid_s;
		S00_AXI_arready <= S00_AXI_arready_s;
		S00_AXI_awready <= S00_AXI_awready_s;
		S00_AXI_bid <= S00_AXI_bid_s;
		S00_AXI_bresp <= S00_AXI_bresp_s;
		S00_AXI_bvalid <= S00_AXI_bvalid_s;
		S00_AXI_rdata <= S00_AXI_rdata_s;
		S00_AXI_rid <= S00_AXI_rid_s;
		S00_AXI_rlast <= S00_AXI_rlast_s;
		S00_AXI_rresp <= S00_AXI_rresp_s;
		S00_AXI_rvalid <= S00_AXI_rvalid_s;
		S00_AXI_wready <= S00_AXI_wready_s;
		stop_ack_0 <= stop_ack_0_s;
	end

endmodule