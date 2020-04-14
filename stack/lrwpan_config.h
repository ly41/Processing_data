/*
 * "Copyright (c) 2006 Robert B. Reese ("AUTHOR")"
 * All rights reserved.
 * (R. Reese, reese@ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHOR" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHOR"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHOR" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHOR" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 * Files in this software distribution may have different usage
 * permissions, see the header in each file. Some files have NO permission
 * headers since parts of the original sources in these files
 * came from vendor  sources with no usage restrictions.
 *
 */

/*
V0.2 added PC-based binding         21/July/2006  RBR
V0.1 Initial Release                10/July/2006  RBR

*/



/*
  Contains configuration information for the stack
  Can be overridden by the user
*/

#ifndef LRWPAN_CONFIG_H
#define LRWPAN_CONFIG_H

#define LRWPAN_VERSION  "0.2.6"
/*******************************************
IEEE��չ��ַ�궨��
********************************************/
#ifdef aExtendedAddress
#define aExtendedAddress_B7 0x20
#define aExtendedAddress_B6 0x11
#define aExtendedAddress_B5 0x00
#define aExtendedAddress_B4 0x00
#define aExtendedAddress_B3 0x00
#define aExtendedAddress_B2 0x00
#define aExtendedAddress_B1 0x00
#define aExtendedAddress_B0 0x01
#endif	//aExtendedAddress
//this is only used if device does not have
//some other way to set the address
//if you redefine one byte, must redefine all bytes
#ifndef aExtendedAddress
#define aExtendedAddress_B7 0x00
#define aExtendedAddress_B6 0x00
#define aExtendedAddress_B5 0x00
#define aExtendedAddress_B4 0x00
#define aExtendedAddress_B3 0x00
#define aExtendedAddress_B2 0x00
#define aExtendedAddress_B1 0x00
#define aExtendedAddress_B0 0x00
#endif //aExtendedAddress

// size for dynamically allocation functions in memalloc
#ifndef LRWPAN_HEAPSIZE
#define LRWPAN_HEAPSIZE    10240
#endif

//only support 2.4GHz right now
#define LRWPAN_DEFAULT_FREQUENCY PHY_FREQ_2405M
#define LRWPAN_SYMBOLS_PER_SECOND   62500


#define LRWPAN_DEFAULT_START_CHANNEL  20   //valid channels are 11 to 26 for 2.4GHz.
//a zero indicates the channel should not be used, so 0xFFFFFFFF allows all 16 channels
//2.4GHz channels are in bits 11-26
#define LRWPAN_DEFAULT_CHANNEL_MASK 0xFFFFFFFF
//PANID to use for this network
#define LRWPAN_USE_STATIC_PANID      //if this is defined, then DEFAULT PANID always used
#define LRWPAN_DEFAULT_PANID 0x1347
#define LRWPAN_DEFAULT_CHANNEL_SCAN_DURATION 4

//maximum number of buffered RX packets in MAC layer
#define LRWPAN_MAX_MAC_RX_PKTS   4

//maximum number of packets waiting to be forwarded to other nodes in NWK layer
//only has to be defined for FFDs
#define LRWPAN_MAX_NWK_RX_PKTS   4


//maximum number of indirect packets waiting to be resolved
//only has to be defined by the coordinator.
#define LRWPAN_MAX_INDIRECT_RX_PKTS 2


/*
If LRWPAN_ENABLE_SLOW_TIMER is defined, then the HAL layer
will configure a timer for periodic interrupt using the SLOWTICKS_PER_SECOND value
Also, the hal layer will call the usr function usrSlowTimerInt() each time
the interrupt occurs.

If the slow timer is enabled, then the EVB switches will be sampled as this rate.

Look in the halStack.c file to see what timer resource is used for this.
It may be different from the timer resource used for the macTimer.

Disable this if you do not want to use this timer resource.

*/



