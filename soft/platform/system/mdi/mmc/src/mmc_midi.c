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


#ifdef MEDIA_MIDI_SUPPORT
#ifndef SHEEN_VC_DEBUG
#include "cswtype.h"
#include "mcip_debug.h"
#include "fs.h"

#include "hal_error.h"

#include "hal_timers.h"

#include "cpu_share.h"
#include "assert.h"


#include "global.h"
#include "audio_api.h"

#include "aud_m.h"

#include "fs_asyn.h"

#include "mci.h"
#include "cos.h"

#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"

#include "string.h"
#include "hal_overlay.h"
#ifdef AUD_3_IN_1_SPK

#include "mmc_audiovibrator.h"
#endif

#endif

#include "mmc_midi.h"

#ifndef USE_TABS_8K
#include "lib_array.h"
#else
#include "lib_array_8k.h"
#endif

#ifndef SHEEN_VC_DEBUG
extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

extern UINT32 pcmbuf_overlay[4608];
#endif

volatile  INT32 Mmc_Midi_Pcm_Half=0;

BOOL g_MidiBT8KEnbleFlag=FALSE;
#ifndef SHEEN_VC_DEBUG
extern MPEG_PLAY MpegPlayer;
#endif

#ifndef USE_ALL_TABS
const unsigned char* const phuf[128+61]=
{

    acpianohuf  ,britepnohuf  ,epiano1huf  ,synpianohuf  ,epiano1huf  ,epiano1huf ,hrpschrdhuf  ,clavinethuf  ,
    celestehuf  ,glockenhuf  ,celestehuf  ,celestehuf  ,marimbahuf  ,celestehuf ,tubebellhuf  ,celestehuf ,
    harmoncahuf ,harmoncahuf ,harmoncahuf ,harmoncahuf ,harmoncahuf ,harmoncahuf ,harmoncahuf  ,harmoncahuf ,
    nyguitarhuf  ,nyguitarhuf  ,jazzgtrhuf  ,jazzgtrhuf  ,mutegtrhuf/*bad mutetrumhuf*/  ,odguitarhuf  ,odguitarhuf  ,NULL ,

    acbasshuf  ,acbasshuf  ,acbasshuf  ,fretlesshuf  ,fretlesshuf  ,fretlesshuf  ,fretlesshuf  ,fretlesshuf  ,
    pizzcatohuf ,pizzcatohuf ,pizzcatohuf ,pizzcatohuf ,pizzcatohuf ,pizzcatohuf  ,pizzcatohuf  ,NULL ,
    slowstrhuf  ,slowstrhuf  ,synstr1huf/*bad synstr2huf*/  ,synstr2huf/*bad synstr2huf*/  ,synstr2huf/*bad synstr2huf*/  ,doohuf  ,voiceshuf  ,doohuf  ,
    trumpethuf  ,trumpethuf  ,NULL ,mutetrumhuf/*bad mutetrumhuf*/  ,synbras2huf  ,synbras2huf  ,synbras2huf  ,synbras2huf  ,

    sprnosaxhuf  ,altosaxhuf  ,altosaxhuf  ,sprnosaxhuf  ,oboehuf  ,NULL ,NULL ,clarinethuf  ,
    whistlehuf  ,whistlehuf  ,whistlehuf ,whistlehuf  ,whistlehuf  ,NULL ,whistlehuf  ,ocarinahuf  ,
    sqrwavehuf  ,sqrwavehuf  ,sqrwavehuf  ,sqrwavehuf  ,sqrwavehuf  ,sqrwavehuf  ,NULL ,NULL ,
    NULL  ,NULL  ,NULL ,NULL ,NULL ,NULL ,NULL ,NULL  ,

    soundtrkhuf  ,soundtrkhuf  ,soundtrkhuf  ,soundtrkhuf  ,soundtrkhuf  ,soundtrkhuf  ,soundtrkhuf  ,soundtrkhuf  ,
    banjohuf  ,banjohuf  ,kalimbahuf  ,kalimbahuf  ,kalimbahuf  ,shannaihuf  ,shannaihuf  ,shannaihuf  ,
    carillonhuf  ,carillonhuf  ,carillonhuf  ,woodblkhuf  ,woodblkhuf  ,woodblkhuf  ,woodblkhuf  ,carillonhuf  ,
    NULL ,NULL ,NULL ,junglehuf/*added*/ ,NULL ,NULL ,NULL ,NULL ,

    stickshuf ,stickshuf  ,stickshuf ,stickshuf ,stickshuf  ,metclickhuf  ,metclickhuf  ,metclickhuf  ,
    kick1huf  ,kick1huf  ,stickshuf  ,kick1huf  ,stickshuf  ,kick1huf  ,kick1huf  ,hihatclhuf  ,
    stickshuf  ,stickshuf  ,stickshuf  ,stickshuf  ,stickshuf  ,stickshuf  ,cymride1huf  ,stickshuf  ,
    cymride1huf  ,cymride1huf  ,stickshuf  ,tamborinhuf  ,cymride1huf  ,cowbellhuf  ,cymride1huf  ,cymride1huf  ,

    cymride1huf  ,bongohihuf  ,bongohihuf  ,congalohuf  ,congalohuf  ,congalohuf  ,NULL ,NULL ,
    NULL ,NULL ,maracashuf  ,maracashuf  ,NULL ,NULL ,NULL ,NULL ,
    clavehuf  ,clavehuf  ,woodblkhuf  ,clavehuf  ,woodblkhuf  ,triangl1huf  ,NULL ,triangl1huf  ,
    NULL ,NULL ,NULL ,NULL  ,NULL  ,
};
#else
const unsigned char* const phuf[128+61]=
{
    acpianohuf, britepnohuf,    synpianohuf,    honkyhuf,   epiano1huf, epiano2huf, hrpschrdhuf,    clavinethuf,
    celestehuf, glockenhuf, musicboxhuf,    vibeshuf,   marimbahuf, xylophonhuf,    tubebellhuf,    santurhuf,
    homeorghuf, percorghuf, rockorghuf, churchhuf,  reedorghuf, accordnhuf, harmoncahuf,    concrtnahuf,
    nyguitarhuf,    acguitarhuf,    jazzgtrhuf, cleangtrhuf,    mutegtrhuf, odguitarhuf,    distgtrhuf, gtrharmhuf,

    acbasshuf,  fngrbasshuf,    pickbasshuf,    fretlesshuf,    slapbas1huf,    slapbas2huf,    synbass1huf,    synbass2huf,
    violinhuf,  violahuf,   cellohuf,   contrabahuf,    tremstrhuf, pizzcatohuf,    harphuf,    timpanihuf,
    marcatohuf, slowstrhuf, synstr1huf, synstr2huf, choirhuf,   doohuf, voiceshuf,  orchhithuf,
    trumpethuf, trombonehuf,    tubahuf,    mutetrumhuf,    frenchrnhuf,    hitbrasshuf,    synbras1huf,    synbras2huf,

    sprnosaxhuf,    altosaxhuf, tenorsaxhuf,    barisaxhuf, oboehuf,    englhornhuf,    bassoonhuf, clarinethuf,
    piccolohuf, flutehuf,   recorderhuf,    woodfluthuf,    bottlehuf,  shakazulhuf,    whistlehuf, ocarinahuf,
    sqrwavehuf, sawwavehuf, calliopehuf,    chifleadhuf,    charanghuf, voxleadhuf, lead5thhuf, bassleadhuf,
    fantasiahuf,    warmpadhuf, polysynhuf, ghostiehuf, bowglasshuf,    metalpadhuf,    halopadhuf, sweeperhuf,

    aurorahuf,  soundtrkhuf,    crystalhuf, atmosphrhuf,    freshairhuf,    unicornhuf, echovoxhuf, startrakhuf,
    sitarhuf,   banjohuf,   shamisenhuf,    kotohuf,    kalimbahuf, bagpipeshuf,    fiddlehuf,  shannaihuf,
    carillonhuf,    agogohuf,   steeldrmhuf,    woodblkhuf, taikohuf,   tomshuf,    syntomhuf,  revcymhuf,
    fx_frethuf, fx_blowhuf, seashorehuf,    junglehuf,  telephonhuf,    helicptrhuf,    applausehuf,    pistolhuf,

    highqhuf,   slaphuf,    scratch1huf,    scratch2huf,    stickshuf,  sqrclickhuf,    metclickhuf,    metbellhuf,
    kick1huf,   kick2huf,   stickrimhuf,    snare1huf,  clapshuf,   snare2huf,  tomlo2huf,  hihatclhuf,
    tomlo1huf,  hihatpdhuf, tommid2huf, hihatophuf, tommid1huf, tomhi2huf,  cymcrsh1huf,    tomhi1huf,
    cymride1huf,    cymchinahuf,    cymbellhuf, tamborinhuf,    cymsplshhuf,    cowbellhuf, cymcrsh2huf,    vibslaphuf,

    cymride2huf,    bongohihuf, bongolohuf, congahi1huf,    congahi2huf,    congalohuf, timbalehhuf,    timbalelhuf,
    agogohihuf, agogolohuf, cabasahuf,  maracashuf, whistle1huf,    whistle2huf,    guiro1huf,  guiro2huf,
    clavehuf,   woodblk1huf,    woodblk2huf,    cuica1huf,  cuica2huf,  triangl1huf,    triangl2huf,    shakerhuf,
    jingleshuf, belltreehuf,    castinethuf,    surdo1huf,  surdo2,
};
#endif

#ifndef USE_ALL_TABS
const unsigned char* const pgm[128+61]=
{
    acpiano ,britepno ,epiano1 ,synpiano ,epiano1 ,epiano1 ,hrpschrd ,clavinet ,
    celeste ,glocken ,celeste ,celeste ,marimba ,celeste ,tubebell ,celeste ,
    harmonca ,harmonca ,harmonca ,harmonca ,harmonca ,harmonca ,harmonca ,harmonca ,
    nyguitar ,nyguitar ,jazzgtr ,jazzgtr ,mutegtr/*bad mutetrum*/ ,odguitar ,odguitar ,NULL ,

    acbass ,acbass ,acbass ,fretless ,fretless ,fretless ,fretless ,fretless ,
    pizzcato ,pizzcato ,pizzcato ,pizzcato ,pizzcato ,pizzcato ,pizzcato ,NULL ,
    slowstr ,slowstr ,synstr1/*bad synstr2*/ ,synstr2/*bad synstr2*/ ,synstr2/*bad synstr2*/ ,doo ,voices ,doo ,
    trumpet ,trumpet ,NULL ,mutetrum/*bad mutetrum*/ ,synbras2 ,synbras2 ,synbras2 ,synbras2 ,

    sprnosax ,altosax ,altosax ,sprnosax ,oboe ,NULL ,NULL ,clarinet ,
    whistle ,whistle ,whistle ,whistle ,whistle ,NULL ,whistle ,ocarina ,
    sqrwave ,sqrwave ,sqrwave ,sqrwave ,sqrwave ,sqrwave ,NULL ,NULL ,
    NULL ,NULL ,NULL ,NULL ,NULL ,NULL ,NULL ,NULL ,

    soundtrk ,soundtrk ,soundtrk ,soundtrk ,soundtrk ,soundtrk ,soundtrk ,soundtrk ,
    banjo ,banjo ,kalimba ,kalimba ,kalimba ,shannai ,shannai ,shannai ,
    carillon ,carillon ,carillon ,woodblk ,woodblk ,woodblk ,woodblk ,carillon ,
    NULL ,NULL ,NULL ,jungle/*added*/ ,NULL ,NULL ,NULL ,NULL ,

    sticks ,sticks ,sticks ,sticks ,sticks ,metclick ,metclick ,metclick ,
    kick1 ,kick1 ,sticks ,kick1 ,sticks ,kick1 ,kick1 ,hihatcl ,
    sticks ,sticks ,sticks ,sticks ,sticks ,sticks ,cymride1 ,sticks ,
    cymride1 ,cymride1 ,sticks ,tamborin ,cymride1 ,cowbell ,cymride1 ,cymride1 ,

    cymride1 ,bongohi ,bongohi ,congalo ,congalo ,congalo ,NULL ,NULL ,
    NULL ,NULL ,maracas ,maracas ,NULL ,NULL ,NULL ,NULL ,
    clave ,clave ,woodblk ,clave ,woodblk ,triangl1 ,NULL ,triangl1 ,
    NULL ,NULL ,NULL ,NULL ,NULL ,
};
#else
const unsigned char* const pgm[128+61]=
{
    acpiano,    britepno,   synpiano,   honky,  epiano1,    epiano2,    hrpschrd,   clavinet,
    celeste,    glocken,    musicbox,   vibes,  marimba,    xylophon,   tubebell,   santur,
    homeorg,    percorg,    rockorg,    church, reedorg,    accordn,    harmonca,   concrtna,
    nyguitar,   acguitar,   jazzgtr,    cleangtr,   mutegtr,    odguitar,   distgtr,    gtrharm,

    acbass, fngrbass,   pickbass,   fretless,   slapbas1,   slapbas2,   synbass1,   synbass2,
    violin, viola,  cello,  contraba,   tremstr,    pizzcato,   harp,   timpani,
    marcato,    slowstr,    synstr1,    synstr2,    choir,  doo,    voices, orchhit,
    trumpet,    trombone,   tuba,   mutetrum,   frenchrn,   hitbrass,   synbras1,   synbras2,

    sprnosax,   altosax,    tenorsax,   barisax,    oboe,   englhorn,   bassoon,    clarinet,
    piccolo,    flute,  recorder,   woodflut,   bottle, shakazul,   whistle,    ocarina,
    sqrwave,    sawwave,    calliope,   chiflead,   charang,    voxlead,    lead5th,    basslead,
    fantasia,   warmpad,    polysyn,    ghostie,    bowglass,   metalpad,   halopad,    sweeper,

    aurora, soundtrk,   crystal,    atmosphr,   freshair,   unicorn,    echovox,    startrak,
    sitar,  banjo,  shamisen,   koto,   kalimba,    bagpipes,   fiddle, shannai,
    carillon,   agogo,  steeldrm,   woodblk,    taiko,  toms,   syntom, revcym,
    fx_fret,    fx_blow,    seashore,   jungle, telephon,   helicptr,   applause,   pistol,

    highq,  slap,   scratch1,   scratch2,   sticks, sqrclick,   metclick,   metbell,
    kick1,  kick2,  stickrim,   snare1, claps,  snare2, tomlo2, hihatcl,
    tomlo1, hihatpd,    tommid2,    hihatop,    tommid1,    tomhi2, cymcrsh1,   tomhi1,
    cymride1,   cymchina,   cymbell,    tamborin,   cymsplsh,   cowbell,    cymcrsh2,   vibslap,

    cymride2,   bongohi,    bongolo,    congahi1,   congahi2,   congalo,    timbaleh,   timbalel,
    agogohi,    agogolo,    cabasa, maracas,    whistle1,   whistle2,   guiro1, guiro2,
    clave,  woodblk1,   woodblk2,   cuica1, cuica2, triangl1,   triangl2,   shaker,
    jingles,    belltree,   castinet,   surdo1, surdo2//,,,,
};
#endif

//note the repeat instrument, {num,...} mean same to same_pgm_flag[num], to self mean no repeat, 255 mean NULL,
//must match with pgm[128+61],
//woodblk can not reuse before 128. for 128 tonebank and follow 61 drumset.
//sheen
#ifndef USE_ALL_TABS
const unsigned char same_pgm_flag[128+61]=
{
    0 ,1 ,2 ,3 ,2 ,2 ,6 ,7 ,
    8 ,9 ,8 ,8 ,12 ,8 ,14 ,8 ,
    16 ,16 ,16 ,16 ,16 ,16 ,16 ,16 ,
    24 ,24 ,26 ,26 ,28/*bad 28*/ ,29 ,29 ,255 ,

    32 ,32 ,32 ,35 ,35 ,35 ,35 ,35 ,
    40 ,40 ,40 ,40 ,40 ,40 ,40 ,255 ,
    48 ,48 ,50/*bad2 50*/ ,51/*bad2 50*/ ,51/*bad2 50*/ ,53 ,54 ,53 ,
    56 ,56 ,255 ,59/*bad 28*/ ,60 ,60 ,60 ,60 ,

    64 ,65 ,65 ,64 ,68 ,255 ,255 ,71 ,
    72 ,72 ,72 ,72 ,72 ,255 ,72 ,79 ,
    80 ,80 ,80 ,80 ,80 ,80 ,255 ,255 ,
    255 ,255 ,255 ,255 ,255 ,255 ,255 ,255 ,

    96 ,96 ,96 ,96 ,96 ,96 ,96 ,96 ,
    104 ,104 ,106 ,106 ,106 ,109 ,109 ,109 ,
    112 ,112 ,112 ,115 ,115 ,115 ,115 ,112 ,
    255 ,255 ,255 ,123/*added*/ ,255 ,255 ,255 ,255 ,

    128 ,128 ,128 ,128 ,128 ,133 ,133 ,133 ,
    136 ,136 ,128 ,136 ,128 ,136 ,136 ,143 ,
    128 ,128 ,128 ,128 ,128 ,128 ,150 ,128 ,
    150 ,150 ,128 ,155 ,150 ,157 ,150 ,150 ,

    150 ,161 ,161 ,163 ,163 ,163 ,255 ,255 ,
    255 ,255 ,170 ,170 ,255 ,255 ,255 ,255 ,
    176 ,176 ,178 ,176 ,178 ,181 ,255 ,181 ,
    255 ,255 ,255 ,255 ,255 ,
};
#else
const unsigned char same_pgm_flag[128+61]=
{
    0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
    8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,
    16 ,17 ,18 ,19 ,20 ,21 ,22 ,23 ,
    24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,

    32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,
    40 ,41 ,42 ,43 ,44 ,45 ,46 ,47 ,
    48 ,49 ,50 ,51 ,52 ,53 ,54 ,55 ,
    56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,

    64 ,65 ,66 ,67 ,68 ,69 ,70 ,71 ,
    72 ,73 ,74 ,75 ,76 ,77 ,78 ,79 ,
    80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,
    88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,

    96 ,97 ,98 ,99 ,100 ,101 ,102 ,103 ,
    104 ,105 ,106 ,107 ,108 ,109 ,110 ,111 ,
    112 ,113 ,114 ,115 ,116 ,117 ,118 ,119 ,
    120 ,121 ,122 ,123 ,124 ,125 ,126 ,127 ,

    128 ,129 ,130 ,131 ,132 ,133 ,134 ,135 ,
    136 ,137 ,138 ,139 ,140 ,141 ,142 ,143 ,
    144 ,145 ,146 ,147 ,148 ,149 ,150 ,151 ,
    152 ,153 ,154 ,155 ,156 ,157 ,158 ,159 ,

    160 ,161 ,162 ,163 ,164 ,165 ,166 ,167 ,
    168 ,169 ,170 ,171 ,172 ,173 ,174 ,175 ,
    176 ,177 ,178 ,179 ,180 ,181 ,182 ,183 ,
    184 ,185 ,186 ,187 ,188 ,
};
#endif

#ifndef USE_TABS_8K
#ifndef USE_ALL_TABS
const short  libsize[128+61]=
{
    14041,  1741,   2087,   346,    2087,   2087,   292,    265,
    3155,   1678,   3155,   3155,   746,    3155,   1784,   3155,
    830,    830,    830,    830,    830,    830,    830,    830,
    664,    664,    1416,   1416,   769/*bad 845*/, 2532,   2532,   0,

    1999,   1999,   1999,   1757,   1757,   1757,   1757,   1757,
    1825,   1825,   1825,   1825,   1825,   1825,   1825,   0,
    5394,   5394,   8823/*bad 4800*/,   4800/*bad 4800*/,   4800/*bad 4800*/,   1043,   9680,   1043,
    886,    886,    0,  845/*bad 845*/, 7594,   7594,   7594,   7594,

    1001,   985,    985,    1001,   672,    0,  0,  823,
    1656,   1656,   1656,   1656,   1656,   0,  1656,   448,
    4123,   4123,   4123,   4123,   4123,   4123,   0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,

    5389,   5389,   5389,   5389,   5389,   5389,   5389,   5389,
    3311,   3311,   745,    745,    745,    1174,   1174,   1174,
    2045,   2045,   2045,   745,    745,    745,    745,    2045,
    0,  0,  0,  15449/*added*/, 0,  0,  0,  0,

    2010,   2010,   2010,   2010,   2010,   143,    143,    143,
    834,    834,    2010,   834,    2010,   834,    834,    836,
    2010,   2010,   2010,   2010,   2010,   2010,   7200,   2010,
    7200,   7200,   2010,   2468,   7200,   1387,   7200,   7200,

    7200,   1061,   1061,   1567,   1567,   1567,   0,  0,
    0,  0,  1195,   1195,   0,  0,  0,  0,
    692,    692,    745,    692,    745,    699,    0,  699,
    0,  0,  0,  0,  0,
};
#else
const short  libsize[128+61]=
{
    14041,  1741,   346,    6782,   2087,   9904,   292,    265,
    3155,   1678,   6931,   2871,   746,    2157,   1784,   7753,
    295,    929,    8477,   684,    486,    1587,   830,    1593,
    664,    4686,   1416,   3842,   769,    2532,   2363,   3229,

    1999,   2591,   10551,  1757,   4233,   4654,   7331,   3297,
    2448,   286,    1409,   1266,   8888,   1825,   1807,   2667,
    8882,   5394,   8823,   4800,   6192,   1043,   9680,   5979,
    886,    2934,   917,    845,    10158,  15889,  7244,   7594,

    1001,   985,    985,    1001,   672,    1033,   923,    823,
    1478,   9690,   730,    686,    3153,   7581,   1656,   448,
    4123,   3903,   10550,  7178,   3491,   9433,   1292,   1599,
    23983,  8171,   13682,  11733,  10083,  8270,   6116,   7538,

    20648,  5389,   10277,  8502,   15275,  36426,  9683,   13497,
    6025,   3311,   1820,   2755,   745,    1118,   844,    1174,
    2045,   2681,   3273,   745,    2480,   3217,   4473,   4680,
    3802,   16360,  31206,  15449,  1176,   5011,   17793,  9738,

    1136,   3020,   2645,   1820,   2010,   140,    143,    140,
    834,    1140,   1283,   4221,   2389,   2001,   3251,   836,
    3242,   1128,   3242,   6193,   3241,   3241,   13287,  3216,
    7200,   10966,  6910,   2468,   13284,  1387,   13156,  4194,

    7199,   1061,   1158,   1200,   1567,   1567,   2118,   2278,
    1140,   1141,   1998,   1195,   1270,   562,    1353,   2385,
    692,    691,    747,    2345,   2601,   699,    4324,   784,
    8403,   17099,  1219,   3255,   3252,
};
#endif
#else
#ifndef USE_ALL_TABS
const short  libsize[128+61]=
{
    8340,   984,    1248,   211,    1248,   1248,   169,    153,
    1913,   871,    1913,   1913,   418,    1913,   1026,   1913,
    480,    480,    480,    480,    480,    480,    480,    480,
    454,    454,    901,    901,    486,    1483,   1483,   0,

    1330,   1330,   1330,   1063,   1063,   1063,   1063,   1063,
    1159,   1159,   1159,   1159,   1159,   1159,   1159,   0,
    3009,   3009,   5351,   2662,   2662,   576,    5881,   576,
    540,    540,    0,  468,    4469,   4469,   4469,   4469,

    595,    550,    550,    595,    405,    0,  0,  528,
    978,    978,    978,    978,    978,    0,  978,    278,
    2372,   2372,   2372,   2372,   2372,   2372,   0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,

    3155,   3155,   3155,   3155,   3155,   3155,   3155,   3155,
    1998,   1998,   425,    425,    425,    697,    697,    697,
    1092,   1092,   1092,   469,    469,    469,    469,    1092,
    0,  0,  0,  8640,   0,  0,  0,  0,

    1072,   1072,   1072,   1072,   1072,   94, 94, 94,
    551,    551,    1072,   551,    1072,   551,    551,    435,
    1072,   1072,   1072,   1072,   1072,   1072,   3480,   1072,
    3480,   3480,   1072,   1192,   3480,   789,    3480,   3480,

    3480,   643,    643,    1007,   1007,   1007,   0,  0,
    0,  0,  612,    612,    0,  0,  0,  0,
    403,    403,    469,    403,    469,    354,    0,  354,
    0,  0,  0,  0,  0,
};
#else
const short  libsize[128+61]=
{
    8340,   984,    211,    3876,   1248,   5903,   169,    153,
    1913,   871,    4154,   1668,   418,    1260,   1026,   4683,
    213,    564,    4858,   402,    294,    891,    480,    886,
    454,    2909,   901,    2164,   486,    1483,   1335,   2021,

    1330,   1676,   3839,   1063,   2438,   2894,   4749,   2007,
    1425,   191,    874,    754,    5031,   1159,   1101,   1667,
    5028,   3009,   5351,   2662,   3556,   576,    5881,   3548,
    540,    1777,   598,    468,    5910,   9303,   4319,   4469,

    595,    550,    550,    595,    405,    610,    566,    528,
    811,    5543,   475,    412,    1797,   4455,   978,    278,
    2372,   2466,   6190,   4294,   2024,   5753,   815,    1023,
    14727,  4983,   8591,   7015,   6169,   4837,   3764,   4665,

    11549,  3155,   5899,   5159,   8791,   20826,  5886,   8117,
    3555,   1998,   1063,   1666,   425,    647,    522,    697,
    1092,   1447,   1873,   469,    1628,   2142,   2773,   2408,
    2155,   9065,   18649,  8640,   655,    2734,   9938,   5539,

    696,    1649,   1482,   992,    1072,   94, 94, 94,
    551,    719,    692,    2138,   1369,   1070,   2145,   435,
    2155,   553,    2155,   2999,   2154,   2154,   6870,   2141,
    3480,   5711,   3697,   1192,   6868,   789,    6838,   2341,

    3479,   643,    774,    768,    1007,   1007,   1251,   1424,
    659,    660,    1012,   612,    678,    306,    728,    1238,
    403,    402,    471,    1391,   1605,   354,    2250,   414,
    4028,   8011,   643,    2150,   2146,
};
#endif
#endif

