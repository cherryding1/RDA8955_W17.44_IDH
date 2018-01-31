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


#ifndef _GD_DRV_LCD_INIT_CODE_H_
#define _GD_DRV_LCD_INIT_CODE_H_


//c121 赛特 亚星 GD_ALL_WRITE
#ifdef GD_TFT_9163C_SAITE_YAXIN
extern VOID lcddp_Init_9163C_SAITE_YAXIN(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_SAITE_YAXIN();
#endif

//c01 康维斯//本派力拓
#ifdef GD_TFT_C01_KANGWEISI
extern VOID lcddp_Init_C01_KANGWEISI(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_C01_KANGWEISI();
#endif


//c01d 艾立丰
#ifdef GD_TFT_C01D_AILIFENG
extern VOID lcddp_Init_C01D_AILIFENG(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_C01D_AILIFENG();
#endif

//c121希而可 f29
#ifdef GD_TFT_C121_XIERKE_F29
extern  VOID lcddp_Init_C121_XIERKE_F29(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_C121_XIERKE_F29();
#endif
//
//c121天利屏// 威而肯//c01d //C121B 奥利丰
#ifdef GD_TFT_C121_TIANLI
extern  VOID lcddp_Init_C121_TIANLI(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_C121_TIANLI();
#define GD_LCD_DIRECT_ROTATION   0xA0
#define GD_LCD_DIRECT_DEFALUT    0xC0
#endif
//20120410
//德世普 解决背光闪动问题
#ifdef GD_TFT_9163C_DESHIPU
extern   VOID lcddp_Init_9163C_DESHIPU(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_DESHIPU();
#endif
//C01D聚腾达//模组维拓
#ifdef GD_TFT_9163C_WEITUO
extern   VOID lcddp_Init_9163C_WEITUO(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_WEITUO();
#endif
//c01d摩西//中光电
#ifdef GD_TFT_9163C_MOXI_ZHONGGUANDIAN
extern   VOID lcddp_Init_9163C_MOXI_ZHONGGUANDIAN(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_MOXI_ZHONGGUANDIAN();
#endif
//c01d摩西//亚新屏
#ifdef GD_TFT_9163C_MOXI_YAXIN
#define GD_TFT_9163C_ZHONGGUANGDIAN
// extern   VOID lcddp_Init_9163C_MOXI_YAXIN(VOID);
//#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_MOXI_YAXIN();
#endif
//20120420
//c01 威而肯天利
#ifdef GD_TFT_9163C_TIANlLI
extern   VOID lcddp_Init_9163C_TIANLI(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_TIANLI();
#define GD_LCD_DIRECT_ROTATION   0xA0
#define GD_LCD_DIRECT_DEFALUT    0xC0
#endif

//c01d 东欣
//中广电屏厂
//2012-04-27
#ifdef GD_TFT_9163C_ZHONGGUANGDIAN
extern   VOID lcddp_Init_9163C_zhongguangdian(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_zhongguangdian();
#endif
//c10d中光电//智博
#ifdef GD_TFT_9163C_ZHONGGUANGDIAN_ZHIBO
extern   VOID lcddp_Init_9163C_zhongguangdian_zhibo(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_9163C_zhongguangdian_zhibo();
#endif
//天威c01d
#ifdef GD_TFT_C01D_9163C_TIANWEI
extern   VOID lcddp_Init_C01D_9163C_TIANWEI(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_C01D_9163C_TIANWEI();
#endif

//天威c01d---中光电
#ifdef GD_TFT_C01D_9163C_TIANWEI_ZGD
extern   VOID lcddp_Init_C01D_9163C_TIANWEI_ZGD(VOID);
#define GD_LCDDP_INIT_FUNC  lcddp_Init_C01D_9163C_TIANWEI_ZGD();
#endif

#endif // _LCDD_CONFIG_H_




