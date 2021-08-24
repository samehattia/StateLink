module StateLink_AXIS_REG_WRAPPER (
	input wire axis_clk_0,
	input wire axis_rst_n_0
);
	parameter AXIS_DATA_WIDTH = 32;
	parameter AXIS_KEEP_WIDTH = AXIS_DATA_WIDTH / 8;

	wire M00_AXIS1_0_tready;
	wire [0:0] M00_AXIS_0_tready;
	wire [AXIS_DATA_WIDTH-1:0] S00_AXIS1_0_tdata;
	wire [AXIS_KEEP_WIDTH-1:0] S00_AXIS1_0_tkeep;
	wire [0:0] S00_AXIS1_0_tlast;
	wire [0:0] S00_AXIS1_0_tvalid;
	wire [AXIS_DATA_WIDTH-1:0] S00_AXIS_0_tdata;
	wire [AXIS_KEEP_WIDTH-1:0] S00_AXIS_0_tkeep;
	wire S00_AXIS_0_tlast;
	wire S00_AXIS_0_tvalid;
	wire decouple_1_0;

	(* dont_touch = "true" *) reg [AXIS_DATA_WIDTH-1:0] M00_AXIS1_0_tdata;
	(* dont_touch = "true" *) reg [AXIS_KEEP_WIDTH-1:0] M00_AXIS1_0_tkeep;
	(* dont_touch = "true" *) reg M00_AXIS1_0_tlast;
	(* dont_touch = "true" *) reg M00_AXIS1_0_tvalid;
	(* dont_touch = "true" *) reg [AXIS_DATA_WIDTH-1:0] M00_AXIS_0_tdata;
	(* dont_touch = "true" *) reg [AXIS_KEEP_WIDTH-1:0] M00_AXIS_0_tkeep;
	(* dont_touch = "true" *) reg [0:0] M00_AXIS_0_tlast;
	(* dont_touch = "true" *) reg [0:0] M00_AXIS_0_tvalid;
	(* dont_touch = "true" *) reg [0:0] S00_AXIS1_0_tready;
	(* dont_touch = "true" *) reg S00_AXIS_0_tready;
	(* dont_touch = "true" *) reg [1:0] stop_ack_1_0;

	(* dont_touch = "true" *) reg M00_AXIS1_0_tready_r;
	(* dont_touch = "true" *) reg [0:0] M00_AXIS_0_tready_r;
	(* dont_touch = "true" *) reg [AXIS_DATA_WIDTH-1:0] S00_AXIS1_0_tdata_r;
	(* dont_touch = "true" *) reg [AXIS_KEEP_WIDTH-1:0] S00_AXIS1_0_tkeep_r;
	(* dont_touch = "true" *) reg [0:0] S00_AXIS1_0_tlast_r;
	(* dont_touch = "true" *) reg [0:0] S00_AXIS1_0_tvalid_r;
	(* dont_touch = "true" *) reg [AXIS_DATA_WIDTH-1:0] S00_AXIS_0_tdata_r;
	(* dont_touch = "true" *) reg [AXIS_KEEP_WIDTH-1:0] S00_AXIS_0_tkeep_r;
	(* dont_touch = "true" *) reg S00_AXIS_0_tlast_r;
	(* dont_touch = "true" *) reg S00_AXIS_0_tvalid_r;
	(* dont_touch = "true" *) reg decouple_1_0_r;

	wire [AXIS_DATA_WIDTH-1:0] M00_AXIS1_0_tdata_s;
	wire [AXIS_KEEP_WIDTH-1:0] M00_AXIS1_0_tkeep_s;
	wire M00_AXIS1_0_tlast_s;
	wire M00_AXIS1_0_tvalid_s;
	wire [AXIS_DATA_WIDTH-1:0] M00_AXIS_0_tdata_s;
	wire [AXIS_KEEP_WIDTH-1:0] M00_AXIS_0_tkeep_s;
	wire [0:0] M00_AXIS_0_tlast_s;
	wire [0:0] M00_AXIS_0_tvalid_s;
	wire [0:0] S00_AXIS1_0_tready_s;
	wire S00_AXIS_0_tready_s;
	wire [1:0] stop_ack_1_0_s;

	wire axis_clk_bufg_0;
	wire axis_rst_n_bufg_0;
	assign axis_clk_bufg_0 = axis_clk_0;
	assign axis_rst_n_bufg_0 = axis_rst_n_0;

	StateLink_AXIS_wrapper STATELINK_AXIS_WRAPPER_ (
		.axis_clk_0(axis_clk_0),
		.axis_rst_n_0(axis_rst_n_0),
		.M00_AXIS1_0_tready(M00_AXIS1_0_tready_r),
		.M00_AXIS_0_tready(M00_AXIS_0_tready_r),
		.S00_AXIS1_0_tdata(S00_AXIS1_0_tdata_r),
		.S00_AXIS1_0_tkeep(S00_AXIS1_0_tkeep_r),
		.S00_AXIS1_0_tlast(S00_AXIS1_0_tlast_r),
		.S00_AXIS1_0_tvalid(S00_AXIS1_0_tvalid_r),
		.S00_AXIS_0_tdata(S00_AXIS_0_tdata_r),
		.S00_AXIS_0_tkeep(S00_AXIS_0_tkeep_r),
		.S00_AXIS_0_tlast(S00_AXIS_0_tlast_r),
		.S00_AXIS_0_tvalid(S00_AXIS_0_tvalid_r),
		.axis_clk_bufg_0(axis_clk_bufg_0),
		.axis_rst_n_bufg_0(axis_rst_n_bufg_0),
		.decouple_1_0(decouple_1_0_r),

		.M00_AXIS1_0_tdata(M00_AXIS1_0_tdata_s),
		.M00_AXIS1_0_tkeep(M00_AXIS1_0_tkeep_s),
		.M00_AXIS1_0_tlast(M00_AXIS1_0_tlast_s),
		.M00_AXIS1_0_tvalid(M00_AXIS1_0_tvalid_s),
		.M00_AXIS_0_tdata(M00_AXIS_0_tdata_s),
		.M00_AXIS_0_tkeep(M00_AXIS_0_tkeep_s),
		.M00_AXIS_0_tlast(M00_AXIS_0_tlast_s),
		.M00_AXIS_0_tvalid(M00_AXIS_0_tvalid_s),
		.S00_AXIS1_0_tready(S00_AXIS1_0_tready_s),
		.S00_AXIS_0_tready(S00_AXIS_0_tready_s),
		.stop_ack_1_0(stop_ack_1_0_s)
	);

	always @(posedge axis_clk_0)
	begin
		M00_AXIS1_0_tready_r <= M00_AXIS1_0_tready;
		M00_AXIS_0_tready_r <= M00_AXIS_0_tready;
		S00_AXIS1_0_tdata_r <= S00_AXIS1_0_tdata;
		S00_AXIS1_0_tkeep_r <= S00_AXIS1_0_tkeep;
		S00_AXIS1_0_tlast_r <= S00_AXIS1_0_tlast;
		S00_AXIS1_0_tvalid_r <= S00_AXIS1_0_tvalid;
		S00_AXIS_0_tdata_r <= S00_AXIS_0_tdata;
		S00_AXIS_0_tkeep_r <= S00_AXIS_0_tkeep;
		S00_AXIS_0_tlast_r <= S00_AXIS_0_tlast;
		S00_AXIS_0_tvalid_r <= S00_AXIS_0_tvalid;
		decouple_1_0_r <= decouple_1_0;

		M00_AXIS1_0_tdata <= M00_AXIS1_0_tdata_s;
		M00_AXIS1_0_tkeep <= M00_AXIS1_0_tkeep_s;
		M00_AXIS1_0_tlast <= M00_AXIS1_0_tlast_s;
		M00_AXIS1_0_tvalid <= M00_AXIS1_0_tvalid_s;
		M00_AXIS_0_tdata <= M00_AXIS_0_tdata_s;
		M00_AXIS_0_tkeep <= M00_AXIS_0_tkeep_s;
		M00_AXIS_0_tlast <= M00_AXIS_0_tlast_s;
		M00_AXIS_0_tvalid <= M00_AXIS_0_tvalid_s;
		S00_AXIS1_0_tready <= S00_AXIS1_0_tready_s;
		S00_AXIS_0_tready <= S00_AXIS_0_tready_s;
		stop_ack_1_0 <= stop_ack_1_0_s;
	end

endmodule