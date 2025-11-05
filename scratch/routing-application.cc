


#include "ns3/applications-module.h"
#include "routing-application.h"


using namespace ns3;
/* This app will simulate a two hop sending of a packet 


*/


RoutingApp::RoutingApp()
    : m_socket(nullptr),
      m_destinationPeer(InetSocketAddress(Ipv4Address::GetAny(), 0)),
      m_forwardingPeer(InetSocketAddress(Ipv4Address::GetAny(), 0)),   
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_creatingPackets(false),
      m_packetsSent(0)
{
}

RoutingApp::~RoutingApp()
{
    m_socket = nullptr;
}

/* static */
TypeId
RoutingApp::GetTypeId()
{
    static TypeId tid = TypeId("RoutingApp")
                            .SetParent<Application>()
                            .SetGroupName("Routing")
                            .AddConstructor<RoutingApp>();
    return tid;
}

void
RoutingApp::Setup(Ptr<Socket> socket,
               Ipv4Address forwardingAddress,
               uint16_t forwardingPort,           
               Ipv4Address finalAddress,
               uint16_t finalPort,
               uint32_t packetSize,
               uint32_t nPackets,
               DataRate dataRate,
               bool creatingPackets
                )
{
    m_socket = socket;
    m_forwardingPeer =InetSocketAddress(forwardingAddress,forwardingPort);
    m_destinationPeer=InetSocketAddress(finalAddress,finalPort);
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_creatingPackets=creatingPackets;
}

void
RoutingApp::StartApplication()
{
    std::cout<<"Starting App!"<<std::endl;
    m_running = true;
    m_packetsSent = 0;
    uint16_t port = 8000;
    InetSocketAddress local=InetSocketAddress (Ipv4Address::GetAny(),port);
    m_socket->Bind(local);
    m_socket->SetRecvCallback(MakeCallback(&RoutingApp::ReceivingTwoHopPacket, this));
     if(m_creatingPackets){
        SendTwoHopPacket();
     }
     
    }
    
     


void
RoutingApp::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsPending())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
RoutingApp::SendTwoHopPacket()
{
    
    
    RoutingHeader twoHopHead;
    twoHopHead.SetNodeId(m_socket);
    
    twoHopHead.SetDestinationAddress(m_destinationPeer);
    twoHopHead.SetForwardingAddress(m_forwardingPeer);
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    packet->AddHeader(twoHopHead);
    std::cout<<"About to send packet!"<<std::endl;
    if(m_socket->SendTo(packet,0,m_forwardingPeer)==-1)
    {std::cout<<"Failed to send packet!"<<std::endl;}
    else{std::cout<<"Success sent packet!"<<std::endl;}

   
    if (++m_packetsSent < m_nPackets)
    {
        ScheduleTwoHopTx();
    }
}
void
RoutingApp::ReceivingTwoHopPacket(Ptr<Socket> socket){
    std::cout<<"Inside ReceivingTwoHopPacket"<<std::endl;
    Ptr<Packet> packet;
    RoutingHeader Head1;
    Address senderAddr;
    Ptr<Node> node = socket->GetNode();            // get the node of this socket
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();      // get its Ipv4 object
    Ipv4Address nodeAddress = ipv4->GetAddress(1, 0).GetLocal();

    if((packet=socket->RecvFrom(senderAddr))){
        packet->PeekHeader(Head1);
       std::cout<<"Inside recvFrom"<<std::endl;
        InetSocketAddress pktForwardingAddress=Head1.GetForwardingAddress();
        InetSocketAddress pktDestinationAddress=Head1.GetDestinationAddress();
        InetSocketAddress senderAddress= InetSocketAddress::ConvertFrom(senderAddr);
        std::cout<<"My address is "<<nodeAddress<<" destination address is: "<<pktDestinationAddress.GetIpv4()<<" forwarding address is "<<pktForwardingAddress.GetIpv4()<<std::endl;
        if(pktDestinationAddress.GetIpv4()==nodeAddress){
            std::cout<<"Node "<<socket->GetNode()->GetId()
                <<" Rx from "<<senderAddress.GetIpv4()
                <<" Two hop succesful!"<<std::endl;
        }
        
        else if(pktForwardingAddress.GetIpv4()==nodeAddress){ std::cout<<"Why is this not WORKING"<<std::endl;
            std::cout<<"Node "<<socket->GetNode()->GetId()
                << "Rx from "<<senderAddress.GetIpv4()
                <<" with destination "<<pktDestinationAddress.GetIpv4()<<std::endl;
            ForwardPacket(packet);
        }
    }

}
void
RoutingApp::ForwardPacket(Ptr<Packet> packet){
    RoutingHeader FrwdHeader;
    packet->PeekHeader(FrwdHeader);
    InetSocketAddress destinationAddress=FrwdHeader.GetDestinationAddress();
    

    if(m_socket->SendTo(packet,0,destinationAddress)==-1){
        std::cout<<"Failed to forward packet"<<std::endl;}
    else{
        std::cout<<"Node "<<m_socket->GetNode()->GetId()<<"forwarded packet to: "<<destinationAddress.GetIpv4()<<std::endl;
    }


}
    


void
RoutingApp::ScheduleTwoHopTx()
{
    if (m_running)
    {   

        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate()))); //must this be adjusted?
        m_sendEvent = Simulator::Schedule(tNext, &RoutingApp::SendTwoHopPacket, this);
    }
}
/*
int
main(int argc, char* argv[])
{
    // placholder main for scratch
    return 0;
} */
