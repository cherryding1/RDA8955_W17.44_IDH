#ifndef __HTTP_H_
#define __HTTP_H_

#include  "cs_types.h"
#include "ts.h"

#define HTTP_CRITICAL    0x00
#define HTTP_ERROR       0X01
#define HTTP_WARNING     0X02
#define HTTP_INFO        0X03
#define HTTP_DEBUG       0x04
#define HTTP_DUMP        0x05

#define HTTP_FAILED		 0x01
#define HTTP_SUCCESS     0x00

#define kCRLFNewLine     "\r\n"
#define kCRLFLineEnding  "\r\n\r\n"

#define Http_Printf(level, ...)

int32 Http_POST( int32 socketid, const int8 *host,const int8 *passcode,const int8 *mac,
                        const int8 *product_key );
int32 Http_GET( const int8 *host,const int8 *did,int32 socketid );
int32 Http_GetHost( int8 *downloadurl,int8 **host,int8 **url );
int32 Http_ReadSocket( int32 socket,uint8 *Http_recevieBuf,int32 bufLen );
int32 Http_Response_DID( uint8 *Http_recevieBuf,int8 *DID );
int32 Http_Response_Code( uint8 *Http_recevieBuf );
int32 Http_getdomain_port( uint8 *Http_recevieBuf,int8 *domain, int32 *port );

int32 Http_Delete( int32 socketid,const int8 *host,const int8 *did,const int8 *passcode);

int32 Http_HeadLen( uint8 *httpbuf );
int32 Http_BodyLen( uint8 *httpbuf );
int32 Http_GetMD5( uint8 *httpbuf,uint8 *MD5,int8 *strMD5);

#endif // endof __HTTP_H_

