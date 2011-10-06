#include "Cetone033.h"
#include "SynthOscillator.h"

CSynthOscillator::CSynthOscillator(void)
{
	this->SetPitch(7200);

	this->SetPw(0.5f);
	this->SetWave(WAVE_SAW);

	this->IndexInt = 0;
	this->IndexFrac = 0;
}

CSynthOscillator::~CSynthOscillator(void)
{
}

void CSynthOscillator::Set(float pw, int wave)
{
	if(pw < 0.01f)
		pw = 0.01f;
	else if(pw > 0.99f)
		pw = 0.99f;

	this->PwOffset =  truncate(pw * WAVETABLE_LENGTHf);
	float f = (float)this->PwOffset / WAVETABLE_LENGTHf;
	this->PwWaveOffset  = 1.f - (2.f * f);
	this->TriWaveScale = 1.f / (8.f * (f - f * f));

	if(wave < 0)
		wave = 0;
	else if(wave > WAVE_MAX)
		wave = WAVE_MAX;

	this->Waveform = wave;
}

void CSynthOscillator::SetPitch(int pitch)
{
	if(pitch < 0)
		pitch = 0;
	else if(pitch > PITCH_MAX1)
		pitch = PITCH_MAX1;

	this->StepInt = CCetone033::FreqStepInt[pitch];
	this->StepFrac = CCetone033::FreqStepFrac[pitch];

	int t = CCetone033::LookupTable[CCetone033::FreqTableInt[pitch]] * WAVETABLE_LENGTH;

	this->SawPtr = &(CCetone033::SawTable[t]);
	this->ParabolaPtr = &(CCetone033::ParabolaTable[t]);
}

void CSynthOscillator::SetPw(float pw)
{
	if(pw < 0.01f)
		pw = 0.01f;
	else if(pw > 0.99f)
		pw = 0.99f;

	this->PwOffset =  truncate(pw * WAVETABLE_LENGTHf);
	float f = (float)this->PwOffset / WAVETABLE_LENGTHf;
	this->PwWaveOffset  = 1.f - (2.f * f);
	this->TriWaveScale = 1.f / (8.f * (f - f * f));
}

void CSynthOscillator::SetWave(int wave)
{
	if(wave < 0)
		wave = 0;
	else if(wave > WAVE_MAX)
		wave = WAVE_MAX;

	this->Waveform = wave;
}

float CSynthOscillator::Run()
{
	int sample0, sample1;
    float interpolate, out0, out1;

    float output = 0.f;

    sample0 = this->IndexInt;
    sample1 = (sample0 + 1) & WAVETABLE_MASK;
	interpolate = CCetone033::Int2FloatTab[this->IndexFrac];

	switch (this->Waveform)
	{
	case WAVE_SAW:
        out0 = this->SawPtr[sample0];
        out1 = this->SawPtr[sample1];
        output = (1.f - interpolate) * out0 + interpolate * out1;
		break;
	case WAVE_PULSE:
        out0 = this->SawPtr[sample0];
        out1 = this->SawPtr[sample1];

        sample0 += this->PwOffset;
        sample1 += this->PwOffset;
        sample0 &= WAVETABLE_MASK;
        sample1 &= WAVETABLE_MASK;

        out0 -= this->SawPtr[sample0];
        out1 -= this->SawPtr[sample1];

        output = (1.f - interpolate) * out0 + interpolate * out1;
		output += this->PwWaveOffset;
		break;
	case WAVE_TRI:
        out0 = this->ParabolaPtr[sample0];
        out1 = this->ParabolaPtr[sample1];

        sample0 += this->PwOffset;
        sample1 += this->PwOffset;
        sample0 &= WAVETABLE_MASK;
        sample1 &= WAVETABLE_MASK;

        out0 -= this->ParabolaPtr[sample0];
        out1 -= this->ParabolaPtr[sample1];

        output = (1.f - interpolate) * out0 + interpolate * out1;
        output *= this->TriWaveScale;
		break;
	}

	this->IndexFrac += this->StepFrac;

	if (this->IndexFrac > 65535)
	{
		this->IndexFrac &= 65535;
		this->IndexInt++;
	}

	this->IndexInt += this->StepInt;
	this->IndexInt &= WAVETABLE_MASK;

	return output;
}