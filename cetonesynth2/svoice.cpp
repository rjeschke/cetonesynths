#include <windows.h>
#include "svoice.h"
#include "statics.h"
#include "parameters.h"

SVoice::SVoice(sPRG* p)
{
	int i;
	this->osc[0] = new OSC();
	this->osc[1] = new OSC();
	this->osc[2] = new OSC();

	this->osc[0]->SetSlave(this->osc[2]);
	this->osc[2]->SetMaster(this->osc[0]);
	this->osc[1]->SetSlave(this->osc[0]);
	this->osc[0]->SetMaster(this->osc[1]);
	this->osc[2]->SetSlave(this->osc[1]);
	this->osc[1]->SetMaster(this->osc[2]);

	this->env[0] = new Envelope();
	this->env[1] = new Envelope();
	this->env[2] = new Envelope();

	this->lfo[0] = new LFO();
	this->lfo[1] = new LFO();

	this->filter = new Filter();
	this->arp = new ARP();

	this->prg = p;

	for(i = 0; i < 128; i++)
		this->glides[i] = 0;
}

SVoice::~SVoice()
{
	delete this->osc[0];
	delete this->osc[1];
	delete this->osc[2];

	delete this->env[0];
	delete this->env[1];
	delete this->env[2];

	delete this->lfo[0];
	delete this->lfo[1];
	delete this->arp;
	delete this->filter;
}

//********************************************************************************
//***** MOD SLOT DEFINE STARTS                                               *****
//********************************************************************************

#define MD_FCO 0
#define MD_FQ 1
#define MD_FGAIN 2
#define MD_VOLUME 3
#define MD_PAN 4
#define MD_VOL1 5
#define MD_VOL2 6
#define MD_VOL3 7
#define MD_PITCH1 8
#define MD_PITCH2 9
#define MD_PITCH3 10
#define MD_MORPH1 11
#define MD_MORPH2 12
#define MD_MORPH3 13
#define MD_SPREAD1 14
#define MD_SPREAD2 15
#define MD_SPREAD3 16
#define MD_LFO1SPEED 17
#define MD_LFO2SPEED 18
#define MD_PITCH 19
#define MD_MOD6AMOUNT 20
#define MD_AUDIOVOL 21
#define MD_AUDIOPAN 22

#define PMD 1200.0f

#define DO_MODSLOT(x) \
{ \
	if(this->prg->mod[x].source)\
	{ \
		switch(this->prg->mod[x].dest) \
		{ \
		case MD_FCO: \
			co += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			break; \
		case MD_FQ: \
			q += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			break; \
		case MD_FGAIN: \
			fgain += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			fgain = nCLAMP(0.0f, fgain, 4.0f); \
			break; \
		case MD_VOLUME: \
			mv += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 4.0f; \
			mv = nCLAMP(0.0f, mv, 4.0f); \
			break; \
		case MD_PAN: \
			pan += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			pan = nCLAMP(0, pan, 1.0f); \
			break; \
		case MD_VOL1: \
			v0 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			v0 = nCLAMP(0.0f, v0, 2.0f); \
			break; \
		case MD_VOL2: \
			v1 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			v1 = nCLAMP(0.0f, v1, 2.0f); \
			break; \
		case MD_VOL3: \
			v2 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			v2 = nCLAMP(0.0f, v2, 2.0f); \
			break; \
		case MD_PITCH1: \
			p0 += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_PITCH2: \
			p1 += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_PITCH3: \
			p2 += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_MORPH1: \
			pw0 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 2048.0f); \
			break; \
		case MD_MORPH2: \
			pw1 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 2048.0f); \
			break; \
		case MD_MORPH3: \
			pw2 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 2048.0f); \
			break; \
		case MD_SPREAD1: \
			spr0 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 32.0f); \
			this->osc[0]->SetSpread(spr0); \
			break; \
		case MD_SPREAD2: \
			spr1 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 32.0f); \
			this->osc[1]->SetSpread(spr1); \
			break; \
		case MD_SPREAD3: \
			spr2 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 32.0f); \
			this->osc[2]->SetSpread(spr2); \
			break; \
		case MD_LFO1SPEED: \
			l0speed += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 4096.0f); \
			break; \
		case MD_LFO2SPEED: \
			l1speed += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 4096.0f); \
			break; \
		case MD_PITCH: \
			a += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_MOD6AMOUNT: \
			mod6 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			break; \
		case MD_AUDIOVOL: \
			avol += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			avol = nCLAMP(0.0f, avol, 2.0f); \
			break; \
		case MD_AUDIOPAN: \
			apan += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			apan = nCLAMP(0, apan, 1.0f); \
			break; \
		} \
	} \
}

#define DO_MODSLOT6(x) \
{ \
	if(this->prg->mod[x].source)\
	{ \
		switch(this->prg->mod[x].dest) \
		{ \
		case MD_FCO: \
			co += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			break; \
		case MD_FQ: \
			q += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			break; \
		case MD_FGAIN: \
			fgain += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			fgain = nCLAMP(0.0f, fgain, 4.0f); \
			break; \
		case MD_VOLUME: \
			mv += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 4.0f; \
			mv = nCLAMP(0.0f, mv, 4.0f); \
			break; \
		case MD_PAN: \
			pan += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			pan = nCLAMP(0, pan, 1.0f); \
			break; \
		case MD_VOL1: \
			v0 += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 2.0f; \
			v0 = nCLAMP(0.0f, v0, 2.0f); \
			break; \
		case MD_VOL2: \
			v1 += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 2.0f; \
			v1 = nCLAMP(0.0f, v1, 2.0f); \
			break; \
		case MD_VOL3: \
			v2 += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 2.0f; \
			v2 = nCLAMP(0.0f, v2, 2.0f); \
			break; \
		case MD_PITCH1: \
			p0 += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_PITCH2: \
			p1 += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_PITCH3: \
			p2 += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_MORPH1: \
			pw0 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 2048.0f); \
			break; \
		case MD_MORPH2: \
			pw1 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 2048.0f); \
			break; \
		case MD_MORPH3: \
			pw2 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 2048.0f); \
			break; \
		case MD_SPREAD1: \
			spr0 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 32.0f); \
			this->osc[0]->SetSpread(spr0); \
			break; \
		case MD_SPREAD2: \
			spr1 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 32.0f); \
			this->osc[1]->SetSpread(spr1); \
			break; \
		case MD_SPREAD3: \
			spr2 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 32.0f); \
			this->osc[2]->SetSpread(spr2); \
			break; \
		case MD_LFO1SPEED: \
			l0speed += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 4096.0f); \
			break; \
		case MD_LFO2SPEED: \
			l1speed += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 4096.0f); \
			break; \
		case MD_PITCH: \
			a += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_AUDIOVOL: \
			avol += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * mod6 * 2.0f; \
			avol = nCLAMP(0.0f, avol, 2.0f); \
			break; \
		case MD_AUDIOPAN: \
			apan += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * mod6; \
			apan = nCLAMP(0, apan, 1.0f); \
			break; \
		} \
	} \
}

