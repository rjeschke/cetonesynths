#include "CetoneSynth.h"
#include "SynthLfo.h"
#include <math.h>

CSynthLfo::CSynthLfo(void)
{
	this->SetSpeed(0.05f);
	this->SetPw(32768);
	this->SetTrigger(false);
	this->SetWave(WAVE_SAW);

	this->Reset();
}

CSynthLfo::~CSynthLfo(void)
{
}

void CSynthLfo::Reset()
{
	this->OscNow = 0;
	this->Value = 0.f;
	this->ShiftRegister = 0x7ffff8;
}

void CSynthLfo::Set(float speed, int pw, int wave, bool trig)
{
	if(speed < 0.00001f)
		speed = 0.00001f;

	this->Pitch = speed;

    float t2;
	t2 = (speed * 65536.f) / CCetoneSynth::SampleRate;
    this->OscStep = truncate(t2 * 16384.f + 0.5f);

	if (pw < 0)
		pw = 0;
	else if(pw > 65536)
		pw = 65536;

	this->PulseWidth = pw;

	if (wave < 0)
		wave = 0;
	else if(wave > WAVE_MAX)
		wave = WAVE_MAX;

	this->Waveform = wave;

	this->DoTrigger = trig;
}

void CSynthLfo::SetSpeed(float speed)
{
	if(speed < 0.00001f)
		speed = 0.00001f;

	this->Pitch = speed;

    float t2;
	t2 = (speed * 65536.f) / CCetoneSynth::SampleRate;
    this->OscStep = truncate(t2 * 16384.f + 0.5f);
}

float CSynthLfo::GetSpeed()
{
	return this->Pitch;
}

void CSynthLfo::SetPw(int pw)
{
	if (pw < 0)
		pw = 0;
	else if(pw > 65536)
		pw = 65536;

	this->PulseWidth = pw;
}

int CSynthLfo::GetPw()
{
	return this->PulseWidth;
}

void CSynthLfo::SetWave(int wave)
{
	if (wave < 0)
		wave = 0;
	else if(wave > WAVE_MAX)
		wave = WAVE_MAX;

	this->Waveform = wave;
}

int CSynthLfo::GetWave()
{
	return this->Waveform;
}

void CSynthLfo::SetTrigger(bool trig)
{
	this->DoTrigger = trig;
}

bool CSynthLfo::GetTrigger()
{
	return this->DoTrigger;
}

void CSynthLfo::Trigger()
{
	if (this->DoTrigger)
	{
		this->ShiftRegister = 0x7ffff8;
		this->OscNow = 0;
	}
}

float CSynthLfo::Run()
{
	int sr = this->ShiftRegister;
	int ret;
	int osc = this->OscNow >> 14;

	float output = 0.f;

	switch(this->Waveform)
	{
	case WAVE_SAW:
		output = CCetoneSynth::Int2FloatTab2[osc];
		break;
	case WAVE_PULSE:
		if (osc < this->PulseWidth)
			output = -1.f;
		else
			output = 1.f;
		break;
	case WAVE_TRI:
        if (osc < 0x8000)
            ret = osc << 1;
        else
            ret = 131071 - (osc << 1);
		output = CCetoneSynth::Int2FloatTab2[ret];
        break;
	case WAVE_SINE:
		output = CCetoneSynth::SineTable[osc];
		break;
	case WAVE_C64NOISE:
		ret = ((sr & 0x400000) >> 11) |
			((sr & 0x100000) >> 10) |
			((sr & 0x010000) >> 7) |
			((sr & 0x002000) >> 5) |
			((sr & 0x000800) >> 4) |
			((sr & 0x000080) >> 1) |
			((sr & 0x000010) << 1) |
			((sr & 0x000004) << 2);
		output = CCetoneSynth::Int2FloatTab2[ret << 4];
		break;
	default:
		break;
	}

    this->OscNow += this->OscStep;

    if (this->OscNow > 0x3fffffff)
    {
		int bit0 = ((sr >> 22) ^ (sr >> 17)) & 0x1;
        sr <<= 1;
        sr &= 0x7fffff;
        sr |= bit0;
		this->OscNow &= 0x3fffffff;
    }

    this->Value = output;

	return output;
}