MidToneBank *master_tonebank, *master_drumset;
const sint32 freq_table[128]=
{
    8176, 8662, 9177, 9723,
    10301, 10913, 11562, 12250,
    12978, 13750, 14568, 15434,

    16352, 17324, 18354, 19445,
    20602, 21827, 23125, 24500,
    25957, 27500, 29135, 30868,

    32703, 34648, 36708, 38891,
    41203, 43654, 46249, 48999,
    51913, 55000, 58270, 61735,

    65406, 69296, 73416, 77782,
    82407, 87307, 92499, 97999,
    103826, 110000, 116541, 123471,

    130813, 138591, 146832, 155563,
    164814, 174614, 184997, 195998,
    207652, 220000, 233082, 246942,

    261626, 277183, 293665, 311127,
    329628, 349228, 369994, 391995,
    415305, 440000, 466164, 493883,

    523251, 554365, 587330, 622254,
    659255, 698456, 739989, 783991,
    830609, 880000, 932328, 987767,

    1046502, 1108731, 1174659, 1244508,
    1318510, 1396913, 1479978, 1567982,
    1661219, 1760000, 1864655, 1975533,

    2093005, 2217461, 2349318, 2489016,
    2637020, 2793826, 2959955, 3135963,
    3322438, 3520000, 3729310, 3951066,

    4186009, 4434922, 4698636, 4978032,
    5274041, 5587652, 5919911, 6271927,
    6644875, 7040000, 7458620, 7902133,

    8372018, 8869844, 9397273, 9956063,
    10548082, 11175303, 11839822, 12543854
};

//my fixed point table
const short bend_fine_q13[256]=
{
    8192,8193,8195,8197,8199,8201,8203,8204,
    8206,8208,8210,8212,8214,8216,8217,8219,
    8221,8223,8225,8227,8229,8230,8232,8234,
    8236,8238,8240,8242,8243,8245,8247,8249,
    8251,8253,8255,8256,8258,8260,8262,8264,
    8266,8268,8270,8271,8273,8275,8277,8279,
    8281,8283,8284,8286,8288,8290,8292,8294,
    8296,8298,8299,8301,8303,8305,8307,8309,
    8311,8313,8314,8316,8318,8320,8322,8324,
    8326,8328,8329,8331,8333,8335,8337,8339,
    8341,8343,8344,8346,8348,8350,8352,8354,
    8356,8358,8360,8361,8363,8365,8367,8369,
    8371,8373,8375,8377,8378,8380,8382,8384,
    8386,8388,8390,8392,8394,8395,8397,8399,
    8401,8403,8405,8407,8409,8411,8413,8414,
    8416,8418,8420,8422,8424,8426,8428,8430,
    8432,8433,8435,8437,8439,8441,8443,8445,
    8447,8449,8451,8452,8454,8456,8458,8460,
    8462,8464,8466,8468,8470,8472,8474,8475,
    8477,8479,8481,8483,8485,8487,8489,8491,
    8493,8495,8496,8498,8500,8502,8504,8506,
    8508,8510,8512,8514,8516,8518,8520,8521,
    8523,8525,8527,8529,8531,8533,8535,8537,
    8539,8541,8543,8545,8546,8548,8550,8552,
    8554,8556,8558,8560,8562,8564,8566,8568,
    8570,8572,8574,8575,8577,8579,8581,8583,
    8585,8587,8589,8591,8593,8595,8597,8599,
    8601,8603,8605,8606,8608,8610,8612,8614,
    8616,8618,8620,8622,8624,8626,8628,8630,
    8632,8634,8636,8638,8640,8641,8643,8645,
    8647,8649,8651,8653,8655,8657,8659,8661,
    8663,8665,8667,8669,8671,8673,8675,8677,
};

const int bend_coarse_q16[128]=
{
    65536 ,   69432 ,   73561 ,   77935 ,   82570 ,   87480 ,   92681 ,   98193 ,
    104031 ,  110217 ,  116771 ,  123715 ,  131072 ,  138865 ,  147123 ,  155871 ,
    165140 ,  174960 ,  185363 ,  196386 ,  208063 ,  220435 ,  233543 ,  247430 ,
    262144 ,  277731 ,  294246 ,  311743 ,  330280 ,  349920 ,  370727 ,  392772 ,
    416127 ,  440871 ,  467087 ,  494861 ,  524288 ,  555463 ,  588493 ,  623487 ,
    660561 ,  699840 ,  741455 ,  785544 ,  832255 ,  881743 ,  934175 ,  989723 ,
    1048576 , 1110927 , 1176986 , 1246974 , 1321122 , 1399681 , 1482910 , 1571088 ,
    1664510 , 1763487 , 1868350 , 1979447 , 2097152 , 2221855 , 2353973 , 2493948 ,
    2642245 , 2799362 , 2965820 , 3142177 , 3329021 , 3526975 , 3736700 , 3958895 ,
    4194304 , 4443710 , 4707947 , 4987896 , 5284491 , 5598724 , 5931641 , 6284355 ,
    6658042 , 7053950 , 7473400 , 7917791 , 8388608 , 8887420 , 9415894 , 9975792 ,
    10568983 ,11197448 ,11863283 ,12568710 ,13316085 ,14107900 ,14946800 ,15835583 ,
    16777216 ,17774841 ,18831788 ,19951584 ,21137967 ,22394896 ,23726566 ,25137421 ,
    26632170 ,28215801 ,29893600 ,31671166 ,33554432 ,35549682 ,37663576 ,39903169 ,
    42275935 ,44789793 ,47453132 ,50274842 ,53264340 ,56431603 ,59787200 ,63342332 ,
    67108864 ,71099364 ,75327152 ,79806338 ,84551870 ,89579586 ,94906265 ,100549685 ,
};

const int bend_coarse_q7[128]=
{
    128 ,     134 ,     142 ,     152 ,     160 ,     170 ,     180 ,     190 ,
    202 ,     214 ,     228 ,     240 ,     256 ,     270 ,     286 ,     304 ,
    322 ,     340 ,     362 ,     382 ,     406 ,     430 ,     456 ,     482 ,
    512 ,     542 ,     574 ,     608 ,     644 ,     682 ,     724 ,     766 ,
    812 ,     860 ,     912 ,     966 ,    1024 ,    1084 ,    1148 ,    1216 ,
    1290 ,    1366 ,    1448 ,    1534 ,    1624 ,    1722 ,    1824 ,    1932 ,
    2048 ,    2168 ,    2298 ,    2434 ,    2580 ,    2732 ,    2896 ,    3068 ,
    3250 ,    3444 ,    3648 ,    3866 ,    4096 ,    4338 ,    4596 ,    4870 ,
    5160 ,    5466 ,    5792 ,    6136 ,    6500 ,    6888 ,    7298 ,    7732 ,
    8192 ,    8678 ,    9194 ,    9740 ,   10320 ,   10934 ,   11584 ,   12274 ,
    13002 ,   13776 ,   14596 ,   15464 ,   16384 ,   17358 ,   18390 ,   19482 ,
    20642 ,   21870 ,   23170 ,   24548 ,   26006 ,   27554 ,   29192 ,   30928 ,
    32768 ,   34716 ,   36780 ,   38966 ,   41284 ,   43740 ,   46340 ,   49096 ,
    52014 ,   55108 ,   58384 ,   61856 ,   65536 ,   69432 ,   73560 ,   77934 ,
    82570 ,   87480 ,   92680 ,   98192 ,  104030 ,  110216 ,  116770 ,  123714 ,
    131072 ,  138864 ,  147122 ,  155870 ,  165140 ,  174960 ,  185362 ,  196386 ,
};

const short sinx_q15[64]= //(sine(i*16)*32768.0)
{
    0 ,    3211 ,    6392 ,    9512 ,   12539 ,   15446 ,   18204 ,   20787 ,
    23170 ,   25330 ,   27245 ,   28898 ,   30273 ,   31357 ,   32138 ,   32610 ,
    32767 ,   32610 ,   32138 ,   31357 ,   30273 ,   28898 ,   27245 ,   25330 ,
    23170 ,   20787 ,   18204 ,   15446 ,   12539 ,    9512 ,    6392 ,    3211 ,
    0 ,   -3211 ,   -6392 ,   -9512 ,  -12539 ,  -15446 ,  -18204 ,  -20787 ,
    -23170 ,  -25330 ,  -27245 ,  -28898 ,  -30273 ,  -31357 ,  -32138 ,  -32610 ,
    -32767 ,  -32610 ,  -32138 ,  -31357 ,  -30273 ,  -28898 ,  -27245 ,  -25330 ,
    -23170 ,  -20787 ,  -18204 ,  -15446 ,  -12539 ,   -9512 ,   -6392 ,   -3211 ,
};

const short sinx_basic_q15[257]= //(sine(i)*(double)(1<<15))
{
    0 ,     201 ,     402 ,     603 ,     804 ,    1005 ,    1206 ,    1407 ,
    1607 ,    1808 ,    2009 ,    2210 ,    2410 ,    2611 ,    2811 ,    3011 ,
    3211 ,    3411 ,    3611 ,    3811 ,    4011 ,    4210 ,    4409 ,    4609 ,
    4808 ,    5006 ,    5205 ,    5403 ,    5602 ,    5800 ,    5997 ,    6195 ,
    6392 ,    6589 ,    6786 ,    6983 ,    7179 ,    7375 ,    7571 ,    7766 ,
    7961 ,    8156 ,    8351 ,    8545 ,    8739 ,    8933 ,    9126 ,    9319 ,
    9512 ,    9704 ,    9896 ,   10087 ,   10278 ,   10469 ,   10659 ,   10849 ,
    11039 ,   11228 ,   11416 ,   11605 ,   11793 ,   11980 ,   12167 ,   12353 ,
    12539 ,   12725 ,   12910 ,   13094 ,   13278 ,   13462 ,   13645 ,   13828 ,
    14010 ,   14191 ,   14372 ,   14552 ,   14732 ,   14912 ,   15090 ,   15269 ,
    15446 ,   15623 ,   15800 ,   15976 ,   16151 ,   16325 ,   16499 ,   16673 ,
    16846 ,   17018 ,   17189 ,   17360 ,   17530 ,   17700 ,   17869 ,   18037 ,
    18204 ,   18371 ,   18537 ,   18703 ,   18868 ,   19032 ,   19195 ,   19358 ,
    19519 ,   19681 ,   19841 ,   20001 ,   20159 ,   20318 ,   20475 ,   20631 ,
    20787 ,   20942 ,   21097 ,   21250 ,   21403 ,   21555 ,   21706 ,   21856 ,
    22005 ,   22154 ,   22301 ,   22448 ,   22594 ,   22740 ,   22884 ,   23027 ,
    23170 ,   23312 ,   23453 ,   23593 ,   23732 ,   23870 ,   24007 ,   24144 ,
    24279 ,   24414 ,   24547 ,   24680 ,   24812 ,   24943 ,   25073 ,   25201 ,
    25330 ,   25457 ,   25583 ,   25708 ,   25832 ,   25955 ,   26077 ,   26199 ,
    26319 ,   26438 ,   26557 ,   26674 ,   26790 ,   26905 ,   27020 ,   27133 ,
    27245 ,   27356 ,   27466 ,   27576 ,   27684 ,   27791 ,   27897 ,   28002 ,
    28106 ,   28208 ,   28310 ,   28411 ,   28511 ,   28609 ,   28707 ,   28803 ,
    28898 ,   28993 ,   29086 ,   29178 ,   29269 ,   29359 ,   29447 ,   29535 ,
    29621 ,   29707 ,   29791 ,   29874 ,   29956 ,   30037 ,   30117 ,   30196 ,
    30273 ,   30350 ,   30425 ,   30499 ,   30572 ,   30644 ,   30714 ,   30784 ,
    30852 ,   30919 ,   30985 ,   31050 ,   31114 ,   31176 ,   31237 ,   31298 ,
    31357 ,   31414 ,   31471 ,   31526 ,   31581 ,   31634 ,   31685 ,   31736 ,
    31785 ,   31834 ,   31881 ,   31927 ,   31971 ,   32015 ,   32057 ,   32098 ,
    32138 ,   32176 ,   32214 ,   32250 ,   32285 ,   32319 ,   32351 ,   32383 ,
    32413 ,   32442 ,   32469 ,   32496 ,   32521 ,   32545 ,   32568 ,   32589 ,
    32610 ,   32629 ,   32647 ,   32663 ,   32679 ,   32693 ,   32706 ,   32718 ,
    32728 ,   32737 ,   32745 ,   32752 ,   32758 ,   32762 ,   32765 ,   32767 ,
    32767 ,
};

const short vol_table_q12[128]=
{
    64 ,      66 ,      68 ,      70 ,      72 ,      75 ,      77 ,      80 ,
    83 ,      85 ,      88 ,      91 ,      94 ,      97 ,     101 ,     104 ,
    108 ,     111 ,     115 ,     119 ,     123 ,     127 ,     131 ,     135 ,
    140 ,     145 ,     149 ,     154 ,     160 ,     165 ,     170 ,     176 ,
    182 ,     188 ,     194 ,     201 ,     208 ,     214 ,     222 ,     229 ,
    237 ,     245 ,     253 ,     261 ,     270 ,     279 ,     288 ,     298 ,
    308 ,     318 ,     329 ,     340 ,     351 ,     363 ,     375 ,     387 ,
    400 ,     413 ,     427 ,     441 ,     456 ,     471 ,     487 ,     503 ,
    520 ,     537 ,     555 ,     574 ,     593 ,     613 ,     633 ,     654 ,
    676 ,     698 ,     722 ,     746 ,     770 ,     796 ,     823 ,     850 ,
    878 ,     908 ,     938 ,     969 ,    1001 ,    1035 ,    1069 ,    1105 ,
    1142 ,    1180 ,    1219 ,    1260 ,    1301 ,    1345 ,    1390 ,    1436 ,
    1484 ,    1533 ,    1584 ,    1637 ,    1691 ,    1748 ,    1806 ,    1866 ,
    1928 ,    1992 ,    2059 ,    2127 ,    2198 ,    2271 ,    2347 ,    2425 ,
    2506 ,    2589 ,    2675 ,    2765 ,    2857 ,    2952 ,    3050 ,    3151 ,
    3256 ,    3365 ,    3477 ,    3593 ,    3712 ,    3836 ,    3964 ,    4096 ,
};

#if 0
const size_t  pool_slop[MIDIPOOL_NUMPOOLS] =
{
    //16000,            /* first PERMANENT pool */
    //16000         /* first IMAGE pool */
    0,  /*sheen*/
    0,  /*sheen*/
    1200 /**/
};
midi_pool_ptr small_list[MIDIPOOL_NUMPOOLS];
#endif
///////////////////////////////////////////////////////////////

int      midi_huf_max_value[17], midi_valoffset[17];
unsigned int      *midi_huffucode;//[256]
const int    midi_And[33]= {0,1,3,7,0xf,0x1f,0x3f,0x7f,0xff,0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff,0xffff,
                            0x1ffff,0x3ffff,0x7ffff,0xfffff,0x1fffff,0x3fffff,0x7fffff,0xffffff,0x1ffffff,0x3ffffff,
                            0x7ffffff,0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff,
                           };
#define MIDI_DEBUG_MODE
char *  midi_chunkptr=NULL;
char *  midi_chunk_end=NULL;
int       midi_current_byte = 0;
int       midi_current_bit = 0;
const unsigned char * midi_code_len_table=NULL;
const unsigned char * midi_code_value_table=NULL;
MidSong *song1=NULL;
UINT8    midi_g_loop = 0;
UINT8    midi_g_buffer = 0;

#ifdef MIDI_DEBUG_MODE
size_t total_mem_count1 = 0;
#endif

/////////////////////////////////////////////////////////////////
sample_t *resample_buffer;
sint32 resample_buffer_count;
char *pResample_buffer;//sheen
sint32     *common_buffer;
//#define WRITE_PCM_FILE
//#define MIDBUFFERSIZE   (2304*2)//4608*2 use only half of the pcmbuf_overlay buffer. sheen
#define MIDBUFFERSIZE   (1024)//same as sbc encode frame. sheen
//#define WRIET_PCM_OUTDATA
static U8 USE_CHUNK_BUF;// 1=read buffer,0= read file,
#define FILE_TEMP_CHUNK_SIZE 256//every chunk buffer size for file read.sheen
static MidEventListShort *pEvlistShort;//max=MAX_TRACK ([0,1,2...15] sign track num)
static MidEventListShort *pEvlistShortHead;//first of the list pos
static MidEventListShort *pEvlistShortTail;//last of the list pos for write.
static MidEvent eventTemp;//store one event
static MidEvent eventTemp2;//store one event
//static uint32 *pTrkPos;//every track current file position.
static uint8 trksParseOverFlag;//0=open and first read track. 1=parse over.
static int32 trackAppend;
static uint32 lastEvtTrkNum;

static int32 current_bank[16], current_set[16], current_program[16];
static sint32 our_event_count;
static sint32 counting_time=2; /* We strip any silence before the first NOTE ON. */
static sint32 sample_cum,at, st;
static sint32 at_per_trk[MAX_TRACK];//at per track, sheen
static sint32 max_instrument_num;//max instrument malloc num, <=MAX_TRACK

static uint8 *pgm_use_flag;//128+61 note the same instrument have use or not, num=which repeat have used, 255=not yet

typedef struct
{
    uint8 laststatus;
    uint8 lastchan;
    uint8 nrpn;
    uint8 rpn_msb[16];/* one per channel */
    uint8 rpn_lsb[16]; /* one per channel */
} EVT_READ_STRU;

static EVT_READ_STRU *pEvtRedParm;//MAX_TRACK
//static uint32 firstTrkPos;//first trak position in file.

typedef struct
{
    uint32 trkSize;//track size.
    uint32 trkStart;//track start position in file.
    uint32 trkPos;//current track read position in file.
    sint8* chunk;//chunk buffer start position.
    sint8* pchunk;//current read position in chunk buffer.
    sint32 byteLeft;//byte left in chunk buffer

} TRK_READ_STRU;

static TRK_READ_STRU *pTrkRedParm;//MAX_TRACK
static uint8 trkReadFlag;//read track num in file, 255=not track
static uint32 mid_dec_t;//test sheen
static uint32 mid_dec_num;//test sheen

//static sint8*  chunk= NULL;
//static sint8*  pchunk = NULL;
//static sint32 chunkLen;
//static INT32 byteLeftInChunk=0;
static BOOL bt_state;//for bt A2DP,sheen
static BOOL sco_state;//for bt sco,sheen
static AUD_ITF_T last_audioItf;
static INT32 *G_MidSBCConstY=NULL;
extern const INT32 G_VppSBCConstX[];
static INT32 voc_data_remain;
vpp_AudioJpeg_DEC_OUT_T Voc_AudioDecStatus;
vpp_AudioJpeg_DEC_IN_T Voc_AudioDecIN;
static BOOL skip_vocopen_isr;


#ifndef SHEEN_VC_DEBUG
extern U16 AnsiiToUnicodeString(S8 *pOutBuffer, S8 *pInBuffer );
extern PUBLIC UINT32 hal_TimGetUpTime(VOID);
extern sint32  mult32_r1(sint32 x,sint32 y,sint32 r);
#endif

static size_t mid_read_array (void *ptr, size_t size);
static MidEvent *get_one_event(MidSong *song, sint32 divisions, uint32 *trackNum);
static int read_track(MidIStream stream, MidSong *song, int append, uint32 trackNum);
static int32 mid_stream_seek(MidIStream stream, size_t size, int32 origin);

static inline int div_320_320(sint32 divd,sint32 divs)
{
    return divd/divs;
}

#ifndef SHEEN_VC_DEBUG
//for sbc encode
void MMC_MidSbcVocISR(void)
{
    //diag_printf("voc done");
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MID_SBC_VOC);
}

/*********************************/
//hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);
//aud_StreamStop(audioItf);
//hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
//mmc_MemFreeAll();
void MMC_MidiHalfPcmISR(void)
{
    Mmc_Midi_Pcm_Half = 0;
    //DEBUG_MSG ("[MCI_AUDIO_MIDI]HALF!  Mmc_Midi_Pcm_Half=%d\n",Mmc_Midi_Pcm_Half);
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MIDI_PLAY);

}

/*
* NAME: MMC_AudioPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/
void MMC_MidiEndPcmISR(void)
{
    Mmc_Midi_Pcm_Half = 1;
    //DEBUG_MSG ("[MCI_AUDIO_MIDI]END  Mmc_Midi_Pcm_Half=%d\n",Mmc_Midi_Pcm_Half);
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MIDI_PLAY);

}

extern void GpioSpeakerModeStart()  ;
void MMC_MidiPcmOutStart(void*  startbufaddr)
{
    // Stream out
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;

    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);
    audioStream.startAddress  = (UINT32*)startbufaddr;
    audioStream.length        = (MIDBUFFERSIZE)*2;//MP3DEC_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
    audioStream.sampleRate    = HAL_AIF_FREQ_16000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
    audioStream.channelNb     = HAL_AIF_MONO;//HAL_AIF_STEREO;//HAL_AIF_MONO;
    audioStream.voiceQuality         = !TRUE;    // To adjust with a key
    audioStream.playSyncWithRecord = FALSE;
    audioStream.halfHandler   = MMC_MidiHalfPcmISR;
    audioStream.endHandler    = MMC_MidiEndPcmISR;

    if(audioItf!=AUD_ITF_EAR_PIECE && audioItf!=AUD_ITF_LOUD_SPEAKER && audioItf!=AUD_ITF_BLUETOOTH)
    {
        // Initial cfg
        audioItf = AUD_ITF_EAR_PIECE;
    }

    if(MpegPlayer.MpegInput.bt_sco==TRUE)
    {
        audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
        audioStream.channelNb     = HAL_AIF_MONO;//HAL_AIF_STEREO;//HAL_AIF_MONO;
        audioItf=AUD_ITF_BLUETOOTH;
        audioStream.voiceQuality   = TRUE;
    }

    {
        UINT32 errStatus =  aud_StreamStart(audioItf, &audioStream, &audio_cfg);
        last_audioItf = audioItf;
        DEBUG_MSG ( "MMC_MidiPcmOutStart errStatus = %d audioItf=%d", errStatus,audioItf);
    }

    Mmc_Midi_Pcm_Half = 1;


}


MCI_ERR_T MMC_MidiDecClose (void)
{
    aud_StreamStop(audioItf);

    midi_free_all();//mmc_MemFreeAll();
    /*  if (FS_Close(input->fileHandle) < 0)
        {
            diag_printf("[MMC_AUDIO]##WARNING##fail to close file!handle: %d \n", input->fileHandle);
            return MCI_ERR_ERROR;
        }   */
    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
    vpp_AudioJpegDecClose();

    if(mid_dec_num)
    {
        DEBUG_MSG("midi decode half pcm buffer(104ms) avg time=%d ms",mid_dec_t*1000/(mid_dec_num*16384));
    }

    return  MCI_ERR_NO;

}
#endif

PUBLIC MCI_ERR_T MMC_MidGetFileInformation (
    CONST INT32 FileHander,
    AudDesInfoStruct  * CONST DecInfo  )
{
    DecInfo->sampleRate = HAL_AIF_FREQ_16000HZ;
    DecInfo->stereo = 1;

    diag_printf("MMC_MidGetFileInformation sampleRate %d , stereo %d\n", DecInfo->sampleRate,DecInfo->stereo );
    return MCI_ERR_NO;
}


//#endif
MCI_ERR_T MMC_MidiVocSbcOpen(void)
{
    HAL_ERR_T voc_ret = 0;
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    //bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;
    voc_data_remain=0;
    //for A2DP
    bt_state= is_SSHdl_valid(my_ss_handle);
    //if((bt_state= is_SSHdl_valid(my_ss_handle)) && (my_audio_config_handle->codec_type == 1))      //BT_A2DP_sbc
    {
        //open voc always for bt state change.
        memset(&Voc_AudioDecStatus,0,sizeof(vpp_AudioJpeg_DEC_OUT_T));
        memset(&Voc_AudioDecIN,0,sizeof(vpp_AudioJpeg_DEC_IN_T));
        Voc_AudioDecIN.EQ_Type=-1;
        Voc_AudioDecIN.reset=1;

        G_MidSBCConstY = (INT32*)mmc_MemMalloc(468*4);
        if( G_MidSBCConstY==NULL)
        {
            //mmc_MemFreeAll();
            return MCI_ERR_OUT_OF_MEMORY;
        }

        skip_vocopen_isr=TRUE;
        /*open voc decoder*/
        voc_ret = vpp_AudioJpegDecOpen((HAL_VOC_IRQ_HANDLER_T)MMC_MidSbcVocISR);

        switch (voc_ret)
        {
            case HAL_ERR_NO:
                break;

            case HAL_ERR_RESOURCE_RESET:
            case HAL_ERR_RESOURCE_BUSY:
            case HAL_ERR_RESOURCE_TIMEOUT:
            case HAL_ERR_RESOURCE_NOT_ENABLED:
            case HAL_ERR_BAD_PARAMETER:
            case HAL_ERR_UART_RX_OVERFLOW:
            case HAL_ERR_UART_TX_OVERFLOW:
            case HAL_ERR_UART_PARITY:
            case HAL_ERR_UART_FRAMING:
            case HAL_ERR_UART_BREAK_INT:
            case HAL_ERR_TIM_RTC_NOT_VALID:
            case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
            case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
            case HAL_ERR_COMMUNICATION_FAILED:
            case HAL_ERR_QTY:
                diag_printf("Fail to Call vpp_AudioJpegDecOpen(): %d \n", voc_ret);
                return MCI_ERR_ERROR;
            default:
                break;
        }
        diag_printf("vpp_AudioJpegDecOpen ok!");
    }

    return MCI_ERR_NO;
}

