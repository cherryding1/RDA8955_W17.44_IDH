/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cupnpav.h
*
*	Revision:
*       2008/06/16
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_CUPNPAV_H_
#define _CG_CLINKCAV_CUPNPAV_H_

#include "chttp.h"
#include "cupnp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CG_UPNPAV_OBJECT_ID "id"
#define CG_UPNPAV_OBJECT_PARENTID "parentID"
#define CG_UPNPAV_OBJECT_CHILDCOUNT "childCount"
#define CG_UPNPAV_OBJECT_RESTRICTED "restricted"
#define CG_UPNPAV_OBJECT_TITLE "dc:title"
#define CG_UPNPAV_OBJECT_DATE "dc:date"
#define CG_UPNPAV_OBJECT_UPNPCLASS "upnp:class"
#define CG_UPNPAV_OBJECT_ALBUMARTURI "upnp:albumArtURI"
#define CG_UPNPAV_OBJECT_ROOT_ID "0"

typedef BOOL (*CG_UPNPAV_HTTP_LISTENER)(CgHttpRequest *);
typedef BOOL (*CG_UPNPAV_ACTION_LISTNER)(CgUpnpAction *);
typedef BOOL (*CG_UPNPAV_STATEVARIABLE_LISTNER)(CgUpnpStateVariable *);

#ifdef __cplusplus
}
#endif

#include "cdidl.h"
#include "ccontent.h"
#include "cmd5.h"
#include "cresource.h"
#include "cprotocolinfo.h"
#include "cmediaserver.h"
#include "cmediarenderer.h"

#endif
