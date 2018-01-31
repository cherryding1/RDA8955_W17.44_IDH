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

#ifndef __APP_HTTP_H__
#define __APP_HTTP_H__

#ifdef HTTP_STACK_ENABLE

#include <jdi_transportmodule.h>
#endif
#ifdef HTTP_LOG_ENABLED
#define __MODULE_ID__                   HTTP_MODULE
#else
#define __MODULE_ID__                   0
#endif
#ifdef HTTP_STACK_ENABLE
#include <jcalnet.h>
#include <jdi_stackheader.h>

#include <http_info.h>
#endif


//#include "http_lunch_api.h"
#include "http_api.h"

#ifdef HTTP_STACK_ENABLE
#ifdef HTTP_WITH_SSL
#if defined SSL_4_5
#include <ssl_interface.h>
#endif
#endif



/** @endcond */

#ifdef HTTP_WITH_SSL
#ifdef JCF_SSL

/**
 * @brief   Specifies the cipher suites used incase of secure conection for handshake operation
 */
static const E_SSL_CIPHER_SUITE aeCipherSuite[25] =
{
    E_SSL_CS_RSA_WITH_RC4_128_SHA, /**< RSA with RC4 128 bit SHA algorithm. */
    E_SSL_CS_RSA_WITH_AES_256_CBC_SHA, /**< RSA with AES 256 bit CBC SHA algorithm. */
    E_SSL_CS_RSA_WITH_AES_128_CBC_SHA, /**< RSA with AES 128 bit CBC SHA algorithm. */
    E_SSL_CS_RSA_WITH_RC4_128_MD5, /**< RSA with RC4 128 bit MD5 algorithm. */
    E_SSL_CS_RSA_WITH_3DES_EDE_CBC_SHA, /**< RSA with 3DES EDE CBC SHA algorithm. */
    E_SSL_CS_RSA_WITH_DES_CBC_SHA, /**< RSA with AES DES CBC SHA algorithm. */
    E_SSL_CS_RSA_EXPORT_WITH_RC4_40_MD5, /**< RSA Export with RC4 40 MD5 algorithm. */
    E_SSL_CS_RSA_EXPORT_WITH_DES40_CBC_SHA, /**< RSA EXPORT with DES 40 CBC SHA algorithm. */
    E_SSL_CS_RSA_WITH_NULL_MD5, /**< RSA WITH MD5 algorithm. */

    E_SSL_CS_ECDH_ECDSA_SECT163K1_WITH_RC4_128_SHA, /**< ECDH ECDSA SECT163K1 with RC4 128 bit SHA algorithm. */
    E_SSL_CS_ECDH_ECDSA_SECT163K1_NULL_SHA, /**< ECDH ECDSA SECT163K1 with SHA algorithm. */

    E_SSL_CS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA, /**< DHE DSS EXPORT with DES 40 CBC SHA algorithm. */
    E_SSL_CS_DHE_DSS_WITH_DES_CBC_SHA, /**< DHE DSS with DES CBC SHA algorithm. */
    E_SSL_CS_DHE_DSS_WITH_3DES_EDE_CBC_SHA, /**< DHE DSS with 3DES EDE CBC SHA algorithm. */
    E_SSL_CS_DHE_DSS_EXPORT1024_WITH_DES_CBC_SHA, /**< DHE DSS EXPORT1024 with DES CBC SHA algorithm. */
    E_SSL_CS_DHE_DSS_EXPORT1024_WITH_RC4_56_SHA, /**< DHE DSS EXPORT1024 with RC4 56 bit SHA algorithm. */
    E_SSL_CS_DHE_DSS_WITH_RC4_128_SHA, /**< DHE DSS with RC4 128 SHA algorithm. */
    E_SSL_CS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA, /**< DHE RSA EXPORT with DES 40 CBC SHA algorithm. */
    E_SSL_CS_DHE_RSA_WITH_DES_CBC_SHA, /**< DHE RSA with DES CBC SHA algorithm. */
    E_SSL_CS_DHE_RSA_WITH_3DES_EDE_CBC_SHA, /**< DHE RSA with 3DES EDE CBC SHA algorithm. */

    E_SSL_CS_DH_ANON_EXPORT_WITH_DES40_CBC_SHA, /**< DH Anonymous EXPORT with DES 40 CBC SHA algorithm. */
    E_SSL_CS_DH_ANON_EXPORT_WITH_RC4_40_MD5, /**< DH Anonymous EXPORT with RC4 40 MD5 algorithm. */
    E_SSL_CS_DH_ANON_WITH_RC4_128_MD5, /**< DH Anonymous with RC4 128 bit MD5 algorithm. */
    E_SSL_CS_DH_ANON_WITH_DES_CBC_SHA, /**< DH Anonymous with DES CBC SHA algorithm. */
    E_SSL_CS_DH_ANON_WITH_3DES_EDE_CBC_SHA /**< DH Anonymous with 3DES EDE CBC SHA algorithm. */
} ;
#endif
#endif

