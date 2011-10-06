#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "Synth.h"
#include "parameters.h"
#include <windows.h>

Synth::Synth(sPRG* ps)
{
	int i;
	for(i = 0; i < 16; i++)
		this->Voices[i] = new SVoice(&(this->sPrg));

	this->iBlockSize = 0;
	this->fLeft = this->fRight = 0;
	this->programs = ps;
	this->rlfo[0] = new LFO();
	this->rlfo[1] = new LFO();
	this->Reset();
	memset(&(this->sPrg), 0, sizeof(sPRG));
	this->FS = 0;
	this->oBpm = 0;
	this->_CPU = 0;
	this->curProgram = 0;
}

Synth::~Synth()
{
	int i;
	for(i = 0; i < 16; i++)
		delete this->Voices[i];

	delete this->rlfo[0];
	delete this->rlfo[1];

	if(this->fLeft != 0)
	{
		free(this->fLeft);
		free(this->fRight);
	}
}

void Synth::SetBlockSize(int size)
{
	if(size > this->iBlockSize)
	{
		if(this->fLeft != 0)
		{
			this->fLeft = (float*)realloc(this->fLeft, sizeof(float) * size);
			this->fRight = (float*)realloc(this->fLeft, sizeof(float) * size);
		}
		else
		{
			this->fLeft = (float*)malloc(sizeof(float) * size);
			this->fRight = (float*)malloc(sizeof(float) * size);
		}
		this->iBlockSize = size;
	}
}

void Synth::SetSampleRate(float fs)
{
	if(this->FS != fs)
	{
		int i;
		for(i = 0; i < 16; i++)
			this->Voices[i]->SetSampleRate(fs);

		this->rlfo[0]->SetSampleRate(fs);
		this->rlfo[1]->SetSampleRate(fs);
		this->FS = fs;
		this->oBpm = 0.0f;
	}
}

void Synth::SetTempo(float bpm)
{
	if(this->oBpm != bpm)
	{
		int i;
		this->oBpm = bpm;
		this->spq = (this->FS * 30.0f) / (bpm * 1.5f);
		for(i = 0; i < 16; i++)
			this->Voices[i]->SetTempo(this->spq);
		this->rlfo[0]->SetTempo(this->spq);
		this->rlfo[1]->SetTempo(this->spq);
	}
}

void Synth::VoiceOn(int v, int note, int vel)
{
	this->midiMap[note] = v;
	this->Voices[v]->NoteOn(note, vel, this->lastNote);
	this->lastNote = note;
}

void Synth::NoteOn(int note, int vel)
{
	unsigned __int32 time;

	if(!this->sPrg.voices)
		this->VoiceOn(0, note, vel);
	else
	{
		int i, v;

		int p = 0;
		for(i = 0; i < 128; i++)
		{
			v = this->midiMap[i];
			if(v != -1)
			{
				if(v > this->sPrg.voices)
					this->midiMap[i] = -1;
				else if(this->Voices[v]->IsDone())
					this->midiMap[i] = -1;
				else
					p++;
			}
		}

		v = this->midiMap[note];
		if(v != -1)
		{
			this->VoiceOn(v, note, vel);
			return;
		}

		v = -1;
		for(i = 0; i <= this->sPrg.voices; i++)
		{
			if(this->Voices[i]->IsDone())
			{
				this->VoiceOn(i, note, vel);
				return;
			}
		}

		time = (unsigned __int32)GetTickCount();
		v = -1;
		for(i = 0; i <= this->sPrg.voices; i++)
		{
			if(this->Voices[i]->IsRelease() && this->Voices[i]->GetTime() < time)
			{
				time = this->Voices[i]->GetTime();
				v = i;
			}
		}

		if(v != -1)
		{
			this->VoiceOn(v, note, vel);
			return;
		}

		time = (unsigned __int32)GetTickCount();
		v = -1;
		for(i = 0; i <= this->sPrg.voices; i++)
		{
			if(this->Voices[i]->GetTime() < time)
			{
				time = this->Voices[i]->GetTime();
				v = i;
			}
		}

		if(v != -1)
			this->VoiceOn(v, note, vel);
	}
}

void Synth::NoteOff(int note)
{
	if(!this->sPrg.voices)
	{
		if(this->lastNote == note)
			this->Voices[0]->NoteOff();
		this->midiMap[note] = -1;
	}
	else
	{
		int v = this->midiMap[note];
		if(v != -1)
		{
			this->midiMap[note] = -1;
			this->Voices[v]->NoteOff();
		}
	}
}

