#include "defines.h"

#include "CetoneSynth.h"
#include "SynthEnvelope.h"

CSynthEnvelope::CSynthEnvelope(void)
{
	this->Reset();

	this->SetPreAttack(0.015f);
	this->Set(0.01f, 0.f, 0.25f, 0.5f, 0.4f);
}

CSynthEnvelope::~CSynthEnvelope(void)
{
	this->LastPhase = -1;
	this->Phase = -1;
	this->VolNow = 0.f;
}

void CSynthEnvelope::Reset()
{
	this->LastPhase = -1;
	this->Phase = -1;
	this->VolNow = 0.f;
}

void CSynthEnvelope::Gate(bool gate)
{
	if (gate)
	{
		this->LastPhase = -1;

#if ANALOGUE_ENVELOPES != 0
#pragma message("SynthEnvelope: Analogue mode")
		this->Phase = 0;
#else
#pragma message("SynthEnvelope: Digital mode")
		if (this->Phase == -1)
			this->Phase = 0;
		else
			this->Phase = 5;
#endif
	} 
	else
	{
		this->LastPhase = -1;
		this->Phase = 4;
	}
}

float CSynthEnvelope::TimeValue(float val)
{
	if(val <= 8.f)
		return val;
	else
		return val * 26.f - 200.f;
}

void CSynthEnvelope::Set(float a, float h, float d, float s, float r)
{
	if(a < 0.005f)
		a = 0.005f;
	
	if(d < 0.005f)
		d = 0.005f;
	
	if(r < 0.005f)
		r = 0.005f;

	this->Attack = this->TimeValue(a);
	this->sAttack = truncate(a * CCetoneSynth::SampleRate + 0.5f);
	this->sAttackf = (float)this->sAttack;

	this->Hold = this->TimeValue(h);
	this->sHold = truncate(h * CCetoneSynth::SampleRate + 0.5f);
	this->sHoldf = (float)this->sHold;

	this->Decay = this->TimeValue(d);
	this->sDecay = truncate(d * CCetoneSynth::SampleRate + 0.5f);
	this->sDecayf = (float)this->sDecay;

	this->Sustain = s;

	this->Release = this->TimeValue(r);
	this->sRelease = truncate(r * CCetoneSynth::SampleRate + 0.5f);
	this->sReleasef = (float)this->sRelease;
}

void CSynthEnvelope::SetPreAttack(float val)
{
	this->PreAttack = val;
	this->sPreAttack = truncate(val * CCetoneSynth::SampleRate + 0.5f);
	this->sPreAttackf = (float)this->sPreAttack;
}

float CSynthEnvelope::GetAttack()
{
	return this->Attack;
}

float CSynthEnvelope::GetHold()
{
	return this->Hold;
}

float CSynthEnvelope::GetDecay()
{
	return this->Decay;
}

float CSynthEnvelope::GetSustain()
{
	return this->Sustain;
}

float CSynthEnvelope::GetRelease()
{
	return this->Release;
}

float CSynthEnvelope::Run()
{
    float vol = this->VolNow;

    switch (this->Phase)
    {
    case 0:
        if (this->LastPhase != 0)
        {
            this->LastPhase = 0;

            if (this->sAttack != 0)
                this->VolStep = 1.f / this->sAttackf;
            else
            {
                vol = 1.f;
                this->Phase = 1;
                break;
            }
        }

        vol += this->VolStep;

		if(vol >= 1.f)
		{
			vol = 1.f;
            this->Phase = 1;
		}
        break;
    case 1:
        if (this->LastPhase != 1)
        {
            this->LastPhase = 1;
			this->Samples = this->sHold;
			this->Counter = 0;
        }

        vol = 1.f;
        this->Counter++;

        if (this->Counter >= this->Samples)
            this->Phase = 2;
        break;
    case 2:
        if (this->LastPhase != 2)
        {
            this->LastPhase = 2;

            if (this->sDecay != 0)
#if ANALOGUE_ENVELOPES != 0
                this->VolStep = 1.f / this->sDecayf;
#else
                this->VolStep = (1.f - this->Sustain) / this->sDecayf;
#endif
            else
            {
                vol = this->Sustain;
                this->Phase = 3;
                break;
            }
        }

        vol -= this->VolStep;

		if (vol <= this->Sustain)
		{
			vol = this->Sustain;
            this->Phase = 3;
		}
        break;
    case 3:
        vol = (float)this->Sustain;
        break;
    case 4:
        if (this->LastPhase != 4)
        {
            this->LastPhase = 4;

            if (this->sRelease != 0)
#if ANALOGUE_ENVELOPES != 0
                this->VolStep = 1.f / this->sReleasef;
#else
                this->VolStep = this->VolNow / this->sReleasef;
#endif
            else
            {
                vol = 0.f;
                this->Phase = -1;
                break;
            }
        }

        vol -= this->VolStep;

		if(vol <= 0.f)
		{
			vol = 0.f;
            this->Phase = -1;
		}
        break;
#if ANALOGUE_ENVELOPES == 0
	case 5:
        if (this->LastPhase != 5)
        {
            this->LastPhase = 5;

            if (this->sPreAttack != 0)
                this->VolStep = this->VolNow / this->sPreAttackf;
            else
            {
                vol = 0.f;
                this->Phase = 0;
                break;
            }
        }

        vol -= this->VolStep;

		if(vol <= 0.f)
		{
			vol = 0.f;
            this->Phase = 0;
		}
		break;
#endif
    default:
        vol = 0.f;
        break;
    }

    this->VolNow = vol;

    return vol;
}
