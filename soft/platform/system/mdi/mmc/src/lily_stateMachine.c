/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/




#include "lily_statemachine.h"
#include "mmc_camera.h"
#include "mcip_debug.h"




#if 1

static uint32 lilyActiveMode=0x1;

boolean lily_getModuleState(uint16 module)
{
    return  lilyActiveMode&module;
}
boolean lily_getLilyActiveState(void)
{
    return  (lilyActiveMode>1);
}
boolean checkModuleConflict(uint16 module)
{
    boolean isConflict = FALSE;

    USER_TRACE(1,"checkModuleConflict,module is %d,lily_getLilyActiveState is %d",module,lily_getLilyActiveState());
    switch(module)
    {
        case STM_MOD_IDLE:
            isConflict =FALSE;
            break;
        case STM_MOD_SLEEP:
            if(lily_getLilyActiveState() == TRUE)
            {
                isConflict =TRUE;
            }
            else
            {
                isConflict =FALSE;
            }
            break;
        case STM_MOD_AUD_PLAY:
        case STM_MOD_AUD_PAUSE:
            if(lily_getModuleState(STM_MOD_IMG)||lily_getModuleState(STM_MOD_IDLE))
            {
                isConflict =FALSE;
            }
            else
            {
                isConflict =TRUE;
            }
            break;
        case STM_MOD_IMG:
            if(lily_getModuleState(STM_MOD_AUD_PLAY)|| lily_getModuleState(STM_MOD_AUD_PAUSE)|| lily_getModuleState(STM_MOD_IDLE))
            {
                isConflict =FALSE;
            }
            else
            {
                isConflict =TRUE;
            }
            break;
        case STM_MOD_AUDREC:
        case STM_MOD_CAM:
        case STM_MOD_VID:
        case STM_MOD_VIRECD:
        case STM_MOD_USB:
            if(lily_getModuleState(STM_MOD_IDLE))
            {
                isConflict =FALSE;
            }
            else
            {
                isConflict =TRUE;
            }
            break;
        case STM_MOD_FMREC:
        case STM_MOD_FM:
            if(lily_getModuleState(STM_MOD_IDLE)||lily_getModuleState(STM_MOD_FMREC)|| lily_getModuleState(STM_MOD_FM))
            {
                isConflict =FALSE;
            }
            else
            {
                isConflict =TRUE;
            }
            break;

        case STM_MOD_FS:
            isConflict =FALSE;
            break;
        default:
            USER_TRACE(1,"checkModuleConflict,error para");
            //ass(1);
            break;
    }
    return isConflict;
}
STA_RESULT lily_changeToState(uint16 module)
{
    if(checkModuleConflict(module))
    {
        USER_TRACE(1,"lily_changeToState,STA_RES_FAIL,module is %d",module);
        return STA_RES_FAIL;
    }
    else
    {
        USER_TRACE(1,"lily_changeToState,STA_RES_OK");
        lilyActiveMode|=module;
#ifdef ENABLE_LILY_SLEEP
        entryPowerSaveState(lilyActiveMode);
#endif
        return STA_RES_OK;
    }
}
STA_RESULT lily_exitModule(uint16 module)
{
#ifdef ENABLE_LILY_SLEEP
    leavePowerSaveState(module);
#endif
    lilyActiveMode&=(~module);
    // USER_TRACE(1,"lily_exitModule,lilyActiveMode is %d",lily_getLilyActiveState());
    return STA_RES_OK;
}

#else

#include "mmc_camera.h"
#include "cpu_sleep.h"

STA_MAC_STATES_RECORD statsList;
boolean TcardState = FALSE;
extern const STA_MAC_MANAGE_LIST lilyStaMacList[];

//private function
STA_MAC_MODULE stm_getStateMod(STA_MAC_STATES state)
{
    if(STA_MAC_IDLE==state)
    {
        return STA_MAC_MOD_IDLE;
    }
    else if(STA_MAC_SLEEP==state)
    {
        return STA_MAC_MOD_SLEEP;
    }
    else if(state>=STA_MAC_MEDIA_AUD_PLAY&&state<=STA_MAC_MEDIA_AUD_STOP) //audio module
    {
        return STA_MAC_MOD_AUD;
    }
    else if(state>=STA_MAC_MEDIA_IMG_DECODE&&state<=STA_MAC_MEDIA_IMG_STOP)
    {
        return STA_MAC_MOD_IMG;
    }
    /*you should add new "else if" here with new module*/
    else
    {
        return STA_MAC_MOD_NONE;
    }

}


STA_MAC_STATES stm_getParallelState(uint16 index)
{
    switch(index)
    {
        case 0:
            return statsList.parallelState1;
            break;
        case 1:
            return statsList.parallelState2;
            break;
        case 2:
            return statsList.parallelState3;
            break;
        default:
            return STA_MAC_MAX;
            break;
    }
}


