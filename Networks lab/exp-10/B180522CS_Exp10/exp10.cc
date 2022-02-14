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
#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Wifi-2-nodes-fixed");


void print_dist(double i)
{
	printf("Node 1 distance from AP: %lf\n", i);
	printf("Node 2 distance from AP: %lf\n", i);
	printf("Distance between Node 1 and Node 2: %lf\n\n", 2*i);
}

	
int main (int argc, char *argv[])
{
	//for(int i=0;i<100;i++)
	{
		bool verbose = true;
		uint32_t nWifi = 2;
	
		if (verbose)
		{
			LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
			LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
		}
	
		NodeContainer wifiStaNodes, wifiApNode;
	
		wifiStaNodes.Create (nWifi);
		wifiApNode.Create (1);
	
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
	
		NetDeviceContainer apDevice;
		apDevice = wifi.Install (phy, mac, wifiApNode);
	
		MobilityHelper mobility;
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
										"MinX", DoubleValue (0.0),
										"MinY", DoubleValue (0.0),
										"DeltaX", DoubleValue (5.0),
										"DeltaY", DoubleValue (5.0),
										"GridWidth", UintegerValue (3),
										"LayoutType", StringValue ("RowFirst"));
	
		mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		mobility.Install (wifiApNode);
		
		mobility.SetMobilityModel ("ns3::WaypointMobilityModel");
		mobility.Install (wifiStaNodes);
		
		Ptr<WaypointMobilityModel> ueWaypointMobility = DynamicCast<WaypointMobilityModel>( wifiStaNodes.Get(0)->GetObject<MobilityModel>());

		ueWaypointMobility->SetPosition(Vector (0.0, 0, 0));
		for(int i=1;i<100;i++) {
		 	ueWaypointMobility->AddWaypoint(Waypoint(Seconds(0+0.5*i),Vector(0.0 + i,0,0)));
		}
		
		Ptr<WaypointMobilityModel> ueWaypointMobility1 = DynamicCast<WaypointMobilityModel>( wifiStaNodes.Get(1)->GetObject<MobilityModel>());

		ueWaypointMobility1->SetPosition(Vector (0.0, 0, 0));
		for(int i=1;i<100;i++) {
		 	ueWaypointMobility1->AddWaypoint(Waypoint(Seconds(0+0.5*i),Vector(-0.0 - i,0,0)));
		}
		
		
		//mobility.Install (wifiStaNodes);
	
		InternetStackHelper stack;
		stack.Install (wifiApNode);
		stack.Install (wifiStaNodes);
	
		Ipv4AddressHelper address;
		Ipv4InterfaceContainer wifiInterfaces, wifiApInterface;
	
		address.SetBase ("192.168.2.0", "255.255.255.0");
		wifiApInterface = address.Assign (apDevice);
		wifiInterfaces = address.Assign (staDevices);
	
		UdpEchoServerHelper echoServer (8080); // Port # 9 0 and 1
	
		ApplicationContainer serverApps = echoServer.Install (wifiStaNodes.Get (0));
		serverApps.Start (Seconds (0.0));
		serverApps.Stop (Seconds (60.0));
	
		UdpEchoClientHelper echoClient (wifiInterfaces.GetAddress (0), 8080);
	
		echoClient.SetAttribute ("MaxPackets", UintegerValue (100));
		echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
		echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
	
		/*ApplicationContainer clientApp1 = echoClient.Install (wifiStaNodes.Get (0));
		clientApp1.Start (Seconds (2.0));
		clientApp1.Stop (Seconds (10.0));*/
	
		ApplicationContainer clientApp2 = echoClient.Install (wifiStaNodes.Get (1));
		clientApp2.Start (Seconds (0.0));
		clientApp2.Stop (Seconds (60.0));
	
		Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
		Simulator::Stop (Seconds (60.0));
	
		AnimationInterface anim ("anim3.xml");
		anim.SetMobilityPollInterval (Seconds (0.5));
		//anim.SetConstantPosition(wifiApNode.Get(0), 100.0, 100.0);
		//anim.SetConstantPosition(wifiStaNodes.Get(0), 100.0+i, 100.0);
		//anim.SetConstantPosition(wifiStaNodes.Get(1), 100.0-i, 100.0);
	
		printf("\n-------------------\n");
	
		for(int i=0; i<120; i++)
			Simulator::Schedule(Seconds(0.5*i), &print_dist, i);
		Simulator::Run ();
		

		//printf("Node 1 distance from AP: %lf\n", sqrt(mobility.GetDistanceSquaredBetween (wifiStaNodes.Get(0), wifiApNode.Get(0))));
		//printf("Node 2 distance from AP: %lf\n", sqrt(mobility.GetDistanceSquaredBetween (wifiStaNodes.Get(1), wifiApNode.Get(0))));
		//printf("Distance between Node 1 and Node 2: %lf\n", sqrt(mobility.GetDistanceSquaredBetween (wifiStaNodes.Get(1), wifiStaNodes.Get(0))));
		//fflush(stdout);
		// std::cout << "Distance: " << sqrt(mobility.GetDistanceSquaredBetween (wifiStaNodes.Get(0), wifiApNode.Get(0))) << endl;
		// std::cout << "Distance: " << sqrt(mobility.GetDistanceSquaredBetween (wifiStaNodes.Get(1), wifiApNode.Get(0))) << endl;
	
		Simulator::Destroy ();
	}
	return 0;
}