//uncomment this if you want the ASSOC_RESPONSE, ASSOC_REQUEST to be 802.15.4 compatible
#define IEEE_802_COMPLY
//#define   LRWPAN_FORCE_ASSOCIATION_TARGET


//uncomment this if you want to force association to a particular target
//#ifdef LRWPAN_FORCE_ASSOCIATION_TARGET
//set the following to the long address of the parent to associate with
//if using forced association.
//if you use forced association, then you must NOT define IEEE_802_COMPLY
//as forced association depends upon our special associate request/response
#define parent_addr_B0 0x00
#define parent_addr_B1 0x12
#define parent_addr_B2 0x4B
#define parent_addr_B3 0x00
#define parent_addr_B4 0x00
#define parent_addr_B5 0x01
#define parent_addr_B6 0x21
#define parent_addr_B7 0x6F
//#endif

//MAC Capability Info

//if either router or coordinator, then one of these must be defined
//#define LRWPAN_COORDINATOR
//#define LRWPAN_ROUTER



#if (defined (LRWPAN_COORDINATOR) || defined (LRWPAN_ROUTER) )
#define LRWPAN_FFD
#define LRWPAN_ROUTING_CAPABLE
#endif
#if !defined (LRWPAN_FFD)
#define LRWPAN_RFD
#endif

//define this if ACMAIN POWERED
#define LRWPAN_ACMAIN_POWERED
//define this if Receiver on when idle
#define LRWPAN_RCVR_ON_WHEN_IDLE
//define this if capable of RX/TX secure frames
//#define LRWPAN_SECURITY_CAPABLE



//comment this if you want the phy to call the EVBPOLL function
//do this if you want to poll EVB inputs during the stack idle
//time
#define LRWPAN_PHY_CALL_EVBPOLL

#define LRWPAN_ZIGBEE_PROTOCOL_ID   0
#define LRWPAN_ZIGBEE_PROTOCOL_VER  0
#define LRWPAN_STACK_PROFILE  0         //indicates this is a closed network.
#define LRWPAN_APP_PROFILE    0xFFFF    //filter data packets by this profile number
#define LRWPAN_APP_CLUSTER    0x2A    //default cluster, random value for debugging

//define this if you want the beacon payload to comply with the Zigbee standard
#define LRWPAN_ZIGBEE_BEACON_COMPLY

//Network parameters



//this is a magic number exchanged with nodes wishing to join our
//network. If they do not match this number, then they are rejected.
//Sent in beacon payload
#define LRWPAN_NWK_MAGICNUM_B0 0x0AA
#define LRWPAN_NWK_MAGICNUM_B1 0x055
#define LRWPAN_NWK_MAGICNUM_B2 0x0C3
#define LRWPAN_NWK_MAGICNUM_B3 0x03C



/*
These numbers determine affect the size of the neighbor
table, and the maximum number of nodes in the network,
and how short addresses are assigned to nodes.

*/
#define LRWPAN_MAX_DEPTH                   5
#define LRWPAN_MAX_ROUTERS_PER_PARENT      4
//these are total children, includes routers!
#define LRWPAN_MAX_CHILDREN_PER_PARENT    17
#define LRWPAN_MAX_NON_ROUTER_CHILDREN    (LRWPAN_MAX_CHILDREN_PER_PARENT-LRWPAN_MAX_ROUTERS_PER_PARENT)



//if using Indirect addressing, then this number determines the
//maximum size of the address table map used by the coordinator
//that matches long addresses with short addresses.
//You should set this value to the maximum number of RFDs that
//use indirect addressing. The value below is just chosen for testing.
//Its minimum value must be the maximum number of neighbors (RFDs+Routers+1), as this
//is also used in the neighbor table construction.
#ifdef LRWPAN_COORDINATOR
#define LRWPAN_MAX_ADDRESS_MAP_ENTRIES   (LRWPAN_MAX_CHILDREN_PER_PARENT*2)
#endif


