#include <math.h>

#include "aeffguieditor.h"
#include "cetone033.h"

extern bool TablesBuilt;

VstInt32 CCetone033::getNumMidiInputChannels() { return 1; }
VstInt32 CCetone033::getNumMidiOutputChannels(){ return 0; }

void CCetone033::setSampleRate (float fs)
{
	AudioEffectX::setSampleRate (fs);

	if(this->SampleRate != fs)
	{
		TablesBuilt = false;
		this->InitFreqTables(fs);
		this->Filter->SetSampleRate(fs);
		this->UpdateEnvelopes();
		this->SetGlideSpeed(this->GlideSpeed);
	}
}

void CCetone033::setBlockSize (VstInt32 blockSize)
{
	AudioEffectX::setBlockSize (blockSize);
}

VstInt32 CCetone033::getVendorVersion ()
{
	return VERSION_NUMBER; 
}

bool CCetone033::getEffectName(char* name)
{
	vst_strncpy(name, EFFECT_NAME, kVstMaxEffectNameLen);
	return true;
}

bool CCetone033::getVendorString(char* text)
{
	vst_strncpy(text, VENDOR_NAME, kVstMaxVendorStrLen);
	return true;
}

bool CCetone033::getProductString(char* text)
{
	vst_strncpy(text, PRODUCT_NAME, kVstMaxProductStrLen);
	return true;
}

