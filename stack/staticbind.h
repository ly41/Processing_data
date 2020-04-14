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



// Default static bind resolution functions


#ifndef STATICBIND_H
#define STATICBIND_H
BOOL evbBindTableIterInit(BYTE srcEP, SADDR srcSADDR, BYTE cluster);
BOOL evbResolveBind(BYTE *dstEP, SADDR *dstSADDR);


#ifdef LRWPAN_COORDINATOR
#ifdef LRWPAN_USE_DEMO_STATIC_BIND

typedef struct  _BINDENTRY {
	LADDR src;
	BYTE  srcEP;
	LADDR dst;
	BYTE  dstEP;
	BYTE  cluster;
}BINDENTRY;

//modify this table to fill in your static binds!!!!
//for simplicity, this uses the LSB of the example nodes
//as the endpoint number

/* binds
First binding pair allows RFD1, RFD2 to exchange indirect messages
RFD1 (LSB= 0x70) -> RFD2 (LSB= 0x71)
RFD2 (LSB= 0x71) -> RFD1 (LSB= 0x70)


Second binding pair allows endpoint on Coordinator to exchange
indirect messages with RFD1

Coord(LSB=0x6F) -> RFD1 (LSB= 0x70)
RFD1 (LSB= 0x70) -> Coord(LSB=0x6F)
*/


#define BINDTABLE_ENTRIES  4
BINDENTRY bindtable[BINDTABLE_ENTRIES] =
#ifdef MCC18
//my PICDEMZ nodes
{
	//this entry allows node 0x64 to send to 0x85
	{0x64,0x03,0x53,0x30,0x00,0xA3,0x04,0x00,  //SRC, little endian
	0x64,                                      //src endpoint
        0x85,0x03,0x53,0x30,0x00,0xA3,0x04,0x00,  //DST, little endian
	0x85,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER},                         //Cluster
	//this is the reverse direction, allows node 0x85 -> 0x64
    {0x85,0x03,0x53,0x30,0x00,0xA3,0x04,0x00,  //SRC, little endian
	0x85,                                      //src endpoint
   0x64,0x03,0x53,0x30,0x00,0xA3,0x04,0x00,  //DST, little endian
	0x64,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER},                         //Cluster

	//this entry allows node 0x64 to send to 0x22 
	{0x64,0x03,0x53,0x30,0x00,0xA3,0x04,0x00,  //SRC, little endian
	0x64,                                      //src endpoint
    0x22,0x84,0x53,0x30,0x00,0xA3,0x04,0x00,  //DST, little endian
	0x22,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER},                         //Cluster
	//this is the reverse direction, allows node 0x22 -> 0x64
    {0x22,0x84,0x53,0x30,0x00,0xA3,0x04,0x00,  //SRC, little endian
	0x22,                                      //src endpoint
    0x64,0x03,0x53,0x30,0x00,0xA3,0x04,0x00,  //DST, little endian
	0x64,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER}                         //Cluster

};

#else
//my CC2430, WIN32 nodes
{
	//this entry allows node 0x70 to send to 0x71
	{0x70,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //SRC, little endian
	0x70,                                      //src endpoint
    0x71,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //DST, little endian
	0x71,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER},                         //Cluster
	//this is the reverse direction, allows node 0x71 -> 0x70
    {0x71,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //SRC, little endian
	0x71,                                      //src endpoint
    0x70,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //DST, little endian
	0x70,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER},                         //Cluster

	//this entry allows node 0x70 to send to 0x6F 
	{0x70,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //SRC, little endian
	0x70,                                      //src endpoint
    0x6F,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //DST, little endian
	0x6F,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER},                         //Cluster
	//this is the reverse direction, allows node 0x6F -> 0x70
    {0x6F,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //SRC, little endian
	0x6F,                                      //src endpoint
    0x70,0x21,0x01,0x00,0x00,0x4B,0x12,0x00,  //DST, little endian
	0x70,                                      //dst endpoint 
	LRWPAN_APP_CLUSTER}                         //Cluster

};
#endif

#endif
#endif

#endif