void Synth::InitRender()
{
	memset(this->fLeft, 0, this->iBlockSize * sizeof(float));
	memset(this->fRight, 0, this->iBlockSize * sizeof(float));
	this->iFramePos = 0;
}

void Synth::Render(int frames, float** inputs)
{
	int i;
	float* l = this->fLeft + this->iFramePos;
	float* r = this->fRight + this->iFramePos;
	float* inl = inputs[0] + this->iFramePos;
	float* inr = inputs[1] + this->iFramePos;
	for(i = 0; i <= this->sPrg.voices; i++)
	{
		if(!this->Voices[i]->IsDone())
			this->Voices[i]->Render(l, r, inl, inr, frames);
	}

	if(this->guiOpen)
	{
		for(i = 0; i < frames; i++)
		{
			this->rlfo[0]->Process();
			this->rlfo[1]->Process();
		}
	}

	this->iFramePos += frames;
}

void Synth::Reset()
{
	int i;

	for(i = 0; i < 128; i++)
		this->midiMap[i] = -1;

	for(i = 0; i < 16; i++)
		this->Voices[i]->Reset();

	this->rlfo[0]->Reset();
	this->rlfo[1]->Reset();
	this->lastNote = -1;
}

#define OMASK(x,y,z) (x) = ((x) & (~y)) | (z)

void Synth::SetProgram(int prg)
{
	this->curProgram = prg;
	this->ChangeVoices(this->programs[prg].voices);
	memcpy(&(this->sPrg), &(this->programs[prg]), sizeof(sPRG));

	this->rlfo[0]->SetSpeed(this->sPrg.lfo[0].speed);
	this->rlfo[1]->SetSpeed(this->sPrg.lfo[1].speed);

	this->ReadProgram();
}

bool Synth::HasPrgChanged()
{
	memcpy(this->sPrg.name, this->programs[this->curProgram].name, 26);
	return (memcmp(&(this->programs[this->curProgram]), &(this->sPrg), sizeof(sPRG)) == 0) ? false : true;
}

void Synth::Recall()
{
	this->SetProgram(this->curProgram);
}

void Synth::Store()
{
	memcpy(this->sPrg.name, this->programs[this->curProgram].name, 26);
	memcpy(&(this->programs[this->curProgram]), &(this->sPrg), sizeof(sPRG));
}

