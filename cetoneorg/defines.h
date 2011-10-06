#pragma once

//#define CODEBUG

#ifdef CODEBUG
#include "log.h"
#endif

#define nCLAMP(min, val, max) ((val > max) ? max : ((val < min) ? min : val))

#define PI 3.14159265f
#define PId 3.14159265358979323846
#define UNDENORM(v) if( (*((unsigned __int32*)&v) & 0x7f800000) == 0) v = 0
#define DENORM_CONST 1e-15f

#define SINE_SIZE 2048
#define SINE_SIZEf 2048.0f

#define EFFECT_NAME "CetoneOrg"
#define CLASS_NAME "CetoneOrg"
#define VENDOR_NAME "Neotec Software"
#define PRODUCT_NAME "CetoneOrg"
#define VERSION_NUMBER 5
#define REAL_VERSION 0x006000

typedef struct
{
	unsigned __int16 frac;
	__int16 index;
} sFIXED;

typedef union
{
	sFIXED fixed;
	__int32 int32;
} sSTEP;

#if _M_IX86_FP > 0
#define truncate(x) ((int)(x))
#else
__forceinline int truncate(float flt)
{
    int i;
    _asm
    {
        fld flt
        fistp i
    }
    return i;
}
#endif

enum kParams
{
	pDb0, pDb1, pDb2, pDb3, pDb4, pDb5, pDb6, pDb7, pDb8,
	pTone, pClick, pVolume,
	pLfoSpeed, pVibDepth, pPanPhase, pPanDepth,
	pChDelayL, pChDelayR, pChDepth, pChSpeed, pChFeedback, pChMix,
	pRevDamp, pRevRoom, pRevWidth, pRevWet, pRevDry,
	pOutVol,

	pNumParameters
};

#define UINT8 unsigned __int8

typedef struct
{
	int version;
	char name[26];
	UINT8 bars[9];
	UINT8 volume;
	UINT8 tone;
	UINT8 vibDepth;
	UINT8 lfoSpeed;
	UINT8 panPhase;
	UINT8 panDepth;
	UINT8 click;
	UINT8 c_delayl;
	UINT8 c_delayr;
	UINT8 c_depth;
	UINT8 c_speed;
	UINT8 c_mix;
	UINT8 r_damp;
	UINT8 r_room;
	UINT8 r_width;
	UINT8 r_dry;
	UINT8 r_wet;
	UINT8 outvol;
	UINT8 c_feedback;
	UINT8 pad[6];
} PROGRAM;
