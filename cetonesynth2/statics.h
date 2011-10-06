#pragma once

#include "defines.h"

#define RANDa 1664525
#define RANDc 1013904223

extern float	g_sine[WAVETABLE_LENGTH];
extern float	g_freqtab[PITCH_MAX + 1];
extern __int32	g_lookup[65536];
extern float	g_saw[(NOTE_MAX + 25) * WAVETABLE_LENGTH * 2];
extern float	g_parabola[(NOTE_MAX + 25) * WAVETABLE_LENGTH * 2];
extern __int32	g_step[PITCH_MAX];

extern __int32	g_semi[128];
extern __int32	g_fine[128];
extern float	g_m2f0[128];
extern float	g_m2f1[128];
extern float	g_m2co[128];
extern float	g_m2vol[128];
extern float	g_m2fgain[128];
extern float	g_m2fenv[128];
extern float	g_m2mod[128];

extern int		g_osc_pw[128 * 16];
extern float	g_osc_pwo[128 * 16];
extern float	g_osc_tris[128 * 16];

extern unsigned __int32 _RND;
extern __int32 _RND1, _RND2;

extern double	g_fsin[32768];
extern double	g_fcos[32768];

extern float	g_lfo_saw[WAVETABLE_LENGTH * 2];
extern float	g_lfo_tri[WAVETABLE_LENGTH * 2];
extern __int32	g_lfo_step[4096];

extern const float g_syncSpeed[132];

void initStatics(float fs);
float spline(float t, float* ps);
float ntanh(float x);
int irand();

__forceinline float nrand()
{
	
    _RND1 ^= _RND2;
    float ret = _RND2 * (2.0f / 0xffffffff);
    _RND2 += _RND1;
	return ret;

	/*
	_RND = (_RND * RANDa) + RANDc;
	return g_i2f2[_RND >> 16];
	*/
}

/*
__forceinline float ntanh(float x)
{
	int ix;
	
	if(x < 0.0f)
	{
		x *= -200.0f;
		ix = truncate(x);
		return -(_tanh[ix] + (x - (float)ix) * (_tanh[ix + 1] - _tanh[ix]));
	}
	else
	{
		x *= 200.0f;
		ix = truncate(x);
		return (_tanh[ix] + (x - (float)ix) * (_tanh[ix + 1] - _tanh[ix]));
	}
}
*/