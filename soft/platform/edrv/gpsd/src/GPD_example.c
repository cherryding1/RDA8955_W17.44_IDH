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
#include "sxr_ops.h"
#include "sxs_io.h"
#include "cs_types.h"
#include "sxr_tls.h"
#include "gpsddp_debug.h"
#include "gpsd_config.h"
#include "gpsd_m.h"
#include "tgt_gpsd_cfg.h"
#include "hal_host.h"
#include "gpsd_hw_interface.h"
#include "cos.h"
#include "event.h"
#include "at_common.h"
#include "at_define.h"
#include "pmd_m.h"
#include "at_trace.h"
#include "at_cmd_gps.h"
#define NMEA_STN_LENGTH 	256
#define BIN_EX_LENGTH 		(2+2+2+512)	// size + id + seqNo + Data
#define BIN_HDTL_LENGTH		9			// 9 = head(2) + size(2) + id(2) + chksum(1) + tail(2)
#define BIN_STN_LENGTH 		(BIN_HDTL_LENGTH+2+512)	// 9 + seqNo + Data


//---------------------------------------------------------------------------
// 全局变量
//---------------------------------------------------------------------------
BOOL bBinID3Coming = FALSE;		// 标识GPD数据帧的应答是否收到
BOOL bBinACK149Coming = FALSE;	// 标识149命令的应答是否收到
unsigned char aPkgBIN[100];		// 用来存放芯片发来的应答帧

/**********************************************************************
//说明	:向串口写数据
//参数1	:数据指针
//参数2	:长度
//返回	:无
**********************************************************************/
void GPS_DataSend(uint8 *pr,int16 len)
{
	GPS_UART_PutBytes(GPS_DEFAULT_UART,pr,len);
}

void guoke_uart_send(uint8* cmd)
{
	uint8  buf[256];
	AT_MemZero(buf,sizeof(buf));
	MakeNmeaStn(cmd,strlen(cmd),buf);
	GPS_UART_PutBytes(GPS_DEFAULT_UART,buf,strlen(buf));
}

