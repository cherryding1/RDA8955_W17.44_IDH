/*
===============================================================================
  Legal     : This source code is property of StackCom. 
              The information contained in this file is confidential.
              Distribution, reproduction, exploitation, or transmisison
              of any content of this file is not allowed except if expressly
              permitted. Infringements result in damage claims.

              Copyright StackCom (c) 2003
===============================================================================
*/

/*
===============================================================================
  File       : fsm.h
===============================================================================

  Scope      : Finite State Machine strcutures and Macros

-------------------------------------------------------------------------------
  Date       | Author | Modification
-------------------------------------------------------------------------------
  Oct 11 05  |  FCH   | In macro FSM_GET_STATE, remove of mask with ~FSM_STABLE
  May 13 04  |  FCH   | Add macro FSM_GET_STATE
  Jun 12 03  |  PFA   | Creation            
===============================================================================
*/

#ifndef __FSM_H__
#define __FSM_H__        

#include "sxs_type.h"    // Common Types
#include "cmn_defs.h"    // EvtDsc In and Out
#include "sxr_cnf.h"

#define FSM_NOT_FOUND -1 
#define FSM_SAME_STATE 0

#define FSM_MAX_SAV_EVT 255

#define FSM_STABLE   (1 << 7)

/*
** Some macros for FSM declaration
*/

#define FSM_STATE(_Nam, _func)   \
                 { #_Nam, sizeof(_Nam)/sizeof(EvtFunc_t), _Nam, _func }

#define FSM_INIT(_String, _Nam, _TrcId, _InitState, _SavMbx, _Mbx)  \
                 { _String, _Nam, _TrcId, _InitState,        \
                   sizeof(_Nam )/sizeof(State_t ),         \
                   0, _SavMbx, _Mbx}

/*
** Couple (Event,Function) Description 
*/
typedef const struct
{
   InEvtDsc_t    * EvtDsc     ;    /* Ptr to descriptor*/
   u8           (* Func)(void);    /* function address */
}EvtFunc_t;

/*
** State Description : A state is a list of couples (Evt, Fun)
*/
typedef const struct
{
 const ascii   * Name               ;    /* Trace string     */
 u32             EvtFuncNb          ;    /* Num of EvtFun    */
 EvtFunc_t     * EvtFuncList        ;    /* List of Evt-Fn   */
 void         (* InitFunc)   (void) ;    /* Init of the state function */

} State_t;

/*
** FSM Description : An FSM is a list of States
*/
typedef struct
{
   ascii         * Name       ;    /* Trace string     */
   State_t       * StateList  ;    /* Ptr to States    */
   u16             TrcId      ;    /* Trace Level      */
   u8              CurState   ;    /* Current State    */
   u8              StateNb    ;    /* Number of State  */
   u8              NbSavedEvt ;   
   u8              SavMbx     ;    /* If = NO_MBX, no save for this FSM  */
   u8              MainMbx    ;    /* Mbx where saved evts are forwarded */
} Fsm_t;

bool fsm_Run     (Fsm_t       * Fsm,    InEvtDsc_t * EvtDsc) ;
s16  fsm_IdxEvt  (InEvtDsc_t ** EvtDsc, u32 EvtId, u16 Size) ;
s16  fsm_FindEvt (State_t  * State,  u32 EvtId )             ;
void fsm_Save    (Fsm_t * Fsm, bool IsMsg, void * Data)      ;
bool fsm_Retrieve(Fsm_t * Fsm)                               ;


  // Macro to get/set the current state of a FSM (use it parsimoniously)
//#define FSM_GET_STATE(_Fsm)         (( (_Fsm)->CurState ) & ~FSM_STABLE)
#define FSM_GET_STATE(_Fsm)         ( (_Fsm)->CurState )
#define FSM_SET_STATE(_Fsm, _State) ((_Fsm)->CurState = _State)

#endif