MCI_ERR_T MMC_MidiVocSbcEnc(char *pcmbuf,int pcmbufsize,MidSong *song)
{
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;
    bt_a2dp_sbc_codec_cap_struct *sbc_config_handle =  &my_audio_config_handle->codec_cap.sbc;

    if(!(is_SSHdl_valid(my_ss_handle)) || my_audio_config_handle->codec_type != 1)//BT_A2DP_sbc
        return MCI_ERR_NO;

#if 0// enable volume gain or not
    {
        int bt_music_gain,i;
        short *pcm_p=(short *)pcmbuf;

        switch(audio_cfg.spkLevel)
        {
            case SND_SPK_MUTE:
                bt_music_gain=0;//mute
                break;
            case SND_SPK_VOL_1:
                bt_music_gain=0x80;//-18db
                break;
            case SND_SPK_VOL_2:
                bt_music_gain=0xb5;//-15db
                break;
            case SND_SPK_VOL_3:
                bt_music_gain=0x100;//-12db
                break;
            case SND_SPK_VOL_4:
                bt_music_gain=0x16a;//-9db
                break;
            case SND_SPK_VOL_5:
                bt_music_gain=0x200;//-6db
                break;
            case SND_SPK_VOL_6:
                bt_music_gain=0x2d4;//-3db
                break;
            case SND_SPK_VOL_7:
                bt_music_gain=0x400;//0db
                break;
            default:
                bt_music_gain=0x400;//0db
                break;

        }

        for( i=0; i<(pcmbufsize/2); i++)
        {
            pcm_p[i]=(pcm_p[i]*bt_music_gain)>>10;
        }

    }
#endif

    memcpy( (UINT8*) hal_VocGetPointer(OUTPUT_PCM_BUFFER1_ADDR),  pcmbuf, pcmbufsize);//pcm data

    Voc_AudioDecIN.mode=MMC_SBC_ENC;
    Voc_AudioDecIN.audioItf=audioItf;
    Voc_AudioDecIN.BsbcEnable = 1;

    {
        int mode =SBC_MODE_JOINT_STEREO;

        if( sbc_config_handle->channel_mode == 0x01 )
            mode = SBC_MODE_JOINT_STEREO; // joint stereo
        else if( sbc_config_handle->channel_mode == 0x02 )
            mode = SBC_MODE_STEREO; // stereo
        else if( sbc_config_handle->channel_mode == 0x04 )
            mode = SBC_MODE_DUAL_CHANNEL; // dual
        else if( sbc_config_handle->channel_mode == 0x08 )
            mode= SBC_MODE_MONO; // mono
        else
            diag_printf("Error SBC channel set! default stero.\n");
        Voc_AudioDecIN.Source_width = mode;//sbc chl

        if( sbc_config_handle->alloc_method  == 0x01 )
            Voc_AudioDecIN.Source_height= SBC_AM_LOUDNESS;//
        else if( sbc_config_handle->alloc_method  == 0x02 )
            Voc_AudioDecIN.Source_height= SBC_AM_SNR;
        else
            diag_printf("Error SBC alloc_method set! default AM_LOUDNESS\n");

        Voc_AudioDecIN.Cut_width =    SBC_BIT_POOL;
        Voc_AudioDecIN.Zoomed_width = ((song->encoding & PE_MONO) ? 1 : 2);//pcm channels
        Voc_AudioDecIN.Zoomed_height = song->rate;  //pcm SampleRate
        Voc_AudioDecIN.inStreamUBufAddr = (INT32*)G_VppSBCConstX;
        Voc_AudioDecIN.inStreamVBufAddr = G_MidSBCConstY;
    }

    vpp_AudioJpegDecScheduleOneFrame(&Voc_AudioDecIN);
    //diag_printf("start SBC enc.\n");
    return MCI_ERR_NO;

}


//int32 rd_t,sk_t;//test sheen

MIDIDEC_ERROR_TYPE  MMC_Midi2Raw(HANDLE  file_stream,int filesize,int32 PlayProgress)
{
    INT32 rate = HAL_AIF_FREQ_16000HZ;//22050;//32000;
    INT32 bits = 16;
    INT32 volume = 60;
    int channels = 1;//2;
    MidSongOptions options;
#ifndef SHEEN_VC_DEBUG
    //for sco
    if(MpegPlayer.MpegInput.bt_sco==TRUE)
    {
        rate = 8000;
        diag_printf("BT SCO TRUE.\n");
    }
    sco_state= MpegPlayer.MpegInput.bt_sco;
#endif
    //rd_t=0;
    //sk_t=0;

    USE_CHUNK_BUF=0;
    pEvlistShort=0;
    trksParseOverFlag=0;
    trkReadFlag=255;
    mid_dec_t=0;
    mid_dec_num=1;

    if(g_MidiBT8KEnbleFlag==TRUE)
    {
        rate = 8000;
        channels = 1;

    }

#ifdef WRIET_PCM_OUTDATA
    HANDLE  output;
    char inname[520]="D:/mygm_simple/output.pcm";
    uint8 tmpArrya[128]="/t";
#endif

    diag_printf("***MMC_Midi2Raw***  file_stream=%x\n",file_stream);

#ifdef MIDI_DEBUG_MODE
    total_mem_count1 = 0;
#endif

#ifdef WRIET_PCM_OUTDATA
    strcat(tmpArrya,inname+2);
    AnsiiToUnicodeString(inname,tmpArrya);
//creat output file
#if 1
    if ((output = FS_Open(inname, FS_O_RDWR, 0))>=0)
    {
        diag_printf("[record]append operation,seek to file end,handle:%d\n",inname);
    }
    else
    {
        output = FS_Create(inname, 0);
        diag_printf("creat new hfile=%0x\n",inname);
        if (output<0)
        {
            diag_printf("creat file fail!\n");
            return -1;
        }
    }
#else
    if ((output = FS_Open(inname, FS_O_RDONLY, 0))>=0)
    {
        diag_printf("open pcm file\n",inname);
    }
    else
    {
        diag_printf("open pcm  file fail!\n");
    }
#endif

#endif
    if(mid_init_no_config())
    {
        midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
        MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
        mmc_SetCurrMode(MMC_MODE_IDLE);

#endif
        return MIDIDEC_ERROR_OUTOF_SPACE;
    }
    midi_g_loop = 0;
    midi_g_buffer = 0;


    options.rate = rate;
    options.format = (bits == 16) ? MID_AUDIO_S16LSB : MID_AUDIO_S8;
    options.channels = channels;
    options.buffer_size = MIDBUFFERSIZE / (bits * channels / 8);
#ifndef SHEEN_VC_DEBUG
//int32 tt1=hal_TimGetUpTime();
#endif

    song1 = mid_song_load ((MidIStream)file_stream, &options);
    //midi_FCloseFile(stream);//mid_istream_close (stream);
#ifndef SHEEN_VC_DEBUG
//int32 tt2=hal_TimGetUpTime();
//  diag_printf("mid_song_load time=%d ",(tt2-tt1)*1000/16384);
#endif
    if (song1 == NULL)
    {
        DEBUG_MSG ("Invalid MIDI file\n");
        //mmc_MemFreeAll();
        //hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
        //mid_exit ();
        midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
        MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
        mmc_SetCurrMode(MMC_MODE_IDLE);
#endif
        return MIDIDEC_ERROR_FORMAT;
    }

    //trksParseOverFlag=1;

    mid_song_set_volume (song1, volume);
    mid_song_start (song1);

    {
        size_t     bytes_read;

        if(PlayProgress)
        {
            PlayProgress =(INT32)( ((INT64)PlayProgress*song1->samples)/10000);
            skip_to(song1, PlayProgress);
        }
#ifndef SHEEN_VC_DEBUG
        bytes_read = mid_song_read_wave (song1,  (INT32*)(pcmbuf_overlay), MIDBUFFERSIZE*2);//decode data to fill all the buffer

        if(bytes_read)
        {
            if(MMC_MidiVocSbcOpen() != MCI_ERR_NO)
            {
                midi_free_all();
                MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
                mmc_SetCurrMode(MMC_MODE_IDLE);
                return MIDIDEC_ERROR_FORMAT;
            }
            //enc half of pcmbuf_overlay for sbc.
            //MMC_MidiVocSbcEnc(pcmbuf_overlay,MIDBUFFERSIZE,song1);

            if(!(is_SSHdl_valid(MpegPlayer.MpegInput.ss_handle)))
            {
                //start pcm play.(for bt sbc only a time line)
                MMC_MidiPcmOutStart( (INT32*)(pcmbuf_overlay));
                DEBUG_MSG("***********MMC_MidiPcmOutStart***************\n");
            }
            else
            {
                Mmc_Midi_Pcm_Half = 0;
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MIDI_PLAY);
                diag_printf("start bt A2DP play.");
            }

            return MIDIDEC_ERROR_NO;
        }
        else
        {
            midi_free_all();//mmc_MemFreeAll();
            MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
            mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
            return MIDIDEC_ERROR_FORMAT;
        }
#else
        return MIDIDEC_ERROR_NO;
#endif
    }
}

MIDIDEC_ERROR_TYPE  MMC_Midi2RawBuffer(INT32 *pBuffer,  INT32 len, UINT8 loop)
{
    INT32 rate = HAL_AIF_FREQ_16000HZ;//22050;//32000;
    INT32 bits = 16;
    INT32 volume = 60;
    int channels=1;//2;

    MidSongOptions options;
#ifndef SHEEN_VC_DEBUG
    if(MpegPlayer.MpegInput.bt_sco==TRUE)
    {
        rate = 8000;
    }
    sco_state= MpegPlayer.MpegInput.bt_sco;
#endif

    USE_CHUNK_BUF=1;
    //byteLeftInChunk=0;
    pEvlistShort=0;
    trksParseOverFlag=0;
    trkReadFlag=255;
    mid_dec_t=0;
    mid_dec_num=1;

    DEBUG_MSG("MMC_Midi2RawBuffer");

#ifdef MIDI_DEBUG_MODE
    total_mem_count1 = 0;
#endif

    if(mid_init_no_config())
    {
        midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
        if(MCI_RingFinished!=NULL)
            MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
        mmc_SetCurrMode(MMC_MODE_IDLE);
#endif
        return MIDIDEC_ERROR_OUTOF_SPACE;
    }

    midi_g_loop = loop;
    midi_g_buffer = 1;
    pTrkRedParm[0].byteLeft=len;
    pTrkRedParm[0].pchunk=pTrkRedParm[0].chunk = (unsigned char*)pBuffer;
    if(pTrkRedParm[0].chunk==NULL)
    {
        midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
        if(MCI_RingFinished!=NULL)
            MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
        mmc_SetCurrMode(MMC_MODE_IDLE);
#endif
        return  MIDIDEC_ERROR_OUTOF_SPACE;
    }
#ifndef SHEEN_VC_DEBUG
    mmc_EntryMode(MMC_MODE_AUDIO,MCI_TYPE_MIDI);
#endif
    //buffer = (INT32*)(pcmbuf_overlay);

    //copybytes=0;
    //firsttime=1;

    options.rate = rate;
    options.format = (bits == 16) ? MID_AUDIO_S16LSB : MID_AUDIO_S8;
    options.channels = channels;
    options.buffer_size = MIDBUFFERSIZE / (bits * channels / 8);

    song1 = mid_song_load (0, &options);
    //midi_FCloseFile(stream);//mid_istream_close (stream);

    if (song1 == NULL)
    {
        DEBUG_MSG ("Invalid MIDI file\n");
        //mmc_MemFreeAll();
        //hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
        //mid_exit ();
        midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
        if(MCI_RingFinished!=NULL)
            MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
        mmc_SetCurrMode(MMC_MODE_IDLE);
#endif
        return MIDIDEC_ERROR_FORMAT;
    }

    mid_song_set_volume (song1, volume);
    mid_song_start (song1);

    {
        size_t     bytes_read;


        bytes_read = mid_song_read_wave (song1,  (INT32*)(pcmbuf_overlay), MIDBUFFERSIZE*2);//decode data to fill all the buffer

        if(bytes_read)
        {
#ifndef SHEEN_VC_DEBUG
            if(MMC_MidiVocSbcOpen() != MCI_ERR_NO)
            {
                midi_free_all();
                MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
                mmc_SetCurrMode(MMC_MODE_IDLE);
                return MIDIDEC_ERROR_FORMAT;
            }

            //MMC_MidiPcmOutStart( (INT32*)(pcmbuf_overlay));
            if(!(is_SSHdl_valid(MpegPlayer.MpegInput.ss_handle)))
            {
                //start pcm play.(for bt sbc only a time line)
                MMC_MidiPcmOutStart( (INT32*)(pcmbuf_overlay));
                DEBUG_MSG("***********MMC_MidiPcmOutStart***************\n");
            }
            else
            {
                Mmc_Midi_Pcm_Half = 0;
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MIDI_PLAY);
                diag_printf("start bt A2DP play.");
            }
#endif
            DEBUG_MSG("***********MMC_MidiPcmOutStart***************\n");
            return MIDIDEC_ERROR_NO;
        }
        else
        {
            midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
            if(MCI_RingFinished!=NULL)
                MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
            mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
#endif
            return MIDIDEC_ERROR_FORMAT;
        }
    }

}

////detailed function defination/////////////////
int mid_init_no_config()
{
    /* Allocate memory for the standard tonebank and drumset */
    int i;

    master_tonebank = midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidToneBank));
    if(master_tonebank==NULL)
        return -1;
    memset(master_tonebank, 0, sizeof(MidToneBank));
    master_tonebank->tone = midi_alloc_small(MIDIPOOL_PERMANENT,128 * sizeof(MidToneBankElement));
    if(master_tonebank->tone==NULL)
        return -1;
    memset(master_tonebank->tone, 0, 128 * sizeof(MidToneBankElement));

    master_drumset = midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidToneBank));
    if(master_drumset==NULL)
        return -1;
    memset(master_drumset, 0, sizeof(MidToneBank));
    master_drumset->tone = midi_alloc_small(MIDIPOOL_PERMANENT,128 * sizeof(MidToneBankElement));
    if(master_drumset->tone==NULL)
        return -1;
    memset(master_drumset->tone, 0, 128 * sizeof(MidToneBankElement));

    midi_huffucode=(unsigned int*)midi_alloc_small(MIDIPOOL_PERMANENT,1024 );
    if(midi_huffucode==NULL)
        return -1;
    memset(midi_huffucode,0,1024);

    if(USE_CHUNK_BUF==0)
    {
        pTrkRedParm=(TRK_READ_STRU*)midi_alloc_small(MIDIPOOL_PERMANENT,MAX_TRACK*(sizeof(TRK_READ_STRU)+FILE_TEMP_CHUNK_SIZE));
        if(pTrkRedParm==NULL)
            return -1;
        memset(pTrkRedParm,0,MAX_TRACK*(sizeof(TRK_READ_STRU)+FILE_TEMP_CHUNK_SIZE));

        pTrkRedParm[0].pchunk=pTrkRedParm[0].chunk= ((sint8*)pTrkRedParm)+ MAX_TRACK*sizeof(TRK_READ_STRU);

        for (i=1; i<MAX_TRACK; i++)
        {
            pTrkRedParm[i].pchunk= pTrkRedParm[i].chunk= pTrkRedParm[i-1].chunk+ FILE_TEMP_CHUNK_SIZE;
        }
    }
    else
    {
        //no chunk buffer
        pTrkRedParm=(TRK_READ_STRU*)midi_alloc_small(MIDIPOOL_PERMANENT,MAX_TRACK*sizeof(TRK_READ_STRU));
        if(pTrkRedParm==NULL)
            return -1;
        memset(pTrkRedParm,0,MAX_TRACK*sizeof(TRK_READ_STRU));
    }

    for (i=0; i<128; i++)
    {
        master_tonebank->tone[i].note=master_tonebank->tone[i].amp=master_tonebank->tone[i].pan=
                                          master_tonebank->tone[i].strip_loop=master_tonebank->tone[i].strip_envelope=
                                                  master_tonebank->tone[i].strip_tail=-1;
    }

    for (i=0; i<128; i++)
    {
        master_drumset->tone[i].note=master_drumset->tone[i].amp=master_drumset->tone[i].pan=
                                         master_drumset->tone[i].strip_loop=master_drumset->tone[i].strip_envelope=
                                                 master_drumset->tone[i].strip_tail=-1;
    }

    //in this project we only use sound lib from gm,the following is the according parameters in the gm lib
    {
        master_tonebank->tone[5].amp=80;
        master_tonebank->tone[6].amp=50;
        master_tonebank->tone[7].amp=110;

        master_tonebank->tone[9].amp=80;

        master_tonebank->tone[20].amp=80;
        master_tonebank->tone[22].amp=70;

        master_tonebank->tone[27].amp=80;
        master_tonebank->tone[29].amp=75;
        master_tonebank->tone[30].amp=120;
        master_tonebank->tone[31].amp=70;

        master_tonebank->tone[32].amp=120;
        master_tonebank->tone[33].amp=120;
        master_tonebank->tone[34].amp=120;
        master_tonebank->tone[35].amp=120;
        master_tonebank->tone[36].amp=120;
        master_tonebank->tone[37].amp=120;
        master_tonebank->tone[38].amp=120;
        master_tonebank->tone[39].amp=120;

        master_tonebank->tone[40].amp=70;
        master_tonebank->tone[41].amp=80;
        master_tonebank->tone[45].amp=80;
        master_tonebank->tone[47].amp=120;

        master_tonebank->tone[52].amp=130;

        master_tonebank->tone[56].amp=120;
        master_tonebank->tone[57].amp=80;
        master_tonebank->tone[59].amp=60;
        master_tonebank->tone[60].amp=110;

        master_tonebank->tone[64].amp=80;
        master_tonebank->tone[65].amp=90;
        master_tonebank->tone[66].amp=70;

        master_tonebank->tone[119].amp=80;
        master_tonebank->tone[119].strip_loop=0;


        master_drumset->tone[28].amp=70;
        master_drumset->tone[42].amp=60;
        master_drumset->tone[44].amp=60;
        master_drumset->tone[46].amp=60;
        master_drumset->tone[80].amp=30;
        master_drumset->tone[81].amp=30;

    }

    return 0;
}


MidSong *mid_song_load_dls(MidIStream stream, MidSongOptions *options)
{
    MidSong *song;
//  int i=0;

// if (stream == NULL)
//     return NULL;

    /* Allocate memory for the song */
    song = (MidSong *)midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(*song));
    if(song==NULL)
        return NULL;
    memset(song, 0, sizeof(*song));

    if (master_tonebank)
    {
        song->tonebank = master_tonebank;
    }
    if (master_drumset)
    {
        song->drumset = master_drumset;
    }

    song->fileStrm=stream;//sheen

    song->amplification = DEFAULT_AMPLIFICATION;
    song->voices = DEFAULT_VOICES;
    song->drumchannels = DEFAULT_DRUMCHANNELS;

    song->rate = options->rate;
    song->encoding = 0;
    if ((options->format & 0xFF) == 16)
        song->encoding |= PE_16BIT;
    if (options->format & 0x8000)
        song->encoding |= PE_SIGNED;
    if (options->channels == 1)
        song->encoding |= PE_MONO;
    switch (options->format)
    {
        case MID_AUDIO_S8:
            song->write = s32tos8;
            break;
        case MID_AUDIO_U8:
            song->write = s32tou8;
            break;
        case MID_AUDIO_S16LSB:
            song->write = s32tos16l;
            break;
        case MID_AUDIO_S16MSB:
            song->write = s32tos16b;
            break;
        case MID_AUDIO_U16LSB:
            song->write = s32tou16l;
            break;
        default:
            DEBUG_MSG("Unsupported audio format\n");
            song->write = s32tou16l;
            break;
    }

    pgm_use_flag=midi_alloc_small(MIDIPOOL_PERMANENT,128+61);
    if(pgm_use_flag==NULL)
    {
        DEBUG_MSG("pgm_use_flag malloc err!\n");
        return NULL;
    }
    memset(pgm_use_flag,255,128+61);

    song->buffer_size = options->buffer_size;
    //resample_buffer = safe_malloc(options->buffer_size * sizeof(sample_t));
    common_buffer = midi_alloc_small(MIDIPOOL_PERMANENT,options->buffer_size * (2 * sizeof(sint32)+sizeof(sample_t)));
    if(common_buffer==NULL)
    {
        return NULL;
    }
    resample_buffer = (sample_t*)((sint8*)common_buffer + options->buffer_size * 2 * sizeof(sint32));
    resample_buffer_count= options->buffer_size;
    song->bytes_per_sample =
        ((song->encoding & PE_MONO) ? 1 : 2)
        * ((song->encoding & PE_16BIT) ? 2 : 1);

    song->control_ratio = options->rate / CONTROLS_PER_SECOND;
    if (song->control_ratio < 1)
        song->control_ratio = 1;
    else if (song->control_ratio > MAX_CONTROL_RATIO)
        song->control_ratio = MAX_CONTROL_RATIO;

    song->lost_notes = 0;
    song->cut_notes = 0;

#ifndef SHEEN_VC_DEBUG
//int32 tt1=hal_TimGetUpTime();
#endif
    song->events = read_midi_file(stream, song);
#ifndef SHEEN_VC_DEBUG
//int32 tt2=hal_TimGetUpTime();
//  diag_printf("read_midi_file time=%d",(tt2-tt1)*1000/16384);
#endif
    /* Make sure everything is okay */
    if (!song->events)
    {
        //midi_freespace(song);
        return(NULL);
    }

    //byteLeftInChunk=0;

//midi_freespace(chunk);
    song->default_instrument = 0;
    song->default_program = DEFAULT_PROGRAM;

#ifndef SHEEN_VC_DEBUG
//tt1=hal_TimGetUpTime();
#endif

    max_instrument_num=0;//sheen
    load_missing_instruments(song);


#ifndef SHEEN_VC_DEBUG
//tt2=hal_TimGetUpTime();
//  diag_printf("load_missing_instruments time=%d",(tt2-tt1)*1000/16384);
#endif
    return(song);
}

MidSong *mid_song_load(MidIStream stream, MidSongOptions *options)
{
    return mid_song_load_dls(stream, options);
}

//huffuman decode part////
void  midi_make_huffutable()
{
    int p, i, l, numsymbols;
    int code;

    unsigned int si;

    //size
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        i = midi_code_len_table[l-1];
        while (i--)
            midi_huffucode[p++] = l;
    }
    midi_huffucode[p] = 0;
    numsymbols = p;
    //code
    code = 0;
    p = 0;
    si = midi_huffucode[0];
    while (midi_huffucode[p])
    {
        while (( midi_huffucode[p]) == si)
        {
            midi_huffucode[p++] = code;
            code++;
        }
        code <<= 1;
        si++;
    }

    //max  valoffset
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        if (midi_code_len_table[l-1])
        {
            midi_valoffset[l] = p - midi_huffucode[p];
            p += midi_code_len_table[l-1];
            midi_huf_max_value[l] = midi_huffucode[p-1]; /* maximum code of length l */
        }
        else
        {
            midi_huf_max_value[l] = -1; /* -1 if no codes of this length */
        }
    }

}

int  midi_ReadByte()
{
    int i;

    if(midi_chunkptr >= midi_chunk_end)
    {
        return 257;
    }
    i=*(midi_chunkptr++);

    midi_current_bit+=8;

    return i;
}

int midi_DecodeElement()
{
    int nb;
    int code;
    unsigned short valux;

    while(midi_current_bit < 16)
    {
        if ((code=midi_ReadByte())==257)
        {
            return 257;
        }
        midi_current_byte = midi_current_byte<<8 |(code&0xff) ;
        midi_current_byte=midi_current_byte&midi_And[midi_current_bit];
    }

    nb = 1;
    midi_current_bit-=nb;
    code=(midi_current_byte >> midi_current_bit);
    midi_current_byte=midi_current_byte & midi_And[midi_current_bit];
    while (code > midi_huf_max_value[nb])
    {
        code <<= 1;
        midi_current_bit--;
        code |= midi_current_byte >>midi_current_bit;
        midi_current_byte=midi_current_byte & midi_And[midi_current_bit];
        nb++;

    }

    valux = midi_code_value_table[code+midi_valoffset[nb]];

    return valux;
}

int  get_last()
{

    int nb;
    int code;
    unsigned short valux;

    nb = 1;
    midi_current_bit-=nb;
    code=(midi_current_byte >> midi_current_bit);
    midi_current_byte=midi_current_byte & midi_And[midi_current_bit];
    while (code > midi_huf_max_value[nb])
    {
        code <<= 1;
        midi_current_bit--;
        if (midi_current_bit < 0)
        {
            return 257;
        }
        code |= midi_current_byte >>midi_current_bit;
        midi_current_byte=midi_current_byte & midi_And[midi_current_bit];
        nb++;
    }

    valux = midi_code_value_table[code+midi_valoffset[nb]];

    return valux;
}


