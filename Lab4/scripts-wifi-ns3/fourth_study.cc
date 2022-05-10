#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/internet-module.h"
#include <iostream>
#include "ns3/flow-monitor-module.h"
#include <cstring>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Fourth_Study");

//Print simulation time
void CurrentSimulationTime ()
{
	std::cout << "SimTime: "<< Simulator::Now().GetSeconds() << "\n";
	Simulator::Schedule(Seconds(1), &CurrentSimulationTime);
}

int main(int argc, char** argv) 
{
	//command variables
	uint32_t run=1;
	bool verbose=false; 
	double txpower = 20; //dBm = 100mW
	double antenna_gain = 0;  //dBi
	double distance = 100; //distance between nodes in meters
	double duration = 10; //s duration of the flow

	CommandLine cmd;
	
	cmd.AddValue("run", "set run", run);
	cmd.AddValue("verbose", "turn on all WifiNetDevice log components", verbose);
	cmd.AddValue("txpower", "transceiver tx power in dB", txpower);
	cmd.AddValue("antenna_gain", "RF gain of the antenna in dBi", antenna_gain);
	cmd.AddValue("distance", "distance between sender and sink nodes (relay in the middle) in m", distance);
	cmd.AddValue("duration", "duration of the TCP connection en s", duration); 
    	cmd.Parse(argc, argv);

	//logs filenames
	std::ostringstream o_filename;
	o_filename << "scratch/4th_Study_"<<"TotalDistance="<<distance<<"m_"<<"Duration="<<duration<<"s_"<<"Run="<<run;
	

	SeedManager::SetSeed(1);
	SeedManager::SetRun(run);

    uint16_t freq_MHz = 5180; //channel 36 of 5 GHz WiFi Band
	double freq = freq_MHz * 1e6; //Hz
	uint8_t channel_bw = 20; //MHz


	//Node container for the two WiFi nodes
    NodeContainer adhocNodes;
    adhocNodes.Create(3);

	WifiHelper wifi;
	if (verbose)
	{
		wifi.EnableLogComponents ();  // Turn on all Wifi logging
	}
	wifi.SetStandard (WIFI_STANDARD_80211a); //802.11a  (supports 11a operation at 5GHz frequency band)

	  
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(freq));
	
	YansWifiPhyHelper wifiPhy;// =  YansWifiPhyHelper::Default ();
	wifiPhy.Set("Frequency", UintegerValue(freq_MHz));
	wifiPhy.Set("ChannelWidth", UintegerValue(channel_bw));
   	wifiPhy.Set ("RxGain", DoubleValue(antenna_gain) ); 
	wifiPhy.Set ("TxGain", DoubleValue(antenna_gain) );
	wifiPhy.Set ("TxPowerStart", DoubleValue(txpower) );
	wifiPhy.Set ("TxPowerEnd", DoubleValue(txpower) );
	wifiPhy.SetErrorRateModel ("ns3::NistErrorRateModel"); //just to make sure NistError is selected
    	
	// ns-3 supports RadioTap and Prism tracing extensions for 802.11
    wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 
	
    wifiPhy.SetChannel (wifiChannel.Create ());
 
	// Add a mac and enable Auto-rate control (Minstrel)
	WifiMacHelper wifiMac;
	wifi.SetRemoteStationManager ("ns3::MinstrelWifiManager");
	
	// Set it to adhoc mode
    wifiMac.SetType ("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, adhocNodes.Get (0));
	devices.Add (wifi.Install (wifiPhy, wifiMac, adhocNodes.Get (1)));
	devices.Add (wifi.Install (wifiPhy, wifiMac, adhocNodes.Get (2)));
   
    //Set RSSI Threshold for preamble detection
	Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PreambleDetectionModel/$ns3::ThresholdPreambleDetectionModel/MinimumRssi", DoubleValue(-94));
   
 	//now set the geographic positions for the nodes and Mobility Model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
   	positionAlloc->Add (Vector (0.0, 0.0, 0.0)); //node 0 Sink node
    positionAlloc->Add (Vector (distance/2, 0.0, 0.0)); //node 1 Relay node
	positionAlloc->Add (Vector (distance, 0.0, 0.0)); //node 2 Sender node
    
    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); //constant position mobility model for the two (static) nodes
    mobility.Install (adhocNodes);
  
	//install internet stack and assign IP addresses for the nodes
	InternetStackHelper internet;
	internet.Install (adhocNodes);

	Ipv4AddressHelper ipv4;
	NS_LOG_INFO ("Assign IP Addresses.");
	ipv4.SetBase ("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);
   
	//TCP FLOW FROM node 2 (sender) TO node 1 (sink)
	//SINK NODE IN BS	
	PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9)); //port number 9
	ApplicationContainer SINKapps = sink.Install(adhocNodes.Get(0)); //node 0 is the Sink
	SINKapps.Start(Seconds(1.0));
	SINKapps.Stop(Seconds(duration+10.0));

	//SENDER NODE
	BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (interfaces.GetAddress (0), 9));
	// Set the amount of data to send in bytes.  Zero is unlimited.
	source.SetAttribute ("MaxBytes", UintegerValue (0));
	ApplicationContainer sourceApps = source.Install (adhocNodes.Get (2));
	sourceApps.Start (Seconds (2.0));
	sourceApps.Stop (Seconds (duration+2.0));

	//SET MSS of TCP (not used for UDP)
	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1448)); //1448 tomaso

	//route from node 3 to node 1 through node 2
	//and
	//route from node 1 to node 3 through node 2
	Ipv4StaticRoutingHelper staticRoutingHelper;
	
	Ptr<Ipv4> ipv4_node1 = adhocNodes.Get(0)->GetObject<Ipv4> ();
	Ptr<Ipv4> ipv4_node3 = adhocNodes.Get(2)->GetObject<Ipv4> ();

	Ptr<Ipv4StaticRouting> staticRouting_node1 = staticRoutingHelper.GetStaticRouting(ipv4_node1);
	Ptr<Ipv4StaticRouting> staticRouting_node3 = staticRoutingHelper.GetStaticRouting(ipv4_node3);

    staticRouting_node1->AddHostRouteTo(Ipv4Address("10.0.0.3"), Ipv4Address("10.0.0.2"), 1);
    staticRouting_node3->AddHostRouteTo(Ipv4Address("10.0.0.1"), Ipv4Address("10.0.0.2"), 1);

	// Tracing
    wifiPhy.EnablePcap (o_filename.str(), devices.Get(0));
	
	// Flow monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();

	//just to print the current simulation time (s)
	Simulator::Schedule(Seconds(1), &CurrentSimulationTime);

	Simulator::Stop (Seconds(duration+10.0));

	Simulator::Run ();
    Simulator::Destroy();

    //print flowmonitor statistics
    flowMonitor->CheckForLostPackets (); 
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

 	    std::cout << std::endl;
 	    std::cout << "Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress << std::endl;
    	std::cout << "       Tx Packets = " << iter->second.txPackets << std::endl;
    	std::cout << "       Rx Packets = " << iter->second.rxPackets << std::endl;
    	std::cout << "       Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps" << std::endl;
    }

	//o_filename << ".xml";
	//flowMonitor->SerializeToXmlFile(o_filename.str(), true, true);




    return 0;
}
