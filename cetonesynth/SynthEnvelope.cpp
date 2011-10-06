#include "defines.h"

#include "CetoneSynth.h"
#include "SynthEnvelope.h"

CSynthEnvelope::CSynthEnvelope(void)
{
	this->Reset();

	this->SetPreAttack(0.015f);
	this->SetAttack(0.01f);
	this->SetHold(0.0f);
	this->SetDecay(0.25f);
	this->SetSustain(0.5f);
	this->SetRelease(0.4f);
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

void CSynthEnvelope::Set(float a, float h, float d, float s, float r)
{
	if(a == 0.f)
		a = 0.005f;

	this->Attack = a;
	this->sAttack = truncate(a * CCetoneSynth::SampleRate + 0.5f);
	this->sAttackf = (float)this->sAttack;

	this->Hold = h;
	this->sHold = truncate(h * CCetoneSynth::SampleRate + 0.5f);
	this->sHoldf = (float)this->sHold;

	this->Decay = d;
	this->sDecay = truncate(d * CCetoneSynth::SampleRate + 0.5f);
	this->sDecayf = (float)this->sDecay;

	this->Sustain = s;

	this->Release = r;
	this->sRelease = truncate(r * CCetoneSynth::SampleRate + 0.5f);
	this->sReleasef = (float)this->sRelease;
}

void CSynthEnvelope::SetPreAttack(float val)
{
	this->PreAttack = val;
	this->sPreAttack = truncate(val * CCetoneSynth::SampleRate + 0.5f);
	this->sPreAttackf = (float)this->sPreAttack;
}

void CSynthEnvelope::SetAttack(float val)
{
	if(val == 0.f)
		val = 0.001f;

	this->Attack = val;
	this->sAttack = truncate(val * CCetoneSynth::SampleRate + 0.5f);
	this->sAttackf = (float)this->sAttack;
}

float CSynthEnvelope::GetAttack()
{
	return this->Attack;
}

void CSynthEnvelope::SetHold(float val)
{
	this->Hold = val;
	this->sHold = truncate(val * CCetoneSynth::SampleRate + 0.5f);
	this->sHoldf = (float)this->sHold;
}

float CSynthEnvelope::GetHold()
{
	return this->Hold;
}

void CSynthEnvelope::SetDecay(float val)
{
	this->Decay = val;
	this->sDecay = truncate(val * CCetoneSynth::SampleRate + 0.5f);
	this->sDecayf = (float)this->sDecay;
}

float CSynthEnvelope::GetDecay()
{
	return this->Decay;
}

void CSynthEnvelope::SetSustain(float val)
{
	this->Sustain = val;
}

float CSynthEnvelope::GetSustain()
{
	return this->Sustain;
}

void CSynthEnvelope::SetRelease(float val)
{
	this->Release = val;
	this->sRelease = truncate(val * CCetoneSynth::SampleRate + 0.5f);
	this->sReleasef = (float)this->sRelease;
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
