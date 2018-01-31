/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_edc1_h
#define __rrd_edc1_h
#include "csn1lib.h"
#include "ed_lib.h"
#include "rrd_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IA_Rest_Octets".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_IA_Rest_Octets (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IA_Rest_Octets* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IAR_Rest_Octets".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_IAR_Rest_Octets (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IAR_Rest_Octets* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IAX_Rest_Octets".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_IAX_Rest_Octets (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IAX_Rest_Octets* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "Packet_Channel_Description".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_Packet_Channel_Description (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_Packet_Channel_Description* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PDownlink_Assignment_Msg".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PDownlink_Assignment_Msg (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PDownlink_Assignment_Msg* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PDownlink_Assignment_Msg (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PDownlink_Assignment_Msg* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PPDCH_Release_Msg".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_PPDCH_Release_Msg (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PPDCH_Release_Msg* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PPwrCtrl_TA_Msg".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_PPwrCtrl_TA_Msg (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PPwrCtrl_TA_Msg* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PTBF_Release_Msg".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_PTBF_Release_Msg (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PTBF_Release_Msg* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PUplink_Assignment_Msg".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PUplink_Assignment_Msg (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PUplink_Assignment_Msg* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PUplink_Assignment_Msg (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PUplink_Assignment_Msg* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PTimeslot_Reconfigure_Msg".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_PTimeslot_Reconfigure_Msg (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PTimeslot_Reconfigure_Msg* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "GPTiming_Advance_IE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_GPTiming_Advance_IE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_GPTiming_Advance_IE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_GPTiming_Advance_IE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_GPTiming_Advance_IE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "MacCtrlHder".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_MacCtrlHder (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MacCtrlHder* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "Mac_First_Octet".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_Mac_First_Octet (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_Mac_First_Octet* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PUplinkAckNack".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_PUplinkAckNack (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PUplinkAckNack* ED_CONST Destin, long Length));

