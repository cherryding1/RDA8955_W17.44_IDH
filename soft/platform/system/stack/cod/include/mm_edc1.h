/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __mm_edc1_h
#define __mm_edc1_h
#include "csn1lib.h"
#include "ed_lib.h"
#include "mm_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RxNpduNumList".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RxNpduNumList (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RxNpduNumList* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RxNpduNumList (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RxNpduNumList* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "Packet_Paging_Request".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_Packet_Paging_Request (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_Packet_Paging_Request* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_Packet_Paging_Request (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_Packet_Paging_Request* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "P1_Rest_Octets".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_P1_Rest_Octets (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_P1_Rest_Octets* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "P2_Rest_Octets".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_P2_Rest_Octets (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_P2_Rest_Octets* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "P3_Rest_Octets".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_P3_Rest_Octets (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_P3_Rest_Octets* ED_CONST Destin, long Length));

extern TCSN1Action CSN1Action_mm_1623356742;
extern TCSN1Action CSN1Action_mm_812881617;
extern TCSN1Action CSN1Action_mm_2109956447;
extern TCSN1Action CSN1Action_mm_3106624490;
extern TCSN1ActionStandardPush CSN1Action_mm_3793200950;
extern TCSN1Action CSN1Action_mm_452156319;
extern TCSN1Action CSN1Action_mm_3915230450;
extern TCSN1Action CSN1Action_mm_1949137066;
extern TCSN1Action CSN1Action_mm_2687984519;
extern TCSN1Action CSN1Action_mm_3454008772;
extern TCSN1Action CSN1Action_mm_2497997005;
extern TCSN1ActionStandardPush CSN1Action_mm_906338781;
extern TCSN1Action CSN1Action_mm_3458707629;
extern TCSN1Action CSN1Action_mm_3089161910;
extern TCSN1Action CSN1Action_mm_4219277482;
extern TCSN1Action CSN1Action_mm_3473561638;
extern TCSN1Action CSN1Action_mm_378923677;
extern TCSN1ActionStandardPush CSN1Action_mm_2828025132;
extern TCSN1ActionEnterField CSN1Action_mm_3663591043;
extern TCSN1Action CSN1Action_mm_305719935;
extern TCSN1ActionEnterField CSN1Action_mm_1031329792;
extern TCSN1Action CSN1Action_mm_816369318;
extern TCSN1ActionStandardPush CSN1Action_mm_898331810;
extern TCSN1Action CSN1Action_mm_2519741375;
extern TCSN1Action CSN1Action_mm_3047440023;
extern TCSN1Action CSN1Action_mm_2940555322;
extern TCSN1ActionStandardPush CSN1Action_mm_929139762;
extern TCSN1Action CSN1Action_mm_2277539003;
extern TCSN1Action CSN1Action_mm_2253262392;
extern TCSN1Action CSN1Action_mm_2249596254;
extern TCSN1Action CSN1Action_mm_3672295627;
extern TCSN1Action CSN1Action_mm_102177801;
extern TCSN1Action CSN1Action_mm_3524270380;
extern TCSN1ActionStandardPush CSN1Action_mm_2188005202;
extern TCSN1Action CSN1Action_mm_2257402512;
extern TCSN1Action CSN1Action_mm_2228969384;
extern TCSN1Action CSN1Action_mm_2405797206;
extern TCSN1ActionStandardPush CSN1Action_mm_1262602591;
extern TCSN1Action CSN1Action_mm_1736699828;
extern TCSN1Action CSN1Action_mm_2690191006;
extern TCSN1Action CSN1Action_mm_3924915129;
extern TCSN1Action CSN1Action_mm_891301704;
extern TCSN1Action CSN1Action_mm_2257242352;
extern TCSN1ActionStandardPush CSN1Action_mm_1083490662;
extern TCSN1Action CSN1Action_mm_1998667685;
extern TCSN1ActionStandardPush CSN1Action_mm_338533924;
extern TCSN1Action CSN1Action_mm_3309716439;
extern TCSN1Action CSN1Action_mm_1896178442;
extern TCSN1Action CSN1Action_mm_2504552536;
extern TCSN1Action CSN1Action_mm_2069162026;
extern TCSN1ActionStandardPush CSN1Action_mm_2658244617;
extern TCSN1Action CSN1Action_mm_3559637444;
extern TCSN1Action CSN1Action_mm_2975093361;
extern TCSN1Action CSN1Action_mm_3973644031;
extern TCSN1Action CSN1Action_mm_3702817432;
extern TCSN1Action CSN1Action_mm_382868336;
extern TCSN1Action CSN1Action_mm_703584885;
extern TCSN1ActionStandardPush CSN1Action_mm_852952723;
extern TCSN1ActionStandardPush CSN1Action_mm_2160892305;
extern TCSN1ActionStandardPush CSN1Action_mm_3297163296;
extern TCSN1Action CSN1Action_mm_1039706923;
extern TCSN1Action CSN1Action_mm_3021259705;
extern TCSN1ActionStandardPush CSN1Action_mm_1588104227;
extern TCSN1Action CSN1Action_mm_1426301634;
extern TCSN1Action CSN1Action_mm_1714678469;
extern TCSN1Action CSN1Action_mm_745283224;
extern TCSN1Action CSN1Action_mm_3648354277;
extern TCSN1Action CSN1Action_mm_1019069955;
extern TCSN1ActionStandardPush CSN1Action_mm_2349324667;
extern TCSN1ActionEnterField CSN1Action_mm_2983985688;
extern TCSN1Action CSN1Action_mm_1051650937;
extern TCSN1ActionStandardPush CSN1Action_mm_3624339234;
extern TCSN1ActionEnterField CSN1Action_mm_3243020998;
extern TCSN1Action CSN1Action_mm_4006116147;
extern TCSN1Action CSN1Action_mm_1329795257;
extern TCSN1Action CSN1Action_mm_2040935313;
extern TCSN1ActionStandardPush CSN1Action_mm_1285208213;
extern TCSN1Action CSN1Action_mm_705805430;
extern TCSN1ActionStandardPush CSN1Action_mm_740434718;
extern TCSN1ActionEnterField CSN1Action_mm_2226064234;
extern TCSN1ActionEnterField CSN1Action_mm_2864229810;
extern TCSN1ActionStandardPush CSN1Action_mm_3264869494;
extern TCSN1Action CSN1Action_mm_976167061;
extern TCSN1Action CSN1Action_mm_2383332620;
extern TCSN1Action CSN1Action_mm_348479049;
extern TCSN1ActionEnterField CSN1Action_mm_1105332698;
extern TCSN1Action CSN1Action_mm_3925280648;
extern TCSN1ActionStandardPush CSN1Action_mm_1748033528;
extern TCSN1Action CSN1Action_mm_2354150077;
extern TCSN1Action CSN1Action_mm_3307903284;
extern TCSN1Action CSN1Action_mm_2490248654;
extern TCSN1Action CSN1Action_mm_3587088796;
extern TCSN1ActionStandardPush CSN1Action_mm_503608320;
extern TCSN1Action CSN1Action_mm_2904529805;
extern TCSN1Action CSN1Action_mm_3603340544;
extern TCSN1Action CSN1Action_mm_445249957;
extern TCSN1Action CSN1Action_mm_370274160;
extern TCSN1Action CSN1Action_mm_1286476466;
extern TCSN1Action CSN1Action_mm_2430021842;
extern TCSN1Action CSN1Action_mm_791498803;
extern TCSN1Action CSN1Action_mm_390578230;
extern TCSN1Action CSN1Action_mm_4139192109;
extern TCSN1Action CSN1Action_mm_64959846;
extern TCSN1Action CSN1Action_mm_2387339982;
extern TCSN1ActionStandardPush CSN1Action_mm_1690988736;
extern TCSN1Action CSN1Action_mm_2335260607;
extern TCSN1Action CSN1Action_mm_313259946;
extern TCSN1Action CSN1Action_mm_392853104;
extern TCSN1Action CSN1Action_mm_2067045791;
extern TCSN1Action CSN1Action_mm_992909587;
extern TCSN1Action CSN1Action_mm_3501535647;
extern TCSN1ActionEnterField CSN1Action_mm_2421815279;
extern TCSN1Action CSN1Action_mm_3557132037;
extern TCSN1Action CSN1Action_mm_4018032394;
extern TCSN1ActionEnterField CSN1Action_mm_3417461136;
extern TCSN1Action CSN1Action_mm_4035434035;
extern TCSN1ActionStandardPush CSN1Action_mm_3863658337;
extern TCSN1Action CSN1Action_mm_3368039029;
extern TCSN1ActionStandardPush CSN1Action_mm_2814179522;
extern TCSN1ActionEnterField CSN1Action_mm_1980997949;
extern TCSN1Action CSN1Action_mm_2634098025;
extern TCSN1Action CSN1Action_mm_2470336396;
extern TCSN1Action CSN1Action_mm_1446670203;
extern TCSN1Action CSN1Action_mm_1150058242;
extern TCSN1Action CSN1Action_mm_165832568;
extern TCSN1Action CSN1Action_mm_815343711;
extern TCSN1Action CSN1Action_mm_654552679;
extern TCSN1ActionEnterField CSN1Action_mm_3103887201;
extern TCSN1Action CSN1Action_mm_3112399967;
extern TCSN1ActionStandardPush CSN1Action_mm_2881506751;
extern TCSN1Action CSN1Action_mm_4272582726;
extern TCSN1ActionStandardPush CSN1Action_mm_188563141;
extern TCSN1ActionEnterField CSN1Action_mm_1574136610;
extern TCSN1Action CSN1Action_mm_3111078956;
extern TCSN1ActionEnterField CSN1Action_mm_3955403918;
extern TCSN1Action CSN1Action_mm_2649562394;
extern TCSN1Action CSN1Action_mm_978864089;
extern TCSN1Action CSN1Action_mm_3073337609;
extern TCSN1Action CSN1Action_mm_2363480941;
extern TCSN1Action CSN1Action_mm_3249652503;
extern TCSN1Action CSN1Action_mm_369355673;
extern TCSN1Action CSN1Action_mm_1533932515;

#ifdef __cplusplus
	};
#endif
#endif

