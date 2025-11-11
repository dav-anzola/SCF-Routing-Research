#ifndef ROUTING_HEADER_H
#define ROUTING_HEADER_H

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/stats-module.h"
#include "ns3/header.h"
#include "ns3/address.h"

#include <iomanip>



using namespace ns3;

class RoutingHeader : public Header
{
  public:
    RoutingHeader();
    ~RoutingHeader() override;
 
    /**
     * Set the header data.
     * @param data The data.
     */
    void SetData(uint16_t data);

    void SetNodeId(Ptr<Socket> socket);

    void SetClusterId(uint16_t id);

    void SetSenderAddress(InetSocketAddress ad);

    void SetForwardingAddress(InetSocketAddress ad);

    void SetDestinationAddress(InetSocketAddress ad);
   
    uint16_t GetData() const;
    uint16_t GetNodeId() const;
    uint16_t GetClusterId() const;
    InetSocketAddress GetForwardingAddress()const;
    InetSocketAddress GetDestinationAddress() const;
    InetSocketAddress GetSenderAddress() const;
    
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    void Print(std::ostream& os) const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    uint32_t GetSerializedSize() const override;
 
  private:
    uint16_t m_data; //!< Header data
    uint16_t node_id;
    uint16_t cluster_id;
    InetSocketAddress forwarding_address = InetSocketAddress(Ipv4Address("0.0.0.0"), 0);
    InetSocketAddress destination_address = InetSocketAddress(Ipv4Address("0.0.0.0"), 0);
    InetSocketAddress sender_address = InetSocketAddress(Ipv4Address("0.0.0.0"), 0);
   // vector current_position
};
 
RoutingHeader::RoutingHeader(){
    m_data=999;
    node_id=999;
    cluster_id=999;

}
 
RoutingHeader::~RoutingHeader()
{
}
 
TypeId
RoutingHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::RoutingHeader").SetParent<Header>().AddConstructor<RoutingHeader>();
    return tid;
}
 
TypeId
RoutingHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}
 
void
RoutingHeader::Print(std::ostream& os) const
{
    // This method is invoked by the packet printing
    // routines to print the content of my header.
    // os << "data=" << m_data << std::endl;
    os << "data=" << m_data;
}
 
uint32_t
RoutingHeader::GetSerializedSize() const
{
    
    return 24;
}
 
void
RoutingHeader::Serialize(Buffer::Iterator start) const
{
    start.WriteHtonU16(m_data);
    start.WriteHtonU16(node_id);
    start.WriteHtonU16(cluster_id);
    //serializing ip addresses
    start.WriteHtonU32(forwarding_address.GetIpv4().Get());
    start.WriteHtonU16(forwarding_address.GetPort());

    start.WriteHtonU32(destination_address.GetIpv4().Get());
    start.WriteHtonU16(destination_address.GetPort());

    start.WriteHtonU32(sender_address.GetIpv4().Get());
    start.WriteHtonU16(sender_address.GetPort());
}
 
uint32_t
RoutingHeader::Deserialize(Buffer::Iterator start)
{
    // we can deserialize two bytes from the start of the buffer.
    // we read them in network byte order and store them
    // in host byte order.
    m_data = start.ReadNtohU16();
    node_id= start.ReadNtohU16();
    cluster_id=start.ReadNtohU16();
   //iP stuff
    Ipv4Address fwdAddr(start.ReadNtohU32());
    uint16_t fwdPort = start.ReadNtohU16();
    forwarding_address = InetSocketAddress(fwdAddr, fwdPort);

    Ipv4Address destAddr(start.ReadNtohU32());
    uint16_t destPort = start.ReadNtohU16();
    destination_address = InetSocketAddress(destAddr, destPort);

    Ipv4Address senderAddr(start.ReadNtohU32());
    uint16_t senderPort = start.ReadNtohU16();
    sender_address = InetSocketAddress(senderAddr, senderPort);

    return 24;
}
 
void
RoutingHeader::SetData(uint16_t data)
{
    
    m_data = data;
}
 void RoutingHeader::SetNodeId(Ptr<Socket> socket){
   node_id=socket->GetNode()->GetId();
    
 }

 void RoutingHeader::SetClusterId(uint16_t id){

    cluster_id=id;

 }

 void RoutingHeader::SetForwardingAddress(InetSocketAddress ad){

    forwarding_address=ad;

 }
 void RoutingHeader::SetDestinationAddress(InetSocketAddress ad){

    destination_address=ad;

 }
void RoutingHeader::SetSenderAddress(InetSocketAddress ad){
    sender_address=ad;
}

uint16_t
RoutingHeader::GetData() const
{
    return m_data;
}
 uint16_t 
 RoutingHeader::GetNodeId() const
 {
    return node_id;
 }

 uint16_t 
 RoutingHeader::GetClusterId() const{
    return cluster_id;
 }

 InetSocketAddress
 RoutingHeader::GetForwardingAddress() const{
    return forwarding_address;
 }

 InetSocketAddress
 RoutingHeader::GetDestinationAddress() const{
    return destination_address;
 }

 InetSocketAddress
 RoutingHeader::GetSenderAddress() const{
    return sender_address;
 }

 #endif


/*
 int
main(int argc, char* argv[])
{
    // Enable the packet printing through Packet::Print command.
    Packet::EnablePrinting();
 
    // instantiate a header.
    RoutingHeader sourceHeader;
    sourceHeader.SetData(2);
 
    // instantiate a packet
    Ptr<Packet> p = Create<Packet>();
 
    // and store my header into the packet.
    p->AddHeader(sourceHeader);
 
    // print the content of my packet on the standard output.
    p->Print(std::cout);
    std::cout << std::endl;
 
    // you can now remove the header from the packet:
    RoutingHeader destinationHeader;
    p->RemoveHeader(destinationHeader);
 
    // and check that the destination and source
    // headers contain the same values.
    NS_ASSERT(sourceHeader.GetData() == destinationHeader.GetData());
 
    return 0;
}*/
    
