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




#ifndef BLUETOOTH_HCI_API_DECLARED
#define BLUETOOTH_HCI_API_DECLARED


/* No operation command, may be used for flow control */
#define HCI_cmdNOP                                  0x0000


/* Link types for connection complete, etc */
#define HCI_linkACL         0x01
#define HCI_linkSCO         0x00
#define HCI_linkESCO        0x02

#define HCI_LINKACL_URGENCY 0x81

/* No operation command, may be used for flow control */
#define HCI_cmdNOP                                  0x0000

/*
    HCI link control commands - Opcode Group Field = 0x01
*/


#define HCI_cmdINQUIRY                              ((0x1<<10) | 0x01)
#define HCI_cmdINQUIRYCANCEL                        ((0x1<<10) | 0x02)
#define HCI_cmdPERIODICINQUIRYMODE                  ((0x1<<10) | 0x03)
#define HCI_cmdEXITPERIODICINQUIRYMODE              ((0x1<<10) | 0x04)
#define HCI_cmdCREATECONNECTION                     ((0x1<<10) | 0x05)
#define HCI_cmdDISCONNECT                           ((0x1<<10) | 0x06)
#define HCI_cmdADDSCOCONNECTION                     ((0x1<<10) | 0x07)
#define HCI_cmdCREATECONNECTIONCANCEL               ((0x1<<10) | 0x08)
#define HCI_cmdACCEPTCONNECTIONREQUEST              ((0x1<<10) | 0x09)
#define HCI_cmdREJECTCONNECTIONREQEUST              ((0x1<<10) | 0x0A)
#define HCI_cmdLINKKEYREQUESTREPLY                  ((0x1<<10) | 0x0B)
#define HCI_cmdLINKKEYREQUESTNEGATIVEREPLY          ((0x1<<10) | 0x0C)
#define HCI_cmdPINCODEREQUESTREPLY                  ((0x1<<10) | 0x0D)
#define HCI_cmdPINCODEREQUESTNEGATIVEREPLY          ((0x1<<10) | 0x0E)
#define HCI_cmdCHANGECONNECTIONPACKETTYPE           ((0x1<<10) | 0x0F)
#define HCI_cmdAUTHENTICATIONREQUESTED              ((0x1<<10) | 0x11)
#define HCI_cmdSETCONNECTIONENCRYPTION              ((0x1<<10) | 0x13)
#define HCI_cmdCHANGECONNECTIONLINKKEY              ((0x1<<10) | 0x15)
#define HCI_cmdMASTERLINKKEY                        ((0x1<<10) | 0x17)
#define HCI_cmdREMOTENAMEREQUEST                    ((0x1<<10) | 0x19)
#define HCI_cmdREMOTENAMEREQUESTCANCEL              ((0x1<<10) | 0x1A)
#define HCI_cmdREADREMOTESUPPORTEDFEATURES          ((0x1<<10) | 0x1B)
#define HCI_cmdREADREMOTEEXTENDEDFEATURES           ((0x1<<10) | 0x1C)
#define HCI_cmdREADREMOTEVERSIONINFORMATION         ((0x1<<10) | 0x1D)
#define HCI_cmdREADCLOCKOFFSET                      ((0x1<<10) | 0x1F)
#define HCI_cmdREADLMPHANDLE                        ((0x1<<10) | 0x20)
#define HCI_cmdEXCHANGEFIXEDINFO                    ((0x1<<10) | 0x21)
#define HCI_cmdEXCHANGEALIASINFO                    ((0x1<<10) | 0x22)
#define HCI_cmdEXCHANGEALIASINFO                    ((0x1<<10) | 0x22)
#define HCI_cmdPRIVATEPAIRINGREQUESTREPLY           ((0x1<<10) | 0x23)
#define HCI_cmdPRIVATEPAIRINGREQUESTNEGATIVEREPLY   ((0x1<<10) | 0x24)
#define HCI_cmdGENERATEDALIAS                       ((0x1<<10) | 0x25)
#define HCI_cmdALIASADDRESSREQUESTREPLY             ((0x1<<10) | 0x26)
#define HCI_cmdALIASADDRESSREQUESTNEGATIVEREPLY     ((0x1<<10) | 0x27)
#define HCI_cmdSETUPSYNCHRONOUSCONNECTION           ((0x1<<10) | 0x28)
#define HCI_cmdACCEPTSYNCHRONOUSCONNECTIONREQUEST   ((0x1<<10) | 0x29)
#define HCI_cmdREJECTSYNCHRONOUSCONNECTIONREQUEST   ((0x1<<10) | 0x2A)
#define HCI_cmdIOCAPABILITYREQUESTREPLY             ((0x1<<10) | 0x2B)  //Zhu Jianguo added for the new Feature of BT 2.1 2008.2.25
#define HCI_cmdUSERCONFIRMATIONREQUESTREPLY         ((0x1<<10) | 0x2C)
#define HCI_cmdUSERCONFIRMATIONREQUESTNEGATIVEREPLY ((0x1<<10) | 0x2D)
#define HCI_cmdUSERPASSKEYREQUESTREPLY              ((0x1<<10) | 0x2E)
#define HCI_cmdUSERPASSKEYREQUESTNEGATIVEREPLY      ((0x1<<10) | 0x2F)
#define HCI_cmdREMOTEOOBDATAREQUESTREPLY            ((0x1<<10) | 0x30)
#define HCI_cmdREMOTEOOBDATAREQUESTNEGATIVEREPLY    ((0x1<<10) | 0x33)
#define HCI_cmdIOCAPABILITYREQUESTNEGATIVEREPLY     ((0x1<<10) | 0x34)   //end of added by Zhu Jianguo. 2008.2.25
#define HCI_cmdCREATEPHYSICALLINK                       ((0x1<<10) |0x35)
#define HCI_cmdACCEPTPHYSICALLINK                       ((0x1<<10) |0x36)
#define HCI_cmdDISCONNECTPHYSICALLINK               ((0x1<<10) |0x37)
#define HCI_cmdCREATELOGICALLINK                        ((0x1<<10) |0x38)
#define HCI_cmdACCEPTLOGICALLINK                        ((0x1<<10) |0x39)
#define HCI_cmdDISCONNECTLOGICALLINK                    ((0x1<<10) |0x3A)
#define HCI_cmdLOGICALLINKCANCEL                        ((0x1<<10) |0x3B)
#define HCI_cmdFLOWSPECMODIFY                       ((0x1<<10) |0x3C)


/*
    HCI link policy commands - Opcode Group Field = 0x02
*/

#define HCI_cmdHOLDMODE                             ((0x2<<10) | 0x01)
#define HCI_cmdSNIFFMODE                            ((0x2<<10) | 0x03)
#define HCI_cmdEXITSNIFFMODE                        ((0x2<<10) | 0x04)
#define HCI_cmdPARKMODE                             ((0x2<<10) | 0x05)
#define HCI_cmdEXITPARKMODE                         ((0x2<<10) | 0x06)
#define HCI_cmdQOSSETUP                             ((0x2<<10) | 0x07)
#define HCI_cmdROLEDISCOVERY                        ((0x2<<10) | 0x09)
#define HCI_cmdSWITCHROLE                           ((0x2<<10) | 0x0B)
#define HCI_cmdREADLINKPOLICYSETTINGS               ((0x2<<10) | 0x0C)
#define HCI_cmdWRITELINKPOLICYSETTINGS              ((0x2<<10) | 0x0D)
#define HCI_cmdREADDEFAULTLINKPOLICYSETTINGS        ((0x2<<10) | 0x0E)
#define HCI_cmdWRITEDEFAULTLINKPOLICYSETTINGS       ((0x2<<10) | 0x0F)
#define HCI_cmdFLOWSPECIFICATION                    ((0x2<<10) | 0x10)
#define HCI_cmdSNIFFSUBRATING                       ((0x2<<10) | 0x11)

/*
    HCI host controller and baseband commands - Opcode Group Field = 0x03
*/

