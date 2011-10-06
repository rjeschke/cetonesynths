#pragma once

#include <math.h>
#include "defines.h"
#include "statics.h"

#define FILTER_INLINE

#define FILTM_12DB 0
#define FILTM_24DB 1

#define FILTT_LP 0
#define FILTT_BP 1
#define FILTT_HP 2
#define FILTT_BR 3

#define DRY 0.65
#define WET 0.35
#define BPCO 440.0
#define DENORM_CONST 1e-20

class Filter
{
public:
	Filter();
	~Filter();
	void Reset();
	void SetSampleRate(float fs);
	void SetType(int type);
	void SetMode(int mode);

	__forceinline void SetGain(float gain)
	{
		this->gain = gain * 0.5f;
	}

	__forceinline void Set(float cutoff, float q)
	{
		int co = truncate(cutoff * 32768.0f);
		if(co < 0)
			co = 0;
		else if(co > 32767)
			co = 32767;

		if(q < 0.f)
			q = 0.f;
		else if(q > 1.f)
			q = 1.f;

		q = 1.f + q * 9.f;

		double cs = g_fcos[co];
		double alpha = g_fsin[co] / q;
		double a0 = 1.0 / (1.0 + alpha);
		
		switch(this->type)
		{
		case FILTT_LP:
			this->b2 = this->b0 = (1.0 - cs) * (0.5 * a0);
			this->b1 = (1.0 - cs) * a0;
			this->a1 = (2.0 * cs) * a0;
			this->a2 = (alpha - 1.0) * a0;
			break;
		case FILTT_HP:
			this->b2 = this->b0 = (1.0 + cs) * (0.5 * a0);
			this->b1 = -((1.0 + cs) * a0);
			this->a1 = (2.0 * cs) * a0;
			this->a2 = (alpha - 1.0) * a0;
			break;
		case FILTT_BP:
			this->b0 = alpha * a0;
			this->b1 = 0.0;
			this->b2 = -this->b0;
			this->a1 = (2.0 * cs) * a0;
			this->a2 = (alpha - 1.0) * a0;
			break;
		case FILTT_BR:
			this->b2 = this->b0 = a0;
			this->b1 = (-2.0 * cs) * a0;
			this->a1 = -this->b1;
			this->a2 = (alpha - 1.0) * a0;
			break;
		}
	}       

#ifdef FILTER_INLINE
	__forceinline	float CallProcess(float _inputout)
	{
		if(this->modus)
		{
			double inputout = (double)_inputout;
			this->buffer2[2] = this->buffer2[1];
			this->buffer2[1] = this->buffer2[0];
			this->buffer2[0] = inputout;
			this->buffer2[5] = this->buffer2[4];
			this->buffer2[4] = this->buffer2[3];
			this->buffer2[3] = 
				this->b0b * this->buffer2[0] 
				+ this->b1b * this->buffer2[1] 
				+ this->b2b * this->buffer2[2]
				+ this->a1b * this->buffer2[4] 
				+ this->a2b * this->buffer2[5] + DENORM_CONST;

			inputout = DRY * inputout + WET * this->buffer2[3];
			
			this->buffer0[2] = this->buffer0[1];
			this->buffer0[1] = this->buffer0[0];
			this->buffer0[0] = (this->lastIn + inputout) * 0.5;
			this->buffer0[5] = this->buffer0[4];
			this->buffer0[4] = this->buffer0[3];
			this->buffer0[3] = 
				this->b0 * this->buffer0[0] 
				+ this->b1 * this->buffer0[1] 
				+ this->b2 * this->buffer0[2] 
				+ this->a1 * this->buffer0[4]	
				+ this->a2 * this->buffer0[5] + DENORM_CONST;
			
			this->buffer0[2] = this->buffer0[1];
			this->buffer0[1] = this->buffer0[0];
			this->lastIn = this->buffer0[0] = inputout;
			this->buffer0[5] = this->buffer0[4];
			this->buffer0[4] = this->buffer0[3];
			this->buffer0[3] = 
				this->b0 * this->buffer0[0] 
				+ this->b1 * this->buffer0[1] 
				+ this->b2 * this->buffer0[2] 
				+ this->a1 * this->buffer0[4]	
				+ this->a2 * this->buffer0[5] + DENORM_CONST;

			this->buffer1[2] = this->buffer1[1];
			this->buffer1[1] = this->buffer1[0];
			this->buffer1[0] = (this->lastIn2 + this->buffer0[3]) * 0.5;
			this->buffer1[5] = this->buffer1[4];
			this->buffer1[4] = this->buffer1[3];
			this->buffer1[3] = 
				this->b0 * this->buffer1[0] 
				+ this->b1 * this->buffer1[1] 
				+ this->b2 * this->buffer1[2] 
				+ this->a1 * this->buffer1[4]	
				+ this->a2 * this->buffer1[5] + DENORM_CONST;
			
			this->buffer1[2] = this->buffer1[1];
			this->buffer1[1] = this->buffer1[0];
			this->lastIn2 = this->buffer1[0] = this->buffer0[3];
			this->buffer1[5] = this->buffer1[4];
			this->buffer1[4] = this->buffer1[3];
			this->buffer1[3] = 
				this->b0 * this->buffer1[0] 
				+ this->b1 * this->buffer1[1] 
				+ this->b2 * this->buffer1[2] 
				+ this->a1 * this->buffer1[4]	
				+ this->a2 * this->buffer1[5] + DENORM_CONST;

			if(this->buffer0[3] > 320.0)
				this->Reset();

			return tanhf((float)this->buffer1[3] * this->gain);	
		}
		else
		{
			double inputout = (double)_inputout;
			this->buffer2[2] = this->buffer2[1];
			this->buffer2[1] = this->buffer2[0];
			this->buffer2[0] = inputout;
			this->buffer2[5] = this->buffer2[4];
			this->buffer2[4] = this->buffer2[3];
			this->buffer2[3] = 
				this->b0b * this->buffer2[0] 
				+ this->b1b * this->buffer2[1] 
				+ this->b2b * this->buffer2[2]
				+ this->a1b * this->buffer2[4] 
				+ this->a2b * this->buffer2[5] + DENORM_CONST;

			inputout = DRY * inputout + WET * this->buffer2[3];
			
			this->buffer0[2] = this->buffer0[1];
			this->buffer0[1] = this->buffer0[0];
			this->buffer0[0] = (this->lastIn + inputout) * 0.5;
			this->buffer0[5] = this->buffer0[4];
			this->buffer0[4] = this->buffer0[3];
			this->buffer0[3] = 
				this->b0 * this->buffer0[0] 
				+ this->b1 * this->buffer0[1] 
				+ this->b2 * this->buffer0[2] 
				+ this->a1 * this->buffer0[4]	
				+ this->a2 * this->buffer0[5] + DENORM_CONST;
			
			this->buffer0[2] = this->buffer0[1];
			this->buffer0[1] = this->buffer0[0];
			this->lastIn = this->buffer0[0] = inputout;
			this->buffer0[5] = this->buffer0[4];
			this->buffer0[4] = this->buffer0[3];
			this->buffer0[3] = 
				this->b0 * this->buffer0[0] 
				+ this->b1 * this->buffer0[1] 
				+ this->b2 * this->buffer0[2] 
				+ this->a1 * this->buffer0[4]	
				+ this->a2 * this->buffer0[5] + DENORM_CONST;

			if(this->buffer0[3] > 320.0)
				this->Reset();

			return tanhf((float)this->buffer0[3] * this->gain);	
		}
	};
#else
	__forceinline	float CallProcess(float input) { return (this->*Process)(input); };
	float	(Filter::*Process)(float);

	float Process12(float inputout);
	float Process24(float inputout);
#endif
private:
	double a1, a2;
	double b0, b1, b2;
	double buffer0[6];
	double buffer1[6];
	double a1b, a2b;
	double b0b, b1b, b2b;
	double buffer2[6];
	double lastIn, lastIn2;

	int type;
	int modus;
	float gain;
};
