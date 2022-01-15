/* -- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -- */
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
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"
#include <unistd.h>

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    
//   n2   n1  wifiapnode 
//                

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int
main (int argc, char *argv[])
{
for(int i = 0; i < 100; i++){
  bool tracing = true, verbose = true;

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (2);
// Yet another Network simulator -  
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));
// active probing - keeps checking for wifi signals


  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

    // this is for ap
  mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
// THis is for the ap
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
// This is the network
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("172.12.30.0", "255.255.255.0");
  Ipv4InterfaceContainer ApInterfaces;
  ApInterfaces = address.Assign (apDevices);
  address.Assign (staDevices);

  UdpEchoServerHelper echoServer (69);

  ApplicationContainer serverApps = echoServer.Install (wifiApNode.Get (0));
  serverApps.Start (Seconds (0.50));
  serverApps.Stop (Seconds (20.0));

  UdpEchoClientHelper echoClient (ApInterfaces.GetAddress (0), 69);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes);
  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (20.0));
// This is the routing algo
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (20.0));

  if (tracing == true)
    {
      phy.EnablePcap ("assg2works", apDevices);
    }
  printf("\n-----------------------\n");
  printf("x = %d, x2 = %d\n\n", 100+i,100+i);


    Ptr<ConstantPositionMobilityModel> s1 = wifiStaNodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> s2 = wifiStaNodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> s3 = wifiApNode.Get (0)->GetObject<ConstantPositionMobilityModel> ();

    //    std::cout << "\n\nPosition: (" << x << "," << y << "):\n";
    s1->SetPosition (Vector (100+i, 100, 0));
    s2->SetPosition (Vector (100+i, 100, 0));
    s3->SetPosition (Vector (100, 100, 0));
     
  Simulator::Run ();
  Simulator::Destroy ();
}
  return 0;
}