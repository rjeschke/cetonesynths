#pragma once

#include "defines.h"
#include "statics.h"

class LFO
{
public:
	LFO()
	{
		this->Speed = 10.0f;
		this->Pos.int32 = this->Step.int32 = 0;
		this->Phase = SINE_SIZE / 2;
	}

	~LFO()
	{
	}

	void setSpeed(float speed)
	{
		this->Speed = speed;
		this->Step.int32 = truncate(((SINE_SIZEf * speed) / this->FS) * 65536.0f + 0.5f);
	}

	void setPhase(int phase)
	{
		this->Phase = phase * (SINE_SIZE / 128);
	}

	void setSampleRate(float fs)
	{
		this->FS = fs;
		this->setSpeed(this->Speed);
	}

	void Render(float* buf0, float* buf1, int samples)
	{
		int i;
		for(i = 0; i < samples; i++)
		{
			buf0[i] = g_sine[this->Pos.fixed.index];
			buf1[i] = g_sine[(this->Pos.fixed.index + this->Phase) & (SINE_SIZE-1)];
			this->Pos.int32 += this->Step.int32;
			this->Pos.fixed.index &= SINE_SIZE - 1;
		}
	}

private:
	float FS;
	float Speed;
	sSTEP Pos;
	sSTEP Step;
	int Phase;
};