#include <math.h>

#include "cetone033.h"

void CCetone033::process(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->SynthProcess(inputs, outputs, sampleFrames, false);
}

void CCetone033::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->SynthProcess(inputs, outputs, sampleFrames, true);
}

void CCetone033::SynthProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace)
{
	float* out = outputs[0];
	
	int p0, p1, p2, delta, tdelta = 0;
	float output;
 
	float evolume0 = this->MainVolume;
	float evolume1 = this->Volume[0];
	float evolume2 = this->Volume[1];

	int pitch0 = this->Coarse[0] * 100 + this->Fine[0];
	int pitch1 = this->Coarse[1] * 100 + this->Fine[1];

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
		
		output = 0.f;

		// Filter counter

		if(this->FilterCounter <= 0)
			this->FilterCounter = FILTER_DELAY;
		else
			this->FilterCounter--;

		// Filter set-safe 

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

		// Velocity mod slide

		if(this->CurrentNote != -1)
		{
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

			// Glide

			if (this->DoGlide)
			{
				this->GlideFrac += this->GlideStep;
				int itmp = this->GlideFrac >> 14;

				if(this->GlideStep < 0)
				{
					if(itmp <= this->GlidePitch)
					{
						itmp = this->GlidePitch;
						this->DoGlide = false;
					}
				}
				else
				{
					if(itmp >= this->GlidePitch)
					{
						itmp = this->GlidePitch;
						this->DoGlide = false;
					}
				}

				this->CurrentPitch = itmp;
			}

			this->Oscs[0]->SetPitch(this->CurrentPitch + pitch0);
			this->Oscs[1]->SetPitch(this->CurrentPitch + pitch1);

			// Envelopes

			for(int i = 0; i < 2; i++)
			{
				switch(this->EnvPos[i])
				{
				case 0:
					this->VoiceVolume[i] += this->AttackFactor[i];
					if(this->VoiceVolume[i] >= 1.f)
					{
						this->VoiceVolume[i] = 1.f;
						this->EnvPos[i] = 1;
					}
					break;
				case 1:
					this->VoiceVolume[i] -= this->DecayFactor[i];
					if(this->VoiceVolume[i] <= 0.f)
					{
						this->VoiceVolume[i] = 0.f;
						this->EnvPos[i] = -1;
					}
					break;
				default:
					this->VoiceVolume[i] = 0.f;
					break;
				}
			}

			float output_volume = this->VoiceVolume[0] * evolume0;

			// Oscillators

			float f;

			f = this->Oscs[0]->Run() * evolume1;
			if(f > 2.f)
				f = 2.f;
			else if(f < -2.f)
				f = -2.f;

			output += f;

			f = this->Oscs[1]->Run() * evolume2;

			if(f > 2.f)
				f = 2.f;
			else if(f < -2.f)
				f = -2.f;

			output = output + f;

			// Filters

			if(this->FilterCounter == FILTER_DELAY)
			{
				float c = this->Cutoff;
				c += this->ModEnv * this->VoiceVolume[1] * this->SampleRateEnv;
				c += this->ModVel * this->VelocityMod * this->SampleRateVel;
				this->DecayResonance *= this->ModResValue;
				this->Filter->Set(c, this->DecayResonance);
			}

			output = this->Filter->Run(output);

			// End

			output *= output_volume;

			if(this->ClipState)
			{	
				float s;

				if(output < 0.f)
				{
					s = -1.f;
					output = -output;
				}
				else
					s = 1.f;

                if (output > 1.f)
                    output = 0.9f;
				else if (output >= 0.8f)
                    output = 0.8f + (output - 0.8f) / (1.f + powf(((output - 0.8f) * 5.f), 2.f));
                
                output *= s;
			}
		}

		/****************************************************************************

		Run ends

		****************************************************************************/

		if (replace)
			(*out++) = output;
		else
			(*out++) += output;
		
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

VstInt32 CCetone033::processEvents(VstEvents *events)
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

void CCetone033::HandleMidi(int p0, int p1, int p2)
{
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
		case 5:			// Portamento time
			this->setParameterAutomated(pGlideSpeed, (float)p2 / 127.f);
			break;
		case 7:			// Volume
			this->setParameterAutomated(pVolume, (float)p2 / 127.f);
			break;
		case 65:		// Portamento switch
			this->setParameterAutomated(pGlideState, (p2 < 64) ? 0.f : 1.f);
			break;
		case 75:		// Cutoff
			this->setParameterAutomated(pCutoff, (float)p2 / 127.f);
			break;
		case 76:		// Resonance
			this->setParameterAutomated(pResonance, (float)p2 / 127.f);
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

void CCetone033::NoteOn(int note, int vel)
{
	int tmp;
	bool glide = (this->GlideState && (this->GlideSpeed != 0.f) && (this->CurrentNote != -1)) ? true : false;

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

	if(glide)
	{
		this->GlideStep		= (int)(((tmp - this->CurrentPitch) / (this->GlideSamples)) * 16384.f + 0.5f);
		this->GlideFrac     = this->CurrentPitch << 14;
		this->GlidePitch	= tmp;
	}
	else
	{
		this->CurrentPitch = tmp;
	}

	this->Oscs[0]->Set(this->Morph[0], this->Wave[0]);
	this->Oscs[1]->Set(this->Morph[1], this->Wave[1]);

	this->EnvPos[0] = 0;
	this->EnvPos[1] = 0;

	this->DecayResonance = this->Resonance;

	this->DoGlide		= glide;
}

void CCetone033::NoteOff(int note, int vel)
{

}

void CCetone033::SetGlideSpeed(float speed)
{
	if(speed < 0.001f)
		speed = 0.001f;

	this->GlideSamples = floorf(speed * this->SampleRate + 0.5f);
	
	this->GlideSpeed = speed;
	this->Programs[this->CurrentProgram].GlideSpeed = speed;
}

void CCetone033::SetGlideState(bool state)
{
	if(!state && this->DoGlide)
	{
		this->CurrentPitch  = this->GlidePitch;
		this->DoGlide		= false;
	}

	this->GlideState = state;
	this->Programs[this->CurrentProgram].GlideState = state;
}

void CCetone033::SetModRes(float value)
{
	this->ModRes = value;
	this->Programs[this->CurrentProgram].ModRes = value;
	this->ModResValue = 1.f - value * 0.1f;
}

void CCetone033::resume()
{
	AudioEffectX::resume();

	this->FilterCounter		= FILTER_DELAY;

	this->DecayResonance	= this->Resonance;

	this->SetModRes(this->ModRes);

	this->VoiceVolume[0]	= 0.f;
	this->VoiceVolume[1]	= 0.f;

	this->VelocityMod		= 0.f;
	this->VelocityModStep	= 0.f;

	this->EnvPos[0]			= -1;
	this->EnvPos[1]			= -1;

	this->CutoffStep		= 0.f;
	this->ResonanceStep		= 0.f;

	this->Filter->Reset();

	this->Filter->Set(this->Cutoff, this->Resonance);

	for(int i = 0; i < 4096; i++)
		float t = this->Filter->Run(0.f);
}

void CCetone033::UpdateEnvelopes()
{
	// Envelope voodoo goes here ...

	float a, d;

	a = this->Attack[0];
	d = this->Decay[0];
	if(a < 0.01f)
		a = 0.01f;
	if(d < 0.01f)
		d = 0.01f;
	this->AttackFactor[0] =  1.f / (this->SampleRate * a);
	this->DecayFactor[0] =  1.f / (this->SampleRate * d);

	a = this->Attack[1];
	d = this->Decay[1];
	if(a < 0.05f)
		a = 0.05f;
	if(d < 0.05f)
		d = 0.05f;
	this->AttackFactor[1] =  1.f / (this->SampleRate * a);
	this->DecayFactor[1] =  1.f / (this->SampleRate * d);
}

void CCetone033::SetCutoffSave(float value)
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

void CCetone033::SetResonanceSave(float value)
{
	if(value != this->Resonance)
	{
		this->ResonanceDest = value;
		this->ResonanceStep = (this->Resonance - value) / (this->ModChangeSamples);
	}
	else
		this->ResonanceStep = 0.f;
}
