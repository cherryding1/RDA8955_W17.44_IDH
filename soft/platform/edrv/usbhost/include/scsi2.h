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

#ifndef _SCSI2_
#define _SCSI2_

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************

//*****************************************************************************
// D E F I N E S
//*****************************************************************************


#define MASS_INTERFACE_CLASS        0x08    // storage class
#define MASS_INTERFACE_SUBCLASS     0x06    // SCSI command set


#define USBSCSI_SIG     'SCSI'

// timeout values (milliseconds)

#define SCSI_MEDIA_POLL_INTERVAL    1250

#define SCSI_READ_SECTOR_TIMEOUT    10000

#define SCSI_WRITE_SECTOR_TIMEOUT   10000

#define SCSI_COMMAND_TIMEOUT        5000

// TEST UNIT READY timeout

#define UNIT_ATTENTION_REPEAT       10

// sizes
#define FAT_SECTOR_SIZE             512
#define CDSECTOR_SIZE               2048
#define MAX_SECTOR_SIZE             CDSECTOR_SIZE
// #define SG_BUFF_SIZE                65536
#define SCSI_CDB_6                  6
#define SCSI_CDB_10                 10
#define UFI_CDB                     12
#define ATAPI_CDB                   12
#define MAX_CDB                     UFI_CDB

#define VALID_CONTEXT( p ) \
    ( p != NULL && USBSCSI_SIG == p->Sig )

#define ACCEPT_IO( p ) \
   ( VALID_CONTEXT( p ) && \
     p->Flags.Open && \
    !p->Flags.DeviceRemoved  && \
    !p->Flags.PoweredDown )

// SCSI-2 device types
#define SCSI_DEVICE_DIRECT_ACCESS     0x00
#define SCSI_DEVICE_SEQUENTIAL_ACCESS 0x01
#define SCSI_DEVICE_PRINTER           0x02
#define SCSI_DEVICE_PROCESSOR         0x03
#define SCSI_DEVICE_WRITE_ONCE        0x04
#define SCSI_DEVICE_CDROM             0x05
#define SCSI_DEVICE_SCANNER           0x06
#define SCSI_DEVICE_OPTICAL           0x07
#define SCSI_DEVICE_MEDIUM_CHANGER    0x08
#define SCSI_DEVICE_COMM              0x09
#define SCSI_DEVICE_UNKNOWN           0x1F

// SCSI-2 medium types
#define SCSI_MEDIUM_UNKNOWN           0xFF


// mandatory SCSI-2 commands
#define SCSI_TEST_UNIT_READY      0x00
#define SCSI_REQUEST_SENSE        0x03
#define SCSI_INQUIRY              0x12
#define SCSI_SEND_DIAGNOSTIC      0x1D

// mandatory device-specific SCSI-2 commands
#define SCSI_READ10               0x28
#define SCSI_READ_CAPACITY        0x25

// optional device-specific SCSI-2 commands
#define SCSI_MODE_SELECT6         0x15
#define SCSI_MODE_SENSE6          0x1A
#define SCSI_START_STOP           0x1B
#define SCSI_WRITE10              0x2A
#define SCSI_MODE_SELECT10        0x55
#define SCSI_MODE_SENSE10         0x5A


// mode pages
#define MODE_PAGE_FLEXIBLE_DISK   0x05


// SCSI-2 sense keys
#define SENSE_NONE                0x00
#define SENSE_RECOVERED_ERROR     0x01
#define SENSE_NOT_READY           0x02
#define SENSE_MEDIUM_ERROR        0x03
#define SENSE_HARDWARE_ERROR      0x04
#define SENSE_ILLEGAL_REQUEST     0x05
#define SENSE_UNIT_ATTENTION      0x06
#define SENSE_DATA_PROTECT        0x07
#define SENSE_BLANK_CHECK         0x08

// SCSI-2 ASC
#define ASC_LUN                   0x04
#define ASC_INVALID_COMMAND_FIELD 0x24
#define ASC_MEDIA_CHANGED         0x28
#define ASC_RESET                 0x29
#define ASC_COMMANDS_CLEARED      0x2F
#define ASC_MEDIUM_NOT_PRESENT    0x3A

#define START                     TRUE
#define STOP                      FALSE

#define FAILURE (-1)


typedef struct _DISK_INFO
{
    uint32 di_total_sectors;
    uint32 di_bytes_per_sect;
    uint32 di_cylinders;
    uint32 di_heads;
    uint32 di_sectors;
    uint32 di_flags;       // see definitions below
} DISK_INFO, * PDISK_INFO;

//
// DISK_INFO.di_flags bit definitions
//
#define DISK_INFO_FLAG_MBR           0x00000001 // device type has/needs an MBR
#define DISK_INFO_FLAG_CHS_UNCERTAIN 0x00000002 // driver generated CHS values are suspect
#define DISK_INFO_FLAG_UNFORMATTED   0x00000004 // low level format (DISK_IOCTL_FORMAT_MEDIA) needed
#define DISK_INFO_FLAG_PAGEABLE      0x00000008 // device can be used for demand paging