/**
 * @brief   This function initializes HTTP module by initializing the HTTP_INFO structure.
 * @param[in] pucServer Specifies the server address to connect incase of proxy connection
 * @param[in] uhPort Specifies the port to connect incase of proxy connection
 * @param[in] vMemHandle Handle used in memory operations
 * @param[in] uiConnID  Specifies the connection ID, which is created using the data connection abstraction layer.
 *                      This ID shall be passed back to the socket layer while creating the new socket.
 * @param[in] iMaxSockets       Maximum number of socket connections that can be used at a time.
 * @param[in] iMaxTcpRecvBuf    Maximum data that can be polled from socket layer at a time.
 * @param[in,out] pvHttpHandle Stores the initialized structure HTT_INFO
 * @retval      JC_OK   Successfully initialized the HTTP module.
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @see         jdi_WPHTTPDeInitialize
 * @note        The parameter pvHttpHandle should not be NULL.
 * @note        The parameter pvHttpHandle should be passed to all
 *              other jdi functions in HTTP module.
 *
 * This function is called by transporttask module when it gets
 * some request from the application.
 */
JC_RETCODE app_WPHTTPInitialize (const JC_UINT8     *pucServer,
                                 JC_UINT16          uhPort,
                                 JC_HANDLE          vMemHandle,
                                 JC_UINT32          uiConnID,
                                 JC_INT32           iMaxSockets,
                                 JC_UINT32          iMaxTcpRecvBuf,
                                 JC_HANDLE          *pvHttpHandle);

/**
 * @brief This function sets the port number to be used for socket connection for secure requests.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   uhSecurePort    Port number to be used for tunneling process
 * @return      No Return value
 *
 * Using this function application can specify the alternative port number which should be used
 * for socket connection instead of the normal proxy port number for secure requests. This is
 * application only for the proxy connection.
 */
void app_WPHTTPSetSecurePortNumber (JC_HANDLE   *vHttpHandle,
                                    JC_UINT16   uhSecurePort);


/**
 * @brief   This function creates a transaction with the given request details.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   pstHttpRequest  Contains information about the request
 * @retval      JC_ERR_WPHTTP_INVALID_URL The given URL is invalid
 * @retval      JC_ERR_WPHTTP_INVALID_PORT_NUMBER Port number in the given URL is invalid
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @see         jdi_WPHTTPDeleteTransaction
 * @note        The parameters vHttpHandle and pstHttpRequest should not be NULL.
 *
 * This function is called by transporttask to create a transaction,
 * when ever it gets a request from the application.
 */

JC_RETCODE app_WPHTTPCreateTransaction (JC_HANDLE               vHttpHandle,
                                        const ST_HTTP_REQUEST   *pstHttpRequest);



/**
 * @brief   This function deletes a trasaction with the given request id.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   uiReqId     Specifies the request id of a transaction
 * @retval      JC_ERR_INVALID_PARAMETER The given request id is invalid
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error
 * @retval      JC_OK On success.
 * @retval      Others.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask to delete a transaction,
 * when that request is completely processed.
 */

JC_RETCODE app_WPHTTPDeleteTransaction (JC_HANDLE   vHttpHandle,
                                        JC_UINT32   uiReqId);