MidInstrument *load_instrument_from_array(MidSong *song, int num, int percussion,
        int panning, int amp, int note_to_use,
        int strip_loop, int strip_envelope,
        int strip_tail, MidInstrument *pReUseInstrument)
{

    MidInstrument *ip;
    MidSample *sp;
    int j/*,noluck=0*/;
    int  code,count;
    sint32 datalen=0;
    char history_code;

    /************************************************************************/
    /* huffuman decode                                                      */
    /************************************************************************/
//open huffuman data table to get code_len_table[] and code_value_table[]


    if (phuf[num]==NULL)
    {
        DEBUG_MSG("***instrument lib array %d is NULL*** \n",num);
        return 0;
    }


    midi_code_len_table = phuf[num];
    midi_code_value_table = midi_code_len_table+16;
    /* Open patch file */
    midi_chunkptr  = (sint8*)pgm[num];
    midi_chunk_end = (sint8*)midi_chunkptr + libsize[num];
    midi_current_bit = 0;
    midi_current_byte=0;
    midi_make_huffutable();

    count = 0;
    //pchunk = (unsigned char *)resample_buffer; sheen
    pResample_buffer= (unsigned char *)resample_buffer;
    while ((code=midi_DecodeElement())!=257 && count++<94)
    {
        //*pchunk++ = (sint8)code;//fwrite(&code,1,1,f1); sheen
        *pResample_buffer++ = (sint8)code;
    }
    //pchunk = (unsigned char *)resample_buffer; sheen
    pResample_buffer = (unsigned char *)resample_buffer;


    /* Read some headers and do cursory sanity checks. There are loads
    of magic offsets. This could be rewritten... */
    ip=midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidInstrument));

    if(ip==NULL)
        return 0;
    ip->samples = 1;


    ip->sample = midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidSample) * ip->samples);
    if(ip->sample==NULL)
        return 0;
    //for (i=0; i<ip->samples; i++)
    {
        sp=ip->sample;

        //if (mid_istream_read(0,&sp->data_length,4,1)<0) sheen
        if (mid_read_array(&sp->data_length,4)<0)
        {
            return 0;
        }
        datalen = sp->data_length;

        //if (mid_istream_read(0,&sp->loop_start,4,1)<0) sheen
        if (mid_read_array(&sp->loop_start,4)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->loop_end,4,1)<0)
        if (mid_read_array(&sp->loop_end,4)<0)
        {
            return 0;
        }

        memset(&sp->sample_rate,0,4);
        //if (mid_istream_read(0,&sp->sample_rate,2,1)<0) sheen
        if (mid_read_array(&sp->sample_rate,2)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->low_freq,4,1)<0)  sheen
        if (mid_read_array(&sp->low_freq,4)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->high_freq,4,1)<0)  sheen
        if (mid_read_array(&sp->high_freq,4)<0)
        {
            return 0;
        }
        // if (mid_istream_read(0,&sp->root_freq,4,1)<0)  sheen
        if (mid_read_array(&sp->root_freq,4)<0)
        {
            return 0;
        }
        sp->low_vel = 0;
        sp->high_vel = 127;
        // if (mid_istream_read(0,&sp->panning,1,1)<0)  sheen
        if (mid_read_array(&sp->panning,1)<0)
        {
            return 0;
        }

        /* envelope, tremolo, and vibrato */
        //if (mid_istream_read(0,&sp->tremolo_sweep_increment,4,1)<0)  sheen
        if (mid_read_array(&sp->tremolo_sweep_increment,4)<0)
        {
            return 0;
        }

        //if (mid_istream_read(0,&sp->tremolo_phase_increment,4,1)<0)  sheen
        if (mid_read_array(&sp->tremolo_phase_increment,4)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->tremolo_depth,1,1)<0)  sheen
        if (mid_read_array(&sp->tremolo_depth,1)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->vibrato_control_ratio,4,1)<0)  sheen
        if (mid_read_array(&sp->vibrato_control_ratio,4)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->vibrato_sweep_increment,4,1)<0) sheen
        if (mid_read_array(&sp->vibrato_sweep_increment,4)<0)
        {
            return 0;
        }
        //if (mid_istream_read(0,&sp->vibrato_depth,1,1)<0)  sheen
        if (mid_read_array(&sp->vibrato_depth,1)<0)
        {
            DEBUG_MSG("Error reading vibrato_depth\n");
            return 0;
        }

        /* Mark this as a fixed-pitch instrument if such a deed is desired. */
        if (note_to_use!=-1)
            sp->note_to_use=(uint8)(note_to_use);
        else
            sp->note_to_use=0;

        //if (mid_istream_read(0,&sp->modes,1,1)<0)  sheen
        if (mid_read_array(&sp->modes,1)<0)
        {
            return 0;
        }

        for (j=0; j<6; j++)
        {
            //if (mid_istream_read(0,&sp->envelope_rate[j],4,1)<0)  sheen
            if (mid_read_array(&sp->envelope_rate[j],4)<0)
            {
                return 0;
            }

            //if (mid_istream_read(0,&sp->envelope_offset[j],4,1)<0)  sheen
            if (mid_read_array(&sp->envelope_offset[j],4)<0)
            {
                return 0;
            }

        }


        /* Then read the sample data */
        sp->data_length = (((uint32)(sp->data_length))>>FRACTION_BITS);

        if(!pReUseInstrument)
        {
            sp->data = midi_alloc_small(MIDIPOOL_PERMANENT,sp->data_length*2+4);
            if(sp->data ==NULL)
                return 0;
            //huffuman deocde and 8-bit -> 16-bit
            {
                uint16*  pd = (uint16*)sp->data;
                history_code = (char)code;
                *pd++ = (uint16)(code) << 8;//*pd++ = (sint8)code;//fwrite(&code,1,1,f1);

                while ((code=midi_DecodeElement())!=257)
                {
                    code=(char)code+history_code;
                    history_code = (char)code;
                    *pd++ = (uint16)(code) << 8;//*pchunk++ = (sint8)code;//fwrite(&code,1,1,f1);
                }
                while (midi_current_bit > 0)
                {
                    code = get_last();
                    if (code==257)
                    {
                        break;
                    }
                    code=(char)code+history_code;
                    history_code = (char)code;
                    *pd++ = (uint16)(code) << 8;//*pchunk++ = (sint8)code;//fwrite(&code,1,1,f1);
                }

                *pd = *(pd-1);
                *(pd+1) = *pd;
                sp->data_length *= 2;
            }
        }
        else//reuse instrument data, sheen
        {
            sp->data=pReUseInstrument->sample->data;
            sp->data_length *= 2;
        }

        if (amp!=-1)
        {
            sp->volume_fix = (amp<<5) + (amp<<3) + amp;
        }
        else
        {
            /* Try to determine a volume scaling factor for the sample.
            This is a very crude adjustment, but things sound more
                balanced with it. Still, this should be a runtime option. */
            sint32 ii=sp->data_length/2;
            sint16 maxamp=0,a;
            sint16 *tmp=(sint16 *)sp->data;
            while (ii--)
            {
                a=*tmp++;
                if (a<0) a=-a;
                if (a>maxamp)
                    maxamp=a;
            }
            //sp->volume=(float)(32768.0 / maxamp);
            //sp->volume_fix = div_320_320_lshift(1<<(12+15),maxamp,0);
            sp->volume_fix = div_320_320(1<<(12+15),maxamp);
            //DEBUG_MSG(" * volume comp: %d\n", sp->volume_fix);
        }


        sp->data_length =datalen;

        /* If this instrument will always be played on the same note,
        and it's not looped, we can resample it now. */
        //if (sp->note_to_use && !(sp->modes & MODES_LOOPING))
        //pre_resample(song, sp);

        if (strip_tail==1)
        {
            /* Let's not really, just say we did. */
            //DEBUG_MSG(" - Stripping tail\n");
            sp->data_length = sp->loop_end;
        }
    }

    return ip;
}


static int fill_bank(MidSong *song, int dr, int b)
{

    int i, errors=0;
    MidToneBank *bank=((dr) ? song->drumset/*[b]*/ : song->tonebank/*[b]*/);
    uint8 same_pgm_num;//the base one but may not the first use one, sheen
    uint8 used_pgm_num;//the first used in the same sequence, sheen
    int32 num;//sheen
    MidInstrument *pReUseInstrument;//sheen

    /************************************************************************/
    //there should be 128 kinds of drumsets song->drumset[0]-song->drumset[127],and 128 kinds of tonebanks song->tonebank[0]-song->tonebank[127]
    //  in this project we only use sound lib from gm,the following is the detailed sound samples in the gm lib
    //  In this gm lib only exit song->drumset[0] and song->tonebank[0],and song->tonebank[0] contains 128tonebanks and song->drumset[0] contains 61 drumsets
    //  I merge them into one table called "tonename"
    /************************************************************************/
    DEBUG_MSG("fill_bank %d\n",dr);
    if (!bank)
    {
        DEBUG_MSG("Huh. Tried to load instruments in non-existent %s %d\n",
                  (dr) ? "drumset" : "tone bank", b);
        return 0;
    }

    for (i=0; i<128; i++)
    {
        if(bank->instrument[i]==MAGIC_LOAD_INSTRUMENT)
        {
            DEBUG_MSG("instrument num %d\n",i);
            bank->instrument[i]=0;

            //if(dr==0)continue;//test


            if (dr==1 && (i<27||i>87))
            {
                bank->instrument[i]=0;
                DEBUG_MSG("*****dr==1 && (i<27||i>87)******");
                continue;
            }

            //reuse the same instrument for reduce memory,sheen
            num=i+(dr)*101;
            same_pgm_num=same_pgm_flag[num];
            pReUseInstrument=0;
            if(same_pgm_num!=255)//255=NULL
            {
                //not itself && get real used num,255=not used yet
                if(same_pgm_num!=num && (used_pgm_num=pgm_use_flag[same_pgm_num])!=255)
                {
                    pReUseInstrument=bank->instrument[used_pgm_num-(dr)*101];
                    DEBUG_MSG("reuse data %d \n", used_pgm_num-(dr)*101);
                }
                else
                {
                    max_instrument_num+=1;//add one data malloc.
                    if(max_instrument_num > MAX_PGM_MLC)//max instrument malloc, sheen
                    {
                        DEBUG_MSG("up to max instrument malloc num=%d > 16 !!\n",max_instrument_num);
                        continue;
                    }
                    pgm_use_flag[same_pgm_num]=num;//mark the real used num in base.
                    if(same_pgm_num!=num)
                        pgm_use_flag[num]=num;//for memory free
                }

            }
            //end,sheen

            if (!(bank->instrument[i] =load_instrument_from_array(song,num,//bank->tone[i].name,
                                       (dr) ? 1 : 0,bank->tone[i].pan,bank->tone[i].amp,
                                       (bank->tone[i].note!=-1) ? bank->tone[i].note :((dr) ? i : -1),
                                       (bank->tone[i].strip_loop!=-1) ? bank->tone[i].strip_loop : ((dr) ? 1 : -1),
                                       (bank->tone[i].strip_envelope != -1) ? bank->tone[i].strip_envelope :((dr) ? 1 : -1),
                                       bank->tone[i].strip_tail, pReUseInstrument)))
            {
                DEBUG_MSG("Couldn't load instrument %d (%s %d, program %d)\n",
                          num,(dr)? "drum set" : "tone bank", b, i);
                errors++;
                if(same_pgm_num!=255)
                {
                    pgm_use_flag[same_pgm_num]=255;
                    if(!pReUseInstrument)
                        max_instrument_num-=1;//malloc fail.
                }

            }

        }
        else
            bank->instrument[i]=0;

    }


    return errors;
}

int load_missing_instruments(MidSong *song)
{
    int errors=0;

    errors+=fill_bank(song,0,0);
    errors+=fill_bank(song,1,0);

    return errors;
}

/* Returns 1 if envelope runs out */
int recompute_envelope(MidSong *song, int v)
{
    int stage;

    stage = song->voice[v].envelope_stage;

    if (stage>5)
    {
        /* Envelope ran out. */
        song->voice[v].status = VOICE_FREE;
        return 1;
    }

    if (song->voice[v].sample->modes & MODES_ENVELOPE)
    {
        if (song->voice[v].status==VOICE_ON || song->voice[v].status==VOICE_SUSTAINED)
        {
            if (stage>2)
            {
                /* Freeze envelope until note turns off. Trumpets want this. */
                song->voice[v].envelope_increment=0;
                return 0;
            }
        }
    }
    song->voice[v].envelope_stage=stage+1;

    if (song->voice[v].envelope_volume==song->voice[v].sample->envelope_offset[stage])
        return recompute_envelope(song, v);
    song->voice[v].envelope_target = song->voice[v].sample->envelope_offset[stage];
    song->voice[v].envelope_increment = song->voice[v].sample->envelope_rate[stage];
    if (song->voice[v].envelope_target < song->voice[v].envelope_volume)
        song->voice[v].envelope_increment = -song->voice[v].envelope_increment;
    return 0;
}

void apply_envelope_to_amp(MidSong *song, int v)
{

    //sint32 la,ra;
    sint32 lamp_fix=song->voice[v].lamp_fix,ramp_fix;


    if (song->voice[v].panned == PANNED_MYSTERY)
    {

        ramp_fix = song->voice[v].ramp_fix;

        if (song->voice[v].tremolo_phase_increment)
        {
            lamp_fix = mult32_r1(lamp_fix,song->voice[v].tremolo_volume_fix,12);//Q12
            ramp_fix = mult32_r1(ramp_fix,song->voice[v].tremolo_volume_fix,12);//Q12
        }

        if (song->voice[v].sample->modes & MODES_ENVELOPE)
        {

            lamp_fix = mult32_r1(lamp_fix,vol_table_q12[song->voice[v].envelope_volume>>23],12);//Q12
            ramp_fix = mult32_r1(ramp_fix,vol_table_q12[song->voice[v].envelope_volume>>23],12);//Q12
        }

        if (lamp_fix>MAX_AMP_VALUE)
            lamp_fix=MAX_AMP_VALUE;
        if (ramp_fix>MAX_AMP_VALUE)
            ramp_fix=MAX_AMP_VALUE;

        song->voice[v].left_mix  = lamp_fix;
        song->voice[v].right_mix = ramp_fix;

    }
    else
    {
        if (song->voice[v].tremolo_phase_increment)
        {

            lamp_fix = mult32_r1(lamp_fix,song->voice[v].tremolo_volume_fix,12);//Q12
        }


        if (song->voice[v].sample->modes & MODES_ENVELOPE)
        {
            lamp_fix = mult32_r1(lamp_fix,vol_table_q12[song->voice[v].envelope_volume>>23],12);//Q12
        }


        if (lamp_fix>MAX_AMP_VALUE)
            lamp_fix=MAX_AMP_VALUE;

        song->voice[v].left_mix  = lamp_fix;

    }
}

static int update_envelope(MidSong *song, int v)
{
    song->voice[v].envelope_volume += song->voice[v].envelope_increment;
    /* Why is there no ^^ operator?? */
    if (((song->voice[v].envelope_increment < 0) &&
            (song->voice[v].envelope_volume <= song->voice[v].envelope_target)) ||
            ((song->voice[v].envelope_increment > 0) &&
             (song->voice[v].envelope_volume >= song->voice[v].envelope_target)))
    {
        song->voice[v].envelope_volume = song->voice[v].envelope_target;
        if (recompute_envelope(song, v))
            return 1;
    }
    return 0;
}

static void update_tremolo(MidSong *song, int v)
{
    sint32 depth = song->voice[v].sample->tremolo_depth << 7;
    sint32 t1,t2,t3;


    if (song->voice[v].tremolo_sweep)
    {
        /* Update sweep position */

        song->voice[v].tremolo_sweep_position += song->voice[v].tremolo_sweep;
        if (song->voice[v].tremolo_sweep_position >= (1 << SWEEP_SHIFT))
            song->voice[v].tremolo_sweep=0; /* Swept to max amplitude */
        else
        {
            /* Need to adjust depth */
            depth *= song->voice[v].tremolo_sweep_position;
            depth >>= SWEEP_SHIFT;
        }
    }

    song->voice[v].tremolo_phase += song->voice[v].tremolo_phase_increment;

    {

        t1 = (song->voice[v].tremolo_phase >> RATE_SHIFT) & 0xff;// t1 = (i) & 0xff;//
        t2 = (song->voice[v].tremolo_phase >> RATE_SHIFT) >> 8 ;//t2 = (i) >> 8 ;//
        t2 = t2 & 0x3 ;
        if (t2>=0 && t2<1)
        {
            t3 = sinx_basic_q15[t1];
        }
        else if(t1==0 && t2== 1)
        {
            t3 = sinx_basic_q15[256];
        }
        else if (t2>=1 && t2<2)
        {
            t3 = sinx_basic_q15[256-t1];
        }
        else if (t2>=2 && t2 <3)
        {
            t3 = -sinx_basic_q15[t1];
        }
        else if (t2==3 && t1==0)
        {
            t3 = -sinx_basic_q15[256];
        }
        else
        {
            t3 = -sinx_basic_q15[256-t1];
        }

        //tmp = (sint32)(sine(i)*(float)(1<<15));

        //if (tmp!=t3)
        //{
        //      printf("stop");
        //}
    }
    t3+=(1<<15);
    t3 = mult32_r1(t3,depth,17+3);
    t3 = (1<<12) - t3;
    song->voice[v].tremolo_volume_fix = (sint16)t3;


    /* I'm not sure about the +1.0 there -- it makes tremoloed voices'
       volumes on average the lower the higher the tremolo amplitude. */
}

/* Returns 1 if the note died */
static int update_signal(MidSong *song, int v)
{
    if (song->voice[v].envelope_increment && update_envelope(song, v))
        return 1;

    if (song->voice[v].tremolo_phase_increment)
        update_tremolo(song, v);

    apply_envelope_to_amp(song, v);
    return 0;
}

#define MIXATION(a) *lp++ += (a)*s;

static void mix_mystery_signal(MidSong *song, sample_t *sp, sint32 *lp, int v,
                               int count)
{
    MidVoice *vp = song->voice + v;
    final_volume_t
    left=vp->left_mix,
    right=vp->right_mix;
    int cc;
    sample_t s;


    if (!(cc = vp->control_counter))
    {
        cc = song->control_ratio;
        if (update_signal(song, v))
            return; /* Envelope ran out */
        left = vp->left_mix;
        right = vp->right_mix;
    }

    while (count)
        if (cc < count)
        {
            count -= cc;
            while (cc--)
            {
                s = *sp++;
                MIXATION(left);
                MIXATION(right);

            }
            cc = song->control_ratio;
            if (update_signal(song, v))
                return;   /* Envelope ran out */
            left = vp->left_mix;
            right = vp->right_mix;
        }
        else
        {
            vp->control_counter = cc - count;
            while (count--)
            {
                s = *sp++;
                MIXATION(left);
                MIXATION(right);

            }
            return;
        }
}

static void mix_center_signal(MidSong *song, sample_t *sp, sint32 *lp, int v,
                              int count)
{
    MidVoice *vp = song->voice + v;
    final_volume_t
    left=vp->left_mix;
    int cc;
    sample_t s;

    if (!(cc = vp->control_counter))
    {
        cc = song->control_ratio;
        if (update_signal(song, v))
            return; /* Envelope ran out */
        left = vp->left_mix;
    }

    while (count)
        if (cc < count)
        {
            count -= cc;
            while (cc--)
            {
                s = *sp++;
                MIXATION(left);
                MIXATION(left);
            }
            cc = song->control_ratio;
            if (update_signal(song, v))
                return;   /* Envelope ran out */
            left = vp->left_mix;
        }
        else
        {
            vp->control_counter = cc - count;
            while (count--)
            {
                s = *sp++;
                MIXATION(left);
                MIXATION(left);
            }
            return;
        }
}

static void mix_single_signal(MidSong *song, sample_t *sp, sint32 *lp, int v,
                              int count)
{
    MidVoice *vp = song->voice + v;
    final_volume_t
    left=vp->left_mix;
    int cc;
    sample_t s;

    if (!(cc = vp->control_counter))
    {
        cc = song->control_ratio;
        if (update_signal(song, v))
            return; /* Envelope ran out */
        left = vp->left_mix;
    }

    while (count)
        if (cc < count)
        {
            count -= cc;
            while (cc--)
            {
                s = *sp++;
                MIXATION(left);
                lp++;
            }
            cc = song->control_ratio;
            if (update_signal(song, v))
                return;   /* Envelope ran out */
            left = vp->left_mix;
        }
        else
        {
            vp->control_counter = cc - count;
            while (count--)
            {
                s = *sp++;
                MIXATION(left);
                lp++;
            }
            return;
        }
}

static void mix_mono_signal(MidSong *song, sample_t *sp, sint32 *lp, int v,
                            int count)
{
    MidVoice *vp = song->voice + v;
    final_volume_t
    left=vp->left_mix;
    int cc;
    sample_t s;

    if (!(cc = vp->control_counter))
    {
        cc = song->control_ratio;
        if (update_signal(song, v))
            return; /* Envelope ran out */
        left = vp->left_mix;
    }

    while (count)
        if (cc < count)
        {
            count -= cc;
            while (cc--)
            {
                s = *sp++;
                MIXATION(left);
            }
            cc = song->control_ratio;
            if (update_signal(song, v))
                return;   /* Envelope ran out */
            left = vp->left_mix;
        }
        else
        {
            vp->control_counter = cc - count;
            while (count--)
            {
                s = *sp++;
                MIXATION(left);
            }
            return;
        }
}

static void mix_mystery(MidSong *song, sample_t *sp, sint32 *lp, int v, int count)
{
    final_volume_t
    left = song->voice[v].left_mix,
    right = song->voice[v].right_mix;
    sample_t s;

    while (count--)
    {
        s = *sp++;
        MIXATION(left);
        MIXATION(right);
    }
}

static void mix_center(MidSong *song, sample_t *sp, sint32 *lp, int v, int count)
{
    final_volume_t
    left = song->voice[v].left_mix;
    sample_t s;

    while (count--)
    {
        s = *sp++;
        MIXATION(left);
        MIXATION(left);
    }
}

static void mix_single(MidSong *song, sample_t *sp, sint32 *lp, int v, int count)
{
    final_volume_t
    left = song->voice[v].left_mix;
    sample_t s;

    while (count--)
    {
        s = *sp++;
        MIXATION(left);
        lp++;
    }
}

static void mix_mono(MidSong *song, sample_t *sp, sint32 *lp, int v, int count)
{
    final_volume_t
    left = song->voice[v].left_mix;
    sample_t s;

    while (count--)
    {
        s = *sp++;
        MIXATION(left);
    }
}

/* Ramp a note out in c samples */
static void ramp_out(MidSong *song, sample_t *sp, sint32 *lp, int v, sint32 c)
{

    /* should be final_volume_t, but uint8 gives trouble. */
    sint32 left, right, li, ri;

    sample_t s=0; /* silly warning about uninitialized s */

    /* Fix by James Caldwell */
    if ( c == 0 ) c = 1;

    left=song->voice[v].left_mix;
    li=-(left/c);
    if (!li) li=-1;

    /* printf("Ramping out: left=%d, c=%d, li=%d\n", left, c, li); */

    if (!(song->encoding & PE_MONO))
    {
        if (song->voice[v].panned==PANNED_MYSTERY)
        {
            right=song->voice[v].right_mix;
            ri=-(right/c);
            while (c--)
            {
                left += li;
                if (left<0)
                    left=0;
                right += ri;
                if (right<0)
                    right=0;
                s=*sp++;
                MIXATION(left);
                MIXATION(right);
            }
        }
        else if (song->voice[v].panned==PANNED_CENTER)
        {
            while (c--)
            {
                left += li;
                if (left<0)
                    return;
                s=*sp++;
                MIXATION(left);
                MIXATION(left);
            }
        }
        else if (song->voice[v].panned==PANNED_LEFT)
        {
            while (c--)
            {
                left += li;
                if (left<0)
                    return;
                s=*sp++;
                MIXATION(left);
                lp++;
            }
        }
        else if (song->voice[v].panned==PANNED_RIGHT)
        {
            while (c--)
            {
                left += li;
                if (left<0)
                    return;
                s=*sp++;
                lp++;
                MIXATION(left);
            }
        }
    }
    else
    {
        /* Mono output.  */
        while (c--)
        {
            left += li;
            if (left<0)
                return;
            s=*sp++;
            MIXATION(left);
        }
    }
}


/**************** interface function ******************/

void mix_voice(MidSong *song, sint32 *buf, int v, sint32 c)
{
    MidVoice *vp = song->voice + v;
    sample_t *sp;
    if (vp->status==VOICE_DIE)
    {
        if (c>=MAX_DIE_TIME)
            c=MAX_DIE_TIME;
        sp=resample_voice(song, v, &c);
        ramp_out(song, sp, buf, v, c);
        vp->status=VOICE_FREE;
    }
    else
    {
        sp=resample_voice(song, v, &c);

        if (song->encoding & PE_MONO)
        {
            /* Mono output. */
            if (vp->envelope_increment || vp->tremolo_phase_increment)
                mix_mono_signal(song, sp, buf, v, c);
            else
                mix_mono(song, sp, buf, v, c);
        }
        else
        {
            if (vp->panned == PANNED_MYSTERY)
            {
                if (vp->envelope_increment || vp->tremolo_phase_increment)
                    mix_mystery_signal(song, sp, buf, v, c);
                else
                    mix_mystery(song, sp, buf, v, c);
            }
            else if (vp->panned == PANNED_CENTER)
            {
                if (vp->envelope_increment || vp->tremolo_phase_increment)
                    mix_center_signal(song, sp, buf, v, c);
                else
                    mix_center(song, sp, buf, v, c);
            }
            else
            {
                /* It's either full left or full right. In either case,
                every other sample is 0. Just get the offset right: */
                if (vp->panned == PANNED_RIGHT) buf++;

                if (vp->envelope_increment || vp->tremolo_phase_increment)
                    mix_single_signal(song, sp, buf, v, c);
                else
                    mix_single(song, sp, buf, v, c);
            }
        }
    }
}

/*****************************************************************/
/* Some functions to convert signed 32-bit data to other formats */

void s32tos8(void *dp, sint32 *lp, sint32 c)
{
    sint8 *cp=(sint8 *)(dp);
    sint32 l;
    while (c--)
    {
        l=(*lp++)>>(32-8-GUARD_BITS);
        if (l>127) l=127;
        else if (l<-128) l=-128;
        *cp++ = (sint8) (l);
    }
}

void s32tou8(void *dp, sint32 *lp, sint32 c)
{
    uint8 *cp=(uint8 *)(dp);
    sint32 l;
    while (c--)
    {
        l=(*lp++)>>(32-8-GUARD_BITS);
        if (l>127) l=127;
        else if (l<-128) l=-128;
        *cp++ = 0x80 ^ ((uint8) l);
    }
}

void s32tos16(void *dp, sint32 *lp, sint32 c)
{
    sint16 *sp=(sint16 *)(dp);
    sint32 l;
    //static int  count=0;

    while (c--)
    {
        l=(*lp++)>>(32-16-GUARD_BITS);
        if (l > 32767) l=32767;
        else if (l<-32768) l=-32768;
        *sp++ = (sint16)(l);
    }
}

void s32tou16(void *dp, sint32 *lp, sint32 c)
{
    uint16 *sp=(uint16 *)(dp);
    sint32 l;
    while (c--)
    {
        l=(*lp++)>>(32-16-GUARD_BITS);
        if (l > 32767) l=32767;
        else if (l<-32768) l=-32768;
        *sp++ = 0x8000 ^ (uint16)(l);
    }
}

void s32tos16x(void *dp, sint32 *lp, sint32 c)
{
    sint16 *sp=(sint16 *)(dp);
    sint32 l;
    while (c--)
    {
        l=(*lp++)>>(32-16-GUARD_BITS);
        if (l > 32767) l=32767;
        else if (l<-32768) l=-32768;
        *sp++ = XCHG_SHORT((sint16)(l));
    }
}


static void adjust_amplification(MidSong *song)
{
    //song->master_volume = (float)(song->amplification) / (float)100.0;
    //song->master_volume_fix = div_320_320_lshift(song->amplification<<12,(100),0);

    song->master_volume_fix = (song->amplification<<5) + (song->amplification<<3) + song->amplification;

}

static void reset_voices(MidSong *song)
{
    int i;
    for (i=0; i<MID_MAX_VOICES; i++)
        song->voice[i].status=VOICE_FREE;
}

/* Process the Reset All Controllers event */
static void reset_controllers(MidSong *song, int c)
{
    song->channel[c].volume=90; /* Some standard says, although the SCC docs say 0. */
    song->channel[c].expression=127; /* SCC-1 does this. */
    song->channel[c].sustain=0;
    song->channel[c].pitchbend=0x2000;
    //song->channel[c].pitchfactor=0; /* to be computed */
    song->channel[c].pitch_fix=0;//zouying add
}

static void reset_midi(MidSong *song)
{
    int i;
    for (i=0; i<16; i++)
    {
        reset_controllers(song, i);
        /* The rest of these are unaffected by the Reset All Controllers event */
        song->channel[i].program=song->default_program;
        song->channel[i].panning=NO_PANNING;
        song->channel[i].pitchsens=2;
        song->channel[i].bank=0; /* tone bank or drum set */
    }
    reset_voices(song);
}