//********************************************************************************
//***** MOD SLOT DEFINE ENDS                                                 *****
//********************************************************************************

void SVoice::Render(float *left, float *right, float *inl, float* inr, int frames)
{
	int i;
	int opw0, opw1, opw2;
	float opan, oapan;
	int ol0speed, ol1speed;
	float mod[14];
	float oco, oq;
	float ov0, ov1, ov2, ovm, fenv, ofgain, oavol;
	bool kt0, kt1, kt2;
	float keytrack;

	if(this->oscKT1)
		this->pitch0 = this->npitch + g_semi[this->prg->osc[0].semi] + g_fine[this->prg->osc[0].fine];
	else
		this->pitch0 = 6400 + g_semi[this->prg->osc[0].semi] + g_fine[this->prg->osc[0].fine];
	
	if(this->oscKT2)
		this->pitch1 = this->npitch + g_semi[this->prg->osc[1].semi] + g_fine[this->prg->osc[1].fine];
	else
		this->pitch1 = 6400 + g_semi[this->prg->osc[1].semi] + g_fine[this->prg->osc[1].fine];
	
	if(this->oscKT3)
		this->pitch2 = this->npitch + g_semi[this->prg->osc[2].semi] + g_fine[this->prg->osc[2].fine];
	else
		this->pitch2 = 6400 + g_semi[this->prg->osc[2].semi] + g_fine[this->prg->osc[2].fine];

	opw0 = this->prg->osc[0].morph * 16;
	opw1 = this->prg->osc[1].morph * 16;
	opw2 = this->prg->osc[2].morph * 16;

	opan = (float)this->prg->mixer.panning / 127.0f;
	oapan = (float)this->prg->audioPan / 127.0f;

	ov0 = g_m2vol[this->prg->mixer.osc1];
	ov1 = g_m2vol[this->prg->mixer.osc2];
	ov2 = g_m2vol[this->prg->mixer.osc3];
	oavol = g_m2vol[this->prg->audioVol];
	i = this->prg->mixer.volume + this->mvolVel;
	if(i < 0) i = 0; else if(i > 127) i = 127;
	ovm = g_m2vol[i] * 2.0f;

	kt0 = (this->prg->osc[0].flags & 0x80) ? true : false;
	kt1 = (this->prg->osc[1].flags & 0x80) ? true : false;
	kt2 = (this->prg->osc[2].flags & 0x80) ? true : false;

	oco = g_m2co[this->prg->filter.cutoff] + this->coVel;
	oq = g_m2f0[this->prg->filter.q];
	fenv = g_m2fenv[this->prg->filter.env2];

	keytrack = g_m2f1[this->currentNote];
	ol0speed = this->prg->lfo[0].speed * 32 + truncate(keytrack * g_m2mod[this->prg->lfo[0].keytrack] * 4096.0f);
	ol1speed = this->prg->lfo[1].speed * 32 + truncate(keytrack * g_m2mod[this->prg->lfo[1].keytrack] * 4096.0f);
	ofgain = g_m2fgain[this->prg->filter.gain];

	mod[1] = this->velocity;
	mod[9] = this->modwheel;
	mod[11] = this->aftertouch;

	for(i = 0; i < frames; i++)
	{
		float osc0, osc1, osc2;
		float v0 = ov0, v1 = ov1, v2 = ov2, mv = ovm, avol = oavol;
		float fgain = ofgain;
		float e0, e1, e2;
		float co = 0.0f, q = 0.0f, mix;
		float l0, l1;
		float mod6 = 0.0f;
		int l0speed = ol0speed;
		int l1speed = ol1speed;
		int spr0 = this->prg->osc[0].spread;
		int spr1 = this->prg->osc[1].spread;
		int spr2 = this->prg->osc[2].spread;
		float pan = opan, apan = oapan;
		int p0, p1, p2;
		int pw0, pw1, pw2;
		int a = 0, arp;

		e0 = this->env[0]->Process();
		e1 = this->env[1]->Process();
		e2 = this->env[2]->Process();
		l0 = this->lfo[0]->Process();
		l1 = this->lfo[1]->Process();
		arp = this->arp->Process();
		
		if(this->doGlide)
		{
			if(this->gStep.int32 < 0)
			{
				this->gIndex.int32 += this->gStep.int32;
				if(this->gIndex.fixed.index < this->gDest)
				{
					this->gIndex.fixed.index = this->gDest;
					this->gStep.int32 = 0;
				}
			}
			else if(this->gStep.int32 > 0)
			{
				this->gIndex.int32 += this->gStep.int32;
				this->gIndex.int32 += this->gStep.int32;
				if(this->gIndex.fixed.index > this->gDest)
				{
					this->gStep.int32 = 0;
					this->gIndex.fixed.index = this->gDest;
				}
			}
			a += this->gIndex.fixed.index;
			keytrack = g_m2f1[this->currentNote + this->gIndex.fixed.index / 100];
		}

		p0 = this->pitch0 + arp; // if(kt0) p0 += arp;
		p1 = this->pitch1 + arp; // if(kt1) p1 += arp;
		p2 = this->pitch2 + arp; // if(kt2) p2 += arp;

		pw0 = opw0;
		pw1 = opw1;
		pw2 = opw2;

		mod[2] = keytrack;
		mod[3] = l0;
		mod[4] = l1;
		mod[5] = e1;
		mod[6] = e2;
		mod[7] = l0 * e1;
		mod[8] = l0 * l1;
		mod[10] = this->modwheel * l1;
		mod[12] = inl[i];
		mod[13] = inr[i];

		DO_MODSLOT(0);
		DO_MODSLOT(1);
		DO_MODSLOT(2);
		DO_MODSLOT(3);
		DO_MODSLOT(4);
		DO_MODSLOT6(5);

		p0 += a;
		p1 += a;
		p2 += a;

		// Filter
		if(this->filtExp) 
			co += fenv * e2 * e2;
		else
			co += fenv * e2;

		this->iSet(iCUTOFF, co + oco);
		this->iDo(iCUTOFF);
		this->iSet(iQ, q + oq);
		this->iDo(iQ);
		this->filter->Set(this->iGet(iCUTOFF), this->iGet(iQ));

		// LFO
		this->lfo[0]->SetSpeed(l0speed);
		this->lfo[1]->SetSpeed(l1speed);

		this->iSet(iPW1, (float)pw0);
		this->iDo(iPW1);
		this->iSet(iPW2, (float)pw1);
		this->iDo(iPW2);
		this->iSet(iPW3, (float)pw2);
		this->iDo(iPW3);
		// OSC
		this->osc[0]->SetPw(truncate(this->iGet(iPW1)));
		this->osc[1]->SetPw(truncate(this->iGet(iPW2)));
		this->osc[2]->SetPw(truncate(this->iGet(iPW3)));

		this->osc[0]->SetPitch(p0);
		this->osc[1]->SetPitch(p1);
		this->osc[2]->SetPitch(p2);
		this->osc[0]->SetTables();
		this->osc[1]->SetTables();
		this->osc[2]->SetTables();

		osc0 = this->osc[0]->CallProcess();
		osc1 = this->osc[1]->CallProcess();
		osc2 = this->osc[2]->CallProcess();

		// Main Interpolation

		this->iSet(iOSC1VOL, v0);
		this->iDo(iOSC1VOL);
		v0 = this->iGet(iOSC1VOL);

		this->iSet(iOSC2VOL, v1);
		this->iDo(iOSC2VOL);
		v1 = this->iGet(iOSC2VOL);

		this->iSet(iOSC3VOL, v2);
		this->iDo(iOSC3VOL);
		v2 = this->iGet(iOSC3VOL);

		this->iSet(iAUDIOPAN, apan);
		this->iDo(iAUDIOPAN);
		this->iSet(iAUDIOVOL, avol);
		this->iDo(iAUDIOVOL);
		apan = this->iGet(iAUDIOPAN);
		mix = ((this->oscAM1) ? (osc0 * osc1 * v0) : (osc0 * v0))
			+ ((this->oscAM2) ? (osc1 * osc2 * v1) : (osc1 * v1))
			+ ((this->oscAM3) ? (osc2 * osc0 * v2) : (osc2 * v2))
			+ (inl[i] * (1.0f - apan) + inr[i] * apan) * this->iGet(iAUDIOVOL);
		
		this->iSet(iFGAIN, fgain);
		this->iDo(iFGAIN);
		this->filter->SetGain(this->iGet(iFGAIN));

		this->iSet(iMAINVOL, mv);
		this->iDo(iMAINVOL);
		mix = this->filter->CallProcess(mix) * e0 * this->iGet(iMAINVOL);

		this->iSet(iPAN, pan);
		this->iDo(iPAN);
		pan = this->iGet(iPAN);

		left[i] += mix * sqrtf(1.0f - pan);
		right[i] += mix * sqrtf(pan);
	}
}

