#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /home/sameh/StateMover/StateLink/IPs/Ethernet_AXI/ProjEthernetAXI/SolutionX/.autopilot/db/a.g.bc ${1+"$@"}