#define HCI_cmdSETEVENTMASK                         ((0x3<<10) | 0x01)
#define HCI_cmdRESET                                ((0x3<<10) | 0x03)
#define HCI_cmdSETEVENTFILTER                       ((0x3<<10) | 0x05)
#define HCI_cmdFLUSH                                ((0x3<<10) | 0x08)
#define HCI_cmdREADPINTYPE                          ((0x3<<10) | 0x09)
#define HCI_cmdWRITEPINTYPE                         ((0x3<<10) | 0x0A)
#define HCI_cmdCREATENEWUNITKEY                     ((0x3<<10) | 0x0B)
#define HCI_cmdREADSTOREDLINKKEY                    ((0x3<<10) | 0x0D)
#define HCI_cmdWRITESTOREDLINKKEY                   ((0x3<<10) | 0x11)
#define HCI_cmdDELETESTOREDLINKKEY                  ((0x3<<10) | 0x12)
#define HCI_cmdCHANGELOCALNAME                      ((0x3<<10) | 0x13)
#define HCI_cmdREADLOCALNAME                        ((0x3<<10) | 0x14)
#define HCI_cmdREADCONNECTIONACCEPTTIMEOUT          ((0x3<<10) | 0x15)
#define HCI_cmdWRITECONNECTIONACCEPTTIMEOUT         ((0x3<<10) | 0x16)
#define HCI_cmdREADPAGETIMEOUT                      ((0x3<<10) | 0x17)
#define HCI_cmdWRITEPAGETIMEOUT                     ((0x3<<10) | 0x18)
#define HCI_cmdREADSCANENABLE                       ((0x3<<10) | 0x19)
#define HCI_cmdWRITESCANENABLE                      ((0x3<<10) | 0x1A)
#define HCI_cmdREADPAGESCANACTIVITY                 ((0x3<<10) | 0x1B)
#define HCI_cmdWRITEPAGESCANACTIVITY                ((0x3<<10) | 0x1C)
#define HCI_cmdREADINQUIRYSCANACTIVITY              ((0x3<<10) | 0x1D)
#define HCI_cmdWRITEINQUIRYSCANACTIVITY             ((0x3<<10) | 0x1E)
#define HCI_cmdREADAUTHENTICATIONENABLE             ((0x3<<10) | 0x1F)
#define HCI_cmdWRITEAUTHENTICATIONENABLE            ((0x3<<10) | 0x20)
#define HCI_cmdREADENCRYPTIONMODE                   ((0x3<<10) | 0x21)
#define HCI_cmdWRITEENCRYPTIONMODE                  ((0x3<<10) | 0x22)
#define HCI_cmdREADCLASSOFDEVICE                    ((0x3<<10) | 0x23)
#define HCI_cmdWRITECLASSOFDEVICE                   ((0x3<<10) | 0x24)
#define HCI_cmdREADVOICESETTING                     ((0x3<<10) | 0x25)
#define HCI_cmdWRITEVOICESETTING                    ((0x3<<10) | 0x26)
#define HCI_cmdREADAUTOMATICFLUSHTIMEOUT            ((0x3<<10) | 0x27)
#define HCI_cmdWRITEAUTOMATICFLUSHTIMEOUT           ((0x3<<10) | 0x28)
#define HCI_cmdREADNUMBROADCASTRETRANSMISSIONS      ((0x3<<10) | 0x29)
#define HCI_cmdWRITENUMBROADCASTRETRANSMISSIONS     ((0x3<<10) | 0x2A)
#define HCI_cmdREADHOLDMODEACTIVITY                 ((0x3<<10) | 0x2B)
#define HCI_cmdWRITEHOLDMODEACTIVITY                ((0x3<<10) | 0x2C)
#define HCI_cmdREADTRANSMITPOWERLEVEL               ((0x3<<10) | 0x2D)
#define HCI_cmdREADSCOFLOWCONTROLENABLE             ((0x3<<10) | 0x2E)
#define HCI_cmdWRITESCOFLOWCONTROLENABLE            ((0x3<<10) | 0x2F)
#define HCI_cmdSETHOSTCONTROLLERTOHOSTFLOWCONTROL   ((0x3<<10) | 0x31)
#define HCI_cmdHOSTBUFFERSIZE                       ((0x3<<10) | 0x33)
#define HCI_cmdHOSTNUMBEROFCOMPLETEDPACKETS         ((0x3<<10) | 0x35)
#define HCI_cmdREADLINKSUPERVISIONTIMEOUT           ((0x3<<10) | 0x36)
#define HCI_cmdWRITELINKSUPERVISIONTIMEOUT          ((0x3<<10) | 0x37)
#define HCI_cmdREADNUMBEROFSUPPORTEDIAC             ((0x3<<10) | 0x38)
#define HCI_cmdREADCURRENTIACLAP                    ((0x3<<10) | 0x39)
#define HCI_cmdWRITECURRENTIACLAP                   ((0x3<<10) | 0x3A)
#define HCI_cmdREADPAGESCANPERIODMODE               ((0x3<<10) | 0x3B)
#define HCI_cmdWRITEPAGESCANPERIODMODE              ((0x3<<10) | 0x3C)
#define HCI_cmdREADPAGESCANMODE                     ((0x3<<10) | 0x3D)
#define HCI_cmdWRITEPAGESCANMODE                    ((0x3<<10) | 0x3E)
#define HCI_cmdSETAFHHOSTCHANNELCLASSIFICATION      ((0x3<<10) | 0x3F)
#define HCI_cmdREADINQUIRYSCANTYPE                  ((0x3<<10) | 0x42)
#define HCI_cmdWRITEINQUIRYSCANTYPE                 ((0x3<<10) | 0x43)
#define HCI_cmdREADINQUIRYMODE                      ((0x3<<10) | 0x44)
#define HCI_cmdWRITEINQUIRYMODE                     ((0x3<<10) | 0x45)
#define HCI_cmdREADPAGESCANTYPE                     ((0x3<<10) | 0x46)
#define HCI_cmdWRITEPAGESCANTYPE                    ((0x3<<10) | 0x47)
#define HCI_cmdREADAFHCHANNELASSESSMENTMODE         ((0x3<<10) | 0x48)
#define HCI_cmdWRITEAFHCHANNELASSESSMENTMODE        ((0x3<<10) | 0x49)
#define HCI_cmdREADANONYMITYMODE                    ((0x3<<10) | 0x4A)
#define HCI_cmdWRITEANONYMITYMODE                   ((0x3<<10) | 0x4B)
#define HCI_cmdREADALIASAUTHENTICATIONENABLE        ((0x3<<10) | 0x4C)
#define HCI_cmdWRITEALIASAUTHENTICATIONENABLE       ((0x3<<10) | 0x4D)
#define HCI_cmdREADANONYMOUSADDRESSCHANGEPARAMETERS ((0x3<<10) | 0x4E)
#define HCI_cmdWRITEANONYMOUSADDRESSCHANGEPARAMETERS ((0x3<<10) | 0x4F)
#define HCI_cmdRESETFIXEDADDRESSATTEMPTSCOUNTER     ((0x3<<10) | 0x50)
#define HCI_cmdREADEXTENDEDINQUIRYRESPONSE          ((0x3<<10) | 0x51)
#define HCI_cmdWRITEEXTENDEDINQUIRYRESPONSE         ((0x3<<10) | 0x52)
#define HCI_cmdREFRESHENCRYPTIONKEY                 ((0x3<<10) | 0x53)
#define HCI_cmdREADSIMPLEPAIRINGMODE                ((0x3<<10) | 0x55)
#define HCI_cmdWRITESIMPLEPAIRINGMODE               ((0x3<<10) | 0x56)
#define HCI_cmdREADLOCALOOBDATA                     ((0x3<<10) | 0x57)
#define HCI_cmdREADINQUIRYRESPONSETRANSMITPOWERLEVEL ((0x3<<10)| 0x58)
#define HCI_cmdWRITEINQUIRYTRANSMITPOWERLEVEL       ((0x3<<10) | 0x59)
#define HCI_cmdSENDKEYPRESSNOTIFICATION             ((0x3<<10) | 0x60)
#define HCI_cmdREADDEFAULTERRONEOUSDATAREPORTING    ((0x3<<10) | 0x5A)
#define HCI_cmdWRITEDEFAULTERRONEOUSDATAREPORTING   ((0x3<<10) | 0x5B)
#define HCI_cmdENHANCEDFLUSH                            ((0x3<<10) | 0x5F)
#define HCI_cmdREADLOGICALLINKACCEPTTIMEOUT         ((0x3<<10) | 0x61)
#define HCI_cmdWRITELOGICALLINKACCEPTTIMEOUT        ((0x3<<10) | 0x62)
#define HCI_cmdSETEVENTMASKPAGE2                        ((0x3<<10) | 0x63)
#define HCI_cmdREADLOCATIONDATA                     ((0x3<<10) | 0x64)
#define HCI_cmdWRITELOCATIONDATA                        ((0x3<<10) | 0x65)
#define HCI_cmdREADFLOWCONTROLMODE                  ((0x3<<10) | 0x66)
#define HCI_cmdWRITEFLOWCONTROLMODE                 ((0x3<<10) | 0x67)
#define HCI_cmdREADENHANCEDTRANSMITPOWERLEVEL       ((0x3<<10) | 0x68)
#define HCI_cmdREADBESTEFFORTFLUSHTIMEOUT           ((0x3<<10) | 0x69)
#define HCI_cmdWRITEBESTEFFORTFLUSHTIMEOUT          ((0x3<<10) | 0x6A)
#define HCI_cmdSHORTRANGEMODE                       ((0x3<<10) | 0x6B)
#define HCI_cmdREADLEHOSTSUPPORTED                  ((0X3<<10) | 0x6C)
#define HCI_cmdWRITELEHOSTSUPPORTED                 ((0x3<<10) | 0x6D)


