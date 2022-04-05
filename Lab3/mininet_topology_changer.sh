# !/bin/sh
#
# Coded by: Eduardo Nuno Almeida [eduardo.n.almeida@inesctec.pt]
# 			Pedro Fortuna [pedro.fortuna@fe.up.pt]
#
# This script should be run on every PC.
#
# Note: To add a topology, just add another elif (else-if)
# to the final if clause.


################################################
# VARIABLES
################################################
MAC1="00:00:00:00:00:01"
MAC2="00:00:00:00:00:02"
MAC3="00:00:00:00:00:03"
MAC4="00:00:00:00:00:04"

PC_NUMBER=$(ls /proc/sys/net/ipv6/conf | grep "h.*-eth0" | cut -c2)

################################################
# FUNCTIONS
################################################
function blockmac
{
	# params ($@): List of MAC address to block on this PC.

	for mac in $@; do
		ip6tables -t filter -A INPUT -m mac --mac-source $mac -j DROP
	done
}

function flushfilters
{
	ip6tables -F
}

################################################
# MAIN
################################################
topology_number="$1"

# Check program usage
if [ -z $topology_number ]; then
	echo "OLSR Topology Changer utility for Mininet"
	echo "Usage: ./mininet_topology_changer.sh <topology_number>"
	echo "    topology_number:"
	echo "      - 0 for full mesh (every node sees every other node)"
	echo "      - 1 for basic inline scenario (1-2-3-4)"
	echo "      - 2 for star topology centered on node 2"

	exit
fi

# Change topology
if [ $topology_number -eq "0" ]; then
	echo "PC $PC_NUMBER: Setting full mesh topology..."
	flushfilters

elif [ $topology_number -eq "1" ]; then
	echo "PC $PC_NUMBER: Setting the Ad-Hoc line scenario topology..."

	flushfilters

	if [ $PC_NUMBER -eq "1" ]; then
		blockmac $MAC3 $MAC4
	elif [ $PC_NUMBER -eq "2" ]; then
		blockmac $MAC4
	elif [ $PC_NUMBER -eq "3" ]; then
		blockmac $MAC1
	elif [ $PC_NUMBER -eq "4" ]; then
		blockmac $MAC1 $MAC2
	fi

elif [ $topology_number -eq "2" ]; then
	echo "PC $PC_NUMBER: Setting the Ad-Hoc star topology based on Node 2 scenario..."

	flushfilters

	if [ $PC_NUMBER -eq "1" ]; then
		blockmac $MAC3 $MAC4
	#elif [ $PC_NUMBER -eq "2" ]; then

	elif [ $PC_NUMBER -eq "3" ]; then
		blockmac $MAC1 $MAC4
	elif [ $PC_NUMBER -eq "4" ]; then
		blockmac $MAC1 $MAC3
	fi

else
	echo "PC $PC_NUMBER: Unknown topology! Edit mininet_topology_changer.sh and add a new topology if you like."
fi

echo "Done."
