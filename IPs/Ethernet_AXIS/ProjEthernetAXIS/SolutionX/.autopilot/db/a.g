#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /home/sameh/StateMover/StateLink/IPs/Ethernet_AXIS/ProjEthernetAXIS/SolutionX/.autopilot/db/a.g.bc ${1+"$@"}