void SVoice::SetSampleRate(float fs)
{
	this->FS = fs;

	this->filter->SetSampleRate(fs);

	this->env[0]->SetSampleRate(fs);
	this->env[1]->SetSampleRate(fs);
	this->env[2]->SetSampleRate(fs);

	this->lfo[0]->SetSampleRate(fs);
	this->lfo[1]->SetSampleRate(fs);

	this->arp->SetSampleRate(fs);

	int i;
	for(i = 0; i < 128; i++)
	{
		float f = (float)i / 128.0f;
		this->glides[i] = truncate((powf(2.0f, f * 12.0f) * 655360.0f) / fs);
	}

	this->iSamples = fs * iSPEED;
}

void SVoice::SetTempo(float tempo)
{
	this->arp->SetTempo(tempo);
	this->lfo[0]->SetTempo(tempo);
	this->lfo[1]->SetTempo(tempo);
}

void SVoice::Reset()
{
	this->osc[0]->Reset();
	this->osc[1]->Reset();
	this->osc[2]->Reset();

	this->filter->Reset();

	this->env[0]->Reset();
	this->env[1]->Reset();
	this->env[2]->Reset();

	this->lfo[0]->Reset();
	this->lfo[1]->Reset();

	this->iReset(iOSC1VOL);
	this->iReset(iOSC2VOL);
	this->iReset(iOSC3VOL);
	this->iReset(iMAINVOL);
	this->iReset(iPAN);
	this->iReset(iFGAIN);
	this->iReset(iCUTOFF);
	this->iReset(iQ);
	this->iReset(iAUDIOVOL);
	this->iReset(iAUDIOPAN);
	this->iReset(iPW1);
	this->iReset(iPW2);
	this->iReset(iPW3);
}

void SVoice::NoteOn(int note, int vel, int last)
{
	float fvel = this->velocity = g_m2f0[vel];
	this->time = (unsigned __int32) GetTickCount();

	if(note > NOTE_MAX1)
		note = NOTE_MAX1;

	if(this->prg->glide && note != last && last != -1)
	{
		this->currentNote = last;
		this->npitch = last * 100;
		this->gDest = note * 100 - this->npitch;
		this->gIndex.int32 = 0;
		this->gStep.int32 = (note < last) ? -this->glides[this->prg->glide] : this->glides[this->prg->glide];
		this->doGlide = true;
	}
	else
	{
		this->currentNote = note;
		this->npitch = note * 100;
		this->doGlide = false;
	}
	this->coVel = fvel * g_m2f1[this->prg->filter.velsens];
	this->mvolVel = truncate(fvel * g_m2f1[this->prg->mixer.velsens] * 128.0f);

	if(this->env[0]->state == sOff)
	{
		this->iReset(iOSC1VOL);
		this->iReset(iOSC2VOL);
		this->iReset(iOSC3VOL);
		this->iReset(iMAINVOL);
		this->iReset(iPAN);
		this->iReset(iFGAIN);
		this->iReset(iCUTOFF);
		this->iReset(iQ);
		this->iReset(iAUDIOVOL);
		this->iReset(iAUDIOPAN);
		this->iReset(iPW1);
		this->iReset(iPW2);
		this->iReset(iPW3);
		this->filter->Reset();
		this->osc[0]->SetPw(this->prg->osc[0].morph * 16);
		this->osc[1]->SetPw(this->prg->osc[1].morph * 16);
		this->osc[2]->SetPw(this->prg->osc[2].morph * 16);
	}

	this->env[1]->Reset();
	this->env[2]->Reset();

	this->env[0]->GateOn();
	this->env[1]->GateOn();
	this->env[2]->GateOn();

	this->arp->Retrigger();
	this->lfo[0]->Retrigger();
	this->lfo[1]->Retrigger();

}