/*
    HCI informational commands - Opcode Group Field = 0x04
*/

#define HCI_cmdREADLOCALVERSIONINFORMATION          ((0x4<<10) | 0x01)
#define HCI_cmdREADLOCALSUPPORTEDCOMMANDS           ((0x4<<10) | 0x02)
#define HCI_cmdREADLOCALSUPPORTEDFEATURES           ((0x4<<10) | 0x03)
#define HCI_cmdREADLOCALEXTENDEDFEATURES            ((0x4<<10) | 0x04)
#define HCI_cmdREADBUFFERSIZE                       ((0x4<<10) | 0x05)
#define HCI_cmdREADCOUNTRYCODE                      ((0x4<<10) | 0x07)
#define HCI_cmdREADBDADDR                           ((0x4<<10) | 0x09)
#define HCI_cmdREADDATABLOCKSIZE                        ((0x4<<10) | 0x0a)

/*
    HCI status commands - Opcode Group Field = 0x05
*/

#define HCI_cmdREADFAILEDCONTACTCOUNTER             ((0x5<<10) | 0x01)
#define HCI_cmdRESETFAILEDCONTACTCOUNTER            ((0x5<<10) | 0x02)
#define HCI_cmdGETLINKQUALITY                       ((0x5<<10) | 0x03)
#define HCI_cmdREADRSSI                             ((0x5<<10) | 0x05)
#define HCI_cmdREADAFHCHANNELMAP                    ((0x5<<10) | 0x06)
#define HCI_cmdREADCLOCK                                ((0x5<<10) | 0x07)
#define HCI_cmdREADENCRYPTIONKEYSIZE                    ((0x5<<10) | 0x08)
#define HCI_cmdREADLOCALAMPINFO                     ((0x5<<10) | 0x09)
#define HCI_cmdREADLOCALAMPASSOC                        ((0x5<<10) | 0x0A)
#define HCI_cmdWRITEREMOTEAMPASSOC                  ((0x5<<10) | 0x0B)

/*
    HCI test commands - Opcode Group Field = 0x06
*/

#define HCI_cmdREADLOOPBACKMODE                     ((0x6<<10) | 0x01)
#define HCI_cmdWRITELOOPBACKMODE                    ((0x6<<10) | 0x02)
#define HCI_cmdENABLEDUT                            ((0x6<<10) | 0x03)
#define HCI_cmdWRITESIMPLEPAIRINGDEBUGMODE          ((0x6<<10) | 0x04)
#define HCI_cmdENABLEAMPRECEIVERREPORTS             ((0x6<<10) | 0x07)
#define HCI_cmdAMPTESTEND                               ((0x6<<10) | 0x08)
#define HCI_cmdAMPTEST                                  ((0x6<<10) | 0x09)

/*
    HCI rda extension commands - Opcode Group Field = 0x3f
*/
#define RDABT_HCI_EXTEND_CMD_OGF                        0x3f

#define HCI_WRITE_LOCAL_DEVICE_ADDRESS   ((0x3f<<10) | 0x1a)
#define HCI_WRITE_UART_BAUD_RATE         ((0x3f<<10) | 0x34)
#define HCI_WAKEUP                       ((0x3f<<10) |0x0c0)

#define HCI_WARM_RESET                   ((0x3f<<10) |0x100)
#define HCI_READ_PHY_REGISTER            ((0x3f<<10) |0x101)
#define HCI_WRITE_PHY_REGISTER           ((0x3f<<10) |0x102)
#define HCI_BASEBAND_TXTEST              ((0x3f<<10) |0x110)
#define HCI_BASEBAND_RXTEST              ((0x3f<<10) |0x111)
#define HCI_BASEBAND_TESTEND             ((0x3f<<10) |0x112)
#define HCI_CALIB_CLOCK                  ((0x3f<<10) |0x177)

#define HCI_SET_LOCAL_KEY                ((0x3f<<10) |0x220)
#define HCI_SET_PEER_KEY                 ((0x3f<<10) |0x221)


//2012-2-2
/*
    HCI LE Controller Commands - Opcode Group Field = 0x08
*/
#define HCI_cmdLESETEVENTMASK               ((0x08<<10) |0x01)
#define HCI_cmdLEREADBUFFERSIZE         ((0x08<<10) |0x02)
#define HCI_cmdLEREADLOCALSUPPORTEDFEATURES ((0x08<<10) |0x03)
#define HCI_cmdLESETRANDOMADDRESS       ((0x08<<10) |0x05)
#define HCI_cmdLESETADVERTISINGPARAMETERS       ((0x08<<10) |0x06)
#define HCI_cmdLEREADADVERTISINGCHANNELTXPOWER  ((0x08<<10) |0x07)
#define HCI_cmdLESETADVERTISINGDATA     ((0x08<<10) |0x08)
#define HCI_cmdLESETSCANRESPONSEDATA        ((0x08<<10) |0x09)
#define HCI_cmdLESETADVERTISEENABLE     ((0x08<<10) |0x0a)
#define HCI_cmdLESETSCANPARAMETERS      ((0x08<<10) |0x0b)
#define HCI_cmdLESETSCANENABLE          ((0x08<<10) |0x0c)
#define HCI_cmdLECREATECONNECTION       ((0x08<<10) |0x0d)
#define HCI_cmdLECREATECONNECTIONCANCEL  ((0x08<<10) |0x0e)
#define HCI_cmdLEREADWHITELISTSIZE      ((0x08<<10) |0x0f)
#define HCI_cmdLECLEARWHITELIST         ((0x08<<10) |0x10)
#define HCI_cmdLEADDDEVICETOWHITELIST   ((0x08<<10) |0x11)
#define HCI_cmdLEREMOVEDEVICEFROMWHITELIST  ((0x08<<10) |0x12)
#define HCI_cmdLECONNECTIONUPDATE       ((0x08<<10) |0x13)
#define HCI_cmdLESETHOSTCHANNELCLASSIFICATION   ((0x08<<10) |0x14)
#define HCI_cmdLEREADCHANNELMAP         ((0x08<<10) |0x15)
#define HCI_cmdLEREADREMOTEUSEDFEATURES ((0x08<<10) |0x16)
#define HCI_cmdLEENCRYPT                    ((0x08<<10) |0x17)
#define HCI_cmdLERAND                       ((0x08<<10) |0x18)
#define HCI_cmdLESTARTENCRYPTION            ((0x08<<10) |0x19)
#define HCI_cmdLELONETERMKEYREQUESTREPLY  ((0x08<<10) |0x1a)
#define HCI_cmdLELONETERMKEYREQUESTNEGATIVEREPLY  ((0x08<<10) |0x1b)
#define HCI_cmdLEREADSUPPORTEDSTATES    ((0x08<<10) |0x1c)
#define HCI_cmdLERECEIVERTEST               ((0x08<<10) |0x1d)
#define HCI_cmdLETRANSMITTERTEST            ((0x08<<10) |0x1e)
#define HCI_cmdLETESTEND                    ((0x08<<10) |0x1f)
#define HCI_cmdLEREMOTECONNECTIONPARAMETERREQUESTREPLY  ((0x08<<10) |0x20)
#define HCI_cmdLEREMOTECONNECTIONPARAMETERREQUESTNEGATIVEREPLY ((0x08<<10) |0x21)
//4.2 new added
#define HCI_cmdLESETDATALENGTH                              ((0x08<<10) |0x22)
#define HCI_cmdLEREADSUGGESTEDDEFAULTDATALENGTH             ((0x08<<10) |0x23)
#define HCI_cmdLEWRITESUGGESTEDDEFAULTDATALENGTH            ((0x08<<10) |0x24)
#define HCI_cmdLEREADLOCALP256PUBLICKEY                     ((0x08<<10) |0x25)
#define HCI_cmdLEGENERATEDHKEY                              ((0x08<<10) |0x26)
#define HCI_cmdLEADDDEVICETORESOLVINGLIST                   ((0x08<<10) |0x27)
#define HCI_cmdLEREMOVEDEVICEFROMRESOLVINGLIST              ((0x08<<10) |0x28)
#define HCI_cmdLECLEARRESOLVINGLIST                         ((0x08<<10) |0x29)
#define HCI_cmdLEREADRESOLVINGLISTSIZE                      ((0x08<<10) |0x2A)
#define HCI_cmdLEREADPEERRESOLVABLEADDRESS                  ((0x08<<10) |0x2B)
#define HCI_cmdLEREADLOCALRESOLVABLEADDRESS                 ((0x08<<10) |0x2C)
#define HCI_cmdLESETADDRESSRESOLUTIONENABLE                 ((0x08<<10) |0x2D)
#define HCI_cmdLESETRESOLVABLEPRIVATEADDRESSTIMEOUT         ((0x08<<10) |0x2E)
#define HCI_cmdLEREADMAXIMUMDATALENGTH                      ((0x08<<10) |0x2F)

