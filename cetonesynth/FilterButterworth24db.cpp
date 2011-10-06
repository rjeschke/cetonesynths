#include <math.h>
#include "cetonesynth.h"

#define BUDDA_Q_SCALE 40.f

#include "FilterButterworth24db.h"

CFilterButterworth24db::CFilterButterworth24db(void)
{
	this->Reset();

	this->SetSampleRate(CCetoneSynth::SampleRate);
	this->Set(CCetoneSynth::SampleRate2, 0.0);
}

CFilterButterworth24db::~CFilterButterworth24db(void)
{
}

void CFilterButterworth24db::Reset()
{
	this->history1 = 0.f;
	this->history2 = 0.f;
	this->history3 = 0.f;
	this->history4 = 0.f;
}

void CFilterButterworth24db::SetSampleRate(float fs)
{
	/* No damn optimizations here, 'cause this func should
	   get called only once, so ... who cares!?
    */

	float pi = 4.f * atanf(1.f);

	this->t0 = 4.f * fs * fs;
	this->t1 = 8.f * fs * fs;
	this->t2 = 2.f * fs;
	this->t3 = pi / fs;

	this->min_cutoff = fs * 0.0005f;
	this->max_cutoff = fs * 0.45f;
}

void CFilterButterworth24db::Set(float cutoff, float q)
{
	if (cutoff < this->min_cutoff)
		cutoff = this->min_cutoff;
	else if(cutoff > this->max_cutoff)
		cutoff = this->max_cutoff;

	if(q < 0.f)
		q = 0.f;
	else if(q > 1.f)
		q = 1.f;

	float wp = this->t2 * tanf(this->t3 * cutoff);
	float bd, bd_tmp, b1, b2;

	q *= BUDDA_Q_SCALE;
	q += 1.f;

	b1 = (0.765367f / q) / wp;
	b2 = 1.f / (wp * wp);

	bd_tmp = this->t0 * b2 + 1.f;

	bd = 1.f / (bd_tmp + this->t2 * b1);

	this->gain = bd;

	this->coef2 = (2.f - this->t1 * b2);

	this->coef0 = this->coef2 * bd;
	this->coef1 = (bd_tmp - this->t2 * b1) * bd;

	b1 = (1.847759f / q) / wp;

	bd = 1.f / (bd_tmp + this->t2 * b1);

	this->gain *= bd;
	this->coef2 *= bd;
	this->coef3 = (bd_tmp - this->t2 * b1) * bd;
}

float CFilterButterworth24db::Run(float input)
{
	float output = input * this->gain;
	float new_hist;

	output -= this->history1 * this->coef0;
	new_hist = output - this->history2 * this->coef1;

	output = new_hist + this->history1 * 2.f;
	output += this->history2;

	this->history2 = this->history1;
	this->history1 = new_hist;

	output -= this->history3 * this->coef2;
	new_hist = output - this->history4 * this->coef3;

	output = new_hist + this->history3 * 2.f;
	output += this->history4;

	this->history4 = this->history3;
	this->history3 = new_hist;

	return output;
}
/*

float wp = t2 * tanf(t3 * fc);
float bd, bd_tmp;

// Section 1

b1 = (0.765367f / Q) / wp;
b2 = 1.f / (wp * wp);

bd_tmp = t0 * b2 + 1.f;

bd = 1.f / (bd_tmp + t2 * b1);
gain *= bd;

coef[2] = (2.f - t1 * b2);

coef[0] = coef[2] * bd;
coef[1] = (bd_tmp - t2 * b1) * bd;

// Section 2

b1 = (1.847759f / Q) / wp;

bd = 1.f / (bd_tmp + t2 * b1);

gain *= bd;
coef[2] *= bd;
coef[3] = (bd_tmp - t2 * b1) * bd;

// Compute filter


float output = input * gain;
float new_hist;

output -= history1 * coef[0]
new_hist = output - history2 * coef[1];

output = new_hist + history1 * 2.f;
output += history2;

history2 = history1;
histroy1 = new_hist;

output -= history3 * coef[2]
new_hist = output - history4 * coef[3];

output = new_hist + history3 * 2.f;
output += history4;

history4 = history3;
histroy3 = new_hist;

return output;
*/