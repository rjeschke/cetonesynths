#include <math.h>
#include <stdio.h>

#include "cetone033.h"

void wave2str(int wave, char* text)
{
	switch(wave)
	{
	case WAVE_SAW:		vst_strncpy (text, "Saw", kVstMaxParamStrLen);		break;
	case WAVE_PULSE:	vst_strncpy (text, "Pulse", kVstMaxParamStrLen);	break;
	case WAVE_TRI:		vst_strncpy (text, "Tri", kVstMaxParamStrLen);	break;
	default:			text[0] = 0;										break;
	}
}

void myfloat2string(float val, char* text)
{
	char tmp[256];
	sprintf(tmp, "%f", val);
	vst_strncpy(text, tmp, kVstMaxParamStrLen);
}

void bool2string(bool val, char* text)
{
	vst_strncpy (text, (val) ? "On" : "Off", kVstMaxParamStrLen);
}

float c_val2cutoff(float value)
{
	float tmp = floor(value * 1000.f) / 1000.f;

	return tmp * CCetone033::SampleRate2;
}

float c_cutoff2val(float value)
{
	float tmp = value / CCetone033::SampleRate2;

	tmp = floor(tmp * 1000.f) / 1000.f;

	return tmp;
}

bool c_val2bool(float value)
{
	return (value < 0.5f) ? false : true;
}

float c_bool2val(bool value)
{
	return (value) ? 1.f : 0.f;
}

int c_val2coarse(float value)
{
	return (int)(value * 100.f + 0.5f) - 50;
}

float c_coarse2val(int value)
{
	return (float)(value + 50) / 100.f;
}

int c_val2fine(float value)
{
	return (int)(value * 200.f + 0.5f) - 100;
}

float c_fine2val(int value)
{
	return (float)(value + 100) / 200.f;
}

int pf2i(float val, int max)
{
	int tmp = (int)floor(((float)(max + 1) * (float)val) + 0.5f);

	if(tmp < 0)
		tmp = 0;
	else if(tmp > max)
		tmp = max;

	return tmp;
}

float pi2f(int val, int max)
{
	return (float)val / (float)(max + 1);
}