void Synth::ReadProgram()
{
	this->Reset();

	this->NotifyVoices(pOWave1, this->sPrg.osc[0].wave);
	this->NotifyVoices(pOWave2, this->sPrg.osc[1].wave);
	this->NotifyVoices(pOWave3, this->sPrg.osc[2].wave);

	this->NotifyVoices(pOMode1, this->sPrg.osc[0].mode);
	this->NotifyVoices(pOMode2, this->sPrg.osc[1].mode);
	this->NotifyVoices(pOMode3, this->sPrg.osc[2].mode);

	this->NotifyVoices(pOFlags1, this->sPrg.osc[0].flags);
	this->NotifyVoices(pOFlags2, this->sPrg.osc[1].flags);
	this->NotifyVoices(pOFlags3, this->sPrg.osc[2].flags);

	this->NotifyVoices(pOSpread1, this->sPrg.osc[0].spread);
	this->NotifyVoices(pOSpread2, this->sPrg.osc[1].spread);
	this->NotifyVoices(pOSpread3, this->sPrg.osc[2].spread);

	this->NotifyVoices(pAttack1, this->sPrg.env[0].a);
	this->NotifyVoices(pHold1, this->sPrg.env[0].h);
	this->NotifyVoices(pDecay1, this->sPrg.env[0].d);
	this->NotifyVoices(pSustain1, this->sPrg.env[0].s);
	this->NotifyVoices(pRelease1, this->sPrg.env[0].r);

	this->NotifyVoices(pAttack2, this->sPrg.env[1].a);
	this->NotifyVoices(pHold2, this->sPrg.env[1].h);
	this->NotifyVoices(pDecay2, this->sPrg.env[1].d);
	this->NotifyVoices(pSustain2, this->sPrg.env[1].s);
	this->NotifyVoices(pRelease2, this->sPrg.env[1].r);

	this->NotifyVoices(pAttack3, this->sPrg.env[2].a);
	this->NotifyVoices(pHold3, this->sPrg.env[2].h);
	this->NotifyVoices(pDecay3, this->sPrg.env[2].d);
	this->NotifyVoices(pSustain3, this->sPrg.env[2].s);
	this->NotifyVoices(pRelease3, this->sPrg.env[2].r);

	this->NotifyVoices(pFMode, this->sPrg.filter.db);
	this->NotifyVoices(pFType, this->sPrg.filter.mode);
	this->NotifyVoices(pFExp, this->sPrg.filter.exp);
	this->NotifyVoices(pFGain, this->sPrg.filter.gain);

	this->NotifyVoices(pArpSpeed, this->sPrg.arp.speed);
	this->NotifyVoices(pArpLength, this->sPrg.arp.length);
	this->NotifyVoices(pArpNote1, this->sPrg.arp.note[0]);
	this->NotifyVoices(pArpNote2, this->sPrg.arp.note[1]);
	this->NotifyVoices(pArpNote3, this->sPrg.arp.note[2]);
	this->NotifyVoices(pArpNote4, this->sPrg.arp.note[3]);
	this->NotifyVoices(pArpNote5, this->sPrg.arp.note[4]);
	this->NotifyVoices(pArpNote6, this->sPrg.arp.note[5]);
	this->NotifyVoices(pArpNote7, this->sPrg.arp.note[6]);
	this->NotifyVoices(pArpNote8, this->sPrg.arp.note[7]);
	this->NotifyVoices(pArpSync, this->sPrg.arp.sync);
	this->NotifyVoices(pArpLoop, this->sPrg.arp.loop);
	
	this->NotifyVoices(pL1Speed, this->sPrg.lfo[0].speed);
	this->NotifyVoices(pL1PW, this->sPrg.lfo[0].pw);
	this->NotifyVoices(pL1Keytrack, this->sPrg.lfo[0].keytrack);
	this->NotifyVoices(pL1Delay, this->sPrg.lfo[0].delay);
	this->NotifyVoices(pL1Sync, this->sPrg.lfo[0].sync);
	this->NotifyVoices(pL1Retrig, this->sPrg.lfo[0].retrig);
	this->NotifyVoices(pL1SNH, this->sPrg.lfo[0].snh);
	this->NotifyVoices(pL1Wave, this->sPrg.lfo[0].wave);
	this->rlfo[0]->SetSpeed(this->sPrg.lfo[0].speed * 32);
	this->rlfo[0]->SetSync(this->sPrg.lfo[0].sync);

	this->NotifyVoices(pL2Speed, this->sPrg.lfo[1].speed);
	this->NotifyVoices(pL2PW, this->sPrg.lfo[1].pw);
	this->NotifyVoices(pL2Keytrack, this->sPrg.lfo[1].keytrack);
	this->NotifyVoices(pL2Delay, this->sPrg.lfo[1].delay);
	this->NotifyVoices(pL2Sync, this->sPrg.lfo[1].sync);
	this->NotifyVoices(pL2Retrig, this->sPrg.lfo[1].retrig);
	this->NotifyVoices(pL2SNH, this->sPrg.lfo[1].snh);
	this->NotifyVoices(pL2Wave, this->sPrg.lfo[1].wave);
	this->rlfo[1]->SetSpeed(this->sPrg.lfo[1].speed * 32);
	this->rlfo[1]->SetSync(this->sPrg.lfo[1].sync);
}

