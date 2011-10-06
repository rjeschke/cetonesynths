#include "Synth.h"
#include "data.h"
#include <malloc.h>
#include <memory.h>
#include <math.h>

Synth::Synth()
{
	int i;
	for(i = 0; i < 89; i++)
	{
		this->Wheels[i] = new ToneWheel(g_rotors[i * 3], g_rotors[i * 3 + 1], g_rotors[i * 3 + 2]);
		this->Wheels[i]->setWave(g_sine + 127 * SINE_SIZE * 3 + SINE_SIZE);
	}

	this->_LFO = new LFO();
	this->_Chorus = new Chorus();
	this->_Reverb = new Reverb();
	this->isChorus = 0;
	this->isReverb = 0;
	this->iBlockSize = 0;
	this->fLeft = this->fRight = this->mixBuf = this->lfo1 = this->lfo2 = 0;
	this->vibrato = 0;
	this->VibDepth = 0.0f;
	this->PanDepth = 0.0f;
	this->Chorus_dry = 1.0f;
	this->Chorus_wet = 0.0f;
	this->Volume = 0.2f;
	this->OutVol = 0.5f;

	this->volumes[0] = 0.5f;
	this->volumes[1] = 0.5f;
	this->volumes[2] = 0.5f;
	this->volumes[3] = 0.5f;
	this->volumes[4] = 0.5f;
	this->volumes[5] = 0.5f;
	this->volumes[6] = 0.5f;
	this->volumes[7] = 0.5f;
	this->volumes[8] = 0.5f;

	this->Reset();
}

Synth::~Synth()
{
	int i;
	for(i = 0; i < 89; i++)
		delete this->Wheels[i];
	
	delete this->_LFO;
	delete this->_Chorus;
	delete this->_Reverb;

	if(this->fLeft) free(this->fLeft);
	if(this->fRight) free(this->fRight);
	if(this->mixBuf) free(this->mixBuf);
	if(this->lfo1) free(this->lfo1);
	if(this->lfo2) free(this->lfo2);
	if(this->vibrato) free(this->vibrato);
}

void Synth::setSampleRate(float fs)
{
	int i;
	for(i = 0; i < 89; i++)
		this->Wheels[i]->setSampleRate(fs);
	this->FS = fs;

	float omega = 2.0f * PI * 2900.0f / fs;
	float cs = cosf(omega);
	float sn = sinf(omega);
	float a0 = 1.0f / (1.0f + sn);
	
	this->b2 = this->b0 = (1.0f - cs) * (0.5f * a0);
	this->b1 = (1.0f - cs) * a0;
	this->a1 = (2.0f * cs) * a0;
	this->a2 = (sn - 1.0f) * a0;

	this->_Chorus->setSampleRate(fs);
	this->_LFO->setSampleRate(fs);
	this->_Reverb->setSampleRate(fs);
}

void Synth::setBlockSize(int size)
{
	if(size > this->iBlockSize)
	{
		if(this->fLeft != 0)
		{
			this->fLeft = (float*)realloc(this->fLeft, sizeof(float) * size);
			this->fRight = (float*)realloc(this->fLeft, sizeof(float) * size);
			this->mixBuf = (float*)realloc(this->mixBuf, sizeof(float) * size);
			this->lfo1 = (float*)realloc(this->lfo1, sizeof(float) * size);
			this->lfo2 = (float*)realloc(this->lfo2, sizeof(float) * size);
			this->vibrato = (int*)realloc(this->vibrato, sizeof(int) * size);
		}
		else
		{
			this->fLeft = (float*)malloc(sizeof(float) * size);
			this->fRight = (float*)malloc(sizeof(float) * size);
			this->mixBuf = (float*)malloc(sizeof(float) * size);
			this->lfo1 = (float*)malloc(sizeof(float) * size);
			this->lfo2 = (float*)malloc(sizeof(float) * size);
			this->vibrato = (int*)malloc(sizeof(int) * size);
		}
		this->iBlockSize = size;
	}
}

void Synth::InitRender()
{
	memset(this->mixBuf, 0, this->iBlockSize * sizeof(float));
	this->iFramePos = 0;
}

void Synth::FinishRender(int samples)
{
	int i;
	for(i = 0; i < samples; i++)
	{
		float v = tanhf(this->mixBuf[i] * this->Volume);
		float pan = this->lfo2[i] * this->PanDepth + 0.5f;
		this->fLeft[i] = v * sqrtf(1.0f - pan);
		this->fRight[i] = v * sqrtf(pan);
	}

	if(this->isChorus)
		this->_Chorus->Render(this->fLeft, this->fRight, samples, this->Chorus_dry, this->Chorus_wet);

	if(this->isReverb)
		this->_Reverb->Render(this->fLeft, this->fRight, samples);

	for(i = 0; i < samples; i++)
	{
		this->buffer[2] = this->buffer[1];
		this->buffer[1] = this->buffer[0];
		this->buffer[0] = this->fLeft[i];
		this->buffer[5] = this->buffer[4];
		this->buffer[4] = this->buffer[3];
		this->buffer[3] = 
			this->b0 * this->buffer[0] 
			+ this->b1 * this->buffer[1] 
			+ this->b2 * this->buffer[2] 
			+ this->a1 * this->buffer[4]	
			+ this->a2 * this->buffer[5];		

		UNDENORM(this->buffer[3]);
		this->fLeft[i] = this->buffer[3] * this->OutVol;

		this->buffer1[2] = this->buffer1[1];
		this->buffer1[1] = this->buffer1[0];
		this->buffer1[0] = this->fRight[i];
		this->buffer1[5] = this->buffer1[4];
		this->buffer1[4] = this->buffer1[3];
		this->buffer1[3] = 
			this->b0 * this->buffer1[0] 
			+ this->b1 * this->buffer1[1] 
			+ this->b2 * this->buffer1[2] 
			+ this->a1 * this->buffer1[4]	
			+ this->a2 * this->buffer1[5];		

		UNDENORM(this->buffer1[3]);
		this->fRight[i] = this->buffer1[3] * this->OutVol;
	}		
}

void Synth::NoteOn(int note, int vel)
{
	int i;
	float v = ((float)(127 - vel) * 0.00003f + 0.00008f) * this->FS;
	for(i = 0; i < 9; i++)
		this->Wheels[g_generators[note].offset[i]]->NoteOn(note, this->volumes[i], truncate(v * (float)i));
}

void Synth::NoteOff(int note)
{
	int i;
	for(i = 0; i < 9; i++)
		this->Wheels[g_generators[note].offset[i]]->NoteOff(note);
}

void Synth::Render(int frames)
{
	int i;
	float* l = this->mixBuf + this->iFramePos;
	float* l1 = this->lfo1 + this->iFramePos;
	float* l2 = this->lfo2 + this->iFramePos;
	int* vib = this->vibrato + this->iFramePos;

	this->_LFO->Render(l1, l2, frames);

	for(i = 0; i < frames; i++)
		vib[i] = truncate((l1[i] * this->VibDepth) * 65536.0f);

	for(i = 0; i < 89; i++)
		this->Wheels[i]->Render(l, frames, vib);

	this->iFramePos += frames;
}
