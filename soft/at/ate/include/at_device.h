/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _AT_DEVICE_H_
#define _AT_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>

struct AT_DISPATCH_T;
struct AT_DEVICE_T;
typedef struct AT_DEVICE_T AT_DEVICE_T;

typedef enum AT_DEVICE_LP_MODE_T {
    AT_DEVICE_LP_DISABLE,
    AT_DEVICE_LP_BY_DTR,
    AT_DEVICE_LP_BY_RX
} AT_DEVICE_LP_MODE_T;

// V.250 ICF
typedef enum AT_DEVICE_FORMAT_T {
    AT_DEVICE_FORMAT_AUTO_DETECT,
    AT_DEVICE_FORMAT_8N2,
    AT_DEVICE_FORMAT_811,
    AT_DEVICE_FORMAT_8N1,
    AT_DEVICE_FORMAT_7N2,
    AT_DEVICE_FORMAT_711,
    AT_DEVICE_FORMAT_7N1
} AT_DEVICE_FORMAT_T;

// V.250 ICF
typedef enum AT_DEVICE_PARITY_T {
    AT_DEVICE_PARITY_ODD,
    AT_DEVICE_PARITY_EVEN,
    AT_DEVICE_PARITY_MARK,
    AT_DEVICE_PARITY_SPACE
} AT_DEVICE_PARITY_T;

// V.250 IFC
typedef enum AT_DEVICE_RXFC_T {
    AT_DEVICE_RXFC_NONE,
    AT_DEVICE_RXFC_HW = 2
} AT_DEVICE_RXFC_T;

// V.250 IFC
typedef enum AT_DEVICE_TXFC_T {
    AT_DEVICE_TXFC_NONE,
    AT_DEVICE_TXFC_HW = 2
} AT_DEVICE_TXFC_T;

struct AT_DEVICE_T
{
    void (*destroy)(AT_DEVICE_T *th);
    bool (*open)(AT_DEVICE_T *th);
    void (*close)(AT_DEVICE_T *th);
    int (*write)(AT_DEVICE_T *th, const uint8_t *data, unsigned length);
    int (*read)(AT_DEVICE_T *th, uint8_t *data, unsigned size);
    void (*sleep)(AT_DEVICE_T *th, bool wakeupEnabled);
    void (*wakeup)(AT_DEVICE_T *th);
    bool (*isBaudSupported)(AT_DEVICE_T *th, int baud);
    bool (*setFlowCtrl)(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc);
    void (*setFormat)(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity);

    uint8_t devno;
    struct AT_DISPATCH_T *recv;
};

static inline uint8_t at_DeviceGetNumber(AT_DEVICE_T *th) { return th->devno; }
static inline void at_DeviceSetDispatch(AT_DEVICE_T *th, struct AT_DISPATCH_T *recv) { th->recv = recv; }
static inline struct AT_DISPATCH_T *at_DeviceGetDispatch(AT_DEVICE_T *th) { return th->recv; }
void at_DeviceScheduleRead(AT_DEVICE_T *th);

void at_DeviceDestroy(AT_DEVICE_T *th);
bool at_DeviceOpen(AT_DEVICE_T *th);
void at_DeviceClose(AT_DEVICE_T *th);
int at_DeviceWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length);
int at_DeviceRead(AT_DEVICE_T *th, uint8_t *data, unsigned length);
void at_DeviceSleep(AT_DEVICE_T *th, bool wakeupEnabled);
void at_DeviceWakeup(AT_DEVICE_T *th);
bool at_DeviceIsBaudSupported(AT_DEVICE_T *th, int baud);
bool at_DeviceSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc);
void at_DeviceSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity);

AT_DEVICE_T *at_GetDeviceByChannel(int channel);

#endif