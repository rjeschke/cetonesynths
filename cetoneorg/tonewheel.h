#pragma once

#include "defines.h"
#include "statics.h"

class ToneWheel
{
public:
	ToneWheel(int mul, int f0, int f1)
	{
		int i;
		this->Frequency = (float)mul * (20.0f / (float)f1) * (float)f0;	
		for(i = 0; i < 61; i++)
			this->Volumes[i] = 0.0f;
		this->Volume = this->destVolume = 0.0f;
		this->Index.int32 = 0;
		this->Envelope = 0;
		this->Attack = 0.0001f;
	};

	~ToneWheel()
	{
	};

	void setWave(float* wave)
	{
		this->Sine = wave;
	}

	void setSampleRate(float fs)
	{
		this->FS = fs;
		this->Step.int32 = truncate( ((this->Frequency * SINE_SIZEf) / fs) * 65536.0f + 0.5f);
		this->volStep = 1.0f / (fs * this->Attack);
		this->volDownStep = 1.0f / (fs * 0.025f);
	};

	void setAttack(float val)
	{
		this->Attack = val;
		this->volStep = 1.0f / (this->FS * this->Attack);
	}

	void NoteOn(int note, float vol, int delay)
	{
		int i;
		float v = 0.0;
		
		if(vol == 0.0f)
			return;

		if(this->Volume == 0.0f)
		{
			this->Index.fixed.index = irand() & (SINE_SIZE - 1);
			this->Delay = delay;
		}
		else
			this->Delay = 0;

		this->Volumes[note] = vol;

		for(i = 0; i < 61; i++)
		{
			if(this->Volumes[i] > v)
				v = this->Volumes[i];
		}

		this->destVolume = v;
		this->Envelope = -1;
	}
	
	void NoteOff(int note)
	{
		int i;
		float v = 0.0;

		this->Volumes[note] = 0.0f;

		for(i = 0; i < 61; i++)
		{
			if(this->Volumes[i] > v)
				v = this->Volumes[i];
		}

		this->destVolume = v;
		this->Envelope = -1;
	}

	__forceinline void Render(float* out, int samples, int* vibrato)
	{
		if(this->Volume || this->destVolume)
		{
			int i;
			for(i = 0; i < samples; i++)
			{
				if(this->Delay)
				{
					this->Delay--;
					continue;
				}

				out[i] += spline((float)this->Index.fixed.frac / 65536.0f, this->Sine + this->Index.fixed.index - 1) * this->Volume;

				this->Index.int32 = (this->Index.int32 + this->Step.int32 + vibrato[i]) & (((SINE_SIZE - 1) * 0x10000) | 0xffff);
				
				if(this->Envelope)
				{
					if(this->Volume < this->destVolume)
					{
						this->Volume += this->volStep;
						if(this->Volume > this->destVolume)
						{
							this->Volume = this->destVolume;
							this->Envelope = 0;
						}
					}
					else
					{
						this->Volume -= this->volDownStep;
						if(this->Volume < this->destVolume)
						{
							this->Volume = this->destVolume;
							this->Envelope = 0;
						}
					}
				}
			}
		}
	}
private:
	float* Sine;
	float Frequency;
	float FS;
	float Volume;
	float destVolume;
	float volStep;
	float volDownStep;
	float Volumes[61];
	float Attack;
	int Delay;
	int Envelope;
	sSTEP Step;
	sSTEP Index;
};
