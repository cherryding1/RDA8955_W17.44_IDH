/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdevice_advertiser.c
*
*	Revision:
*
*	06/14/05
*		- first revision
*
*	10/31/05
*		- Added comments
*
*	03/20/06 Theo Beisch
*		- added NULL check on advertiser_stop
*
******************************************************************/

#include "cdevice.h"
#include "ctime.h"
#include "clog.h"
#include "event.h"

/**
 * cg_upnp_device_advertiser_action
 * 
 * Function run as a UPnP device advertisement
 * thread.
 *
 * \param thread The thread context that this function is running in
 */
static void cg_upnp_device_advertiser_action(CgThread *thread) 
{
	CgUpnpDevice *dev;
	CgSysTime leaseTime;
	CgSysTime notifyInterval = 1500; //ms
  
	cg_log_debug_l4("Entering...\n");

	/**
	* Get the device struct that this function is advertising.
	*/
	dev = (CgUpnpDevice *)cg_thread_getuserdata(thread);

	/* Sleep *notifyInterval* msecs */
	cg_wait(notifyInterval); 

	/* Check if we must exit before sending new announce */
	if ( cg_thread_isrunnable(thread) == FALSE )
		 return;

	/* Initiate advertise routine after sleeping */
	cg_upnp_device_announce(dev);

	{
		COS_EVENT ev;
		ev.nEventId = EV_SSDP_ADVERTISE_LOOP_REQ;
		ev.nParam1 = thread;
		COS_SendEvent(MOD_SSDP_ADVERTISE, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}

	cg_log_debug_l4("Leaving...\n");
}

/**
 * cg_upnp_device_advertiser_start
 *
 * Start the advertiser thread for the given device
 *
 * \param dev The device that is to be advertised
 */
BOOL cg_upnp_device_advertiser_start(CgUpnpDevice *dev)
{
	CgThread *advertiser;

	cg_log_debug_l4("Entering...\n");

	/* Get the already existing (created in device_new) advertiser thread */
	advertiser = cg_upnp_device_getadvertiserthead(dev);

	/* Store the device into the advertiser thread struct's user data */
	cg_thread_setuserdata(advertiser, dev);

	/* Set the function that will run the thread */
	cg_thread_setaction(advertiser, cg_upnp_device_advertiser_action);

	/* Start the thread */
	cg_thread_start(advertiser);

	{
		COS_EVENT ev;
		ev.nEventId = EV_SSDP_ADVERTISE_START_REQ;
		ev.nParam1 = (UINT32)advertiser;
		COS_SendEvent(MOD_SSDP_ADVERTISE, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);		
	}

	cg_log_debug_l4("Leaving...\n");
	return TRUE;
}

/**
 * cg_upnp_device_advertiser_stop
 *
 * Stop the advertiser thread for the given device
 *
 * \param dev Device that is being advertised
 */
BOOL cg_upnp_device_advertiser_stop(CgUpnpDevice *dev)
{
	CgThread *advertiser;
  
	cg_log_debug_l4("Entering...\n");

	/* Get the advertiser thread from the device struct */
	advertiser = cg_upnp_device_getadvertiserthead(dev);
  
	//Theo Beisch 
	if (advertiser != NULL)
 		return cg_thread_stop(advertiser);
	return FALSE;
}

/**
 * cg_upnp_device_advertiser_isrunning
 *
 * Check if the advertiser has been started
 *
 * \param dev Device that is being advertised
 */
BOOL cg_upnp_device_advertiser_isrunning(CgUpnpDevice *dev)
{
	CgThread *advertiser;
	
	cg_log_debug_l4("Entering...\n");
	
	advertiser = cg_upnp_device_getadvertiserthead(dev);
	if (advertiser != NULL)
 		return cg_thread_isrunnable(advertiser);

	return FALSE;
}

void BAL_SSDP_AdvertiseTask(void* pData)
{
    COS_EVENT ev;
	CgThread *pThread = NULL;
	CgUpnpDevice *dev;
	CgHttpServer *httpServer = NULL;

	for(;;)
	{
		COS_WaitEvent(MOD_SSDP_ADVERTISE, &ev, COS_WAIT_FOREVER);
		switch(ev.nEventId)
		{
		case EV_SSDP_ADVERTISE_START_REQ:
		case EV_SSDP_ADVERTISE_LOOP_REQ:
			{
				hal_HstSendEvent(SYS_EVENT, 0x08070061);
				if(pThread == NULL)
				{
					pThread = (CgThread*)ev.nParam1;
				}

				if(ev.nEventId == EV_SSDP_ADVERTISE_START_REQ)
					cg_thread_start(pThread);
				cg_upnp_device_advertiser_action(pThread);
			}
			break;
		case EV_SSDP_ADVERTISE_STOP_REQ:
			{
				hal_HstSendEvent(SYS_EVENT, 0x09080001);
				if(pThread)
				{
					cg_thread_stop(pThread);
				}
			}
			break;
		case EV_HTTP_CLIENT_THREAD_EXIT_IND:
			{
				httpServer = ev.nParam2;
				cg_socket_delete((CgSocket*)ev.nParam1);
				if(pThread)
				{
					cg_thread_stop(pThread);
				}
			}
			break;
		case EV_HTTP_PLAY_START_REQ:
			{
				hal_HstSendEvent(SYS_EVENT, 0x09020023);
				if(httpServer)
				{
					//COS_EVENT ev2;
					httpServer->playing = TRUE;
					WRB_SendPlayUrlREQ((INT8*)ev.nParam1, ev.nParam2);
					//ev2.nEventId = EV_HTTP_PLAY_START_REQ;
					//ev2.nParam1 = ev.nParam1;
					//COS_SendEvent(MOD_HTTP_SERVER, &ev2, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
				}
			}
			break;
		default:
			break;
		}
	}
		
}

