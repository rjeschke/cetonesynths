#include "cetonesynth.h"
#include "FilterBiquad.h"
#include <math.h>

CFilterBiquad::CFilterBiquad()
{
	this->min_fc = 0.0025f;

	this->Reset();

	this->SetSampleRate(CCetoneSynth::SampleRate);
	this->Set(1.f, 0.f);
}

CFilterBiquad::~CFilterBiquad()
{
}

void CFilterBiquad::Reset()
{
	for(int i = 0; i < 6; i++)
		this->buffer[i] = 0.f;
}

void CFilterBiquad::Set(float cutoff, float q)
{
	if(cutoff < this->min_fc)
		cutoff = this->min_fc;
	else if(cutoff > 0.999f)
		cutoff = 0.999f;
		
	if(q < 0.f)
		q = 0.f;
	else if(q > 1.f)
		q = 1.f;

	q = 1.f + q * 199.f;

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
}

float CFilterBiquad::Run(float inputout)
{
	float* buf = this->buffer;

	buf[2] = buf[1];
	buf[1] = buf[0];
	buf[5] = buf[4];
	buf[4] = buf[3];

	buf[0] = inputout;

	buf[3] = this->b0 * buf[0]
		+ this->b1 * buf[1]
		+ this->b2 * buf[2]
		- this->a1 * buf[4]
		- this->a2 * buf[5];

	inputout = buf[3];

	return inputout;	
}
