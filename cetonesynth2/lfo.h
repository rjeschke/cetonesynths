#pragma once

#include "defines.h"
#include "statics.h"

class LFO
{
public:
	LFO();
	~LFO();
	void SetWave(int wave);
	void SetPW(int pw);
	void SetTempo(float tempo);
	void SetSync(int sync);
	void SetRetrigger(int retrig);
	void SetSNH(int snh);
	void SetDelay(int delay);
	void Reset();
	void Retrigger();
	void SetSampleRate(float fs);
	int GetPos() { return this->Index.fixed.index; };
	__forceinline float Process()
	{
		if(!this->Wave)
			return 0.0f;

		if(this->Delay)
		{
			this->Delay--;
			return 0.0f;
		}
		
		float output = 0.0f;
		
		switch(this->Wave)
		{
		case OSCW_SAW:
			output = g_lfo_saw[this->Index.fixed.index];
			break;
		case OSCW_PULSE:
			if(this->Index.fixed.index < this->PW)
				output = -1.0f;
			else
				output = 1.0f;
			break;
		case OSCW_TRI:
			output = g_lfo_tri[this->Index.fixed.index];
			break;
		case OSCW_NOISE:
			output = this->lastNoise;
			break;
		}
		
		int old = this->Index.int32;
		this->Index.int32 = (this->Index.int32 + this->Step.int32) & (WAVETABLE_MASK * 0x10000 + 0xffff);
		if(old > this->Index.int32)
			this->lastNoise = nrand();

		if(this->SNH)
		{
			if(this->Gate)
			{
				this->Gate = false;
				this->SampleOutput = output;
			}

			return this->SampleOutput;
		}
		else
			return output;
	}

	__forceinline void SetSpeed(int speed)
	{
		if(speed < 0)
			speed = 0;
		else if(speed > 4095)
			speed = 4095;
		this->Speed = speed;
		if(this->Sync)
		{
			if(this->Tempo)
				this->Step.int32 = truncate((WAVETABLE_LENGTHf * 65536.0f) / (this->Tempo * g_syncSpeed[(speed >> 5) + 1]) + 0.5f);
			else
				this->Step.int32 = 0;
		}
		else
			this->Step.int32 = g_lfo_step[speed];
	}

private:
	float	m2f[4096];
	float	FS;
	float	Tempo;
	float	SampleOutput;
	float	lastNoise;
	int		Wave;
	int		PW;
	int		Speed;
	int		mDelay;
	int		DelayLen;
	int		Delay;
	oSTEP	Index;
	oSTEP	Step;
	bool	Sync;
	bool	isRetrigger;
	bool	SNH;
	bool	Gate;
};
