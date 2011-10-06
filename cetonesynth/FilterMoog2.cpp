#include "CetoneSynth.h"
#include "FilterMoog2.h"
#include <math.h>

CFilterMoog2::CFilterMoog2(void)
{
	this->Reset();

	this->Set(CCetoneSynth::SampleRate2, 0.f);
	this->SetMode(FMODE_LOW);	
}

CFilterMoog2::~CFilterMoog2(void)
{
}

void CFilterMoog2::Reset()
{
	this->b0 = 0.f;
	this->b1 = 0.f;
	this->b2 = 0.f;
	this->b3 = 0.f;
	this->b4 = 0.f;
}

void CFilterMoog2::Set(float cutoff, float q)
{
	if(cutoff < 0.f)
		cutoff = 0.f;
	else if(cutoff > CCetoneSynth::SampleRate2)
		cutoff = CCetoneSynth::SampleRate2;
	
	if(q < 0.f)
		q = 0.f;
	else if(q > 1.f)
		q = 1.f;

#if ANALOGUE_BEHAVIOR > 0
	q *= MOOG2_Q_MAX;
#endif
    
	float frequency = cutoff * CCetoneSynth::SampleRate2_1;

    this->q2 = 1.f - frequency;
    this->p = frequency + 0.8f * frequency * this->q2;
    this->f = this->p + this->p - 1.f;
    this->q2 = q * (1.f + 0.5f * this->q2 * (1.f - this->q2 + 5.6f * this->q2 * this->q2));

	this->CutOff = cutoff;
	this->Q = q;
}

void CFilterMoog2::SetMode(int mode)
{
	this->Mode = mode;
}

int CFilterMoog2::GetMode()
{
	return this->Mode;
}

float CFilterMoog2::Run(float val)
{
    float t1, t2;

    val -= this->q2 * this->b4; //feedback
    t1 = this->b1;
    this->b1 = (val + this->b0) * this->p - this->b1 * this->f;
    t2 = this->b2;
    this->b2 = (this->b1 + t1) * this->p - this->b2 * this->f;
    t1 = this->b3;
    this->b3 = (this->b2 + t2) * this->p - this->b3 * this->f;
    this->b4 = (this->b3 + t1) * this->p - this->b4 * this->f;
    this->b4 = this->b4 - this->b4 * this->b4 * this->b4 * 0.1666667f; //clipping
    this->b0 = val;

    switch (this->Mode)
    {
	default:
		return 0.f;
    case FMODE_LOW:
        return this->b4;
    case FMODE_HIGH:
        return val - this->b4;
    case FMODE_BAND:
        return 3.f * (this->b3 - this->b4);
    case FMODE_NOTCH:
        return this->b4 + (val - this->b4);
    }
}
