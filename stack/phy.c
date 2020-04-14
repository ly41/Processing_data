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



#include "compiler.h"
#include "lrwpan_config.h"         //user configurations
#include "lrwpan_common_types.h"   //types common acrosss most files
#include "ieee_lrwpan_defs.h"
#include "hal.h"
#include "halStack.h"

#include "console.h"
#include "debug.h"
#include "memalloc.h"
#include "phy.h"


#include "evboard.h"



PHY_PIB phy_pib;
PHY_SERVICE a_phy_service;
PHY_STATE_ENUM phyState;

//static tmp space for that is used by NET, APS, MAC layers
//since only one TX can be in progress at a time, there will be
//not contention for this.
//The current frame is built up in this space, in reverse transmit order.
BYTE tmpTxBuff[LRWPAN_MAX_FRAME_SIZE];

void phyInit(void ) {
  MemInit();  //initialize memory
  phyState = PHY_STATE_IDLE;
  phy_pib.flags.val = 0;
}


//call back from HAL to here, can be empty functions
//not needed in this stack
void phyRxCallback(void) {
}

void phyTxStartCallBack(void) {
phy_pib.txStartTime = halGetMACTimer();

}

void phyTxEndCallBack(void) {
phy_pib.flags.bits.txFinished = 1;   //TX is finished.
}



void phyFSM(void) {
	
  //do evbpolling here
  evbPoll();

  //check background tasks here

  switch (phyState) {
  case PHY_STATE_IDLE:
	  halIdle();  //Hal Layer might want to do something in idle state
    break;
  case PHY_STATE_COMMAND_START:
    switch(a_phy_service.cmd) {
      case LRWPAN_SVC_PHY_INIT_RADIO: //not split phase
       a_phy_service.status = halInitRadio(phy_pib.phyCurrentFrequency,
                                                  phy_pib.phyCurrentChannel,
                                                  a_phy_service.args.phy_init_radio_args.radio_flags
                                                    );
	   phyState = PHY_STATE_IDLE;
       break;
      case LRWPAN_SVC_PHY_TX_DATA:
        phy_pib.flags.bits.txFinished = 0;
        a_phy_service.status =
           halSendPacket(phy_pib.currentTxFlen,
                         phy_pib.currentTxFrm);
        if (a_phy_service.status == LRWPAN_STATUS_SUCCESS) {
          //TX started, wait for it to end.
          phyState = PHY_STATE_TX_WAIT;
        }else {
          //something failed, will give up on this, MAC can take action if it wants
          //should not happen, indicate an error to console
          DEBUG_STRING(1,"PHY: TX did not start\n");
          phyState = PHY_STATE_IDLE;
        }
       break;
     default: break;
    }//end switch cmd
    break;
  case PHY_STATE_TX_WAIT:  //wait for TX out of radio to complete or timeout
    if (phy_pib.flags.bits.txFinished){
        phyState = PHY_STATE_IDLE;
     }
    else if  (halMACTimerNowDelta(phy_pib.txStartTime) > MAX_TX_TRANSMIT_TIME){
      //should not happen, indicate an error to console
      DEBUG_STRING(1,"PHY: MAX_TX_TRANSMIT_TIME timeout\n");
	  a_phy_service.status = LRWPAN_STATUS_PHY_TX_FINISH_FAILED;
      //no action for now, will see if this happens
      phyState = PHY_STATE_IDLE;
    }
    break;
  default: break;
  }//end switch phyState
}