/**
 * @brief   This function establishes a socket connection and sends the request to the server.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[out]  puiReqId        Holds the Request ID for which userdata is needed
 * @retval      JC_ERR_NET_WOULDBLOCK A non blocking socket operation is going on
 * @retval      JC_ERR_WPHTTP_SENDING_USERDATA Userdata is being sent to the server
 * @retval      JC_ERR_WPHTTP_TUNNEL_RESPONSE Response for the tunnel request
 *              has to be received from the server
 * @retval      JC_ERR_WPHTTP_HANDSHAKE Handshake has to be done with the server
 * @retval      JC_ERR_WPHTTP_HANDSHAKE_SUCCESSFUL Handshake process is successful
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error
 * @retval      JC_OK On success.
 * @retval      Others.
 * @see         jdi_WPHTTPDataPoll
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask when a request has to be sent to the server
 */

JC_RETCODE app_WPHTTPSendData (JC_HANDLE    vHttpHandle,
                               JC_UINT32    *puiReqId);




/**
 * @brief   This function sends the given user data to the server.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   pucBuffer   Contains the user data
 * @param[in]   uiBufferLen Specifies length of the user data
 * @param[in]   uiReqId     Request id for which the user data has to be sent
 * @retval      JC_ERR_INVALID_PARAMETER The given request id is invalid
 * @retval      JC_ERR_NET_WOULDBLOCK A non blocking socket operation is going on
 * @retval      JC_ERR_WPHTTP_SENDING_USERDATA Userdata is being sent to the server
 * @retval      JC_ERR_WPHTTP_MORE_DATA_TO_SEND Some more data has to be sent to the server
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error
 * @retval      JC_OK On success.
 * @retval      Others.
 * @see         jdi_CacheInvalidateObject
 * @note        The parameters vHttpHandle and pucBuffer should not be NULL.
 *
 * This function is called by transporttask when it gets user data for a
 * request from the application which has to be sent to the server.
 */
JC_RETCODE app_WPHTTPSendMoreData (JC_HANDLE    vHttpHandle,
                                   JC_UINT8     *pucBuffer,
                                   JC_UINT32    uiBufferLen,
                                   JC_UINT32    uiReqId);




/**
 * @brief   This function receives response from the server and processes it.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[out]  pstHttpResponse Structure containing the processed response
 * @retval      JC_ERR_WPHTTP_RECV_CONTINUE Some more data has to be received
 *              for the current request from the server
 * @retval      JC_ERR_HTTPDEC_PARTIAL_CONTENT The response received from
 *              the server is partial.
 * @retval      JC_ERR_HTTPDEC_PARTIAL_HEADER The header data received from
 *              the server is partial.
 * @retval      JC_ERR_WPHTTP_RECV_CONTINUE_NEXT_BUFFER The current request has
 *              received complete response and the next request has to start
 *              receiving response from the server.
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @retval      Others.
 * @note        The parameters vHttpHandle and pstHttpResponse should not be NULL.
 *
 * This function is called by transporttask when it has to receive response
 * for a request. This function will be called once the function jdi_WPHTTPSendData
 * is completed successfully.
 */
JC_RETCODE app_WPHTTPDataPoll (JC_HANDLE        vHttpHandle,
                               ST_HTTP_RESPONSE *pstHttpResponse);




/**
 * @brief   This function closes all the socket connections
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask when it gets a user cancel request
 * from the application.
 */

JC_RETCODE app_WPHTTPCloseConnection (JC_HANDLE     vHttpHandle);


/**
 * @brief   This function gets the state of a request.
 * @param[in]   vHttpHandle         Pointer to the HTTP_INFO structure
 * @param[in]   uiReqId             Identification of a request
 * @param[out]  puiSentBytes        No of bytes sent to  the server
 * @param[out]  puiReceivedBytes    No of bytes received from the server
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask when it has to send the status
 * of a request to the application.
 */

JC_RETCODE app_WPHTTPGetTransactionStatus (JC_HANDLE    vHttpHandle,
        JC_UINT32    uiReqId,
        JC_UINT32    *puiSentBytes,
        JC_UINT32    *puiReceivedBytes);




