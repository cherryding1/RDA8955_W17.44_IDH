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



#include "cs_types.h"
#include "global_macros.h"
#include "keypad.h"

#include "tgt_m.h"
#include "hal_config.h"
#include "halp_config.h"

#include "hal_key.h"
#include "halp_debug.h"
#include "hal_gpio.h"
#include "hal_map.h"
#include "hal_timers.h"


///
///     Type used to describe the state of the keypad.
///     - low data (first 30 keys) are described by the lowest 30 bits in
///     the first UINT32 ([0])
///     - high data (the last 6 keys) are described by the lowest 6 bits in
///     the second UINT32 ([1])
///
///
typedef UINT32 HAL_KEY_MATRIX_T[2];
///
///     Keep track of the user callback function.
///     This function will be called when a key event occurs (up, down or pressed).
///
PRIVATE HAL_KEY_IRQ_HANDLER_T g_keyIrqHandler
    = NULL;

///
///     Global variables used to keep track of the state of the keypad.
///     The xxxKeys variables are images of the Key_Map array, where each
///     case is TRUE when the corresponding key is pressed and FALSE when
///     it is released
///     @todo Decide if using bit(fields) wouldn't improve that (speed and space)
///
PRIVATE HAL_KEY_MATRIX_T    g_previousKeys  ;
PRIVATE BOOL                g_previousOnOff ;

#define KEY_ON_OFF_STATE_NULL 0xff
PRIVATE UINT8               g_keyOnOffAtPowerOn = KEY_ON_OFF_STATE_NULL;

///
///     Global variables used to decide which keypad events must be reported.
///
PRIVATE BOOL g_eventOnUp         = FALSE;
PRIVATE BOOL g_eventOnPressed    = FALSE;
PRIVATE BOOL g_eventOnDown       = FALSE;

#ifdef TGT_WITH_EXPKEY
#include "tgt_expkey_cfg.h"
///
///     Global variables used for expand key processing
///
#define COL0_PATTERN  0x01041041
#define COL1_PATTERN  0x02082082
#define COL2_PATTERN  0x04104104
#define COL3_PATTERN  0x08208208
#define COL4_PATTERN  0x10410410
#define COL5_PATTERN  0x20820820

PRIVATE UINT8   g_RowToDisable = 7;
PRIVATE BOOL    g_ItvTimeShort = FALSE;
PRIVATE BOOL    g_ExpKeyPressed = FALSE;

typedef UINT8 EXP_KEY_MATRIX_T[2];
PRIVATE CONST TGT_CFG_EXPKEY_T* g_ExpKeyCfg;
PRIVATE EXP_KEY_MATRIX_T g_expKey;

PRIVATE UINT32 g_kpCtrlReg;
PRIVATE UINT8  g_kpItvReg;
PRIVATE UINT8  g_kpdbReg;
PRIVATE UINT8  g_kpItvCnt = 0;

PRIVATE HAL_APO_ID_T g_keyOut6;
PRIVATE HAL_APO_ID_T g_keyOut7;

#endif

