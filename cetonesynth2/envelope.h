#pragma once

#include "defines.h"

enum envState
{
	sOff, sAttack, sHold, sDecay, sSustain, sRelease, sPreAttack
};

class Envelope
{
public:
	Envelope();
	~Envelope();
	void SetSampleRate(float fs);
	void Attack(int val);
	void Hold(int val);
	void Decay(int val);
	void Sustain(int val);
	void Release(int val);
	void GateOn();
	void GateOff();
	__forceinline float Process()
	{
		float val = this->Value;
		switch(this->state)
		{
		case sOff:
			val = 0.0f;
			break;
		case sAttack:
			val += this->step;
			if(--this->samples <= 0)
			{
				val = 1.0f;
				this->state = sHold;
				this->samples = truncate(this->H * this->FS);
			}
			else
			{
				if(val > 1.0f)
					val = 1.0f;
			}
			break;
		case sHold:
			if(--this->samples <= 0)
			{
				this->state = sDecay;
				this->samples = truncate(this->D * this->FS);
				this->step = (1.0f - this->S) / (float)this->samples;
			}
			break;
		case sDecay:
			val -= this->step;
			if(--this->samples <= 0)
			{
				val = this->S;
				this->state = sSustain;
			}
			else
			{
				if(val < this->S)
					val = this->S;
			}
			break;
		case sSustain:
			break;
		case sRelease:
			val -= this->step;
			if(--this->samples <= 0)
			{
				val = 0.0f;
				this->state = sOff;
			}
			else
			{
				if(val < 0.0f)
					val = 0.0f;
			}
			break;
		case sPreAttack:
			val -= this->step;
			if(--this->samples <= 0)
			{
				this->state = sAttack;
				this->samples = truncate(this->A * this->FS);
				this->step = 1.0f / (float)this->samples;
				val = 0.0f;
			}
			else
			{
				if(val < 0.0f)
					val = 0.0f;
			}
			break;
		}

		this->Value = val;
		return val;
	}

	void Reset();
	int state;
private:
	float Value;
	float FS;
	float A, H, D, S, R;
	float step;
	int samples;
	float m2f[128];
};