static void select_sample(MidSong *song, int v, MidInstrument *ip, int vel)
{
    MidSample *sp;

    sp=ip->sample;
    song->voice[v].sample=sp;
    return;


}

static void recompute_freq(MidSong *song, int v)
{


    int
    sign=(song->voice[v].sample_increment < 0), /* for bidirectional loops */
    pb=song->channel[song->voice[v].channel].pitchbend;
    sint32  ta,tb;


    if (!song->voice[v].sample->sample_rate)
        return;

    if (song->voice[v].vibrato_control_ratio)
    {
        /* This instrument has vibrato. Invalidate any precomputed
           sample_increments. */

        int i=MID_VIBRATO_SAMPLE_INCREMENTS;
        while (i--)
            song->voice[v].vibrato_sample_increment[i]=0;
    }

    if (pb==0x2000 || pb<0 || pb>0x3FFF)
    {

        song->voice[v].frequency = song->voice[v].orig_frequency;

    }
    else
    {
        pb-=0x2000;

        if (!(song->channel[song->voice[v].channel].pitch_fix))
        {
            /* Damn. Somebody bent the pitch. */
            sint32 i=pb*song->channel[song->voice[v].channel].pitchsens;
            if (pb<0)
                i=-i;

            song->channel[song->voice[v].channel].pitch_fix = mult32_r1(bend_fine_q13[(i>>5) & 0xFF],bend_coarse_q16[i>>13],10);

        }
        if (pb>0)
        {
            song->voice[v].frequency = mult32_r1(song->channel[song->voice[v].channel].pitch_fix,song->voice[v].orig_frequency,19);
        }
        else
        {
            song->voice[v].frequency = div_320_320((song->voice[v].orig_frequency)<<7,(song->channel[song->voice[v].channel].pitch_fix)>>12);
            //DEBUG_MSG("midi orig frq %d, pitch %d\n", song->voice[v].orig_frequency, song->channel[song->voice[v].channel].pitch_fix);
        }

    }


    ta =  div_320_320((song->voice[v].sample->sample_rate)<<12,(song->rate)>>8);
    tb = div_320_320((song->voice[v].frequency)<<9,(song->voice[v].sample->root_freq)>>11);
    ta =  mult32_r1(ta,tb,40-FRACTION_BITS);

    if (sign)
        ta = -ta;

    song->voice[v].sample_increment = ta;


}

static void recompute_amp(MidSong *song, int v)
{
    sint32 tempamp;
    //sint16  lamp;

    /* TODO: use fscale */

    tempamp= (song->voice[v].velocity *
              song->channel[song->voice[v].channel].volume *
              song->channel[song->voice[v].channel].expression); /* 21 bits */

    if (!(song->encoding & PE_MONO))
    {
        if (song->voice[v].panning > 60 && song->voice[v].panning < 68)
        {
            song->voice[v].panned=PANNED_CENTER;

            song->voice[v].lamp_fix = mult32_r1(song->voice[v].sample->volume_fix,song->master_volume_fix,12);
            song->voice[v].lamp_fix = mult32_r1(song->voice[v].lamp_fix  ,tempamp ,21);

        }
        else if (song->voice[v].panning<5)
        {
            song->voice[v].panned = PANNED_LEFT;

            song->voice[v].lamp_fix = mult32_r1(song->voice[v].sample->volume_fix,song->master_volume_fix,12);
            song->voice[v].lamp_fix = mult32_r1(song->voice[v].lamp_fix  ,tempamp ,20);

        }
        else if (song->voice[v].panning>123)
        {
            song->voice[v].panned = PANNED_RIGHT;
            /* left_amp will be used */

            song->voice[v].lamp_fix = mult32_r1(song->voice[v].sample->volume_fix,song->master_volume_fix,12);
            song->voice[v].lamp_fix = mult32_r1(song->voice[v].lamp_fix  ,tempamp ,20);

        }
        else
        {
            song->voice[v].panned = PANNED_MYSTERY;


            song->voice[v].lamp_fix = mult32_r1(song->voice[v].sample->volume_fix,song->master_volume_fix,12);
            song->voice[v].lamp_fix = mult32_r1(song->voice[v].lamp_fix  ,tempamp ,27);

            song->voice[v].ramp_fix =  song->voice[v].lamp_fix * (song->voice[v].panning);
            song->voice[v].lamp_fix *= (127 - song->voice[v].panning);

        }
    }
    else
    {
        song->voice[v].panned = PANNED_CENTER;

        song->voice[v].lamp_fix = mult32_r1(song->voice[v].sample->volume_fix,song->master_volume_fix,12);
        song->voice[v].lamp_fix = mult32_r1(song->voice[v].lamp_fix  ,tempamp ,21);

    }
}

static void start_note(MidSong *song, MidEvent *e, int i)
{
    MidInstrument *ip;
    int j;

    if (ISDRUMCHANNEL(song, e->channel))
    {
        {
            if (!(ip=song->drumset->instrument[e->a]))
                return;
        }
        if (ip->samples != 1)
        {
            DEBUG_MSG("Strange: percussion instrument with %d samples!\n",
                      ip->samples);
        }

        if (ip->sample->note_to_use) /* Do we have a fixed pitch? */
            song->voice[i].orig_frequency = freq_table[(int)(ip->sample->note_to_use)];
        else
            song->voice[i].orig_frequency = freq_table[e->a & 0x7F];

        /* drums are supposed to have only one sample */
        song->voice[i].sample = ip->sample;
    }
    else
    {
        if (song->channel[e->channel].program == SPECIAL_PROGRAM)
            ip=song->default_instrument;
        else
        {
            if (!(ip=song->tonebank->instrument[song->channel[e->channel].program]))
                return; /* No instrument? Then we can't play. */
        }

        if (ip->sample->note_to_use) /* Fixed-pitch instrument? */
            song->voice[i].orig_frequency = freq_table[(int)(ip->sample->note_to_use)];
        else
            song->voice[i].orig_frequency = freq_table[e->a & 0x7F];
        select_sample(song, i, ip, e->b);
    }

    song->voice[i].status = VOICE_ON;
    song->voice[i].channel = e->channel;
    song->voice[i].note = e->a;
    song->voice[i].velocity = e->b;
    song->voice[i].sample_offset = 0;
    song->voice[i].sample_increment = 0; /* make sure it isn't negative */

    song->voice[i].tremolo_phase = 0;
    song->voice[i].tremolo_phase_increment = song->voice[i].sample->tremolo_phase_increment;
    song->voice[i].tremolo_sweep = song->voice[i].sample->tremolo_sweep_increment;
    song->voice[i].tremolo_sweep_position = 0;

    song->voice[i].vibrato_sweep = song->voice[i].sample->vibrato_sweep_increment;
    song->voice[i].vibrato_sweep_position = 0;
    song->voice[i].vibrato_control_ratio = song->voice[i].sample->vibrato_control_ratio;
    song->voice[i].vibrato_control_counter = song->voice[i].vibrato_phase = 0;
    for (j=0; j<MID_VIBRATO_SAMPLE_INCREMENTS; j++)
        song->voice[i].vibrato_sample_increment[j] = 0;

    if (song->channel[e->channel].panning != NO_PANNING)
        song->voice[i].panning = song->channel[e->channel].panning;
    else
        song->voice[i].panning = song->voice[i].sample->panning;

    recompute_freq(song, i);
    recompute_amp(song, i);
    if (song->voice[i].sample->modes & MODES_ENVELOPE)
    {
        /* Ramp up from 0 */
        song->voice[i].envelope_stage = 0;
        song->voice[i].envelope_volume = 0;
        song->voice[i].control_counter = 0;
        recompute_envelope(song, i);
        apply_envelope_to_amp(song, i);
    }
    else
    {
        song->voice[i].envelope_increment = 0;
        apply_envelope_to_amp(song, i);
    }
}

static void kill_note(MidSong *song, int i)
{
    song->voice[i].status = VOICE_DIE;
}

/* Only one instance of a note can be playing on a single channel. */
static void note_on(MidSong *song)
{
    int i = song->voices, lowest=-1;
    sint32 lv=0x7FFFFFFF, v;
    MidEvent *e = song->current_event;

    while (i--)
    {
        if (song->voice[i].status == VOICE_FREE)
            lowest=i; /* Can't get a lower volume than silence */
        else if (song->voice[i].channel==e->channel &&
                 (song->voice[i].note==e->a || song->channel[song->voice[i].channel].mono))
            kill_note(song, i);
    }

    if (lowest != -1)
    {
        /* Found a midi_freespace voice. */
        start_note(song,e,lowest);
        return;
    }

    /* Look for the decaying note with the lowest volume */
    i = song->voices;
    while (i--)
    {
        if ((song->voice[i].status != VOICE_ON) &&
                (song->voice[i].status != VOICE_DIE))
        {
            v = song->voice[i].left_mix;
            if ((song->voice[i].panned == PANNED_MYSTERY)
                    && (song->voice[i].right_mix > v))
                v = song->voice[i].right_mix;
            if (v<lv)
            {
                lv=v;
                lowest=i;
            }
        }
    }

    if (lowest != -1)
    {
        /* This can still cause a click, but if we had a midi_freespace voice to
        spare for ramping down this note, we wouldn't need to kill it
         in the first place... Still, this needs to be fixed. Perhaps
         we could use a reserve of voices to play dying notes only. */

        song->cut_notes++;
        song->voice[lowest].status=VOICE_FREE;
        start_note(song,e,lowest);
    }
    else
        song->lost_notes++;
}

static void finish_note(MidSong *song, int i)
{
    if (song->voice[i].sample->modes & MODES_ENVELOPE)
    {
        /* We need to get the envelope out of Sustain stage */
        song->voice[i].envelope_stage = 3;
        song->voice[i].status = VOICE_OFF;
        recompute_envelope(song, i);
        apply_envelope_to_amp(song, i);
    }
    else
    {
        /* Set status to OFF so resample_voice() will let this voice out
           of its loop, if any. In any case, this voice dies when it
           hits the end of its data (ofs>=data_length). */
        song->voice[i].status = VOICE_OFF;
    }
}

static void note_off(MidSong *song)
{
    int i = song->voices;
    MidEvent *e = song->current_event;

    while (i--)
        if (song->voice[i].status == VOICE_ON &&
                song->voice[i].channel == e->channel &&
                song->voice[i].note == e->a)
        {
            if (song->channel[e->channel].sustain)
            {
                song->voice[i].status = VOICE_SUSTAINED;
            }
            else
                finish_note(song, i);
            return;
        }
}

/* Process the All Notes Off event */
static void all_notes_off(MidSong *song)
{
    int i = song->voices;
    int c = song->current_event->channel;

    //DEBUG_MSG("All notes off on channel %d\n", c);
    while (i--)
        if (song->voice[i].status == VOICE_ON &&
                song->voice[i].channel == c)
        {
            if (song->channel[c].sustain)
                song->voice[i].status = VOICE_SUSTAINED;
            else
                finish_note(song, i);
        }
}

/* Process the All Sounds Off event */
static void all_sounds_off(MidSong *song)
{
    int i = song->voices;
    int c = song->current_event->channel;

    while (i--)
        if (song->voice[i].channel == c &&
                song->voice[i].status != VOICE_FREE &&
                song->voice[i].status != VOICE_DIE)
        {
            kill_note(song, i);
        }
}

static void adjust_pressure(MidSong *song)
{
    MidEvent *e = song->current_event;
    int i = song->voices;

    while (i--)
        if (song->voice[i].status == VOICE_ON &&
                song->voice[i].channel == e->channel &&
                song->voice[i].note == e->a)
        {
            song->voice[i].velocity = e->b;
            recompute_amp(song, i);
            apply_envelope_to_amp(song, i);
            return;
        }
}

static void drop_sustain(MidSong *song)
{
    int i = song->voices;
    int c = song->current_event->channel;

    while (i--)
        if (song->voice[i].status == VOICE_SUSTAINED && song->voice[i].channel == c)
            finish_note(song, i);
}

static void adjust_pitchbend(MidSong *song)
{
    int c = song->current_event->channel;
    int i = song->voices;

    while (i--)
        if (song->voice[i].status != VOICE_FREE && song->voice[i].channel == c)
        {
            recompute_freq(song, i);
        }
}

static void adjust_volume(MidSong *song)
{
    int c = song->current_event->channel;
    int i = song->voices;

    while (i--)
        if (song->voice[i].channel == c &&
                (song->voice[i].status==VOICE_ON || song->voice[i].status==VOICE_SUSTAINED))
        {
            recompute_amp(song, i);
            apply_envelope_to_amp(song, i);
        }
}

static void seek_forward(MidSong *song, sint32 until_time)
{
    MidEvent *eventPtr;

    reset_voices(song);
    if(!song->current_event) return;//sheen

    while (song->current_event->time < until_time)
    {
        switch(song->current_event->type)
        {
            /* All notes stay off. Just handle the parameter changes. */

            case ME_PITCH_SENS:
                song->channel[song->current_event->channel].pitchsens =
                    song->current_event->a;
                //song->channel[song->current_event->channel].pitchfactor = 0;
                song->channel[song->current_event->channel].pitch_fix = 0;
                break;

            case ME_PITCHWHEEL:
                song->channel[song->current_event->channel].pitchbend =
                    song->current_event->a + song->current_event->b * 128;
                //song->channel[song->current_event->channel].pitchfactor = 0;
                song->channel[song->current_event->channel].pitch_fix = 0;
                break;

            case ME_MAINVOLUME:
                song->channel[song->current_event->channel].volume =
                    song->current_event->a;
                break;

            case ME_PAN:
                song->channel[song->current_event->channel].panning =
                    song->current_event->a;
                break;

            case ME_EXPRESSION:
                song->channel[song->current_event->channel].expression =
                    song->current_event->a;
                break;

            case ME_PROGRAM:
                if (ISDRUMCHANNEL(song, song->current_event->channel))
                    /* Change drum set */
                    song->channel[song->current_event->channel].bank =
                        song->current_event->a;
                else
                    song->channel[song->current_event->channel].program =
                        song->current_event->a;
                break;

            case ME_SUSTAIN:
                song->channel[song->current_event->channel].sustain =
                    song->current_event->a;
                break;

            case ME_RESET_CONTROLLERS:
                reset_controllers(song, song->current_event->channel);
                break;

            case ME_TONE_BANK:
                song->channel[song->current_event->channel].bank =
                    song->current_event->a;
                break;

            case ME_EOT:
                song->current_sample = song->current_event->time;
                return;
        }

        //song->current_event++; sheen
        do
        {
            eventPtr=get_one_event(song, song->divisions, &lastEvtTrkNum);
            if(!eventPtr)break;
            song->current_event=eventPtr;
            read_track(song->fileStrm, song, trackAppend, lastEvtTrkNum);
        }
        while((int32)eventPtr==-1); //not a skip
        if(!eventPtr)
            break;

    }
    /*song->current_sample=song->current_event->time;*/
    //if (song->current_event != song->events) sheen
    //  song->current_event--;
    song->current_sample=until_time;
}

void skip_to(MidSong *song, sint32 until_time)
{
    int32 i,res;
    if (song->current_sample > until_time)
    {
        song->current_sample = 0;

        //add sheen
        for(i=0; i<song->tracksNum; i++)
        {
            pTrkRedParm[i].trkPos= pTrkRedParm[i].trkStart;
            if(USE_CHUNK_BUF==0)
            {
                pTrkRedParm[i].byteLeft=0;
            }
            pTrkRedParm[i].pchunk=pTrkRedParm[i].chunk;
        }

        memset(pEvlistShort,0, (MAX_TRACK)*sizeof(MidEventListShort));
        for(i=0; i<MAX_TRACK; i++)
        {
            pEvlistShort[i].trackNum=i;
        }

        pEvlistShortHead=pEvlistShort;
        pEvlistShortTail=pEvlistShort;

        memset(at_per_trk,0,MAX_TRACK*sizeof(sint32));

        switch(song->format)
        {
            case 0:
                res=read_track(song->fileStrm, song, trackAppend, 0);
                if(res==0 || res==-3)//-3= end track
                {
                    break;
                }
                else
                    return;
            case 1:
                for (i=0; i<song->tracksNum; i++)
                {
                    res=read_track(song->fileStrm, song, trackAppend, i);
                    if(res!=0 && res!=-3)//-3= end track
                        return;
                }
                break;

            case 2: /* We simply play the tracks sequentially */
                for (i=0; i<song->tracksNum; i++)
                {
                    res=read_track(song->fileStrm, song, trackAppend, i);
                    if(res!=0 && res!=-3)//-3= end track
                        return;
                }
                break;
            default:
                return;
        }

        for (i=0; i<16; i++)
        {
            current_bank[i]=0;
            current_set[i]=0;
            current_program[i]=song->default_program;
        }

        our_event_count=0;
        st=at=sample_cum=0;
        counting_time=2; /* We strip any silence before the first NOTE ON. */

        do
        {
            song->events=get_one_event(song, song->divisions, &lastEvtTrkNum);
            if(!song->events)return;
            read_track(song->fileStrm, song, trackAppend, lastEvtTrkNum);
        }
        while((int32)song->events==-1); //not a skip
        //add end. sheen
    }

    reset_midi(song);
    song->current_event = song->events;

    if (until_time)
        seek_forward(song, until_time);
}

static void do_compute_data(MidSong *song, sint32 count)
{
    int i;
    memset(common_buffer, 0,   (song->encoding & PE_MONO) ? (count * 4) : (count * 8));
    for (i = 0; i < song->voices; i++)
    {
        if(song->voice[i].status != VOICE_FREE)
            mix_voice(song, common_buffer, i, count);
    }
    song->current_sample += count;
}

/* count=0 means flush remaining buffered data to output device, then
   flush the device itself */
static void compute_data(MidSong *song, sint8 *stream, sint32 count)
{
    int channels;

    if ( song->encoding & PE_MONO )
        channels = 1;
    else
        channels = 2;

    while (count)
    {
        sint32 block = count;
        if (block > song->buffer_size)
            block = song->buffer_size;
        do_compute_data(song, block);
        song->write(stream, common_buffer, channels * block);
        stream += song->bytes_per_sample * block;
        count -= block;
    }
}

void mid_song_start(MidSong *song)
{
    song->playing = 1;
    adjust_amplification(song);
    skip_to(song, 0);
}

void mid_song_seek(MidSong *song, uint32 ms)
{
    skip_to(song, (ms * (song->rate / 100)) / 10);
}

uint32 mid_song_get_total_time(MidSong *song)
{
#if 0//sheen
    MidEvent *last_event = &song->events[song->groomed_event_count - 1];
    /* We want last_event->time * 1000 / song->rate */
    uint32 retvalue = (last_event->time / song->rate) * 1000;
    retvalue       += (last_event->time % song->rate) * 1000 / song->rate;
#else
    uint32 retvalue = (song->totalSample / song->rate) * 1000;
    retvalue       += (song->totalSample % song->rate) * 1000 / song->rate;
#endif
    return retvalue;
}

uint32 mid_song_get_time(MidSong *song)
{
    uint32 retvalue = (song->current_sample / song->rate) * 1000;
    retvalue       += (song->current_sample % song->rate) * 1000 / song->rate;
    return retvalue;
}

char *mid_song_get_meta(MidSong *song, MidSongMetaId what)
{
    return song->meta_data[what];
}

size_t mid_song_read_wave(MidSong *song, void *ptr, size_t size)
{
    sint32 start_sample, end_sample, samples;
    MidEvent *eventPtr;

    uint32 mid_time0,mid_time1;
#ifndef SHEEN_VC_DEBUG
    mid_time0=hal_TimGetUpTime();
#endif

    if (!song->playing)
        return 0;
    if(!song->current_event)//sheen
        return 0;

    samples = size / song->bytes_per_sample;

    start_sample = song->current_sample;
    end_sample = song->current_sample+samples;
    while ( song->current_sample < end_sample )
    {
        /* Handle all events that should happen at this time */
        while (song->current_event->time <= song->current_sample)
        {
            switch(song->current_event->type)
            {

                /* Effects affecting a single note */

                case ME_NOTEON:
                    if (!(song->current_event->b)) /* Velocity 0? */
                        note_off(song);
                    else
                        note_on(song);
                    break;

                case ME_NOTEOFF:
                    note_off(song);
                    break;

                case ME_KEYPRESSURE:
                    adjust_pressure(song);
                    break;

                /* Effects affecting a single channel */

                case ME_PITCH_SENS:
                    song->channel[song->current_event->channel].pitchsens =
                        song->current_event->a;
                    //song->channel[song->current_event->channel].pitchfactor = 0;
                    song->channel[song->current_event->channel].pitch_fix = 0;
                    break;

                case ME_PITCHWHEEL:
                    song->channel[song->current_event->channel].pitchbend =
                        song->current_event->a + song->current_event->b * 128;
                    //song->channel[song->current_event->channel].pitchfactor = 0;
                    song->channel[song->current_event->channel].pitch_fix = 0;
                    /* Adjust pitch for notes already playing */
                    adjust_pitchbend(song);
                    break;

                case ME_MAINVOLUME:
                    song->channel[song->current_event->channel].volume =
                        song->current_event->a;
                    adjust_volume(song);
                    break;

                case ME_PAN:
                    song->channel[song->current_event->channel].panning =
                        song->current_event->a;
                    break;

                case ME_EXPRESSION:
                    song->channel[song->current_event->channel].expression =
                        song->current_event->a;
                    adjust_volume(song);
                    break;

                case ME_PROGRAM:
                    if (ISDRUMCHANNEL(song, song->current_event->channel))
                    {
                        /* Change drum set */
                        song->channel[song->current_event->channel].bank =
                            song->current_event->a;
                    }
                    else
                        song->channel[song->current_event->channel].program =
                            song->current_event->a;
                    break;

                case ME_SUSTAIN:
                    song->channel[song->current_event->channel].sustain =
                        song->current_event->a;
                    if (!song->current_event->a)
                        drop_sustain(song);
                    break;

                case ME_RESET_CONTROLLERS:
                    reset_controllers(song, song->current_event->channel);
                    break;

                case ME_ALL_NOTES_OFF:
                    all_notes_off(song);
                    break;

                case ME_ALL_SOUNDS_OFF:
                    all_sounds_off(song);
                    break;

                case ME_TONE_BANK:
                    song->channel[song->current_event->channel].bank =
                        song->current_event->a;
                    break;

                case ME_EOT:
                    /* Give the last notes a couple of seconds to decay  */
                    DEBUG_MSG("Playing time: %d seconds\n", song->current_sample/song->rate+2);
                    DEBUG_MSG("Notes cut: %d\n", song->cut_notes);
                    DEBUG_MSG("Notes lost totally: %d\n", song->lost_notes);
                    song->playing = 0;
                    return (song->current_sample - start_sample) * song->bytes_per_sample;
            }

            //song->current_event++; sheen
            do
            {
                eventPtr=get_one_event(song, song->divisions, &lastEvtTrkNum);
                if(!eventPtr)break;
                if(eventPtr!=-1)
                    song->current_event=eventPtr;
                read_track(song->fileStrm, song, trackAppend, lastEvtTrkNum);
            }
            while((int32)eventPtr==-1);
            if(eventPtr==0)
                break;
        }
        if (song->current_event->time > end_sample)
        {
            int  step= end_sample-song->current_sample;
            compute_data(song, (sint8 *)ptr, end_sample-song->current_sample);
            ptr=(sint8 *)ptr+song->bytes_per_sample*step;
        }
        else
        {
            int step = song->current_event->time-song->current_sample;
            compute_data(song, (sint8 *)ptr, song->current_event->time-song->current_sample);
            ptr=(sint8 *)ptr+song->bytes_per_sample*step;
        }
    }
#ifndef SHEEN_VC_DEBUG
    mid_time1=hal_TimGetUpTime();
#endif
    mid_dec_t+=mid_time1 - mid_time0;
    mid_dec_num++;
    //DEBUG_MSG("mid_song_read_wave time=%d\n", mid_time1*1000/16384);

    return samples * song->bytes_per_sample;
}

void mid_song_set_volume(MidSong *song, int volume)
{
    int i;
    if (volume > MAX_AMPLIFICATION)
        song->amplification = MAX_AMPLIFICATION;
    else if (volume < 0)
        song->amplification = 0;
    else
        song->amplification = volume;
    adjust_amplification(song);
    for (i = 0; i < song->voices; i++)
        if (song->voice[i].status != VOICE_FREE)
        {
            recompute_amp(song, i);
            apply_envelope_to_amp(song, i);
        }
}

/* Computes how many (fractional) samples one MIDI delta-time unit contains */
static void compute_sample_increment(MidSong *song, sint32 tempo,
                                     sint32 divisions)
{
    sint32  fix_a1,fix_a2;
    fix_a1 = div_320_320(tempo<<8,10000);
    fix_a2 = div_320_320((song->rate)<<8,divisions*100);
    fix_a1 = fix_a1 * fix_a2;

    song->sample_correction = fix_a1 & 0xFFFF;
    song->sample_increment = fix_a1 >> 16;

    DEBUG_MSG("Samples per delta-t: %d (correction %d)\n",
              song->sample_increment, song->sample_correction);
}

/* Read variable-length number (7 bits per byte, MSB first) */
static sint32 getvl(MidIStream stream)
{
    sint32 l=0;
    uint8 c;
    for (;;)
    {
        c=0;
        mid_istream_read(stream, &c, 1, 1);
        l += (c & 0x7f);
        if (!(c & 0x80)) return l;
        l<<=7;
    }
}

/* Print a string from the file, followed by a newline. Any non-ASCII
   or unprintable characters will be converted to periods. */
static int read_meta_data(MidIStream stream, sint32 len, uint8 type, MidSong *song)
{
    mid_istream_skip(stream,len);
    return 0;
}

/* sheen
#define MIDIEVENT(at,t,ch,pa,pb) \
  new=midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidEventList)); \
  if(new==NULL)  return new;\
  new->event.time=at; new->event.type=t; new->event.channel=ch; \
  new->event.a=pa; new->event.b=pb; new->next=0;\
  return new;
  */
#define MIDIEVENT(at,t,ch,pa,pb) \
    eventTemp.time=at; eventTemp.type=t; eventTemp.channel=ch; \
    eventTemp.a=pa; eventTemp.b=pb; \
  return (&eventTemp);

//#define MAGIC_EOT ((MidEventList *)(-1))
#define MAGIC_EOT ((MidEvent *)(-1))

/* Read a MIDI event, returning a freshly allocated element that can
   be linked to the event list */
