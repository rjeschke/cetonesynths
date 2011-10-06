#include <math.h>

#include "cetonesynth.h"

#include "Filter303.h"

CFilter303::CFilter303(void)
{
	this->Reset();

	this->Q = 1.0;

	this->Set(CCetoneSynth::SampleRate2, 0.f, 0.f);
}

CFilter303::~CFilter303(void)
{

}

void CFilter303::Reset()
{
	this->a			= 0.f;
	this->b			= 0.f;
	this->c			= 0.f;
	this->d1		= 0.f;
	this->d2		= 0.f;
}

void CFilter303::Set(float cutoff, float q, float mod)
{
	if (cutoff < 0.f)
		cutoff = 0.f;
	else if (cutoff > CCetoneSynth::SampleRate2)
		cutoff = CCetoneSynth::SampleRate2;
	
	if (q < 0.f)
		q = 0.f;
	else if (q > 1.f)
		q = 1.f;

	if (mod < -1.f)
		mod = -1.f;
	else if(mod > 1.f)
		mod = 1.f;

	this->CutOff	= cutoff;
	if(q != this->Q)
	{
		this->Q			= q;
		this->res_coef = expf(-1.2f + 3.455f * q); 
	}

	float w = expf(6.109f + 1.5876f * mod + 2.1553f * (cutoff * CCetoneSynth::SampleRate2_1) - 1.2f * (1.f - q)) * CCetoneSynth::SampleRatePi;
	float k = expf(-w / this->res_coef);

	this->a = 2.f * cosf(2.f * w) * k;
	this->b = -k * k;
	this->c = 1.f - this->a - this->b;

	/*
	vcf_e1 = exp(6.109 + 1.5876*vcf_envmod + 2.1553*vcf_cutoff - 1.2*(1.0-vcf_reso));
	vcf_e0 = exp(5.613 - 0.8*vcf_envmod + 2.1553*vcf_cutoff - 0.7696*(1.0-vcf_reso));
	vcf_e0*=M_PI/44100.0;
	vcf_e1*=M_PI/44100.0;
	vcf_e1 -= vcf_e0;
	vcf_envpos = ENVINC;

	w = vcf_e0 + vcf_c0;
	k = exp(-w/vcf_rescoeff);
	//vcf_c0 *= vcf_envdecay;
	vcf_a = 2.0*cos(2.0*w) * k;
	vcf_b = -k*k;
	vcf_c = 1.0 - vcf_a - vcf_b;
	vcf_envpos = 0;
*/
}

void CFilter303::SetMode(int mode)
{
	// Nothing here
}

int CFilter303::GetMode()
{
	return FMODE_LOW;
}

float CFilter303::Run(float val)
{
	float output;

	output = this->a * this->d1 + this->b * this->d2 + this->c * val;
	this->d2 = this->d1;
	this->d1 = output;

	/*
	// compute sample
	outbuf[i]=vcf_a*vcf_d1 + vcf_b*vcf_d2 + vcf_c*vco_k*vca_a;
	vcf_d2=vcf_d1; 
	vcf_envpos++;
	vcf_d1=outbuf[i];
	*/

	return output;
}