int Synth::GetParameter(int id)
{
	switch(id)
	{
	case pGlide:	return this->sPrg.glide;
	//********************************************************************************
	//***** OSC                                                                  *****
	//********************************************************************************
	case pOSemi1:	return this->sPrg.osc[0].semi;
	case pOSemi2:	return this->sPrg.osc[1].semi;
	case pOSemi3:	return this->sPrg.osc[2].semi;
	case pOFine1:	return this->sPrg.osc[0].fine;
	case pOFine2:	return this->sPrg.osc[1].fine;
	case pOFine3:	return this->sPrg.osc[2].fine;
	case pOMorph1:	return this->sPrg.osc[0].morph;
	case pOMorph2:	return this->sPrg.osc[1].morph;
	case pOMorph3:	return this->sPrg.osc[2].morph;
	case pOWave1:	return this->sPrg.osc[0].wave;
	case pOWave2:	return this->sPrg.osc[1].wave;
	case pOWave3:	return this->sPrg.osc[2].wave;
	case pOMode1:	return this->sPrg.osc[0].mode;
	case pOMode2:	return this->sPrg.osc[1].mode;
	case pOMode3:	return this->sPrg.osc[2].mode;
	case pOFlags1:	return this->sPrg.osc[0].flags;
	case pOFlags2:	return this->sPrg.osc[1].flags;
	case pOFlags3:	return this->sPrg.osc[2].flags;
	case pOSpread1:	return this->sPrg.osc[0].spread;
	case pOSpread2:	return this->sPrg.osc[1].spread;
	case pOSpread3:	return this->sPrg.osc[2].spread;
	//********************************************************************************
	//***** ENVELOPES                                                            *****
	//********************************************************************************
	case pAttack1:	return this->sPrg.env[0].a;
	case pHold1:	return this->sPrg.env[0].h;
	case pDecay1:	return this->sPrg.env[0].d;
	case pSustain1:	return this->sPrg.env[0].s;
	case pRelease1:	return this->sPrg.env[0].r;
	case pAttack2:	return this->sPrg.env[1].a;
	case pHold2:	return this->sPrg.env[1].h;
	case pDecay2:	return this->sPrg.env[1].d;
	case pSustain2:	return this->sPrg.env[1].s;
	case pRelease2:	return this->sPrg.env[1].r;
	case pAttack3:	return this->sPrg.env[2].a;
	case pHold3:	return this->sPrg.env[2].h;
	case pDecay3:	return this->sPrg.env[2].d;
	case pSustain3:	return this->sPrg.env[2].s;
	case pRelease3:	return this->sPrg.env[2].r;
	//********************************************************************************
	//***** MIXER                                                                *****
	//********************************************************************************
	case pMixVol:	return this->sPrg.mixer.volume;
	case pMixVelsens:	return this->sPrg.mixer.velsens;
	case pMixPan:	return this->sPrg.mixer.panning;
	case pMixVol1:	return this->sPrg.mixer.osc1;
	case pMixVol2:	return this->sPrg.mixer.osc2;
	case pMixVol3:	return this->sPrg.mixer.osc3;
	//********************************************************************************
	//***** FILTER                                                               *****
	//********************************************************************************
	case pFCutoff:	return this->sPrg.filter.cutoff;
	case pFQ:		return this->sPrg.filter.q;
	case pFVelsens: return this->sPrg.filter.velsens;
	case pFGain:	return this->sPrg.filter.gain;
	case pFEnv2:	return this->sPrg.filter.env2;
	case pFMode:	return this->sPrg.filter.db;
	case pFType:	return this->sPrg.filter.mode;
	case pFExp:		return this->sPrg.filter.exp;
	//********************************************************************************
	//***** ARPEGGIATOR                                                          *****
	//********************************************************************************
	case pArpSpeed: return this->sPrg.arp.speed;
	case pArpLength: return this->sPrg.arp.length;
	case pArpNote1: return this->sPrg.arp.note[0];
	case pArpNote2: return this->sPrg.arp.note[1];
	case pArpNote3: return this->sPrg.arp.note[2];
	case pArpNote4: return this->sPrg.arp.note[3];
	case pArpNote5: return this->sPrg.arp.note[4];
	case pArpNote6: return this->sPrg.arp.note[5];
	case pArpNote7: return this->sPrg.arp.note[6];
	case pArpNote8: return this->sPrg.arp.note[7];
	case pArpSync: return this->sPrg.arp.sync;
	case pArpLoop: return this->sPrg.arp.loop;
	//********************************************************************************
	//***** LFO                                                                  *****
	//********************************************************************************
	case pL1Speed: return this->sPrg.lfo[0].speed;
	case pL2Speed: return this->sPrg.lfo[1].speed;
	case pL1Delay: return this->sPrg.lfo[0].delay;
	case pL2Delay: return this->sPrg.lfo[1].delay;
	case pL1PW: return this->sPrg.lfo[0].pw;
	case pL2PW: return this->sPrg.lfo[1].pw;
	case pL1Keytrack: return this->sPrg.lfo[0].keytrack;
	case pL2Keytrack: return this->sPrg.lfo[1].keytrack;
	case pL1Sync: return this->sPrg.lfo[0].sync;
	case pL2Sync: return this->sPrg.lfo[1].sync;
	case pL1SNH: return this->sPrg.lfo[0].snh;
	case pL2SNH: return this->sPrg.lfo[1].snh;
	case pL1Retrig: return this->sPrg.lfo[0].retrig;
	case pL2Retrig: return this->sPrg.lfo[1].retrig;
	case pL1Wave: return this->sPrg.lfo[0].wave;
	case pL2Wave: return this->sPrg.lfo[1].wave;
	//********************************************************************************
	//***** MOD                                                                  *****
	//********************************************************************************
	case pM1Source: return this->sPrg.mod[0].source;
	case pM1Amount: return this->sPrg.mod[0].amount;
	case pM1Dest: return this->sPrg.mod[0].dest;

	case pM2Source: return this->sPrg.mod[1].source;
	case pM2Amount: return this->sPrg.mod[1].amount;
	case pM2Dest: return this->sPrg.mod[1].dest;

	case pM3Source: return this->sPrg.mod[2].source;
	case pM3Amount: return this->sPrg.mod[2].amount;
	case pM3Dest: return this->sPrg.mod[2].dest;

	case pM4Source: return this->sPrg.mod[3].source;
	case pM4Amount: return this->sPrg.mod[3].amount;
	case pM4Dest: return this->sPrg.mod[3].dest;

	case pM5Source: return this->sPrg.mod[4].source;
	case pM5Amount: return this->sPrg.mod[4].amount;
	case pM5Dest: return this->sPrg.mod[4].dest;

	case pM6Source: return this->sPrg.mod[5].source;
	case pM6Amount: return this->sPrg.mod[5].amount;
	case pM6Dest: return this->sPrg.mod[5].dest;

	case pAudioVol: return this->sPrg.audioVol;
	case pAudioPan: return this->sPrg.audioPan;
	}
	return 0;
}

