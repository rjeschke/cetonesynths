#include <math.h>
#include "defines.h"
#include "envelope.h"

Envelope::Envelope()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		float f = (float)(i + 5) / 128.0f;
		f *= f;
		this->m2f[i] = powf(2.0f, f * 5.0f) - 1.0f;
	}
	this->Reset();
}

Envelope::~Envelope()
{
}

void Envelope::SetSampleRate(float fs)
{
	this->FS = fs;
	this->Reset();
}

void Envelope::Attack(int val)
{
	this->A = this->m2f[val & 127];
}

void Envelope::Hold(int val)
{
	this->H = this->m2f[val & 127];
}

void Envelope::Decay(int val)
{
	this->D = this->m2f[val & 127];
}

void Envelope::Sustain(int val)
{
	this->S = (float)(val & 127) / 127.0f;
}

void Envelope::Release(int val)
{
	this->R = this->m2f[val & 127];
}

void Envelope::GateOn()
{
	if(this->state != sOff)
	{
		this->state = sPreAttack;
		this->samples = truncate(m2f[0] * this->FS);
		this->step = this->Value / (float)this->samples;
	}
	else
	{
		this->Value = 0.0f;
		this->state = sAttack;
		this->samples = truncate(this->A * this->FS);
		this->step = 1.0f / (float)this->samples;
	}
}

void Envelope::GateOff()
{
	if(this->state != sOff)
	{
		this->state = sRelease;
		this->samples = truncate(this->R * this->FS);
		this->step = this->Value / (float)this->samples;
	}
}

void Envelope::Reset()
{
	this->Value = 0.0f;
	this->state = sOff;
}