/**
 * @brief   This function adds the digest authorization information to a request.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   bIsProxy    Specifies whether it is a proxy server authorization
 * @param[in]   pstDigestAuth Structure containing information about digest authorization
 * @param[in]   pucUrl Specifies the URL for which digest authorization is required
 * @param[in]   uiReqId Identification of the request for which
 *              digest authorization has to be added
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @retval      Others.
 * @note        The parameters vHttpHandle and pstDigestAuth should not be NULL.
 *
 * This function is called by transporttask when it gets a request from the application
 * which requires digest authentication.
 */

JC_RETCODE app_WPHTTPAddDigestAuthorization (JC_HANDLE                          vHttpHandle,
        JC_BOOLEAN                         bIsProxy,
        const ST_HTTP_DIGEST_AUTHORIZATION *pstDigestAuth,
        const JC_UINT8                     *pucUrl,
        JC_UINT32                          uiReqId);



/**
 * @brief   This function adds the basic authorization information to a request.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   bIsProxy    Specifies whether it is a proxy server authorization
 * @param[in]   pucName     Specifies the username
 * @param[in]   pucPassword Specifies the password
 * @param[in]   uiReqId Identification of the request for which
 *              basic authorization has to be added
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @retval      Others.
 * @note        The parameters vHttpHandle and pucName should not be NULL.
 *
 * This function is called by transporttask when it gets a request from the application
 * which requires basic authentication.
 */

JC_RETCODE app_WPHTTPAddAuthorization (JC_HANDLE        vHttpHandle,
                                       JC_BOOLEAN       bIsProxy,
                                       const JC_UINT8   *pucName,
                                       const JC_UINT8   *pucPassword,
                                       JC_UINT32        uiReqId);




/**
 * @brief   This function adds the binding information to the HTTP_INFO structure.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   pcBindAddress   Specifies the IP address to which the
 *              opened sokcets has to be bound
 * @param[in]   uhBindPort      Specifes the biding port
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @retval      Others.
 * @note        The parameters vHttpHandle and pcBindAddress should not be NULL.
 *
 * This function is called by transporttask if the sockets opened in HTTP
 * should be bound to a particular IP address.
 */

JC_RETCODE app_WPHTTPAddBindInfo (JC_HANDLE         vHttpHandle,
                                  const JC_INT8     *pcBindAddress,
                                  JC_UINT16         uhBindPort);


/**
 * @brief   This function sets the IP address for a given host name for making a connection.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   pucHostName Specifies the host name with which connection has to be made
 * @param[in]   pucAddress  Specifies IP address used for making a connection
 * @param[out]  puiReqID Specifies the Request id of the current request
 * @retval      JC_ERR_FILE_READ If fails during reading from cache file.
 * @retval      JC_ERR_FILE_WRITE If fails during writting to cache file.
 * @retval      JC_ERR_CACHE_HEADER_NOT_EXIST If header not found in cache file.
 * @retval      JC_ERR_CACHE_DOMAIN_MISMATCH If domain mismatch occurs.
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @retval      Others.
 * @see         jdi_CacheInvalidateObject
 * @note        None of the parameters should be NULL.
 *
 * This function is called by transporttask when it gets a DNS event.
 */
JC_RETCODE app_WPHTTPSetHostName (JC_HANDLE         vHttpHandle,
                                  const JC_UINT8    *pucHostName,
                                  JC_UINT8          *pucAddress,
                                  JC_UINT32         *puiReqID);


/**
 * @brief   This function sets the state of a socket connection.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   iSocketId   Specifies the socket id
 * @param[in]   eHttpState  HTTP state
 * @param[out]  puiRequestID    Holds the request id which is completed
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and puiRequestID should not be NULL.
 *
 * This function is called by transporttask when it gets a socket event from the application
 */

JC_RETCODE app_WPHTTPSetConnectionState (JC_HANDLE      vHttpHandle,
        JC_INT32       iSocketId,
        E_HTTP_STATE   eHttpState,
        JC_UINT32      *puiRequestID);




/**
 * @brief   This function deinitializes the HTTP module.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask when it gets a session destroy
 * event from the application.
 */
JC_RETCODE app_WPHTTPDeInitialize (JC_HANDLE    vHttpHandle);




