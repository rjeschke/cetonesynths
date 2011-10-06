#pragma once

#include <math.h>
#include "statics.h"

static float o_addvol[16] =
	{ 
		0.00f, 1.00f, 1.00f, 0.50f,
		1.00f, 0.50f, 0.50f, 0.33f,
		1.00f, 0.50f, 0.50f, 0.33f,
		0.50f, 0.33f, 0.33f, 0.25f,
	};

class OSC
{
public:
	OSC();
	~OSC() { };

	void	SetWave(int wave);
	void	SetMode(int mode);
	void	SetSync(int sync);
	void	SetSpread(int val);

	__forceinline void SetPitch(int pitch)
	{
		int p;
		if(pitch < 0)
			pitch = 0;
		else if(pitch > PITCH_MAX1)
			pitch = PITCH_MAX1;

		if(pitch == this->Pitch)
			return;

		this->Pitch = pitch;

		this->SStep[2].int32 = g_step[pitch];

		p = pitch - this->Spread[1]; if(p < 0) p = 0;
		this->SStep[0].int32 = g_step[p];

		p = pitch - this->Spread[0]; if(p < 0) p = 0;
		this->SStep[1].int32 = g_step[p];

		p = pitch + this->Spread[0]; if(p > PITCH_MAX1) p = PITCH_MAX1;
		this->SStep[3].int32 = g_step[p];

		p = pitch + this->Spread[1]; if(p > PITCH_MAX1) p = PITCH_MAX1;
		this->SStep[4].int32 = g_step[p];
		this->SuperPitch = p;
	}

	__forceinline void SetTables()
	{
		int f;
		if(this->IsSync || this->IsAM)
			f = g_lookup[truncate( (g_freqtab[this->Master->Pitch] + g_freqtab[this->Pitch]) * 2.0f)];
		else if(this->Modus == OSCM_SUPER)
			f = g_lookup[truncate(g_freqtab[this->SuperPitch] * 2.0f)];
		else
			f = g_lookup[truncate(g_freqtab[this->Pitch] * 2.0f)];
		this->SawPtr = g_saw + f;
		this->ParabolaPtr = g_parabola + f;
	};

	__forceinline void SetPw(int pw)
	{
		if(pw < 0)
			pw = 0;
		else if(pw > 2047)
			pw = 2047;

		this->PwOffset = g_osc_pw[pw];
		this->PwWaveOffset = g_osc_pwo[pw];
		this->TriScalar = g_osc_tris[pw];
	}

	__forceinline void Sync()
	{
		this->SIndex[0].int32 =
		this->SIndex[1].int32 =
		this->SIndex[2].int32 =
		this->SIndex[3].int32 = 
		this->SIndex[4].int32 = 0;
	}

	void	AlertSlave(bool sync);
	void	AlertAM(bool am)
	{
		this->IsAM = am;
		this->SetTables();
	};

	void	SetSlave(OSC* osc) { this->Slave = osc; };
	void	SetMaster(OSC* osc) { this->Master = osc; };

	void    Reset();	

