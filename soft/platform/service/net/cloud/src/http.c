#include "cs_types.h"
#include "cos.h"
#include "string.h"
#include "stdlib.h"
#include "tcpip_netif.h"
#include "tcpip_netdb.h"
#include "http.h"

extern int32 g_MQTTStatus;

int32 Http_POST( int32 socketid, const int8 *host,const int8 *passcode,const int8 *mac,const int8 *product_key )
{
    int32 ret=0;
    uint8 *postBuf=NULL;
    int8 url[] = "/dev/devices";
    int8 Content[100]={0};
    int32 ContentLen=0;
    int32 totalLen=0;
    
    postBuf = (uint8*)COS_MALLOC(400);
    if (postBuf==NULL) return 1;
    //g_globalvar.http_sockettype =HTTP_GET_DID;//http_sockettype=1 :http_post type.

    sprintf(Content,"passcode=%s&mac=%s&product_key=%s",passcode,mac,product_key);
    ContentLen=strlen((char*)Content);
    snprintf( (char *)postBuf,400,"%s %s %s%s%s %s%s%s %d%s%s%s%s%s",
              "POST" ,url,"HTTP/1.1",kCRLFNewLine,
              "Host:",host,kCRLFNewLine,
              "Content-Length:",ContentLen,kCRLFNewLine,
              "Content-Type: application/x-www-form-urlencoded",kCRLFNewLine,
              kCRLFNewLine,
              Content
        );
    totalLen = strlen( (char *)postBuf );
    Http_Printf(HTTP_DEBUG,"http_post:%s %d",postBuf,totalLen);
    ret = send( socketid,postBuf,totalLen,0 );
    Http_Printf(HTTP_DEBUG,"http_post ret: %d",ret);
    COS_FREE( postBuf );
    return 0;
}

int32 Http_GET( const int8 *host,const int8 *did,int32 socketid )
{
    static int8 *getBuf=NULL;
    int32 totalLen=0;
    int32 ret=0;
    int8 url[] = "/dev/devices/";

    getBuf = (int8*)COS_MALLOC( 200 );
    if(getBuf == NULL)
    {
        return 1;
    }
    memset( getBuf,0,200 );
    //g_globalvar.http_sockettype =HTTP_PROVISION;//http get type.
    snprintf( getBuf,200,"%s %s%s %s%s%s %s%s%s%s%s",
              "GET",url,did,"HTTP/1.1",kCRLFNewLine,
              "Host:",host,kCRLFNewLine
              "Cache-Control: no-cache",kCRLFNewLine,
              "Content-Type: application/x-www-form-urlencoded",kCRLFLineEnding);
    totalLen =strlen((char*)getBuf );
    ret = send( socketid, getBuf,totalLen,0 );
    Http_Printf(HTTP_DEBUG,"Sent provision:\n %s\n", getBuf);
    COS_FREE(getBuf);
    getBuf = NULL;

    if(ret<=0 ) 
    {
        return 1;
    }
    else
    {
        return 0;
    }    
}
/******************************************************
 *
 *   FUNCTION        :   delete device did by http 
 *     
 *   return          :   0--send successful 
 *                       1--fail. 
 *   Add by Alex lin  --2014-12-16
 *
 ********************************************************/