/*
    HCI Event Codes
*/
#define HCI_evINQUIRYCOMPLETE                       0x01
#define HCI_evINQUIRYRESULT                         0x02
#define HCI_evCONNECTIONCOMPLETE                    0x03
#define HCI_evCONNECTIONREQUEST                     0x04
#define HCI_evDISCONNECTIONCOMPLETE                 0x05
#define HCI_evAUTHENTICATIONCOMPLETE                0x06
#define HCI_evREMOTENAMEREQUESTCOMPLETE             0x07
#define HCI_evENCRYPTIONCHANGE                      0x08
#define HCI_evCHANGECONNECTIONLINKKEYCOMPLETE       0x09
#define HCI_evMASTERLINKKEYCOMPLETE                 0x0A
#define HCI_evREADREMOTESUPPORTEDFEATURESCOMPLETE   0x0B
#define HCI_evREADREMOTEVERSIONINFORMATIONCOMPLETE  0x0C
#define HCI_evQOSSETUPCOMPLETE                      0x0D
#define HCI_evCOMMANDCOMPLETE                       0x0E
#define HCI_evCOMMANDSTATUS                         0x0F
#define HCI_evHARDWAREERROR                         0x10
#define HCI_evFLUSHOCCURRED                         0x11
#define HCI_evROLECHANGE                            0x12
#define HCI_evNUMCOMPLETEDPACKETS                   0x13
#define HCI_evMODECHANGE                            0x14
#define HCI_evRETURNLINKKEYS                        0x15
#define HCI_evPINCODEREQUEST                        0x16
#define HCI_evLINKKEYREQUEST                        0x17
#define HCI_evLINKKEYNOTIFICATION                   0x18
#define HCI_evLOOPBACKCOMMAND                       0x19
#define HCI_evDATABUFFEROVERFLOW                    0x1A
#define HCI_evMAXSLOTSCHANGE                        0x1B
#define HCI_evREADCLOCKOFFSETCOMPLETE               0x1C
#define HCI_evCONNECTIONPACKETTYPECHANGED           0x1D
#define HCI_evQOSVIOLATION                          0x1E
#define HCI_evPAGESCANMODECHANGE                    0x1F
#define HCI_evPAGESCANREPETITIONMODECHANGE          0x20
#define HCI_evFLOWSPECIFICATIONCOMPLETE             0x21
#define HCI_evINQUIRYRESULTWITHRSSI                 0x22
#define HCI_evREADREMOTEEXTENDEDFEATURESCOMPLETE    0x23
#define HCI_evFIXEDADDRESS                          0x24
#define HCI_evALIASADDRESS                          0x25
#define HCI_evGENERATEALIASREQUEST                  0x26
#define HCI_evACTIVEADDRESS                         0x27
#define HCI_evALLOWPRIVATEPAIRING                   0x28
#define HCI_evALIASADDRESSREQUEST                   0x29
#define HCI_evALIASNOTRECOGNISED                    0x2A
#define HCI_evFIXEDADDRESSATTEMPT                   0x2B
#define HCI_evSYNCHRONOUSCONNECTIONCOMPLETE         0x2C
#define HCI_evSYNCHRONOUSCONNECTIONCHANGED          0x2D
#define HCI_evSNIFFSUBRATING                            0x2E
#define HCI_evEXTENDEDINQUIRYRESULT                 0x2F
#define HCI_evENCRYPTIONKEYREFRESHCOMPLETE          0x30
#define HCI_evIOCAPABILITYREQUEST                       0x31
#define HCI_evIOCAPABILITYRESPONSE                  0x32
#define HCI_evUSERCONFIRMATIONREQUEST               0x33
#define HCI_evUSERPASSKEYREQUEST                        0x34
#define HCI_evREMOTEOOBDATAREQUEST                  0x35
#define HCI_evSIMPLEPAIRINGCOMPLETE                 0x36
#define HCI_evLINKSUPERVISIONTIMEOUT                    0x38
#define HCI_evENHANCEDFLUSHCOMPLETE                 0x39
#define HCI_evUSERPASSKEYNOTIFICATION               0x3B
#define HCI_evKEYPRESSNOTIFICATION                  0x3C
#define HCI_evREMOTEHOSTSUPPORTEDFEATURESNOTIFICATION 0x3D
//added for bt 4.0 events
#define HCI_evLEMETAE                               0x3E
#define HCI_evLECONNECTIONCOMPLETE                  0x01
#define HCI_evLEADVERTISINGREPORT                   0x02
#define HCI_evLECONNECTIONUPDATECOMPLETE            0x03
#define HCI_evLEREADREMOTEUSEDFEATURESCOMPLETE      0x04
#define HCI_evLELONGTERMKEYREQUEST                  0x05
#define HCI_evLEREMOTECONNECTIONPARAMETERREQUEST    0x06
#define HCI_evLEDATALENGTHCHANGE                    0x07
#define HCI_evLEREADLOCALP256PUBLICKEYCOMPLETE      0x08
#define HCI_evLEGENERATEDHKEYCOMPLETE               0x09
#define HCI_evLEENHANCEDCONNECTIONCOMPLETE          0x0a
#define HCI_evLEDIRECTADVERTISINGREPORT             0x0b

#define HCI_evPHYSICALLINKCOMPLETE                  0x40
#define HCI_evCHANNELSELECTED                           0x41
#define HCI_evDISCONNECTIONPHYSICALLINKCOMPLETE     0x42
#define HCI_evPHYSICALLINKLOSSEARLYWARNING          0x43
#define HCI_evPHYSICALLINKRECOVERY                  0x44
#define HCI_evLOGICALLINKCOMPLETE                       0x45
#define HCI_evDISCONNECTIONLOGICALLINKCOMPLETE      0x46
#define HCI_evFLOWSPECMODIFYCOMPLETE                    0x47
#define HCI_evNUMBEROFCOMPLETEDDATABLOCKS           0x48
#define HCI_evSHORTRANGEMODECHANGECOMPLETE          0x49
#define HCI_evAMPSTATUSCHANGE                       0x4A
#define HCI_evAMPSTARTTEST                              0x4B
#define HCI_evAMPTESTEND                                0x4C
#define HCI_evAMPRECEIVERREPORT                     0x4D

#define HCI_evTESTING                               0xFE
#define HCI_evPROPRIETARY                           0xFF


/*
    HCI Status/Error Codes
*/

