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

NS_LOG_COMPONENT_DEFINE ("Third_Study");

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
	std::string udp_data_rate = "1Mbps"; //bit/s

	CommandLine cmd;
	
	cmd.AddValue("run", "set run", run);
	cmd.AddValue("verbose", "turn on all WifiNetDevice log components", verbose);
	cmd.AddValue("txpower", "transceiver tx power in dB", txpower);
	cmd.AddValue("antenna_gain", "RF gain of the antenna in dBi", antenna_gain);
	cmd.AddValue("distance", "distance between nodes in m", distance);
	cmd.AddValue("duration", "duration of the TCP connection in s", duration); 
	cmd.AddValue("udp_data_rate", "transmission data-rate for the UDP connection in Mbps", udp_data_rate);
    cmd.Parse(argc, argv);
	
	//logs filenames
	std::ostringstream o_filename;
	o_filename << "scratch/3rd_Study_"<<"UDP_DataRate="<<udp_data_rate<<"_"<<"Duration="<<duration<<"s_"<<"Run="<<run;

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
    positionAlloc->Add (Vector (distance, 0.0, 0.0)); //node 1 Sender node TCP flow
	positionAlloc->Add (Vector (0.0, -distance, 0.0)); //node 2 Sender node UDP flow
    
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
	//SINK NODE IN BS (TCP)	
	PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9)); //port number 9
    ApplicationContainer SINKapps = sink.Install(adhocNodes.Get(0)); //node 0 is the Sink
    SINKapps.Start(Seconds(1.0));
    SINKapps.Stop(Seconds(duration+10.0));
	
	//SINK NODE IN BS (UDP)	
	PacketSinkHelper sinkUDP("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9)); //port number 9
    ApplicationContainer SINKappsUDP = sinkUDP.Install(adhocNodes.Get(0)); //node 0 is the Sink
    SINKappsUDP.Start(Seconds(1.0));
    SINKappsUDP.Stop(Seconds(duration+10.0));

	//SENDER NODE UDP
	OnOffHelper onoffUDP("ns3::UdpSocketFactory", interfaces.GetAddress (2)); //interface of node 2 (sender) as source
    onoffUDP.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    onoffUDP.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]")); //allways ON

	onoffUDP.SetAttribute("PacketSize", UintegerValue(1400));
    onoffUDP.SetAttribute("DataRate", DataRateValue(DataRate(udp_data_rate))); //to fully load the connection
    AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress (0), 9)); //pointing to the sink
    onoffUDP.SetAttribute("Remote", remoteAddress);

    ApplicationContainer SENDERapps = onoffUDP.Install(adhocNodes.Get(2)); //install in node 2 (sender)
    SENDERapps.Start(Seconds(2.5));
    SENDERapps.Stop(Seconds(duration+2.0));

	//SENDER NODE TCP
	BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (interfaces.GetAddress (0), 9));
	// Set the amount of data to send in bytes.  Zero is unlimited.
	source.SetAttribute ("MaxBytes", UintegerValue (0));
	ApplicationContainer sourceApps = source.Install (adhocNodes.Get (1));
	sourceApps.Start (Seconds (2.0));
	sourceApps.Stop (Seconds (duration+2.0));

	//SET MSS of TCP (not used for UDP)
	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1448)); //1448 tomaso

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
