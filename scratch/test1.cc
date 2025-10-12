//David Anzola's First ns-3 script
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/aodv-module.h"

/*This is a simple topology including 1 "car node" which is centered at the origin
and two "drone nodes" which are placed away from the car on both the z and the y axis.
All nodes are stationary and are using aodv routing protocol. Car node 1 has an echo client,
drone 2 has the echo server. Drone 1 is stationed somewhere inbetween. It is observed that without 
drone 1, the packets fail to reach drone 2, showing that the AODV routing is in effect

    Default Network Topology: Wifi 10.1.1.0

                *                       *
     *          |                       |
     |          |                       |
    n0 (car)    n1 (drone1)             n2 (drone 2)
    echo client                         echo server
*/ 

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Test1");
int
main(int argc, char* argv[]){


    NodeContainer carNode;
    carNode.Create(1);

    NodeContainer droneNodes;
    droneNodes.Create(2);
    

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    WifiMacHelper mac;
    Ssid ssid = Ssid("wifi");
    mac.SetType("ns3::AdhocWifiMac");
    
    WifiHelper wifi;
    
    
    NetDeviceContainer carDevices;
    carDevices=wifi.Install(phy,mac,carNode);

    NetDeviceContainer droneDevices;
    droneDevices=wifi.Install(phy,mac,droneNodes);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc= CreateObject<ListPositionAllocator>();
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    positionAlloc->Add(Vector(0.0,0.0,0.0));    //Car 1
    positionAlloc->Add(Vector(0.0,20.0,20.0));  //Drone 1, try setting Y to 2000, the packet will not reach its destination!
    positionAlloc->Add(Vector(0.0,70.0,25.0));  //Drone 2
    
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(carNode);
    mobility.Install(droneNodes);

    //AODV helper
    AodvHelper aodv;

    InternetStackHelper stack;
    stack.SetRoutingHelper(aodv);   //This allows for two hop routing.
    stack.Install(droneNodes);
    stack.Install(carNode);

    Ipv4AddressHelper adress;

    adress.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer carInterface;
    carInterface=adress.Assign(carDevices);
    Ipv4InterfaceContainer droneInterface;
    droneInterface=adress.Assign(droneDevices);
    
    //The echo server is installed on drone 2, the furthest node from car node. 
    UdpEchoServerHelper UdpEchoServer(9);
    ApplicationContainer serverApps= UdpEchoServer.Install(droneNodes.Get(1));
    serverApps.Start(Seconds(1));
    serverApps.Stop(Seconds(5));
    
    //This assigns the echo client to car, with the destination being drone 2
    //the application sends a single packet.
    UdpEchoClientHelper echoClient(droneInterface.GetAddress(1),9);
    echoClient.SetAttribute("MaxPackets",UintegerValue(1));
    echoClient.SetAttribute("PacketSize",UintegerValue(1024));
    echoClient.SetAttribute("Interval",TimeValue(Seconds(1.5)));
    
    ApplicationContainer clientApps= echoClient.Install(carNode);
    clientApps.Start(Seconds(2));
    clientApps.Stop(Seconds(4));

   // Ipv4GlobalRoutingHelper::PopulateRoutingTables(); ...It seems this is not needed

    Simulator::Stop(Seconds(16));
  
    LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
    
    //Generates Pcap files in /ns-3-dev
    phy.EnablePcap("Car",carDevices.Get(0),false);
    phy.EnablePcap("Drone1",droneDevices.Get(0),false);
    phy.EnablePcap("Drone2",droneDevices.Get(1),false);



    Simulator::Run();
    Simulator::Destroy();
    return 0;








    


    

    
    

}