#define HCI_errNOERROR              0x00
#define HCI_errUNKNOWNCOMMAND       0x01
#define HCI_errNOCONNECTION         0x02
#define HCI_errHWFAILURE            0x03
#define HCI_errPAGETIMEOUT          0x04
#define HCI_errAUTHFAILURE          0x05
#define HCI_errKEYMISSING           0x06
#define HCI_errMEMORYFULL           0x07
#define HCI_errCONNTIMEOUT          0x08
#define HCI_errMAXCONNECTIONS       0x09
#define HCI_errMAXSCOS              0x0A
#define HCI_errACLEXISTS            0x0B
#define HCI_errCMDDISALLOWED        0x0C
#define HCI_errREJRESOURCES         0x0D
#define HCI_errREJSECURITY          0x0E
#define HCI_errREJPERSONALDEV       0x0F
#define HCI_errHOSTTIMEOUT          0x10
#define HCI_errUNSUPPORTED          0x11
#define HCI_errINVALIDPARAM         0x12
#define HCI_errTERMUSER             0x13
#define HCI_errTERMRESOURCES        0x14
#define HCI_errTERMPOWEROFF         0x15
#define HCI_errLOCALTERM            0x16
#define HCI_errREPEATED             0x17
#define HCI_errNOPARING             0x18
#define HCI_errUNKNOWNLMPPDU        0x19
#define HCI_errUNSUPPORTEDREMOTE    0x1A
#define HCI_errSCOOFFSETREJ         0x1B
#define HCI_errSCOINTERVALREJ       0x1C
#define HCI_errSCOMODEREJ           0x1D
#define HCI_errLMPINVALIDPARAM      0x1E
#define HCI_errUNSPECERROR          0x1F
#define HCI_errLMPUNSUPPPARAM       0x20
#define HCI_errNOROLECHANGE         0x21
#define HCI_errLMPTIMEOUT           0x22
#define HCI_errLMPTRANSCOLLISION    0x23
#define HCI_errLMPPDUDISALLOWED     0x24
#define HCI_errBADCRYPTMODE         0x25
#define HCI_errUNITKEYUSED          0x26
#define HCI_errQOSUNSUPPORTED       0x27
#define HCI_errINSTANTPASSED        0x28
#define HCI_errNOUNITPAIRING        0x29


