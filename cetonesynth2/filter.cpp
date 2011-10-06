#include "filter.h"
#include "statics.h"
#include <math.h>

Filter::Filter()
{
	this->Reset();

	this->SetSampleRate(44100);
	this->SetMode(FILTM_12DB);
	this->SetType(FILTT_LP);
	this->Set(1.0f, 0.0f);
	this->SetGain(1.0f);
}

Filter::~Filter()
{
}

void Filter::Reset()
{
	this->lastIn = this->lastIn2 = 0.0;
	for(int i = 0; i < 6; i++)
	{
		this->buffer0[i] = 0.0;
		this->buffer1[i] = 0.0;
		this->buffer2[i] = 0.0;
	}
}

void Filter::SetMode(int mode)
{
#ifdef FILTER_INLINE
	this->modus = mode;
#else
	if(mode)
		this->Process = &Filter::Process24;
	else
		this->Process = &Filter::Process12;
#endif
}

void Filter::SetType(int type)
{
	if(type != this->type)
	{
		this->Reset();
		this->type = type;
	}
}

void Filter::SetSampleRate(float fs)
{
	double omega = PId * BPCO / (double)(fs * 0.5f);
	double cs = cos(omega);
	double sn = sin(omega);

	double a0 = 1.0 + sn;
	this->b0b = sn / a0;
	this->b1b = 0.0;
	this->b2b = -this->b0b;
	this->a1b = (2.0 * cs) / a0;
	this->a2b = (sn - 1.0) / a0;

	this->Reset();
}

// Oversampled, 24dB
/*
	Well, my first aproach using standard biquads just sounded ... sounded ...
	hmm ... well, no-good. This one is much more CPU consuming (5 biquad sections)
	but sounds really nice.
	A Neotec-filter ... my own ... no other filter sounds like this^^
*/

#ifndef FILTER_INLINE
float Filter::Process24(float _inputout)
{
	double inputout = (double)_inputout;
	this->buffer2[2] = this->buffer2[1];
	this->buffer2[1] = this->buffer2[0];
	this->buffer2[0] = inputout;
	this->buffer2[5] = this->buffer2[4];
	this->buffer2[4] = this->buffer2[3];
	this->buffer2[3] = 
		this->b0b * this->buffer2[0] 
		+ this->b1b * this->buffer2[1] 
		+ this->b2b * this->buffer2[2]
		+ this->a1b * this->buffer2[4] 
		+ this->a2b * this->buffer2[5] + DENORM_CONST;

	inputout = DRY * inputout + WET * this->buffer2[3];
	
	this->buffer0[2] = this->buffer0[1];
	this->buffer0[1] = this->buffer0[0];
	this->buffer0[0] = (this->lastIn + inputout) * 0.5;
	this->buffer0[5] = this->buffer0[4];
	this->buffer0[4] = this->buffer0[3];
	this->buffer0[3] = 
		this->b0 * this->buffer0[0] 
		+ this->b1 * this->buffer0[1] 
		+ this->b2 * this->buffer0[2] 
		+ this->a1 * this->buffer0[4]	
		+ this->a2 * this->buffer0[5] + DENORM_CONST;
	
	this->buffer0[2] = this->buffer0[1];
	this->buffer0[1] = this->buffer0[0];
	this->lastIn = this->buffer0[0] = inputout;
	this->buffer0[5] = this->buffer0[4];
	this->buffer0[4] = this->buffer0[3];
	this->buffer0[3] = 
		this->b0 * this->buffer0[0] 
		+ this->b1 * this->buffer0[1] 
		+ this->b2 * this->buffer0[2] 
		+ this->a1 * this->buffer0[4]	
		+ this->a2 * this->buffer0[5] + DENORM_CONST;

	this->buffer1[2] = this->buffer1[1];
	this->buffer1[1] = this->buffer1[0];
	this->buffer1[0] = (this->lastIn2 + this->buffer0[3]) * 0.5;
	this->buffer1[5] = this->buffer1[4];
	this->buffer1[4] = this->buffer1[3];
	this->buffer1[3] = 
		this->b0 * this->buffer1[0] 
		+ this->b1 * this->buffer1[1] 
		+ this->b2 * this->buffer1[2] 
		+ this->a1 * this->buffer1[4]	
		+ this->a2 * this->buffer1[5] + DENORM_CONST;
	
	this->buffer1[2] = this->buffer1[1];
	this->buffer1[1] = this->buffer1[0];
	this->lastIn2 = this->buffer1[0] = this->buffer0[3];
	this->buffer1[5] = this->buffer1[4];
	this->buffer1[4] = this->buffer1[3];
	this->buffer1[3] = 
		this->b0 * this->buffer1[0] 
		+ this->b1 * this->buffer1[1] 
		+ this->b2 * this->buffer1[2] 
		+ this->a1 * this->buffer1[4]	
		+ this->a2 * this->buffer1[5] + DENORM_CONST;

	if(this->buffer1[3] > 15.0)
		this->Reset();

	return ntanh((float)this->buffer1[3] * this->gain);	
}

float Filter::Process12(float _inputout)
{
	double inputout = (double)_inputout;
	this->buffer2[2] = this->buffer2[1];
	this->buffer2[1] = this->buffer2[0];
	this->buffer2[0] = inputout;
	this->buffer2[5] = this->buffer2[4];
	this->buffer2[4] = this->buffer2[3];
	this->buffer2[3] = 
		this->b0b * this->buffer2[0] 
		+ this->b1b * this->buffer2[1] 
		+ this->b2b * this->buffer2[2]
		+ this->a1b * this->buffer2[4] 
		+ this->a2b * this->buffer2[5] + DENORM_CONST;

	inputout = DRY * inputout + WET * this->buffer2[3];
	
	this->buffer0[2] = this->buffer0[1];
	this->buffer0[1] = this->buffer0[0];
	this->buffer0[0] = (this->lastIn + inputout) * 0.5;
	this->buffer0[5] = this->buffer0[4];
	this->buffer0[4] = this->buffer0[3];
	this->buffer0[3] = 
		this->b0 * this->buffer0[0] 
		+ this->b1 * this->buffer0[1] 
		+ this->b2 * this->buffer0[2] 
		+ this->a1 * this->buffer0[4]	
		+ this->a2 * this->buffer0[5] + DENORM_CONST;
	
	this->buffer0[2] = this->buffer0[1];
	this->buffer0[1] = this->buffer0[0];
	this->lastIn = this->buffer0[0] = inputout;
	this->buffer0[5] = this->buffer0[4];
	this->buffer0[4] = this->buffer0[3];
	this->buffer0[3] = 
		this->b0 * this->buffer0[0] 
		+ this->b1 * this->buffer0[1] 
		+ this->b2 * this->buffer0[2] 
		+ this->a1 * this->buffer0[4]	
		+ this->a2 * this->buffer0[5] + DENORM_CONST;

	if(this->buffer0[3] > 15.0)
		this->Reset();

	return ntanh((float)this->buffer0[3] * this->gain);	
}
#endif
