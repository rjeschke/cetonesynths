#include "CetoneSynth.h"
#include "FilterDirty.h"

CFilterDirty::CFilterDirty(void)
{
	this->Reset();

	this->Set(CCetoneSynth::SampleRate2, 0.f);
	this->SetMode(FMODE_LOW);	
}

CFilterDirty::~CFilterDirty(void)
{
}

void CFilterDirty::Reset()
{
	this->buf0 = this->buf1 = 0.f;
}

void CFilterDirty::SetMode(int mode)
{
	if(mode < 0)
		mode = 0;
	else if(mode > FMODE_NOTCH)
		mode = FMODE_NOTCH;

	this->Mode = mode;
}

int CFilterDirty::GetMode()
{
	return this->Mode;
}

void CFilterDirty::Set(float cutoff, float q)
{
	if(cutoff < 0.f)
		cutoff = 0.f;
	else if(cutoff > 20000.f)
		cutoff = 20000.f;
	
	if(q < 0.f)
		q = 0.f;
	else if(q > 1.f)
		q = 1.f;

#if ANALOGUE_BEHAVIOR > 0
	q *= DIRTY_Q_MAX;
#endif

	this->f = cutoff * CCetoneSynth::SampleRate2_1;
    if (this->f == 1.f)
        this->f = 0.99999f;

    this->fb = q + q / (1.f - this->f);

	this->CutOff = cutoff;
	this->Q = q;
}

float CFilterDirty::Run(float val)
{
    float hp = val - this->buf0;
    float bp = this->buf0 - this->buf1;

    this->buf0 += this->f * (hp + this->fb * bp);
    this->buf1 += this->f * (this->buf0 - this->buf1);

    switch (this->Mode)
    {
	default:
		return 0.f;
    case FMODE_LOW:
        return this->buf1;
    case FMODE_HIGH:
        return hp;
    case FMODE_BAND:
        return bp;
    case FMODE_NOTCH:
        return this->buf1 + hp;
    }
}
