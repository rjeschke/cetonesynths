#include "cetone033.h"
#include "CetoneLpFilter.h"

CCetoneLpFilter::CCetoneLpFilter(void)
{
	this->FilterBiquad = new CFilterBiquad();
	this->FilterMoog = new CFilterMoog();

	this->Type = 0;
	this->Set(1.f, 0.f);
}

CCetoneLpFilter::~CCetoneLpFilter(void)
{
	if(this->FilterMoog != NULL)
		delete this->FilterMoog;
	if(this->FilterBiquad != NULL)
		delete this->FilterBiquad;

	this->FilterMoog = NULL;
	this->FilterBiquad = NULL;
	this->Type = -1;
}

int CCetoneLpFilter::GetType()
{
	return this->Type;
}

void CCetoneLpFilter::Reset()
{
	if (this->Type == -1)
		return;

	this->FilterBiquad->Reset();
	this->FilterMoog->Reset();
}

void CCetoneLpFilter::SetSampleRate(float fs)
{
	if (this->Type == -1)
		return;

	this->FilterBiquad->SetSampleRate(fs);
}

void CCetoneLpFilter::SetType(int type)
{
	if(type < 0)
		type = 0;
	else if(type > FILTER_TYPE_MAX)
		type = FILTER_TYPE_MAX;

	this->Type = type;

	this->Set(this->Cutoff, this->Resonance);
}

void CCetoneLpFilter::Set(float cutoff, float q)
{
	this->Cutoff = cutoff;
	this->Resonance = q;

	switch(this->Type)
	{
	case FILTER_TYPE_BIQUAD:
		this->FilterBiquad->Set(cutoff, q);
		break;
	case FILTER_TYPE_MOOG:
		this->FilterMoog->Set(cutoff, q);
		break;
	}
}

float CCetoneLpFilter::Run(float inputout)
{

	switch(this->Type)
	{
	case FILTER_TYPE_BIQUAD:
		inputout = this->FilterBiquad->Run(inputout);
		break;
	case FILTER_TYPE_MOOG:
		inputout = this->FilterMoog->Run(inputout);
		break;
	}

	return inputout;
}

void CCetoneLpFilter::Name(int type, char* text)
{
	switch(type)
	{
	case FILTER_TYPE_BIQUAD:	vst_strncpy (text, "Biquad", kVstMaxParamStrLen);	break;
	case FILTER_TYPE_MOOG:		vst_strncpy (text, "Moogle", kVstMaxParamStrLen);	break;
	default:					text[0] = 0;										break;
	}
}