/**
 * @brief   This function sets the timeout for connect, send and receive operations.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   eTimeOutType    Specifies the timeout type
 * @param[in]   uiTimeOut       Timeout value in seconds
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask to set the timeout value for connect,
 * send and receive operations.
 */

JC_RETCODE app_WPHTTPSetTimeOut (JC_HANDLE      vHttpHandle,
                                 E_TIMEOUT_TYPE eTimeOutType,
                                 JC_UINT32      uiTimeOut);




/**
 * @brief   This function sets the timeout for select operations.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   eTimeOutType    Specifies the timeout type
 * @param[in]   uiTimeout       Timeout value in seconds
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask to set the timeout value for select
 * operation for connect, send and receive operations.
 */

JC_RETCODE app_WPHTTPSetSocketSelectTimeout (JC_HANDLE              vHttpHandle,
        E_SELECT_TIMEOUT_TYPE  eTimeOutType,
        JC_UINT32              uiTimeout);



/**
 * @brief   This function checks the validity of an HTTP Handle for the given socket.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   iSocket Specifies the URL Domain which need to be invalidated.
 * @retval      E_TRUE  If the given socket belongs to the given HTTP handle
 * @retval      E_TRUE  If the given socket does not belong to the given HTTP handle
 * @see         jdi_CacheInvalidateObject
 * @note        The parameter vHttpHandleshould not be NULL.
 *
 * This function is called by transporttask to check the validity of a handle
 * for a given socket.
 */

JC_BOOLEAN app_WPHTTPIsHandleForSocket (JC_HANDLE   vHttpHandle,
                                        JC_INT32    iSocket);




#ifdef HTTP_WITH_SSL
/* Security Functions */

/**
 * @brief   This function adds the list of cipher suites to HTTP_INFO structure,
 *          which will be used during handshake process.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   peCipherSuite   List of cipher suites
 * @param[in]   uiCipherSuiteCount Specifies the count of cipher suites
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and phCipherSuite should not be NULL.
 *
 * This function is called by transporttask to set the list of cipher suites
 * in HTTP, which will be used in handshake process.
 */

JC_RETCODE app_WPHTTPSetCipherSuites (JC_HANDLE                 vHttpHandle,
                                      const E_SSL_CIPHER_SUITE  *peCipherSuite,
                                      JC_UINT32                 uiCipherSuiteCount);

/**
 * @brief   This function adds the user certificate information to HTTP_INFO structure,
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   pstUserCertInfo Contains the user certificate information
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and pstUserCertInfo should not be NULL.
 *
 * This function is called by transporttask to set user certificate information
 * in HTTP, which will be used in handshake process.
 */

JC_RETCODE app_WPHTTPAddUserCertificate (JC_HANDLE                          vHttpHandle,
        const ST_SSL_USER_CERTIFICATE_INFO *pstUserCertInfo);



/**
 * @brief   This function adds the root certificate information to HTTP_INFO structure,
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   pucCert     Root certificate information
 * @param[in]   uiCertLen   Length of the certificate data
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and pucCert should not be NULL and
 *              uiCertLen should not be zero.
 *
 * This function is called by transporttask to set root certificate information
 * in HTTP, which will be used in handshake process.
 */

JC_RETCODE app_WPHTTPAddRootCertificate (JC_HANDLE      vHttpHandle,
        const JC_UINT8 *pucCert,
        JC_UINT32      uiCertLen);




/**
 * @brief   This function gets the cipher suite negotiated by the server
 *          during handshake process.
 * @param[in]   vHttpHandle             Pointer to the HTTP_INFO structure
 * @param[in]   peNegotiatedCipherSuite Holds the negotiated cipher suite
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and peNegotiatedCipherSuite should not be NULL.
 *
 * This function is called by transporttask when it has to send the details
 * about the handshake process to the application.
 */

JC_RETCODE app_WPHTTPGetNegotiatedCipherSuites (JC_HANDLE               vHttpHandle,
        E_SSL_CIPHER_SUITE      *peNegotiatedCipherSuite);




