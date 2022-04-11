/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

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


  NodeContainer router_12;
  router_12.Create (2);

  NodeContainer csmanodes;
  csmanodes.Add (router_12.Get (0));
  csmanodes.Create (4);

  //Creating Channels
  //point to point 5 Mbs
  PointToPointHelper pointtopoint;
  pointtopoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointtopoint.SetChannelAttribute ("Delay", StringValue ("20ms"));

  PointToPointHelper pointtopoint2;
  pointtopoint.SetDeviceAttribute ("DataRate", StringValue ("2Mbps"));
  pointtopoint.SetChannelAttribute ("Delay", StringValue ("20ms"));


  NetDeviceContainer router_12devices;
  router_12devices = pointtopoint.Install (router_12);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmadevices;
  csmadevices = csma.Install (csmanodes);

  InternetStackHelper stack;
  stack.Install (router_12.Get (1));
  stack.Install (csmanodes);

  Ipv4AddressHelper address;

  //router 1 and router 2
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer router_12interfaces;
  router_12interfaces = address.Assign (router_12devices);

	//LAN topology 
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmainterfaces;
  csmainterfaces = address.Assign (csmadevices);
	
	Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
	em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
	csmadevices.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  //router 2 and router 3
  NodeContainer router_23;
	router_23.Add(router_12.Get(1));
  router_23.Create (1);
	

	NetDeviceContainer router_23devices;
	router_23devices = pointtopoint2.Install(router_23);
	stack.Install (router_23.Get(1));
	address.SetBase("10.1.3.0", "255.255.255.0");

Ipv4InterfaceContainer router_23interfaces;
  router_23interfaces = address.Assign(router_23devices);

 NodeContainer serverNode_1;
  //r3 -- FTP
 

  serverNode_1.Add(router_23.Get(1));
  serverNode_1.Create(1);
  NetDeviceContainer serverNode_1devices;
  serverNode_1devices = pointtopoint.Install(serverNode_1);
  stack.Install (serverNode_1.Get(1));
  address.SetBase("10.1.4.0", "255.255.255.0");

  Ipv4InterfaceContainer serverNode_1interfaces;
  serverNode_1interfaces = address.Assign(serverNode_1devices);



	//r3 -- server_2
  NodeContainer serverNode_2;

  serverNode_2.Add(router_23.Get(1));
  serverNode_2.Create(1);
  NetDeviceContainer serverNode_2devices;
  serverNode_2devices = pointtopoint.Install(serverNode_2);
  stack.Install (serverNode_2.Get(1));
  address.SetBase("10.1.5.0", "255.255.255.0");

  Ipv4InterfaceContainer serverNode_2interfaces;
  serverNode_2interfaces = address.Assign(serverNode_2devices);

/*
	//r2 -- wap
	nodecontainer r2wap;
	r2wap.add(router_12.get(1));
	r2wap.create(1);
	netdevicecontainer r2wapdevices;
	r2wapdevices = pointtopoint.install(r2wap);
	
	stack.install(r2wap.get(1));
	address.setbase("10.1.6.0", "255.255.255.0");

	ipv4interfacecontainer r2wapinterfaces;
	r2wapinterfaces = address.assign(r2wapdevices);


  nodecontainer wifistanodes;
  wifistanodes.create (3);
  nodecontainer wifiapnode = r2wap.get(1);

  yanswifichannelhelper channel = yanswifichannelhelper::default ();
  yanswifiphyhelper phy;
  phy.setchannel (channel.create ());

  wifihelper wifi;
  wifi.setremotestationmanager ("ns3::aarfwifimanager");

  wifimachelper mac;
  ssid ssid = ssid ("ns-3-ssid");
  mac.settype ("ns3::stawifimac",
               "ssid", ssidvalue (ssid),
               "activeprobing", booleanvalue (false));

  netdevicecontainer stadevices;
  stadevices = wifi.install (phy, mac, wifistanodes);

  mac.settype ("ns3::apwifimac",
               "ssid", ssidvalue (ssid));

  netdevicecontainer apdevices;
  apdevices = wifi.install (phy, mac, wifiapnode);

  mobilityhelper mobility;

  mobility.setpositionallocator ("ns3::gridpositionallocator",
                                 "minx", doublevalue (0.0),
                                 "miny", doublevalue (0.0),
                                 "deltax", doublevalue (5.0),
                                 "deltay", doublevalue (10.0),
                                 "gridwidth", uintegervalue (3),
                                 "layouttype", stringvalue ("rowfirst"));

  mobility.setmobilitymodel ("ns3::randomwalk2dmobilitymodel",
                             "bounds", rectanglevalue (rectangle (-50, 50, -50, 50)));
  mobility.install (wifistanodes);

  mobility.setmobilitymodel ("ns3::constantpositionmobilitymodel");
  mobility.install (wifiapnode);

  stack.install(wifistanodes);
 	address.setbase("10.1.7.0", "255.255.255.0");
	ipv4interfacecontainer wifinodeinterfaces;


  wifinodeinterfaces = address.assign (stadevices);
  address.assign (apdevices);
    */



  //Traffic sending
  //FTP


  //server
	uint16_t port = 9;
  BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (csmainterfaces.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (30));
  ApplicationContainer sourceApps = source.Install (serverNode_1.Get (1));
  sourceApps.Start (Seconds (1.0));
  sourceApps.Stop (Seconds (100.0));


  //client
   PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (csmanodes.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (100.0));

  AsciiTraceHelper ascii;
      pointtopoint.EnableAsciiAll (ascii.CreateFileStream ("teja.tr"));
      pointtopoint.EnablePcapAll ("teja", false);



/*
// server_2
	Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (250));
  	Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("5kb/s"));

	//Server
 	OnOffHelper serverHelper ("ns3::TcpSocketFactory",  InetSocketAddress (wifiNodeInterfaces.GetAddress(1), port));
  	serverHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	serverHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
ApplicationContainer serverApps;
      serverApps.Add (serverHelper.Install (serverNode_2.Get(1)));
	

	  //Client
	PacketSinkHelper sinkserver_2 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkAppsserver_2 = sinkserver_2.Install (wifiStaNodes.Get (1));
  sinkAppsserver_2.Start (Seconds (3.0));
  sinkAppsserver_2.Stop (Seconds (100.0));
*/

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	Simulator::Stop(Seconds(100.0));
  Simulator::Run ();
  Simulator::Destroy ();
   Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  return 0;
}

