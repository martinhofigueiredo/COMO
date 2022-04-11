# OLSR and IPv6 using Mininet
This  work  was  prepared  by  Prof.  Manuel  Ricardo  and  Filipe  Abrantes,  and  later  updated  and 
adapted to Mininet by Eduardo Nuno Almeida. 
---

This laboratory work consists in setting up an IPv6 ad hoc network controlled by the OLSR routing protocol. It is prepared to be  used in FEUP’s NetLab or using the Mininet network emulator. This guide is organized in three sections. The first section explains the Mininet software  that will be used to run an emulated network testbed replicating the testbed created in FEUP’s NetLab. __Read it carefully__, as it explains how to use Mininet, as well as how to run OLSR and Wireshark on Mininet. The  second  part  contains  the  original  OLSR  guide,  explaining  the  network  setup  and  tests  to perform. Please note that this guide corresponds to the original guide for the  OLSR lab  work, which was originally thought to be done in FEUP’s NetLab. As such, all configurations related to a “PC” should be applied to the corresponding Mininet host (e.g., PC1 corresponds to Mininet host h1). The final section contains the questions that **should** be answered on the final report. 

# 1. Mininet
In this work, we will use the Mininet software to emulate a wireless network testbed. Mininet is a  network emulator  which  creates  a  network  of  virtual  hosts,  switches,  controllers  and  links. Mininet hosts are virtualized in Linux network namespaces, which allows the creation of multiple independent TCP/IP network stacks. Also, since they are running on top of the Linux kernel, all Linux software works exactly in the same way. 

