#pragma once

#define UNDENORM(v) if((*(unsigned __int32*)&v & 0x7f800000) == 0) v = 0.0f

#define FILTER_DELAY 256

#define FILTER_TYPE_BIQUAD 0
#define FILTER_TYPE_MOOG 1

#define FILTER_TYPE_MAX 1

#define MOD_CHANGE_SPEED 0.05f

#define START_VOLUME 0.01f

#define NOTE_MAX 144
#define NOTE_MAX1 143
#define PITCH_MAX 14400
#define PITCH_MAX1 14399
#define BASE_FREQUENCE 6.875f
#define NOTE_OFFSET 3

#define WAVETABLE_LENGTH 2048
#define WAVETABLE_LENGTHf 2048.f
#define WAVETABLE_LENGTH2 1024
#define WAVETABLE_LENGTH4 512
#define WAVETABLE_MASK 2047

#define EFFECT_NAME "Cetone033"
#define VENDOR_NAME "Neotec Software"
#define PRODUCT_NAME "Cetone033"
#define VERSION_NUMBER 1000;

#define WAVE_SAW 0
#define WAVE_PULSE 1
#define WAVE_TRI 2

#define WAVE_MAX 2