VstInt32 CCetone033::canDo(char* text)
{
	if (!strcmp(text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	return -1;
}

VstInt32 CCetone033::getChunk(void** data, bool isPreset)
{
	if(isPreset)
	{
		*data = &Programs[this->CurrentProgram];
		return sizeof(SynthProgram);
	}
	else
	{
		*data = &Programs[0];
		return sizeof(SynthProgram) * 128;
	}
}

VstInt32 CCetone033::setChunk(void* data, VstInt32 byteSize, bool isPreset)
{
	if(isPreset)
	{
		if (byteSize == sizeof(SynthProgram))
		{
			memcpy(&Programs[this->CurrentProgram], data, sizeof(SynthProgram));

			this->ReadProgram(this->CurrentProgram);

			for(int i = 0; i < pParameters; i++)
				this->setParameterAutomated(i, this->getParameter(i));

			return sizeof(SynthProgram);
		} else if(byteSize == sizeof(SynthProgramOld))
		{
			memcpy(&OldPrograms[this->CurrentProgram], data, sizeof(SynthProgramOld));

			this->ImportProgram(&OldPrograms[this->CurrentProgram], &Programs[this->CurrentProgram]);

			this->ReadProgram(this->CurrentProgram);

			for(int i = 0; i < pParameters; i++)
				this->setParameterAutomated(i, this->getParameter(i));

			return sizeof(SynthProgramOld);
		}
		else
			return 0;
	}
	else
	{
		if (byteSize == sizeof(SynthProgram) * 128)
		{
			memcpy(&Programs[0], data, sizeof(SynthProgram) * 128);

			this->ReadProgram(this->CurrentProgram);

			for(int i = 0; i < pParameters; i++)
				this->setParameterAutomated(i, this->getParameter(i));

			return sizeof(SynthProgram) * 128;
		}
		else if (byteSize == sizeof(SynthProgramOld) * 128)
		{
			memcpy(&OldPrograms[0], data, sizeof(SynthProgramOld) * 128);

			for(int i = 0; i < 128; i++)
				this->ImportProgram(&OldPrograms[i], &Programs[i]);

			this->ReadProgram(this->CurrentProgram);

			for(int i = 0; i < pParameters; i++)
				this->setParameterAutomated(i, this->getParameter(i));

			return sizeof(SynthProgramOld) * 128;
		}
		else
			return 0;
	}
}

VstInt32 CCetone033::getProgram()
{
	return this->CurrentProgram;
}

void CCetone033::setProgram(VstInt32 program)
{
	if(program < 0 || program > 127)
		return;

	this->ReadProgram(program);

	for(int i = 0; i < pParameters; i++)
		this->setParameterAutomated(i, this->getParameter(i));
}

void CCetone033::setProgramName(char* name)
{
	vst_strncpy(this->Programs[this->CurrentProgram].Name, name, kVstMaxProgNameLen);
}

void CCetone033::getProgramName(char* name)
{
	vst_strncpy(name, this->Programs[this->CurrentProgram].Name, kVstMaxProgNameLen);
}

bool CCetone033::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
	if(index < 0 || index > 127)
		return false;

	vst_strncpy(text, this->Programs[index].Name, kVstMaxProgNameLen);

	return true;
}

void CCetone033::getParameterLabel(VstInt32 index, char* label)
{
	label[0] = 0;
}

void CCetone033::getParameterDisplay(VstInt32 index, char* text)
{
	SynthProgram* p = &Programs[this->CurrentProgram];

	switch (index)
	{
	default:			vst_strncpy(text, "Unknown", kVstMaxParamStrLen); break;
	case pOsc1Coarse:	int2string(p->Coarse[0], text, kVstMaxParamStrLen); break;
	case pOsc1Fine:		int2string(p->Fine[0], text, kVstMaxParamStrLen); break;
	case pOsc1Wave:		wave2str(p->Wave[0], text); break;
	case pOsc1Morph:	myfloat2string(p->Morph[0], text); break;
	case pOsc1Volume:	myfloat2string(p->Volume[0], text); break;

	case pOsc2Coarse:	int2string(p->Coarse[1], text, kVstMaxParamStrLen); break;
	case pOsc2Fine:		int2string(p->Fine[1], text, kVstMaxParamStrLen); break;
	case pOsc2Wave:		wave2str(p->Wave[1], text); break;
	case pOsc2Morph:	myfloat2string(p->Morph[1], text); break;
	case pOsc2Volume:	myfloat2string(p->Volume[1], text); break;

	case pVolume:		myfloat2string(p->MainVolume, text); break;
	case pCutoff:		myfloat2string(p->Cutoff, text); break;
	case pResonance:	myfloat2string(p->Resonance, text); break;
	case pFilterType:   this->Filter->Name(this->FilterType, text); break;

	case pEnv1Attack:	myfloat2string(p->Attack[0], text); break;
	case pEnv1Decay:	myfloat2string(p->Decay[0], text); break;

	case pEnv2Attack:	myfloat2string(p->Attack[1], text); break;
	case pEnv2Decay:	myfloat2string(p->Decay[1], text); break;

	case pModEnv:		myfloat2string(p->ModEnv, text); break;
	case pModVel:		myfloat2string(p->ModVel, text); break;
	case pModRes:		myfloat2string(p->ModRes, text); break;

	case pGlideSpeed:	myfloat2string(p->GlideSpeed, text); break;
	case pGlideState:	bool2string(p->GlideState, text); break;
	case pClipState:	bool2string(p->ClipState, text); break;
	}
}

void CCetone033::getParameterName(VstInt32 index, char* text)
{
	switch (index)
	{
	default:			vst_strncpy(text, "Unknown", kVstMaxParamStrLen);	break;
	case pOsc1Coarse:	vst_strncpy(text, "Coarse 1", kVstMaxParamStrLen);	break;
	case pOsc2Coarse:	vst_strncpy(text, "Coarse 2", kVstMaxParamStrLen);	break;
	case pOsc1Fine:		vst_strncpy(text, "Fine 1", kVstMaxParamStrLen);	break;
	case pOsc2Fine:		vst_strncpy(text, "Fine 2", kVstMaxParamStrLen);	break;
	case pOsc1Wave:		vst_strncpy(text, "Wave 1", kVstMaxParamStrLen);	break;
	case pOsc2Wave:		vst_strncpy(text, "Wave 2", kVstMaxParamStrLen);	break;
	case pOsc1Morph:	vst_strncpy(text, "Morph 1", kVstMaxParamStrLen);	break;
	case pOsc2Morph:	vst_strncpy(text, "Morph 2", kVstMaxParamStrLen);	break;
	case pOsc1Volume:	vst_strncpy(text, "Vol. 1", kVstMaxParamStrLen);	break;
	case pOsc2Volume:	vst_strncpy(text, "Vol. 2", kVstMaxParamStrLen);	break;
	
	case pVolume:		vst_strncpy(text, "Volume", kVstMaxParamStrLen);	break;
	case pCutoff:		vst_strncpy(text, "Cutoff", kVstMaxParamStrLen);	break;
	case pResonance:	vst_strncpy(text, "Q", kVstMaxParamStrLen);			break;
	case pFilterType:	vst_strncpy(text, "F.Type", kVstMaxParamStrLen);	break;

	case pEnv1Attack:	vst_strncpy(text, "Env1 A", kVstMaxParamStrLen);	break;
	case pEnv2Attack:	vst_strncpy(text, "Env2 A", kVstMaxParamStrLen);	break;

	case pEnv1Decay:	vst_strncpy(text, "Env1 D", kVstMaxParamStrLen);	break;
	case pEnv2Decay:	vst_strncpy(text, "Env2 D", kVstMaxParamStrLen);	break;

	case pModEnv:		vst_strncpy(text, "ModEnv", kVstMaxParamStrLen);	break;
	case pModVel:		vst_strncpy(text, "ModVel", kVstMaxParamStrLen);	break;
	case pModRes:		vst_strncpy(text, "ModRes", kVstMaxParamStrLen);	break;

	case pGlideState:	vst_strncpy(text, "Glide", kVstMaxParamStrLen);	break;
	case pGlideSpeed:	vst_strncpy(text, "G.Speed", kVstMaxParamStrLen);	break;
	case pClipState:	vst_strncpy(text, "Clip", kVstMaxParamStrLen);	break;
	}
}

void CCetone033::setParameter(VstInt32 index, float value)
{
	SynthProgram* p = &Programs[this->CurrentProgram];

	switch (index)
	{
	case pOsc1Coarse:	this->Coarse[0] = p->Coarse[0] = c_val2coarse(value); break;
	case pOsc2Coarse:	this->Coarse[1] = p->Coarse[1] = c_val2coarse(value); break;
	case pOsc1Fine:		this->Fine[0] = p->Fine[0] = c_val2fine(value); break;
	case pOsc2Fine:		this->Fine[1] = p->Fine[1] = c_val2fine(value); break;
	case pOsc1Morph:	this->Morph[0] = p->Morph[0] = value; break;
	case pOsc2Morph:	this->Morph[1] = p->Morph[1] = value; break;
	case pOsc1Wave:		this->Wave[0] = p->Wave[0] = pf2i(value, WAVE_MAX); break;
	case pOsc2Wave:		this->Wave[1] = p->Wave[1] = pf2i(value, WAVE_MAX); break;
	case pOsc1Volume:	this->Volume[0] = p->Volume[0] = value * 2.f; break;
	case pOsc2Volume:	this->Volume[1] = p->Volume[1] = value * 2.f; break;

	case pVolume:		this->MainVolume = p->MainVolume = value * 2.f; break;
	case pCutoff:		p->Cutoff = value; this->SetCutoffSave(value); break;
	case pResonance:	this->Resonance = p->Resonance = value; break;
	case pFilterType:	this->FilterType = p->FilterType = pf2i(value, FILTER_TYPE_MAX); this->Filter->SetType(p->FilterType); break;

	case pEnv1Attack:	this->Attack[0] = p->Attack[0] = value; this->UpdateEnvelopes(); break;
	case pEnv2Attack:	this->Attack[1] = p->Attack[1] = value; this->UpdateEnvelopes(); break;

	case pEnv1Decay:	this->Decay[0] = p->Decay[0] = value; this->UpdateEnvelopes(); break;
	case pEnv2Decay:	this->Decay[1] = p->Decay[1] = value; this->UpdateEnvelopes(); break;

	case pModEnv:		this->ModEnv = p->ModEnv = value - 0.5f; break;
	case pModVel:		this->ModVel = p->ModVel = value; break;
	case pModRes:		this->SetModRes(value); break;

	case pGlideState:	this->SetGlideState(c_val2bool(value)); break;
	case pGlideSpeed:	this->SetGlideSpeed(value); break;
	case pClipState:	this->ClipState = p->ClipState = c_val2bool(value); break;
	}

	if(this->getEditor())
	{
		if(this->getEditor()->isOpen())
			((AEffGUIEditor*)this->getEditor())->setParameter(index, value);
	}
}

float CCetone033::getParameter(VstInt32 index)
{
	SynthProgram* p = &Programs[this->CurrentProgram];

	float ret = 0.f;
	switch (index)
	{
	case pOsc1Coarse:	ret = c_coarse2val(p->Coarse[0]); break;
	case pOsc2Coarse:	ret = c_coarse2val(p->Coarse[1]); break;
	case pOsc1Fine:		ret = c_fine2val(p->Fine[0]); break;
	case pOsc2Fine:		ret = c_fine2val(p->Fine[1]); break;
	case pOsc1Morph:	ret = p->Morph[0]; break;
	case pOsc2Morph:	ret = p->Morph[1]; break;
	case pOsc1Wave:		ret = pi2f(p->Wave[0], WAVE_MAX); break;
	case pOsc2Wave:		ret = pi2f(p->Wave[1], WAVE_MAX); break;
	case pOsc1Volume:	ret = p->Volume[0] * 0.5f; break;
	case pOsc2Volume:	ret = p->Volume[1] * 0.5f; break;

	case pVolume:		ret = p->MainVolume * 0.5f; break;
	case pCutoff:		ret = p->Cutoff; break;
	case pResonance:	ret = p->Resonance; break;
	case pFilterType:	ret = pi2f(p->FilterType, FILTER_TYPE_MAX); break;

	case pEnv1Attack:	ret = p->Attack[0]; break;
	case pEnv2Attack:	ret = p->Attack[1]; break;

	case pEnv1Decay:	ret = p->Decay[0]; break;
	case pEnv2Decay:	ret = p->Decay[1]; break;

	case pModEnv:		ret = p->ModEnv + 0.5f; break;
	case pModVel:		ret = p->ModVel; break;
	case pModRes:		ret = p->ModRes; break;

	case pGlideState:	ret = c_bool2val(p->GlideState); break;
	case pGlideSpeed:	ret = p->GlideSpeed; break;
	case pClipState:	ret = c_bool2val(p->ClipState); break;
	}

	return ret;
}

void CCetone033::ImportProgram(SynthProgramOld *src, SynthProgram *dest)
{
	for(int i = 0; i < 128; i++)
		dest->Name[i] = src->Name[i];

	dest->Attack[0] = src->Attack[0];
	dest->Attack[1] = src->Attack[1];
	dest->Decay[0] = src->Decay[0];
	dest->Decay[1] = src->Decay[1];
	dest->Coarse[0] = src->Coarse[0];
	dest->Coarse[1] = src->Coarse[1];
	dest->Fine[0] = src->Fine[0];
	dest->Fine[1] = src->Fine[1];
	dest->Wave[0] = src->Wave[0];
	dest->Wave[1] = src->Wave[1];
	dest->Volume[0] = src->Volume[0];
	dest->Volume[1] = src->Volume[1];
	dest->Morph[0] = src->Morph[0];
	dest->Morph[1] = src->Morph[1];

	dest->ClipState = src->ClipState;
	dest->MainVolume = src->MainVolume;

	dest->GlideState = src->GlideState;
	dest->GlideSpeed = src->GlideSpeed;

	dest->FilterType = FILTER_TYPE_BIQUAD;
	dest->Cutoff = src->Cutoff;
	dest->Resonance = src->Resonance;

	dest->ModEnv = src->ModEnv;
	dest->ModRes = src->ModRes;
	dest->ModVel = src->ModVel;
}
