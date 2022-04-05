"""
Mininet COMO OLSR
Author: Eduardo Nuno Almeida [eduardo.n.almeida@inesctec.pt]

Python script to define the Mininet topology below.


 PC5             PC1          PC2      PC3     PC4
  |        eth1 |   | eth0     |        |       |
  |             |   |          |        |       |
  |             |   |          |        |       |
  --- Switch2 ---   ----------- Switch1 ---------

"""

from mininet.topo import Topo

class OlsrTopo(Topo):
	""" COMO OLSR topology """

	def __init__(self):
		"Create custom topo."

		# Initialize topology
		Topo.__init__(self)

		# Add hosts and switches
		pc1 = self.addHost('h1', mac='00:00:00:00:00:01')
		pc2 = self.addHost('h2', mac='00:00:00:00:00:02')
		pc3 = self.addHost('h3', mac='00:00:00:00:00:03')
		pc4 = self.addHost('h4', mac='00:00:00:00:00:04')
		pc5 = self.addHost('h5', mac='00:00:00:00:00:05')

		sw1 = self.addSwitch('s1')
		sw2 = self.addSwitch('s2')

		# Add links
		self.addLink(pc1, sw1)
		self.addLink(pc2, sw1)
		self.addLink(pc3, sw1)
		self.addLink(pc4, sw1)

		self.addLink(pc1, sw2)
		self.addLink(pc5, sw2)


# Adding the 'topos' dict with a key/value pair to generate our newly defined
# topology enables one to pass in '--topo=mytopo' from the command lin
topos = { 'olsr_topo': ( lambda: OlsrTopo() ) }
