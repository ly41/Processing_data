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


#ifndef DEBUG_H
#define DEBUG_H
#include "compiler.h"

//verbosity increases as the debug level increases
#define DBG_MAX_LEVEL 10
#define DBG_FSM    5
#define DBG_ITRACE 4    //any interrupts
#define DBG_INFO   3    //general information
#define DBG_TX     2    //send a character
#define DBG_ERR    1    //serious error


//single char debug
#define DBG_CHAR_TXSTART     '!'
#define DBG_CHAR_TXBUSY      '@'
#define DBG_CHAR_RXOFLOW     '+'
#define DBG_CHAR_RXRCV       '$'
#define DBG_CHAR_ACKPKT      '%'
#define DBG_CHAR_MEMFULL     '^'
#define DBG_CHAR_TXFIN       '&'
#define DBG_CHAR_OURACK      '*'
#define DBG_CHAR_MACFULL     '~'   // MAC RX is full
#define DBG_CHAR_FIFO_OFLOW      '{'


#ifdef LRWPAN_DEBUG

#define DEBUG_STRING(level,s) if (0) conPrintROMString(s)
#define DEBUG_CHAR(level,c) if (0) halPutch(c)
#define DEBUG_UINT8(level,x) if (0) conPrintUINT8(x)
#define DEBUG_UINT16(level,x) if (0) conPrintUINT16(x)
#define DEBUG_UINT32(level,x) if (0) conPrintUINT32(x)
#define DEBUG_PRINTNEIGHBORS(level) if (0) dbgPrintNeighborTable()
#define DEBUG_PRINTPACKET(level, ptr, plen) if (0) dbgPrintPacket(ptr, plen)

//#define DEBUG_STRING(level,s) if (debug_level >= level) conPrintROMString(s)
//#define DEBUG_CHAR(level,c) if (debug_level >= level) halPutch(c)
//#define DEBUG_UINT8(level,x) if (debug_level >= level) conPrintUINT8(x)
//#define DEBUG_UINT16(level,x) if (debug_level >= level) conPrintUINT16(x)
//#define DEBUG_UINT32(level,x) if (debug_level >= level) conPrintUINT32(x)
//#define DEBUG_PRINTNEIGHBORS(level) if (debug_level >= level) dbgPrintNeighborTable()
//#define DEBUG_PRINTPACKET(level, ptr, plen) if (debug_level >= level)dbgPrintPacket(ptr, plen)

BYTE *dbgPrintMacPacket (BYTE *ptr, BYTE len);
void dbgPrintPacket(BYTE *ptr, BYTE plen);
void dbgPrintNeighborTable(void);

#define DEBUG_SET_LEVEL(x) debug_level=x

#else

#define DEBUG_STRING(level,s)
#define DEBUG_CHAR(level,c)
#define DEBUG_UINT8(level,x)
#define DEBUG_UINT16(level,x)
#define DEBUG_UINT32(level,x)
#define DEBUG_PRINTNEIGHBORS(level, x)
#define DEBUG_PRINTPACKET(level, ptr, plen) 


#endif

extern BYTE debug_level;
#endif