/**********************************************************************
//说明:在指定内存中找出指定字符串
//参数1:指定内存指针
//参数2:要查找的字符串指针
//返回:第一次匹配该字符串的位置，如没找到则返回-1
**********************************************************************/
int16 ht_SearchStr(uint8 *pBuf,uint8 *pStr)
{
	int16 iBufLen;
	int16 iStrLen;
	int16 iBufPos;
	int16 iStrPos;
	int16 iMatchCnt;

	iBufLen = iStrLen = 0;
	while(pBuf[iBufLen] != 0)
	{
		iBufLen++;
	}
	while(pStr[iStrLen] != 0)
	{
		iStrLen++;
	}

	if(iBufLen < iStrLen) return -1;

	iMatchCnt = iBufPos = iStrPos = 0;
	while(iBufPos < iBufLen)
	{
		if(pBuf[iBufPos] == pStr[iStrPos])
		{
			iMatchCnt++;
			if(iMatchCnt == iStrLen)
			{
				return (iBufPos-iStrLen+1);
			}
			iBufPos++;
			iStrPos++;
		}
		else
		{
			iBufPos++;
			iStrPos = 0;
			iMatchCnt = 0;
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
// 封装NMEA格式的数据帧
//---------------------------------------------------------------------------
void MakeNmeaStn(const char* pSrcStr, int iSrcSize, char* pNmeaStr)
{
  	int i;
  	unsigned char chksum;
  	char ch;

  	if (iSrcSize == 0)
  	{
 		pNmeaStr[0] = 0x00;
    	return;
  	}

  	if (iSrcSize > (NMEA_STN_LENGTH - 7))
  	{
    	iSrcSize = NMEA_STN_LENGTH - 7;
 	}

  	chksum = 0;
  	for (i = 0; i < iSrcSize; i++)
  	{
    	if (pSrcStr[i] != '*')
    	{
      		chksum ^= pSrcStr[i];
    	}
    	else
    	{
      		iSrcSize = i;
      		break;
    	}
  	}

  	pNmeaStr[0] = '$';
  	for (i = 0; i < iSrcSize; i++)
  	{
    	pNmeaStr[i+1] = pSrcStr[i];
  	}
  	pNmeaStr[iSrcSize+1] = '*';

  	ch = (chksum & 0xF0) >> 4;
  	if (ch >= 10)
  	{
    	ch = 'A' + ch - 10;
  	}
  	else
  	{
    	ch = '0' + ch;
  	}
  	pNmeaStr[iSrcSize + 2] = ch;

  	ch = chksum & 0x0F;
  	if (ch >= 10)
  	{
    	ch = 'A' + ch - 10;
  	}
  	else
  	{
    	ch = '0' + ch;
  	}
  	pNmeaStr[iSrcSize + 3] = ch;
  	pNmeaStr[iSrcSize + 4] = '\r';
  	pNmeaStr[iSrcSize + 5] = '\n';
  	pNmeaStr[iSrcSize + 6] = '\0';
}

//---------------------------------------------------------------------------
// 封装BIN格式的数据帧
//---------------------------------------------------------------------------
void vMakeBinStn(const char* pSrcStr, int iSrcSize, char* pBinStr)
{
  	int i;
  	unsigned char chksum;

  	if (iSrcSize == 0)
  	{
    	pBinStr[0] = 0x00;
    	return;
  	}

  	if (iSrcSize > BIN_EX_LENGTH)
  	{
    	iSrcSize = BIN_EX_LENGTH;
  	}

  	chksum = 0;
  	for (i = 0; i < iSrcSize; i++)
  	{
    	chksum ^= pSrcStr[i];
  	}
  
	// HEAD
  	pBinStr[0] = 0xAA;
  	pBinStr[1] = 0xF0;
	// SIZE ID SEQ DATA
  	for (i = 0; i < iSrcSize; i++)
  	{
    	pBinStr[i+2] = pSrcStr[i];
  	}
	// CHKSUM
  	pBinStr[iSrcSize + 2] = chksum&0xFF;
	// TAIL
  	pBinStr[iSrcSize + 3] = 0x0D;
  	pBinStr[iSrcSize + 4] = 0x0A;
}

//---------------------------------------------------------------------------
// 解析应答帧
//---------------------------------------------------------------------------
int ProcBinStn(unsigned short *id, unsigned short *seq, unsigned char *valid)
{
	unsigned short stnLen = 0;
	unsigned char chksum = 0;	
	unsigned char temp = 0;
	int i;
	
	stnLen = (aPkgBIN[3]<<8) | aPkgBIN[2];	// (aPkgBIN[2]<<8) | aPkgBIN[3];
	if(stnLen<10)
		return -1;
	if(stnLen>100)
		return -2;
	
	if( ((aPkgBIN[0]&0xFF) == 0xAA) &&
		((aPkgBIN[1]&0xFF) == 0xF0) &&
		((aPkgBIN[stnLen-2]&0xFF) == 0x0D) &&
		((aPkgBIN[stnLen-1]&0xFF) == 0x0A) )
	{
		chksum = aPkgBIN[stnLen-3];
		for(i=0; i<stnLen-5; i++)	// 5 = head(2) + chksum(1) + tail(2)
		{
			temp ^= aPkgBIN[i+2];
		}
		if(temp != chksum)
		{
			return -4;
		}
		else
		{
			*id  	= ((aPkgBIN[5]&0xFF)<<8) | (aPkgBIN[4]&0xFF);
			*seq 	= ((aPkgBIN[7]&0xFF)<<8) | (aPkgBIN[6]&0xFF);
			*valid 	=  aPkgBIN[8]&0xFF;
			return 0;
		}
	}
	else
	{
		return -3;
	}
}

//---------------------------------------------------------------------------
// 封装并发送GPD数据
//---------------------------------------------------------------------------
void SendBinGpdData(int offset, short seqNo, int gpdSize, char *gpdBuff)
{
	int ii = 0;
	char sSrcBuf[BIN_EX_LENGTH];
	char sDstBuf[BIN_EX_LENGTH + 5];	// 5 = head(2) + chksum(1) + tail(2)
			
	// SIZE
	sSrcBuf[0] = BIN_STN_LENGTH&0xFF;
	sSrcBuf[1] = (BIN_STN_LENGTH>>8)&0xFF;
	// ID
	sSrcBuf[2] = 614&0xFF;
	sSrcBuf[3] = (614>>8)&0xFF;
	// SEQ
	sSrcBuf[4] = seqNo&0xFF;
	sSrcBuf[5] = (seqNo>>8)&0xFF;
	// DATA
	for(ii=0; ii<512; ii++)
	{
		if(offset+ii<gpdSize)
			sSrcBuf[ii+6] = gpdBuff[offset+ii];
		else
			sSrcBuf[ii+6] = 0xFF;
	}
	
	// make binary sentence
	vMakeBinStn( sSrcBuf,
             	 BIN_EX_LENGTH,
             	 sDstBuf);
	// send binary sentence
	GPS_DataSend(sDstBuf, (BIN_EX_LENGTH+5));// 5 = head(2) + chksum(1) + tail(2)
	
	AT_TC(g_sw_GPRS,"SendBinGpdData");
}

//---------------------------------------------------------------------------
// 发送GPD发送完毕命令
//---------------------------------------------------------------------------
void SendBinGpdEndding()
{
	char sSrcBuf[BIN_EX_LENGTH];
	char sDstBuf[BIN_EX_LENGTH + 5];	// 5 = head(2) + chksum(1) + tail(2)

	// SIZE
	sSrcBuf[0] = (BIN_HDTL_LENGTH+2)&0xFF;	// 9+seqNo
	sSrcBuf[1] = ((BIN_HDTL_LENGTH+2)>>8)&0xFF;
	// ID
	sSrcBuf[2] = 614&0xFF;
	sSrcBuf[3] = (614>>8)&0xFF;
	// SEQ
	sSrcBuf[4] = 0xFF;
	sSrcBuf[5] = 0xFF;
	
	// make binary sentence
	vMakeBinStn( sSrcBuf,
             	 6,			// size(2) + id(2) + seqNo(2)
             	 sDstBuf);
	// send binary sentence
	GPS_DataSend(sDstBuf, (BIN_HDTL_LENGTH+2));
}

//---------------------------------------------------------------------------
// 发送命令让串口回到正常的NMEA打印模式
//---------------------------------------------------------------------------
void SendBin2Nmea(int baudrate)
{
	char sSrcBuf[BIN_EX_LENGTH];
	char sDstBuf[BIN_EX_LENGTH + 5];	// 5 = head(2) + chksum(1) + tail(2)

	// SIZE
	sSrcBuf[0] = (BIN_HDTL_LENGTH+5)&0xFF;	// 5 = mode(1) + baudrate(4)
	sSrcBuf[1] = ((BIN_HDTL_LENGTH+5)>>8)&0xFF;
	// ID
	sSrcBuf[2] = 149&0xFF;
	sSrcBuf[3] = (149>>8)&0xFF;
	// MODE
	sSrcBuf[4] = 0x00;
	// BAUDRATE
	sSrcBuf[5] = baudrate&0xFF;
	sSrcBuf[6] = (baudrate>>8)&0xFF;
	sSrcBuf[7] = (baudrate>>16)&0xFF;
	sSrcBuf[8] = (baudrate>>24)&0xFF;
	
	// make binary sentence
	vMakeBinStn( sSrcBuf,
             	 9,			// size(2) + id(2) + mode(1) + baudrate(4)
             	 sDstBuf);
	// send binary sentence
	GPS_DataSend(sDstBuf, (BIN_HDTL_LENGTH+5));
	AT_TC(g_sw_GPRS,"SendBin2Nmea");
}

void Isuartbin149(char *aBinTemp)
{  
    if( ((((aBinTemp[5]&0xFF)<<8)|(aBinTemp[4]&0xFF)) == 1) && ((((aBinTemp[7]&0xFF)<<8)|(aBinTemp[6]&0xFF)) == 149) )
	{
		memcpy(aPkgBIN, aBinTemp, 100);
		bBinACK149Coming = TRUE;
	}
	
    AT_TC(g_sw_GPRS,"Isuartbin149 bBinACK149Coming=%d",bBinACK149Coming);
}
void isbinright(char *aBinTemp,int len)
{
	//bBinACK149Coming = FALSE;
	// 芯片收到一段GPD数据帧之后的应答
	int i;
	int16 pos_aBinTemp;
	uint8 aBinTemp_header[]={0xAA,0xF0};
	pos_aBinTemp=ht_SearchStr(aBinTemp,aBinTemp_header);
	if((pos_aBinTemp>=0)&&((((aBinTemp[pos_aBinTemp+5]&0xFF)<<8)|(aBinTemp[pos_aBinTemp+4]&0xFF)) == 3 ))	
	{
		memcpy(aPkgBIN, &aBinTemp[pos_aBinTemp], 20);
		bBinID3Coming = TRUE;
	}
    AT_TC(g_sw_GPRS,"isbinright bBinID3Coming=%d",bBinID3Coming);

}

void initbinand149(void)
{
	bBinACK149Coming=FALSE;	
	bBinID3Coming = FALSE;
	memset(aPkgBIN, 0, 100);

}
//---------------------------------------------------------------------------
// 发送GPD文件的过程示例
//---------------------------------------------------------------------------
char*  gpdBuff=NULL;//[4000];		// 用来存放GPD文件，以下伪代码中假设GPD文件已经存放在数组里
int gpdSize = 0;		// 用来记录GPD文件的长度，以下伪代码中假设长度已经获取
int m_baudrate = 115200;//记录当前串口通信的波特率
int counter = 0;		// 用于超时计数
int Gpd149resolve(void)
{
	unsigned short id, seq;
	unsigned char valid;
	// 解析收到的149应答
	int ret = ProcBinStn(&id, &seq, &valid);
	if(!ret)
	{
		if( (id == 1) && (seq == 149) )
		{
			switch(valid)
			{
				case 0:
					AT_TC(g_sw_GPRS,"ACK error Invalid");
					return 0;
					
				case 1:
					AT_TC(g_sw_GPRS,"ACK error Unsupport");
					return 0;

				case 2:
					AT_TC(g_sw_GPRS,"ACK error Fail");
					return 0;

				case 3:
					//
					return 1;

				default:
					AT_TC(g_sw_GPRS,"ACK error Unknow");
					return 0;
			}
		}
		else
		{
			AT_TC(g_sw_GPRS,"ACK error, id %d, cmd %d", id, seq);	
			return 0;
		}
	}
	else
	{
		AT_TC(g_sw_GPRS,"ACK error %d, cmd %d", ret, seq);
		return 0;
	}
	AT_TC(g_sw_GPRS,"Gpd149resolve OK,=%d",bBinACK149Coming);
	return 1;

}
int Gdpbinresovle(int seqNo)
{
        
		unsigned short id, seq;
		unsigned char valid;
		int offset = 0;	
		offset = seqNo*512;
		SendBinGpdData(offset, seqNo, gpdSize, gpdBuff);
		
		// 解析应答帧
		counter = 0;
		while(!bBinID3Coming)
		{
				counter++;
				// 超时
				if(counter>200)
				{
					SendBin2Nmea(m_baudrate);
					AT_TC(g_sw_GPRS,"ACK timeout 3");
					return 0;
				}
				COS_Sleep(50);
		}
		int ret = ProcBinStn(&id, &seq, &valid);
		if(!ret)
		{
			if( (id == 3) && (seq == (seqNo)) )
			{
				if( valid == 1 )
				{
					return 1;
				}
				else
				{
					
					AT_TC(g_sw_GPRS,"ACK error 2, seq %d", seqNo);
					return 0;
				}
			}
			else
			{
				AT_TC(g_sw_GPRS,"ACK error 1, seq %d", seq);
				return 0;
			}
		}
		else
		{
			AT_TC(g_sw_GPRS,"ACK error %d, seq %d", ret, seqNo);
			return 0;
		}
		AT_TC(g_sw_GPRS,"Gdpbinresovle ok");   
		return 1;
}

extern struct GPS_STATE g_GPSState; 
INT32 guoke_agps_process(void)
{
	uint16 i;
	uint16 agps_retry=0;
	if(0 == g_GPSState.EPHlen)
	{
		return 1;
	}

	gpdBuff = g_GPSState.pEPH;
	//memset(gpdBuff,0x00,sizeof(gpdBuff));
	//AT_MemCpy(gpdBuff, g_GPSState.pEPH,g_GPSState.EPHlen);
	gpdSize = g_GPSState.EPHlen;
	bBinACK149Coming = FALSE;  
	guoke_uart_send("PGKC149,1,115200");
	g_GPSState.gpsbinary = 1;
	COS_Sleep(100);
	while(1)
	{
	    if(bBinACK149Coming==TRUE)
	    {
	        if(Gpd149resolve()==1){
		      	for(i=0;i<=gpdSize/512;i++)
				{
					bBinID3Coming = FALSE;
					Gdpbinresovle(i);
				}
				AT_TC(g_sw_GPRS,"for end");
				break;
	        }
		else
		{
		   	break;
		}
		 COS_Sleep(10);

	    }
	    else
	    {
	    	guoke_uart_send("PGKC149,1,115200");
		COS_Sleep(100);
		agps_retry++;
		if(agps_retry>5)
			break;

	    }
	}
	SendBin2Nmea(m_baudrate);
	initbinand149();
	g_GPSState.gpsbinary = 0;
	return 0;
}


