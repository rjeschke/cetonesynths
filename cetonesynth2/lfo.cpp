#include "lfo.h"
#include <math.h>

LFO::LFO()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		float f = (float)(i + 10) / 128.0f;
		f *= f;
		this->m2f[i] = powf(2.0f, f * 3.0f) - 1.0f;
	}

	this->mDelay = 0;
	this->Speed = 0;
	this->SetSampleRate(44100.0f);
	this->SetTempo(0);
	this->SetPW(64);
	this->SetSpeed(0);
	this->SetRetrigger(0);
	this->SetSNH(0);
	this->SetDelay(0);
	this->SetWave(OSCW_SAW);
	this->SetSync(0);
	this->Reset();
}

LFO::~LFO()
{
}

void LFO::SetPW(int pw)
{
	if(pw < 1)
		pw = 1;
	else if(pw > 126)
		pw = 126;
	this->PW = pw * (WAVETABLE_LENGTH / 128);
}

void LFO::SetWave(int wave)
{
	this->Wave = wave;
}

void LFO::Reset()
{
	this->Index.int32 = 0;
	this->lastNoise = 0.0f;
	this->SampleOutput = 0.0f;
}

void LFO::SetSampleRate(float fs)
{
	this->FS = fs;
	this->SetDelay(this->mDelay);
	this->SetSpeed(this->Speed);
}

void LFO::Retrigger()
{
	this->Gate = true;
	if(this->mDelay)
		this->Delay = this->DelayLen;
	else
		this->Delay = 0;
	if(this->isRetrigger)
		this->Index.int32 = 0;
}

void LFO::SetTempo(float f)
{
	if(this->Tempo == f)
		return;

	this->Tempo = f;
	if(this->Sync)
		this->SetSpeed(this->Speed);
}

void LFO::SetSync(int sync)
{
	this->Sync = (sync) ? true : false;
	this->SetSpeed(this->Speed);
}

void LFO::SetSNH(int snh)
{
	this->SNH = (snh) ? true : false;
	if(this->SNH)
	{
		this->SampleOutput = 0;
		this->Gate = false;
	}
}

void LFO::SetRetrigger(int retrig)
{
	this->isRetrigger = (retrig) ? true : false;
}

void LFO::SetDelay(int delay)
{
	this->mDelay = delay;
	this->DelayLen = truncate(this->FS * this->m2f[delay]);
	if(!delay)
		this->Delay = 0;
}

