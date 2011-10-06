#include <math.h>
#include <stdio.h>

#include "cetonesynth.h"

void wave2str(int wave, char* text)
{
	switch(wave)
	{
	case WAVE_SINE:		vst_strncpy (text, "Sine", kVstMaxParamStrLen);		break;
	case WAVE_TRI:		vst_strncpy (text, "Tri", kVstMaxParamStrLen);		break;
	case WAVE_SAW:		vst_strncpy (text, "Saw", kVstMaxParamStrLen);		break;
	case WAVE_PULSE:	vst_strncpy (text, "Pulse", kVstMaxParamStrLen);	break;
	case WAVE_C64NOISE:	vst_strncpy (text, "Noise", kVstMaxParamStrLen);	break;
	default:			text[0] = 0;										break;
	}
}

void owave2str(int wave, char* text)
{
	switch(wave)
	{
	case OWAVE_SINE:		vst_strncpy (text, "Sine", kVstMaxParamStrLen);		break;
	case OWAVE_TRI:			vst_strncpy (text, "Tri", kVstMaxParamStrLen);		break;
	case OWAVE_SAW:			vst_strncpy (text, "Saw", kVstMaxParamStrLen);		break;
	case OWAVE_PULSE:		vst_strncpy (text, "Pulse", kVstMaxParamStrLen);	break;
	case OWAVE_C64NOISE:	vst_strncpy (text, "Noise", kVstMaxParamStrLen);	break;
	default:			text[0] = 0;										break;
	}
}

void fmode2str(int mode, char* text)
{
	switch(mode)
	{
	case FMODE_LOW:		vst_strncpy (text, "Low", kVstMaxParamStrLen);		break;
	case FMODE_HIGH:	vst_strncpy (text, "High", kVstMaxParamStrLen);		break;
	case FMODE_BAND:	vst_strncpy (text, "Band", kVstMaxParamStrLen);		break;
	case FMODE_NOTCH:	vst_strncpy (text, "Notch", kVstMaxParamStrLen);	break;
	default:			text[0] = 0;										break;
	}
}

void ftype2str(int type, char* text)
{
	switch(type)
	{
	case FTYPE_NONE:	vst_strncpy (text, "None", kVstMaxParamStrLen);		break;
	case FTYPE_DIRTY:	vst_strncpy (text, "Dirty", kVstMaxParamStrLen);	break;
	case FTYPE_MOOG:	vst_strncpy (text, "Moog", kVstMaxParamStrLen);		break;
	case FTYPE_MOOG2:	vst_strncpy (text, "Moog2", kVstMaxParamStrLen);	break;
	case FTYPE_CH12DB:	vst_strncpy (text, "Ch12db", kVstMaxParamStrLen);	break;
	case FTYPE_303:		vst_strncpy (text, "x0x", kVstMaxParamStrLen);		break;
	case FTYPE_8580:	vst_strncpy (text, "8580", kVstMaxParamStrLen);		break;
	case FTYPE_BUDDA:	vst_strncpy (text, "Bi12db", kVstMaxParamStrLen);	break;
	default:			text[0] = 0;										break;
	}
}

void msrc2str(int val, char* text)
{
	switch(val)
	{
	case MOD_SRC_NONE:			vst_strncpy (text, "None", kVstMaxParamStrLen);		break;
	case MOD_SRC_VEL:			vst_strncpy (text, "Vel.", kVstMaxParamStrLen);		break;
	case MOD_SRC_CTRL1:			vst_strncpy (text, "Ctrl 1", kVstMaxParamStrLen);	break;
	case MOD_SRC_MENV1:			vst_strncpy (text, "MEnv", kVstMaxParamStrLen);	break;
	case MOD_SRC_LFO1:			vst_strncpy (text, "LFO", kVstMaxParamStrLen);	break;
	case MOD_SRC_MENV1xLFO1:	vst_strncpy (text, "ME1xL1", kVstMaxParamStrLen);	break;
	default:					text[0] = 0;										break;
	}
}

void mdest2str(int val, char* text)
{
	switch(val)
	{
	case MOD_DEST_MAINVOL:		vst_strncpy (text, "Volume", kVstMaxParamStrLen);	break;
	case MOD_DEST_CUTOFF:		vst_strncpy (text, "Cutoff", kVstMaxParamStrLen);	break;
	case MOD_DEST_RESONANCE:	vst_strncpy (text, "Q", kVstMaxParamStrLen);		break;
	case MOD_DEST_PANNING:		vst_strncpy (text, "Pan.", kVstMaxParamStrLen);		break;
	case MOD_DEST_MAINPITCH:	vst_strncpy (text, "Pitch", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC1VOL:		vst_strncpy (text, "Vol 1", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC2VOL:		vst_strncpy (text, "Vol 2", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC3VOL:		vst_strncpy (text, "Vol 3", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC1PITCH:	vst_strncpy (text, "Pitch1", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC2PITCH:	vst_strncpy (text, "Pitch2", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC3PITCH:	vst_strncpy (text, "Pitch3", kVstMaxParamStrLen);	break;
	case MOD_DEST_OSC1PW:		vst_strncpy (text, "PW 1", kVstMaxParamStrLen);		break;
	case MOD_DEST_OSC2PW:		vst_strncpy (text, "PW 2", kVstMaxParamStrLen);		break;
	case MOD_DEST_OSC3PW:		vst_strncpy (text, "PW 3", kVstMaxParamStrLen);		break;
	case MOD_DEST_LFO1SPEED:	vst_strncpy (text, "L1Spd.", kVstMaxParamStrLen);	break;
	case MOD_DEST_ENVMOD:		vst_strncpy (text, "F.Param.", kVstMaxParamStrLen);	break;
	default:					text[0] = 0;										break;
	}
}

void arp2str(int val, char* text)
{
	switch(val)
	{
	case -1:	vst_strncpy (text, "Off", kVstMaxParamStrLen);		break;
	case 0:		vst_strncpy (text, "Minor", kVstMaxParamStrLen);	break;
	case 1:		vst_strncpy (text, "Major", kVstMaxParamStrLen);	break;
	case 2:		vst_strncpy (text, "MinOct", kVstMaxParamStrLen);	break;
	case 3:		vst_strncpy (text, "MajOct", kVstMaxParamStrLen);	break;
	case 4:		vst_strncpy (text, "Octave", kVstMaxParamStrLen);	break;
	case 5:		vst_strncpy (text, "Oct2", kVstMaxParamStrLen);		break;
	case 6:		vst_strncpy (text, "Quint", kVstMaxParamStrLen);	break;
	case 7:		vst_strncpy (text, "Quint2", kVstMaxParamStrLen);	break;
	default:	text[0] = 0;										break;
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

	return tmp * CCetoneSynth::SampleRate2;
}

float c_cutoff2val(float value)
{
	float tmp = value / CCetoneSynth::SampleRate2;

	tmp = floor(tmp * 1000.f) / 1000.f;

	return tmp;
}

int c_val2pw(float value)
{
	return (int)(value * 65536.f + 0.5f);
}

float c_pw2val(int value)
{
	return (float)value / 65536.f;
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

