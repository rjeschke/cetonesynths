#pragma once

#include "defines.h"
#include "statics.h"
#include "tonewheel.h"
#include "lfo.h"
#include "chorus.h"
#include "reverb.h"

class Synth
{
public:
	Synth();
	~Synth();
	void NoteOn(int note, int vel);
	void NoteOff(int note);
	void InitRender();
	void FinishRender(int samples);
	void Render(int frames);
	void setSampleRate(float fs);
	void setBlockSize(int size);

	void Reset()
	{
		this->buffer[0] = 
			this->buffer[1] = 
			this->buffer[2] = 
			this->buffer[3] = 
			this->buffer[4] = 
			this->buffer[5] = 0.0f;
		this->buffer1[0] = 
			this->buffer1[1] = 
			this->buffer1[2] = 
			this->buffer1[3] = 
			this->buffer1[4] = 
			this->buffer1[5] = 0.0f;
		this->_Chorus->Reset();
		this->_Reverb->Reset();
	}

	void setTone(int val)
	{
		int i;
		for(i = 0; i < 89; i++)
			this->Wheels[i]->setWave(g_sine + val * SINE_SIZE * 3 + SINE_SIZE);
	}

	void setLfoSpeed(int val)
	{
		float v = (float)val / 127.0f;
		this->_LFO->setSpeed(v * v * 32.0f);
	}

	void setLfoPhase(int phase)
	{
		this->_LFO->setPhase(phase);
	}

	void setVolume(int val)
	{
		float v = (float)val / 127.0f;
		this->Volume = v * v * 4.0f;
	}

	void setDrawbars(int idx, int val)
	{
		this->volumes[idx] = ((float)val / 8.0f) * 0.5f;
	}

	void setVibDepth(int val)
	{
		this->VibDepth = (float)val / 127.0f * 0.95f;
	}

	void setPanDepth(int val)
	{
		this->PanDepth = (float)val / 127.0f * 0.48f;
	}

	void setChorusDelayL(int val)
	{
		this->_Chorus->setDelayL(val);
	}

	void setChorusDelayR(int val)
	{
		this->_Chorus->setDelayR(val);
	}

	void setChorusSpeed(int val)
	{
		this->_Chorus->setLfoSpeed(val);
	}

	void setChorusDepth(int val)
	{
		this->_Chorus->setDepth(val);
	}

	void setChorusMix(int val)
	{
		float v = (float)val / 127.0f;
		this->Chorus_dry = 1.0f - v;
		this->Chorus_wet = v;

		if(val > 0)
		{
			if(!this->isChorus)
				this->_Chorus->Reset();
			this->isChorus = -1;
		}
		else
			this->isChorus = 0;
	}

	void setChorusFeedback(int val)
	{
		this->_Chorus->setFeedback((float)(val - 64) / 65.0f);
	}

	void setClickAmount(int val)
	{
		int i;
		float v = (float)(127 - val) / 127.0f;
		float click = v * v * 0.005f + 0.00001f;
		for(i = 0; i < 89; i++)
			this->Wheels[i]->setAttack(click);
	}

	void setRevDamp(int val)
	{
		this->_Reverb->setDamp((float)val / 127.0f);
	}

	void setRevRoom(int val)
	{
		this->_Reverb->setRoomsize((float)val / 127.0f);
	}

	void setRevWidth(int val)
	{
		this->_Reverb->setWidth((float)val / 127.0f);
	}

	void setRevDry(int val)
	{
		this->_Reverb->setDry((float)val / 127.0f);
	}

	void setRevWet(int val)
	{
		this->_Reverb->setWet((float)val / 127.0f);
		if(val > 0)
		{
			if(!this->isReverb)
				this->_Reverb->Reset();
			this->isReverb = -1;
		}
		else
			this->isReverb = 0;
	}

	void setOutVol(int val)
	{
		float v = (float)val / 127.0f;
		this->OutVol = v * v * 4.0f;
	}

	float *fLeft;
	float *fRight;

	float volumes[9];
	float Volume;
	float VibDepth;
	float PanDepth;
private:
	ToneWheel *Wheels[89];
	float FS;
	float *mixBuf;
	float *lfo1;
	float *lfo2;
	int *vibrato;
	LFO *_LFO;
	Chorus *_Chorus;
	Reverb *_Reverb;
	int iBlockSize;
	int iFramePos;
	int isChorus;
	int isReverb;
	float OutVol;
	float a1, a2;
	float b0, b1, b2;
	float buffer[6];
	float buffer1[6];
	float Chorus_wet, Chorus_dry;
};