void SVoice::NoteOff()
{
	this->env[0]->GateOff();
	this->env[1]->GateOff();
	this->env[2]->GateOff();
}

bool SVoice::IsDone()
{
	return (this->env[0]->state == sOff) ? true : false;
}

bool SVoice::IsRelease()
{
	return (this->env[0]->state == sRelease) ? true : false;
}

unsigned __int32 SVoice::GetTime()
{
	return this->time;
}

void SVoice::UpdateValue(int id, int val)
{
	switch(id)
	{
	case pModwheel:		this->modwheel = g_m2f0[val]; break;
	case pAftertouch:	this->aftertouch = g_m2f0[val]; break;
	//********************************************************************************
	//***** OSC                                                                  *****
	//********************************************************************************
	case pOWave1:	this->osc[0]->SetWave(val);	break;
	case pOWave2:	this->osc[1]->SetWave(val);	break;
	case pOWave3:	this->osc[2]->SetWave(val); break;
	case pOMode1:	this->osc[0]->SetMode(val); break;
	case pOMode2:	this->osc[1]->SetMode(val); break;
	case pOMode3:	this->osc[2]->SetMode(val); break;
	case pOSpread1:	this->osc[0]->SetSpread(val); break;
	case pOSpread2:	this->osc[1]->SetSpread(val); break;
	case pOSpread3:	this->osc[2]->SetSpread(val); break;
	case pOFlags1:	
		this->osc[1]->SetSync(val & 0x01);
		this->oscAM1 = (val & 0x02) ? true : false;
		this->oscKT1 = (val & 0x80) ? true : false;
		this->osc[0]->AlertAM(this->oscAM1);
		break;
	case pOFlags2:	
		this->osc[2]->SetSync(val & 0x01);
		this->oscAM2 = (val & 0x02) ? true : false;
		this->oscKT2 = (val & 0x80) ? true : false;
		this->osc[1]->AlertAM(this->oscAM2);
		break;
	case pOFlags3:	
		this->osc[0]->SetSync(val & 0x01);
		this->oscAM3 = (val & 0x02) ? true : false;
		this->oscKT3 = (val & 0x80) ? true : false;
		this->osc[2]->AlertAM(this->oscAM3);
		break;

	//********************************************************************************
	//***** ENVELOPES                                                            *****
	//********************************************************************************
	case pAttack1:	this->env[0]->Attack(val); break;
	case pHold1:	this->env[0]->Hold(val); break;
	case pDecay1:	this->env[0]->Decay(val); break;
	case pSustain1:	this->env[0]->Sustain(val); break;
	case pRelease1:	this->env[0]->Release(val); break;

	case pAttack2:	this->env[1]->Attack(val); break;
	case pHold2:	this->env[1]->Hold(val); break;
	case pDecay2:	this->env[1]->Decay(val); break;
	case pSustain2:	this->env[1]->Sustain(val); break;
	case pRelease2:	this->env[1]->Release(val); break;

	case pAttack3:	this->env[2]->Attack(val); break;
	case pHold3:	this->env[2]->Hold(val); break;
	case pDecay3:	this->env[2]->Decay(val); break;
	case pSustain3:	this->env[2]->Sustain(val); break;
	case pRelease3:	this->env[2]->Release(val); break;

	//********************************************************************************
	//***** FILTER                                                               *****
	//********************************************************************************
	case pFMode: this->filter->SetMode(val); break;
	case pFType: this->filter->SetType(val); break;
	case pFExp: this->filtExp = (val) ? true : false; break;
	case pFGain: this->filter->SetGain(g_m2fgain[val]); break;

	//********************************************************************************
	//***** ARPEGGIATOR                                                          *****
	//********************************************************************************
	case pArpSpeed: this->arp->SetSpeed(val); break;
	case pArpLength: this->arp->SetLength(val); break;
	case pArpNote1: this->arp->SetNote(0, val - 64); break;
	case pArpNote2: this->arp->SetNote(1, val - 64); break;
	case pArpNote3: this->arp->SetNote(2, val - 64); break;
	case pArpNote4: this->arp->SetNote(3, val - 64); break;
	case pArpNote5: this->arp->SetNote(4, val - 64); break;
	case pArpNote6: this->arp->SetNote(5, val - 64); break;
	case pArpNote7: this->arp->SetNote(6, val - 64); break;
	case pArpNote8: this->arp->SetNote(7, val - 64); break;
	case pArpSync: this->arp->SetSync(val); break;
	case pArpLoop: this->arp->SetLoop(val); break;
	//********************************************************************************
	//***** LFO                                                                  *****
	//********************************************************************************
	case pL1Speed: this->lfo[0]->SetSpeed(val * 32); break;
	case pL1Delay: this->lfo[0]->SetDelay(val); break;
	case pL1PW: this->lfo[0]->SetPW(val); break;
	case pL1Keytrack: /* TODO */ break;
	case pL1Sync: this->lfo[0]->SetSync(val); break;
	case pL1SNH: this->lfo[0]->SetSNH(val); break;
	case pL1Retrig: this->lfo[0]->SetRetrigger(val); break;
	case pL1Wave: this->lfo[0]->SetWave(val); break;

	case pL2Speed: this->lfo[1]->SetSpeed(val * 32); break;
	case pL2Delay: this->lfo[1]->SetDelay(val); break;
	case pL2PW: this->lfo[1]->SetPW(val); break;
	case pL2Keytrack: /* TODO */ break;
	case pL2Sync: this->lfo[1]->SetSync(val); break;
	case pL2SNH: this->lfo[1]->SetSNH(val); break;
	case pL2Retrig: this->lfo[1]->SetRetrigger(val); break;
	case pL2Wave: this->lfo[1]->SetWave(val); break;
	}
}