#ifndef LRWPAN_MAX_ADDRESS_MAP_ENTRIES
//this is the minimum value for this, minimum value used by routers
#ifdef LRWPAN_ROUTER
#define LRWPAN_MAX_ADDRESS_MAP_ENTRIES (LRWPAN_MAX_CHILDREN_PER_PARENT+1)
#endif
#ifdef LRWPAN_RFD
#define LRWPAN_MAX_ADDRESS_MAP_ENTRIES 1
#endif
#endif

#ifdef LRWPAN_FFD
#if (LRWPAN_MAX_ADDRESS_MAP_ENTRIES < (LRWPAN_MAX_CHILDREN_PER_PARENT+1))
#error "In lrwpan_config.h, LRWPAN_MAX_ADDRESS_MAP_ENTRIES too small!"
#endif
#endif



//these precalculated based upon MAX_DEPTH, MAX_ROUTERS, MAX_CHILDREN
//Coord at depth 0, only endpoints are at depth MAX_DEPTH
//LRWPAN_CSKIP_(MAX_DEPTH) must be a value of 0.
//this hardcoding supports a max depth of 10, should be PLENTY
//Use the spreadsheet supplied with the distribution to calculate these
#define LRWPAN_CSKIP_1     1446
#define LRWPAN_CSKIP_2      358
#define LRWPAN_CSKIP_3       86
#define LRWPAN_CSKIP_4       18
#define LRWPAN_CSKIP_5       0
#define LRWPAN_CSKIP_6       0
#define LRWPAN_CSKIP_7       0
#define LRWPAN_CSKIP_8       0
#define LRWPAN_CSKIP_9       0
#define LRWPAN_CSKIP_10      0


#define LRWPAN_NWK_MAX_RADIUS  LRWPAN_MAX_DEPTH*2

//Binding
//if the following is defined, then the EVB binding functions use
//the binding resolution functions defined in stack/staticbind.c
#define LRWPAN_USE_DEMO_STATIC_BIND

//Define this if you want to use the binding functions
//in pcbind.c/h that store the binding table on a PC client
//using the bindingdemo application
//#define LRWPAN_USE_PC_BIND
//PC_BIND_CACHE_SIZE only needed if USE_PC_BIND is defined
//number of bindings cached by the PC bind code
//#define LRWPAN_PC_BIND_CACHE_SIZE  4

//these are defaults, can be changed by user
#define LRWPAN_APS_ACK_WAIT_DURATION 200  //in milliseconds, for depth=1
#define LRWPAN_NWK_JOIN_WAIT_DURATION 200  //in milliseconds!



#define LRWPAN_APS_MAX_FRAME_RETRIES 1  //for acknowledge frames.
#define LRWPAN_MAC_MAX_FRAME_RETRIES 1  //for MAC ack requests .

//maximum number of endpoints, controls size of endpoint data structure
//in APS.h
#define LRWPAN_MAX_ENDPOINTS    6

//data for node descriptor, not  currently used
#define LRWPAN_MAX_USER_PAYLOAD   93      //currently 93 bytes.
#define LRWPAN_MANUFACTURER_CODE  0x0000  //assigned by Zigbee Alliance



//unsupported at this time
// #define LRWPAN_ALT_COORDINATOR
// #define LRWPAN_SECURITY_ENABLED

//HAL Stuff

#define LRWPAN_ENABLE_SLOW_TIMER

#define SLOWTICKS_PER_SECOND 10
//#define LRWPAN_DEFAULT_BAUDRATE 57600
#define LRWPAN_ASYNC_RX_BUFSIZE   32

#define LRWPAN_ASYNC_INTIO

#if (defined(LRWPAN_USE_PC_BIND)&&defined(LRWPAN_COORDINATOR)&&!defined(LRWPAN_ASYNC_INTIO))
//ASYNC RX interrupt IO *must* be used with coordinator if using PC Binding application
//so that serial input from the PC client is not missed.
#define LRWPAN_ASYNC_INTIO

#endif


#endif