/**
 * @brief   This function registers a callback function to get the cartificate information.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   cbCertCallBack  Specifies the callback
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and cbCertCallBack should not be NULL.
 *
 * This function is called by transporttask to register for the certificate information
 */

JC_RETCODE app_WPHTTPRegisterCertificateCallback (JC_HANDLE             vHttpHandle,
        CB_SSL_CERT_CALLBACK  cbCertCallBack);



/**
 * @brief   This function registers a callback function to get intimation of secure session
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   cbSecureSessionCallback Specifies the callback
 * @param[in]   pvAppArg    Specifies the application argument that should be passed when
 *              the callback is called
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and cbCertCallBack should not be NULL.
 *
 * This function is called by transporttask to register for the secure session intimation.
 */

JC_RETCODE app_WPHTTPRegisterSecureSessionIndicationCallback (JC_HANDLE                     vHttpHandle,
        HTTP_SECURE_SESSION_CALLBACK  cbSecureSessionCallback,
        JC_HANDLE                     pvAppArg);

#endif



/**
 * @brief   This function checks all the sockets and closes those sockets that are timedout.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   bForceful       Specifies if the empty socketpools have
 *                              to be closed forcefully.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle should not be NULL.
 *
 * This function is called by transporttask to clean up the timedout sockets
 * before creating a transaction in HTTP. If bForceful is E_TRUE then all the
 * empty socketpools will be closed forcefully.
 */

JC_RETCODE app_WPHTTPSocketPoolCleanUp (JC_HANDLE   vHttpHandle,
                                        JC_BOOLEAN  bForceful);



/**
 * @brief   This function adds a header to the HTTP_INFO structure which
 *          will be added to all the requests.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   pstMimeHeaders  List of headers
 * @param[in]   bIsAccept   Specifies if the header is an accept contenttype header
 * @param[in]   eHttpRequestType    Specifies the type of request
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and pstMimeHeaders should not be NULL.
 *
 * This function is called by transporttask to add different headers after
 * initializing the HTTP module.
 */

JC_RETCODE app_WPHTTPAddStaticHeader (JC_HANDLE             vHttpHandle,
                                      ST_MIME_HEADERS       *pstMimeHeaders,
                                      JC_BOOLEAN            bIsAccept,
                                      E_HTTP_REQUEST_TYPE   eHttpRequestType);


/**
 * @brief   This function adds a header to a particular request.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   uiRequestId     Reqest id for which the given headers have to be added
 * @param[in]   pstMimeHeaders  List of headers
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle and pstMimeHeaders should not be NULL.
 *
 * This function is called by transporttask when it gets header information
 * for a request from the application.
 */

JC_RETCODE app_WPHTTPAddDynamicHeader (JC_HANDLE        vHttpHandle,
                                       JC_UINT32        uiRequestId,
                                       ST_MIME_HEADERS  *pstMimeHeaders);


/**
 * @brief   This function reads data from the given socket.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   iSocketId   Socket id from which data has to be read
 * @param[in]   pucReadBuffer   Specifies the data read from the socket (if available).
 * @param[in]   uiDataLen   No of bytes read, if the pucBuffer is not NULL.
 *                          If pucBuffer is NULL, this specifies the number of bytes that can be read from the socket.
 * @retval      JC_ERR_NET_WOULDBLOCK A non blocking socket operation is going on
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameter vHttpHandle should not be NULL.
 *
 * This function is called by transporttask when it gets socket read event
 * from the application.
 */

JC_RETCODE app_WPHTTPReadData (JC_HANDLE            vHttpHandle,
                               JC_INT32             iSocketId,
                               const JC_UINT8       *pucReadBuffer,
                               JC_UINT32            uiDataLen);


/**
 * @brief   This function checks the data queue in a socket pool if data is
 *          available for a given request or not.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   uiReqId     request for which data availability has to be checked
 * @retval      E_TRUE  Data is available for the given request
 * @retval      E_FALSE Data is not available for the given request
 * @note        The parameter vHttpHandleshould not be NULL.
 *
 * This function is called by transporttask to check for data for a particular
 * request when the bearer connection is down.
 */