/*
#include <windows.h>
#include "svoice.h"
#include "statics.h"
#include "parameters.h"

SVoice::SVoice(sPRG* p)
{
	int i;
	this->osc[0] = new OSC();
	this->osc[1] = new OSC();
	this->osc[2] = new OSC();

	this->osc[0]->SetSlave(this->osc[2]);
	this->osc[2]->SetMaster(this->osc[0]);
	this->osc[1]->SetSlave(this->osc[0]);
	this->osc[0]->SetMaster(this->osc[1]);
	this->osc[2]->SetSlave(this->osc[1]);
	this->osc[1]->SetMaster(this->osc[2]);

	this->env[0] = new Envelope();
	this->env[1] = new Envelope();
	this->env[2] = new Envelope();

	this->lfo[0] = new LFO();
	this->lfo[1] = new LFO();

	this->filter = new Filter();
	this->arp = new ARP();

	this->prg = p;

	for(i = 0; i < 128; i++)
		this->glides[i] = 0;
}

SVoice::~SVoice()
{
	delete this->osc[0];
	delete this->osc[1];
	delete this->osc[2];

	delete this->env[0];
	delete this->env[1];
	delete this->env[2];

	delete this->lfo[0];
	delete this->lfo[1];
	delete this->arp;
	delete this->filter;
}

//********************************************************************************
//***** MOD SLOT DEFINE STARTS                                               *****
//********************************************************************************

#define MD_FCO 0
#define MD_FQ 1
#define MD_FGAIN 2
#define MD_VOLUME 3
#define MD_PAN 4
#define MD_VOL1 5
#define MD_VOL2 6
#define MD_VOL3 7
#define MD_PITCH1 8
#define MD_PITCH2 9
#define MD_PITCH3 10
#define MD_MORPH1 11
#define MD_MORPH2 12
#define MD_MORPH3 13
#define MD_SPREAD1 14
#define MD_SPREAD2 15
#define MD_SPREAD3 16
#define MD_LFO1SPEED 17
#define MD_LFO2SPEED 18
#define MD_PITCH 19
#define MD_MOD6AMOUNT 20
#define MD_AUDIOVOL 21
#define MD_AUDIOPAN 22

#define PMD 1200.0f

#define DO_MODSLOT(x) \
{ \
	if(this->prg->mod[x].source)\
	{ \
		switch(this->prg->mod[x].dest) \
		{ \
		case MD_FCO: \
			co += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			break; \
		case MD_FQ: \
			q += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			break; \
		case MD_FGAIN: \
			fgain += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			fgain = nCLAMP(0.0f, fgain, 4.0f); \
			break; \
		case MD_VOLUME: \
			mv += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 4.0f; \
			mv = nCLAMP(0.0f, mv, 4.0f); \
			break; \
		case MD_PAN: \
			pan += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 4096.0f); \
			pan = nCLAMP(0, pan, 4095); \
			break; \
		case MD_VOL1: \
			v0 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			v0 = nCLAMP(0.0f, v0, 2.0f); \
			break; \
		case MD_VOL2: \
			v1 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			v1 = nCLAMP(0.0f, v1, 2.0f); \
			break; \
		case MD_VOL3: \
			v2 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			v2 = nCLAMP(0.0f, v2, 2.0f); \
			break; \
		case MD_PITCH1: \
			p0 += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_PITCH2: \
			p1 += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_PITCH3: \
			p2 += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_MORPH1: \
			pw0 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 2048.0f); \
			break; \
		case MD_MORPH2: \
			pw1 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 2048.0f); \
			break; \
		case MD_MORPH3: \
			pw2 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 2048.0f); \
			break; \
		case MD_SPREAD1: \
			spr0 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 32.0f); \
			this->osc[0]->SetSpread(spr0); \
			break; \
		case MD_SPREAD2: \
			spr1 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 32.0f); \
			this->osc[1]->SetSpread(spr1); \
			break; \
		case MD_SPREAD3: \
			spr2 += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 32.0f); \
			this->osc[2]->SetSpread(spr2); \
			break; \
		case MD_LFO1SPEED: \
			l0speed += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 4096.0f); \
			break; \
		case MD_LFO2SPEED: \
			l1speed += truncate(mod[this->prg->mod[x].source] * g_m2f1[this->prg->mod[x].amount] * 4096.0f); \
			break; \
		case MD_PITCH: \
			a += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * PMD); \
			break; \
		case MD_MOD6AMOUNT: \
			mod6 += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount]; \
			break; \
		case MD_AUDIOVOL: \
			avol += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 2.0f; \
			avol = nCLAMP(0.0f, avol, 2.0f); \
			break; \
		case MD_AUDIOPAN: \
			apan += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * 4096.0f); \
			apan = nCLAMP(0, apan, 4095); \
			break; \
		} \
	} \
}

#define DO_MODSLOT6(x) \
{ \
	if(this->prg->mod[x].source)\
	{ \
		switch(this->prg->mod[x].dest) \
		{ \
		case MD_FCO: \
			co += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			break; \
		case MD_FQ: \
			q += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			break; \
		case MD_FGAIN: \
			fgain += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6); \
			fgain = nCLAMP(0.0f, fgain, 4.0f); \
			break; \
		case MD_VOLUME: \
			mv += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 4.0f; \
			mv = nCLAMP(0.0f, mv, 4.0f); \
			break; \
		case MD_PAN: \
			pan += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 4096.0f); \
			pan = nCLAMP(0, pan, 4095); \
			break; \
		case MD_VOL1: \
			v0 += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 2.0f; \
			v0 = nCLAMP(0.0f, v0, 2.0f); \
			break; \
		case MD_VOL2: \
			v1 += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 2.0f; \
			v1 = nCLAMP(0.0f, v1, 2.0f); \
			break; \
		case MD_VOL3: \
			v2 += mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * 2.0f; \
			v2 = nCLAMP(0.0f, v2, 2.0f); \
			break; \
		case MD_PITCH1: \
			p0 += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_PITCH2: \
			p1 += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_PITCH3: \
			p2 += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_MORPH1: \
			pw0 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 2048.0f); \
			break; \
		case MD_MORPH2: \
			pw1 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 2048.0f); \
			break; \
		case MD_MORPH3: \
			pw2 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 2048.0f); \
			break; \
		case MD_SPREAD1: \
			spr0 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 32.0f); \
			this->osc[0]->SetSpread(spr0); \
			break; \
		case MD_SPREAD2: \
			spr1 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 32.0f); \
			this->osc[1]->SetSpread(spr1); \
			break; \
		case MD_SPREAD3: \
			spr2 += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 32.0f); \
			this->osc[2]->SetSpread(spr2); \
			break; \
		case MD_LFO1SPEED: \
			l0speed += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 4096.0f); \
			break; \
		case MD_LFO2SPEED: \
			l1speed += truncate(mod[this->prg->mod[x].source] * (g_m2f1[this->prg->mod[x].amount] * mod6) * 4096.0f); \
			break; \
		case MD_PITCH: \
			a += truncate(mod[this->prg->mod[x].source] * (g_m2mod[this->prg->mod[x].amount] * mod6) * PMD); \
			break; \
		case MD_AUDIOVOL: \
			avol += mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * mod6 * 2.0f; \
			avol = nCLAMP(0.0f, avol, 2.0f); \
			break; \
		case MD_AUDIOPAN: \
			apan += truncate(mod[this->prg->mod[x].source] * g_m2mod[this->prg->mod[x].amount] * mod6 * 4096.0f); \
			apan = nCLAMP(0, apan, 4095); \
			break; \
		} \
	} \
}

//********************************************************************************
//***** MOD SLOT DEFINE ENDS                                                 *****
//********************************************************************************

void SVoice::Render(float *left, float *right, float *inl, float* inr, int frames)
{
	int i;
	int opw0, opw1, opw2;
	int opan, oapan;
	int ol0speed, ol1speed;
	float mod[14];
	float oco, oq;
	float ov0, ov1, ov2, ovm, fenv, ofgain, oavol;
	bool kt0, kt1, kt2;
	float keytrack;

	if(this->oscKT1)
		this->pitch0 = this->npitch + g_semi[this->prg->osc[0].semi] + g_fine[this->prg->osc[0].fine];
	else
		this->pitch0 = 6400 + g_semi[this->prg->osc[0].semi] + g_fine[this->prg->osc[0].fine];
	
	if(this->oscKT2)
		this->pitch1 = this->npitch + g_semi[this->prg->osc[1].semi] + g_fine[this->prg->osc[1].fine];
	else
		this->pitch1 = 6400 + g_semi[this->prg->osc[1].semi] + g_fine[this->prg->osc[1].fine];
	
	if(this->oscKT3)
		this->pitch2 = this->npitch + g_semi[this->prg->osc[2].semi] + g_fine[this->prg->osc[2].fine];
	else
		this->pitch2 = 6400 + g_semi[this->prg->osc[2].semi] + g_fine[this->prg->osc[2].fine];

	opw0 = this->prg->osc[0].morph * 16;
	opw1 = this->prg->osc[1].morph * 16;
	opw2 = this->prg->osc[2].morph * 16;

	opan = this->prg->mixer.panning * 32;
	oapan = this->prg->audioPan * 32;

	ov0 = g_m2vol[this->prg->mixer.osc1];
	ov1 = g_m2vol[this->prg->mixer.osc2];
	ov2 = g_m2vol[this->prg->mixer.osc3];
	oavol = g_m2vol[this->prg->audioVol];
	i = this->prg->mixer.volume + this->mvolVel;
	if(i < 0) i = 0; else if(i > 127) i = 127;
	ovm = g_m2vol[i] * 2.0f;

	kt0 = (this->prg->osc[0].flags & 0x80) ? true : false;
	kt1 = (this->prg->osc[1].flags & 0x80) ? true : false;
	kt2 = (this->prg->osc[2].flags & 0x80) ? true : false;

	oco = g_m2co[this->prg->filter.cutoff] + this->coVel;
	oq = g_m2f0[this->prg->filter.q];
	fenv = g_m2fenv[this->prg->filter.env2];

	keytrack = g_m2f1[this->currentNote];
	ol0speed = this->prg->lfo[0].speed * 32 + truncate(keytrack * g_m2mod[this->prg->lfo[0].keytrack] * 4096.0f);
	ol1speed = this->prg->lfo[1].speed * 32 + truncate(keytrack * g_m2mod[this->prg->lfo[1].keytrack] * 4096.0f);
	ofgain = g_m2fgain[this->prg->filter.gain];

	mod[1] = this->velocity;
	mod[9] = this->modwheel;
	mod[11] = this->aftertouch;

	for(i = 0; i < frames; i++)
	{
		float osc0, osc1, osc2;
		float v0 = ov0, v1 = ov1, v2 = ov2, mv = ovm, avol = oavol;
		float fgain = ofgain;
		float e0, e1, e2;
		float co = 0.0f, q = 0.0f, mix;
		float l0, l1;
		float mod6 = 0.0f;
		int l0speed = ol0speed;
		int l1speed = ol1speed;
		int spr0 = this->prg->osc[0].spread;
		int spr1 = this->prg->osc[1].spread;
		int spr2 = this->prg->osc[2].spread;
		int pan = opan, apan = oapan;
		int p0, p1, p2;
		int pw0, pw1, pw2;
		int a = 0, arp;

		e0 = this->env[0]->Process();
		e1 = this->env[1]->Process();
		e2 = this->env[2]->Process();
		l0 = this->lfo[0]->Process();
		l1 = this->lfo[1]->Process();
		arp = this->arp->Process();
		
		if(this->doGlide)
		{
			if(this->gStep.int32 < 0)
			{
				this->gIndex.int32 += this->gStep.int32;
				if(this->gIndex.fixed.index < this->gDest)
				{
					this->gIndex.fixed.index = this->gDest;
					this->gStep.int32 = 0;
				}
			}
			else if(this->gStep.int32 > 0)
			{
				this->gIndex.int32 += this->gStep.int32;
				this->gIndex.int32 += this->gStep.int32;
				if(this->gIndex.fixed.index > this->gDest)
				{
					this->gStep.int32 = 0;
					this->gIndex.fixed.index = this->gDest;
				}
			}
			a += this->gIndex.fixed.index;
			keytrack = g_m2f1[this->currentNote + this->gIndex.fixed.index / 100];
		}

		p0 = this->pitch0 + arp; // if(kt0) p0 += arp;
		p1 = this->pitch1 + arp; // if(kt1) p1 += arp;
		p2 = this->pitch2 + arp; // if(kt2) p2 += arp;

		pw0 = opw0;
		pw1 = opw1;
		pw2 = opw2;

		mod[2] = keytrack;
		mod[3] = l0;
		mod[4] = l1;
		mod[5] = e1;
		mod[6] = e2;
		mod[7] = l0 * e1;
		mod[8] = l0 * l1;
		mod[10] = this->modwheel * l1;
		mod[12] = inl[i];
		mod[13] = inr[i];

		DO_MODSLOT(0);
		DO_MODSLOT(1);
		DO_MODSLOT(2);
		DO_MODSLOT(3);
		DO_MODSLOT(4);
		DO_MODSLOT6(5);

		p0 += a;
		p1 += a;
		p2 += a;

		// Filter
		if(this->filtExp) 
			co += fenv * e2 * e2;
		else
			co += fenv * e2;

		this->iSet(iCUTOFF, co + oco);
		this->iDo(iCUTOFF);
		this->iSet(iQ, q + oq);
		this->iDo(iQ);
		this->filter->Set(this->iGet(iCUTOFF), this->iGet(iQ));

		// LFO
		this->lfo[0]->SetSpeed(l0speed);
		this->lfo[1]->SetSpeed(l1speed);

		this->iSet(iPW1, (float)pw0);
		this->iDo(iPW1);
		this->iSet(iPW2, (float)pw1);
		this->iDo(iPW2);
		this->iSet(iPW3, (float)pw2);
		this->iDo(iPW3);
		// OSC
		this->osc[0]->SetPw(truncate(this->iGet(iPW1)));
		this->osc[1]->SetPw(truncate(this->iGet(iPW2)));
		this->osc[2]->SetPw(truncate(this->iGet(iPW3)));

		this->osc[0]->SetPitch(p0);
		this->osc[1]->SetPitch(p1);
		this->osc[2]->SetPitch(p2);
		this->osc[0]->SetTables();
		this->osc[1]->SetTables();
		this->osc[2]->SetTables();

		osc0 = this->osc[0]->CallProcess();
		osc1 = this->osc[1]->CallProcess();
		osc2 = this->osc[2]->CallProcess();

		// Main Interpolation

		this->iSet(iOSC1VOL, v0);
		this->iDo(iOSC1VOL);
		v0 = this->iGet(iOSC1VOL);

		this->iSet(iOSC2VOL, v1);
		this->iDo(iOSC2VOL);
		v1 = this->iGet(iOSC2VOL);

		this->iSet(iOSC3VOL, v2);
		this->iDo(iOSC3VOL);
		v2 = this->iGet(iOSC3VOL);

		this->iSet(iAUDIOPAN, (float)apan);
		this->iDo(iAUDIOPAN);
		this->iSet(iAUDIOVOL, (float)avol);
		this->iDo(iAUDIOVOL);
		apan = truncate(this->iGet(iAUDIOPAN));
		mix = ((this->oscAM1) ? (osc0 * osc1 * v0) : (osc0 * v0))
			+ ((this->oscAM2) ? (osc1 * osc2 * v1) : (osc1 * v1))
			+ ((this->oscAM3) ? (osc2 * osc0 * v2) : (osc2 * v2))
			+ (inl[i] * g_apan[apan].left + inr[i] * g_apan[apan].right) * this->iGet(iAUDIOVOL);
		
		this->iSet(iFGAIN, fgain);
		this->iDo(iFGAIN);
		this->filter->SetGain(this->iGet(iFGAIN));

		this->iSet(iMAINVOL, mv);
		this->iDo(iMAINVOL);
		mix = this->filter->CallProcess(mix) * e0 * this->iGet(iMAINVOL);

		this->iSet(iPAN, (float)pan);
		this->iDo(iPAN);
		pan = truncate(this->iGet(iPAN));

		left[i] += mix * g_pan[pan].left;
		right[i] += mix * g_pan[pan].right;
	}
}

void SVoice::SetSampleRate(float fs)
{
	this->FS = fs;

	this->filter->SetSampleRate(fs);

	this->env[0]->SetSampleRate(fs);
	this->env[1]->SetSampleRate(fs);
	this->env[2]->SetSampleRate(fs);

	this->lfo[0]->SetSampleRate(fs);
	this->lfo[1]->SetSampleRate(fs);

	this->arp->SetSampleRate(fs);

	int i;
	for(i = 0; i < 128; i++)
	{
		float f = (float)i / 128.0f;
		this->glides[i] = truncate((powf(2.0f, f * 12.0f) * 655360.0f) / fs);
	}

	this->iSamples = fs * iSPEED;
}

void SVoice::SetTempo(float tempo)
{
	this->arp->SetTempo(tempo);
	this->lfo[0]->SetTempo(tempo);
	this->lfo[1]->SetTempo(tempo);
}

void SVoice::Reset()
{
	this->osc[0]->Reset();
	this->osc[1]->Reset();
	this->osc[2]->Reset();

	this->filter->Reset();

	this->env[0]->Reset();
	this->env[1]->Reset();
	this->env[2]->Reset();

	this->lfo[0]->Reset();
	this->lfo[1]->Reset();

	this->iReset(iOSC1VOL);
	this->iReset(iOSC2VOL);
	this->iReset(iOSC3VOL);
	this->iReset(iMAINVOL);
	this->iReset(iPAN);
	this->iReset(iFGAIN);
	this->iReset(iCUTOFF);
	this->iReset(iQ);
	this->iReset(iAUDIOVOL);
	this->iReset(iAUDIOPAN);
	this->iReset(iPW1);
	this->iReset(iPW2);
	this->iReset(iPW3);
}

void SVoice::NoteOn(int note, int vel, int last)
{
	float fvel = this->velocity = g_m2f0[vel];
	this->time = (unsigned __int32) GetTickCount();

	if(note > NOTE_MAX1)
		note = NOTE_MAX1;

	if(this->prg->glide && note != last && last != -1)
	{
		this->currentNote = last;
		this->npitch = last * 100;
		this->gDest = note * 100 - this->npitch;
		this->gIndex.int32 = 0;
		this->gStep.int32 = (note < last) ? -this->glides[this->prg->glide] : this->glides[this->prg->glide];
		this->doGlide = true;
	}
	else
	{
		this->currentNote = note;
		this->npitch = note * 100;
		this->doGlide = false;
	}
	this->coVel = fvel * g_m2f1[this->prg->filter.velsens];
	this->mvolVel = truncate(fvel * g_m2f1[this->prg->mixer.velsens] * 128.0f);

	if(this->env[0]->state == sOff)
	{
		this->iReset(iOSC1VOL);
		this->iReset(iOSC2VOL);
		this->iReset(iOSC3VOL);
		this->iReset(iMAINVOL);
		this->iReset(iPAN);
		this->iReset(iFGAIN);
		this->iReset(iCUTOFF);
		this->iReset(iQ);
		this->iReset(iAUDIOVOL);
		this->iReset(iAUDIOPAN);
		this->iReset(iPW1);
		this->iReset(iPW2);
		this->iReset(iPW3);
		this->filter->Reset();
		this->osc[0]->SetPw(this->prg->osc[0].morph * 16);
		this->osc[1]->SetPw(this->prg->osc[1].morph * 16);
		this->osc[2]->SetPw(this->prg->osc[2].morph * 16);
	}

	this->env[1]->Reset();
	this->env[2]->Reset();

	this->env[0]->GateOn();
	this->env[1]->GateOn();
	this->env[2]->GateOn();

	this->arp->Retrigger();
	this->lfo[0]->Retrigger();
	this->lfo[1]->Retrigger();

}

void SVoice::NoteOff()
{
	this->env[0]->GateOff();
	this->env[1]->GateOff();
	this->env[2]->GateOff();
}

bool SVoice::IsDone()
{
	return (this->env[0]->state == sOff) ? true : false;
}

bool SVoice::IsRelease()
{
	return (this->env[0]->state == sRelease) ? true : false;
}

unsigned __int32 SVoice::GetTime()
{
	return this->time;
}

void SVoice::UpdateValue(int id, int val)
{
	switch(id)
	{
	case pModwheel:		this->modwheel = g_m2f0[val]; break;
	case pAftertouch:	this->aftertouch = g_m2f0[val]; break;
	//********************************************************************************
	//***** OSC                                                                  *****
	//********************************************************************************
	case pOWave1:	this->osc[0]->SetWave(val);	break;
	case pOWave2:	this->osc[1]->SetWave(val);	break;
	case pOWave3:	this->osc[2]->SetWave(val); break;
	case pOMode1:	this->osc[0]->SetMode(val); break;
	case pOMode2:	this->osc[1]->SetMode(val); break;
	case pOMode3:	this->osc[2]->SetMode(val); break;
	case pOSpread1:	this->osc[0]->SetSpread(val); break;
	case pOSpread2:	this->osc[1]->SetSpread(val); break;
	case pOSpread3:	this->osc[2]->SetSpread(val); break;
	case pOFlags1:	
		this->osc[1]->SetSync(val & 0x01);
		this->oscAM1 = (val & 0x02) ? true : false;
		this->oscKT1 = (val & 0x80) ? true : false;
		this->osc[0]->AlertAM(this->oscAM1);
		break;
	case pOFlags2:	
		this->osc[2]->SetSync(val & 0x01);
		this->oscAM2 = (val & 0x02) ? true : false;
		this->oscKT2 = (val & 0x80) ? true : false;
		this->osc[1]->AlertAM(this->oscAM2);
		break;
	case pOFlags3:	
		this->osc[0]->SetSync(val & 0x01);
		this->oscAM3 = (val & 0x02) ? true : false;
		this->oscKT3 = (val & 0x80) ? true : false;
		this->osc[2]->AlertAM(this->oscAM3);
		break;

	//********************************************************************************
	//***** ENVELOPES                                                            *****
	//********************************************************************************
	case pAttack1:	this->env[0]->Attack(val); break;
	case pHold1:	this->env[0]->Hold(val); break;
	case pDecay1:	this->env[0]->Decay(val); break;
	case pSustain1:	this->env[0]->Sustain(val); break;
	case pRelease1:	this->env[0]->Release(val); break;

	case pAttack2:	this->env[1]->Attack(val); break;
	case pHold2:	this->env[1]->Hold(val); break;
	case pDecay2:	this->env[1]->Decay(val); break;
	case pSustain2:	this->env[1]->Sustain(val); break;
	case pRelease2:	this->env[1]->Release(val); break;

	case pAttack3:	this->env[2]->Attack(val); break;
	case pHold3:	this->env[2]->Hold(val); break;
	case pDecay3:	this->env[2]->Decay(val); break;
	case pSustain3:	this->env[2]->Sustain(val); break;
	case pRelease3:	this->env[2]->Release(val); break;

	//********************************************************************************
	//***** FILTER                                                               *****
	//********************************************************************************
	case pFMode: this->filter->SetMode(val); break;
	case pFType: this->filter->SetType(val); break;
	case pFExp: this->filtExp = (val) ? true : false; break;
	case pFGain: this->filter->SetGain(g_m2fgain[val]); break;

	//********************************************************************************
	//***** ARPEGGIATOR                                                          *****
	//********************************************************************************
	case pArpSpeed: this->arp->SetSpeed(val); break;
	case pArpLength: this->arp->SetLength(val); break;
	case pArpNote1: this->arp->SetNote(0, val - 64); break;
	case pArpNote2: this->arp->SetNote(1, val - 64); break;
	case pArpNote3: this->arp->SetNote(2, val - 64); break;
	case pArpNote4: this->arp->SetNote(3, val - 64); break;
	case pArpNote5: this->arp->SetNote(4, val - 64); break;
	case pArpNote6: this->arp->SetNote(5, val - 64); break;
	case pArpNote7: this->arp->SetNote(6, val - 64); break;
	case pArpNote8: this->arp->SetNote(7, val - 64); break;
	case pArpSync: this->arp->SetSync(val); break;
	case pArpLoop: this->arp->SetLoop(val); break;
	//********************************************************************************
	//***** LFO                                                                  *****
	//********************************************************************************
	case pL1Speed: this->lfo[0]->SetSpeed(val * 32); break;
	case pL1Delay: this->lfo[0]->SetDelay(val); break;
	case pL1PW: this->lfo[0]->SetPW(val); break;
	case pL1Sync: this->lfo[0]->SetSync(val); break;
	case pL1SNH: this->lfo[0]->SetSNH(val); break;
	case pL1Retrig: this->lfo[0]->SetRetrigger(val); break;
	case pL1Wave: this->lfo[0]->SetWave(val); break;

	case pL2Speed: this->lfo[1]->SetSpeed(val * 32); break;
	case pL2Delay: this->lfo[1]->SetDelay(val); break;
	case pL2PW: this->lfo[1]->SetPW(val); break;
	case pL2Sync: this->lfo[1]->SetSync(val); break;
	case pL2SNH: this->lfo[1]->SetSNH(val); break;
	case pL2Retrig: this->lfo[1]->SetRetrigger(val); break;
	case pL2Wave: this->lfo[1]->SetWave(val); break;
	}
}
*/