int32 Http_Delete( int32 socketid, const int8 *host,const int8 *did,const int8 *passcode )
{
    int32 ret=0;
    int8 *sendBuf=NULL;
    int8 url[] = "/dev/devices";
    int8 *Content = NULL;
    int32 ContentLen=0;
    int32 totalLen=0;
    int8 *DELETE=NULL;
    int8 *HOST=NULL;
    int8 Content_Length[20]={0};
    int8 contentType[]="Content-Type: application/x-www-form-urlencoded\r\n\r\n";

    DELETE = (int8*)COS_MALLOC(strlen("DELETE  HTTP/1.1\r\n")+strlen((char*)url)+1);//+1 for sprintf
    if( DELETE ==NULL ) 
    {
        return 1;
    }
    HOST = (int8*)COS_MALLOC(strlen("Host: \r\n")+strlen((char*)host)+1);// +1 for sprintf
    if( HOST==NULL)
    {
      COS_FREE(DELETE);
      return 1;
    }
    Content = (int8*)COS_MALLOC(strlen("did=&passcode=")+strlen((char*)did)+strlen((char*)passcode)+1);// +1 for sprintf
    if( Content==NULL )
    {
      COS_FREE(DELETE);
      COS_FREE(HOST);
      return 1;      
    }

    sprintf(Content,"did=%s&passcode=%s",did,passcode);
    ContentLen=strlen((char*)Content);
    sprintf(DELETE,"DELETE %s HTTP/1.1\r\n",url);
    sprintf(HOST,"Host: %s\r\n",host);
    sprintf(Content_Length,"Content-Length: %d\r\n",ContentLen);
    sendBuf = (int8*)COS_MALLOC(strlen((char*)DELETE)+strlen((char*)HOST)+strlen((char*)Content_Length)+strlen((char*)contentType)+ContentLen+1);//+1 for sprintf
    if (sendBuf==NULL) 
    {
      COS_FREE(DELETE);
      COS_FREE(HOST);
      COS_FREE(Content);
      return 1;
    }
    sprintf(sendBuf,"%s%s%s%s%s",DELETE,HOST,Content_Length,contentType,Content);
    totalLen = strlen((char*)sendBuf);
    ret = send( socketid, sendBuf,totalLen,0 );
    if(ret<=0)
    {
      Http_Printf(HTTP_ERROR," send fail %s %s %d",__FILE__,__FUNCTION__,__LINE__);
      return 1;
    }
    
    Http_Printf( HTTP_DEBUG , "totalLen = %d",totalLen);
    Http_Printf(HTTP_DEBUG,"%s",sendBuf);

    COS_FREE(DELETE);
    COS_FREE(HOST);
    COS_FREE(Content);
    COS_FREE(sendBuf);
    return 0;
}
/******************************************************
        functionname    :   Http_ReadSocket
        description     :   read data form socket
        socket          :   http server socket
        Http_recevieBuf :   data buf.
        bufLen          :   read data length 
        return          :   >0 the data length read form 
                            socket 
                            <0 error,and need to close 
                            the socket.
******************************************************/
int32 Http_ReadSocket( int32 socket,uint8 *Http_recevieBuf,int32 bufLen )
{
    int32 bytes_rcvd = 0; 
    if( socket<0 )
        return bytes_rcvd;
    memset(Http_recevieBuf, 0, bufLen);  

    bytes_rcvd = recv(socket, Http_recevieBuf, bufLen, 0 );
    if(bytes_rcvd <= 0)
    {
        Http_Printf(HTTP_DEBUG,"Close HTTP Socket[%d].", socket);
        return -2;
    }
    return bytes_rcvd;
}
/******************************************************
 *
 *   Http_recevieBuf :   http receive data
 *   return          :   http response code
 *   Add by Alex lin  --2014-09-11
 *
 ********************************************************/
int32 Http_Response_Code( uint8 *Http_recevieBuf )
{
    int32 response_code=0;
    int8* p_start = NULL;
    int8* p_end =NULL; 
    int8 re_code[10] ={0};
    memset(re_code,0,sizeof(re_code));

    p_start = (int8*)strstr((const char*)Http_recevieBuf, (char*)" " );
    if(NULL == p_start)
    {
        return HTTP_FAILED;
    }
    p_end = (int8*)strstr( (const char*)++p_start, (char*)" " );
    if(p_end)
    {
        if(p_end - p_start > sizeof(re_code))
        {
            return HTTP_FAILED;
        }
        memcpy( re_code,p_start,(p_end-p_start) );
    }
    
    response_code = atoi((char*)re_code);
    return response_code;
}


int32 Http_getdomain_port( uint8 *Http_recevieBuf,int8 *domain,int32 *port )
{
    int8 *p_start = NULL;
    int8 *p_end =NULL;
    int8 Temp_port[10]={0};
    memset( domain,0,100 );
    p_start = (int8*)strstr( (char *)Http_recevieBuf, (char*)"host=");
    if( p_start==NULL ) return 1;
    p_start = p_start+strlen((char*)"host=");
    p_end = (int8*)strstr((char*)p_start,"&");
    if( p_end==NULL )   return 1;
    memcpy( domain,p_start,( p_end-p_start ));
    domain[p_end-p_start] = '\0';
    p_start = (int8*)strstr((char*)(++p_end), (char*)"port=");
    if( p_start==NULL ) return 1;
    p_start =p_start+strlen((char*)"port=");
    p_end = (int8*)strstr( (char*)p_start, (char*)"&" );
    memcpy(Temp_port,p_start,( p_end-p_start));
    *port = atoi((char*)Temp_port);
    return 0;
}