void Synth::SetParameter(int id, int val)
{
	switch(id)
	{
	case pModwheel:		this->NotifyVoices(id, val); break;
	case pAftertouch:	this->NotifyVoices(id, val); break;
	case pVoices:	this->ChangeVoices(val); break;
	case pGlide:	this->sPrg.glide = val; break;
	//********************************************************************************
	//***** OSC                                                                  *****
	//********************************************************************************
	case pOSemi1:	this->sPrg.osc[0].semi = val; break;
	case pOSemi2:	this->sPrg.osc[1].semi = val; break;
	case pOSemi3:	this->sPrg.osc[2].semi = val; break;
	case pOFine1:	this->sPrg.osc[0].fine = val; break;
	case pOFine2:	this->sPrg.osc[1].fine = val; break;
	case pOFine3:	this->sPrg.osc[2].fine = val; break;
	case pOMorph1:	this->sPrg.osc[0].morph = val; break;
	case pOMorph2:	this->sPrg.osc[1].morph = val; break;
	case pOMorph3:	this->sPrg.osc[2].morph = val; break;
	case pOWave1:	this->sPrg.osc[0].wave = val; this->NotifyVoices(id, val); break;
	case pOWave2:	this->sPrg.osc[1].wave = val; this->NotifyVoices(id, val); break;
	case pOWave3:	this->sPrg.osc[2].wave = val; this->NotifyVoices(id, val); break;
	case pOMode1:	this->sPrg.osc[0].mode = val; this->NotifyVoices(id, val); break;
	case pOMode2:	this->sPrg.osc[1].mode = val; this->NotifyVoices(id, val); break;
	case pOMode3:	this->sPrg.osc[2].mode = val; this->NotifyVoices(id, val); break;
	case pOFlags1:	this->sPrg.osc[0].flags = val; this->NotifyVoices(id, val); break;
	case pOFlags2:	this->sPrg.osc[1].flags = val; this->NotifyVoices(id, val); break;
	case pOFlags3:	this->sPrg.osc[2].flags = val; this->NotifyVoices(id, val); break;
	case pOSpread1:	this->sPrg.osc[0].spread = val; this->NotifyVoices(id, val); break;
	case pOSpread2:	this->sPrg.osc[1].spread = val; this->NotifyVoices(id, val); break;
	case pOSpread3:	this->sPrg.osc[2].spread = val; this->NotifyVoices(id, val); break;
	//********************************************************************************
	//***** ENVELOPES                                                            *****
	//********************************************************************************
	case pAttack1:	this->sPrg.env[0].a = val; this->NotifyVoices(id, val); break;
	case pHold1:	this->sPrg.env[0].h = val; this->NotifyVoices(id, val); break;
	case pDecay1:	this->sPrg.env[0].d = val; this->NotifyVoices(id, val); break;
	case pSustain1:	this->sPrg.env[0].s = val; this->NotifyVoices(id, val); break;
	case pRelease1:	this->sPrg.env[0].r = val; this->NotifyVoices(id, val); break;
	case pAttack2:	this->sPrg.env[1].a = val; this->NotifyVoices(id, val); break;
	case pHold2:	this->sPrg.env[1].h = val; this->NotifyVoices(id, val); break;
	case pDecay2:	this->sPrg.env[1].d = val; this->NotifyVoices(id, val); break;
	case pSustain2:	this->sPrg.env[1].s = val; this->NotifyVoices(id, val); break;
	case pRelease2:	this->sPrg.env[1].r = val; this->NotifyVoices(id, val); break;
	case pAttack3:	this->sPrg.env[2].a = val; this->NotifyVoices(id, val); break;
	case pHold3:	this->sPrg.env[2].h = val; this->NotifyVoices(id, val); break;
	case pDecay3:	this->sPrg.env[2].d = val; this->NotifyVoices(id, val); break;
	case pSustain3:	this->sPrg.env[2].s = val; this->NotifyVoices(id, val); break;
	case pRelease3:	this->sPrg.env[2].r = val; this->NotifyVoices(id, val); break;
	//********************************************************************************
	//***** MIXER                                                                *****
	//********************************************************************************
	case pMixVol:	this->sPrg.mixer.volume = val; break;
	case pMixVelsens:	this->sPrg.mixer.velsens = val; break;
	case pMixPan:	this->sPrg.mixer.panning = val; break;
	case pMixVol1:	this->sPrg.mixer.osc1 = val; break;
	case pMixVol2:	this->sPrg.mixer.osc2 = val; break;
	case pMixVol3:	this->sPrg.mixer.osc3 = val; break;
	//********************************************************************************
	//***** FILTER                                                               *****
	//********************************************************************************
	case pFCutoff: this->sPrg.filter.cutoff = val; break;
	case pFQ: this->sPrg.filter.q = val; break;
	case pFVelsens: this->sPrg.filter.velsens = val; break;
	case pFGain: this->sPrg.filter.gain = val; this->NotifyVoices(id, val); break;
	case pFEnv2: this->sPrg.filter.env2 = val; break;
	case pFMode: this->sPrg.filter.db = val; this->NotifyVoices(id, val); break;
	case pFType: this->sPrg.filter.mode = val; this->NotifyVoices(id, val); break;
	case pFExp: this->sPrg.filter.exp = val;  this->NotifyVoices(id, val); break;
	//********************************************************************************
	//***** ARPEGGIATOR                                                          *****
	//********************************************************************************
	case pArpSpeed: this->sPrg.arp.speed = val; this->NotifyVoices(id, val); break;
	case pArpLength: this->sPrg.arp.length = val; this->NotifyVoices(id, val); break;
	case pArpNote1: this->sPrg.arp.note[0] = val; this->NotifyVoices(id, val); break;
	case pArpNote2: this->sPrg.arp.note[1] = val; this->NotifyVoices(id, val); break;
	case pArpNote3: this->sPrg.arp.note[2] = val; this->NotifyVoices(id, val); break;
	case pArpNote4: this->sPrg.arp.note[3] = val; this->NotifyVoices(id, val); break;
	case pArpNote5: this->sPrg.arp.note[4] = val; this->NotifyVoices(id, val); break;
	case pArpNote6: this->sPrg.arp.note[5] = val; this->NotifyVoices(id, val); break;
	case pArpNote7: this->sPrg.arp.note[6] = val; this->NotifyVoices(id, val); break;
	case pArpNote8: this->sPrg.arp.note[7] = val; this->NotifyVoices(id, val); break;
	case pArpSync: this->sPrg.arp.sync = val; this->NotifyVoices(id, val); break;
	case pArpLoop: this->sPrg.arp.loop = val; this->NotifyVoices(id, val); break;
	//********************************************************************************
	//***** LFO                                                                  *****
	//********************************************************************************
	case pL1Speed: this->sPrg.lfo[0].speed = val; this->rlfo[0]->SetSpeed(val * 32); this->NotifyVoices(id, val); break;
	case pL2Speed: this->sPrg.lfo[1].speed = val; this->rlfo[1]->SetSpeed(val * 32); this->NotifyVoices(id, val); break;
	case pL1Delay: this->sPrg.lfo[0].delay = val; this->NotifyVoices(id, val); break;
	case pL2Delay: this->sPrg.lfo[1].delay = val; this->NotifyVoices(id, val); break;
	case pL1PW: this->sPrg.lfo[0].pw = val; this->NotifyVoices(id, val); break;
	case pL2PW: this->sPrg.lfo[1].pw = val; this->NotifyVoices(id, val); break;
	case pL1Keytrack: this->sPrg.lfo[0].keytrack = val; this->NotifyVoices(id, val); break;
	case pL2Keytrack: this->sPrg.lfo[1].keytrack = val; this->NotifyVoices(id, val); break;
	case pL1Sync: this->sPrg.lfo[0].sync = val; this->rlfo[0]->SetSync(val); this->NotifyVoices(id, val); break;
	case pL2Sync: this->sPrg.lfo[1].sync = val; this->rlfo[1]->SetSync(val); this->NotifyVoices(id, val); break;
	case pL1SNH: this->sPrg.lfo[0].snh = val; this->NotifyVoices(id, val); break;
	case pL2SNH: this->sPrg.lfo[1].snh = val; this->NotifyVoices(id, val); break;
	case pL1Retrig: this->sPrg.lfo[0].retrig = val; this->NotifyVoices(id, val); break;
	case pL2Retrig: this->sPrg.lfo[1].retrig = val; this->NotifyVoices(id, val); break;
	case pL1Wave: this->sPrg.lfo[0].wave = val; this->NotifyVoices(id, val); break;
	case pL2Wave: this->sPrg.lfo[1].wave = val; this->NotifyVoices(id, val); break;
	//********************************************************************************
	//***** MOD                                                                  *****
	//********************************************************************************
	case pM1Source: this->sPrg.mod[0].source = val; break;
	case pM1Amount: this->sPrg.mod[0].amount = val; break;
	case pM1Dest: this->sPrg.mod[0].dest = val; break;

	case pM2Source: this->sPrg.mod[1].source = val; break;
	case pM2Amount: this->sPrg.mod[1].amount = val; break;
	case pM2Dest: this->sPrg.mod[1].dest = val; break;

	case pM3Source: this->sPrg.mod[2].source = val; break;
	case pM3Amount: this->sPrg.mod[2].amount = val; break;
	case pM3Dest: this->sPrg.mod[2].dest = val; break;

	case pM4Source: this->sPrg.mod[3].source = val; break;
	case pM4Amount: this->sPrg.mod[3].amount = val; break;
	case pM4Dest: this->sPrg.mod[3].dest = val; break;

	case pM5Source: this->sPrg.mod[4].source = val; break;
	case pM5Amount: this->sPrg.mod[4].amount = val; break;
	case pM5Dest: this->sPrg.mod[4].dest = val; break;

	case pM6Source: this->sPrg.mod[5].source = val; break;
	case pM6Amount: this->sPrg.mod[5].amount = val; break;
	case pM6Dest: this->sPrg.mod[5].dest = val; break;

	case pAudioVol: this->sPrg.audioVol = val; break;
	case pAudioPan: this->sPrg.audioPan = val; break;
	}
}

void Synth::NotifyVoices(int id, int val)
{
	int i;
	for(i = 0; i < 16; i++)
		this->Voices[i]->UpdateValue(id, val);
}

void Synth::ChangeVoices(int nr)
{
	int i;
	if(nr > this->sPrg.voices)
	{
		for(i = this->sPrg.voices + 1; i <= nr; i++)
			this->Voices[i]->Reset();
		this->sPrg.voices = nr;
	}
	else if(nr < this->sPrg.voices)
	{
		this->sPrg.voices = nr;
	}
}

int Synth::GetVoices()
{
	int i, v = 0;
	for(i = 0; i <= this->sPrg.voices; i++)
	{
		if(!this->Voices[i]->IsDone())
			v++;
	}
	return v;
}

int Synth::GetLFOPos(int lfo)
{
	return this->rlfo[lfo]->GetPos();
}
