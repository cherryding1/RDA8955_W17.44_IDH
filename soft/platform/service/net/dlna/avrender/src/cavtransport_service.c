/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cavtransport_service.c
*
*	Revision:
*		2009/06/11
*        - first release.
*
************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "dlna_types.h"
#include "cmediarenderer.h"
#include "clog.h"
#include "event.h"

#define printf(format, ...) 

/****************************************
* Service Description (AVTransport)
****************************************/

static char *CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_DESCRIPTION =
#if defined(CG_CLINKCAV_USE_UPNPSTD_XML)
"<?xml version=\"1.0\"encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"  <specVersion>\n"
"     <major>1</major>\n"
"     <minor>0</minor>\n"
"  </specVersion>\n"
"  <actionList>\n"
"     <action>\n"
"        <name>GetCurrentTransportActions</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>Actions</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentTransportActions</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>GetDeviceCapabilities</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>PlayMedia</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>PossiblePlaybackStorageMedia</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>RecMedia</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>PossibleRecordStorageMedia</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>RecQualityModes</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>PossibleRecordQualityModes</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>GetMediaInfo</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>NrTracks</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>NumberOfTracks</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>MediaDuration</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentMediaDuration</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentURI</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>AVTransportURI</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentURIMetaData</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>NextURI</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>NextAVTransportURI</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>NextURIMetaData</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>PlayMedium</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>PlaybackStorageMedium</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>RecordMedium</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>RecordStorageMedium</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>WriteStatus</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>RecordMediumWriteStatus</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>GetPositionInfo</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>Track</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentTrack</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>TrackDuration</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentTrackDuration</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>TrackMetaData</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentTrackMetaData</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>TrackURI</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentTrackURI</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>RelTime</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>RelativeTimePosition</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>AbsTime</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>AbsoluteTimePosition</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>RelCount</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>RelativeCounterPosition</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>AbsCount</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>AbsoluteCounterPosition</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>GetTransportInfo</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentTransportState</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>TransportState</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentTransportStatus</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>TransportStatus</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentSpeed</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>GetTransportSettings</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>PlayMode</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentPlayMode</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>RecQualityMode</name>\n"
"              <direction>out</direction>\n"
"              <relatedStateVariable>CurrentRecordQualityMode</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>Next</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>Pause</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>Play</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>Speed</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>Previous</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>Seek</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>Unit</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_SeekMode</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>Target</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_SeekTarget</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>SetAVTransportURI</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentURI</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>AVTransportURI</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>CurrentURIMetaData</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>SetNextAVTransportURI</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>NextURI</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>NextAVTransportURI</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>NextURIMetaData</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>SetPlayMode</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"           <argument>\n"
"              <name>NewPlayMode</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>CurrentPlayMode</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"     <action>\n"
"        <name>Stop</name>\n"
"        <argumentList>\n"
"           <argument>\n"
"              <name>InstanceID</name>\n"
"              <direction>in</direction>\n"
"              <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"           </argument>\n"
"        </argumentList>\n"
"     </action>\n"
"  </actionList>\n"
"  <serviceStateTable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentPlayMode</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>NORMAL</allowedValue>\n"
"           <allowedValue>REPEAT_ALL</allowedValue>\n"
"           <allowedValue>SHUFFLE</allowedValue>\n"
"        </allowedValueList>\n"
"        <defaultValue>NORMAL</defaultValue>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>RecordStorageMedium</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>UNKNOWN</allowedValue>\n"
"           <allowedValue>DV</allowedValue>\n"
"           <allowedValue>MINI-DV</allowedValue>\n"
"           <allowedValue>VHS</allowedValue>\n"
"           <allowedValue>W-VHS</allowedValue>\n"
"           <allowedValue>S-VHS</allowedValue>\n"
"           <allowedValue>D-VHS</allowedValue>\n"
"           <allowedValue>VHSC</allowedValue>\n"
"           <allowedValue>VIDEO8</allowedValue>\n"
"           <allowedValue>HI8</allowedValue>\n"
"           <allowedValue>CD-ROM</allowedValue>\n"
"           <allowedValue>CD-DA</allowedValue>\n"
"           <allowedValue>CD-R</allowedValue>\n"
"           <allowedValue>CD-RW</allowedValue>\n"
"           <allowedValue>VIDEO-CD</allowedValue>\n"
"           <allowedValue>SACD</allowedValue>\n"
"           <allowedValue>MD-AUDIO</allowedValue>\n"
"           <allowedValue>MD-PICTURE</allowedValue>\n"
"           <allowedValue>DVD-ROM</allowedValue>\n"
"           <allowedValue>DVD-VIDEO</allowedValue>\n"
"           <allowedValue>DVD-R</allowedValue>\n"
"           <allowedValue>DVD+RW</allowedValue>\n"
"           <allowedValue>DVD-RW</allowedValue>\n"
"           <allowedValue>DVD-RAM</allowedValue>\n"
"           <allowedValue>DVD-AUDIO</allowedValue>\n"
"           <allowedValue>DAT</allowedValue>\n"
"           <allowedValue>LD</allowedValue>\n"
"           <allowedValue>HDD</allowedValue>\n"
"           <allowedValue>MICRO-MV</allowedValue>\n"
"           <allowedValue>NETWORK</allowedValue>\n"
"           <allowedValue>NONE</allowedValue>\n"
"           <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"           <allowedValue>vendor-defined </allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"yes\">\n"
"        <name>LastChange</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>RelativeTimePosition</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentTrackURI</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentTrackDuration</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentRecordQualityMode</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>0:EP</allowedValue>\n"
"           <allowedValue>1:LP</allowedValue>\n"
"           <allowedValue>2:SP</allowedValue>\n"
"           <allowedValue>0:BASIC</allowedValue>\n"
"           <allowedValue>1:MEDIUM</allowedValue>\n"
"           <allowedValue>2:HIGH</allowedValue>\n"
"           <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"           <allowedValue>vendor-defined </allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentMediaDuration</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>AbsoluteCounterPosition</name>\n"
"        <dataType>i4</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>RelativeCounterPosition</name>\n"
"        <dataType>i4</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>A_ARG_TYPE_InstanceID</name>\n"
"        <dataType>ui4</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>AVTransportURI</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>TransportState</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>STOPPED</allowedValue>\n"
"           <allowedValue>PAUSED_PLAYBACK</allowedValue>\n"
"           <allowedValue>PAUSED_RECORDING</allowedValue>\n"
"           <allowedValue>PLAYING</allowedValue>\n"
"           <allowedValue>RECORDING</allowedValue>\n"
"           <allowedValue>TRANSITIONING</allowedValue>\n"
"           <allowedValue>NO_MEDIA_PRESENT</allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentTrackMetaData</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>NextAVTransportURI</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>PossibleRecordQualityModes</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentTrack</name>\n"
"        <dataType>ui4</dataType>\n"
"        <allowedValueRange>\n"
"           <minimum>0</minimum>\n"
"           <maximum>4294967295</maximum>\n"
"           <step>1</step>\n"
"        </allowedValueRange>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>AbsoluteTimePosition</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>NextAVTransportURIMetaData</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>PlaybackStorageMedium</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>UNKNOWN</allowedValue>\n"
"           <allowedValue>DV</allowedValue>\n"
"           <allowedValue>MINI-DV</allowedValue>\n"
"           <allowedValue>VHS</allowedValue>\n"
"           <allowedValue>W-VHS</allowedValue>\n"
"           <allowedValue>S-VHS</allowedValue>\n"
"           <allowedValue>D-VHS</allowedValue>\n"
"           <allowedValue>VHSC</allowedValue>\n"
"           <allowedValue>VIDEO8</allowedValue>\n"
"           <allowedValue>HI8</allowedValue>\n"
"           <allowedValue>CD-ROM</allowedValue>\n"
"           <allowedValue>CD-DA</allowedValue>\n"
"           <allowedValue>CD-R</allowedValue>\n"
"           <allowedValue>CD-RW</allowedValue>\n"
"           <allowedValue>VIDEO-CD</allowedValue>\n"
"           <allowedValue>SACD</allowedValue>\n"
"           <allowedValue>MD-AUDIO</allowedValue>\n"
"           <allowedValue>MD-PICTURE</allowedValue>\n"
"           <allowedValue>DVD-ROM</allowedValue>\n"
"           <allowedValue>DVD-VIDEO</allowedValue>\n"
"           <allowedValue>DVD-R</allowedValue>\n"
"           <allowedValue>DVD+RW</allowedValue>\n"
"           <allowedValue>DVD-RW</allowedValue>\n"
"           <allowedValue>DVD-RAM</allowedValue>\n"
"           <allowedValue>DVD-AUDIO</allowedValue>\n"
"           <allowedValue>DAT</allowedValue>\n"
"           <allowedValue>LD</allowedValue>\n"
"           <allowedValue>HDD</allowedValue>\n"
"           <allowedValue>MICRO-MV</allowedValue>\n"
"           <allowedValue>NETWORK</allowedValue>\n"
"           <allowedValue>NONE</allowedValue>\n"
"           <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"           <allowedValue>vendor-defined </allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>CurrentTransportActions</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>RecordMediumWriteStatus</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>WRITABLE</allowedValue>\n"
"           <allowedValue>PROTECTED</allowedValue>\n"
"           <allowedValue>NOT_WRITABLE</allowedValue>\n"
"           <allowedValue>UNKNOWN</allowedValue>\n"
"           <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>PossiblePlaybackStorageMedia</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>AVTransportURIMetaData</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>NumberOfTracks</name>\n"
"        <dataType>ui4</dataType>\n"
"        <allowedValueRange>\n"
"           <minimum>0</minimum>\n"
"           <maximum>4294967295</maximum>\n"
"        </allowedValueRange>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>A_ARG_TYPE_SeekMode</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>TRACK_NR</allowedValue>\n"
"           <allowedValue>REL_TIME</allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>A_ARG_TYPE_SeekTarget</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>PossibleRecordStorageMedia</name>\n"
"        <dataType>string</dataType>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>TransportStatus</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>OK</allowedValue>\n"
"           <allowedValue>ERROR_OCCURRED</allowedValue>\n"
"           <allowedValue>vendor-defined </allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"     <stateVariable sendEvents=\"no\">\n"
"        <name>TransportPlaySpeed</name>\n"
"        <dataType>string</dataType>\n"
"        <allowedValueList>\n"
"           <allowedValue>1</allowedValue>\n"
"           <allowedValue>vendor-defined </allowedValue>\n"
"        </allowedValueList>\n"
"     </stateVariable>\n"
"  </serviceStateTable>\n"
"</scpd>\n";
#else
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"   <actionList>\n"
"      <action>\n"
"         <name>GetCurrentTransportActions</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Actions</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentTransportActions</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetDeviceCapabilities</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PlayMedia</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>PossiblePlaybackStorageMedia</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RecMedia</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>PossibleRecordStorageMedia</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RecQualityModes</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>PossibleRecordQualityModes</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetMediaInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NrTracks</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>NumberOfTracks</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>MediaDuration</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentMediaDuration</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentURI</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>AVTransportURI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentURIMetaData</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NextURI</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>NextAVTransportURI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NextURIMetaData</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PlayMedium</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>PlaybackStorageMedium</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RecordMedium</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>RecordStorageMedium</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>WriteStatus</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>RecordMediumWriteStatus</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetPositionInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Track</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentTrack</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TrackDuration</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentTrackDuration</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TrackMetaData</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentTrackMetaData</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TrackURI</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentTrackURI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RelTime</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>RelativeTimePosition</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>AbsTime</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>AbsoluteTimePosition</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RelCount</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>RelativeCounterPosition</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>AbsCount</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>AbsoluteCounterPosition</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetTransportInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentTransportState</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>TransportState</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentTransportStatus</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>TransportStatus</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentSpeed</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetTransportSettings</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PlayMode</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentPlayMode</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RecQualityMode</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentRecordQualityMode</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Next</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Pause</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Play</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Speed</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Previous</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Seek</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Unit</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_SeekMode</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Target</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_SeekTarget</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>SetAVTransportURI</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentURI</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>AVTransportURI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentURIMetaData</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>SetNextAVTransportURI</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NextURI</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>NextAVTransportURI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NextURIMetaData</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>SetPlayMode</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NewPlayMode</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>CurrentPlayMode</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Stop</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>InstanceID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"   </actionList>\n"
"   <serviceStateTable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentPlayMode</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>NORMAL</allowedValue>\n"
"            <allowedValue>REPEAT_ALL</allowedValue>\n"
"            <allowedValue>SHUFFLE</allowedValue>\n"
"         </allowedValueList>\n"
"         <defaultValue>NORMAL</defaultValue>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>RecordStorageMedium</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>UNKNOWN</allowedValue>\n"
"            <allowedValue>DV</allowedValue>\n"
"            <allowedValue>MINI-DV</allowedValue>\n"
"            <allowedValue>VHS</allowedValue>\n"
"            <allowedValue>W-VHS</allowedValue>\n"
"            <allowedValue>S-VHS</allowedValue>\n"
"            <allowedValue>D-VHS</allowedValue>\n"
"            <allowedValue>VHSC</allowedValue>\n"
"            <allowedValue>VIDEO8</allowedValue>\n"
"            <allowedValue>HI8</allowedValue>\n"
"            <allowedValue>CD-ROM</allowedValue>\n"
"            <allowedValue>CD-DA</allowedValue>\n"
"            <allowedValue>CD-R</allowedValue>\n"
"            <allowedValue>CD-RW</allowedValue>\n"
"            <allowedValue>VIDEO-CD</allowedValue>\n"
"            <allowedValue>SACD</allowedValue>\n"
"            <allowedValue>MD-AUDIO</allowedValue>\n"
"            <allowedValue>MD-PICTURE</allowedValue>\n"
"            <allowedValue>DVD-ROM</allowedValue>\n"
"            <allowedValue>DVD-VIDEO</allowedValue>\n"
"            <allowedValue>DVD-R</allowedValue>\n"
"            <allowedValue>DVD+RW</allowedValue>\n"
"            <allowedValue>DVD-RW</allowedValue>\n"
"            <allowedValue>DVD-RAM</allowedValue>\n"
"            <allowedValue>DVD-AUDIO</allowedValue>\n"
"            <allowedValue>DAT</allowedValue>\n"
"            <allowedValue>LD</allowedValue>\n"
"            <allowedValue>HDD</allowedValue>\n"
"            <allowedValue>MICRO-MV</allowedValue>\n"
"            <allowedValue>NETWORK</allowedValue>\n"
"            <allowedValue>NONE</allowedValue>\n"
"            <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"            <allowedValue>vendor-defined </allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>LastChange</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>RelativeTimePosition</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentTrackURI</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentTrackDuration</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentRecordQualityMode</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>0:EP</allowedValue>\n"
"            <allowedValue>1:LP</allowedValue>\n"
"            <allowedValue>2:SP</allowedValue>\n"
"            <allowedValue>0:BASIC</allowedValue>\n"
"            <allowedValue>1:MEDIUM</allowedValue>\n"
"            <allowedValue>2:HIGH</allowedValue>\n"
"            <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"            <allowedValue>vendor-defined </allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentMediaDuration</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>AbsoluteCounterPosition</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>RelativeCounterPosition</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_InstanceID</name>\n"
"         <dataType>ui4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>AVTransportURI</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>TransportState</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>STOPPED</allowedValue>\n"
"            <allowedValue>PAUSED_PLAYBACK</allowedValue>\n"
"            <allowedValue>PAUSED_RECORDING</allowedValue>\n"
"            <allowedValue>PLAYING</allowedValue>\n"
"            <allowedValue>RECORDING</allowedValue>\n"
"            <allowedValue>TRANSITIONING</allowedValue>\n"
"            <allowedValue>NO_MEDIA_PRESENT</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentTrackMetaData</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>NextAVTransportURI</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>PossibleRecordQualityModes</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentTrack</name>\n"
"         <dataType>ui4</dataType>\n"
"         <allowedValueRange>\n"
"            <minimum>0</minimum>\n"
"            <maximum>4294967295</maximum>\n"
"            <step>1</step>\n"
"         </allowedValueRange>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>AbsoluteTimePosition</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>NextAVTransportURIMetaData</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>PlaybackStorageMedium</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>UNKNOWN</allowedValue>\n"
"            <allowedValue>DV</allowedValue>\n"
"            <allowedValue>MINI-DV</allowedValue>\n"
"            <allowedValue>VHS</allowedValue>\n"
"            <allowedValue>W-VHS</allowedValue>\n"
"            <allowedValue>S-VHS</allowedValue>\n"
"            <allowedValue>D-VHS</allowedValue>\n"
"            <allowedValue>VHSC</allowedValue>\n"
"            <allowedValue>VIDEO8</allowedValue>\n"
"            <allowedValue>HI8</allowedValue>\n"
"            <allowedValue>CD-ROM</allowedValue>\n"
"            <allowedValue>CD-DA</allowedValue>\n"
"            <allowedValue>CD-R</allowedValue>\n"
"            <allowedValue>CD-RW</allowedValue>\n"
"            <allowedValue>VIDEO-CD</allowedValue>\n"
"            <allowedValue>SACD</allowedValue>\n"
"            <allowedValue>MD-AUDIO</allowedValue>\n"
"            <allowedValue>MD-PICTURE</allowedValue>\n"
"            <allowedValue>DVD-ROM</allowedValue>\n"
"            <allowedValue>DVD-VIDEO</allowedValue>\n"
"            <allowedValue>DVD-R</allowedValue>\n"
"            <allowedValue>DVD+RW</allowedValue>\n"
"            <allowedValue>DVD-RW</allowedValue>\n"
"            <allowedValue>DVD-RAM</allowedValue>\n"
"            <allowedValue>DVD-AUDIO</allowedValue>\n"
"            <allowedValue>DAT</allowedValue>\n"
"            <allowedValue>LD</allowedValue>\n"
"            <allowedValue>HDD</allowedValue>\n"
"            <allowedValue>MICRO-MV</allowedValue>\n"
"            <allowedValue>NETWORK</allowedValue>\n"
"            <allowedValue>NONE</allowedValue>\n"
"            <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"            <allowedValue>vendor-defined </allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>CurrentTransportActions</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>RecordMediumWriteStatus</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>WRITABLE</allowedValue>\n"
"            <allowedValue>PROTECTED</allowedValue>\n"
"            <allowedValue>NOT_WRITABLE</allowedValue>\n"
"            <allowedValue>UNKNOWN</allowedValue>\n"
"            <allowedValue>NOT_IMPLEMENTED</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>PossiblePlaybackStorageMedia</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>AVTransportURIMetaData</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>NumberOfTracks</name>\n"
"         <dataType>ui4</dataType>\n"
"         <allowedValueRange>\n"
"            <minimum>0</minimum>\n"
"            <maximum>4294967295</maximum>\n"
"         </allowedValueRange>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_SeekMode</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>TRACK_NR</allowedValue>\n"
"            <allowedValue>REL_TIME</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_SeekTarget</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>PossibleRecordStorageMedia</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>TransportStatus</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>OK</allowedValue>\n"
"            <allowedValue>ERROR_OCCURRED</allowedValue>\n"
"            <allowedValue>vendor-defined </allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>TransportPlaySpeed</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>1</allowedValue>\n"
"            <allowedValue>vendor-defined </allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"   </serviceStateTable>\n"
"</scpd>\n";
#endif

