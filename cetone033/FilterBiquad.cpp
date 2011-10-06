#include "cetone033.h"
#include "FilterBiquad.h"
#include <math.h>

// Interpolates between ps[1] and ps[2]
static float spline(float t, float* ps)
{
    return (
		(((-t + 3.0f) * t - 3.0f) * t + 1.0f) * ps[0]
		+ (((3.0f * t - 6.0f) * t) * t + 4.0f) * ps[1]
		+ (((-3.0f * t + 3.0f) * t + 3.0f) * t + 1.0f) * ps[2]
		+ (t * t * t) * ps[3]
		) / 6.0f;
}

CFilterBiquad::CFilterBiquad()
{
	int i;
	float points[5] = {-1.2f, 0.0f, 1.2f, 1.5f, 1.2f};
	this->min_fc = 0.0025f;

	this->Reset();

	this->SetSampleRate(CCetone033::SampleRate);
	this->Set(1.f, 0.f);

	for(i = 0; i < 8192; i++)
	{
		this->waveshaper[i] = spline((float)i / 8192.0f, &(points[0]));
		this->waveshaper[i + 8192] = spline((float)i / 8192.0f, &(points[1]));
	}
}

CFilterBiquad::~CFilterBiquad()
{
}

void CFilterBiquad::Reset()
{
	for(int i = 0; i < 6; i++)
	{
		this->buffer0[i] = 0.f;
		this->buffer1[i] = 0.f;
		this->buffer2[i] = 0.f;
	}
}

void CFilterBiquad::Set(float cutoff, float q)
{
	// Just to give some more control in the lower regions
	
	cutoff *= cutoff;
	//q *= q;

	if(cutoff < this->min_fc)
		cutoff = this->min_fc;
	else if(cutoff > 1.0f)
		cutoff = 1.0f;
	cutoff *= 0.5f;

	if(q < 0.f)
		q = 0.f;
	else if(q > 1.f)
		q = 1.f;

	q = 1.f + q * 15.f;

	float omega = this->t1 * cutoff;
	float cs = cosf(omega);
	float alpha = sinf(omega) / q;

	float a0 = 1.0f + alpha;
	this->b0 = (1.0f - cs) / (2.f * a0);
	this->b1 = (1.0f - cs) / a0;
	this->b2 = this->b0;
	this->a1 = (-2.f * cs) / a0;
	this->a2 = (1.0f - alpha) / a0;
}       

void CFilterBiquad::SetSampleRate(float fs)
{
	float pi = 4.f * atanf(1.f);
	this->t0 = pi * fs;	
	this->t1 = pi;// * 0.5f;	

	float omega = this->t1 * 880.0f / CCetone033::SampleRate2;
	float cs = cosf(omega);
	float sn = sinf(omega) / 6.0f;

	float a0 = 1.0f + sn;
	this->b0b = sn / a0;
	this->b1b = 0.0f;
	this->b2b = -this->b0b;
	this->a1b = (-2.f * cs) / a0;
	this->a2b = (1.0f - sn) / a0;
}

// Oversampled, 24dB
/*
	Well, my first aproach using standard biquads just sounded ... sounded ...
	hmm ... well, no-good. This one is much more CPU consuming (5 biquad sections)
	but sounds really nice.
	A Neotec-filter ... my own ... no other filter sounds like this^^
*/
float CFilterBiquad::Run(float inputout)
{
	float* buf;
	float frac;
	int pos;

	// Short explan~s coming up

	// A band-pass filter with Q == 6 and FS == 880 Hz
	buf = this->buffer2;
	buf[2] = buf[1];	buf[1] = buf[0];	buf[0] = inputout;
	buf[5] = buf[4];	buf[4] = buf[3];
	buf[3] = this->b0b * buf[0]	+ this->b1b * buf[1] + this->b2b * buf[2]
		- this->a1b * buf[4] - this->a2b * buf[5];
	UNDENORM(buf[3]);

	// Mix "dry" and "wet"
	inputout = 0.7f * inputout + 0.3f * buf[3];
	
	// This is the first 12dB biquad filter section
	buf = this->buffer0;
	buf[2] = buf[1];	buf[1] = buf[0];	buf[0] = inputout;
	buf[5] = buf[4];	buf[4] = buf[3];	
	buf[3] = this->b0 * buf[0] + this->b1 * buf[1] + this->b2 * buf[2]
		- this->a1 * buf[4]	- this->a2 * buf[5];
	// Jap, UNDENORM ... needed^^
	UNDENORM(buf[3]);
	
	// Oversample, pad with NULL
	buf[2] = buf[1];	buf[1] = buf[0];	buf[0] = 0.0f;
	buf[5] = buf[4];	buf[4] = buf[3];
	buf[3] = this->b1 * buf[1] + this->b2 * buf[2] 
		- this->a1 * buf[4]	- this->a2 * buf[5];
	UNDENORM(buf[3]);

	inputout = buf[3];

	// This is the second 12dB biquad filter section
	buf = this->buffer1;
	buf[2] = buf[1];	buf[1] = buf[0];	buf[0] = inputout;
	buf[5] = buf[4];	buf[4] = buf[3];
	buf[3] = this->b0 * buf[0] + this->b1 * buf[1] + this->b2 * buf[2]
		- this->a1 * buf[4]	- this->a2 * buf[5];
	UNDENORM(buf[3]);

	// Oversample, pad with NULL
	buf[2] = buf[1];	buf[1] = buf[0];	buf[0] = 0.0f;
	buf[5] = buf[4];	buf[4] = buf[3];
	buf[3] = this->b1 * buf[1] + this->b2 * buf[2]
		- this->a1 * buf[4]	- this->a2 * buf[5];
	UNDENORM(buf[3]);

	inputout = buf[3];

	// Waveshaping (spline table)
	if(inputout < 0.0f)
	{
		inputout *= -8192.0f;
		if(inputout > 16382.0f)
			inputout = -this->waveshaper[16383];
		else
		{
			pos = truncate(inputout);	frac = inputout - (float)pos;
			inputout = -(this->waveshaper[pos] 
				+ frac * (this->waveshaper[pos + 1] - this->waveshaper[pos]));
		}
	}
	else
	{
		inputout *= 8192.0f;
		if(inputout > 16382.0f)
			inputout = this->waveshaper[16383];
		else
		{
			pos = truncate(inputout);	frac = inputout - (float)pos;
			inputout = this->waveshaper[pos] 
				+ frac * (this->waveshaper[pos + 1] - this->waveshaper[pos]);
		}
	}

	// done ... finally
	return inputout;	
}