/*
    HCI Function Declarations
*/
#ifdef __cplusplus
extern "C" {
#endif

/* PDU types for HCI_SendRawPDU() HCI_RegisterRxHandler() callback function */
#define HCI_pduERROR        0x00
#define HCI_pduCOMMAND      0x01
#define HCI_pduACLDATA      0x02
#define HCI_pduSCODATA      0x03
#define HCI_pduEVENT        0x04
#define HCI_pduCapture      0x80

/* pdu header lengths */
#define HCI_pduCOMMANDHEADER    0x03
#define HCI_pduACLDATAHEADER    0x04
#define HCI_pduSCODATAHEADER    0x03
#define HCI_pduEVENTHEADER      0x02

/* set up lengths for host_buf.h */
#define HEADERS_FOR_HCITRANSPORT    1  /* pdu type */

#define RDABT_HCI_HEADER_SIZE           HEADERS_FOR_HCITRANSPORT + 4 /* max header=ACL pdu header of 4 bytes */
#define HEADERS_FOR_HCI     RDABT_HCI_HEADER_SIZE

/* handle specifiers used by HCI_GetWriteBuffer */
#define HCI_NO_HANDLE                   0xFFF1
#define HCI_COMMAND_HANDLE              HCI_NO_HANDLE
#define L2CAP_BROADCAST_HANDLE          (0xef1)
#define HCI_ACTIVE_BROADCAST_HANDLE     (L2CAP_BROADCAST_HANDLE|RDABT_HANDLE_FLAG_ACTIVE_BROADCAST )
#define HCI_PICONET_BROADCAST_HANDLE    ((L2CAP_BROADCAST_HANDLE+1)|RDABT_HANDLE_FLAG_PICONET_BROADCAST)
#define HCI_SCO_HANDLE 0x2000
#define HCI_ACL_HANDLE 0x0000

#define HCI_PRIMARY_TRANSPORT          0
#define HCI_AMPWIFI_TRANSPORT     1

APIDECL1 t_api APIDECL2 HCI_GetWriteBuffer(u_int8 transport, u_int16 handle, u_int16 len, struct st_t_dataBuf **writeBuffer);
APIDECL1 t_api APIDECL2 HCI_FreeWriteBuffer(struct st_t_dataBuf *writeBuffer, u_int8 type);
APIDECL1 t_api APIDECL2 HCI_SendRawPDU(u_int8 transport, u_int8 type, struct st_t_dataBuf *pdu);

APIDECL1 t_api APIDECL2 HCI_GetReceiveBuffer(struct st_t_dataBuf **p_buf, u_int32 size);
APIDECL1 t_api APIDECL2 HCI_FreeReceiveBuffer(struct st_t_dataBuf *p_buf, u_int8 type);

APIDECL1 t_api APIDECL2 HCI_SendSCOData(u_int16 handle, u_int32 len, u_int8 *data);
APIDECL1 t_api APIDECL2 HCI_GetSCOData(u_int32 *length, u_int8 *p_data, u_int32 size);


/* Types for command primitives */

typedef struct st_t_HCI_numPackets    /* for HCI_HostNumberOfCompletedPackets() */
{
    u_int16 Handle;
    u_int16 NumCompleted;
} t_HCI_NumPackets;

typedef struct st_t_HCI_WriteLinkKey    /* for HCI_WriteStoredLinkKey() */
{
    t_bdaddr BDAddress;
    u_int8 linkKey[16];
} t_HCI_WriteLinkKey;

typedef struct st_t_HCI_EventFilter    /* for HCI_SetEventFilter() */
{
    u_int8 FilterType;
    u_int8 ConditionType;
    struct st_t_HCI_eventFilter_Condition
    {
        t_bdaddr BDAddress;
        u_int32 DeviceClass;
        u_int32 DeviceClassMask;
        u_int8 AutoAccept;
    } Condition;
} t_HCI_EventFilter;

/*
    HCI link control commands - Opcode Group Field = 0x01
*/
APIDECL1 t_api APIDECL2 HCI_Inquiry(struct st_t_dataBuf **cmd, u_int32 lap, u_int8 inquiryLength, u_int8 numResponses);
APIDECL1 t_api APIDECL2 HCI_InquiryCancel(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_PeriodicInquiryMode(struct st_t_dataBuf **cmd, u_int16 maxPeriodLen, u_int16 minPeriodLen, u_int32 lap, u_int8 inquiryLen, u_int8 numResponses);
APIDECL1 t_api APIDECL2 HCI_ExitPeriodicInquiryMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_CreateConnection(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int16 packetType, u_int8 pageScanRepet, u_int8 pageScanMode, u_int16 clockOffset, u_int8 role);
APIDECL1 t_api APIDECL2 HCI_Disconnect(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 reason);
APIDECL1 t_api APIDECL2 HCI_AddSCOConnection(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 packetType);
APIDECL1 t_api APIDECL2 HCI_CreateConnectionCancel(struct st_t_dataBuf **cmd, t_bdaddr bdaddr);  //added by xiongzhi
APIDECL1 t_api APIDECL2 HCI_AcceptConnectionRequest(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 role);
APIDECL1 t_api APIDECL2 HCI_RejectConnectionRequest(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 reason);
APIDECL1 t_api APIDECL2 HCI_LinkKeyRequestReply(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 *linkKey);
APIDECL1 t_api APIDECL2 HCI_LinkKeyRequestNegativeReply(struct st_t_dataBuf **cmd, t_bdaddr bdaddr);
APIDECL1 t_api APIDECL2 HCI_PINCodeRequestReply(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 pinCodeLen, u_int8 *pincode);
APIDECL1 t_api APIDECL2 HCI_PINCodeRequestNegativeReply(struct st_t_dataBuf **cmd, t_bdaddr bdaddr);
APIDECL1 t_api APIDECL2 HCI_ChangeConnectionPacketType(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 packetType);
APIDECL1 t_api APIDECL2 HCI_AuthenticationRequested(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_SetConnectionEncryption(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 encrypt);
APIDECL1 t_api APIDECL2 HCI_ChangeConnectionLinkKey(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_MasterLinkKey(struct st_t_dataBuf **cmd, u_int8 keyFlag);
APIDECL1 t_api APIDECL2 HCI_RemoteNameRequest(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 pageScanRepet, u_int8 pageScanMode, u_int16 clockOffset);
APIDECL1 t_api APIDECL2 HCI_RemoteNameRequestCancel(struct st_t_dataBuf **cmd, t_bdaddr bdaddr);
APIDECL1 t_api APIDECL2 HCI_ReadRemoteSupportedFeatures(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadRemoteVersionInformation(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadClockOffset(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadRemoteExtendedFeatures(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 PageNumber);
APIDECL1 t_api APIDECL2 HCI_ReadLMPHandle(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_SetupSynchronousConnection(struct st_t_dataBuf **cmd, u_int16 handle, u_int32 TransmitBandwidth, u_int32 ReceiveBandwidth, u_int16 MaxLatency, u_int16 VoiceSetting, u_int8   RetransmissionEffort,u_int16 PacketType);
APIDECL1 t_api APIDECL2 HCI_AcceptSynchronousConnectionRequest(struct st_t_dataBuf **cmd, t_bdaddr address,u_int32 TransmitBandwidth, u_int32 ReceiveBandwidth, u_int16 MaxLatency, u_int16 ContentFormat, u_int8  RetransmissionEffort, u_int16 PacketType);
APIDECL1 t_api APIDECL2 HCI_RejectSynchronousConnectionRequest(struct st_t_dataBuf **cmd, t_bdaddr address, u_int8 reason);
APIDECL1 t_api APIDECL2 HCI_IOCapabilityRequestReply(struct st_t_dataBuf **cmd, t_bdaddr address, u_int8    IOCapability, u_int8    OOBDataPresent, u_int8  AuthenticationRequirements);
APIDECL1 t_api APIDECL2 HCI_UserConfirmationRequestReply(struct st_t_dataBuf **cmd, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_UserConfirmationRequestNegativeReply(struct st_t_dataBuf **cmd, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_UserPasskeyRequestReply(struct st_t_dataBuf **cmd, t_bdaddr address, u_int32 passkey);
APIDECL1 t_api APIDECL2 HCI_UserPasskeyRequestNegativeReply(struct st_t_dataBuf **cmd, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_RemoteOOBDataRequestReply(struct st_t_dataBuf **cmd, t_bdaddr address, u_int8* C, u_int8* R);
APIDECL1 t_api APIDECL2 HCI_RemoteOOBDataRequestNegativeReply(struct st_t_dataBuf **cmd, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_IOCapabilityRequestNegativeReply(struct st_t_dataBuf **cmd, t_bdaddr address, u_int8 reason);
APIDECL1 t_api APIDECL2 HCI_CreatePhysicalLink(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle,  u_int8 AMPkeyLen, u_int8 AMPkeyType, u_int8* AMPkey);
APIDECL1 t_api APIDECL2 HCI_AcceptPhysicalLink(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int8 AMPkeyLen, u_int8 AMPKeyType, u_int8* AMPkey);
APIDECL1 t_api APIDECL2 HCI_DisconnectPhysicalLink(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int8 reason);
APIDECL1 t_api APIDECL2 HCI_CreateLogicalLink(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int8 *TxFlowSpec, u_int8 *RxFlowSpec);
APIDECL1 t_api APIDECL2 HCI_AcceptLogicalLink(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int8 *TxFlowSpec, u_int8 *RxFlowSpec);
APIDECL1 t_api APIDECL2 HCI_DisconnectLogicalLink(struct st_t_dataBuf **cmd, u_int8 LogicalLinkHandle);
APIDECL1 t_api APIDECL2 HCI_LogicalLinkCancel(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int8 TxFlowSpecID);
APIDECL1 t_api APIDECL2 HCI_FlowSpecModify(struct st_t_dataBuf **cmd, u_int16 Handle, u_int8 *TxFlowSpec, u_int8 *RxFlowSpec);

/*
    HCI link policy commands - Opcode Group Field = 0x02
*/
APIDECL1 t_api APIDECL2 HCI_HoldMode(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 maxInterval, u_int16 minInterval);
APIDECL1 t_api APIDECL2 HCI_SniffMode(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 maxInterval, u_int16 minInterval, u_int16 sniffAttempt, u_int16 sniffTimeout);
APIDECL1 t_api APIDECL2 HCI_ExitSniffMode(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ParkMode(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 beaconMaxInterval, u_int16 beaconMinInterval);
APIDECL1 t_api APIDECL2 HCI_ExitParkMode(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_QoSSetup(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 flags, u_int8 serviceType, u_int32 tokenRate, u_int32 peakBandwidth, u_int32 latency, u_int32 delayVar);
APIDECL1 t_api APIDECL2 HCI_RoleDiscovery(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_SwitchRole(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 role);
APIDECL1 t_api APIDECL2 HCI_ReadLinkPolicySettings(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_WriteLinkPolicySettings(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 settings);
APIDECL1 t_api APIDECL2 HCI_ReadDefaultLinkPolicySettings(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteDefaultLinkPolicySettings(struct st_t_dataBuf **cmd, u_int16 defaultSettings);
APIDECL1 t_api APIDECL2 HCI_FlowSpecification(struct st_t_dataBuf **cmd, u_int16 handle, u_int8  flags, u_int8  flowDirection,u_int8  serviceType,u_int32 tokenRate, u_int32 tokenBucketSize, u_int32 peakBandwidth, u_int32 accessLatency);
APIDECL1 t_api APIDECL2 HCI_SniffSubrating(struct st_t_dataBuf **cmd, u_int16 handle,u_int16 maxLatency, u_int16 minRemoteTimeOut, u_int16 minLocalTimeOut);


/*
    HCI host controller and baseband commands - Opcode Group Field = 0x03
*/
APIDECL1 t_api APIDECL2 HCI_SetEventMask(struct st_t_dataBuf **cmd, u_int8 *eventMask);
APIDECL1 t_api APIDECL2 HCI_Reset(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_SetEventFilter(struct st_t_dataBuf **cmd, t_HCI_EventFilter *filter);
APIDECL1 t_api APIDECL2 HCI_Flush(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadPINType(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WritePINType(struct st_t_dataBuf **cmd, u_int8 pinType);
APIDECL1 t_api APIDECL2 HCI_CreateNewUnitKey(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadStoredLinkKey(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 readFlag);
APIDECL1 t_api APIDECL2 HCI_WriteStoredLinkKey(struct st_t_dataBuf **cmd, u_int8 numKeys, t_HCI_WriteLinkKey *links);
APIDECL1 t_api APIDECL2 HCI_DeleteStoredLinkKey(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 deleteFlag);
APIDECL1 t_api APIDECL2 HCI_ChangeLocalName(struct st_t_dataBuf **cmd, u_int8 *name);
APIDECL1 t_api APIDECL2 HCI_ReadLocalName(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadConnectionAcceptTimeout(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteConnectionAcceptTimeout(struct st_t_dataBuf **cmd, u_int16 timeout);
APIDECL1 t_api APIDECL2 HCI_ReadPageTimeout(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WritePageTimeout(struct st_t_dataBuf **cmd, u_int16 timeout);
APIDECL1 t_api APIDECL2 HCI_ReadScanEnable(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteScanEnable(struct st_t_dataBuf **cmd, u_int8 scanEnable);
APIDECL1 t_api APIDECL2 HCI_ReadPageScanActivity(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WritePageScanActivity(struct st_t_dataBuf **cmd, u_int16 interval, u_int16 window);
APIDECL1 t_api APIDECL2 HCI_ReadInquiryScanActivity(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteInquiryScanActivity(struct st_t_dataBuf **cmd, u_int16 interval, u_int16 window);
APIDECL1 t_api APIDECL2 HCI_ReadAuthenticationEnable(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteAuthenticationEnable(struct st_t_dataBuf **cmd, u_int8 enable);
APIDECL1 t_api APIDECL2 HCI_ReadEncryptionMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteEncryptionMode(struct st_t_dataBuf **cmd, u_int8 mode);
APIDECL1 t_api APIDECL2 HCI_ReadClassOfDevice(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteClassOfDevice(struct st_t_dataBuf **cmd, u_int32 cod);
APIDECL1 t_api APIDECL2 HCI_ReadVoiceSetting(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteVoiceSetting(struct st_t_dataBuf **cmd, u_int16 voiceSetting);
APIDECL1 t_api APIDECL2 HCI_ReadAutomaticFlushTimeout(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_WriteAutomaticFlushTimeout(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 timeout);
APIDECL1 t_api APIDECL2 HCI_ReadNumBroadcastRetransmissions(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteNumBroadcastRetransmissions(struct st_t_dataBuf **cmd, u_int8 number);
APIDECL1 t_api APIDECL2 HCI_ReadHoldModeActivity(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteHoldModeActivity(struct st_t_dataBuf **cmd, u_int8 activity);
APIDECL1 t_api APIDECL2 HCI_ReadTransmitPowerLevel(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 type);
APIDECL1 t_api APIDECL2 HCI_ReadSCOFlowControlEnable(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteSCOFlowControlEnable(struct st_t_dataBuf **cmd, u_int8 enable);
APIDECL1 t_api APIDECL2 HCI_SetHostControllerToHostFlowControl(struct st_t_dataBuf **cmd, u_int8 enable);
APIDECL1 t_api APIDECL2 HCI_HostBufferSize(struct st_t_dataBuf **cmd, u_int16 ACLlength, u_int8 SCOlength, u_int16 totalACLpackets, u_int16 totalSCOpackets);
APIDECL1 t_api APIDECL2 HCI_HostNumberOfCompletedPackets(struct st_t_dataBuf **cmd, u_int8 numHandles, t_HCI_NumPackets *numPackets);
APIDECL1 t_api APIDECL2 HCI_ReadLinkSupervisionTimeout(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_WriteLinkSupervisionTimeout(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 timeout);
APIDECL1 t_api APIDECL2 HCI_ReadNumberOfSupportedIAC(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadCurrentIACLAP(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteCurrentIACLAP(struct st_t_dataBuf **cmd, u_int8 numLap, u_int32 *lap);
APIDECL1 t_api APIDECL2 HCI_ReadPageScanPeriodMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WritePageScanPeriodMode(struct st_t_dataBuf **cmd, u_int8 mode);
APIDECL1 t_api APIDECL2 HCI_ReadPageScanMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WritePageScanMode(struct st_t_dataBuf **cmd, u_int8 mode);
APIDECL1 t_api APIDECL2 HCI_SetAFHHostChannelClassification(struct st_t_dataBuf **cmd, u_int8* afh_host_channel_classification);
APIDECL1 t_api APIDECL2 HCI_ReadInquiryScanType(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteInquiryScanType(struct st_t_dataBuf **cmd, u_int8 ScanType);
APIDECL1 t_api APIDECL2 HCI_ReadInquiryMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteInquiryMode(struct st_t_dataBuf **cmd, u_int8 inquiryMode);
APIDECL1 t_api APIDECL2 HCI_ReadPageScanType(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WritePageScanType(struct st_t_dataBuf **cmd, u_int8 PageScanType);
APIDECL1 t_api APIDECL2 HCI_ReadAFHChannelAssessmentMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteAFHChannelAssessmentMode(struct st_t_dataBuf **cmd, u_int8 AFHChnAssessmentMode);
APIDECL1 t_api APIDECL2 HCI_ReadExtendedInquiryResponse(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteExtendedInquiryResponse(struct st_t_dataBuf **cmd, u_int8 FEC_Required, u_int8 *EXInquiryRsp);
APIDECL1 t_api APIDECL2 HCI_RefreshEncryptionKey(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadSimplePairingMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteSimplePairingMode(struct st_t_dataBuf **cmd, u_int8 SimpleMode);
APIDECL1 t_api APIDECL2 HCI_ReadLocalOOBData(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadInquiryResponseTransmitPowerLevel(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteInquiryTransmitPowerLevel(struct st_t_dataBuf **cmd, u_int8 TxPower);
APIDECL1 t_api APIDECL2 HCI_SendKeyPressNotification(struct st_t_dataBuf **cmd, t_bdaddr address, u_int8 Notification);
APIDECL1 t_api APIDECL2 HCI_ReadDefalultErroneousDataReporting(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteDefaultErroneousDataReporting(struct st_t_dataBuf **cmd, u_int8 ErroneousDataReporting);
APIDECL1 t_api APIDECL2 HCI_EnhancedFlush(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 PacketType);
APIDECL1 t_api APIDECL2  HCI_ReadLogicalLinkAcceptTimeout(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2  HCI_WriteLogicalLinkAcceptTimeout(struct st_t_dataBuf **cmd, u_int16 timeSlots);  /* unit: BR/EDR BaseBand Slot */
APIDECL1 t_api APIDECL2  HCI_SetEventMaskPage2(struct st_t_dataBuf **cmd, u_int8 *EventMaskPage2);
APIDECL1 t_api APIDECL2  HCI_ReadLocationData(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2  HCI_WriteLocationData(struct st_t_dataBuf **cmd, u_int8 aware,  u_int8 domain, u_int8 DomainOptions,  u_int8 options);
APIDECL1 t_api APIDECL2  HCI_ReadFlowControlMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2  HCI_WriteFlowControlMode(struct st_t_dataBuf **cmd, u_int8 mode);
APIDECL1 t_api APIDECL2  HCI_ReadEnhancedTransmitPowerLevel(struct st_t_dataBuf **cmd, u_int16 Handle, u_int8 type);
APIDECL1 t_api APIDECL2  HCI_ReadBestEffortFlushTimeout(struct st_t_dataBuf **cmd, u_int16 logicalHandle);
APIDECL1 t_api APIDECL2  HCI_WriteBestEffortFlushTimeout(struct st_t_dataBuf **cmd, u_int16 logicalHandle, u_int32 time); /* unit: microseconds */
APIDECL1 t_api APIDECL2  HCI_ShortRangeMode(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int8 mode);
APIDECL1 t_api APIDECL2  HCI_ReadLeHostSupported(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2  HCI_WriteLeHostSupported(struct st_t_dataBuf **cmd,u_int8 le_support,u_int8 le_simu);

/*
    HCI informational commands - Opcode Group Field = 0x04
*/
APIDECL1 t_api APIDECL2 HCI_ReadFailedContactCounter(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ResetFailedContactCounter(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_GetLinkQuality(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadRSSI(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadAFHChannelMap(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_ReadClock(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 whichClock);
APIDECL1 t_api APIDECL2 HCI_ReadDataBlockSize(struct st_t_dataBuf **cmd);

/*
    HCI status commands - Opcode Group Field = 0x05
*/
APIDECL1 t_api APIDECL2 HCI_ReadLocalVersionInformation(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadLocalSupportedFeatures(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadBufferSize(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadCountryCode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadBDAddress(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LocalSupportedCommands(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LocalExtendedFeatures(struct st_t_dataBuf **cmd, u_int8 page);
APIDECL1 t_api APIDECL2 HCI_ReadEncryptionKeySize(struct st_t_dataBuf **cmd, u_int16 Handle);
APIDECL1 t_api APIDECL2 HCI_ReadLocalAMPInfo(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_ReadLocalAMPassoc(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int16 LenSoFar, u_int16 ampAssocLen);
APIDECL1 t_api APIDECL2 HCI_WriteRemoteAMPassoc(struct st_t_dataBuf **cmd, u_int8 PhyLinkHandle, u_int16 LenSoFar, u_int16 ampAssocRemainingLen, u_int8* ampAssocFrag);

/*
    HCI test commands - Opcode Group Field = 0x06
*/
APIDECL1 t_api APIDECL2 HCI_ReadLoopbackMode(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteLoopbackMode(struct st_t_dataBuf **cmd, u_int8 mode);
APIDECL1 t_api APIDECL2 HCI_EnableDUT(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WriteSimplePairingDebugMode(struct st_t_dataBuf **cmd, u_int8 SimplePairingMode);
APIDECL1 t_api APIDECL2 HCI_EnableAMPReceiverReports(struct st_t_dataBuf **cmd, u_int8 enable, u_int8 interval);
APIDECL1 t_api APIDECL2 HCI_AMPTestEnd(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_AMPTest(struct st_t_dataBuf **cmd, u_int8 controlType);


/*
    HCI rda extension commands - Opcode Group Field = 0x3f
*/
APIDECL1 t_api APIDECL2 HCI_WriteLocalDeviceAddress(struct st_t_dataBuf **cmd, t_bdaddr bd_addr);
APIDECL1 t_api APIDECL2 HCI_WriteUARTBaudrate(struct st_t_dataBuf **cmd, u_int32 baudrate);

APIDECL1 t_api APIDECL2 HCI_ReadPHYRegister(struct st_t_dataBuf **cmd, u_int8 flag, u_int32 address);
APIDECL1 t_api APIDECL2 HCI_WritePHYRegister(struct st_t_dataBuf **cmd, u_int8 flag, const u_int32 address, u_int8 count, const u_int32 * value);
APIDECL1 t_api APIDECL2 HCI_CalibClock(struct st_t_dataBuf **cmd);

APIDECL1 t_api APIDECL2 HCI_BaseBand_TXTest(struct st_t_dataBuf **cmd,  u_int8 hop_frq, u_int8 msg_sco, u_int8 msg_burst,
        u_int8 msg_type, u_int8 power_level,u_int16 PRbs_init, u_int16 msg_data,
        u_int8 tx_packet_type, u_int8 EDR_mode, u_int32 tx_len,
        u_int8 am_addr, u_int32 syncword_low, u_int32 syncword_hight, u_int8 hop_enable);
APIDECL1 t_api APIDECL2 HCI_BaseBand_RXTest(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_BaseBand_TestEnd(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_Wakeup(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_WarmReset(struct st_t_dataBuf **cmd);

APIDECL1 t_api APIDECL2 HCI_Write_Local_Key(struct st_t_dataBuf **cmd, u_int8 *key_x, u_int8 *key_y);
APIDECL1 t_api APIDECL2 HCI_Write_Peer_Key(struct st_t_dataBuf **cmd, u_int8 *key);


//HCI LE Controller Commands - Opcode Group Field = 0x08       2012-2-3
APIDECL1 t_api APIDECL2 HCI_LESetEventMask(struct st_t_dataBuf **cmd, u_int8 *eventMask);
APIDECL1 t_api APIDECL2 HCI_LEReadBufferSize(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEReadLocalSupportedFeatures(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LESetRandomAddress(struct st_t_dataBuf **cmd, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_LESetAdvertisingParameters(struct st_t_dataBuf **cmd, u_int16 advMin, u_int16 advMax,u_int8 advType,
        u_int8 ownAddrType, u_int8 directAddrType, t_bdaddr directAddr, u_int8 advChannMap, u_int8 advFilter);
APIDECL1 t_api APIDECL2 HCI_LEReadAdvertisingChannelTxPower(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LESetAdvertisingData(struct st_t_dataBuf **cmd, u_int8 len, u_int8* data);
APIDECL1 t_api APIDECL2 HCI_LESetScanResponseData(struct st_t_dataBuf **cmd, u_int8 len, u_int8* data);
APIDECL1 t_api APIDECL2 HCI_LESetAdvertiseEnable(struct st_t_dataBuf **cmd, u_int8 enable);
APIDECL1 t_api APIDECL2 HCI_LESetScanParameters(struct st_t_dataBuf **cmd, u_int8 type, u_int16 interval, u_int16 window, u_int8 ownAddrType, u_int8 filter);
APIDECL1 t_api APIDECL2 HCI_LESetScanEnable(struct st_t_dataBuf **cmd, u_int8 scanEn, u_int8 filterEn);
APIDECL1 t_api APIDECL2 HCI_LECreateConnection(struct st_t_dataBuf **cmd, u_int16 interval, u_int16 window, u_int8 filter,
        u_int8 peerAddr_type, t_bdaddr peer_address, u_int8 ownAddr_type,
        u_int16 interval_min, u_int16 interval_max, u_int16 latency, u_int16 timeout,
        u_int16 len_min, u_int16 len_max);
APIDECL1 t_api APIDECL2 HCI_LECreateConnectionCancel(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEReadWhiteListSize(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEClearWhiteList(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEAddDeviceToWhiteList(struct st_t_dataBuf **cmd, u_int8 type, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_LERemoveDeviceFromWhiteList(struct st_t_dataBuf **cmd, u_int8 type, t_bdaddr address);
APIDECL1 t_api APIDECL2 HCI_LEConnectionUpdate(struct st_t_dataBuf **cmd, u_int16 handle, u_int16 interval_min, u_int16 interval_max,
        u_int16 latency, u_int16 timeout, u_int16 len_min, u_int16 len_max);
APIDECL1 t_api APIDECL2 HCI_LESetHostChannelClassification(struct st_t_dataBuf **cmd, u_int8 *channel_map);
APIDECL1 t_api APIDECL2 HCI_LEReadChannelMap(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_LEReadRemoteUsedFeatures(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_LEEncrypt(struct st_t_dataBuf **cmd, u_int8 *key, u_int8* data);
APIDECL1 t_api APIDECL2 HCI_LERand(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEStartEncryption(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 *random_num,
        u_int16 encry_diver, u_int8 *long_key);
APIDECL1 t_api APIDECL2 HCI_LELongTermKeyRequestReply(struct st_t_dataBuf **cmd, u_int16 handle, u_int8 *long_key);
APIDECL1 t_api APIDECL2 HCI_LELongTermKeyRequestNegativeReply(struct st_t_dataBuf **cmd, u_int16 handle);
APIDECL1 t_api APIDECL2 HCI_LEReadSupportedStates(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEReceiverTest(struct st_t_dataBuf **cmd, u_int8 rx_freq);
APIDECL1 t_api APIDECL2 HCI_LETransmitterTest(struct st_t_dataBuf **cmd, u_int8 tx_freq, u_int8 data_len, u_int8 payload);
APIDECL1 t_api APIDECL2 HCI_LETestEnd(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LERemoteConnectionParameterRequestReply(struct st_t_dataBuf **cmd,u_int16 handle,u_int16 interval_min,
        u_int16 interval_max,u_int16 latency,u_int16 timeout,u_int16 min_ce_len,u_int16 max_ce_len);
APIDECL1 t_api APIDECL2 HCI_LERemoteConnectionParameterRequestNegativeReply(struct st_t_dataBuf **cmd,u_int16 handle,u_int8 reason);
APIDECL1 t_api APIDECL2 HCI_LESetDataLength(struct st_t_dataBuf **cmd,u_int16 handle,u_int16 tx_octets,u_int16 tx_time);
APIDECL1 t_api APIDECL2 HCI_LEReadSuggestedDefaultDataLength(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEWriteSuggestedDefaultDataLength(struct st_t_dataBuf **cmd,u_int16 max_tx_octets,u_int16 max_tx_time);
APIDECL1 t_api APIDECL2 HCI_LEReadMaximumDataLength(struct st_t_dataBuf **cmd);

APIDECL1 t_api APIDECL2 HCI_LEReadLocalP256PublicKey(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEGenerateDHKey(struct st_t_dataBuf **cmd,u_int8* remote_P256);
APIDECL1 t_api APIDECL2 HCI_LEAddDeviceToResolvingList(struct st_t_dataBuf **cmd,u_int8 PeerIdentityAddrType,
        u_int8* PeerIdentityAddr,u_int8* PeerIrk,u_int8* LocalIrk);
APIDECL1 t_api APIDECL2 HCI_LERemoveDeviceFromResolvingList(struct st_t_dataBuf **cmd,u_int8 PeerIdentityAddrType,
        u_int8* PeerIdentityAddr);
APIDECL1 t_api APIDECL2 HCI_LEClearResolvingList(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEReadResolvingListSize(struct st_t_dataBuf **cmd);
APIDECL1 t_api APIDECL2 HCI_LEReadPeerResolvableAddress(struct st_t_dataBuf **cmd,u_int8 PeerIdentityAddrType,
        u_int8* PeerIdentityAddr);
APIDECL1 t_api APIDECL2 HCI_LEReadLocalResolvableAddress(struct st_t_dataBuf **cmd,u_int8 PeerIdentityAddrType,
        u_int8* PeerIdentityAddr);
APIDECL1 t_api APIDECL2 HCI_LESetAddressResolutionEnable(struct st_t_dataBuf **cmd,u_int8 enable);
APIDECL1 t_api APIDECL2 HCI_LESetResolvablePrivateAddressTimeout(struct st_t_dataBuf **cmd,u_int16 timeout);



#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BLUETOOTH_HCI_API_DECLARED */
