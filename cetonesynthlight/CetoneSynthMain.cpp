#include <math.h>

#include "cetonesynth.h"

void CCetoneSynth::process(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->SynthProcess(inputs, outputs, sampleFrames, false);
}

void CCetoneSynth::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->SynthProcess(inputs, outputs, sampleFrames, true);
}

void CCetoneSynth::SynthProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace)
{
	float* outl = outputs[0];
	float* outr = outputs[1];
	
	int p0, p1, p2, delta, tdelta = 0;
	float l, r;
 
	int opitch[3];
	int opw[3];
	float output;
	float l_speed;
	float o_val[3];
	float v_vol[3], mEnv, mLfo, mMix;

	int tune[4], mtune;

	tune[0] = this->Voice[0].Coarse * 100 + this->Voice[0].Fine;
	tune[1] = this->Voice[1].Coarse * 100 + this->Voice[1].Fine;
	tune[2] = this->Voice[2].Coarse * 100 + this->Voice[2].Fine;

	while(sampleFrames > 0)
	{
		if(this->CurrentDelta == 0)
		{
			while(!this->MidiStack->IsEmpty() && (this->CurrentDelta == 0))
			{
				this->MidiStack->Pop(&p0, &p1, &p2, &delta);
				
				if(delta != 0)
				{
					this->NextP0 = p0;
					this->NextP1 = p1;
					this->NextP2 = p2;
					this->CurrentDelta = delta;
				}
				else
				{
					this->HandleMidi(p0, p1, p2);
				}
			}
		}

		/****************************************************************************

		Run starts

		****************************************************************************/
		
		if(this->CutoffStep != 0.f)
		{
			this->Cutoff += this->CutoffStep;

			if(this->CutoffStep < 0.f)
			{
				if(this->Cutoff <= this->CutoffDest)
				{
					this->Cutoff		= this->CutoffDest;
					this->CutoffStep	= 0.f;
				}
			}
			else
			{
				if(this->Cutoff >= this->CutoffDest)
				{
					this->Cutoff		= this->CutoffDest;
					this->CutoffStep	= 0.f;
				}
			}
		}

		int m_coarse = this->MainCoarse;
		int m_fine = this->MainFine, itmp;

		float m_vol = this->Volume;
		float m_cutoff = this->Cutoff;
		float m_q = this->Resonance;
		float m_mod = this->EnvMod;
		float m_pan = this->Panning;

		bool set_opw0 = false;
		bool set_opw1 = false;
		bool set_opw2 = false;
		bool set_lfo0 = false;

		if(this->FilterCounter <= 0)
			this->FilterCounter = FILTER_DELAY;
		else
			this->FilterCounter--;

		if(this->Ctrl1ModStep != 0.f)
		{
			this->Ctrl1Mod += this->Ctrl1ModStep;

			if(this->Ctrl1ModStep < 0.f)
			{
				if(this->Ctrl1Mod <= this->Ctrl1ModEnd)
				{
					this->Ctrl1Mod = this->Ctrl1ModEnd;
					this->Ctrl1ModStep = 0.f;
				}
			}
			else
			{
				if(this->Ctrl1Mod >= this->Ctrl1ModEnd)
				{
					this->Ctrl1Mod = this->Ctrl1ModEnd;
					this->Ctrl1ModStep = 0.f;
				}
			}
		}

		if(this->VelocityModStep != 0.f)
		{
			this->VelocityMod += this->VelocityModStep;

			if(this->VelocityModStep < 0.f)
			{
				if(this->VelocityMod <= this->VelocityModEnd)
				{
					this->VelocityMod = this->VelocityModEnd;
					this->VelocityModStep = 0.f;
				}
			}
			else
			{
				if(this->VelocityMod >= this->VelocityModEnd)
				{
					this->VelocityMod = this->VelocityModEnd;
					this->VelocityModStep = 0.f;
				}
			}
		}

		if(this->CurrentNote == -1)
		{
			l = r = 0.f;
		}
		else
		{
			opw[0]		= this->VoicePulsewidth[0];
			opw[1]		= this->VoicePulsewidth[1];
			opw[2]		= this->VoicePulsewidth[2];

			v_vol[0]	= this->Voice[0].Volume;
			v_vol[1]	= this->Voice[1].Volume;
			v_vol[2]	= this->Voice[2].Volume;

			l_speed		= this->LfoSpeed;

			if (this->DoPorta)
			{
				this->PortaFrac += this->PortaStep;
				itmp = this->PortaFrac >> 14;

				if(this->PortaStep < 0)
				{
					if(itmp <= this->PortaPitch)
					{
						itmp = this->PortaPitch;
						this->DoPorta = false;
					}
				}
				else
				{
					if(itmp >= this->PortaPitch)
					{
						itmp = this->PortaPitch;
						this->DoPorta = false;
					}
				}

				this->CurrentPitch = itmp;
			}

			opitch[0] = this->CurrentPitch + tune[0];
			opitch[1] = this->CurrentPitch + tune[1];
			opitch[2] = this->CurrentPitch + tune[2];

			mEnv = this->Envs[1]->Run();
			mLfo = this->Lfo->Run();
			mMix = mEnv * mLfo;

			for(int i = 0; i < 4; i++)
			{
				float am;
				SynthModulation* mod = &(this->Modulations[i]);

				if (mod->Source == MOD_SRC_NONE)
					continue;

				switch(mod->Source)
				{
				case MOD_SRC_VEL:			am = this->VelocityMod;		break;
				case MOD_SRC_CTRL1:			am = this->Ctrl1Mod;		break;
				case MOD_SRC_MENV1:			am = mEnv;				break;
				case MOD_SRC_LFO1:			am = mLfo;				break;
				case MOD_SRC_MENV1xLFO1:	am = mMix;				break;
				}

				am *= mod->Amount;
				am *= mod->Multiplicator;

				switch(mod->Destination)
				{
				default:
					break;
				case MOD_DEST_MAINVOL:
					m_vol += am * 0.0005f;
					if(m_vol < 0.f)	
						m_vol = 0.f;	
					else if(m_vol > 5.f)	
						m_vol = 5.f;
					break;
				case MOD_DEST_CUTOFF:
					m_cutoff += am * 0.0001f;
					if(m_cutoff < 0.f) 
						m_cutoff = 0.f;	
					else if(m_cutoff > 1.f)	
						m_cutoff = 1.f;
					break;
				case MOD_DEST_RESONANCE:
					m_q += am * 0.0001f;
					if(m_q < 0.f)	
						m_q = 0.f;	
					else if(m_q > 1.f)	
						m_q = 1.f;
					break;
				case MOD_DEST_PANNING:
					m_pan += am * 0.0001f;
					if(m_pan < 0.f)	
						m_pan = 0.f; 
					else if(m_pan > 1.f)	
						m_pan = 1.f;
					break;
				case MOD_DEST_MAINPITCH:
					m_fine += truncate(am);
					break;
				case MOD_DEST_OSC1VOL:
					v_vol[0] += am * 0.0005f;
					if(v_vol[0] < 0.f)	
						v_vol[0] = 0.f;	
					else if(v_vol[0] > 5.f)	
						v_vol[0] = 5.f;
					break;
				case MOD_DEST_OSC2VOL:
					v_vol[1] += am * 0.0005f;
					if(v_vol[1] < 0.f)	
						v_vol[1] = 0.f;	
					else if(v_vol[1] > 5.f)	
						v_vol[1] = 5.f;
					break;
				case MOD_DEST_OSC3VOL:
					v_vol[2] += am * 0.0005f;
					if(v_vol[2] < 0.f)	
						v_vol[2] = 0.f;	
					else if(v_vol[2] > 5.f)	
						v_vol[2] = 5.f;
					break;
				case MOD_DEST_OSC1PITCH:
					opitch[0] += truncate(am);
					break;
				case MOD_DEST_OSC2PITCH:
					opitch[1] += truncate(am);
					break;
				case MOD_DEST_OSC3PITCH:
					opitch[2] += truncate(am);
					break;
				case MOD_DEST_OSC1PW:
					opw[0] += truncate(am * 6.5536f);
					set_opw0 = true;
					break;
				case MOD_DEST_OSC2PW:
					opw[1] += truncate(am * 6.5536f);
					set_opw1 = true;
					break;
				case MOD_DEST_OSC3PW:
					opw[2] += truncate(am * 6.5536f);
					set_opw2 = true;
					break;
				case MOD_DEST_LFO1SPEED:
					l_speed += am * 0.005f;
					set_lfo0 = true;
					break;
				case MOD_DEST_ENVMOD:
					m_mod += am * 0.0001f;
					if(m_mod < -1.f)	
						m_mod = -1.f; 
					else if(m_mod > 1.f)	
						m_mod = 1.f;
					break;
				}
			}

			if(set_opw0)
				this->Oscs[0]->SetPw(opw[0]);
			if(set_opw1)
				this->Oscs[1]->SetPw(opw[1]);
			if(set_opw2)
				this->Oscs[2]->SetPw(opw[2]);
			if(set_lfo0)
				this->Lfo->SetSpeed(l_speed);

			if(this->ArpMode != -1)
			{
				if(this->ArpPos >= this->C64Arps[this->ArpMode][15])
					this->ArpPos = 0;

				m_coarse += this->C64Arps[this->ArpMode][this->ArpPos];

				this->ArpCounter--;

				if(this->ArpCounter <= 0)
				{
					this->ArpCounter = this->ArpDelay;
					this->ArpPos++;
				}
			}

			mtune = m_coarse * 100 + m_fine;

			output = 0.f;

			float output_volume = this->Envs[0]->Run() * m_vol;

			this->UpdateFilters(m_cutoff, m_q, m_mod);
			
			for(int i = 0; i < 3; i++)
			{
				opitch[i]	+= mtune;
				this->Oscs[i]->SetPitch(opitch[i]);
				o_val[i]	= this->Oscs[i]->Run();
			}

			for(int i = 0; i < 3; i++)
			{
				float ftmp = o_val[i];

				if(this->Voice[i].Ring)
				{
					switch(i)
					{
						case 0:
							ftmp *= o_val[1];
							break;
						case 1:
							ftmp *= o_val[2];
							break;
						case 2:
							ftmp *= o_val[0];
							break;
					}
				}
				
				ftmp = ftmp * v_vol[i];

				if(ftmp > 1.f)
					ftmp = 1.f;
				else if(ftmp < -1.f)
					ftmp = -1.f;

				output += ftmp;
			}

			this->Oscs[1]->ProcessSync();
			this->Oscs[2]->ProcessSync();
			this->Oscs[0]->ProcessSync();

			output *= 0.333333f;

			switch(this->FilterType)
			{
			default:
				break;
			case FTYPE_DIRTY:
				output = this->FilterDirty->Run(output);
				break;
			case FTYPE_MOOG:
				output = this->FilterMoog->Run(output);
				break;
			case FTYPE_MOOG2:
				output = this->FilterMoog2->Run(output);
				break;
			case FTYPE_CH12DB:
				output = this->FilterCh12db->Run(output);
				break;
			case FTYPE_303:
				output = this->Filter303->Run(output);
				break;
			case FTYPE_8580:
				output = this->Filter8580->Run(output);
				break;
			case FTYPE_BUDDA:
				output = this->FilterBiquad->Run(output);
				break;
			}

			output *= output_volume;

			l = ((1.f - m_pan) * output);
			r = (m_pan * output);
		}

		/****************************************************************************

		Run ends

		****************************************************************************/

		if (replace)
		{
			(*outl++) = l;
			(*outr++) = r;
		}
		else
		{
			(*outl++) += l;
			(*outr++) += r;
		}
		
		sampleFrames--;
		tdelta++;

		if(this->CurrentDelta > 0)
		{
			if(tdelta >= this->CurrentDelta)
			{
				this->CurrentDelta = 0;
				this->HandleMidi(this->NextP0, this->NextP1, this->NextP2);
			}
		}
	}
}

