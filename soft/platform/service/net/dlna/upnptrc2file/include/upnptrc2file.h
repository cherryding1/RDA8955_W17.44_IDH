/* 
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef UPNP_TRC_2_FILE_H
#define UPNP_TRC_2_FILE_H
#if defined(__rda_debug_log__)
#define RDA_DEFAULT_TRC_DIR		"RDA_TRC"
#define	TRC_FILE_HTTPU			"HTTPU"
#define 	TRC_FILE_HTTPMU			"HTTPMU"
#define 	TRC_FILE_SSDP				"SSDP"
#define	TRC_FILE_SOAP				"SOAP"

enum RDA_TRC_FILE_ENUM {
	ENUM_TRC_FILE_HTTPU,
	ENUM_TRC_FILE_HTTPMU,
	ENUM_TRC_FILE_SSDP,
	ENUM_TRC_FILE_SOAP,
};
#define RDA_TRC_PATH_LEN		128
#endif
#endif	//UPNP_TRC_2_FILE_H
