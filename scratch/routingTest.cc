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

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "routing-application.h"
#include "routingHeader.h"
#include "routing-application.cc"

// Default Network Topology
//
//       10.1.1.0
// n0 ------n1------- n2
//   two hop 
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);


    NodeContainer nodes;
    nodes.Create(3);
    NodeContainer n0n1=NodeContainer(nodes.Get(0),nodes.Get(1));
    NodeContainer n1n2=NodeContainer(nodes.Get(1),nodes.Get(2));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devicesn0n1;
    devicesn0n1 = pointToPoint.Install(n0n1);
    
    NetDeviceContainer devicesn1n2;
    devicesn1n2 = pointToPoint.Install(n1n2);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer iface0_1 = address.Assign(devicesn0n1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer iface1_2 = address.Assign(devicesn1n2);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    uint16_t port= 8000;
    //Creating socket and application for sender 
    Ptr<Socket> senderSocket=Socket::CreateSocket(nodes.Get(0),UdpSocketFactory::GetTypeId());
    Ptr<RoutingApp> senderApp = CreateObject<RoutingApp>();
    senderApp->Setup(senderSocket,iface0_1.GetAddress(1),port,iface1_2.GetAddress(1),port,1024,1,DataRate("5Mbps"),true);//iface1_2's 0 and 1 correspond to node 1 and 2 
    nodes.Get(0)->AddApplication(senderApp);
    senderApp->SetStartTime(Seconds(2.0));
    senderApp->SetStopTime(Seconds(10.0));
    
    //Creating socket and application for forwarder
    Ptr<Socket> fwrdSocket=Socket::CreateSocket(nodes.Get(1),UdpSocketFactory::GetTypeId());
    Ptr<RoutingApp> fwrdApp = CreateObject<RoutingApp>();
    fwrdApp->Setup(fwrdSocket,iface0_1.GetAddress(0),port,iface0_1.GetAddress(0),port,1024,1,DataRate("5Mbps"),false);//dummy addresses
    nodes.Get(1)->AddApplication(fwrdApp);
    fwrdApp->SetStartTime(Seconds(1.0));
    fwrdApp->SetStopTime(Seconds(10.0));

    //Creating socket and application for forwarder
    Ptr<Socket> sinkSocket=Socket::CreateSocket(nodes.Get(2),UdpSocketFactory::GetTypeId());
    Ptr<RoutingApp> sinkApp = CreateObject<RoutingApp>();
    sinkApp->Setup(sinkSocket,iface0_1.GetAddress(0),port,iface0_1.GetAddress(0),port,1024,1,DataRate("5Mbps"),false); //need override for non sending apps? 
    nodes.Get(2)->AddApplication(sinkApp);
    sinkApp->SetStartTime(Seconds(1.0));
    sinkApp->SetStopTime(Seconds(10.0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