//static MidEventList *read_midi_event(MidIStream stream, MidSong *song) sheen
static MidEvent *read_midi_event(MidIStream stream, MidSong *song, uint32 trackNum)
{
    //static uint8 laststatus, lastchan; sheen
    //static uint8 nrpn=0, rpn_msb[16], rpn_lsb[16]; /* one per channel */ sheen
    uint8 me, type, a,b,c;
    sint32 len;
    //MidEventList *new=0; sheen
//int32   tt1=hal_TimGetUpTime();
    for (;;)
    {
        *(song->at) += getvl(stream);//sheen
        if (mid_istream_read(stream, &me, 1, 1)<0)
        {
            DEBUG_MSG("read_midi_event: mid_istream_read() failure\n");
            return 0;
        }

        if(me==0xF0 || me == 0xF7) /* SysEx event */
        {
            len=getvl(stream);
            mid_istream_skip(stream, len);
        }
        else if(me==0xFF) /* Meta event */
        {
            mid_istream_read(stream, &type, 1, 1);
            len=getvl(stream);
            if (type>0 && type<16)
            {
                read_meta_data(stream, len, type, song);
            }
            else
                switch(type)
                {
                    case 0x2F: /* End of Track */
                        return MAGIC_EOT;

                    case 0x51: /* Tempo */
                        mid_istream_read(stream, &a, 1, 1);
                        mid_istream_read(stream, &b, 1, 1);
                        mid_istream_read(stream, &c, 1, 1);
                        MIDIEVENT(*(song->at), ME_TEMPO, c, a, b);

                    default:
                        //DEBUG_MSG("(Meta event type 0x%02x, length %d)\n", type, len);
                        mid_istream_skip(stream, len);
                        break;
                }
        }
        else
        {
            a=me;
            if (a & 0x80) /* status byte */
            {
                pEvtRedParm[trackNum].lastchan=a & 0x0F;
                pEvtRedParm[trackNum].laststatus=(a>>4) & 0x07;
                mid_istream_read(stream, &a, 1, 1);
                a &= 0x7F;
            }
            switch(pEvtRedParm[trackNum].laststatus)
            {
                case 0: /* Note off */
                    mid_istream_read(stream, &b, 1, 1);
                    b &= 0x7F;
                    MIDIEVENT(*(song->at), ME_NOTEOFF, pEvtRedParm[trackNum].lastchan, a,b);

                case 1: /* Note on */
                    mid_istream_read(stream, &b, 1, 1);
                    b &= 0x7F;
                    MIDIEVENT(*(song->at), ME_NOTEON, pEvtRedParm[trackNum].lastchan, a,b);

                case 2: /* Key Pressure */
                    mid_istream_read(stream, &b, 1, 1);
                    b &= 0x7F;
                    MIDIEVENT(*(song->at), ME_KEYPRESSURE, pEvtRedParm[trackNum].lastchan, a, b);

                case 3: /* Control change */
                    mid_istream_read(stream, &b, 1, 1);
                    b &= 0x7F;
                    {
                        int control=255;
                        switch(a)
                        {
                            case 7: control=ME_MAINVOLUME; break;
                            case 10: control=ME_PAN; break;
                            case 11: control=ME_EXPRESSION; break;
                            case 64: control=ME_SUSTAIN; break;
                            case 120: control=ME_ALL_SOUNDS_OFF; break;
                            case 121: control=ME_RESET_CONTROLLERS; break;
                            case 123: control=ME_ALL_NOTES_OFF; break;

                            /* These should be the SCC-1 tone bank switch
                               commands. I don't know why there are two, or
                               why the latter only allows switching to bank 0.
                               Also, some MIDI files use 0 as some sort of
                               continuous controller. This will cause lots of
                               warnings about undefined tone banks. */
                            case 0: control=ME_TONE_BANK; break;
                            case 32:
                                if (b!=0)
                                    DEBUG_MSG("(Strange: tone bank change 0x20%02x)\n", b);
                                else
                                    control=ME_TONE_BANK;
                                break;

                            case 100: pEvtRedParm[trackNum].nrpn=0; pEvtRedParm[trackNum].rpn_msb[pEvtRedParm[trackNum].lastchan]=b; break;
                            case 101: pEvtRedParm[trackNum].nrpn=0; pEvtRedParm[trackNum].rpn_lsb[pEvtRedParm[trackNum].lastchan]=b; break;
                            case 99: pEvtRedParm[trackNum].nrpn=1; pEvtRedParm[trackNum].rpn_msb[pEvtRedParm[trackNum].lastchan]=b; break;
                            case 98: pEvtRedParm[trackNum].nrpn=1; pEvtRedParm[trackNum].rpn_lsb[pEvtRedParm[trackNum].lastchan]=b; break;

                            case 6:
                                if (pEvtRedParm[trackNum].nrpn)
                                {
                                    //DEBUG_MSG("(Data entry (MSB) for NRPN %02x,%02x: %d)\n",
                                    //  rpn_msb[lastchan], rpn_lsb[lastchan], b);
                                    break;
                                }

                                switch((pEvtRedParm[trackNum].rpn_msb[pEvtRedParm[trackNum].lastchan]<<8) | pEvtRedParm[trackNum].rpn_lsb[pEvtRedParm[trackNum].lastchan])
                                {
                                    case 0x0000: /* Pitch bend sensitivity */
                                        control=ME_PITCH_SENS;
                                        break;

                                    case 0x7F7F: /* RPN reset */
                                        /* reset pitch bend sensitivity to 2 */
                                        MIDIEVENT(*(song->at), ME_PITCH_SENS, pEvtRedParm[trackNum].lastchan, 2, 0);

                                    default:
                                        //DEBUG_MSG("(Data entry (MSB) for RPN %02x,%02x: %d)\n",
                                        //  rpn_msb[lastchan], rpn_lsb[lastchan], b);
                                        break;
                                }
                                break;

                            default:
                                //DEBUG_MSG("(Control %d: %d)\n", a, b);
                                break;
                        }
                        if (control != 255)
                        {
                            MIDIEVENT(*(song->at), control, pEvtRedParm[trackNum].lastchan, b, 0);
                        }
                    }
                    break;

                case 4: /* Program change */
                    a &= 0x7f;
                    MIDIEVENT(*(song->at), ME_PROGRAM, pEvtRedParm[trackNum].lastchan, a, 0);

                case 5: /* Channel pressure - NOT IMPLEMENTED */
                    break;

                case 6: /* Pitch wheel */
                    mid_istream_read(stream, &b, 1, 1);
                    b &= 0x7F;
                    MIDIEVENT(*(song->at), ME_PITCHWHEEL, pEvtRedParm[trackNum].lastchan, a, b);

                default:
                    DEBUG_MSG("*** Can't happen: status 0x%02X, channel 0x%02X\n",
                              pEvtRedParm[trackNum].laststatus, pEvtRedParm[trackNum].lastchan);
                    return 0;//break;
            }
        }
    }

//int32   tt2=hal_TimGetUpTime();
//  diag_printf("read_midi_event time=%d",(tt2-tt1)*1000/16384);

    //return new;
    return (&eventTemp);
}

#undef MIDIEVENT

/* Read a midi track into the linked list, either merging with any previous
   tracks or appending to them. */
static int read_track(MidIStream stream, MidSong *song, int append, uint32 trackNum)
{
    //MidEventList *meep; sheen
    //MidEventList *next, *new; //sheen
    MidEventListShort *next, *pre;
    MidEvent *pEvt;//sheen
    //sint32 len;
    //char tmp[4];
    trkReadFlag=trackNum;

#if 0//sheen
    meep = song->evlist;
    if (append && meep)
    {
        /* find the last event in the list */
        for (; meep->next; meep=meep->next)
            ;
        song->at = meep->event.time;
    }
    else
        song->at=0;
#else
    if(append)
        song->at=&(at_per_trk[0]);//all use one.
    else
        song->at=&(at_per_trk[trackNum]);
#endif
    /*
      if(trackNum<MAX_TRACK)//sheen
      {
          int32 res;
          if(pTrkPos[trackNum]!=0)
          {
              res=mid_stream_seek(stream,pTrkPos[trackNum],FS_SEEK_SET);
              if((uint32)res!=pTrkPos[trackNum])
                  return -1;
          }
      }
      else
      {
          return -1;
      }*/

#if 0
    /* Check the formalities */
    if(trksParseOverFlag==0)
    {
        if (mid_istream_read(stream, tmp, 1, 4) <0 || mid_istream_read(stream, &len, 4, 1)<0)
        {
            DEBUG_MSG("Can't read track header.\n");
            return -1;
        }
        len=SWAPBE32(len);
        if (memcmp(tmp, "MTrk", 4))
        {
            DEBUG_MSG("Corrupt MIDI file.\n");
            return -2;
        }

        //store cur pos
        //pTrkPos[trackNum]=mid_stream_seek(stream,0,FS_SEEK_CUR);
    }
#endif


    //for (;;) only read one event sheen
    {
        //if (!(new=read_midi_event(stream, song))) /* Some kind of error  */ sheen
        if (!(pEvt=read_midi_event(stream, song, trackNum))) /* Some kind of error  */
            return -2;


        /*
        {
        FILE *plog=fopen("F:\\Դ\\midi\\log2.txt","a+");
        if(pEvt!=-1)
        fprintf(plog,"%d %d %d %d %d \n",pEvt->time,
        pEvt->channel,pEvt->type,
        pEvt->a,pEvt->b);
        fclose(plog);
        }
        */

        //if (new==MAGIC_EOT) /* End-of-track Hack. */ sheen
        if (pEvt==MAGIC_EOT) /* End-of-track Hack. */
        {
            pEvlistShort[trackNum].endFlag=MAGIC_EOT;
            return -3;
        }

        //find next & pre with pEvt center.
        pre=pEvlistShortTail;
        next=pEvlistShortTail->next;
        while(pre && pre->event.time > pEvt->time)
        {
            //tail to head
            //if(!pre)break;
            next=pre;
            pre=pre->pre;
        }

        //debug for order match
        /*
        while(pre && pre->event.time == pEvt->time && trackNum < pre->trackNum)
        {//tail to head
            //if(!pre)break;
            next=pre;
            pre=pre->pre;
        }*/
        //debug end

        if(pre==&(pEvlistShort[trackNum]))
            pre=0;

        //insert event within pre & next
        pEvlistShort[trackNum].event=*pEvt;

        //change next
        if(next)
            next->pre=(void*)&(pEvlistShort[trackNum]);
        //change cur
        pEvlistShort[trackNum].next=next;
        pEvlistShort[trackNum].pre=pre;
        //change pre
        if(pre)
            pre->next=(void*)&(pEvlistShort[trackNum]);
        //update tail
        if(pEvlistShort[trackNum].next==0)
            pEvlistShortTail=&(pEvlistShort[trackNum]);
        //update head
        if(pEvlistShort[trackNum].pre==0)
            pEvlistShortHead=&(pEvlistShort[trackNum]);

        //song->event_count++; /* Count the event. (About one?) */

        //meep=new; sheen

        //diag_printf("event_count=%d time=%d trk=%d\n",song->event_count,pEvt->time, trackNum);
    }

    //store cur pos
    //pTrkPos[trackNum]=mid_stream_seek(stream,0,FS_SEEK_CUR);
    trkReadFlag=255;

    return 0;
}


/* Allocate an array of MidiEvents and fill it from the linked list of
   events, marking used instruments for loading. Convert event times to
   samples: handle tempo changes. Strip unnecessary events from the list.
   Free the linked list. */
#if 0
static MidEvent *groom_list(MidSong *song, sint32 divisions,sint32 *eventsp,
                            sint32 *samplesp)
{
    //MidEvent *groomed_list, *lp;
    //MidEventList *meep; sheen
    sint32 i, our_event_count, tempo, skip_this_event, new_value;
    sint32 sample_cum, samples_to_do, at, st, dt, counting_time;

    int current_bank[16], current_set[16], current_program[16];
    /* Or should each bank have its own current program? */
//int32   tt1=hal_TimGetUpTime();
    for (i=0; i<16; i++)
    {
        current_bank[i]=0;
        current_set[i]=0;
        current_program[i]=song->default_program;
    }

    tempo=500000;
    compute_sample_increment(song, tempo, divisions);

    /* This may allocate a bit more than we need */
    /* sheen
    groomed_list=lp=midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidEvent) * (song->event_count+1));
    if(groomed_list==NULL)
      return NULL;
    meep=song->evlist;
    */

    our_event_count=0;
    st=at=sample_cum=0;
    counting_time=2; /* We strip any silence before the first NOTE ON. */

    for (i = 0; i < song->event_count; i++)
    {
        skip_this_event=0;

        if (meep->event.type==ME_TEMPO)
        {
            tempo=
                meep->event.channel + meep->event.b * 256 + meep->event.a * 65536;
            compute_sample_increment(song, tempo, divisions);
            skip_this_event=1;
        }
        else switch (meep->event.type)
            {
                case ME_PROGRAM:
                    if (ISDRUMCHANNEL(song, meep->event.channel))
                    {
                        if (meep->event.a==0) //if (song->drumset[meep->event.a])/* Is this a defined drumset? */
                            new_value=meep->event.a;
                        else
                        {
                            DEBUG_MSG("Drum set %d is undefined\n", meep->event.a);
                            new_value=meep->event.a=0;
                        }
                        if (current_set[meep->event.channel] != new_value)
                            current_set[meep->event.channel]=new_value;
                        else
                            skip_this_event=1;
                    }
                    else
                    {
                        new_value=meep->event.a;
                        if ((current_program[meep->event.channel] != SPECIAL_PROGRAM)
                                && (current_program[meep->event.channel] != new_value))
                            current_program[meep->event.channel] = new_value;
                        else
                            skip_this_event=1;
                    }
                    break;

                case ME_NOTEON:
                    if (counting_time)
                        counting_time=1;
                    if (ISDRUMCHANNEL(song, meep->event.channel))
                    {
                        if (current_set[meep->event.channel]==0)
                        {
                            if (!(song->drumset->instrument[meep->event.a]))
                                song->drumset->instrument[meep->event.a] = MAGIC_LOAD_INSTRUMENT;
                        }
                        else
                            DEBUG_MSG("[ME_NOTEON] Drum set %d is undefined\n", current_set[meep->event.channel]);
                    }
                    else
                    {
                        if (current_program[meep->event.channel]==SPECIAL_PROGRAM)
                            break;
                        if (current_bank[meep->event.channel]==0)
                        {
                            if (!(song->tonebank->instrument[current_program[meep->event.channel]]))
                                song->tonebank->instrument[current_program[meep->event.channel]] =MAGIC_LOAD_INSTRUMENT;
                        }
                        else
                            DEBUG_MSG("[ME_NOTEON] Tonebank set %d is undefined\n", current_set[meep->event.channel]);

                    }
                    break;

                case ME_TONE_BANK:
                    if (ISDRUMCHANNEL(song, meep->event.channel))
                    {
                        skip_this_event=1;
                        break;
                    }
                    if (meep->event.a==0)//if (song->tonebank[meep->event.a]) /* Is this a defined tone bank? */
                        new_value=meep->event.a;
                    else
                    {
                        DEBUG_MSG("Tone bank %d is undefined\n", meep->event.a);
                        new_value=meep->event.a=0;
                    }
                    if (current_bank[meep->event.channel]!=new_value)
                        current_bank[meep->event.channel]=new_value;
                    else
                        skip_this_event=1;
                    break;
            }

        /* Recompute time in samples*/
        if ((dt=meep->event.time - at) && !counting_time)
        {
            samples_to_do = song->sample_increment * dt;
            sample_cum += song->sample_correction * dt;
            if (sample_cum & 0xFFFF0000)
            {
                samples_to_do += ((sample_cum >> 16) & 0xFFFF);
                sample_cum &= 0x0000FFFF;
            }
            st += samples_to_do;
        }
        else if (counting_time==1) counting_time=0;
        if (!skip_this_event)
        {
            /* Add the event to the list */
            *lp=meep->event;
            lp->time=st;
            lp++;
            our_event_count++;
        }
        at=meep->event.time;
        meep=meep->next;
    }
    /* Add an End-of-Track event */
    lp->time=st;
    lp->type=ME_EOT;
    our_event_count++;

    midi_free_pool(MIDIPOOL_TEMP);//free_midi_list(song);

    *eventsp=our_event_count;
    *samplesp=st;

//int32   tt2=hal_TimGetUpTime();
//  diag_printf("groom_list time=%d",(tt2-tt1)*1000/16384);

    return groomed_list;
}
#endif

/* get MidiEvents from the linked list of
   events, marking used instruments for loading. Convert event times to
   samples: handle tempo changes. Strip unnecessary events from the list.

   add sheen.
     */
static MidEvent *get_one_event(MidSong *song, sint32 divisions, uint32 *trackNum)
{
    //MidEvent *groomed_list, *lp;
    //MidEventList *meep; sheen
    MidEventListShort *next;
    sint32 tempo, skip_this_event, new_value;
    sint32  samples_to_do,  dt;

    if(!pEvlistShortHead)
    {
        eventTemp2.channel=0;
        eventTemp2.time=st;
        eventTemp2.type=ME_EOT;
        eventTemp2.a=0;
        eventTemp2.b=0;
        return 0;
    }

    /*
    if(trksParseOverFlag==1)
    {
        FILE *plog=fopen("F:\\Դ\\midi\\log1.txt","a+");
        fprintf(plog,"%d %d %d %d %d %d \n",pEvlistShortHead->trackNum,pEvlistShortHead->event.time,
            pEvlistShortHead->event.channel,pEvlistShortHead->event.type,
            pEvlistShortHead->event.a,pEvlistShortHead->event.b);
        fclose(plog);

    }*/

    skip_this_event=0;
    if (pEvlistShortHead->event.type==ME_TEMPO)
    {
        tempo=
            pEvlistShortHead->event.channel + pEvlistShortHead->event.b * 256 + pEvlistShortHead->event.a * 65536;
        compute_sample_increment(song, tempo, divisions);
        skip_this_event=1;
    }
    else switch (pEvlistShortHead->event.type)
        {
            case ME_PROGRAM:
                if (ISDRUMCHANNEL(song, pEvlistShortHead->event.channel))
                {
                    if (pEvlistShortHead->event.a==0) //if (song->drumset[meep->event.a])/* Is this a defined drumset? */
                        new_value=pEvlistShortHead->event.a;
                    else
                    {
                        DEBUG_MSG("Drum set %d is undefined\n", pEvlistShortHead->event.a);
                        new_value=pEvlistShortHead->event.a=0;
                    }
                    if (current_set[pEvlistShortHead->event.channel] != new_value)
                        current_set[pEvlistShortHead->event.channel]=new_value;
                    else
                        skip_this_event=1;
                }
                else
                {
                    new_value=pEvlistShortHead->event.a;
                    if ((current_program[pEvlistShortHead->event.channel] != SPECIAL_PROGRAM)
                            && (current_program[pEvlistShortHead->event.channel] != new_value))
                        current_program[pEvlistShortHead->event.channel] = new_value;
                    else
                        skip_this_event=1;
                }
                break;

            case ME_NOTEON:
                if (counting_time)
                    counting_time=1;
                if(trksParseOverFlag==0)//only once.sheen
                {
                    if (ISDRUMCHANNEL(song, pEvlistShortHead->event.channel))
                    {
                        if (current_set[pEvlistShortHead->event.channel]==0)
                        {
                            if (!(song->drumset->instrument[pEvlistShortHead->event.a]))
                                song->drumset->instrument[pEvlistShortHead->event.a] = MAGIC_LOAD_INSTRUMENT;
                        }
                        else
                            DEBUG_MSG("[ME_NOTEON] Drum set %d is undefined\n", current_set[pEvlistShortHead->event.channel]);
                    }
                    else
                    {
                        if (current_program[pEvlistShortHead->event.channel]==SPECIAL_PROGRAM)
                            break;
                        if (current_bank[pEvlistShortHead->event.channel]==0)
                        {
                            if (!(song->tonebank->instrument[current_program[pEvlistShortHead->event.channel]]))
                                song->tonebank->instrument[current_program[pEvlistShortHead->event.channel]] =MAGIC_LOAD_INSTRUMENT;
                        }
                        else
                            DEBUG_MSG("[ME_NOTEON] Tonebank set %d is undefined\n", current_set[pEvlistShortHead->event.channel]);

                    }
                }
                break;

            case ME_TONE_BANK:
                if (ISDRUMCHANNEL(song, pEvlistShortHead->event.channel))
                {
                    skip_this_event=1;
                    break;
                }
                if (pEvlistShortHead->event.a==0)//if (song->tonebank[meep->event.a]) /* Is this a defined tone bank? */
                    new_value=pEvlistShortHead->event.a;
                else
                {
                    DEBUG_MSG("Tone bank %d is undefined\n", pEvlistShortHead->event.a);
                    new_value=pEvlistShortHead->event.a=0;
                }
                if (current_bank[pEvlistShortHead->event.channel]!=new_value)
                    current_bank[pEvlistShortHead->event.channel]=new_value;
                else
                    skip_this_event=1;
                break;
        }

    /* Recompute time in samples*/
    if ((dt=pEvlistShortHead->event.time - at) && !counting_time)
    {
        samples_to_do = song->sample_increment * dt;
        sample_cum += song->sample_correction * dt;
        if (sample_cum & 0xFFFF0000)
        {
            samples_to_do += ((sample_cum >> 16) & 0xFFFF);
            sample_cum &= 0x0000FFFF;
        }
        st += samples_to_do;
    }
    else if (counting_time==1)
        counting_time=0;

    if (!skip_this_event)
    {
        /* Add the event to the list */
        eventTemp2=pEvlistShortHead->event;
        eventTemp2.time=st;
        //lp++;
        our_event_count++;
    }
    at=pEvlistShortHead->event.time;

    //delete head event and point to next.
    next=pEvlistShortHead->next;
    *trackNum=pEvlistShortHead->trackNum;
    if(next)
    {
        //clear current
        pEvlistShortHead->next=0;
        //change next
        next->pre=0;
        //update
        pEvlistShortHead=next;
    }
    else
    {
        //for only one track or end of all tracks.
        pEvlistShortHead=0;
    }

    if(!skip_this_event)
        return (&eventTemp2);
    else
        return (MidEvent*)(-1);//skip
}