	__forceinline	float CallProcess() 
	{ 
		if(!this->Waveform)
			return 0.0f;

		float output = 0.0f;

		switch(this->Modus)
		{
		case OSCM_NORMAL:
			{
				float frac = (float)this->SIndex[2].fixed.frac / 65536.0f;
				unsigned __int16 s0 = this->SIndex[2].fixed.index;
				unsigned __int16 s1 = s0 + 1;
				
				float o0, o1;
				
				switch(this->Waveform)
				{
				case OSCW_SAW: 
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output = o0 + frac * (o1 - o0);
					break;
				case OSCW_PULSE:
					o0 = this->SawPtr[s0] - this->SawPtr[s0 + this->PwOffset];
					o1 = this->SawPtr[s1] - this->SawPtr[s1 + this->PwOffset];
					output = (o0 + frac * (o1 - o0)) + this->PwWaveOffset;
					break;
				case OSCW_TRI:
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0 + this->PwOffset];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1 + this->PwOffset];
					output = (o0 + frac * (o1 - o0)) * this->TriScalar;
					break;
				case OSCW_NOISE:
					output = nrand();
					break;
				}
				this->SIndex[2].int32 += this->SStep[2].int32;
				if(this->SIndex[2].fixed.index > WAVETABLE_LENGTH1)
				{
					if(this->DoSync)
						this->Slave->Sync();
					this->SIndex[2].fixed.index &= WAVETABLE_MASK;
				}
				return output;
			}
			break;
		case OSCM_ADD:
			{
				float frac = (float)this->SIndex[2].fixed.frac / 65536.0f;

				unsigned __int16 s0 = this->SIndex[2].fixed.index;
				unsigned __int16 s1 = s0 + 1;

				unsigned __int16 s0a = s0 + this->PwOffset;
				unsigned __int16 s1a = s0a + 1;
				
				float o0, o1;

				if(this->Waveform & OSCW_SAW)
				{
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output += o0 + frac * (o1 - o0);
				}
				if(this->Waveform & OSCW_PULSE)
				{
					o0 = this->SawPtr[s0] - this->SawPtr[s0a];
					o1 = this->SawPtr[s1] - this->SawPtr[s1a];
					output += (o0 + frac * (o1 - o0)) + this->PwWaveOffset;
				}
				if(this->Waveform & OSCW_TRI)
				{
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0a];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1a];
					output += (o0 + frac * (o1 - o0)) * this->TriScalar;
				}
				if(this->Waveform & OSCW_NOISE)
				{
					output += nrand();
				}

				this->SIndex[2].int32 += this->SStep[2].int32;
				if(this->SIndex[2].fixed.index > WAVETABLE_LENGTH1)
				{
					if(this->DoSync)
						this->Slave->Sync();
					this->SIndex[2].fixed.index &= WAVETABLE_MASK;
				}

				return output * o_addvol[this->Waveform];
			}
			break;
		case OSCM_MUL:
			{
				bool first = true;
				float frac = (float)this->SIndex[2].fixed.frac / 65536.0f;

				unsigned __int16 s0 = this->SIndex[2].fixed.index;
				unsigned __int16 s1 = s0 + 1;

				unsigned __int16 s0a = s0 + this->PwOffset;
				unsigned __int16 s1a = s0a + 1;
				
				float o0, o1;

				if(this->Waveform & OSCW_SAW)
				{
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output = o0 + frac * (o1 - o0);
					first = false;
				}
				if(this->Waveform & OSCW_PULSE)
				{
					o0 = this->SawPtr[s0] - this->SawPtr[s0a];
					o1 = this->SawPtr[s1] - this->SawPtr[s1a];
					if(first)
					{
						output = (o0 + frac * (o1 - o0)) + this->PwWaveOffset;
						first = false;
					}
					else
						output *= (o0 + frac * (o1 - o0)) + this->PwWaveOffset;
				}
				if(this->Waveform & OSCW_TRI)
				{
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0a];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1a];
					if(first)
					{
						output = (o0 + frac * (o1 - o0)) * this->TriScalar;
						first = false;
					}
					else
						output *= (o0 + frac * (o1 - o0)) * this->TriScalar;
				}
				if(this->Waveform & OSCW_NOISE)
				{
					if(first)
						output = nrand();
					else
						output += nrand();
				}

				this->SIndex[2].int32 += this->SStep[2].int32;
				if(this->SIndex[2].fixed.index > WAVETABLE_LENGTH1)
				{
					if(this->DoSync)
						this->Slave->Sync();
					this->SIndex[2].fixed.index &= WAVETABLE_MASK;
				}

				return output;
			}
			break;
		case OSCM_SUPER:
			{
				float o0, o1, frac;
				unsigned __int16 s0, s1;
				
				switch(this->Waveform)
				{
				case OSCW_SAW:
					frac = (float)this->SIndex[0].fixed.frac / 65536.0f;
					s0 = this->SIndex[0].fixed.index;
					s1 = (s0 + 1) & WAVETABLE_MASK;
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output += o0 + frac * (o1 - o0);

					frac = (float)this->SIndex[1].fixed.frac / 65536.0f;
					s0 = this->SIndex[1].fixed.index;
					s1 = (s0 + 1) & WAVETABLE_MASK;
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output += o0 + frac * (o1 - o0);

					frac = (float)this->SIndex[2].fixed.frac / 65536.0f;
					s0 = this->SIndex[2].fixed.index;
					s1 = (s0 + 1) & WAVETABLE_MASK;
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output += o0 + frac * (o1 - o0);

					frac = (float)this->SIndex[3].fixed.frac / 65536.0f;
					s0 = this->SIndex[3].fixed.index;
					s1 = (s0 + 1) & WAVETABLE_MASK;
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output += o0 + frac * (o1 - o0);

					frac = (float)this->SIndex[4].fixed.frac / 65536.0f;
					s0 = this->SIndex[4].fixed.index;
					s1 = (s0 + 1) & WAVETABLE_MASK;
					o0 = this->SawPtr[s0];
					o1 = this->SawPtr[s1];
					output += o0 + frac * (o1 - o0);
					break;
				case OSCW_PULSE:
					frac = (float)this->SIndex[0].fixed.frac / 65536.0f;
					s0 = this->SIndex[0].fixed.index;
					s1 = s0 + 1;
					o0 = this->SawPtr[s0] - this->SawPtr[s0 + this->PwOffset];
					o1 = this->SawPtr[s1] - this->SawPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) + this->PwWaveOffset;

					frac = (float)this->SIndex[1].fixed.frac / 65536.0f;
					s0 = this->SIndex[1].fixed.index;
					s1 = s0 + 1;
					o0 = this->SawPtr[s0] - this->SawPtr[s0 + this->PwOffset];
					o1 = this->SawPtr[s1] - this->SawPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) + this->PwWaveOffset;

					frac = (float)this->SIndex[2].fixed.frac / 65536.0f;
					s0 = this->SIndex[2].fixed.index;
					s1 = s0 + 1;
					o0 = this->SawPtr[s0] - this->SawPtr[s0 + this->PwOffset];
					o1 = this->SawPtr[s1] - this->SawPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) + this->PwWaveOffset;

					frac = (float)this->SIndex[3].fixed.frac / 65536.0f;
					s0 = this->SIndex[3].fixed.index;
					s1 = s0 + 1;
					o0 = this->SawPtr[s0] - this->SawPtr[s0 + this->PwOffset];
					o1 = this->SawPtr[s1] - this->SawPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) + this->PwWaveOffset;

					frac = (float)this->SIndex[4].fixed.frac / 65536.0f;
					s0 = this->SIndex[4].fixed.index;
					s1 = s0 + 1;
					o0 = this->SawPtr[s0] - this->SawPtr[s0 + this->PwOffset];
					o1 = this->SawPtr[s1] - this->SawPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) + this->PwWaveOffset;
					break;
				case OSCW_TRI:
					frac = (float)this->SIndex[0].fixed.frac / 65536.0f;
					s0 = this->SIndex[0].fixed.index;
					s1 = s0 + 1;
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0 + this->PwOffset];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) * this->TriScalar;

					frac = (float)this->SIndex[1].fixed.frac / 65536.0f;
					s0 = this->SIndex[1].fixed.index;
					s1 = s0 + 1;
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0 + this->PwOffset];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) * this->TriScalar;

					frac = (float)this->SIndex[2].fixed.frac / 65536.0f;
					s0 = this->SIndex[2].fixed.index;
					s1 = s0 + 1;
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0 + this->PwOffset];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) * this->TriScalar;

					frac = (float)this->SIndex[3].fixed.frac / 65536.0f;
					s0 = this->SIndex[3].fixed.index;
					s1 = s0 + 1;
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0 + this->PwOffset];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) * this->TriScalar;

					frac = (float)this->SIndex[4].fixed.frac / 65536.0f;
					s0 = this->SIndex[4].fixed.index;
					s1 = s0 + 1;
					o0 = this->ParabolaPtr[s0] - this->ParabolaPtr[s0 + this->PwOffset];
					o1 = this->ParabolaPtr[s1] - this->ParabolaPtr[s1 + this->PwOffset];
					output += (o0 + frac * (o1 - o0)) * this->TriScalar;
					break;
				case OSCW_NOISE:
					output = nrand();
					break;
				}

				this->SIndex[0].int32 = (this->SIndex[0].int32 + this->SStep[0].int32) & (WAVETABLE_MASK * 0x10000 + 0xffff);
				this->SIndex[1].int32 = (this->SIndex[1].int32 + this->SStep[1].int32) & (WAVETABLE_MASK * 0x10000 + 0xffff);
				this->SIndex[3].int32 = (this->SIndex[3].int32 + this->SStep[3].int32) & (WAVETABLE_MASK * 0x10000 + 0xffff);
				this->SIndex[4].int32 = (this->SIndex[4].int32 + this->SStep[4].int32) & (WAVETABLE_MASK * 0x10000 + 0xffff);

				this->SIndex[2].int32 += this->SStep[2].int32;
				if(this->SIndex[2].fixed.index > WAVETABLE_LENGTH1)
				{
					if(this->DoSync)
						this->Slave->Sync();
					this->SIndex[2].fixed.index &= WAVETABLE_MASK;
				}
				
				return output * 0.25f;
			}
			break;
		}

		return 0.0f;
	};

private:
	int		Waveform;
	int		Modus;
	int		Pitch;

	int		Spread[3];

	oSTEP	SIndex[5];
	oSTEP	SStep[5];

	float	PwFloat;
	float	PwWaveOffset;
	float	TriScalar;
	int		PwOffset;
	int		SuperPitch;

	float*	SawPtr;
	float*	ParabolaPtr;

	OSC*	Slave;
	OSC*	Master;

	bool	DoSync;
	bool	IsSync;
	bool	IsAM;
};