extern TCSN1ActionStandardPush CSN1Action_rrd_2669850962;
extern TCSN1Action CSN1Action_rrd_1133827888;
extern TCSN1ActionStandardPush CSN1Action_rrd_753805924;
extern TCSN1Action CSN1Action_rrd_745283224;
extern TCSN1ActionEnterField CSN1Action_rrd_3081564446;
extern TCSN1Action CSN1Action_rrd_2466366319;
extern TCSN1Action CSN1Action_rrd_940626461;
extern TCSN1Action CSN1Action_rrd_3047440023;
extern TCSN1Action CSN1Action_rrd_1078574120;
extern TCSN1ActionStandardPush CSN1Action_rrd_1668883658;
extern TCSN1ActionEnterField CSN1Action_rrd_1556416894;
extern TCSN1Action CSN1Action_rrd_3078853209;
extern TCSN1Action CSN1Action_rrd_3141153001;
extern TCSN1ActionStandardPush CSN1Action_rrd_1305163649;
extern TCSN1ActionEnterField CSN1Action_rrd_1424662158;
extern TCSN1Action CSN1Action_rrd_3657785603;
extern TCSN1Action CSN1Action_rrd_3656882063;
extern TCSN1ActionStandardPush CSN1Action_rrd_421764685;
extern TCSN1ActionEnterField CSN1Action_rrd_2495181824;
extern TCSN1Action CSN1Action_rrd_4270272565;
extern TCSN1Action CSN1Action_rrd_2940555322;
extern TCSN1Action CSN1Action_rrd_2354150077;
extern TCSN1Action CSN1Action_rrd_3307903284;
extern TCSN1Action CSN1Action_rrd_2490248654;
extern TCSN1ActionStandardPush CSN1Action_rrd_725173356;
extern TCSN1Action CSN1Action_rrd_2577981689;
extern TCSN1ActionStandardPush CSN1Action_rrd_1887681871;
extern TCSN1ActionEnterField CSN1Action_rrd_1306676725;
extern TCSN1Action CSN1Action_rrd_4268427334;
extern TCSN1Action CSN1Action_rrd_2257836614;
extern TCSN1ActionStandardPush CSN1Action_rrd_4161943267;
extern TCSN1ActionEnterField CSN1Action_rrd_1108116186;
extern TCSN1Action CSN1Action_rrd_3672725872;
extern TCSN1ActionStandardPush CSN1Action_rrd_3169105553;
extern TCSN1ActionEnterField CSN1Action_rrd_721057967;
extern TCSN1Action CSN1Action_rrd_2164320613;
extern TCSN1ActionEnterField CSN1Action_rrd_1381379278;
extern TCSN1Action CSN1Action_rrd_319401405;
extern TCSN1Action CSN1Action_rrd_3303031633;
extern TCSN1ActionEnterField CSN1Action_rrd_3947361250;
extern TCSN1Action CSN1Action_rrd_931921035;
extern TCSN1Action CSN1Action_rrd_899805472;
extern TCSN1ActionStandardPush CSN1Action_rrd_18114464;
extern TCSN1Action CSN1Action_rrd_2224366378;
extern TCSN1ActionStandardPush CSN1Action_rrd_1407900845;
extern TCSN1ActionEnterField CSN1Action_rrd_682135444;
extern TCSN1Action CSN1Action_rrd_1510805457;
extern TCSN1Action CSN1Action_rrd_1181260475;
extern TCSN1ActionEnterField CSN1Action_rrd_1724648138;
extern TCSN1Action CSN1Action_rrd_2123325159;
extern TCSN1Action CSN1Action_rrd_418297134;
extern TCSN1ActionStandardPush CSN1Action_rrd_3906137386;
extern TCSN1ActionEnterField CSN1Action_rrd_1594171974;
extern TCSN1Action CSN1Action_rrd_3968211519;
extern TCSN1Action CSN1Action_rrd_4006116147;
extern TCSN1Action CSN1Action_rrd_1031905020;
extern TCSN1ActionStandardPush CSN1Action_rrd_4222995185;
extern TCSN1ActionEnterField CSN1Action_rrd_4071547402;
extern TCSN1Action CSN1Action_rrd_3355724553;
extern TCSN1Action CSN1Action_rrd_3032010578;
extern TCSN1ActionStandardPush CSN1Action_rrd_3692219366;
extern TCSN1ActionEnterField CSN1Action_rrd_3957363716;
extern TCSN1ActionEnterField CSN1Action_rrd_583545013;
extern TCSN1Action CSN1Action_rrd_2776807507;
extern TCSN1Action CSN1Action_rrd_2990339883;
extern TCSN1Action CSN1Action_rrd_3603340544;
extern TCSN1Action CSN1Action_rrd_370274160;
extern TCSN1Action CSN1Action_rrd_3324313283;
extern TCSN1ActionStandardPush CSN1Action_rrd_2219928662;
extern TCSN1ActionEnterField CSN1Action_rrd_1461877897;
extern TCSN1Action CSN1Action_rrd_62641068;
extern TCSN1Action CSN1Action_rrd_816369318;
extern TCSN1Action CSN1Action_rrd_1184185014;
extern TCSN1Action CSN1Action_rrd_1512249916;
extern TCSN1ActionStandardPush CSN1Action_rrd_343369592;
extern TCSN1Action CSN1Action_rrd_3768330895;
extern TCSN1ActionStandardPush CSN1Action_rrd_2247000875;
extern TCSN1ActionEnterField CSN1Action_rrd_135794089;
extern TCSN1Action CSN1Action_rrd_1254060480;
extern TCSN1Action CSN1Action_rrd_4130083733;
extern TCSN1ActionStandardPush CSN1Action_rrd_861538626;
extern TCSN1ActionEnterField CSN1Action_rrd_2516376418;
extern TCSN1Action CSN1Action_rrd_1849499894;
extern TCSN1ActionEnterField CSN1Action_rrd_2003684985;
extern TCSN1Action CSN1Action_rrd_1859197061;
extern TCSN1ActionStandardPush CSN1Action_rrd_1586180840;
extern TCSN1Action CSN1Action_rrd_1799979658;
extern TCSN1ActionStandardPush CSN1Action_rrd_2207976254;
extern TCSN1ActionEnterField CSN1Action_rrd_3896439917;
extern TCSN1ActionStandardPush CSN1Action_rrd_3465879629;
extern TCSN1ActionEnterField CSN1Action_rrd_1452974428;
extern TCSN1Action CSN1Action_rrd_3268455810;
extern TCSN1Action CSN1Action_rrd_802502716;
extern TCSN1Action CSN1Action_rrd_1135923121;
extern TCSN1ActionStandardPush CSN1Action_rrd_37342690;
extern TCSN1ActionEnterField CSN1Action_rrd_3046323748;
extern TCSN1Action CSN1Action_rrd_1246972339;
extern TCSN1Action CSN1Action_rrd_1285363278;
extern TCSN1Action CSN1Action_rrd_818107967;
extern TCSN1ActionStandardPush CSN1Action_rrd_852952723;
extern TCSN1Action CSN1Action_rrd_869243321;
extern TCSN1Action CSN1Action_rrd_4050602733;
extern TCSN1Action CSN1Action_rrd_2695541923;
extern TCSN1ActionStandardPush CSN1Action_rrd_1212410099;
extern TCSN1ActionEnterField CSN1Action_rrd_605288451;
extern TCSN1Action CSN1Action_rrd_15908375;
extern TCSN1Action CSN1Action_rrd_305719935;
extern TCSN1ActionEnterField CSN1Action_rrd_2694151951;
extern TCSN1Action CSN1Action_rrd_1566960903;
extern TCSN1ActionEnterField CSN1Action_rrd_43453491;
extern TCSN1Action CSN1Action_rrd_2045230129;
extern TCSN1ActionEnterField CSN1Action_rrd_688123492;
extern TCSN1Action CSN1Action_rrd_1838708030;
extern TCSN1ActionEnterField CSN1Action_rrd_2110697282;
extern TCSN1Action CSN1Action_rrd_1226449928;
extern TCSN1ActionEnterField CSN1Action_rrd_2176988567;
extern TCSN1Action CSN1Action_rrd_1836338509;
extern TCSN1ActionEnterField CSN1Action_rrd_220527403;
extern TCSN1Action CSN1Action_rrd_1240865915;
extern TCSN1ActionStandardPush CSN1Action_rrd_2154547747;
extern TCSN1Action CSN1Action_rrd_921627107;
extern TCSN1ActionStandardPush CSN1Action_rrd_374799880;
extern TCSN1ActionEnterField CSN1Action_rrd_2955119003;
extern TCSN1Action CSN1Action_rrd_4286384533;
extern TCSN1Action CSN1Action_rrd_701879132;
extern TCSN1ActionStandardPush CSN1Action_rrd_4179589784;
extern TCSN1ActionEnterField CSN1Action_rrd_533810775;
extern TCSN1Action CSN1Action_rrd_3690944675;
extern TCSN1ActionEnterField CSN1Action_rrd_1314420743;
extern TCSN1Action CSN1Action_rrd_611381025;
extern TCSN1Action CSN1Action_rrd_3179449490;
extern TCSN1ActionStandardPush CSN1Action_rrd_1552518112;
extern TCSN1ActionEnterField CSN1Action_rrd_3025564689;
extern TCSN1Action CSN1Action_rrd_3061427193;
extern TCSN1ActionEnterField CSN1Action_rrd_691585214;
extern TCSN1Action CSN1Action_rrd_2465987279;
extern TCSN1Action CSN1Action_rrd_1023676628;
extern TCSN1ActionEnterField CSN1Action_rrd_1136111960;
extern TCSN1Action CSN1Action_rrd_820190813;
extern TCSN1Action CSN1Action_rrd_3943667231;
extern TCSN1ActionStandardPush CSN1Action_rrd_771026597;
extern TCSN1ActionEnterField CSN1Action_rrd_1218195568;
extern TCSN1Action CSN1Action_rrd_3958307049;
extern TCSN1ActionEnterField CSN1Action_rrd_2961404821;
extern TCSN1Action CSN1Action_rrd_341921643;
extern TCSN1ActionEnterField CSN1Action_rrd_4293703913;
extern TCSN1ActionStandardPush CSN1Action_rrd_957768310;
extern TCSN1ActionEnterField CSN1Action_rrd_2515158594;
extern TCSN1Action CSN1Action_rrd_1614661360;
extern TCSN1ActionEnterField CSN1Action_rrd_4052952432;
extern TCSN1Action CSN1Action_rrd_3140493380;
extern TCSN1ActionEnterField CSN1Action_rrd_1730304484;
extern TCSN1Action CSN1Action_rrd_2679271794;
extern TCSN1Action CSN1Action_rrd_2430886602;
extern TCSN1ActionStandardPush CSN1Action_rrd_102973751;
extern TCSN1ActionStandardPush CSN1Action_rrd_3869353196;
extern TCSN1ActionEnterField CSN1Action_rrd_1981945723;
extern TCSN1Action CSN1Action_rrd_423049785;
extern TCSN1ActionEnterField CSN1Action_rrd_1946900287;
extern TCSN1Action CSN1Action_rrd_228903749;
extern TCSN1ActionEnterField CSN1Action_rrd_1896258710;
extern TCSN1Action CSN1Action_rrd_1345558613;
extern TCSN1ActionEnterField CSN1Action_rrd_128733942;
extern TCSN1Action CSN1Action_rrd_1035299599;
extern TCSN1Action CSN1Action_rrd_4236104023;
extern TCSN1ActionStandardPush CSN1Action_rrd_2994658505;
extern TCSN1ActionEnterField CSN1Action_rrd_102635966;
extern TCSN1Action CSN1Action_rrd_1957808483;
extern TCSN1Action CSN1Action_rrd_3813075944;
extern TCSN1ActionStandardPush CSN1Action_rrd_988764005;
extern TCSN1ActionEnterField CSN1Action_rrd_1503952678;
extern TCSN1Action CSN1Action_rrd_3601411569;
extern TCSN1ActionEnterField CSN1Action_rrd_294902814;
extern TCSN1Action CSN1Action_rrd_1952313616;
extern TCSN1Action CSN1Action_rrd_3954002228;
extern TCSN1ActionStandardPush CSN1Action_rrd_192561310;
extern TCSN1ActionEnterField CSN1Action_rrd_1968255028;
extern TCSN1Action CSN1Action_rrd_1612855327;
extern TCSN1Action CSN1Action_rrd_206623146;
extern TCSN1ActionStandardPush CSN1Action_rrd_98495214;
extern TCSN1ActionEnterField CSN1Action_rrd_1995359580;
extern TCSN1Action CSN1Action_rrd_1151363369;
extern TCSN1ActionStandardPush CSN1Action_rrd_2961858479;
extern TCSN1ActionEnterField CSN1Action_rrd_134145214;
extern TCSN1Action CSN1Action_rrd_1356832806;
extern TCSN1Action CSN1Action_rrd_500936763;
extern TCSN1Action CSN1Action_rrd_2196599466;
extern TCSN1Action CSN1Action_rrd_1067947333;
extern TCSN1ActionStandardPush CSN1Action_rrd_2609845649;
extern TCSN1ActionEnterField CSN1Action_rrd_1285800502;
extern TCSN1Action CSN1Action_rrd_690395763;
extern TCSN1Action CSN1Action_rrd_14819909;
extern TCSN1Action CSN1Action_rrd_3521060345;
extern TCSN1Action CSN1Action_rrd_1246453319;
extern TCSN1Action CSN1Action_rrd_2160226336;
extern TCSN1Action CSN1Action_rrd_1336578230;
extern TCSN1Action CSN1Action_rrd_3289763382;
extern TCSN1Action CSN1Action_rrd_3173535371;
extern TCSN1Action CSN1Action_rrd_127820280;
extern TCSN1ActionStandardPush CSN1Action_rrd_83782761;
extern TCSN1Action CSN1Action_rrd_4026158073;
extern TCSN1ActionStandardPush CSN1Action_rrd_211161005;
extern TCSN1ActionEnterField CSN1Action_rrd_3505515953;
extern TCSN1Action CSN1Action_rrd_3995775179;
extern TCSN1Action CSN1Action_rrd_2277256880;
extern TCSN1ActionStandardPush CSN1Action_rrd_570169241;
extern TCSN1ActionEnterField CSN1Action_rrd_2874891695;
extern TCSN1Action CSN1Action_rrd_2209104785;
extern TCSN1ActionEnterField CSN1Action_rrd_2148942108;
extern TCSN1Action CSN1Action_rrd_2804814503;
extern TCSN1Action CSN1Action_rrd_1676974286;
extern TCSN1ActionStandardPush CSN1Action_rrd_4033811768;
extern TCSN1ActionEnterField CSN1Action_rrd_1254969784;
extern TCSN1Action CSN1Action_rrd_3738642674;
extern TCSN1ActionEnterField CSN1Action_rrd_1327730987;
extern TCSN1Action CSN1Action_rrd_3400065533;
extern TCSN1ActionStandardPush CSN1Action_rrd_137408325;
extern TCSN1ActionEnterField CSN1Action_rrd_606676690;
extern TCSN1Action CSN1Action_rrd_3904400317;
extern TCSN1Action CSN1Action_rrd_1354286267;
extern TCSN1ActionStandardPush CSN1Action_rrd_830738382;
extern TCSN1ActionEnterField CSN1Action_rrd_2514385061;
extern TCSN1Action CSN1Action_rrd_4240941746;
extern TCSN1ActionStandardPush CSN1Action_rrd_1442630865;
extern TCSN1ActionEnterField CSN1Action_rrd_795736949;
extern TCSN1Action CSN1Action_rrd_3426368139;
extern TCSN1ActionStandardPush CSN1Action_rrd_2716306697;
extern TCSN1ActionEnterField CSN1Action_rrd_3148755953;
extern TCSN1Action CSN1Action_rrd_3628462980;
extern TCSN1ActionStandardPush CSN1Action_rrd_1910817308;
extern TCSN1ActionEnterField CSN1Action_rrd_3809203309;
extern TCSN1Action CSN1Action_rrd_4230716097;
extern TCSN1ActionStandardPush CSN1Action_rrd_1331779826;
extern TCSN1ActionEnterField CSN1Action_rrd_1323989918;
extern TCSN1Action CSN1Action_rrd_4206444983;
extern TCSN1ActionEnterField CSN1Action_rrd_1896850844;
extern TCSN1Action CSN1Action_rrd_4199905732;
extern TCSN1Action CSN1Action_rrd_2770170285;
extern TCSN1ActionStandardPush CSN1Action_rrd_74559072;
extern TCSN1ActionEnterField CSN1Action_rrd_3160594213;
extern TCSN1Action CSN1Action_rrd_3015566299;
extern TCSN1Action CSN1Action_rrd_3846238738;
extern TCSN1ActionStandardPush CSN1Action_rrd_2443672996;
extern TCSN1ActionEnterField CSN1Action_rrd_3955384520;
extern TCSN1Action CSN1Action_rrd_3728412801;
extern TCSN1ActionStandardPush CSN1Action_rrd_1398664390;
extern TCSN1ActionEnterField CSN1Action_rrd_3418953735;
extern TCSN1Action CSN1Action_rrd_2713504209;
extern TCSN1ActionEnterField CSN1Action_rrd_554940704;
extern TCSN1Action CSN1Action_rrd_2235472103;
extern TCSN1ActionStandardPush CSN1Action_rrd_2461632642;
extern TCSN1ActionEnterField CSN1Action_rrd_2913546902;
extern TCSN1Action CSN1Action_rrd_2058432357;
extern TCSN1Action CSN1Action_rrd_2855376912;
extern TCSN1ActionStandardPush CSN1Action_rrd_4188853535;
extern TCSN1ActionEnterField CSN1Action_rrd_2821246457;
extern TCSN1Action CSN1Action_rrd_861517178;
extern TCSN1Action CSN1Action_rrd_3295071691;
extern TCSN1ActionStandardPush CSN1Action_rrd_3159481994;
extern TCSN1Action CSN1Action_rrd_2342353946;
extern TCSN1ActionStandardPush CSN1Action_rrd_1156163123;
extern TCSN1ActionEnterField CSN1Action_rrd_789570462;
extern TCSN1Action CSN1Action_rrd_1580432979;
extern TCSN1ActionStandardPush CSN1Action_rrd_3736931631;
extern TCSN1ActionEnterField CSN1Action_rrd_3632617170;
extern TCSN1Action CSN1Action_rrd_2287117600;
extern TCSN1ActionEnterField CSN1Action_rrd_1475689821;
extern TCSN1Action CSN1Action_rrd_389536831;
extern TCSN1ActionStandardPush CSN1Action_rrd_2268793185;
extern TCSN1Action CSN1Action_rrd_4004516662;
extern TCSN1Action CSN1Action_rrd_2042348747;
extern TCSN1ActionStandardPush CSN1Action_rrd_1499184372;
extern TCSN1ActionStandardPush CSN1Action_rrd_3752349296;
extern TCSN1ActionEnterField CSN1Action_rrd_3266119808;
extern TCSN1Action CSN1Action_rrd_87488053;
extern TCSN1Action CSN1Action_rrd_2131255502;
extern TCSN1ActionStandardPush CSN1Action_rrd_2071733023;
extern TCSN1Action CSN1Action_rrd_1993282762;
extern TCSN1ActionStandardPush CSN1Action_rrd_657403020;
extern TCSN1ActionStandardPush CSN1Action_rrd_3338436673;
extern TCSN1ActionEnterField CSN1Action_rrd_531806091;
extern TCSN1Action CSN1Action_rrd_400286796;
extern TCSN1Action CSN1Action_rrd_3831644075;
extern TCSN1ActionStandardPush CSN1Action_rrd_2644140889;
extern TCSN1Action CSN1Action_rrd_2065487861;
extern TCSN1ActionStandardPush CSN1Action_rrd_2437170820;
extern TCSN1ActionEnterField CSN1Action_rrd_154954186;
extern TCSN1Action CSN1Action_rrd_1756366191;
extern TCSN1ActionEnterField CSN1Action_rrd_3884916139;
extern TCSN1Action CSN1Action_rrd_565487363;
extern TCSN1ActionEnterField CSN1Action_rrd_1783068221;
extern TCSN1Action CSN1Action_rrd_3360651600;
extern TCSN1ActionEnterField CSN1Action_rrd_3511221349;
extern TCSN1Action CSN1Action_rrd_2537534189;
extern TCSN1ActionStandardPush CSN1Action_rrd_1707319245;
extern TCSN1Action CSN1Action_rrd_1410763817;
extern TCSN1Action CSN1Action_rrd_1958843828;
extern TCSN1Action CSN1Action_rrd_2612609894;
extern TCSN1ActionStandardPush CSN1Action_rrd_730981422;
extern TCSN1Action CSN1Action_rrd_3615734445;
extern TCSN1ActionEnterField CSN1Action_rrd_1124905692;
extern TCSN1Action CSN1Action_rrd_2678951837;
extern TCSN1Action CSN1Action_rrd_2860452829;
extern TCSN1ActionStandardPush CSN1Action_rrd_1517254419;
extern TCSN1ActionEnterField CSN1Action_rrd_1788477443;
extern TCSN1Action CSN1Action_rrd_3150178008;
extern TCSN1ActionEnterField CSN1Action_rrd_1511184675;
extern TCSN1Action CSN1Action_rrd_2671884270;
extern TCSN1ActionEnterField CSN1Action_rrd_3039752853;
extern TCSN1Action CSN1Action_rrd_3588243530;
extern TCSN1Action CSN1Action_rrd_162940200;
extern TCSN1ActionStandardPush CSN1Action_rrd_2442720496;
extern TCSN1ActionEnterField CSN1Action_rrd_3236281908;
extern TCSN1Action CSN1Action_rrd_250455806;
extern TCSN1Action CSN1Action_rrd_378371991;
extern TCSN1ActionStandardPush CSN1Action_rrd_420052828;
extern TCSN1ActionEnterField CSN1Action_rrd_3086744707;
extern TCSN1Action CSN1Action_rrd_1206453394;
extern TCSN1Action CSN1Action_rrd_2127239902;
extern TCSN1ActionStandardPush CSN1Action_rrd_3878419657;
extern TCSN1ActionEnterField CSN1Action_rrd_2610768679;
extern TCSN1Action CSN1Action_rrd_1667978660;
extern TCSN1Action CSN1Action_rrd_2545748283;
extern TCSN1Action CSN1Action_rrd_2720851806;
extern TCSN1ActionEnterField CSN1Action_rrd_1035585519;
extern TCSN1Action CSN1Action_rrd_4062936263;
extern TCSN1Action CSN1Action_rrd_367611342;
extern TCSN1ActionStandardPush CSN1Action_rrd_3183967548;
extern TCSN1ActionEnterField CSN1Action_rrd_1752418164;
extern TCSN1Action CSN1Action_rrd_4049736060;
extern TCSN1ActionEnterField CSN1Action_rrd_2829314208;
extern TCSN1Action CSN1Action_rrd_711981000;
extern TCSN1ActionEnterField CSN1Action_rrd_2850852071;
extern TCSN1Action CSN1Action_rrd_3140476587;
extern TCSN1ActionEnterField CSN1Action_rrd_4095745213;
extern TCSN1Action CSN1Action_rrd_1223096184;
extern TCSN1Action CSN1Action_rrd_2489923055;
extern TCSN1ActionStandardPush CSN1Action_rrd_1768190373;
extern TCSN1ActionEnterField CSN1Action_rrd_1112211269;
extern TCSN1ActionEnterField CSN1Action_rrd_1466427016;
extern TCSN1ActionEnterField CSN1Action_rrd_4279342406;
extern TCSN1ActionStandardPush CSN1Action_rrd_1110806973;
extern TCSN1Action CSN1Action_rrd_1843080495;
extern TCSN1ActionEnterField CSN1Action_rrd_483363292;
extern TCSN1Action CSN1Action_rrd_3486522109;
extern TCSN1ActionStandardPush CSN1Action_rrd_2381861864;
extern TCSN1ActionEnterField CSN1Action_rrd_4038189668;
extern TCSN1ActionEnterField CSN1Action_rrd_507223965;
extern TCSN1ActionEnterField CSN1Action_rrd_2952912004;
extern TCSN1Action CSN1Action_rrd_2412969979;
extern TCSN1ActionStandardPush CSN1Action_rrd_334853886;
extern TCSN1ActionEnterField CSN1Action_rrd_2317991597;
extern TCSN1Action CSN1Action_rrd_2994332061;
extern TCSN1ActionStandardPush CSN1Action_rrd_1246317353;
extern TCSN1ActionEnterField CSN1Action_rrd_198831330;
extern TCSN1Action CSN1Action_rrd_3056860181;
extern TCSN1Action CSN1Action_rrd_2819297462;
extern TCSN1ActionEnterField CSN1Action_rrd_3148479826;
extern TCSN1Action CSN1Action_rrd_4007706475;
extern TCSN1ActionStandardPush CSN1Action_rrd_1931385926;
extern TCSN1ActionEnterField CSN1Action_rrd_248884930;
extern TCSN1ActionEnterField CSN1Action_rrd_3749016525;
extern TCSN1ActionEnterField CSN1Action_rrd_3090839876;
extern TCSN1ActionEnterField CSN1Action_rrd_1794889550;
extern TCSN1Action CSN1Action_rrd_3132594304;
extern TCSN1Action CSN1Action_rrd_3068548146;
extern TCSN1ActionStandardPush CSN1Action_rrd_2030131668;
extern TCSN1ActionEnterField CSN1Action_rrd_451415012;
extern TCSN1ActionEnterField CSN1Action_rrd_3085223348;
extern TCSN1ActionEnterField CSN1Action_rrd_501623148;
extern TCSN1Action CSN1Action_rrd_2032260957;
extern TCSN1ActionStandardPush CSN1Action_rrd_3581718616;
extern TCSN1Action CSN1Action_rrd_2301433874;
extern TCSN1Action CSN1Action_rrd_2632245798;
extern TCSN1Action CSN1Action_rrd_1212702238;
extern TCSN1ActionStandardPush CSN1Action_rrd_760487328;
extern TCSN1Action CSN1Action_rrd_341599030;
extern TCSN1Action CSN1Action_rrd_3093738256;
extern TCSN1Action CSN1Action_rrd_3401615348;
extern TCSN1ActionStandardPush CSN1Action_rrd_274780630;
extern TCSN1Action CSN1Action_rrd_2329652229;
extern TCSN1ActionStandardPush CSN1Action_rrd_2342405214;
extern TCSN1Action CSN1Action_rrd_1176402565;
extern TCSN1ActionStandardPush CSN1Action_rrd_264773991;
extern TCSN1ActionEnterField CSN1Action_rrd_1541952114;
extern TCSN1Action CSN1Action_rrd_1997839531;
extern TCSN1ActionStandardPush CSN1Action_rrd_2045584149;
extern TCSN1ActionEnterField CSN1Action_rrd_965469091;
extern TCSN1Action CSN1Action_rrd_656833653;
extern TCSN1ActionEnterField CSN1Action_rrd_3654314478;
extern TCSN1Action CSN1Action_rrd_3857833984;
extern TCSN1ActionEnterField CSN1Action_rrd_225237873;
extern TCSN1Action CSN1Action_rrd_3245584694;
extern TCSN1Action CSN1Action_rrd_3504385127;
extern TCSN1ActionStandardPush CSN1Action_rrd_3447149479;
extern TCSN1ActionEnterField CSN1Action_rrd_408796126;
extern TCSN1Action CSN1Action_rrd_1400567278;
extern TCSN1ActionEnterField CSN1Action_rrd_351339013;
extern TCSN1Action CSN1Action_rrd_2289505114;
extern TCSN1ActionEnterField CSN1Action_rrd_3360767682;
extern TCSN1Action CSN1Action_rrd_2012783832;
extern TCSN1Action CSN1Action_rrd_3466817332;
extern TCSN1ActionStandardPush CSN1Action_rrd_3114596985;
extern TCSN1ActionEnterField CSN1Action_rrd_96542679;
extern TCSN1Action CSN1Action_rrd_444487554;
extern TCSN1ActionStandardPush CSN1Action_rrd_1943730191;
extern TCSN1Action CSN1Action_rrd_312719954;
extern TCSN1ActionStandardPush CSN1Action_rrd_1751255976;
extern TCSN1ActionEnterField CSN1Action_rrd_1691172885;
extern TCSN1Action CSN1Action_rrd_61091139;
extern TCSN1ActionEnterField CSN1Action_rrd_3154444110;
extern TCSN1ActionEnterField CSN1Action_rrd_1823283407;
extern TCSN1Action CSN1Action_rrd_758620798;
extern TCSN1ActionStandardPush CSN1Action_rrd_1027217481;
extern TCSN1ActionEnterField CSN1Action_rrd_2573199844;
extern TCSN1ActionEnterField CSN1Action_rrd_2198201317;
extern TCSN1ActionEnterField CSN1Action_rrd_519562849;
extern TCSN1Action CSN1Action_rrd_875678573;
extern TCSN1ActionStandardPush CSN1Action_rrd_2599842611;
extern TCSN1ActionEnterField CSN1Action_rrd_3321212140;
extern TCSN1ActionStandardPush CSN1Action_rrd_1483226791;
extern TCSN1ActionEnterField CSN1Action_rrd_465287089;
extern TCSN1ActionEnterField CSN1Action_rrd_1110837773;
extern TCSN1Action CSN1Action_rrd_1402391965;
extern TCSN1Action CSN1Action_rrd_3251358857;
extern TCSN1ActionStandardPush CSN1Action_rrd_2818920680;
extern TCSN1Action CSN1Action_rrd_3528460552;
extern TCSN1ActionStandardPush CSN1Action_rrd_137419891;
extern TCSN1ActionEnterField CSN1Action_rrd_3643433079;
extern TCSN1Action CSN1Action_rrd_2901754476;
extern TCSN1ActionEnterField CSN1Action_rrd_3527408420;
extern TCSN1Action CSN1Action_rrd_1889926130;
extern TCSN1Action CSN1Action_rrd_1816175321;
extern TCSN1ActionEnterField CSN1Action_rrd_606156793;
extern TCSN1Action CSN1Action_rrd_3096087395;
extern TCSN1ActionStandardPush CSN1Action_rrd_2028206836;
extern TCSN1Action CSN1Action_rrd_3365294586;
extern TCSN1ActionStandardPush CSN1Action_rrd_3489445354;
extern TCSN1ActionEnterField CSN1Action_rrd_2463003367;
extern TCSN1Action CSN1Action_rrd_3860702602;
extern TCSN1Action CSN1Action_rrd_3105487277;
extern TCSN1Action CSN1Action_rrd_371009464;
extern TCSN1ActionEnterField CSN1Action_rrd_3481058681;
extern TCSN1Action CSN1Action_rrd_1056704180;
extern TCSN1ActionStandardPush CSN1Action_rrd_2067698156;
extern TCSN1Action CSN1Action_rrd_4261741148;
extern TCSN1Action CSN1Action_rrd_96581344;
extern TCSN1Action CSN1Action_rrd_2804258136;
extern TCSN1Action CSN1Action_rrd_3746986023;
extern TCSN1ActionEnterField CSN1Action_rrd_2696491744;
extern TCSN1Action CSN1Action_rrd_1252036399;
extern TCSN1Action CSN1Action_rrd_3111607087;
extern TCSN1ActionStandardPush CSN1Action_rrd_1877292183;
extern TCSN1ActionEnterField CSN1Action_rrd_2015990617;
extern TCSN1Action CSN1Action_rrd_2444061083;
extern TCSN1ActionEnterField CSN1Action_rrd_4138463979;
extern TCSN1Action CSN1Action_rrd_1847778841;
extern TCSN1ActionEnterField CSN1Action_rrd_1712146324;
extern TCSN1Action CSN1Action_rrd_1379398860;
extern TCSN1Action CSN1Action_rrd_2006853871;
extern TCSN1Action CSN1Action_rrd_528587181;
extern TCSN1Action CSN1Action_rrd_3500588652;
extern TCSN1ActionStandardPush CSN1Action_rrd_260805406;
extern TCSN1Action CSN1Action_rrd_2066839617;
extern TCSN1ActionStandardPush CSN1Action_rrd_2848967677;
extern TCSN1Action CSN1Action_rrd_2249596254;
extern TCSN1Action CSN1Action_rrd_1369517849;
extern TCSN1Action CSN1Action_rrd_296027977;
extern TCSN1Action CSN1Action_rrd_10059355;
extern TCSN1Action CSN1Action_rrd_3089161910;
extern TCSN1Action CSN1Action_rrd_2412503066;
extern TCSN1ActionStandardPush CSN1Action_rrd_3570528058;
extern TCSN1Action CSN1Action_rrd_1724122115;
extern TCSN1Action CSN1Action_rrd_891770495;
extern TCSN1Action CSN1Action_rrd_1949137066;
extern TCSN1Action CSN1Action_rrd_133844443;
extern TCSN1Action CSN1Action_rrd_902524428;
extern TCSN1Action CSN1Action_rrd_1041495009;
extern TCSN1Action CSN1Action_rrd_2613449569;
extern TCSN1ActionStandardPush CSN1Action_rrd_3119095379;
extern TCSN1Action CSN1Action_rrd_4169461485;
extern TCSN1Action CSN1Action_rrd_1486988581;
extern TCSN1Action CSN1Action_rrd_3494664592;
extern TCSN1Action CSN1Action_rrd_2082731027;
extern TCSN1Action CSN1Action_rrd_4219277482;
extern TCSN1ActionStandardPush CSN1Action_rrd_3646744771;
extern TCSN1Action CSN1Action_rrd_444953597;
extern TCSN1ActionStandardPush CSN1Action_rrd_2064389508;
extern TCSN1ActionEnterField CSN1Action_rrd_2210362115;
extern TCSN1Action CSN1Action_rrd_4001360806;
extern TCSN1ActionStandardPush CSN1Action_rrd_1742278308;
extern TCSN1ActionEnterField CSN1Action_rrd_3219495542;
extern TCSN1ActionEnterField CSN1Action_rrd_3493767560;
extern TCSN1Action CSN1Action_rrd_3638705156;
extern TCSN1ActionStandardPush CSN1Action_rrd_970255238;
extern TCSN1ActionEnterField CSN1Action_rrd_3591486212;
extern TCSN1ActionEnterField CSN1Action_rrd_4189082912;
extern TCSN1Action CSN1Action_rrd_1189437493;
extern TCSN1ActionEnterField CSN1Action_rrd_2248246476;
extern TCSN1Action CSN1Action_rrd_3883124145;
extern TCSN1ActionEnterField CSN1Action_rrd_1960117797;
extern TCSN1Action CSN1Action_rrd_3287389319;

#ifdef __cplusplus
	};
#endif
#endif