MidEvent *read_midi_file(MidIStream stream, MidSong *song)
{
    sint32 len, divisions;
    sint16 format, tracks, divisions_tmp;
    int i;
    char tmp[4];
    MidEvent *eventPtr;
    int32 res;
    //uint32 trkNum;
    int read_over=0;

    lastEvtTrkNum=0;
    song->event_count=0;
    //song->at=0; sheen
    //song->evlist=0;

    if (mid_istream_read(stream, tmp, 1, 4) <0 || mid_istream_read(stream, &len, 4, 1) <0)
    {
        DEBUG_MSG("Not a MIDI file!\n");
        return 0;
    }
    len=SWAPBE32(len);
    if (memcmp(tmp, "MThd", 4) || len < 6)
    {
        DEBUG_MSG("Not a MIDI file!\n");
        return 0;
    }

    mid_istream_read(stream, &format, 2, 1);
    mid_istream_read(stream, &tracks, 2, 1);
    mid_istream_read(stream, &divisions_tmp, 2, 1);
    song->format=format=SWAPBE16(format);
    tracks=SWAPBE16(tracks);
    divisions_tmp=SWAPBE16(divisions_tmp);

    if(tracks>MAX_TRACK)// sheen
    {
        diag_printf("midi tracks=%d MAX_TRACK=32 !!!\n",tracks);
        tracks=MAX_TRACK;
    }
    pEvlistShort=midi_alloc_small(MIDIPOOL_PERMANENT,(MAX_TRACK)*sizeof(MidEventListShort));//sheen
    if(!pEvlistShort)
    {
        diag_printf("pEvlistShort malloc err!\n");
        return 0;
    }

    /*
    pTrkPos=midi_alloc_small(MIDIPOOL_PERMANENT,(MAX_TRACK)*sizeof(uint32));//sheen
    if(!pTrkPos)
    {
        diag_printf("pTrkPos malloc err!\n");
        return 0;
    }
    memset(pTrkPos,0, (MAX_TRACK)*sizeof(uint32));
    */

    memset(&eventTemp,0,sizeof(MidEvent));//sheen
    memset(&eventTemp2,0,sizeof(MidEvent));//sheen

    pEvtRedParm=midi_alloc_small(MIDIPOOL_PERMANENT,(MAX_TRACK)*sizeof(EVT_READ_STRU));//sheen
    if(!pEvtRedParm)
    {
        diag_printf("pEvtRedParm malloc err!\n");
        return 0;
    }
    memset(pEvtRedParm,0, (MAX_TRACK)*sizeof(EVT_READ_STRU));


    if (divisions_tmp<0)
    {
        /* SMPTE time -- totally untested. Got a MIDI file that uses this? */
        divisions=
            (sint32)(-(divisions_tmp/256)) * (sint32)(divisions_tmp & 0xFF);
    }
    else divisions=(sint32)(divisions_tmp);

    song->divisions=divisions;//sheen

    if (len > 6)
    {
        //DEBUG_MSG("MIDI file header size %u bytes", len);
        mid_istream_skip(stream, len-6); /* skip the excess */
    }
    if (format<0 || format >2)
    {
        DEBUG_MSG("Unknown MIDI file format %d\n", format);
        return 0;
    }
    DEBUG_MSG("Format: %d  Tracks: %d  Divisions: %d\n",
              format, tracks, divisions);

    //recorde
    //firstTrkPos=mid_stream_seek(stream,0,FS_SEEK_CUR);

    //get all tracks pos. sheen
    for(i=0; i<tracks; i++)
    {
        /* Check the formalities */
        if (mid_istream_read(stream, tmp, 1, 4) <0 || mid_istream_read(stream, &len, 4, 1)<0)
        {
            DEBUG_MSG("Can't read track header %d\n",i);
            break;
        }
        len=SWAPBE32(len);
        if (memcmp(tmp, "MTrk", 4))
        {
            DEBUG_MSG("MTrk err in track %d\n",i);
            break;
        }

        //store cur pos
        pTrkRedParm[i].trkStart=mid_stream_seek(stream,0,FS_SEEK_CUR);
        pTrkRedParm[i].trkSize=len;
        //to next track pos
        res=mid_stream_seek(stream,len,FS_SEEK_CUR);
        if((uint32)res!=(pTrkRedParm[i].trkStart+len))
        {
            DEBUG_MSG("MTrk len err in track %d\n",i);
            break;
        }
    }

    if(i==0)
    {
        DEBUG_MSG("Corrupt MIDI file \n");
        return 0;
    }

    song->tracksNum=tracks=i;//valid tracks num


    /* Put a do-nothing event first in the list for easier processing */
#if 0 //sheen
    song->evlist=midi_alloc_small(MIDIPOOL_PERMANENT,sizeof(MidEventList));
    if(song->evlist==NULL)
        return 0;
    song->evlist=(MidEventList*)((char*)(song->evlist));
    song->evlist->event.time=0;
    song->evlist->event.type=ME_NONE;
    song->evlist->next=0;
    song->event_count++;
#endif


TRACKS_HEAD://sheen

    for(i=0; i<tracks; i++)
    {
        pTrkRedParm[i].trkPos= pTrkRedParm[i].trkStart;
        if(USE_CHUNK_BUF==0)
        {
            pTrkRedParm[i].byteLeft=0;
        }
        pTrkRedParm[i].pchunk=pTrkRedParm[i].chunk;
    }

    memset(pEvlistShort,0, (MAX_TRACK)*sizeof(MidEventListShort));
    for(i=0; i<MAX_TRACK; i++)
    {
        pEvlistShort[i].trackNum=i;
    }

    pEvlistShortHead=pEvlistShort;
    pEvlistShortTail=pEvlistShort;

    read_over+=1;

    trackAppend=0;


    memset(at_per_trk,0,MAX_TRACK*sizeof(sint32));

    switch(format)
    {
        case 0:
            trackAppend=0;
            res=read_track(stream, song, 0, 0);
            if(res==0 || res==-3)//-3= end track
            {
                if(read_over==1)
                    song->event_count++; /* Count the event. (About one?) */
                break;
            }
            else
                return 0;
        case 1:
            trackAppend=0;
            for (i=0; i<tracks; i++)
            {
                res=read_track(stream, song, 0, i);
                if(res!=0 && res!=-3)//-3= end track
                    return 0;
                if(read_over==1)
                    song->event_count++; /* Count the event. (About one?) */
            }
            break;

        case 2: /* We simply play the tracks sequentially */
            trackAppend=1;
            for (i=0; i<tracks; i++)
            {
                res=read_track(stream, song, 1, i);
                if(res!=0 && res!=-3)//-3= end track
                    return 0;
                if(read_over==1)
                    song->event_count++; /* Count the event. (About one?) */
            }
            break;
        default:
            return 0;
    }

    //eventPtr=groom_list(song, divisions, count, sp);

    for (i=0; i<16; i++)
    {
        current_bank[i]=0;
        current_set[i]=0;
        current_program[i]=song->default_program;
    }

    compute_sample_increment(song, 500000, divisions);
    our_event_count=0;
    st=at=sample_cum=0;
    counting_time=2; /* We strip any silence before the first NOTE ON. */
    if(read_over==1)
        song->totalSample=0;

#ifndef SHEEN_VC_DEBUG
//int32 tt2, tt1, ttt1=0,ttt2=0;
#endif

    //parse all and get instrument id
    while(1)
    {
#ifndef SHEEN_VC_DEBUG
// tt1=hal_TimGetUpTime();
#endif
        eventPtr=get_one_event(song, divisions, &lastEvtTrkNum);
        if(!eventPtr)
            break;

        if(read_over==1)
        {
            //get total time with sample
            if((int32)eventPtr!=-1 && eventPtr->time > song->totalSample)
                song->totalSample=eventPtr->time;
        }
#ifndef SHEEN_VC_DEBUG
// tt2=hal_TimGetUpTime();
//ttt1+=tt2-tt1;
#endif

        //if(pTrkRedParm[lastEvtTrkNum].byteLeft>0)
        if(pEvlistShort[lastEvtTrkNum].endFlag!=MAGIC_EOT)
        {
            res=read_track(stream, song, trackAppend, lastEvtTrkNum);
            if(res==0 || res==-3)//-3= end track
            {
                if(read_over==1)
                    song->event_count++; /* Count the event. (About one?) */
            }
            else
                return 0;
        }

#ifndef SHEEN_VC_DEBUG
//tt1=hal_TimGetUpTime();
//ttt2+=tt1-tt2;
#endif

        if(read_over==2 && (int32)eventPtr!=-1)//second time, -1=skip
            return eventPtr;
    }

#ifndef SHEEN_VC_DEBUG
//  diag_printf("get_one_event time=%d read_track=%d rd_t=%d sk_t=%d",ttt1*1000/16384,ttt2*1000/16384, rd_t*1000/16384,sk_t*1000/16384);
#endif

    song->groomed_event_count= our_event_count;//total event except skip.
    song->samples=st;//total sample

    //seek to 0 track pos. sheen
    //if(mid_stream_seek(stream,pTrkRedParm[0].trkStart,FS_SEEK_SET) !=(int32)pTrkRedParm[0].trkStart)
    //  return 0;

    trksParseOverFlag=1;

    goto TRACKS_HEAD;//back to tracks start position.

    return eventPtr;
}

/*************** resampling with fixed increment *****************/

static sample_t *rs_plain(MidSong *song, int v, sint32 *countptr)
{

    /* Play sample until end, then midi_freespace the voice. */
    MidVoice
    *vp=&(song->voice[v]);
    sample_t
    *dest=resample_buffer,
     *src=vp->sample->data;
    sint32
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->data_length,
    count=*countptr;
    sint32 i;

    if (incr<0)
        incr = -incr; /* In case we're coming out of a bidir loop */

    /* Precalc how many times we should go through the loop.
       NOTE: Assumes that incr > 0 and that ofs <= le */
    i = (le - ofs) / incr + 1;

    if (i > count)
    {
        i = count;
        count = 0;
    }
    else count -= i;

    while (i--)
    {
        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;
    }

    if (ofs >= le)
    {
        if (ofs == le)
            *dest++ = src[ofs >> FRACTION_BITS];
        vp->status=VOICE_FREE;
        *countptr-=count+1;
    }

    vp->sample_offset=ofs; /* Update offset */
    return resample_buffer;
}

static sample_t *rs_loop(MidSong *song, MidVoice *vp, sint32 count)
{

    /* Play sample until end-of-loop, skip back and continue. */

    sint32
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->loop_end,
    ll=le - vp->sample->loop_start;
    sample_t
    *dest=resample_buffer,
     *src=vp->sample->data;


    while (count)
    {
        if (ofs >= le)
            ofs -= ll;

        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;
        count--;
    }

    vp->sample_offset=ofs; /* Update offset */
    return resample_buffer;
}

static sample_t *rs_bidir(MidSong *song, MidVoice *vp, sint32 count)
{
    sint32
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->loop_end,
    ls=vp->sample->loop_start;
    sample_t
    *dest=resample_buffer,
     *src=vp->sample->data;
    sint32
    le2 = le<<1,
    ls2 = ls<<1,
    i;
    /* Play normally until inside the loop region */

    if (ofs <= ls)
    {
        /* NOTE: Assumes that incr > 0, which is NOT always the case
        when doing bidirectional looping.  I have yet to see a case
         where both ofs <= ls AND incr < 0, however. */
        i = (ls - ofs) / incr + 1;
        if (i > count)
        {
            i = count;
            count = 0;
        }
        else count -= i;
        if(i>0 && i<=resample_buffer_count)
        {
            while (i--)
            {
                *dest++ = src[ofs >> FRACTION_BITS];
                ofs += incr;
            }
        }
    }

    /* Then do the bidirectional looping */
    while(count)
    {
        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;
        count--;

        if (ofs>=le)
        {
            /* fold the overshoot back in */
            ofs = le2 - ofs;
            incr = -incr;
        }
        else if (ofs <= ls)
        {
            ofs = ls2 - ofs;
            incr = -incr;
        }
    }

    vp->sample_increment=incr;
    vp->sample_offset=ofs; /* Update offset */
    return resample_buffer;
}

/*********************** vibrato versions ***************************/

/* We only need to compute one half of the vibrato sine cycle */
static inline int vib_phase_to_inc_ptr(int phase)
{
    if (phase < MID_VIBRATO_SAMPLE_INCREMENTS/2)
        return MID_VIBRATO_SAMPLE_INCREMENTS/2-1-phase;
    else if (phase >= 3*MID_VIBRATO_SAMPLE_INCREMENTS/2)
        return 5*MID_VIBRATO_SAMPLE_INCREMENTS/2-1-phase;
    else
        return phase-MID_VIBRATO_SAMPLE_INCREMENTS/2;
}

static sint32 update_vibrato(MidSong *song, MidVoice *vp, int sign, sint32 fix_a_in)
{

    sint32 depth;
    int phase;
    sint32 fix_a/*,fix_a1*/,fix_pb;


    if (vp->vibrato_phase++ >= 2*MID_VIBRATO_SAMPLE_INCREMENTS-1)
        vp->vibrato_phase=0;
    phase=vib_phase_to_inc_ptr(vp->vibrato_phase);

    if (vp->vibrato_sample_increment[phase])
    {
        if (sign)
            return -vp->vibrato_sample_increment[phase];
        else
            return vp->vibrato_sample_increment[phase];
    }

    /* Need to compute this sample increment. */

    depth=vp->sample->vibrato_depth<<7;

    if (vp->vibrato_sweep)
    {
        /* Need to update sweep */
        vp->vibrato_sweep_position += vp->vibrato_sweep;
        if (vp->vibrato_sweep_position >= (1<<SWEEP_SHIFT))
            vp->vibrato_sweep=0;
        else
        {
            /* Adjust depth */
            depth *= vp->vibrato_sweep_position;
            depth >>= SWEEP_SHIFT;
        }
    }

    fix_a = fix_a_in;
    fix_pb = sinx_q15[vp->vibrato_phase]*depth>>15;

    if (fix_pb<0)
    {
        fix_pb=-fix_pb;
        fix_a = div_320_320(fix_a<<7,(bend_fine_q13[(fix_pb>>5) & 0xFF]*bend_coarse_q7[fix_pb>>13])>>13);//a /= bend_fine[(pb>>5) & 0xFF] * bend_coarse[pb>>13];
    }
    else
        fix_a = mult32_r1(fix_a,bend_fine_q13[(fix_pb>>5) & 0xFF]*bend_coarse_q7[fix_pb>>13],20);//a *= bend_fine[(pb>>5) & 0xFF] * bend_coarse[pb>>13];


    /* If the sweep's over, we can store the newly computed sample_increment */
    if (!vp->vibrato_sweep)
        vp->vibrato_sample_increment[phase]=fix_a;

    if (sign)
        fix_a = -fix_a; /* need to preserve the loop direction */


    return fix_a;


}

static sample_t *rs_vib_plain(MidSong *song, int v, sint32 *countptr)
{

    /* Play sample until end, then midi_freespace the voice. */

    MidVoice *vp=&(song->voice[v]);
    sample_t
    *dest=resample_buffer,
     *src=vp->sample->data;
    sint32
    le=vp->sample->data_length,
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    count=*countptr;
    int
    cc=vp->vibrato_control_counter;

    sint32 fix_a_input, fix_pb;

    fix_a_input =  div_320_320((vp->sample->sample_rate)<<12,(song->rate)>>8);
    fix_pb = div_320_320((vp->frequency)<<9,(vp->sample->root_freq)>>11);
    fix_a_input =  mult32_r1(fix_a_input,fix_pb,40-FRACTION_BITS);

    if (incr<0) incr = -incr; /* In case we're coming out of a bidir loop */

    while (count--)
    {
        if (!cc--)
        {
            cc=vp->vibrato_control_ratio;
            incr=update_vibrato(song, vp, 0, fix_a_input);
        }
        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;

        if (ofs >= le)
        {
            if (ofs == le)
                *dest++ = src[ofs >> FRACTION_BITS];
            vp->status=VOICE_FREE;
            *countptr-=count+1;
            break;
        }
    }

    vp->vibrato_control_counter=cc;
    vp->sample_increment=incr;
    vp->sample_offset=ofs; /* Update offset */
    return resample_buffer;
}

static sample_t *rs_vib_loop(MidSong *song, MidVoice *vp, sint32 count)
{
    /* Play sample until end-of-loop, skip back and continue. */
    sint32
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->loop_end,
    ll=le - vp->sample->loop_start;
    sample_t
    *dest=resample_buffer,
     *src=vp->sample->data;
    int
    cc=vp->vibrato_control_counter;

    sint32 fix_a_input, fix_pb;

    fix_a_input =  div_320_320((vp->sample->sample_rate)<<12,(song->rate)>>8);
    fix_pb = div_320_320((vp->frequency)<<9,(vp->sample->root_freq)>>11);
    fix_a_input =  mult32_r1(fix_a_input,fix_pb,40-FRACTION_BITS);

    while (count)
    {
        if(ofs >= le)
            ofs -= ll;

        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;

        count--;
        cc--;
        if(cc==0)
        {
            cc = vp->vibrato_control_ratio;
            incr = update_vibrato(song, vp, 0, fix_a_input);
        }
    }

    vp->vibrato_control_counter=cc;
    vp->sample_increment=incr;
    vp->sample_offset=ofs; /* Update offset */
    return resample_buffer;
}

static sample_t *rs_vib_bidir(MidSong *song, MidVoice *vp, sint32 count)
{
    sint32
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->loop_end,
    ls=vp->sample->loop_start;
    sample_t
    *dest=resample_buffer,
     *src=vp->sample->data;
    int
    cc=vp->vibrato_control_counter;
    sint32
    le2=le<<1,
    ls2=ls<<1;

    sint32 fix_a_input, fix_pb;

    fix_a_input =  div_320_320((vp->sample->sample_rate)<<12,(song->rate)>>8);
    fix_pb = div_320_320((vp->frequency)<<9,(vp->sample->root_freq)>>11);
    fix_a_input =  mult32_r1(fix_a_input,fix_pb,40-FRACTION_BITS);

    /* Play normally until inside the loop region */
    while (count && (ofs <= ls))
    {
        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;

        count--;
        cc--;
        if(cc==0)
        {
            cc = vp->vibrato_control_ratio;
            incr = update_vibrato(song, vp, 0, fix_a_input);
        }
    }

    /* Then do the bidirectional looping */
    while (count)
    {
        *dest++ = src[ofs >> FRACTION_BITS];
        ofs += incr;

        count--;
        cc--;
        if(cc==0)
        {
            cc = vp->vibrato_control_ratio;
            incr = update_vibrato(song, vp, 0, fix_a_input);
        }

        if (ofs >= le)
        {
            ofs = le2 - ofs;
            incr = -incr;
        }
        else if (ofs <= ls)
        {
            ofs = ls2 - ofs;
            incr = -incr;
        }
    }

    vp->vibrato_control_counter=cc;
    vp->sample_increment=incr;
    vp->sample_offset=ofs; /* Update offset */
    return resample_buffer;
}

sample_t *resample_voice(MidSong *song, int v, sint32 *countptr)
{
    sint32 ofs;
    uint8 modes;
    MidVoice *vp=&(song->voice[v]);

    if (!(vp->sample->sample_rate))
    {
        /* Pre-resampled data -- just update the offset and check if
           we're out of data. */
        ofs=vp->sample_offset >> FRACTION_BITS; /* Kind of silly to use
                         FRACTION_BITS here... */
        if (*countptr >= (vp->sample->data_length>>FRACTION_BITS) - ofs)
        {
            /* Note finished. Free the voice. */
            vp->status = VOICE_FREE;

            /* Let the caller know how much data we had left */
            *countptr = (vp->sample->data_length>>FRACTION_BITS) - ofs;
        }
        else
            vp->sample_offset += *countptr << FRACTION_BITS;

        return vp->sample->data+ofs;
    }

    /* Need to resample. Use the proper function. */
    modes=vp->sample->modes;

    if (vp->vibrato_control_ratio)
    {
        if ((modes & MODES_LOOPING) &&
                ((modes & MODES_ENVELOPE) ||
                 (vp->status==VOICE_ON || vp->status==VOICE_SUSTAINED)))
        {
            if (modes & MODES_PINGPONG)
                return rs_vib_bidir(song, vp, *countptr);
            else
                return rs_vib_loop(song, vp, *countptr);
        }
        else
            return rs_vib_plain(song, v, countptr);
    }
    else
    {
        if ((modes & MODES_LOOPING) &&
                ((modes & MODES_ENVELOPE) ||
                 (vp->status==VOICE_ON || vp->status==VOICE_SUSTAINED)))
        {
            if (modes & MODES_PINGPONG)
                return rs_bidir(song, vp, *countptr);
            else
                return rs_loop(song, vp, *countptr);
        }
        else
            return rs_plain(song, v, countptr);
    }
}


////movable part///////
size_t  midi_Fread(void *dest, size_t unitlen, size_t size, MidIStream fp)
{
    size_t l = 0;
#ifdef  SHEEN_VC_DEBUG
    l = fread(dest,unitlen,size,fp);
    /*
       if (l!=size)
        {
            DEBUG_MSG("***midi_midi_Freadmidi_Fread error***\n");
            return -1;
        }*/
#else

//int32 tt1=hal_TimGetUpTime();

    l = FS_Read(fp,dest,unitlen*size);

//int32 tt2=hal_TimGetUpTime();
//rd_t+=tt2-tt1;

#endif

    return l;
}

/*
for load_instrument_from_array
sheen
*/
static size_t mid_read_array (void *ptr, size_t size)
{
    memcpy(ptr,pResample_buffer,size);
    pResample_buffer+=size;
    return size;
}

size_t mid_istream_read (MidIStream stream, void *ptr, size_t size, size_t nmemb)
{
    size_t len;

    len=size*nmemb;
    if(USE_CHUNK_BUF==1)
    {
        //pTrkRedParm[0].byteLeft keep the buffer size.
        if(trkReadFlag!=255)
        {
            if(len <= pTrkRedParm[trkReadFlag].trkSize-(pTrkRedParm[trkReadFlag].trkPos-pTrkRedParm[trkReadFlag].trkStart))
                memcpy(ptr,pTrkRedParm[0].chunk+pTrkRedParm[trkReadFlag].trkPos,len);
            else
            {
                len=pTrkRedParm[trkReadFlag].trkSize-(pTrkRedParm[trkReadFlag].trkPos-pTrkRedParm[trkReadFlag].trkStart);
                memcpy(ptr,pTrkRedParm[0].chunk+pTrkRedParm[trkReadFlag].trkPos,len);
            }
            pTrkRedParm[trkReadFlag].trkPos+=len;
        }
        else
        {
            if(pTrkRedParm[0].pchunk+len-pTrkRedParm[0].chunk <=pTrkRedParm[0].byteLeft)
                memcpy(ptr,pTrkRedParm[0].pchunk,len);
            else
            {
                len=pTrkRedParm[0].byteLeft-(pTrkRedParm[0].pchunk-pTrkRedParm[0].chunk);
                if(len>0)
                    memcpy(ptr,pTrkRedParm[0].pchunk,len);
            }
            pTrkRedParm[0].pchunk+=len;
        }
        return len;
    }
    else
    {
        if(trkReadFlag!=255)
        {
            if(len<=pTrkRedParm[trkReadFlag].byteLeft)
            {
                memcpy(ptr,pTrkRedParm[trkReadFlag].pchunk,len);
                pTrkRedParm[trkReadFlag].pchunk +=len;
                pTrkRedParm[trkReadFlag].byteLeft-=len;
                return len;
            }
            else
            {
                size_t count;
                memcpy(ptr,pTrkRedParm[trkReadFlag].pchunk,pTrkRedParm[trkReadFlag].byteLeft);
                len-=pTrkRedParm[trkReadFlag].byteLeft;
                ptr=(char*)ptr+pTrkRedParm[trkReadFlag].byteLeft;

                //seek to read position
#ifndef SHEEN_VC_DEBUG
                FS_Seek(stream,(INT64)pTrkRedParm[trkReadFlag].trkPos,FS_SEEK_SET);
#else
                fseek((INT32)stream, pTrkRedParm[trkReadFlag].trkPos,FS_SEEK_SET);
#endif

                if(len<FILE_TEMP_CHUNK_SIZE)
                {
                    count=pTrkRedParm[trkReadFlag].trkSize-(uint32)(pTrkRedParm[trkReadFlag].trkPos-pTrkRedParm[trkReadFlag].trkStart);
                    if(count > FILE_TEMP_CHUNK_SIZE)
                        count=FILE_TEMP_CHUNK_SIZE;

                    count=midi_Fread(pTrkRedParm[trkReadFlag].chunk, 1,count,stream);

                    pTrkRedParm[trkReadFlag].pchunk=pTrkRedParm[trkReadFlag].chunk;
                    if(count>0)
                    {
                        pTrkRedParm[trkReadFlag].byteLeft=count;
                        pTrkRedParm[trkReadFlag].trkPos+=count;
                    }
                    else
                    {
                        pTrkRedParm[trkReadFlag].byteLeft=0;
                    }

                    if(pTrkRedParm[trkReadFlag].byteLeft<FILE_TEMP_CHUNK_SIZE)
                        memset(pTrkRedParm[trkReadFlag].pchunk+pTrkRedParm[trkReadFlag].byteLeft,0,
                               FILE_TEMP_CHUNK_SIZE-pTrkRedParm[trkReadFlag].byteLeft);

                    if(len<=pTrkRedParm[trkReadFlag].byteLeft)
                        count=len;
                    else
                        count=pTrkRedParm[trkReadFlag].byteLeft;

                    memcpy(ptr,pTrkRedParm[trkReadFlag].pchunk,count);
                    pTrkRedParm[trkReadFlag].pchunk+=count;
                    pTrkRedParm[trkReadFlag].byteLeft-=count;
                    len-=count;
                    return (size*nmemb - len);
                }
                else
                {
                    count=pTrkRedParm[trkReadFlag].trkSize-(uint32)(pTrkRedParm[trkReadFlag].trkPos-pTrkRedParm[trkReadFlag].trkStart);
                    if(count > len)
                        count=len;

                    count=midi_Fread(ptr, 1,count,stream);
                    if(count>0)
                    {
                        len-=count;
                        pTrkRedParm[trkReadFlag].trkPos+=count;
                    }

                    count=pTrkRedParm[trkReadFlag].trkSize-(uint32)(pTrkRedParm[trkReadFlag].trkPos-pTrkRedParm[trkReadFlag].trkStart);
                    if(count > FILE_TEMP_CHUNK_SIZE)
                        count=FILE_TEMP_CHUNK_SIZE;

                    count=midi_Fread(pTrkRedParm[trkReadFlag].chunk, 1,count,stream);
                    if(count>0)
                    {
                        pTrkRedParm[trkReadFlag].byteLeft=count;
                        pTrkRedParm[trkReadFlag].trkPos+=count;
                    }
                    else
                        pTrkRedParm[trkReadFlag].byteLeft=0;
                    pTrkRedParm[trkReadFlag].pchunk=pTrkRedParm[trkReadFlag].chunk;

                    if(pTrkRedParm[trkReadFlag].byteLeft<FILE_TEMP_CHUNK_SIZE)
                        memset(pTrkRedParm[trkReadFlag].pchunk+pTrkRedParm[trkReadFlag].byteLeft,0,
                               FILE_TEMP_CHUNK_SIZE-pTrkRedParm[trkReadFlag].byteLeft);

                    return (size*nmemb - len);
                }
            }
        }
        else
        {
            return midi_Fread(ptr, 1,size*nmemb,stream);
        }
    }
}

void mid_istream_skip (MidIStream stream, size_t len)
{
    mid_stream_seek(stream,len,FS_SEEK_CUR);
}

static int32 mid_stream_seek(MidIStream stream, size_t size, int32 origin)
{
    int32 res=0;

    if(size<0)return res;

    if(USE_CHUNK_BUF==1)
    {
        switch(origin)
        {
            case FS_SEEK_CUR:

                if(trkReadFlag!=255)
                {
                    if(size <= pTrkRedParm[trkReadFlag].trkSize-(pTrkRedParm[trkReadFlag].trkPos-pTrkRedParm[trkReadFlag].trkStart))
                    {
                        pTrkRedParm[trkReadFlag].trkPos+=size;
                    }
                    else
                    {
                        pTrkRedParm[trkReadFlag].trkPos=pTrkRedParm[trkReadFlag].trkStart+ pTrkRedParm[trkReadFlag].trkSize;
                    }

                    res=pTrkRedParm[trkReadFlag].trkPos;
                }
                else
                {
                    if(size <= pTrkRedParm[0].byteLeft-(pTrkRedParm[0].pchunk-pTrkRedParm[0].chunk))
                    {
                        pTrkRedParm[0].pchunk+=size;
                    }
                    else
                    {
                        pTrkRedParm[0].pchunk=pTrkRedParm[0].chunk+ pTrkRedParm[0].byteLeft;
                    }

                    res=pTrkRedParm[0].pchunk-pTrkRedParm[0].chunk;

                }

                break;
            case FS_SEEK_SET:

                if(size <= pTrkRedParm[0].byteLeft)
                {
                    pTrkRedParm[0].pchunk += size;
                }
                else
                {
                    pTrkRedParm[0].pchunk += pTrkRedParm[0].byteLeft;
                }
                res=pTrkRedParm[0].pchunk-pTrkRedParm[0].chunk;

                break;
            case FS_SEEK_END:

                if(size <= pTrkRedParm[0].byteLeft)
                {
                    pTrkRedParm[0].pchunk += pTrkRedParm[0].byteLeft- size;
                }
                else
                    pTrkRedParm[0].pchunk=pTrkRedParm[0].chunk;

                res=pTrkRedParm[0].pchunk-pTrkRedParm[0].chunk;

                break;
        }
    }
    else
    {
        if(trkReadFlag!=255)
        {
            switch(origin)
            {
                case FS_SEEK_CUR:

                    if(size <= pTrkRedParm[trkReadFlag].byteLeft)
                    {
                        pTrkRedParm[trkReadFlag].pchunk+=size;
                        pTrkRedParm[trkReadFlag].byteLeft-=size;
                        size=0;
                    }
                    else
                    {
                        size-=pTrkRedParm->byteLeft;
                        pTrkRedParm[trkReadFlag].pchunk=pTrkRedParm[trkReadFlag].chunk;
                        pTrkRedParm[trkReadFlag].byteLeft=0;
                    }

                    if(size>0)
                    {
#ifndef SHEEN_VC_DEBUG
                        res=(int32)FS_Seek(stream,(INT64)pTrkRedParm[trkReadFlag].trkPos +size,FS_SEEK_SET);
#else
                        fseek((INT32)stream, pTrkRedParm[trkReadFlag].trkPos +size, FS_SEEK_SET);
                        res=ftell(stream);
#endif
                        if(res>=0)
                            pTrkRedParm[trkReadFlag].trkPos=res;
                    }

                    break;
                case FS_SEEK_SET:

                    if(size > pTrkRedParm[trkReadFlag].trkStart+pTrkRedParm[trkReadFlag].trkSize)
                        size=pTrkRedParm[trkReadFlag].trkStart+pTrkRedParm[trkReadFlag].trkSize;

                    pTrkRedParm[trkReadFlag].pchunk=pTrkRedParm[trkReadFlag].chunk;
                    pTrkRedParm[trkReadFlag].byteLeft=0;
#ifndef SHEEN_VC_DEBUG
                    res=(int32)FS_Seek(stream,(__int64)size,origin);
#else
                    fseek((INT32)stream, size, origin);
                    res=ftell(stream);
#endif
                    if(res>=0)
                        pTrkRedParm[trkReadFlag].trkPos=res;

                    break;
                case FS_SEEK_END:
                    pTrkRedParm[trkReadFlag].pchunk=pTrkRedParm[trkReadFlag].chunk;
                    pTrkRedParm[trkReadFlag].byteLeft=0;
#ifndef SHEEN_VC_DEBUG
                    res=(int32)FS_Seek(stream,(__int64)size,origin);
#else
                    fseek((INT32)stream, size, origin);
                    res=ftell(stream);
#endif
                    if(res>=0)
                        pTrkRedParm[trkReadFlag].trkPos=res;

                    break;
            }
        }
        else
        {
#ifndef SHEEN_VC_DEBUG
            res=(int32)FS_Seek(stream,(INT64)size,origin);
#else
            fseek((INT32)stream, size, origin);
            res=ftell(stream);
#endif

        }
    }
    return res;
}