//public function
STA_MAC_STATES lily_stmTransferTmpstate(uint16 *state)
{
    USER_TRACE(1,"lily_stmTransferTmpstate,state is %d",*state);
    leavePowerSaveState(STA_MAC_MOD_IDLE);
    //for some temporary states,we change them into its stable state
    if(STA_MAC_MEDIA_AUD_STOP==*state||STA_MAC_MEDIA_IMG_STOP==*state||STA_MAC_MEDIA_CAM_POWEROFF==*state)
    {
        leavePowerSaveState(stm_getStateMod(*state));
        *state = STA_MAC_IDLE;

    }
    else if(STA_MAC_MEDIA_AUD_RESUME ==*state )
    {
        *state = STA_MAC_MEDIA_AUD_PLAY;
    }

    if(STA_MAC_IDLE == *state  )
    {
        //start timer to count down to sleep state
    }
    else
    {
        //stop the sleep timer
    }
    entryPowerSaveState(stm_getStateMod(*state));
    return *state;
}
//attention:desState should not be STA_MAC_IDLE,as stm_getStateMod would return STA_MAC_MOD_IDLE,so idle to idle
STA_RESULT lily_changeToState(STA_MAC_STATES desState)
{
    uint16 *currentState = NULL;

    //local variable
    STA_MAC_MODULE desStateMod =stm_getStateMod(desState);
    STA_MAC_MODULE listState1Mod =stm_getStateMod(statsList.parallelState1);
    STA_MAC_MODULE listState2Mod =stm_getStateMod(statsList.parallelState2);
    STA_MAC_MODULE listState3Mod =stm_getStateMod(statsList.parallelState3);
    uint16 vi=0,vParaelleState=0;
    boolean permission = FALSE;

    USER_TRACE(1,"lily_changeToState,entry,listState1Mod is %d,listState2Mod is %d,listState3Mod is %d,desState is %d",listState1Mod,listState2Mod,listState3Mod,desState);
    if(desStateMod ==listState1Mod )
    {
        currentState = &(statsList.parallelState1);
    }
    else if(desStateMod ==listState2Mod )
    {
        currentState = &(statsList.parallelState2);
    }
    else if(desStateMod ==listState3Mod )
    {
        currentState = &(statsList.parallelState3);
    }
    else
    {

        if(listState1Mod == STA_MAC_MOD_IDLE)
        {
            currentState = &(statsList.parallelState1);
        }
        else if(listState2Mod == STA_MAC_MOD_IDLE)
        {
            currentState = &(statsList.parallelState2);
        }
        else if(listState3Mod == STA_MAC_MOD_IDLE)
        {
            currentState = &(statsList.parallelState3);
        }
        else
        {
            currentState = NULL;
        }
    }

    if((currentState == NULL))
    {
        return STA_RES_FAIL;
    }
    USER_TRACE(1,"lily_changeToState,currentState is %d",*currentState);
    //USER_TRACE(1,"lily_changeToState,currentState is %d",*currentState);
    //check the permmision list
    for(vi=0; vi<lilyStaMacList[*currentState].permissionNum; vi++)
    {
        //USER_TRACE(1,"lily_changeToState,list currentState is %d",*( (lilyStaMacList[*currentState]).permission+vi));
        if(desState == *( (lilyStaMacList[*currentState]).permission+vi) )
        {
            permission = TRUE;
            break;
        }
    }
    USER_TRACE(1,"lily_changeToState,after check permmision list,permission is %d",permission);
    //check the forbiddance list
    if(TRUE== permission)
    {
        uint16 localCurrentState = NULL;
        //any paraelleState forbid the change,we don't change successfully
        for(vParaelleState=0; vParaelleState<3; vParaelleState++)
        {
            switch(vParaelleState)
            {
                case 0:
                    localCurrentState=stm_getParallelState(0);
                    break;
                case 1:
                    localCurrentState=stm_getParallelState(1);
                    break;
                case 2:
                    localCurrentState=stm_getParallelState(2);
                    break;
                default:
                    localCurrentState = STA_MAC_IDLE;
                    break;
            }

            for(vi=0; vi<lilyStaMacList[localCurrentState].forbiddanceNum; vi++)
            {
                if(desState == *( (lilyStaMacList[localCurrentState]).forbiddance+vi) )
                {
                    permission = FALSE;
                    break;
                }
            }
        }
    }

    if(FALSE== permission)
    {
        USER_TRACE(1,"lily_changeToState,error");
        return STA_RES_FAIL;
    }
    else
    {
        *currentState = desState;
        //transfer Tmp state to its stable state
        lily_stmTransferTmpstate(currentState);
        USER_TRACE(1,"lily_changeToState,OK,currentState is %d",*currentState);

        return STA_RES_OK;
    }

}


void lily_stateMachineInit(void)
{
    statsList.parallelState1 = STA_MAC_IDLE;
    statsList.parallelState2 = STA_MAC_IDLE;
    statsList.parallelState3 = STA_MAC_IDLE;
    //TcardState == getcardState();
    initPowerSaveState(STA_MAC_MOD_IDLE);
    if(1)
    {
        lily_changeToState(STA_MAC_IDLE);
    }
    else
    {
        lily_changeToState(STA_MAC_NO_CARD);
    }
}
#endif