// ============================================================================
// hal_KeyOpen
// ----------------------------------------------------------------------------
/// This function initializes the keypad driver. The configuration of the key
/// pins used is board dependent and is built in HAL through the
/// #tgt_BoardConfig.halCfg structure.
///
/// @param debounceTime defines the time to wait for debounce, in number of
/// 16384Hz steps. Boundaries applied to the possible values of this time,
/// which are dependent on the board config, especially the number of
/// out line of the keypad: the value of this parameter must be chosen
/// between 5*Number of Enabled KeyOu and 1280*Number of Enabled
/// Please refer to the keypad documentation for
/// details.
/// @param kpItvTime defines the interval time between the generation of IRQ
/// when one or more keys are being held down. It defines a multiple of the
/// debounce time.
// ============================================================================
PUBLIC VOID hal_KeyOpen(UINT16 debounceTime, UINT16 kpItvTime)
{
    CONST HAL_CFG_CONFIG_T* halCfg = g_halCfg;
    // WORKAROUND:
    // H/w requires keyOutMask bit 0 to be set when keypad is enabled,
    // otherwise the power key cannot be detected.
    UINT32 keyOutMask = halCfg->keyOutMask | 0x1;
    UINT32 temp;
    UINT32 nbKeyOut = 0;
    // Debounce value to enter in the control reg
    UINT32 dbnTimeReg = 0;

    // Save current OnOff key status for power-on check (Otherwise
    // we will have to wait the debounce time after enabling keypad.
    // See the last comments of this function)
    hal_KeyOnOffStateAtPowerOn();

    // Count the number of enabled keyout.
    temp = keyOutMask;
    while (temp != 0)
    {
        if (temp & 0x1)
        {
            nbKeyOut += 1;
        }
        temp = temp >>1;
    }

#ifdef TGT_WITH_EXPKEY
    g_ExpKeyCfg = tgt_GetExpKeyConfig();
    if (g_ExpKeyCfg->expKeyUsed)
    {
        g_keyOut6 = g_ExpKeyCfg->expKeyOut6;
        g_keyOut7 = g_ExpKeyCfg->expKeyOut7;
        if ((g_keyOut6.type == HAL_GPIO_TYPE_O) &&
                (g_keyOut7.type == HAL_GPIO_TYPE_O))
        {
            hal_GpioSet(g_keyOut6);
            hal_GpioSet(g_keyOut7);
        }
        else
        {
            HAL_ASSERT(FALSE, "Should Use GPO as KeyOut!");
        }
    }
#endif

    // Reset the event generation configuration.
    g_eventOnUp      = FALSE;
    g_eventOnPressed = FALSE;
    g_eventOnDown    = FALSE;
    g_keyIrqHandler  = NULL;

    // Initialize the previous state.
    g_previousKeys[0] = 0;
    g_previousKeys[1] = 0;
    g_previousOnOff   = hal_KeyOnOffStateGet();

    // WORKAROUND:
    // H/w requires dbnTimeReg larger than 0 when keypad is enabled,
    // otherwise the power key cannot be detected.
    if(nbKeyOut != 0)
    {
        HAL_ASSERT( (debounceTime >= (5+1)*nbKeyOut) && (debounceTime <= 256*5*nbKeyOut),
                    "Improper debounce times used: %d", debounceTime );

        // Value to enter in the register bitfield
        // De-bounce time = (KP_DBN_TIME + 1) * SCAN_TIME,
        // SCAN_TIME = 0.3125 ms * Number of Enabled KeyOut (determined by KP_OUT_MASK).
        // For example, if KP_DBN_TIME = 7, KP_OUT_MASK = "111111", then
        // De-bounce time = (7+1)*0.3125*6=15 ms. The maximum debounce time is 480 ms.
        dbnTimeReg =  (debounceTime / (5 * nbKeyOut)) - 1;
    }
    else
    {
        HAL_ASSERT( debounceTime >= 2,
                    "Improper debounce times used: %d", debounceTime );
        dbnTimeReg =  (debounceTime) - 1;
    }

    // Ensure keypad runs out of its state machine
    // (Keypad might have been enabled at boot time)
    hwp_keypad->KP_CTRL = 0;
    // It needs 1 cycle of 16K clock to disable keypad
    hal_TimDelay(3);

    hwp_keypad->KP_CTRL = KEYPAD_KP_DBN_TIME(dbnTimeReg)
                          | KEYPAD_KP_ITV_TIME(kpItvTime)
                          | KEYPAD_KP_EN | KEYPAD_KP_IN_MASK(halCfg->keyInMask)
                          | KEYPAD_KP_OUT_MASK(keyOutMask);

    // clear current IRQ
    hwp_keypad->KP_IRQ_CLR = KEYPAD_KP_IRQ_CLR;

    g_halMapAccess.keypadPulseHandler = (void*)hal_KeyRemoteHandler;

#ifdef TGT_WITH_EXPKEY
    g_kpItvReg  = kpItvTime;
    g_kpdbReg   = dbnTimeReg;
    g_kpCtrlReg = KEYPAD_KP_ITV_IRQ_MASK
                  | KEYPAD_KP_EVT1_IRQ_MASK
                  | KEYPAD_KP_EVT0_IRQ_MASK | KEYPAD_KP_EN
                  | KEYPAD_KP_IN_MASK(halCfg->keyInMask)
                  | KEYPAD_KP_OUT_MASK(keyOutMask);
#endif

    // !!! CAUTION !!!
    // After keypad is enabled, the status of individual keys (including
    // OnOff key) will NOT be available until the debouce time elapses.
}