//midi memory management//////
/* This'll allocate memory or die. */


void *safe_malloc(size_t count)
{
    void *p;

#ifdef MIDI_DEBUG_MODE
    total_mem_count1+=count;
#endif

#ifndef SHEEN_VC_DEBUG
    //p=COS_SHMEMMALLOC(count);
    p=mmc_MemMalloc(count);
#else
    p=malloc(count);
#endif
//DEBUG_MSG("safe_malloc p=%x,count=%d,total=%d \n",p,count,total_mem_count1);

    if (p == NULL)
        DEBUG_MSG("Sorry. Couldn't malloc %d bytes.\n", count);

    return p;
}

#if 0//sheen
void midi_freespace(void* p,size_t count)
{
//#ifdef MIDI_DEBUG_MODE
//  total_mem_count1 -= count;
    //DEBUG_MSG("midi_freespace p=%x,count=%d,total_mem_count1=%d",p,count,total_mem_count1);
//#endif

    DEBUG_MSG("midi_freespace p=%x,count=%d\n",p,count);
#ifndef SHEEN_VC_DEBUG
    COS_SHMEMFREE(p);
#else
    free(p);
#endif
}
#endif

void *midi_alloc_small (int pool_id, size_t sizeofobject)
/* Allocate a "small" object */
{

    //midi_pool_ptr  hdr_ptr, prev_hdr_ptr;
    char * data_ptr;
    //size_t odd_bytes, min_request, slop;

#if 0
    /* Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE) */
    odd_bytes = sizeofobject % sizeof(ALIGN_TYPE);
    if (odd_bytes > 0)
        sizeofobject += sizeof(ALIGN_TYPE) - odd_bytes;

    /* See if space is available in any existing pool */
    if (pool_id < 0 || pool_id >=MIDIPOOL_NUMPOOLS)
        return NULL;

    prev_hdr_ptr = NULL;
    hdr_ptr = small_list[pool_id];
    while (hdr_ptr != NULL)
    {
        if (hdr_ptr->bytes_left >= sizeofobject)
            break;            /* found pool with enough space */
        prev_hdr_ptr = hdr_ptr;
        hdr_ptr = hdr_ptr->next;
    }

    /* Time to make a new pool? */
    if (hdr_ptr == NULL)
    {
        /* min_request is what we need now, slop is what will be leftover */
        min_request = sizeofobject + sizeof(midi_pool_hdr);
        slop = pool_slop[pool_id];
        /* Try to get space, if fail reduce slop and try again */
        {
            hdr_ptr = (midi_pool_ptr) safe_malloc(min_request + slop);
            if (hdr_ptr == NULL)
                return NULL;
        }
        //total_mem_count += min_request + slop;
        /* Success, initialize the new pool header and add to end of list */
        hdr_ptr->next = NULL;
        hdr_ptr->bytes_used = 0;
        hdr_ptr->bytes_left = sizeofobject + slop;

        if (prev_hdr_ptr == NULL)   /* first pool in class? */
            small_list[pool_id] = hdr_ptr;
        else
            prev_hdr_ptr->next = hdr_ptr;
    }

    /* OK, allocate the object from the current pool */
    data_ptr = (char *) (hdr_ptr + 1); /* point to first data byte in pool */
    data_ptr += hdr_ptr->bytes_used; /* point to place for object */
    hdr_ptr->bytes_used += sizeofobject;
    hdr_ptr->bytes_left -= sizeofobject;
#else

    data_ptr=safe_malloc(sizeofobject);

#endif

    return (void *) data_ptr;
}

#if 0// sheen
int midi_free_pool (int pool_id)
{

    midi_pool_ptr  shdr_ptr;
    size_t space_freed;

    if (pool_id < 0 || pool_id >= MIDIPOOL_NUMPOOLS)
        return -1;

#if 0
    /* Release small objects */
    shdr_ptr = small_list[pool_id];
    small_list[pool_id] = NULL;

    while (shdr_ptr != NULL)
    {
        midi_pool_ptr next_shdr_ptr = shdr_ptr->next;
        space_freed = shdr_ptr->bytes_used + shdr_ptr->bytes_left + sizeof(midi_pool_hdr);
        midi_freespace((void *) shdr_ptr, space_freed);
#ifdef MIDI_DEBUG_MODE
        if(pool_id==MIDIPOOL_TEMP)
            total_mem_count1 -= space_freed;
        //DEBUG_MSG("midi_freespace p=%x,count=%d,total_mem_count1=%d",p,count,total_mem_count1);
#endif
        //total_mem_count -= space_freed;

        shdr_ptr = next_shdr_ptr;
    }
#endif

    DEBUG_MSG(" midi_free_pool \n" );

    return 0;
}
#endif

void midi_free_all()
{
    //int i;
    //uint8 res;
    /*
    for (i=MIDIPOOL_NUMPOOLS-1;i>=0;i--)
    {
         midi_free_pool(i);
    }*/
#ifndef SHEEN_VC_DEBUG
    mmc_MemFreeAll();
#else
    int i;
    uint8 res;
    if(master_tonebank)
    {
        if(master_tonebank->tone)
        {
            free(master_tonebank->tone);
            master_tonebank->tone=0;
        }
        for (i=0; i<(128); i++)
        {
            if(master_tonebank->instrument[i])
            {
                if(master_tonebank->instrument[i]->sample)
                {
                    if(master_tonebank->instrument[i]->sample->data)
                    {
                        if(pgm_use_flag)
                            res=pgm_use_flag[i];
                        else
                            res=255;

                        if(res==i)//not free repeat
                            free(master_tonebank->instrument[i]->sample->data);
                        else if(res!=255 && res > i)
                            pgm_use_flag[res]=res;

                        master_tonebank->instrument[i]->sample->data=0;
                    }
                    free(master_tonebank->instrument[i]->sample);
                    master_tonebank->instrument[i]->sample=0;
                }
                free(master_tonebank->instrument[i]);
                master_tonebank->instrument[i]=0;
            }
        }
        free(master_tonebank);
        master_tonebank=0;
    }
    if(master_drumset)
    {
        if(master_drumset->tone)
        {
            free(master_drumset->tone);
            master_drumset->tone=0;
        }
        for (i=27; i<88; i++)
        {
            if(master_drumset->instrument[i])
            {
                if(master_drumset->instrument[i]->sample)
                {
                    if(master_drumset->instrument[i]->sample->data)
                    {
                        if(pgm_use_flag)
                            res=pgm_use_flag[i+101];
                        else
                            res=255;

                        if(res==(i+101))//not free repeat
                            free(master_drumset->instrument[i]->sample->data);
                        else if(res!=255 && res > (i+101))
                            pgm_use_flag[res]=res;

                        master_drumset->instrument[i]->sample->data=0;
                    }
                    free(master_drumset->instrument[i]->sample);
                    master_drumset->instrument[i]->sample=0;
                }
                free(master_drumset->instrument[i]);
                master_drumset->instrument[i]=0;
            }
        }
        free(master_drumset);
        master_drumset=0;
    }

    if(song1)
    {
        free(song1);
        song1=0;
    }

    if(pgm_use_flag)
    {
        free(pgm_use_flag);
        pgm_use_flag=0;
    }
    if(common_buffer)
    {
        free(common_buffer);
        common_buffer=0;
    }
    if(pEvlistShort)
    {
        free(pEvlistShort);
        pEvlistShort=0;
    }
    if(pTrkRedParm)
    {
        free(pTrkRedParm);
        pTrkRedParm=0;
    }
    if(pEvtRedParm)
    {
        free(pEvtRedParm);
        pEvtRedParm=0;
    }

#endif


#ifdef MIDI_DEBUG_MODE
    DEBUG_MSG("midi_free_all,total_mem_count1 = %d",total_mem_count1 );
#endif
}

#ifndef SHEEN_VC_DEBUG
/////////////interfaces to mmi///////////////////////////////////////////////////////////
int32 Audio_MidiPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
    int32 result=MCI_ERR_NO;
    int32 filesize = 0;
    MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MpegPlay,fhd is %d",fhd);

    g_MidiBT8KEnbleFlag=FALSE;//TRUE;

    //g_FileHandle=fhd;
    if (fhd< 0)
    {
        diag_printf("[MMC_AUDIO_ERROR]ERROR file hander: %d \n", fhd);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }

    if ((filesize=(INT32)FS_GetFileSize(fhd))< 0)
    {
        diag_printf("[MMC_AUDIO_ERROR]can not get file size!:%d",filesize);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }


    result = MMC_Midi2Raw(fhd,filesize,PlayProgress);
    return  result;
}

int32 Audio_MidiStop (void)
{
    //clear the flag used to play left data in the input buffer and pcm buffer
    //ErrorFrameCount=0;
    //ContinuePlayFlag=0;
    MMC_MidiDecClose();
    mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
    return  MCI_ERR_NO;
}

int32 Audio_MidiPause (void)
{
    MMC_MidiDecClose();
    mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
    return  MCI_ERR_NO;
}


int32 Audio_MidiResume ( HANDLE fhd)
{
    int32 result=MCI_ERR_NO;
    int32 filesize = 0;
    MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MidiResume,fhd is %d",fhd);

    //g_FileHandle=fhd;
    if (fhd< 0)
    {
        diag_printf("[MMC_AUDIO_ERROR]ERROR file hander: %d \n", fhd);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }

    if ((filesize=(INT32)FS_GetFileSize(fhd))< 0)
    {
        diag_printf("[MMC_AUDIO_ERROR]can not get file size!:%d",filesize);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }


    result = MMC_Midi2Raw(fhd,filesize,0);

    return  result;
}


int32 Audio_MidiGetID3 (char * pFileName)
{
    return MCI_ERR_NO;
}
int32 Audio_MidiGetPlayInformation (MCI_PlayInf * MCI_PlayInfWAV)
{
    MCI_PlayInfWAV->PlayProgress=(INT32)(((INT64)song1->current_sample*10000)/song1->samples);

    return MCI_ERR_NO;
}

#endif

uint32 midi_GetDurationTime(
    HANDLE FileHander,
    INT32 filesize,
    INT32 BeginPlayProgress,
    INT32 OffsetPlayProgress
)
{

    INT32 rate = HAL_AIF_FREQ_16000HZ;//22050;//32000;
    INT32 bits = 16;
    //INT32 volume = 60;
    int channels = 1;// 2;
    MidSongOptions options;
    uint32  tim1,tim2;
#ifndef SHEEN_VC_DEBUG
    if(MpegPlayer.MpegInput.bt_sco==TRUE)
    {
        rate = 8000;
    }
#endif

    diag_printf("midi_GetDurationTime");
    USE_CHUNK_BUF=0;
    pEvlistShort=0;
    trksParseOverFlag=0;
    trkReadFlag=255;


#ifdef MIDI_DEBUG_MODE
    total_mem_count1 = 0;
#endif
    if(mid_init_no_config())
    {
        midi_free_all();//mmc_MemFreeAll();
#ifndef SHEEN_VC_DEBUG
        MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
        mmc_SetCurrMode(MMC_MODE_IDLE);
#endif
        return MIDIDEC_ERROR_OUTOF_SPACE;
    }


    options.rate = rate;
    options.format = (bits == 16) ? MID_AUDIO_S16LSB : MID_AUDIO_S8;
    options.channels = channels;
    options.buffer_size = MIDBUFFERSIZE / (bits * channels / 8);

    song1 = mid_song_load ((MidIStream)FileHander, &options);

    if (song1 == NULL)
    {
        DEBUG_MSG ("Invalid MIDI file\n");
        midi_free_all();//mmc_MemFreeAll();
        return 0;
    }

    //mid_song_set_volume (song1, volume);
    //mid_song_start (song1);

    if(BeginPlayProgress)
    {
        BeginPlayProgress =(INT32)( ((INT64)BeginPlayProgress*song1->samples)/10000);
        skip_to(song1, BeginPlayProgress);
        tim1 = mid_song_get_time(song1);
    }
    else
        tim1 = 0;

    if(OffsetPlayProgress)
    {
        OffsetPlayProgress =(INT32)( ((INT64)OffsetPlayProgress*song1->samples)/10000);
        skip_to(song1, OffsetPlayProgress);
        tim2 = mid_song_get_time(song1);
    }
    else
        tim2 = 0;

    midi_free_all();//mmc_MemFreeAll();
    //diag_printf("****t1=%d  t2=%d  time diff  is %d******",tim1,tim2,(tim2-tim1));

    return (tim2-tim1);


}


#ifndef SHEEN_VC_DEBUG

int32 Audio_MidiUserMsg(int32 nMsg)
{
    int bytes_read=0;
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;

    switch (nMsg)
    {
#if 0
        case MSG_MMC_MIDI_STOP:

            MMC_MidiDecClose();
            mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
            MCI_AudioFinished(MCI_ERR_END_OF_FILE);
            DEBUG_MSG ("*MSG_MMC_MIDI_STOP**  end of file\n");

            break;
#endif

        case MSG_MMC_MID_SBC_VOC:
            if(skip_vocopen_isr == FALSE)
            {
                if(is_SSHdl_valid(my_ss_handle))
                {
                    /*            if(voc_data_remain== -1){
                                      voc_data_remain= 0;
                                     diag_printf("skip for new data.\n");
                                     break;
                                  }*/
                    //fill sbc data to bt
                    bytes_read = SSHdl_GetFreeSpace(my_ss_handle);//get bt free space.
                    //diag_printf("free space %d\n",bytes_read);
                    if(voc_data_remain==0)
                    {
                        vpp_AudioJpegDecStatus(&Voc_AudioDecStatus);
                        voc_data_remain= Voc_AudioDecStatus.ImgWidth;
                        if(bytes_read > voc_data_remain)
                        {
                            bytes_read = voc_data_remain;
                        }
                    }
                    else if(bytes_read >= voc_data_remain)
                    {
                        bytes_read = voc_data_remain;
                    }
                    //BT_A2DP_sbc
                    //my_ss_handle->PutData(my_ss_handle, (UINT8*) hal_VocGetPointer(SBC_Out_put_addr),Voc_AudioDecStatus.ImgWidth);
                    my_ss_handle->PutData(my_ss_handle, (UINT8*) hal_VocGetPointer(SBC_Out_put_addr)+(Voc_AudioDecStatus.ImgWidth- voc_data_remain),bytes_read);
                    voc_data_remain -= bytes_read;
                    if(voc_data_remain> 0)
                    {
                        sxr_Sleep(16*16);//half time of 512sample 16k
                        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MID_SBC_VOC);
                        //diag_printf("putdata %d voc_data_remain=%d\n",bytes_read,voc_data_remain);
                    }
                    else
                    {
                        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MIDI_PLAY);
                        //diag_printf("req next frame. putdata %d\n",bytes_read);
                    }
                    //diag_printf("PutData over.size=%d\n",bytes_read);
                }
                else
                {
                    voc_data_remain =0;
                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MIDI_PLAY);
                    diag_printf("bt stop. req pcm play.\n");
                }
            }
            else
            {
                skip_vocopen_isr= FALSE;
                diag_printf("skip voc open isr\n");
            }
            break;

        case MSG_MMC_MIDI_PLAY:
            /*      if(voc_data_remain>0){
                    voc_data_remain= -1;
                    diag_printf("drop voc remain data\n");
                    }*/

            if(is_SSHdl_valid(my_ss_handle))
            {
                Mmc_Midi_Pcm_Half= 1- Mmc_Midi_Pcm_Half;
            }

            //enc last half buf.
            MMC_MidiVocSbcEnc(((char*)pcmbuf_overlay)+(1-Mmc_Midi_Pcm_Half)*MIDBUFFERSIZE,MIDBUFFERSIZE,song1);

            bytes_read = mid_song_read_wave (song1, (INT32*)(pcmbuf_overlay)+Mmc_Midi_Pcm_Half*(MIDBUFFERSIZE>>2), MIDBUFFERSIZE);

#ifdef AUD_3_IN_1_SPK

            if(audioItf==AUD_ITF_LOUD_SPEAKER&&bytes_read>0)
            {

                INT16 * VibratorDataAddr=(INT16 *)((INT32*)(pcmbuf_overlay)+Mmc_Midi_Pcm_Half*(MIDBUFFERSIZE>>2));
                INT32 VibratorDataLength=(MIDBUFFERSIZE>>1);
                INT32 VibratorDataSample=HAL_AIF_FREQ_16000HZ;
                INT32 VibratorDataCh=1;

                //INT32 timebegin=hal_TimGetUpTime();
                mmc_Vibrator(VibratorDataAddr,VibratorDataLength,VibratorDataAddr ,g_MCIAudioVibrator, VibratorDataSample,VibratorDataCh);
                //INT32 timeend_xuml=hal_TimGetUpTime();
                //MCI_TRACE (TSTDOUT,0,"Vibrator time = %dms",((timeend_xuml-timebegin)*1000)/16384);

            }

#endif


            if(bytes_read > 0)
            {
                INT16 *MIDI_out = (INT16 *)((INT32*)(pcmbuf_overlay)+Mmc_Midi_Pcm_Half*(MIDBUFFERSIZE>>2));
                INT16 alg_gain = aud_GetOutAlgGainDb2Val();

                for(INT32 i = 0; i < (MIDBUFFERSIZE>>1); i++)
                {
                    MIDI_out[i] = ((INT32)MIDI_out[i] * alg_gain) >>14;
                }
            }

            //reopen pcm play when bt state change.
            if(bt_state != is_SSHdl_valid(my_ss_handle) /*&&Mmc_Midi_Pcm_Half==1*/ && bytes_read>0)
            {
                bt_state = is_SSHdl_valid(my_ss_handle);//update
                if(bt_state == FALSE)
                {
                    memset(pcmbuf_overlay,0,MIDBUFFERSIZE*2);
                    audioItf= AUD_ITF_LOUD_SPEAKER;
                    MMC_MidiPcmOutStart( (INT32*)(pcmbuf_overlay));
                }
                else if(audioItf != AUD_ITF_BLUETOOTH)
                {
                    audioItf= AUD_ITF_EAR_PIECE;
                    aud_StreamStop(last_audioItf);
                }
                diag_printf("change bt play state");
            }

            if(sco_state != MpegPlayer.MpegInput.bt_sco)
            {
                bytes_read= -1;
                midi_g_loop=0;
                diag_printf("change bt sco play state");
            }

            //DEBUG_MSG ("***  MSG_MMC_MIDI_PLAY ***  bytes_read=%d\n",bytes_read);
            if (bytes_read<=0)
            {
                if(midi_g_loop)
                {
                    //memset(pcmbuf_overlay,0,MIDBUFFERSIZE*2);
                    song1->playing = 1;
                    skip_to(song1, 0);
                    bytes_read = mid_song_read_wave (song1, (INT32*)(pcmbuf_overlay)+Mmc_Midi_Pcm_Half*(MIDBUFFERSIZE>>2), MIDBUFFERSIZE);
                    DEBUG_MSG ("*** play midi in loop=%d  bytes_read=%d***\n",midi_g_loop,bytes_read);

#ifdef AUD_3_IN_1_SPK

                    if(audioItf==AUD_ITF_LOUD_SPEAKER&&bytes_read>0)
                    {
                        INT16 * VibratorDataAddr=(INT16 *)((INT32*)(pcmbuf_overlay)+Mmc_Midi_Pcm_Half*(MIDBUFFERSIZE>>2));;
                        INT32 VibratorDataLength=(MIDBUFFERSIZE>>1);
                        INT32 VibratorDataSample=HAL_AIF_FREQ_16000HZ;
                        INT32 VibratorDataCh=1;

                        //INT32 timebegin=hal_TimGetUpTime();
                        mmc_Vibrator(VibratorDataAddr,VibratorDataLength,VibratorDataAddr ,g_MCIAudioVibrator, VibratorDataSample,VibratorDataCh);
                        //INT32 timeend_xuml=hal_TimGetUpTime();
                        //MCI_TRACE (TSTDOUT,0,"Vibrator time = %dms",((timeend_xuml-timebegin)*1000)/16384);
                    }

#endif

                    if(bytes_read > 0)
                    {
                        INT16 *MIDI_out = (INT16 *)((INT32*)(pcmbuf_overlay)+Mmc_Midi_Pcm_Half*(MIDBUFFERSIZE>>2));
                        INT16 alg_gain = aud_GetOutAlgGainDb2Val();

                        for(INT32 i = 0; i < (MIDBUFFERSIZE>>1); i++)
                        {
                            MIDI_out[i] = ((INT32)MIDI_out[i] * alg_gain) >>14;
                        }
                    }

                    if (bytes_read<=0)
                    {
                        MMC_MidiDecClose();
                        if(MCI_RingFinished!=NULL)
                            MCI_RingFinished(MCI_ERR_END_OF_FILE);
                        mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
                        DEBUG_MSG ("***Fail to play midi in loop***\n");
                    }
                }
                else
                {
                    MMC_MidiDecClose();

                    if(midi_g_buffer)
                    {
                        MCI_RingFinished(MCI_ERR_END_OF_FILE);
                    }
                    else
                    {
                        MCI_AudioFinished(MCI_ERR_END_OF_FILE);
                    }

                    mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
                    DEBUG_MSG ("*MSG_MMC_MIDI_PLAY**  end of file\n");
                }
            }

            break;
#if 0
        case    MSG_MMC_MIDI_ERROR:
            MMC_MidiDecClose();
            mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
            if(midi_g_buffer)
                MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
            else
                MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
            break;
#endif
        default:
            DEBUG_MSG("[MMC_AUDIO]##WARNING##**************Error message******************!\n");
            break;
    }

    return 0;
}
#endif

#else
#include "cswtype.h"
#include "fs.h"
#include "mci.h"
#include "mmc_midi.h"
int32 Audio_MidiPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
    return 1;
}
int32 Audio_MidiPause (void)
{
    return 1;
}
int32 Audio_MidiResume ( HANDLE fhd)
{
    return 1;
}
int32 Audio_MidiGetID3 (char * pFileName)
{
    return 1;
}
int32 Audio_MidiStop (void)
{
    return 1;
}
int32 Audio_MidiUserMsg(int32 nMsg)
{
    return 1;
}
int32 Audio_MidiGetPlayInformation (MCI_PlayInf * MCI_PlayInfWAV)
{
    return 1;
}
MIDIDEC_ERROR_TYPE  MMC_Midi2RawBuffer(INT32 *pBuffer,  INT32 len, UINT8 loop)
{
    return 1;
}
uint32 midi_GetDurationTime(HANDLE FileHander,
                            INT32 filesize,
                            INT32 BeginPlayProgress,
                            INT32 OffsetPlayProgress)
{
    return 1;
}
#endif

#ifdef SHEEN_VC_DEBUG
sint32  mult32_r1(sint32 x,sint32 y,sint32 r)
{
    sint32  xlow,ylow,xhi,yhi;
    sint32  t1,t2;
    sint32  l = 32-r;
    uint32  t3;

    xhi = x>>16;
    xlow= x&0xffff;

    yhi = y>>16;
    ylow= y&0xffff;

    t1 = xhi*yhi<<l;

    if(r>16)
        t2 = (xhi*ylow + xlow*yhi)>>(r-16);//ֵɿ  ûѡ64λ
    else
        t2 = (xhi*ylow + xlow*yhi)<<(16-r);//ֵɿ  ûѡ64λ

    t3 = (uint32)(xlow*ylow);
    t3 = t3>>r;
    t1 = t1+t2+(sint32)t3;

    return t1;

}

void main()
{
    FILE *pFileIn;
    FILE *pFileOut;
    char *pFileBuf;
    int res;

    pFileIn=fopen("/home/sheen/data_f/testfile/midi/incoming_call_5.mid","rb");
    pFileOut=fopen("/home/sheen/data_f/testfile/midi/test55.pcm","wb");
    pcmbuf_overlay=malloc(MIDBUFFERSIZE*2);

#if 0
    res=midi_GetDurationTime(pFileIn, 0,0, 0);
    fseek(pFileIn,0,FS_SEEK_SET);
#endif

#if 1
    res=MMC_Midi2Raw(pFileIn,0,0);
#else
    fseek(pFileIn,0,FS_SEEK_END);
    res=ftell(pFileIn);
    pFileBuf=malloc(res);
    fseek(pFileIn,0,FS_SEEK_SET);
    fread(pFileBuf,1,res,pFileIn);
    res=MMC_Midi2RawBuffer(pFileBuf,res,0);
#endif
    if(res)return;



    mid_song_get_total_time(song1);

    while(1)
    {
        res=mid_song_read_wave (song1, (INT32*)(pcmbuf_overlay), MIDBUFFERSIZE*2);
        if(res<=0)break;
        fwrite(pcmbuf_overlay,1,MIDBUFFERSIZE*2,pFileOut);
    }

    //repeat
#if 0
    song1->playing=1;
    skip_to(song1,0);
    while(1)
    {
        res=mid_song_read_wave (song1, (INT32*)(pcmbuf_overlay), MIDBUFFERSIZE*2);
        if(res<=0)break;
        fwrite(pcmbuf_overlay,1,MIDBUFFERSIZE*2,pFileOut);
    }
#endif

    midi_free_all();

    fclose(pFileIn);
    fclose(pFileOut);
    free(pcmbuf_overlay);
}
#endif
