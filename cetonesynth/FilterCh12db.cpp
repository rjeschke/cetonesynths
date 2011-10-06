#include "CetoneSynth.h"
#include "FilterCh12db.h"
#include <math.h>

CFilterCh12db::CFilterCh12db(void)
{
	this->Reset();

	this->Set(CCetoneSynth::SampleRate2, 0.f);
	this->SetMode(FMODE_LOW);
}

CFilterCh12db::~CFilterCh12db(void)
{
}

void CFilterCh12db::Reset()
{
	this->Low = 0.f;
	this->High = 0.f;
	this->Band = 0.f;
	this->Notch = 0.f;
}

void CFilterCh12db::SetMode(int mode)
{
	this->Mode = mode;
}

int CFilterCh12db::GetMode()
{
	return this->Mode;
}

void CFilterCh12db::Set(float cutoff, float q)
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
	q *= CH12DB_Q_MAX;
#endif

	this->f = sinf(CCetoneSynth::Pi * cutoff * CCetoneSynth::SampleRate_1);

	this->CutOff = cutoff;
	this->Q = 1.f - q;
}

float CFilterCh12db::Run(float val)
{
    this->Low += this->f * this->Band;
    this->High = this->Q * val - this->Low - this->Q * this->Band;
    this->Band = this->f * this->High + this->Band;
    this->Notch = this->High + this->Low;

    switch (this->Mode)
    {
	default:
		return 0.f;
    case FMODE_BAND:
        return this->Band;
    case FMODE_LOW:
        return this->Low;
    case FMODE_HIGH:
        return this->High;
    case FMODE_NOTCH:
        return this->Notch;
    }
}