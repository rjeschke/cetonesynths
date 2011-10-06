#pragma once

//#define CS2DEBUG

#ifdef CS2DEBUG
#include "log.h"
#endif

#define nCLAMP(min, val, max) ((val > max) ? max : ((val < min) ? min : val))

#define PI 3.14159265f
#define PId 3.14159265358979323846
#define UNDENORM(v) if( (*((unsigned __int32*)&v) & 0x7f800000) == 0) v = 0

#define NOTE_MAX 120
#define NOTE_MAX1 (NOTE_MAX-1)
#define PITCH_MAX 12000
#define PITCH_MAX1 (PITCH_MAX-1)

#define WAVETABLE_LENGTH 2048
#define WAVETABLE_LENGTHf 2048.0f
#define WAVETABLE_LENGTH1 (WAVETABLE_LENGTH-1)
#define WAVETABLE_LENGTH2 (WAVETABLE_LENGTH/2)
#define WAVETABLE_LENGTH4 (WAVETABLE_LENGTH/4)
#define WAVETABLE_MASK (WAVETABLE_LENGTH-1)

#define OSCW_OFF 0
#define OSCW_SAW 1
#define OSCW_PULSE 2
#define OSCW_TRI 4
#define OSCW_NOISE 8

#define OSCM_NORMAL 0
#define OSCM_ADD 1
#define OSCM_MUL 2
#define OSCM_SUPER 3

#define EFFECT_NAME "CetoneSynth2"
#define CLASS_NAME "CetoneSynth2"
#define VENDOR_NAME "Neotec Software"
#define PRODUCT_NAME "CetoneSynth2"
#define VERSION_NUMBER 6
#define REAL_VERSION 0x007500

typedef struct
{
	unsigned __int16 frac;
	__int16 index;
} sFIXED;

typedef union
{
	sFIXED fixed;
	__int32 int32;
} oSTEP;

typedef struct
{
	unsigned __int8 semi;
	unsigned __int8 fine;
	unsigned __int8 morph;
	unsigned __int8 flags;
	unsigned __int8 wave;
	unsigned __int8 mode;
	unsigned __int8 spread;
	unsigned __int8 pad;
} sOSC;	// 8

typedef struct
{
	unsigned __int8 a, h, d, s, r;
	unsigned __int8 pad[3];
} sENV;	// 8

typedef struct
{
	unsigned __int8 speed;
	unsigned __int8 delay;
	unsigned __int8 pw;
	unsigned __int8 keytrack;
	unsigned __int8 wave;
	unsigned __int8 sync;
	unsigned __int8 snh;
	unsigned __int8 retrig;
} sLFO;	// 8

typedef struct
{
	unsigned __int8 source;
	unsigned __int8 amount;
	unsigned __int8 dest;
	unsigned __int8 pad[1];
} sMOD;	// 4

typedef struct
{
	unsigned __int8 cutoff;
	unsigned __int8 q;
	unsigned __int8 gain;
	unsigned __int8 velsens;
	unsigned __int8 env2;
	unsigned __int8 db;
	unsigned __int8 mode;
	unsigned __int8 exp;
} sFILT;	// 8

typedef struct
{
	unsigned __int8 volume;
	unsigned __int8 velsens;
	unsigned __int8 panning;
	unsigned __int8 osc1;
	unsigned __int8 osc2;
	unsigned __int8 osc3;
	unsigned __int8 pad[2];
} sMIX;	// 8

typedef struct
{
	unsigned __int8 flags;
	unsigned __int8 speed;
	unsigned __int8 length;
	unsigned __int8 loop;
	unsigned __int8 sync;
	unsigned __int8 note[8];
	unsigned __int8 pad[3];
} sARP;	// 16

typedef struct 
{
	__int32 version;	// 4
	char name[26];		// + 26		= 30
	sOSC osc[3];		// + 3 * 8	= 54
	sLFO lfo[2];		// + 2 * 8	= 70
	sENV env[3];		// + 3 * 8	= 94
	sFILT filter;		// + 8		= 102
	sMIX mixer;			// + 8		= 110
	sMOD mod[6];		// + 6 * 4	= 134
	sARP arp;			// + 16		= 150
	unsigned __int8 voices;	// 151
	unsigned __int8 glide;	// 152
	unsigned __int8 audioVol;
	unsigned __int8 audioPan;
	unsigned __int8 pad[102];
} sPRG;

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
