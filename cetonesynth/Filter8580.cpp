#include "CetoneSynth.h"
#include <math.h>
#include "Filter8580.h"

CFilter8580::CFilter8580(void)
{
	this->Reset();
	this->CalcClock();

	this->SetMode(FMODE_LOW);
	this->Set(CCetoneSynth::SampleRate2, 0.f);
}

CFilter8580::~CFilter8580(void)
{
}

void CFilter8580::Reset()
{
	this->Vlp = 0.0;
	this->Vbp = 0.0;
	this->Vhp = 0.0;
}

void CFilter8580::CalcClock()
{
	this->Clock = (int)(200000.f / CCetoneSynth::SampleRate + 0.5f);
}

void CFilter8580::Set(float cutoff, float q)
{
	if (cutoff < 0.f)
		cutoff = 0.f;
	else if (cutoff > 12000.f)
		cutoff = 12000.f;
	
	if (q < 0.f)
		q = 0.f;
	else if (q > 1.f)
		q = 1.f;

	this->w0 = 2.f * CCetoneSynth::Pi * cutoff * 0.000005f;
	this->q_par = 1.f / (0.707f + q);

	this->CutOff = cutoff;
	this->Q = q;
}

void CFilter8580::SetMode(int mode)
{
	this->Mode = mode;
}

int CFilter8580::GetMode()
{
	return this->Mode;
}

float CFilter8580::Run(float val)
{
	float output = 0.f;

	float w0_ = this->w0;
	float Vbp_ = this->Vbp;
	float Vhp_ = this->Vhp;
	float Vlp_ = this->Vlp;
	float q_par_ = this->q_par;

	for(int i = 0; i < this->Clock; i++)
	{
		Vlp_ -= w0_ * Vbp_;
		Vbp_ -= w0_ * Vhp_;
		Vhp_ = (Vbp_ * q_par_) - Vlp_ - val;
	}

	this->Vlp = Vlp_;
	this->Vhp = Vhp_;
	this->Vbp = Vbp_;

	switch(this->Mode)
	{
	default:
		return 0.f;
	case FMODE_LOW:
		return Vlp_;
	case FMODE_BAND:
		return Vbp_;
	case FMODE_HIGH:
		return Vhp_;
	case FMODE_NOTCH:
		return Vhp_ + Vlp_;
	}
}