// ============================================================================
// hal_KeyClose
// ----------------------------------------------------------------------------
/// This function closes the key driver.
// ============================================================================
PUBLIC VOID hal_KeyClose(VOID)
{
    // disable keypad module
    g_keyIrqHandler     = NULL;
    hwp_keypad->KP_CTRL = 0;
}

// scan the keypad register to fill a BOOLean vector saying if
// each key is pressed or not
PRIVATE VOID hal_KeySingleScan(HAL_KEY_MATRIX_T key)
{
    key[0] = GET_BITFIELD(hwp_keypad->KP_DATA_L, KEYPAD_KP_DATA_L);
    key[1] = GET_BITFIELD(hwp_keypad->KP_DATA_H, KEYPAD_KP_DATA_H);
}



// ============================================================================
// hal_KeyOnOffStateGet
// ----------------------------------------------------------------------------
/// Tell if the key ON key is pressed
/// @return \c TRUE if the ON/OFF button is pressed \n
///         \c FALSE otherwise
// ============================================================================
PUBLIC BOOL hal_KeyOnOffStateGet(VOID)
{
    if (hwp_keypad->KP_STATUS & KEYPAD_KP_ON)
        return TRUE;
    else
        return FALSE;
}


// ============================================================================
// hal_KeyOnOffStateAtPowerOn
// ----------------------------------------------------------------------------
/// Tell if the key ON key was pressed at power-on time.
/// @return \c TRUE if the ON/OFF button was pressed \n
///         \c FALSE otherwise
// ============================================================================
PUBLIC BOOL hal_KeyOnOffStateAtPowerOn(VOID)
{
    if (g_keyOnOffAtPowerOn == KEY_ON_OFF_STATE_NULL)
    {
        g_keyOnOffAtPowerOn = hal_KeyOnOffStateGet();
    }
    return (BOOL)g_keyOnOffAtPowerOn;
}


// ============================================================================
// hal_KeyResetIrq
// ----------------------------------------------------------------------------
/// This function clears the keypad interrupt status. It should only be called
/// to avoid unwanted Irq. In normal usage, Irq clearing is done automatically
/// by the interrupt driver.
// ============================================================================
PUBLIC VOID hal_KeyResetIrq(VOID)
{
    hwp_keypad->KP_IRQ_CLR = KEYPAD_KP_IRQ_CLR;
}

// ============================================================================
// hal_KeyIrqSetHandler
// ----------------------------------------------------------------------------
/// This function configures which user function will be called by the
/// keypad driver when an interruption is generated. See the function
/// #hal_KeyIrqSetMask() for details about when an interruption is generated.
///
/// @param handler Pointer to a user function called when an interruption
/// is generated by the keypad driver.
// ============================================================================
PUBLIC VOID hal_KeyIrqSetHandler(HAL_KEY_IRQ_HANDLER_T handler)
{
    g_keyIrqHandler = handler;

    if (g_keyIrqHandler == NULL)
    {
        // Disable all the interruptions for the keypad.
        hal_KeyClose();
    }
}

