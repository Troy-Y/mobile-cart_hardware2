-makelib ies_lib/xilinx_vip -sv \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/axi4stream_vip_axi4streampc.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/axi_vip_axi4pc.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/xil_common_vip_pkg.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/axi4stream_vip_pkg.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/axi_vip_pkg.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/axi4stream_vip_if.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/axi_vip_if.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/clk_vip_if.sv" \
  "E:/Xilinx2019/Vivado/2019.1/data/xilinx_vip/hdl/rst_vip_if.sv" \
-endlib
-makelib ies_lib/axi_infrastructure_v1_1_0 \
  "../../../../919106840315.srcs/sources_1/bd/ps919106840315/ipshared/ec67/hdl/axi_infrastructure_v1_1_vl_rfs.v" \
-endlib
-makelib ies_lib/axi_vip_v1_1_5 -sv \
  "../../../../919106840315.srcs/sources_1/bd/ps919106840315/ipshared/d4a8/hdl/axi_vip_v1_1_vl_rfs.sv" \
-endlib
-makelib ies_lib/processing_system7_vip_v1_0_7 -sv \
  "../../../../919106840315.srcs/sources_1/bd/ps919106840315/ipshared/8c62/hdl/processing_system7_vip_v1_0_vl_rfs.sv" \
-endlib
-makelib ies_lib/xil_defaultlib \
  "../../../bd/ps919106840315/ip/ps919106840315_processing_system7_0_0/sim/ps919106840315_processing_system7_0_0.v" \
  "../../../bd/ps919106840315/sim/ps919106840315.v" \
-endlib
-makelib ies_lib/xil_defaultlib \
  glbl.v
-endlib

