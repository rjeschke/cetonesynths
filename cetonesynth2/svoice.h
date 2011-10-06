#pragma once

#include <math.h>
#include "osc.h"
#include "lfo.h"
#include "filter.h"
#include "envelope.h"
#include "arp.h"

#define iSPEED 0.004f

#define iOSC1VOL 0
#define iOSC2VOL 1
#define iOSC3VOL 2
#define iMAINVOL 3
#define iFGAIN 4
#define iPAN 5
#define iCUTOFF 6
#define iQ 7
#define iAUDIOVOL 8
#define iAUDIOPAN 9
#define iPW1 10
#define iPW2 11
#define iPW3 12

typedef struct
{
	float current, step, dest;
	__int32 running;
} vINTERPOLATE;

class SVoice
{
public:
	SVoice(sPRG* p);
	~SVoice();
	void Reset();

	void Render(float* left, float* right, float* inl, float* inr, int frames);

	void NoteOn(int note, int vel, int last);
	void NoteOff();

	void UpdateValue(int id, int val);

	void SetTempo(float tempo);
	void SetSampleRate(float fs);

	bool IsDone();
	bool IsRelease();
	unsigned __int32 GetTime();

private:

	__forceinline void iReset(int idx)
	{
		this->inter[idx].current =
			this->inter[idx].step =
			this->inter[idx].dest = 0.0f;
		this->inter[idx].running = 0;
	}

	__forceinline void iSet(int idx, float val)
	{
		if(val != this->inter[idx].dest)
		{
			float delta = val - this->inter[idx].current;
			if(delta != 0)
			{
				this->inter[idx].dest = val;
				this->inter[idx].step = delta / this->iSamples;
				this->inter[idx].running = -1;
			}
		}
	}

	__forceinline float iGet(int idx)
	{
		return this->inter[idx].current;
	}

	__forceinline void iDo(int idx)
	{
		if(this->inter[idx].running)
		{
			this->inter[idx].current += this->inter[idx].step;
			if(this->inter[idx].step < 0.0f)
			{
				if(this->inter[idx].current < this->inter[idx].dest)
				{
					this->inter[idx].current = this->inter[idx].dest;
					this->inter[idx].running = 0;
				}
			}
			else
			{
				if(this->inter[idx].current > this->inter[idx].dest)
				{
					this->inter[idx].current = this->inter[idx].dest;
					this->inter[idx].running = 0;
				}
			}
		}
	}

	vINTERPOLATE inter[13];
	sPRG*		prg;
	OSC*		osc[3];
	Filter*		filter;
	Envelope*	env[3];
	LFO*		lfo[2];
	ARP*		arp;
	int			glides[128];
	unsigned __int32 time;
	float		velocity;
	float		coVel;
	float		modwheel;
	float		aftertouch;
	int			mvolVel;
	int			npitch;
	int			currentNote;
	oSTEP		gStep;
	oSTEP		gIndex;
	__int16		gDest, gPad;
	int			pitch0, pitch1, pitch2;
	bool		oscAM1, oscAM2, oscAM3;
	bool		oscKT1, oscKT2, oscKT3;
	bool        filtExp, doGlide;
	float iSamples;
	float FS;
};
