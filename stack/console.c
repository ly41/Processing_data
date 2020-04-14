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


#include "compiler.h"               //compiler specific
#include "hal.h"
#include "halStack.h"
#include "lrwpan_config.h"
#include "mac.h"
#include "console.h"


//utility print functions that do not use printf and expect ROM strings
//these assume that constants and strings are stored in code memory

void conPCRLF(void){
	conPrintROMString("\n");
}

void conPrintROMString_func (ROMCHAR *s) {
  while(*s) {
    if (*s == '\n') halPutch('\r');
    halPutch(*s);
    s++;
  }
}

void conPrintString (char *s) {
  while(*s) {
    if (*s == '\n') halPutch('\r');
    halPutch(*s);
    s++;
  }
}

void conPrintChar (char c) 
{ 
    halPutch(c);
}

void conPrintUINT8_noleader (UINT8 x) {
  BYTE c;
  c = (x>>4)& 0xf;
  if (c > 9) halPutch('A'+c-10);
   else halPutch('0'+c);
  //LSDigit
  c = x & 0xf;
  if (c > 9) halPutch('A'+c-10);
   else halPutch('0'+c);
}

void conPrintUINT8 (UINT8 x) {
  conPrintROMString("0x");
  conPrintUINT8_noleader(x);
}

void conPrintUINT16 (UINT16 x) {
 BYTE c;

 conPrintROMString("0x");
 c = (x >> 8);
 conPrintUINT8_noleader(c);
 c = (BYTE) x;
 conPrintUINT8_noleader(c);
}


void conPrintUINT32 (UINT32 x) {
 BYTE c;
 conPrintROMString("0x");
 c = (x >> 24);
 conPrintUINT8_noleader(c);
 c = (x >> 16);
 conPrintUINT8_noleader(c);
 c = (x >> 8);
 conPrintUINT8_noleader(c);
 c = x;
 conPrintUINT8_noleader(c);
}

//assumed little endian
void conPrintLADDR_bytes(BYTE *ptr) {
char i;
 conPrintROMString("0x");
 for (i=8;i!=0;i--){
   conPrintUINT8_noleader(*(ptr+i-1));
  }
}

void conPrintLADDR(LADDR *laddr){
 BYTE *ptr;

 ptr = &laddr->bytes[0];
 conPrintLADDR_bytes(ptr);
}

void conPrintConfig(void){
  BYTE b[8];

  conPrintROMString("MSState LRWPAN Version ");
  conPrintROMString(LRWPAN_VERSION)
  conPCRLF();
#ifdef LRWPAN_COORDINATOR
  conPrintROMString("Coordinator, ");
#endif
#ifdef LRWPAN_ROUTER
  conPrintROMString("Router, ");
#endif
#ifdef LRWPAN_RFD
  conPrintROMString("RFD, ");
#endif
  conPrintROMString("Address: ");
  halGetProcessorIEEEAddress(b);
  conPrintLADDR_bytes(b);
  conPCRLF();
#ifdef LRWPAN_USE_STATIC_PANID
  conPrintROMString("Using STATIC PANID: ");
  conPrintUINT16(LRWPAN_DEFAULT_PANID);
#else
  conPrintROMString("Using dynamic PANID");
#endif
  conPrintROMString(",Default Channel: ");
  conPrintUINT8(LRWPAN_DEFAULT_START_CHANNEL);
  conPCRLF();
  conPCRLF();

}