// ============================================================================
// hal_KeyIrqSetMask
// ----------------------------------------------------------------------------
/// This function controls under which conditions the keypad interrupt will
/// be generated. It can be when a key is pressed, when it is held down and
/// when it is released. Use the function #hal_KeyIrqSetHandler() to configure
/// which user function will be called in case of a keypad interruption.
///
/// @param mask Defines which event will generate a call to the user
/// callback function. See the documentation of the type for details.
// ============================================================================
PUBLIC VOID hal_KeyIrqSetMask(HAL_KEY_IRQ_MASK_T mask)
{
    // Keep track of the event generation configuration.
    g_eventOnUp      = mask.onUp;
    g_eventOnPressed = mask.onPressed;
    g_eventOnDown    = mask.onDown;

    if (FALSE == g_eventOnUp &&
            FALSE == g_eventOnPressed &&
            FALSE == g_eventOnDown)
    {
        // Disable all the interruptions for the keypad.
        hal_KeyClose();
    }
    else
    {
        // Enables the IRQ for the keypad.
        hwp_keypad->KP_IRQ_MASK = KEYPAD_KP_EVT0_IRQ_MASK  // key up or down
                                  | KEYPAD_KP_EVT1_IRQ_MASK   // all key released
                                  | (g_eventOnPressed?KEYPAD_KP_ITV_IRQ_MASK:0);
        // Interrupt on key kept pressed state...
        // We might avoid useless IRQ that way
    }
}


// Call the user handler with the given status for each key
// whose bit is set to '1' in the keymatrix
PRIVATE VOID hal_KeyCallHandler(HAL_KEY_MATRIX_T keys, HAL_KEY_STATE_T state)
{
    UINT32 i;

    // This test is useless since the existence of the callback is
    // checked before this function is called, but ...
    if (g_keyIrqHandler)
    {

        // low keys
        for (i=0; i<LOW_KEY_NB ; i++)
        {
            if ((keys[0] & (1<<i)) != 0)
            {
                // ith key held pressed
                g_keyIrqHandler(i, state);
            }
        }

        // high keys
        for (i=0; i<HIGH_KEY_NB ; i++)
        {
            if ((keys[1] & (1<<i)) != 0)
            {
                // ith key held pressed
                g_keyIrqHandler((i+LOW_KEY_NB), state);
            }
        }
    }
}

#ifdef TGT_WITH_EXPKEY
// Expand Key Call Handler
PRIVATE VOID hal_ExpKeyCallHandler(EXP_KEY_MATRIX_T keys, HAL_KEY_STATE_T state)
{
    UINT8 id;

    if (g_keyIrqHandler)
    {
        id = 36 + keys[0] + 6*keys[1];
        g_keyIrqHandler(id, state);
    }
}
#endif

// ============================================================================
// hal_KeyIrqGetMask
// ----------------------------------------------------------------------------
/// This function returns the keypad IRQ mask. Refer to the type documentation
/// for its interpretation.
/// @return The keypad IRQ mask.
// ============================================================================
PUBLIC HAL_KEY_IRQ_MASK_T hal_KeyIrqGetMask(VOID)
{
    HAL_KEY_IRQ_MASK_T ret;

    ret.onUp        = g_eventOnUp;
    ret.onPressed   = g_eventOnPressed;
    ret.onDown      = g_eventOnDown;

    return ret;
}