JC_BOOLEAN app_WPHTTPIsDataAvailable (JC_HANDLE     vHttpHandle,
                                      JC_UINT32     uiReqId);


/**
 * @brief   This function sets the offline mode in HTTP.
 * @param[in]   vHttpHandle Pointer to the HTTP_INFO structure
 * @param[in]   bIsOffLine  If it is true, it specifies that the data has to be
 *              read from the socket pool data queue only
 * @retval      JC_OK   Success code
 * @note        The parameter vHttpHandle should not be NULL.
 *
 * This function is called by transporttask to set offline mode in HTTP
 * so that when bearer connection is down data will be read only from the
 * socket pool data queue and sockets will not be polled for data.
 */
JC_RETCODE app_WPHTTPSetOffLineStatus (JC_HANDLE        vHttpHandle,
                                       JC_BOOLEAN       bIsOffLine);


/**
 * @brief This function sets the max no of sockets that can be opened by http stack.
 * @param[in]   vHttpHandle     Handle that holds the HTTP_INFO structure.
 * @param[in]   uiMaxSockets    Specifies the max no of sockets.
 * @return The status of setting the max no of sockets value.
 * @retval JC_OK    The process is successful.
 * @retval Others   Error in the process.
 * @note    The parameter vHttpHandle should not be NULL and uiMaxSockets should not be zero.
 *
 * This function is called by transporttask to set the max no of socket that
 * can be opened by HTTP Stack at any time to process the requests.
 */

JC_RETCODE app_WPHTTPSetMaxSockets (JC_HANDLE       vHttpHandle,
                                    JC_UINT32       uiMaxSockets);


/**
 * @brief This function gets the max no of requests that can be processed by
 *        http stack currently.
 * @param[in]   vHttpHandle     Handle that holds the HTTP_INFO structure.
 * @param[in]   puiMaxRequests  Holds the max request count.
 * @note        The parameters vHttpHandle and puiMaxRequests should not be NULL.
 * @retval JC_OK    Success code
 *
 * This function is called by transporttask to get the max no of requests that
 * can be processed by HTTP Stack currently.
 */
JC_RETCODE app_WPHTTPGetMaxRequestCount (JC_HANDLE      vHttpHandle,
        JC_UINT32      *puiMaxRequests);


/**
 * @brief This function sets the HTTP protocol version type.
 * @param[in]   vHttpHandle     Handle that holds the HTTP_INFO structure.
 * @param[in]   eVersion        Specifies the HTTP Protocol Version type.
 * @return No Return value
 *
 * This function is called by transporttask to set the protocol version. This value
 * will be used while sending the requests to the server.
 */
void app_WPHTTPSetProtocolVersion (JC_HANDLE                vHttpHandle,
                                   E_HTTP_VERSION_TYPE      eVersion);


#ifdef HTTP_DECODER_ENABLE
/**
 * @brief   This function parses the given header data and forms the header list.
 * @param[in]   vHttpHandle     Pointer to the HTTP_INFO structure
 * @param[in]   pucHeader       Header data that has to be parsed
 * @param[in]   uiHeaderLength  Length of header data
 * @param[in]   ppstMimeHeaders Holds the list of headers that are formed
 * @retval      JC_ERR_MEMORY_ALLOCATION On memory allocation error.
 * @retval      JC_OK On success.
 * @note        The parameters vHttpHandle, pucHeader and ppstMimeHeaders should not
 *              be NULL and uiHeaderlength should not be zero.
 *
 * This function is called by transporttask when it wants to parse the given
 * header data to form the header list.
 */

JC_RETCODE app_WPHTTPParseHeaders (JC_HANDLE        vHttpHandle,
                                   JC_UINT8         *pucHeader,
                                   JC_UINT32        uiHeaderLength,
                                   ST_MIME_HEADERS  **ppstMimeHeaders);

#endif
/** @cond */
#endif /*HTTP_STACK_ENABLE*/

#ifdef AT_HTTP_SUPPORT

BOOL app_WPHTTPGet(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPPost(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPPut(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPHead(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPOptions(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPTrace(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPDelete(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPDownLoad(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL app_WPHTTPAuthorization(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);

#endif

#endif
/** @endcond */
/* End of File */