BOOL cg_upnpav_dmr_avtransport_parseavtransporturl(CgNetURL *url)
{
	char *host;
	int port;
	char *request;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	/*int statusCode;*/
	char *content;
	CgInt64 contentLen;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = cg_net_url_gethost(url);
	port = cg_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	request = cg_net_url_getrequest(url);

	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, request);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, host, port);



	/*statusCode = cg_http_response_getstatuscode(httpRes);
          if (statusCode != CG_HTTP_STATUS_OK) {*/
	if ( !cg_http_response_issuccessful(httpRes)) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	
	cg_http_request_delete(httpReq);
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}


/****************************************
* cg_upnpav_dmr_avtransport_actionreceived
****************************************/
char g_url[256];
char duration[9];
int sec = -2;
unsigned char timerId=0;

void dlna_counter_timeout_cb(void* Handle)
{
	hal_HstSendEvent(SYS_EVENT, 0x09150030);
	sec+=3;
}


BOOL cg_upnpav_dmr_avtransport_actionreceived(CgUpnpAction *action)
{
	CgUpnpAvRenderer *dmr;
	CgUpnpDevice *dev;
	char *actionName;
	CgUpnpArgument *arg;
	char strTime[10] = {0};
	static CgString* strLocation = NULL;

	if(strLocation == NULL)
	{
		strLocation = cg_string_new();
	}

	actionName = cg_upnp_action_getname(action);
	if (cg_strlen(actionName) <= 0)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(cg_upnp_action_getservice(action));
	if (!dev)
		return FALSE;

	dmr = (CgUpnpAvRenderer *)cg_upnp_device_getuserdata(dev);
	if (!dmr)
		return FALSE;

	/* GetCurrentTransportActions*/
	{
		if (cg_streq(actionName, "GetCurrentTransportActions")) {
			arg = cg_upnp_action_getargumentbyname(action, "Actions");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "Play,Stop,Pause,Seek");

			return TRUE;
		}
	}

	/* GetDeviceCapabilities*/
	/* No Need*/
	{
		if (cg_streq(actionName, "GetDeviceCapabilities")) {

			return TRUE;
		}
	}

	/* GetMediaInfo*/
	/* Need it*/
	{
		if (cg_streq(actionName, "GetMediaInfo")) {			
			CgUpnpAction *actionSetAVTransportURI;
			CgUpnpArgument *argTmp;
			arg = cg_upnp_action_getargumentbyname(action, "NrTracks");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "0");
			
			arg = cg_upnp_action_getargumentbyname(action, "MediaDuration");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "");

			arg = cg_upnp_action_getargumentbyname(action, "CurrentURI");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, cg_string_getvalue(strLocation));

			actionSetAVTransportURI = cg_upnp_service_getactionbyname(cg_upnp_action_getservice(action), "SetAVTransportURI");
			argTmp = cg_upnp_action_getargumentbyname(actionSetAVTransportURI, "CurrentURIMetaData");
			if (argTmp){
				arg = cg_upnp_action_getargumentbyname(action, "CurrentURIMetaData");
				if (!arg)
					return FALSE;
				cg_upnp_argument_setvalue(arg, argTmp);
			}

			arg = cg_upnp_action_getargumentbyname(action, "NextURI");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "NOT_IMPLEMENTED");

			arg = cg_upnp_action_getargumentbyname(action, "NextURIMetaData");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "NOT_IMPLEMENTED");

			arg = cg_upnp_action_getargumentbyname(action, "PlayMedium");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "NOT_IMPLEMENTED");

			arg = cg_upnp_action_getargumentbyname(action, "RecordMedium");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "NOT_IMPLEMENTED");		

			arg = cg_upnp_action_getargumentbyname(action, "WriteStatus");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "NOT_IMPLEMENTED");					
			return TRUE;
		}
	}

	/* GetPositionInfo*/
	/* Need it*/
	{
		if (cg_streq(actionName, "GetPositionInfo")) {			
			CgUpnpAction *actionSetAVTransportURI;
			CgUpnpArgument *argTmp;
			arg = cg_upnp_action_getargumentbyname(action, "Track");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "1");
			
			arg = cg_upnp_action_getargumentbyname(action, "TrackDuration");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, duration);
			actionSetAVTransportURI = cg_upnp_service_getactionbyname(cg_upnp_action_getservice(action), "SetAVTransportURI");
			argTmp = cg_upnp_action_getargumentbyname(actionSetAVTransportURI, "CurrentURIMetaData");
			if (argTmp){
				arg = cg_upnp_action_getargumentbyname(action, "TrackMetaData");
				if (!arg)
					return FALSE;
				cg_upnp_argument_setvalue(arg, cg_upnp_argument_getvalue(argTmp));
			}

			arg = cg_upnp_action_getargumentbyname(action, "TrackURI");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, cg_string_getvalue(strLocation));


			arg = cg_upnp_action_getargumentbyname(action, "RelTime");
			if (!arg)
				return FALSE;

			hal_HstSendEvent(SYS_EVENT, 0x09010002);

			if(sec>=0)
			{
				sprintf(strTime, "%02d:%02d:%02d", sec/3600, sec/60, sec%60);
			}
			else
			{
				sprintf(strTime, "00:00:00");
			}

			//hal_HstSendEvent(SYS_EVENT, g_currLen);			
			cg_upnp_argument_setvalue(arg, strTime);

			arg = cg_upnp_action_getargumentbyname(action, "AbsTime");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "strTime");

			/* 2.2.23.AbsoluteTimePosition */
			/* If the service implementation doesn't support relative count-based position information */
			/* then this state variable must be set to the maximum value of the i4 data type*/
			/* * argVal=0x7FFFFFFF*/
			arg = cg_upnp_action_getargumentbyname(action, "RelCount");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "2147483647");		

			/* 2.2.24.RelativeCounterPosition */
			/* If the service implementation doesn't support relative count-based position information */
			/* then this state variable must be set to the maximum value of the i4 data type*/
			/* argVal=0x7FFFFFFF*/
			arg = cg_upnp_action_getargumentbyname(action, "AbsCount");
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "2147483647");					
			return TRUE;
		}
	}
	/* GetTransportInfo*/
	/* Need it*/
	{
		if (cg_streq(actionName, CG_UPNPAV_DMR_AVTRANSPORT_GETTRANSPORTINFO)) {
			CgUpnpAction *actionSetAVTransportURI;
			CgUpnpArgument *argTmp;
			arg = cg_upnp_action_getargumentbyname(action, CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATE);
			if (!arg)
				return FALSE;			
			actionSetAVTransportURI = cg_upnp_service_getactionbyname(cg_upnp_action_getservice(action), "SetAVTransportURI");
			argTmp = cg_upnp_action_getargumentbyname(actionSetAVTransportURI, "CurrentURI");
			if (!argTmp)
				return argTmp;
			if (NULL==cg_upnp_argument_getvalue(argTmp))
			{
				cg_upnp_argument_setvalue(arg, "NO_MEDIA_PRESENT");
			}
			else if (cg_streq("", cg_upnp_argument_getvalue(argTmp)))
			{
				cg_upnp_argument_setvalue(arg, "STOPPED");
			}
			else
			{
				cg_upnp_argument_setvalue(arg, "PLAYING");
			}
			
			arg = cg_upnp_action_getargumentbyname(action, CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATUS);
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATUS_OK);

			arg = cg_upnp_action_getargumentbyname(action, CG_UPNPAV_DMR_AVTRANSPORT_CURRENTSPEED);
			if (!arg)
				return FALSE;
			cg_upnp_argument_setvalue(arg, "1");
			return TRUE;
		}
	}


	/* GetTransportSettings*/
	/* No Need*/
	{
		if (cg_streq(actionName, "GetTransportSettings")) {

			return TRUE;
		}
	}

	/* Next*/
	/* No Need*/
	{
		if (cg_streq(actionName, "Next")) {

			return TRUE;
		}
	}	

	/* Pause*/
	{
		if (cg_streq(actionName, "Pause")) {

			return TRUE;
		}
	}		

	/* Play*/
	{
		char *ptemp;
		char *pSlash;
		char portStr[5] = {0};

		if (cg_streq(actionName, "Play")) {			
			CgUpnpAction *actionSetAVTransportURI;
			CgUpnpArgument *argTmp;
			char *location;
			CgNetURL *url;			
			BOOL parseSuccess = TRUE;

			actionSetAVTransportURI = cg_upnp_service_getactionbyname(cg_upnp_action_getservice(action), "SetAVTransportURI");
			argTmp = cg_upnp_action_getargumentbyname(actionSetAVTransportURI, "CurrentURI");
			if (!argTmp)
				return FALSE;
			
			location=cg_upnp_argument_getvalue(argTmp);
			
			if (NULL==location)
				return FALSE;
			else if (cg_streq("",location))
				return FALSE;
			memset(g_url, 0, sizeof(g_url));
			strcpy(g_url, location);

			//  commented by tanyuping
			//url = cg_net_url_new();
			//cg_string_setvalue(strLocation, location);
			//cg_net_url_set(url, location);

			ptemp = strstr(g_url, "//");
			ptemp = strstr(ptemp, ":");
			pSlash = strstr(ptemp, "/");
			strncpy(portStr, ptemp+1, pSlash-ptemp-1);
			strcpy(ptemp, pSlash);

			argTmp = cg_upnp_action_getargumentbyname(actionSetAVTransportURI, "CurrentURIMetaData");
			if(argTmp)
			{
				ptemp = strstr(cg_string_getvalue(argTmp->value), "duration=");
				if(ptemp != NULL)
				{
					strncpy(duration, ptemp+10, 8);
				}
			}
#if 1		
			sec = -1;
			{
				COS_EVENT ev;
				ev.nEventId = EV_HTTP_PLAY_START_REQ;
				ev.nParam1 = (UINT32)g_url;
				ev.nParam2 = atoi(portStr);
				COS_SendEvent(MOD_SSDP_ADVERTISE, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);		
			}
#endif
			//parseSuccess =	cg_upnpav_dmr_avtransport_parseavtransporturl(url);
			//cg_net_url_delete(url);
			

			return parseSuccess;
		}
	}
	
	/* Previous*/
	/* No Need*/
	{
		if (cg_streq(actionName, "Previous")) {

			return TRUE;
		}
	}		

	/* Seek*/
	{
		if (cg_streq(actionName, "Seek")) {

			return TRUE;
		}
	}
	/* SetAVTransportURI*/
	{
		if (cg_streq(actionName, "SetAVTransportURI")) {
			arg = cg_upnp_action_getargumentbyname(action, "CurrentURI");
			if (!arg)
				return FALSE;
			arg = cg_upnp_action_getargumentbyname(action, "CurrentURIMetaData");
			if (!arg)
				return FALSE;			
			{
				CgXmlParser *xmlParser;
				CgXmlNodeList *rootNode;
				CgXmlNode *didlNode;
				CgXmlNode *cnode;
				char *currentURIMetaDataXML;

				currentURIMetaDataXML=cg_upnp_argument_getvalue(arg);

				rootNode = cg_xml_nodelist_new();
				xmlParser = cg_xml_parser_new();
				if (cg_xml_parse(xmlParser, rootNode, currentURIMetaDataXML, cg_strlen(currentURIMetaDataXML))) {
					didlNode = cg_xml_nodelist_getbyname(rootNode, "DIDL-Lite");
					if (didlNode) {
						for (cnode=cg_xml_node_getchildnodes(didlNode); cnode; cnode=cg_xml_node_next(cnode)) {
							printf("xiao add for test\n");
						}
					}
				}
				cg_xml_nodelist_delete(rootNode);
				cg_xml_parser_delete(xmlParser);

			}
			return TRUE;
		}
	}
	
	/* SetNextAVTransportURI*/
	/* No Need*/
	{
		if (cg_streq(actionName, "SetNextAVTransportURI")) {

			return TRUE;
		}
	}		

	/* SetPlayMode*/
	{
		if (cg_streq(actionName, "SetPlayMode")) {

			return TRUE;
		}
	}

	/* Stop*/
	{
		if (cg_streq(actionName, "Stop")) {

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************
 * cg_upnpav_dmr_avtransport_queryreceived
 ****************************************/

BOOL cg_upnpav_dmr_avtransport_queryreceived(CgUpnpStateVariable *statVar)
{
	return FALSE;
}

/****************************************
* cg_upnpav_dmr_avtransport_init
****************************************/

BOOL cg_upnpav_dmr_avtransport_init(CgUpnpAvRenderer *dmr)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAction *action;

	dev = cg_upnpav_dmr_getdevice(dmr);
	if (!dev)
		return FALSE;

	service = cg_upnp_device_getservicebytype(dev, CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE);
	if (!service)
		return FALSE;

	if (cg_upnp_service_parsedescription(service, CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_DESCRIPTION, cg_strlen(CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_DESCRIPTION)) == FALSE)
		return FALSE;

	cg_upnp_service_setuserdata(service, dmr);
	for (action=cg_upnp_service_getactions(service); action; action=cg_upnp_action_next(action))
		cg_upnp_action_setuserdata(action, dmr);

	return TRUE;
}

/****************************************
 * cg_upnpav_dmr_setavtransportlastchange
 ****************************************/

void cg_upnpav_dmr_setavtransportlastchange(CgUpnpAvRenderer *dmr, char *value)
{
	CgUpnpService *service;
	CgUpnpStateVariable *stateVar;

	service = cg_upnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE);
	stateVar = cg_upnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_AVTRANSPORT_LASTCHANGE);
	cg_upnp_statevariable_setvalue(stateVar, value);
}

/****************************************
 * cg_upnpav_dmr_getavtransportlastchange
 ****************************************/

char *cg_upnpav_dmr_getavtransportlastchange(CgUpnpAvRenderer *dmr)
{
	CgUpnpService *service;
	CgUpnpStateVariable *stateVar;

	service = cg_upnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE);
	stateVar = cg_upnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_AVTRANSPORT_LASTCHANGE);
	return cg_upnp_statevariable_getvalue(stateVar);
}
