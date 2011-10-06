#pragma once

#include "defines.h"
#include "statics.h"
#include <malloc.h>
#include <memory.h>
#include <math.h>

// Chorus/Flanger Effect
/*
Parameters:
Depth Delay L/R
Feedback
LFO Speed
*/
class Chorus
{
public:
	Chorus()
	{
		this->FS = 0.0f;
		this->Size = 0;
		this->bufl = this->bufr = 0;
		this->lfoPos.int32 = this->lfoStep.int32 = 0;
		this->Position = 0;
		this->params[0] = 18;
		this->params[1] = 19;
		this->params[2] = 50;
		this->params[3] = 7;

		int i;
		for(i = 0; i < 16384; i++)
			this->l_sine[i] = (sinf(PI * (float)i / 8192) + 1.0f) * 0.5f;
	}

	~Chorus()
	{
		if(this->bufl) free(this->bufl);
		if(this->bufr) free(this->bufr);
	}

	void Reset()
	{
		this->Position = 0;
		this->lfoPos.int32 = 0;
		memset(this->bufl, 0, this->Size * sizeof(float));
		memset(this->bufr, 0, this->Size * sizeof(float));
	}

	void setDelayL(int val)
	{
		this->params[0] = val;
		float v = (float)val / 127.0f;
		this->DelayL = this->FS * (0.04f * v * v + 0.001f);
	}

	void setDelayR(int val)
	{
		this->params[1] = val;
		float v = (float)val / 127.0f;
		this->DelayR = this->FS * (0.04f * v * v + 0.001f);
	}

	void setDepth(int val)
	{
		this->params[2] = val;
		float v = (float)val / 127.0f;
		this->Depth = this->FS * 0.02f * v * v;
	}

	void setFeedback(float val)
	{
		this->Feedback = val;
	}

	void setLfoSpeed(int val)
	{
		this->params[3] = val;
		float v = (float)val / 127.0f;
		this->lfoStep.int32 = truncate(((16384.0f * v * v * 4.0f + 0.005f) / this->FS) * 65536.0f);
	}

	void setSampleRate(float fs)
	{
		int size = truncate(fs * 0.5f);
		if(size != this->Size || this->FS != fs)
		{
			this->FS = fs;
			this->Size = size;
			if(this->bufl != 0)
			{
				this->bufl = (float*)realloc(this->bufl, size * sizeof(float));
				this->bufr = (float*)realloc(this->bufr, size * sizeof(float));
			}
			else
			{
				this->bufl = (float*)malloc(size * sizeof(float));
				this->bufr = (float*)malloc(size * sizeof(float));
			}
			memset(this->bufl, 0, size * sizeof(float));
			memset(this->bufr, 0, size * sizeof(float));

			this->setDelayL(this->params[0]);
			this->setDelayR(this->params[1]);
			this->setDepth(this->params[2]);
			this->setLfoSpeed(this->params[3]);
		}
	}

	void Render(float *inl, float* inr, int samples, float dry, float wet)
	{
		int i;
		for(i = 0; i < samples; i++)
		{
			float dd, f0, ol, or, tmp;
			int d, d0, d1;
			float lfo = this->ProcessLFO() * this->Depth;
			
			dd = lfo + this->DelayL;
			d = truncate(floorf(dd));

			d0 = (this->Position - d + this->Size - 1) % this->Size;
			d1 = (d0 + 1) % this->Size;
			f0 = dd - (float)d;

			ol = this->bufl[d1] + f0 * (this->bufl[d0] - this->bufl[d1]);

			dd = lfo + this->DelayR;
			d = truncate(floorf(dd));

			d0 = (this->Position - d + this->Size - 1) % this->Size;
			d1 = (d0 + 1) % this->Size;
			f0 = dd - (float)d;

			or = this->bufr[d1] + f0 * (this->bufr[d0] - this->bufr[d1]);

			tmp = inl[i] + this->Feedback * ol;
			UNDENORM(tmp);
			this->bufl[this->Position] = tanhf(tmp);

			tmp = inr[i] + this->Feedback * or;
			UNDENORM(tmp);
			this->bufr[this->Position] = tanhf(tmp);

			inl[i] = ol * wet + inl[i] * dry;
			inr[i] = or * wet + inr[i] * dry;

			this->Position = (this->Position + 1) % this->Size;
		}
	}

private:
	float ProcessLFO()
	{
		float s0 = this->l_sine[this->lfoPos.fixed.index];
		float s1 = this->l_sine[(this->lfoPos.fixed.index + 1) & 16383];
		float out = s0 + ((float)this->lfoPos.fixed.frac / 65536.0f) * (s1 - s0);
		this->lfoPos.int32 += this->lfoStep.int32;
		this->lfoPos.fixed.index &= 16383;
		return out;
	}

	int params[8];
	float FS;
	int Size;
	int Position;
	float* bufl;
	float *bufr;
	float DelayL, DelayR;
	float Depth;
	float Feedback;
	sSTEP lfoPos;
	sSTEP lfoStep;
	float l_sine[16384];
};
