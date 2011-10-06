#include "CetoneSynth.h"
#include "FilterMoog.h"
#include <math.h>

CFilterMoog::CFilterMoog(void)
{
	this->Reset();

	this->Set(CCetoneSynth::SampleRate2, 0.f);
}

CFilterMoog::~CFilterMoog(void)
{
}

void CFilterMoog::Reset()
{
	this->y1 = 0.f;
	this->y2 = 0.f;
	this->y3 = 0.f;
	this->y4 = 0.f;
	this->oldx = 0.f;
	this->oldy1 = 0.f;
	this->oldy3 = 0.f;
	this->oldy2 = 0.f;
}

void CFilterMoog::Set(float cutoff, float q)
{
	if (cutoff < 0.f)
		cutoff = 0.f;
	else if (cutoff > CCetoneSynth::SampleRate2)
		cutoff = CCetoneSynth::SampleRate2;
	
	if (q < 0.f)
		q = 0.f;
	else if (q > 1.f)
		q = 1.f;

#if ANALOGUE_BEHAVIOR > 0
	q *= MOOG_Q_MAX;
#endif
    
	float f = cutoff * CCetoneSynth::SampleRate2_1;
    this->k = 3.6f * f - 1.6f * f * f - 1.f;
    this->p = (this->k + 1.f) * 0.5f;
    float scale = expf((1.f - this->p) * 1.386249f);
    this->r = q * scale;

	this->CutOff = cutoff;
	this->Q = q;

}

void CFilterMoog::SetMode(int mode)
{
	// Nothing here
}

int CFilterMoog::GetMode()
{
	return FMODE_LOW;
}

float CFilterMoog::Run(float val)
{
    float x = val - this->r * this->y4;

    this->y1 = x * this->p + this->oldx * this->p - this->k * this->y1;
    this->y2 = this->y1 * this->p + this->oldy1 * this->p - this->k * this->y2;
    this->y3 = this->y2 * this->p + this->oldy2 * this->p - this->k * this->y3;
    this->y4 = this->y3 * this->p + this->oldy3 * this->p - this->k * this->y4;

    this->y4 = this->y4 - this->y4 * this->y4 * this->y4 * 0.1666667f;

    this->oldx = x;
    this->oldy1 = this->y1;
    this->oldy2 = this->y2;
    this->oldy3 = this->y3;

    return this->y4;
}