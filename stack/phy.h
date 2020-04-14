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



#ifndef PHY_H
#define PHY_H

#include "compiler.h"
#include "halStack.h"


#define aMaxPHYPacketSize 127
#define aTurnaroundTime 12

#define MAX_TX_TRANSMIT_TIME (SYMBOLS_TO_MACTICKS(300))  //a little long..

typedef enum _RADIO_STATUS_ENUM {
  RADIO_STATUS_OFF,
  RADIO_STATUS_RX_ON,
  RADIO_STATUS_TX_ON,
  RADIO_STATUS_RXTX_ON
}RADIO_STATUS_ENUM;

typedef struct _PHY_PIB {
  PHY_FREQ_ENUM phyCurrentFrequency;        //current frequency in KHz (2405000 = 2.405 GHz)
  BYTE phyCurrentChannel;
  UINT32 phyChannelsSupported;
  BYTE phyTransmitPower;
  BYTE phyCCAMode;
  union _PHY_DATA_flags {
    BYTE val;
    struct {
     unsigned txFinished:1;    //indicates if TX at PHY level is finished...
	 unsigned txBuffLock:1;    //lock the TX buffer.
    }bits;
  }flags;
  UINT32 txStartTime;
  BYTE *currentTxFrm;   //current frame
  BYTE currentTxFlen;   //current TX frame length
}PHY_PIB;


typedef union _PHY_ARGS {
  struct _PHY_INIT_RADIO {
    RADIO_FLAGS radio_flags;
  }phy_init_radio_args;
}PHY_ARGS;



typedef struct _PHY_SERVICE {
  LRWPAN_SVC_ENUM cmd;
  PHY_ARGS args;
  LRWPAN_STATUS_ENUM status;
}PHY_SERVICE;

typedef enum _PHY_STATE_ENUM {
  PHY_STATE_IDLE,
  PHY_STATE_COMMAND_START,
  PHY_STATE_TX_WAIT
 } PHY_STATE_ENUM;

extern PHY_STATE_ENUM phyState;
extern PHY_PIB phy_pib;
extern PHY_SERVICE a_phy_service;
extern BYTE tmpTxBuff[LRWPAN_MAX_FRAME_SIZE];

//prototypes
void phyFSM(void);
//void phyDoService(PHY_SERVICE *ps);
void phyInit(void );

#define phyIdle() (phyState == PHY_STATE_IDLE)
#define phyBusy() (phyState != PHY_STATE_IDLE)

#define phyTxLocked()   (phy_pib.flags.bits.txBuffLock == 1)
#define phyTxUnLocked()   (phy_pib.flags.bits.txBuffLock == 0)

#define phyGrabTxLock()	phy_pib.flags.bits.txBuffLock = 1
#define phyReleaseTxLock() phy_pib.flags.bits.txBuffLock = 0







//cannot overlap services
//make this a macro to reduce stack depth
#define phyDoService() \
  a_phy_service.status = LRWPAN_STATUS_PHY_INPROGRESS;\
  phyState = PHY_STATE_COMMAND_START;\
  phyFSM();


#endif