///
///     Internal interrupt handler for the keypad physical interruptions.
///
///     It handles the edge detection by starting the timer IRQ.
///     It handles the key timer interruptions by debouncing and calling the user
///     callback function for each key event (up, down or pressed).
///     It manages the hw interrupts of the keypad (clears the IRQ in the module).
///
///     It handles also the interrupt for the GPIO used for the on-off button.
///     About the interrupt for the on-off button: Physically, this button is
///     mapped on a GPIO. The GPIO HAL handler will call this function when an
///     interrupt is detected on the corresponding GPIO. When this rising edge is
///     detected, the behavior is the same as the one that occurs when a normal
///     key is pressed: it disable the edge IRQ and starts the key timer IRQ.
///
#ifdef TGT_WITH_EXPKEY
PROTECTED VOID hal_KeyIrqHandler(UINT8 interruptId)
{
    // interrupt status
    UINT32 status;
    // Keys pressed a the time of the interrupt
    HAL_KEY_MATRIX_T scannedKeys;
    // On-Off key state at the time of the interrupt
    BOOL scannedOnOff;

    // Variables to store the state of the different keys
    HAL_KEY_MATRIX_T downKeys;
    HAL_KEY_MATRIX_T upKeys;
    HAL_KEY_MATRIX_T pressedKeys;
    BOOL ExpKeyDet = FALSE;

    status = hwp_keypad->KP_IRQ_CAUSE &
             (KEYPAD_KP_EVT0_IRQ_CAUSE|KEYPAD_KP_EVT1_IRQ_CAUSE|KEYPAD_KP_ITV_IRQ_CAUSE) ;

    // Clear IRQ
    hwp_keypad->KP_IRQ_CLR = KEYPAD_KP_IRQ_CLR;


    if ((status & KEYPAD_KP_EVT0_IRQ_CAUSE) || (status & KEYPAD_KP_ITV_IRQ_CAUSE))
    {
        hal_KeySingleScan(scannedKeys);
        if (scannedKeys[0] == COL0_PATTERN)
        {
            g_expKey[0] = 0;
            ExpKeyDet = TRUE;
        }
        else if (scannedKeys[0] == COL1_PATTERN)
        {
            g_expKey[0] = 1;
            ExpKeyDet = TRUE;
        }
        else if (scannedKeys[0] == COL2_PATTERN)
        {
            g_expKey[0] = 2;
            ExpKeyDet = TRUE;
        }
        else if (scannedKeys[0] == COL3_PATTERN)
        {
            g_expKey[0] = 3;
            ExpKeyDet = TRUE;
        }
        else if (scannedKeys[0] == COL4_PATTERN)
        {
            g_expKey[0] = 4;
            ExpKeyDet = TRUE;
        }
        else if (scannedKeys[0] == COL5_PATTERN)
        {
            g_expKey[0] = 5;
            ExpKeyDet = TRUE;
        }
        else
        {
            ExpKeyDet = FALSE;
        }
    }
    if (status & KEYPAD_KP_EVT1_IRQ_CAUSE)
    {
        if (g_ExpKeyPressed)
        {
            HAL_TRACE(HAL_IO_TRC, 0, "HAL_KEY: Key Up -> Finish Exp Key config");
            hwp_keypad->KP_CTRL = g_kpCtrlReg | KEYPAD_KP_ITV_TIME(g_kpItvReg) | KEYPAD_KP_DBN_TIME(g_kpdbReg);
            g_ItvTimeShort = FALSE;
            g_ExpKeyPressed = FALSE;
            g_kpItvCnt = 0;

            if (g_RowToDisable == 6)
            {
                g_RowToDisable = 7;
            }
            else
            {
                g_RowToDisable = 6;
            }

            hal_ExpKeyCallHandler(g_expKey, HAL_KEY_UP);
            // Reset KeyOut6 & 7 to '1'
            hal_GpioSet(g_keyOut6);
            hal_GpioSet(g_keyOut7);
        }
    }
    if (!ExpKeyDet)
    {
        // All keys are released
        if ((status & KEYPAD_KP_EVT1_IRQ_CAUSE) && g_eventOnUp)
        {
            pressedKeys[0] = 0;
            pressedKeys[1] = 0;
            scannedOnOff = FALSE;

            // All previously pressed keys are released
            hal_KeyCallHandler(g_previousKeys, HAL_KEY_UP);

            // on-off button
            if (g_previousOnOff)
            {
                g_keyIrqHandler(255, HAL_KEY_UP);
            }

            // Keep track of the pressed buttons.
            g_previousKeys[0] = pressedKeys[0];
            g_previousKeys[1] = pressedKeys[1];
            g_previousOnOff   = scannedOnOff;
        }

        else

        {
            // Key up or down
            if (status &  KEYPAD_KP_EVT0_IRQ_CAUSE)
            {

                hal_KeySingleScan(scannedKeys);
                scannedOnOff = hal_KeyOnOffStateGet();

                // Send the ***pressed events*** only if user requested it.
                if (g_eventOnPressed && NULL != g_keyIrqHandler)
                {
                    // Pressed key detection
                    // Those are the one previously pressed and still pressed now
                    pressedKeys[0] = (scannedKeys[0] & g_previousKeys[0]);
                    pressedKeys[1] = (scannedKeys[1] & g_previousKeys[1]);

                    hal_KeyCallHandler(pressedKeys, HAL_KEY_PRESSED);
                    // on-off button
                    if (g_previousOnOff & scannedOnOff)
                    {
                        g_keyIrqHandler(255, HAL_KEY_PRESSED);
                    }

                }

                // If the pressed buttons changed.
                if (scannedKeys[0] != g_previousKeys[0] ||
                        scannedKeys[1] != g_previousKeys[1] ||
                        scannedOnOff   != g_previousOnOff)
                {
                    // Send the ***down events*** only if user requested it.
                    if (g_eventOnDown && NULL != g_keyIrqHandler)
                    {
                        // Key that are now pressed but weren't before
                        downKeys[0] = scannedKeys[0] & ~g_previousKeys[0];
                        downKeys[1] = scannedKeys[1] & ~g_previousKeys[1];

                        // low and high keys
                        hal_KeyCallHandler(downKeys, HAL_KEY_DOWN);

                        // on-off button
                        if (~g_previousOnOff & scannedOnOff)
                        {
                            g_keyIrqHandler(255, HAL_KEY_DOWN);
                        }
                    }

                    // Send the ***up events*** only if user requested it.
                    if (g_eventOnUp && NULL != g_keyIrqHandler)
                    {
                        // Keys that are now unpressed but were pressed before
                        upKeys[0] = ~scannedKeys[0] & g_previousKeys[0];
                        upKeys[1] = ~scannedKeys[1] & g_previousKeys[1];

                        // low and high keys
                        hal_KeyCallHandler(upKeys, HAL_KEY_UP);

                        // on-off key
                        if (g_previousOnOff & ~scannedOnOff)
                        {
                            g_keyIrqHandler(255, HAL_KEY_UP);
                        }
                    }

                    // Keep track of the pressed buttons.
                    g_previousKeys[0] = scannedKeys[0];
                    g_previousKeys[1] = scannedKeys[1];
                    g_previousOnOff   = scannedOnOff;
                }
            }
            else
            {
                // There are only already pressed keys
                // still pressed, so no change in pressed keys
                // and no need for a new scan
                if ((status & KEYPAD_KP_ITV_IRQ_CAUSE) && g_eventOnPressed)
                {
                    hal_KeyCallHandler(g_previousKeys, HAL_KEY_PRESSED);
                    // on-off button
                    if (g_previousOnOff)
                    {
                        g_keyIrqHandler(255, HAL_KEY_PRESSED);
                    }
                }
            }
        }
    }
    else // Expand Key
    {
        if (status & KEYPAD_KP_EVT0_IRQ_CAUSE)
        {
            hwp_keypad->KP_CTRL = g_kpCtrlReg | KEYPAD_KP_ITV_TIME(0) | KEYPAD_KP_DBN_TIME(1);
            g_ItvTimeShort = TRUE;
            HAL_TRACE(HAL_IO_TRC, 0, "HAL_KEY: COL_DET");
            if (g_RowToDisable == 6)
            {
                hal_GpioClr(g_keyOut6);
            }
            else
            {
                hal_GpioClr(g_keyOut7);
            }
            g_ExpKeyPressed = TRUE;
        }
        if (status & KEYPAD_KP_ITV_IRQ_CAUSE)
        {
            if (g_ItvTimeShort)
            {
                hwp_keypad->KP_CTRL = g_kpCtrlReg | KEYPAD_KP_ITV_TIME(g_kpItvReg) | KEYPAD_KP_DBN_TIME(g_kpdbReg);
                g_ItvTimeShort=FALSE;
                if (g_RowToDisable == 6)
                {
                    // Row 6 is disabled, so exp key is on row 7
                    g_expKey[1] = 1;
                    HAL_TRACE(HAL_IO_TRC, 0, "HAL_KEY: Key Down 7");
                }
                else
                {
                    // Row 7 is disabled, so exp key is on row 6
                    g_expKey[1] = 0;
                    HAL_TRACE(HAL_IO_TRC, 0, "HAL_KEY: Key Down 6");
                }
                hal_ExpKeyCallHandler(g_expKey, HAL_KEY_DOWN);
            }
            else
            {
                if (g_RowToDisable == 6)
                {
                    // Row 6 is disabled, so exp key is on row 7
                    g_expKey[1] = 1;
                    HAL_TRACE(HAL_IO_TRC, 0, "HAL_KEY: Key Pressed 7");
                }
                else
                {
                    // Row 7 is disabled, so exp key is on row 6
                    g_expKey[1] = 0;
                    HAL_TRACE(HAL_IO_TRC, 0, "HAL_KEY: Key Pressed 6");
                }
                if (g_kpItvCnt)
                {
                    hal_ExpKeyCallHandler(g_expKey, HAL_KEY_PRESSED);
                }
                g_kpItvCnt++;
            }
        }
    }
}
#else
PROTECTED VOID hal_KeyIrqHandler(UINT8 interruptId)
{
    // interrupt status
    UINT32 status;
    // Keys pressed a the time of the interrupt
    HAL_KEY_MATRIX_T scannedKeys;
    // On-Off key state at the time of the interrupt
    BOOL scannedOnOff;

    // Variables to store the state of the different keys
    HAL_KEY_MATRIX_T downKeys;
    HAL_KEY_MATRIX_T upKeys;
    HAL_KEY_MATRIX_T pressedKeys;

    status = hwp_keypad->KP_IRQ_CAUSE &
             (KEYPAD_KP_EVT0_IRQ_CAUSE|KEYPAD_KP_EVT1_IRQ_CAUSE|KEYPAD_KP_ITV_IRQ_CAUSE) ;

    // Clear IRQ
    hwp_keypad->KP_IRQ_CLR = KEYPAD_KP_IRQ_CLR;


    // Key IRQ received, return *status* to the user handler
    // (That is down, up, or held pressed)

    // All keys are released
    if ((status & KEYPAD_KP_EVT1_IRQ_CAUSE) && g_eventOnUp)
    {
        pressedKeys[0] = 0;
        pressedKeys[1] = 0;
        scannedOnOff = FALSE;

        // All previously pressed keys are released
        hal_KeyCallHandler(g_previousKeys, HAL_KEY_UP);

        // on-off button
        if (g_previousOnOff)
        {
            g_keyIrqHandler(255, HAL_KEY_UP);
        }

        // Keep track of the pressed buttons.
        g_previousKeys[0] = pressedKeys[0];
        g_previousKeys[1] = pressedKeys[1];
        g_previousOnOff   = scannedOnOff;
    }

    else

    {
        // Key up or down
        if (status &  KEYPAD_KP_EVT0_IRQ_CAUSE)
        {

            hal_KeySingleScan(scannedKeys);
            scannedOnOff = hal_KeyOnOffStateGet();

            // Send the ***pressed events*** only if user requested it.
            if (g_eventOnPressed && NULL != g_keyIrqHandler)
            {
                // Pressed key detection
                // Those are the one previously pressed and still pressed now
                pressedKeys[0] = (scannedKeys[0] & g_previousKeys[0]);
                pressedKeys[1] = (scannedKeys[1] & g_previousKeys[1]);

                hal_KeyCallHandler(pressedKeys, HAL_KEY_PRESSED);
                // on-off button
                if (g_previousOnOff & scannedOnOff)
                {
                    g_keyIrqHandler(255, HAL_KEY_PRESSED);
                }

            }

            // If the pressed buttons changed.
            if (scannedKeys[0] != g_previousKeys[0] ||
                    scannedKeys[1] != g_previousKeys[1] ||
                    scannedOnOff   != g_previousOnOff)
            {
                // Send the ***down events*** only if user requested it.
                if (g_eventOnDown && NULL != g_keyIrqHandler)
                {
                    // Key that are now pressed but weren't before
                    downKeys[0] = scannedKeys[0] & ~g_previousKeys[0];
                    downKeys[1] = scannedKeys[1] & ~g_previousKeys[1];

                    // low and high keys
                    hal_KeyCallHandler(downKeys, HAL_KEY_DOWN);

                    // on-off button
                    if (~g_previousOnOff & scannedOnOff)
                    {
                        g_keyIrqHandler(255, HAL_KEY_DOWN);
                    }
                }

                // Send the ***up events*** only if user requested it.
                if (g_eventOnUp && NULL != g_keyIrqHandler)
                {
                    // Keys that are now unpressed but were pressed before
                    upKeys[0] = ~scannedKeys[0] & g_previousKeys[0];
                    upKeys[1] = ~scannedKeys[1] & g_previousKeys[1];

                    // low and high keys
                    hal_KeyCallHandler(upKeys, HAL_KEY_UP);

                    // on-off key
                    if (g_previousOnOff & ~scannedOnOff)
                    {
                        g_keyIrqHandler(255, HAL_KEY_UP);
                    }
                }

                // Keep track of the pressed buttons.
                g_previousKeys[0] = scannedKeys[0];
                g_previousKeys[1] = scannedKeys[1];
                g_previousOnOff   = scannedOnOff;
            }
        }
        else
        {
            // There are only already pressed keys
            // still pressed, so no change in pressed keys
            // and no need for a new scan
            if ((status & KEYPAD_KP_ITV_IRQ_CAUSE) && g_eventOnPressed)
            {
                hal_KeyCallHandler(g_previousKeys, HAL_KEY_PRESSED);
                // on-off button
                if (g_previousOnOff)
                {
                    g_keyIrqHandler(255, HAL_KEY_PRESSED);
                }
            }
        }
    }
}
#endif
// =============================================================================
// hal_KeyRemoteHandler
// -----------------------------------------------------------------------------
/// Simulate the press or release of a key. This function is aimed at being
/// executed remotely, by a keyboard emulator integrated in CoolWatcher.
///
/// @param keyCode Key code of the key, as defined in key_defs.h, to press and
/// release.
/// @param state State to set the key to: #HAL_KEY_DOWN, #HAL_KEY_UP, ...
// =============================================================================
PUBLIC VOID hal_KeyRemoteHandler(UINT32 keyCode, HAL_KEY_STATE_T state)
{
    UINT32  keyId = 0;
    BOOL    found = FALSE;

    while (keyId<=HAL_KEY_ON_OFF && !found)
    {
        if (keyCode == tgt_GetKeyCode(keyId))
        {
            // Id of the key found, exit the loop
            found = TRUE;
        }
        else
        {
            // Try next Id.
            keyId += 1;
        }
    }

    if (!found)
    {
        HAL_TRACE(HAL_IO_TRC, 0, "Remotely unknown key: 0x%x - %c", keyCode, keyCode);
        return;
    }

    if (g_keyIrqHandler)
    {
        // Send the key state. taking mask into account.
        if ((g_eventOnUp        && (state == HAL_KEY_UP))
                || (g_eventOnPressed   && (state == HAL_KEY_PRESSED))
                || (g_eventOnDown      && (state == HAL_KEY_DOWN)))
        {
            g_keyIrqHandler(keyId, state);
            HAL_TRACE(HAL_IO_TRC, 0, "Remotely %ded key %c.", state, keyCode);
        }
    }
}



