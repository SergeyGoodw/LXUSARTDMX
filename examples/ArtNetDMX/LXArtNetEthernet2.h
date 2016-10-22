/* LXArtNetEthernet2.h
   Copyright 2015 by Claude Heintz Design
   This code is in the public domain

	Art-Net(TM) Designed by and Copyright Artistic Licence Holdings Ltd.
*/

#ifndef LXARTNET_H
#define LXARTNET_H

#include <Arduino.h>
#include <EthernetUdp2.h>
#include <inttypes.h>
#include "LXDMXEthernet2.h"

#define ARTNET_PORT 0x1936
#define ARTNET_BUFFER_MAX 530
#define ARTNET_REPLY_SIZE 239
#define ARTNET_ADDRESS_OFFSET 17

#define ARTNET_ART_POLL 0x2000
#define ARTNET_ART_POLL_REPLY 0x2100
#define ARTNET_ART_DMX 0x5000
#define ARTNET_ART_ADDRESS 0x6000
#define ARTNET_NOP 0

/*!
@class LXArtNet
@abstract 
   LXArtNet partially implements the Art-Net Ethernet Communication Standard.

	LXArtNet is primarily a node implementation.  It supports output of a single universe
   of DMX data from the network.  It does not support merge and will only accept
   packets from the first IP address from which it receives an ArtDMX packet.
   This can be reset by sending an ArtAddress cancel merge command.
   
   When reading packets, LXArtNet will automatically respond to ArtPoll packets.
   Depending on the constructor used, it will either broadcast the reply or will
   reply directly to the sender of the poll.

   http://www.artisticlicence.com
*/
class LXArtNet : public LXDMXEthernet {

  public:
/*!
* @brief constructor with address used for ArtPollReply
* @param address sent in ArtPollReply
*/   
   LXArtNet  ( IPAddress address );
/*!
* @brief constructor creates broadcast address for Poll Reply
* @param address sent in ArtPollReply
* @param subnet_mask used to set broadcast address
*/  
	LXArtNet  ( IPAddress address, IPAddress subnet_mask );
   ~LXArtNet ( void );

/*!
* @brief UDP port used by protocol
*/   
   uint16_t dmxPort ( void ) { return ARTNET_PORT; }

/*!
* @brief universe for sending and receiving dmx
* @discussion First universe is zero for Art-Net.  High nibble is subnet, low nibble is universe.
* @return universe 0-255
*/   
   uint8_t universe           ( void );
/*!
* @brief set universe for sending and receiving
* @discussion First universe is zero for Art-Net.  High nibble is subnet, low nibble is universe.
* @param u universe 0-255
*/
   void    setUniverse        ( uint8_t u );
/*!
* @brief set subnet/universe for sending and receiving
* @discussion First universe is zero for Art-Net.  Sets separate nibbles: high/subnet, low/universe.
* @param s subnet 0-16
* @param u universe 0-16
*/
   void    setSubnetUniverse  ( uint8_t s, uint8_t u );
/*!
* @brief set universe for sending and receiving
* @discussion First universe is zero for Art-Net.  High nibble is subnet, low nibble is universe.
* 0x7f is no change, otherwise if high bit is set, low nibble becomes universe (subnet remains the same)
* @param u universe 0-16 + flag 0x80
*/
   void    setUniverseAddress ( uint8_t u );
/*!
* @brief set subnet for sending and receiving
* @discussion First universe is zero for Art-Net.  High nibble is subnet, low nibble is universe.
* 0x7f is no change, otherwise if high bit is set, low nibble becomes subnet (universe remains the same)
* @param s subnet 0-16 + flag 0x80
*/
   void    setSubnetAddress   ( uint8_t s );

 /*!
 * @brief number of slots (aka addresses or channels)
 * @discussion Should be minimum of ~24 depending on actual output speed.  Max of 512.
 * @return number of slots/addresses/channels
 */     
   int  numberOfSlots    ( void );
 /*!
 * @brief set number of slots (aka addresses or channels)
 * @discussion Should be minimum of ~24 depending on actual output speed.  Max of 512.
 * @param n number of slots 1 to 512
 */  
   void setNumberOfSlots ( int n );
 /*!
 * @brief get level data from slot/address/channel
 * @param slot 1 to 512
 * @return level for slot (0-255)
 */  
   uint8_t  getSlot      ( int slot );
 /*!
 * @brief set level data (0-255) for slot/address/channel
 * @param slot 1 to 512
 * @param value level 0 to 255
 */  
   void     setSlot      ( int slot, uint8_t value );
 /*!
 * @brief direct pointer to dmx buffer uint8_t[]
 * @return uint8_t* to dmx data buffer
 */ 
   uint8_t* dmxData      ( void );

 /*!
 * @brief read UDP packet
 * @param eUDP EthernetUDP object to be used for getting UDP packet
 * @return 1 if packet contains dmx
 */    
   uint8_t  readDMXPacket       ( EthernetUDP eUDP );
 /*!
 * @brief process packet, reading it into _packet_buffer
 * @param eUDP EthernetUDP (used for Poll Reply if applicable)
 * @return Art-Net opcode of packet
 */
   uint16_t readArtNetPacket    ( EthernetUDP eUDP );
 /*!
 * @brief send Art-Net ArtDMX packet for dmx output from network
 * @param eUDP EthernetUDP object to be used for sending UDP packet
 * @param to_ip target address
 */    
   void     sendDMX             ( EthernetUDP eUDP, IPAddress to_ip );
 /*!
 * @brief send ArtPoll Reply packet for dmx output from network
 * @discussion If broadcast address is defined by passing subnet to constructor, reply is broadcast
 *             Otherwise, reply is unicast to remoteIP belonging to the sender of the poll
 * @param eUDP EthernetUDP object to be used for sending UDP packet
 */  
   void     send_art_poll_reply ( EthernetUDP eUDP );
   
  private:
/*!
* @brief buffer that holds contents of incoming or outgoing packet
* @discussion There is no double buffer for dmx data.
*             readArtNetPacket fills the buffer with the payload of the incoming packet.
*             Previous dmx data is invalidated.
*/
  	uint8_t   _packet_buffer[ARTNET_BUFFER_MAX];
/// number of slots/address/channels
  	int       _dmx_slots;
/// high nibble subnet, low nibble universe
  	uint8_t   _universe;
/// sequence number for sending ArtDMX packets
  	uint8_t   _sequence;

/// address included in poll replies 	
  	IPAddress _my_address;
/// if subnet is supplied in constructor, holds address to broadcast poll replies
  	IPAddress _broadcast_address;
/// first sender of an ArtDMX packet (subsequent senders ignored until cancelMerge)
  	IPAddress _dmx_sender;

/*!
* @brief checks packet for "Art-Net" header
* @return opcode if Art-Net packet
*/
  	uint16_t  parse_header        ( void );	
/*!
* @brief utility for parsing ArtAddress packets
* @return opcode in case command changes dmx data
*/
   uint16_t  parse_art_address   ( void );
   
};

#endif // ifndef LXARTNET_H