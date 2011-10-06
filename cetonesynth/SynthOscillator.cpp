#include "CetoneSynth.h"
#include "SynthOscillator.h"

CSynthOscillator::CSynthOscillator(void)
{
#if NOTE_RANGE == 1
	this->SetPitch(7200);
#else
	this->SetPitch(8400);
#endif

	this->SetPw(32768);
	this->SetWave(WAVE_SAW);

	this->Reset();
	
	this->SyncOsc = NULL;
	this->DoSync = false;
}

CSynthOscillator::~CSynthOscillator(void)
{
}

void CSynthOscillator::Reset()
{
	this->Value = 0;
	this->ShiftRegister = 0x7ffff8;
	this->IndexInt = 0;
	this->IndexFrac = 0;
}

void CSynthOscillator::Set(int pw, int wave, bool sync)
{
	if(pw < 0)
		pw = 0;
	else if(pw > 65535)
		pw = 65535;

	this->PulseWidth = pw;

	pw >>= 4;

	this->PwFloat = CCetoneSynth::Pw2Float[pw];
	this->PwOffset = CCetoneSynth::Pw2Offset[pw];
	this->PwWaveOffset = CCetoneSynth::Pw2WaveOffset[pw];

	if(wave < 0)
		wave = 0;
	else if(wave > OWAVE_MAX)
		wave = OWAVE_MAX;

	this->Waveform = wave;

	this->DoSync = sync;
}

void CSynthOscillator::SetPitch(int pitch)
{
	if(pitch < 0)
		pitch = 0;
	else if(pitch > PITCH_MAX1)
		pitch = PITCH_MAX1;

	this->Pitch = pitch;

	this->StepInt = CCetoneSynth::FreqStepInt[pitch];
	this->StepFrac = CCetoneSynth::FreqStepFrac[pitch];

	int t = CCetoneSynth::LookupTable[CCetoneSynth::FreqTableInt[pitch]] * WAVETABLE_LENGTH;

	this->SawPtr = &(CCetoneSynth::SawTable[t]);
	this->ParabolaPtr = &(CCetoneSynth::ParabolaTable[t]);
}

int CSynthOscillator::GetPitch()
{
	return this->Pitch;
}

void CSynthOscillator::SetPw(int pw)
{
	if(pw < 0)
		pw = 0;
	else if(pw > 65535)
		pw = 65535;

	this->PulseWidth = pw;

	pw >>= 4;

	this->PwFloat = CCetoneSynth::Pw2Float[pw];
	this->PwOffset = CCetoneSynth::Pw2Offset[pw];
	this->PwWaveOffset = CCetoneSynth::Pw2WaveOffset[pw];
}

int CSynthOscillator::GetPw()
{
	return this->PulseWidth;
}

void CSynthOscillator::SetWave(int wave)
{
	if(wave < 0)
		wave = 0;
	else if(wave > OWAVE_MAX)
		wave = OWAVE_MAX;

	this->Waveform = wave;
}

int CSynthOscillator::GetWave()
{
	return this->Waveform;
}

void CSynthOscillator::Sync()
{
}

void CSynthOscillator::SetSync(bool sync)
{
	this->DoSync = sync;
}

void CSynthOscillator::SetSyncDest(CSynthOscillator *osc)
{
	this->SyncOsc = osc;
}

float CSynthOscillator::Run()
{
	int sr = this->ShiftRegister;

	int sample0, sample1, onow;
    float interpolate, out0, out1;

    float output = 0.0;

    sample0 = this->IndexInt;
    sample1 = (sample0 + 1) & WAVETABLE_MASK;
	interpolate = CCetoneSynth::Int2FloatTab[this->IndexFrac];

	switch (this->Waveform)
	{
	case OWAVE_SAW:
        out0 = this->SawPtr[sample0];
        out1 = this->SawPtr[sample1];
        output = (1.f - interpolate) * out0 + interpolate * out1;
		break;
	case OWAVE_PULSE:
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
	case OWAVE_TRI:
        out0 = this->ParabolaPtr[sample0];
        out1 = this->ParabolaPtr[sample1];

        sample0 += WAVETABLE_LENGTH2;
        sample1 += WAVETABLE_LENGTH2;
        sample0 &= WAVETABLE_MASK;
        sample1 &= WAVETABLE_MASK;

        out0 -= this->ParabolaPtr[sample0];
        out1 -= this->ParabolaPtr[sample1];

        output = (1.f - interpolate) * out0 + interpolate * out1;
        output *= 0.5f;
		break;
	case OWAVE_C64NOISE:
		onow = ((sr & 0x400000) >> 11) |
			((sr & 0x100000) >> 10) |
			((sr & 0x010000) >> 7) |
			((sr & 0x002000) >> 5) |
			((sr & 0x000800) >> 4) |
			((sr & 0x000080) >> 1) |
			((sr & 0x000010) << 1) |
			((sr & 0x000004) << 2);
		output = CCetoneSynth::Int2FloatTab2[onow << 4];
		break;
	case OWAVE_SINE:
		onow = (this->IndexInt << 4) + (this->IndexFrac >> 12);
        out0 = CCetoneSynth::SineTable[onow];
        out1 = CCetoneSynth::SineTable[(onow + 1) & 65535];
        output = (1.f - interpolate) * out0 + interpolate * out1;
		break;
	}

	this->IndexFrac += this->StepFrac;

	if (this->IndexFrac > 65535)
	{
		this->IndexFrac &= 65535;
		this->IndexInt++;
	}

	this->IndexInt += this->StepInt;

	if((this->IndexInt >> 12) != 0)
	{
        int bit0 = ((sr >> 22) ^ (sr >> 17)) & 0x1;
        sr <<= 1;
        sr &= 0x7fffff;
        sr |= bit0;
		this->ShiftRegister = sr;
 	}

	this->IndexInt &= WAVETABLE_MASK;
	return output;
}