

#ifndef ROUTING_APP_H
#define ROUTING_APP_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "routingHeader.h"

namespace ns3
{

class Application;

/**
 * Routing - a simple Application sending packets.
 */
class RoutingApp : public Application
{
  public:
    RoutingApp();
    ~RoutingApp() override;

    /**
     * Register this type.
     * \return The TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Setup the socket.
     * \param socket The socket.
     * \param forwardingAddress The forwarding address.
     * \param finalAddress The final destination
     * \param packetSize The packet size to transmit.
     * \param nPackets The number of packets to transmit.
     * \param dataRate the data rate to use.
     */
    void Setup(Ptr<Socket> socket,
               Ipv4Address forwardingAddress,
               uint16_t forwardingPort,           
               Ipv4Address finalAddress,
               uint16_t finalPort,
               uint32_t packetSize,
               uint32_t nPackets,
               DataRate dataRate,
               bool creatingPackets
              );

  private:
    void StartApplication() override;
    void StopApplication() override;

    /// Schedule a new two hop transmission.
    void ScheduleTwoHopTx();

    /// Send a two hop packet.
    void SendTwoHopPacket();
    //
    void ForwardPacket(Ptr<Packet> packet);
    //Recieving a packet, used for forwarder

    void ReceivingTwoHopPacket(Ptr<Socket> socket);

    Ptr<Socket> m_socket;   //!< The transmission socket.
    InetSocketAddress m_destinationPeer;         //!< The destination address.
    InetSocketAddress m_forwardingPeer;
    uint32_t m_packetSize;  //!< The packet size.
    uint32_t m_nPackets;    //!< The number of packets to send.
    DataRate m_dataRate;    //!< The data rate to use.
    EventId m_sendEvent;    //!< Send event.
    bool m_running;         //!< True if the application is running.
    bool m_creatingPackets;  //!< True if application is creating and sending packets. 
    uint32_t m_packetsSent; //!< The number of packets sent.
};

} // namespace ns3

#endif /* Routing_APP_H */
