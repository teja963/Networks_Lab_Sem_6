
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-module.h"
#include "ns3/error-model.h"
#include "ns3/netanim-module.h"

#define lanNum 4
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Secondscriptexample");

int 
main (int argc, char *argv[])
{
  bool verbose = false;



  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }


  NodeContainer r1r2;
  r1r2.Create (2);

  NodeContainer csmanodes;
  csmanodes.Add (r1r2.Get (0));
  csmanodes.Create (lanNum);

  //Creating Channels
  //point to point 5 Mbs
  PointToPointHelper pointtopoint;
  pointtopoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointtopoint.SetChannelAttribute ("Delay", StringValue ("20ms"));

  PointToPointHelper pointtopoint2;
  pointtopoint.SetDeviceAttribute ("DataRate", StringValue ("2Mbps"));
  pointtopoint.SetChannelAttribute ("Delay", StringValue ("20ms"));


  NetDeviceContainer r1r2devices;
  r1r2devices = pointtopoint.Install (r1r2);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmadevices;
  csmadevices = csma.Install (csmanodes);

  InternetStackHelper stack;
  stack.Install (r1r2.Get (1));
  stack.Install (csmanodes);

  Ipv4AddressHelper address;

  //router 1 and router 2
  address.SetBase ("172.16.21.0", "255.255.255.0");
  Ipv4InterfaceContainer r1r2interfaces;
  r1r2interfaces = address.Assign (r1r2devices);
	//LAN topology
  address.SetBase ("172.16.20.0", "255.255.255.0");
  Ipv4InterfaceContainer csmainterfaces;
  csmainterfaces = address.Assign (csmadevices);
	
	Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
	em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
	csmadevices.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  //router 2 and router 3
  NodeContainer r2r3;
	r2r3.Add(r1r2.Get(1));
  r2r3.Create (1);
	
 // PointToPointHelper pointtopointr2r3;
 // pointtopointr2r3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
 // pointtopointr2r3.SetChannelAttribute ("Delay", StringValue ("2ms"));
	NetDeviceContainer r2r3devices;
	r2r3devices = pointtopoint2.Install(r2r3);
	stack.Install (r2r3.Get(1));
	address.SetBase("172.16.22.0", "255.255.255.0");

Ipv4InterfaceContainer r2r3interfaces;
  r2r3interfaces = address.Assign(r2r3devices);


  //r3 -- FTP
  NodeContainer r3Ftp;

  r3Ftp.Add(r2r3.Get(1));
  r3Ftp.Create(1);
  NetDeviceContainer r3Ftpdevices;
  r3Ftpdevices = pointtopoint.Install(r3Ftp);
  stack.Install (r3Ftp.Get(1));
  address.SetBase("172.16.23.0", "255.255.255.0");

  Ipv4InterfaceContainer r3Ftpinterfaces;
  r3Ftpinterfaces = address.Assign(r3Ftpdevices);



	//r3 -- CBR
  NodeContainer r3CBR;

  r3CBR.Add(r2r3.Get(1));
  r3CBR.Create(1);
  NetDeviceContainer r3CBRdevices;
  r3CBRdevices = pointtopoint.Install(r3CBR);
  stack.Install (r3CBR.Get(1));
  address.SetBase("172.16.24.0", "255.255.255.0");

  Ipv4InterfaceContainer r3CBRinterfaces;
  r3CBRinterfaces = address.Assign(r3CBRdevices);


	//r2 -- WAP
	NodeContainer r2WAP;
	r2WAP.Add(r1r2.Get(1));
	r2WAP.Create(1);
	NetDeviceContainer r2WAPdevices;
	r2WAPdevices = pointtopoint.Install(r2WAP);
	
	stack.Install(r2WAP.Get(1));
	address.SetBase("172.16.25.0", "255.255.255.0");

	Ipv4InterfaceContainer r2WAPinterfaces;
	r2WAPinterfaces = address.Assign(r2WAPdevices);


  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (3);
  NodeContainer wifiApNode = r2WAP.Get(1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);

  stack.Install(wifiStaNodes);
 	address.SetBase("172.16.26.0", "255.255.255.0");
	Ipv4InterfaceContainer wifiNodeInterfaces;


  wifiNodeInterfaces = address.Assign (staDevices);
  address.Assign (apDevices);




  //Traffic sending
  //FTP


  //server
	uint16_t port = 9;
  BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (csmainterfaces.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (30));
  ApplicationContainer sourceApps = source.Install (r3Ftp.Get (1));
  sourceApps.Start (Seconds (1.0));
  sourceApps.Stop (Seconds (100.0));


  //client
   PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (csmanodes.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (100.0));

  AsciiTraceHelper ascii;
      pointtopoint.EnableAsciiAll (ascii.CreateFileStream ("assgn.tr"));
      pointtopoint.EnablePcapAll ("assgn", false);