/*********************************************************************
*
*   FUNCTION       :   TO get the http headlen
*     httpbuf      :   http receive buf 
*     return       :   the http headlen.
*   Add by Alex lin  --2014-12-02
*
**********************************************************************/
int32 Http_HeadLen( uint8 *httpbuf )
{
   int8 *p_start = NULL;
   int8 *p_end =NULL;
   int32 headlen=0;
   p_start = (int8 *)httpbuf;
   p_end = (int8 *)strstr( (char *)httpbuf,kCRLFLineEnding);
   if( p_end==NULL )
   {
       Http_Printf(HTTP_DEBUG,"Can't not find the http head!");
       return 0;
   }
   p_end=p_end+strlen(kCRLFLineEnding);
   headlen = (p_end-p_start);
   return headlen;
}
/*********************************************************************
*
*   FUNCTION       :   TO get the http bodylen
*      httpbuf     :   http receive buf 
*      return      :   the http bodylen.(0-error)
*   Add by Alex lin  --2014-12-02
*
**********************************************************************/
int32 Http_BodyLen( uint8 *httpbuf )
{
   int8 *p_start = NULL;
   int8 *p_end =NULL;
   int8 bodyLenbuf[10]={0};
   int32 bodylen=0;  //Content-Length: 
   p_start = (int8 *)strstr( (char *)httpbuf,"Content-Length: ");
   if( p_start==NULL ) return 0;
   p_start = p_start+strlen("Content-Length: ");
   p_end = (int8 *)strstr((char*)p_start,kCRLFNewLine);
   if( p_end==NULL )   return 0;

   memcpy( bodyLenbuf,p_start,(p_end-p_start));
   bodylen = atoi((char*)bodyLenbuf);
   return bodylen;
}

int32 Http_GetHost( int8 *downloadurl,int8 **host,int8 **url )
{
   int8 *p_start = NULL;
   int8 *p_end =NULL; 
   int8 hostlen;

   p_start = (int8 *)strstr((char*)downloadurl,"http://" );
   if(p_start==NULL) 
       return HTTP_FAILED;
   p_start = p_start + strlen("http://");
   p_end = (int8 *)strstr( (char*)p_start,"/");
   if(p_end==NULL)
       return HTTP_FAILED;
   *host = (int8 *)COS_MALLOC((uint32_t)(p_end-p_start+1));
   if( NULL == *host )
   {
       Http_Printf(HTTP_WARNING, "OTA host COS_MALLOC fail!");
       return HTTP_FAILED;
   }
   memcpy(&(*host)[0],p_start,p_end-p_start);
   (*host)[p_end-p_start] = '\0';
   hostlen = p_end-p_start;
   
   p_start = p_end;
   if(p_start==NULL) 
       return HTTP_FAILED;
   p_end = p_start + strlen((char*)downloadurl) - hostlen;
   if(p_end==NULL)
       return HTTP_FAILED;
   *url = (int8 *)COS_MALLOC((uint32_t)(p_end-p_start+1));
   if( NULL == *url )
   {
       Http_Printf(HTTP_WARNING, "OTA url COS_MALLOC fail!");
       return HTTP_FAILED;
   }
   memcpy(&(*url)[0],p_start,p_end-p_start);
   (*url)[p_end-p_start] = '\0';
  
   return HTTP_SUCCESS;
}


/*************************************************************
*
*   FUNCTION  :  get MD5 from http head.
*   httpbuf   :  http buf.
*   MD5       :  MD5 form http head(16b).
*           add by alex.lin ---2014-12-04
*************************************************************/
int32 Http_GetMD5( uint8 *httpbuf,uint8 *MD5,int8 *strMD5)
{
   int8 *p_start = NULL;
   int8 *p_end =NULL;
   int8 MD5_TMP[16];
   uint8 Temp[3]={0};
   int8 *str;
   int32 i=0,j=0;
   p_start = (int8 *)strstr( (char *)httpbuf,"Firmware-MD5: ");
   if(p_start==NULL)
       return 1;
   p_start = p_start+strlen("Firmware-MD5: ");
   p_end = (int8 *)strstr((char *)p_start,kCRLFNewLine);
   if(p_end==NULL)
       return 1;
   if((p_end-p_start)!=32) return 1;
   memcpy( strMD5,p_start,32 );
   strMD5[32] = '\0';
   memcpy( MD5,p_start,16);
   MD5[16]= '\0';

   for(i=0;i<32;i=i+2)
   {
       Temp[0] = strMD5[i];
       Temp[1] = strMD5[i+1];
       Temp[2] = '\0';
       MD5_TMP[j]= strtol((char*)Temp, (char**)&str,16);
       j++;
   }
   memcpy(MD5,MD5_TMP,16);
   Http_Printf(HTTP_INFO," MD5 From HTTP:\n");
   
   for(j=0;j<16;j++)
       Http_Printf(HTTP_DUMP,"%02x",MD5[j]);
   
   return 16;
}