//*****************************************************************************
// T Y P E D E F S
//*****************************************************************************

// registry configurable timeout/repeat values
typedef struct _TIMEOUTS
{
    uint32 MediaPollInterval;
    uint32 ReadSector;
    uint32 WriteSector;
    uint32 ScsiCommandTimeout;
    uint32 UnitAttnRepeat;
} TIMEOUTS, *PTIMEOUTS;

// device flags
typedef struct _DEVICE_FLAGS
{
    uint32 RMB : 1;              // removable media bit (RMB)
    uint32 MediumPresent : 1;    // a medium is present
    uint32 FSDMounted : 1;       // is FSD mounted, deprecated
    uint32 WriteProtect : 1;     // medium/device is write-protected
    uint32 Open : 1;             // device is open (DSK_Xxx)
    uint32 DeviceRemoved : 1;    // device marked for removal; only set by UsbDiskAttach/UsbDiskDetach
    uint32 PoweredDown : 1;      // device marked for suspend; only set by DSK_PowerDown
    uint32 Busy : 1;             // device marked as busy (CD-ROM)
    uint32 MediumChanged : 1;    // medium has changed
    uint32 prevMediumStatus : 1; // previous medium changed status
} DEVICE_FLAGS, *PDEVICE_FLAGS;

// device abstraction
typedef struct _SCSI_DEVICE
{
    uint32           Sig;                  // signature
    uint8            DiskSubClass;         // bInterfaceSubClass from device descriptor (fr. UsbDiskAttach)
    HANDLE           hUsbTransport;        // USB Mass Storage Transport object (BOT, CBIT) (fr. UsbDiskAttach)
    HANDLE           Lock;                 // object lock
    HANDLE           hMediaPollThread;     // media polling thread (only used with UFI)
    HANDLE           hMediaChangedEvent;   // media change event
    HANDLE           hStreamDevice;        // associated streams interface (we call ActivateDevice on ActivePath)
    HANDLE           hProc;                // process handle
    uint8            DeviceType;           // SCSI-2 device type
    uint8            Lun;                  // logical unit number
    uint8            MediumType;           // SCSI-2 medium type
    DEVICE_FLAGS     Flags;                // device flags (characteristics and state)
    int32            OpenCount;            // DSK_Open reference count
    TIMEOUTS         Timeouts;             // timeout/repeat values
    DISK_INFO        DiskInfo;             // disk geometry
} SCSI_DEVICE, *PSCSI_DEVICE;


//*****************************************************************************
// P R O T O T Y P E S
//*****************************************************************************

// Command Block
struct _TRANSPORT_COMMAND;


// Data Block
struct _TRANSPORT_DATA_BUFFER;


PSCSI_DEVICE ScsiUsbDiskAttach(void * configuration, uint8 index);

void ScsiUsbDiskDetach(PSCSI_DEVICE pDevice);

uint32 ScsiGetSenseData(PSCSI_DEVICE pDevice, uint8 Lun);

uint32 ScsiInquiry( PSCSI_DEVICE pDevice, uint8 Lun);

uint32 ScsiModeSense10(PSCSI_DEVICE pDevice, uint8 Lun);

uint32 ScsiModeSense6(PSCSI_DEVICE pDevice, uint8 Lun);

uint32 ScsiPassthrough(PSCSI_DEVICE pDevice, struct _TRANSPORT_COMMAND* Command, struct _TRANSPORT_DATA_BUFFER* Data);

uint32 ScsiReadCapacity(PSCSI_DEVICE pDevice, PDISK_INFO pDiskInfo, uint8 Lun);

uint32 ScsiReadWrite(PSCSI_DEVICE pDevice, uint32 dwStartSector, uint32 dwNumSectors, void* pvBuffer, uint32* pdwTransferSize,  uint8 Lun, BOOL bRead);

uint32 ScsiRWSG( PSCSI_DEVICE pDevice, uint8* pBuf, uint32 nstartSec, uint8 Lun, uint8 bRead, int * pStatus);

uint32 ScsiRequestSense(PSCSI_DEVICE pDevice, struct _TRANSPORT_DATA_BUFFER* pTData, uint8 Lun);

uint32 ScsiSendDiagnostic(PSCSI_DEVICE pDevice, uint8 Lun);

uint32 ScsiStartStopUnit(PSCSI_DEVICE pDevice, BOOL Start, BOOL LoEj, uint8 Lun);

uint32 ScsiTestUnitReady(PSCSI_DEVICE pDevice, uint8 Lun);

uint32 ScsiUnitAttention(PSCSI_DEVICE pDevice, uint8 Lun);


#endif // _SCSI2_