VstInt32 CCetoneSynth::processEvents(VstEvents *events)
{
	for (VstInt32 i = 0; i < events->numEvents; i++)
	{
		if ((events->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* _event = (VstMidiEvent*)events->events[i];
		char* midiData = _event->midiData;
		
		this->MidiStack->Push(midiData[0], midiData[1] & 0x7f, midiData[2] & 0x7f, _event->deltaFrames);

		_event++;
	}
	return 1;
}

void CCetoneSynth::HandleMidi(int p0, int p1, int p2)
{
	bool btmp;
	int status = p0 & 0xf0;

	switch (status)
	{
	case 0x80:			// Note off
		if (p1 == this->CurrentNote)
			this->NoteOff(p1, p2);
		break;
	case 0x90:			// Note on
		if (p2 == 0)
			this->NoteOff(p1, 0);
		else
			this->NoteOn(p1, p2);
		break;
	case 0xb0:			// Control change
		switch (p1)
		{
		case 1:			// Control #1
			this->CurrentCtrl1	= p2;
			this->Ctrl1ModEnd	= (float)(p2 - 64) / 64.f;
			if(this->Ctrl1ModEnd != this->Ctrl1Mod)
			{
				this->Ctrl1ModStep = (this->Ctrl1ModEnd - this->Ctrl1Mod) * this->ModChangeSamples;
			}
			else
				this->Ctrl1ModStep = 0.f;
			break;
		case 5:			// Portamento time
			this->setParameterAutomated(pPortaSpeed, (float)p2 / 127.f);
			break;
		case 7:			// Volume
			this->setParameterAutomated(pVolume, (float)p2 / 127.f);
			break;
		case 8:			// Panning
			this->setParameterAutomated(pPanning, (float)p2 / 127.f);
			break;
		case 65:		// Portamento switch
			btmp = (p2 == 0) ? false : true;
			if(!btmp && this->DoPorta)
			{
				this->CurrentPitch	=	this->PortaPitch;
				this->DoPorta		=	false;
			}
			this->setParameterAutomated(pPortaMode, (btmp) ? 1.f : 0.f);
			break;
		case 75:		// Cutoff
			this->setParameterAutomated(pCutoff, (float)p2 / 127.f);
			break;
		case 76:		// Resonance
			this->setParameterAutomated(pResonance, (float)p2 / 127.f);
			break;
		case 80:		// Mod 1 Amount
			this->setParameterAutomated(pMod1Amount, (float)p2 / 127.f);
			break;
		case 81:		// Mod 2 Amount
			this->setParameterAutomated(pMod2Amount, (float)p2 / 127.f);
			break;
		case 82:		// Mod 3 Amount
			this->setParameterAutomated(pMod3Amount, (float)p2 / 127.f);
			break;
		case 83:		// Mod 4 Amount
			this->setParameterAutomated(pMod4Amount, (float)p2 / 127.f);
			break;
		case 123:
			this->CurrentNote	= -1;
			break;
		}
		break;
	case 0xc0:			// Program change
		this->ReadProgram(p1);
		break;
	}
}

void CCetoneSynth::NoteOn(int note, int vel)
{
	int tmp;
	bool porta = (this->PortaMode && (this->PortaSpeed != 0.f) && (this->CurrentNote != -1)) ? true : false;

	this->CurrentNote		= note;
	this->CurrentVelocity	= vel;
	this->VelocityModEnd	= (float)vel / 127.f;

	if(this->VelocityModEnd != this->VelocityMod)
	{
		this->VelocityModStep = (this->VelocityModEnd - this->VelocityMod) * this->ModChangeSamples;
	}
	else
		this->VelocityModStep = 0.f;

	tmp = (note + NOTE_OFFSET) * 100;

	if(porta)
	{
		this->PortaStep		= (int)(((tmp - this->CurrentPitch) / (this->PortaSamples)) * 16384.f + 0.5f);
		this->PortaFrac     = this->CurrentPitch << 14;
		this->PortaPitch	= tmp;
	}
	else
	{
		this->CurrentPitch = tmp;
	}

	for(int i = 0; i < 3; i++)
	{
		this->VoicePulsewidth[i] = this->Voice[i].Pw;
		this->Oscs[i]->Set(this->VoicePulsewidth[i], this->Voice[i].Wave, this->Voice[i].Sync);
	}

	this->DoPorta		= porta;

	this->Envs[0]->Gate(true);
	this->Envs[1]->Gate(true);

	this->Lfo->Set(this->LfoSpeed, this->LfoPw, this->LfoWave, this->LfoTrigger);
	this->Lfo->Trigger();
}

void CCetoneSynth::NoteOff(int note, int vel)
{
	this->Envs[0]->Gate(false);
	this->Envs[1]->Gate(false);
}

void CCetoneSynth::UpdateFilters()
{
	this->UpdateFilters(this->Cutoff, this->Resonance, this->EnvMod);
}

void CCetoneSynth::UpdateFilters(float cutoff, float q, float mod)
{
	if(this->FilterCounter != FILTER_DELAY)
		return;

	switch (this->FilterType)
	{
	default:
		break;
	case FTYPE_DIRTY:
		this->FilterDirty->Set(cutoff, q);
		break;
	case FTYPE_CH12DB:
		this->FilterCh12db->Set(cutoff, q);
		break;
	case FTYPE_MOOG:
		this->FilterMoog->Set(cutoff, q);
		break;
	case FTYPE_MOOG2:
		this->FilterMoog2->Set(cutoff, q);
		break;
	case FTYPE_303:
		this->Filter303->Set(cutoff, q, mod);
		break;
	case FTYPE_8580:
		this->Filter8580->Set(cutoff, q);
		break;
	case FTYPE_BUDDA:
		this->FilterBiquad->Set(cutoff, q);
		break;
	}

}

void CCetoneSynth::SetFilterMode(int mode)
{
	switch (this->FilterType)
	{
	default:
		mode = 0;
		break;
	case FTYPE_DIRTY:
		this->FilterDirty->SetMode(mode);
		mode = this->FilterDirty->GetMode();
		break;
	case FTYPE_CH12DB:
		this->FilterCh12db->SetMode(mode);
		mode = this->FilterCh12db->GetMode();
		break;
	case FTYPE_MOOG:
		this->FilterMoog->SetMode(mode);
		mode = this->FilterMoog->GetMode();
		break;
	case FTYPE_MOOG2:
		this->FilterMoog2->SetMode(mode);
		mode = this->FilterMoog2->GetMode();
		break;
	case FTYPE_303:
		this->Filter303->SetMode(mode);
		mode = this->Filter303->GetMode();
		break;
	case FTYPE_8580:
		this->Filter8580->SetMode(mode);
		mode = this->Filter8580->GetMode();
		break;
	case FTYPE_BUDDA:
		mode = FMODE_LOW;
		break;
	}

	this->FilterMode = mode;
	this->Programs[this->CurrentProgram].FilterMode = mode;
}

void CCetoneSynth::SetArpSpeed(int ms)
{
	float sec = (float)ms / 1000.f;
	this->ArpDelay = (int)(sec * this->SampleRate + 0.5f);
}

void CCetoneSynth::SetPortaSpeed(float speed)
{
	this->PortaSamples = floorf(speed * this->SampleRate + 0.5f);
}

void CCetoneSynth::UpdateEnvelopes()
{
	for(int i = 0; i < 2; i++)
		this->Envs[i]->Set(this->EnvAttack[i], this->EnvHold[i], this->EnvDecay[i], this->EnvSustain[i], this->EnvRelease[i]);
}

void CCetoneSynth::resume()
{
	AudioEffectX::resume();

#if ANALOGUE_BEHAVIOR == 0
	this->Oscs[0]->Reset();
	this->Oscs[1]->Reset();
	this->Oscs[2]->Reset();

	this->Lfos->Reset();
#endif
	
	this->Envs[0]->Reset();
	this->Envs[1]->Reset();

	this->Filter303->Reset();
	this->Filter8580->Reset();
	this->FilterMoog->Reset();
	this->FilterMoog2->Reset();
	this->FilterDirty->Reset();
	this->FilterCh12db->Reset();

	this->VelocityMod = 0.f;
	this->Ctrl1Mod = 0.f;
	this->VelocityModStep = 0.f;
	this->Ctrl1ModStep = 0.f;

	this->FilterCounter = FILTER_DELAY;
	this->UpdateFilters();

	this->CutoffStep = 0.f;

	for(int i = 0; i < 1000; i++)
	{
		float output;
		
		switch(this->FilterType)
		{
		default:
			break;
		case FTYPE_DIRTY:
			output = this->FilterDirty->Run(0.f);
			break;
		case FTYPE_MOOG:
			output = this->FilterMoog->Run(0.f);
			break;
		case FTYPE_MOOG2:
			output = this->FilterMoog2->Run(0.f);
			break;
		case FTYPE_CH12DB:
			output = this->FilterCh12db->Run(0.f);
			break;
		case FTYPE_303:
			output = this->Filter303->Run(0.f);
			break;
		case FTYPE_8580:
			output = this->Filter8580->Run(0.f);
			break;
		case FTYPE_BUDDA:
			output = this->FilterBiquad->Run(0.f);
			break;
		}
	}
}

void CCetoneSynth::SetCutoffSave(float value)
{
	if(value != this->Cutoff)
	{
		this->CutoffDest = value;

		float delta = this->Cutoff - value;
		float samples = fabs(delta) * 10.f * this->ModChangeSamples;
		this->CutoffStep = delta / samples;
	}
	else
		this->CutoffStep = 0.f;
}