// CBR
	Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (250));
  	Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("5kb/s"));

	//Server
 	OnOffHelper serverHelper ("ns3::TcpSocketFactory",  InetSocketAddress (wifiNodeInterfaces.GetAddress(1), port));
  	serverHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	serverHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
ApplicationContainer serverApps;
      serverApps.Add (serverHelper.Install (r3CBR.Get(1)));
	

	  //Client
	PacketSinkHelper sinkCBR ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkAppsCBR = sinkCBR.Install (wifiStaNodes.Get (1));
  sinkAppsCBR.Start (Seconds (3.0));
  sinkAppsCBR.Stop (Seconds (100.0));



//
  //UdpEchoServerHelper echoserver (9);
////
  //ApplicationContainer ServerApp1 = echoserver.Install (r3Ftp.Get (1));
  //ApplicationContainer ServerApp2 = echoserver.Install (r3CBR.Get (1));
  //ServerApp1.Start (Seconds (1.0));
  //ServerApp1.Stop (Seconds (10.0));
  //ServerApp2.Start (Seconds (1.0));
  //ServerApp2.Stop (Seconds (10.0));
////
  //UdpEchoClientHelper echoclient1 (r3Ftpinterfaces.GetAddress (1), 9);
  //echoclient1.SetAttribute ("MaxPackets", UintegerValue (1));
  //echoclient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoclient1.SetAttribute ("PacketSize", UintegerValue (1024));
////
  //ApplicationContainer clientapp1 = echoclient1.Install (csmanodes.Get (5));
  //clientapp1.Start (Seconds (2.0));
  //clientapp1.Stop (Seconds (10.0));

  //UdpEchoClientHelper echoclient2 (r3CBRinterfaces.GetAddress (1), 9);
  //echoclient2.SetAttribute ("MaxPackets", UintegerValue (1));
  //echoclient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoclient2.SetAttribute ("PacketSize", UintegerValue (1024));
////
  //ApplicationContainer clientapp2 = echoclient2.Install (csmanodes.Get (5));
  //clientapp2.Start (Seconds (3.0));
  //clientapp2.Stop (Seconds (10.0));
//
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();



//  UdpEchoClientHelper echoclient3 (r3CBRinterfaces.GetAddress (1), 9);
//  echoclient3.SetAttribute ("MaxPackets", UintegerValue (1));
//  echoclient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
//  echoclient3.SetAttribute ("PacketSize", UintegerValue (1024));
////
//  ApplicationContainer clientapp3 = echoclient2.Install (wifiStaNodes.Get (0));
//  clientapp3.Start (Seconds (4.0));
//  clientapp3.Stop (Seconds (10.0));

  //pointToPoint.EnablePcapAll ("second");
  //csma.EnablePcap ("second", csmadevices.Get (2), true);
  //AnimationInterface anim("first.xml");

	Simulator::Stop(Seconds(100.0));
  Simulator::Run ();
  Simulator::Destroy ();
   Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  return 0;
}
