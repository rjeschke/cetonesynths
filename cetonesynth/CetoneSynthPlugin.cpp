#include <math.h>

#include "aeffguieditor.h"
#include "cetonesynth.h"

extern bool TablesBuilt;

VstInt32 CCetoneSynth::getNumMidiInputChannels() { return 1; }
VstInt32 CCetoneSynth::getNumMidiOutputChannels(){ return 0; }

void CCetoneSynth::setSampleRate (float fs)
{
	AudioEffectX::setSampleRate (fs);

	if(this->SampleRate != fs)
	{
		CCetoneSynth::SampleRate	= fs;
		CCetoneSynth::SampleRate2	= CCetoneSynth::SampleRate / 2.f;
		CCetoneSynth::SampleRate_1	= 1.f / CCetoneSynth::SampleRate;
		CCetoneSynth::SampleRate2_1	= 1.f / CCetoneSynth::SampleRate2;
		CCetoneSynth::SampleRatePi	= this->Pi / CCetoneSynth::SampleRate;
		this->ModChangeSamples		= 1.f / (this->SampleRate * MOD_CHANGE_SPEED);

		this->Filter8580->CalcClock();

		TablesBuilt = false;

		this->InitFreqTable();
	}
}

void CCetoneSynth::setBlockSize (VstInt32 blockSize)
{
	AudioEffectX::setBlockSize (blockSize);
}

VstInt32 CCetoneSynth::getVendorVersion ()
{
	return VERSION_NUMBER; 
}

bool CCetoneSynth::getEffectName(char* name)
{
	vst_strncpy(name, EFFECT_NAME, kVstMaxEffectNameLen);
	return true;
}

bool CCetoneSynth::getVendorString(char* text)
{
	vst_strncpy(text, VENDOR_NAME, kVstMaxVendorStrLen);
	return true;
}

bool CCetoneSynth::getProductString(char* text)
{
	vst_strncpy(text, PRODUCT_NAME, kVstMaxProductStrLen);
	return true;
}

VstInt32 CCetoneSynth::canDo(char* text)
{
	if (!strcmp(text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	return -1;
}

VstInt32 CCetoneSynth::getChunk(void** data, bool isPreset)
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

VstInt32 CCetoneSynth::setChunk(void* data, VstInt32 byteSize, bool isPreset)
{
	if(isPreset)
	{
		if (byteSize != sizeof(SynthProgram))
			return 0;

		memcpy(&Programs[this->CurrentProgram], data, sizeof(SynthProgram));

		this->ReadProgram(this->CurrentProgram);

		for(int i = 0; i < pParameters; i++)
			this->setParameter(i, this->getParameter(i));

		return sizeof(SynthProgram);
	}
	else
	{
		if (byteSize != sizeof(SynthProgram) * 128)
			return 0;

		memcpy(&Programs[0], data, sizeof(SynthProgram) * 128);

		this->ReadProgram(this->CurrentProgram);

		for(int i = 0; i < pParameters; i++)
			this->setParameter(i, this->getParameter(i));

		return sizeof(SynthProgram) * 128;
	}
}

VstInt32 CCetoneSynth::getProgram()
{
	return this->CurrentProgram;
}

void CCetoneSynth::setProgram(VstInt32 program)
{
	if(program < 0 || program > 127)
		return;

	this->ReadProgram(program);
}

void CCetoneSynth::setProgramName(char* name)
{
	vst_strncpy(this->Programs[this->CurrentProgram].Name, name, kVstMaxProgNameLen);
}

void CCetoneSynth::getProgramName(char* name)
{
	vst_strncpy(name, this->Programs[this->CurrentProgram].Name, kVstMaxProgNameLen);
}

bool CCetoneSynth::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
	if(index < 0 || index > 127)
		return false;

	vst_strncpy(text, this->Programs[index].Name, kVstMaxProgNameLen);

	return true;
}

void CCetoneSynth::getParameterLabel(VstInt32 index, char* label)
{
	switch (index)
	{
	case pOsc1Wave:		case pOsc2Wave:		case pOsc3Wave:
	case pLfo1Wave:		case pLfo2Wave:		case pHfoWave:
		vst_strncpy (label, "Shape", kVstMaxParamStrLen);
		break;
	case pOsc1Coarse:	case pOsc2Coarse:	case pOsc3Coarse:
	case pCoarse:	case pHfoCoarse:
		vst_strncpy (label, "Notes", kVstMaxParamStrLen);
		break;
	case pOsc1Fine:	case pOsc2Fine:	case pOsc3Fine:
	case pFine:	case pHfoFine:
		vst_strncpy (label, "Cents", kVstMaxParamStrLen);
		break;
	case pEnv1A:	case pEnv1H:	case pEnv1D:
	case pEnv1R:	case pEnv2A:	case pEnv2H:	case pEnv2D:
	case pEnv2R:	case pEnv3A:	case pEnv3H:
	case pEnv3D:	case pEnv3R:	case pPortaSpeed:
		vst_strncpy (label, "sec", kVstMaxParamStrLen);
		break;
	case pCutoff:	case pLfo1Speed:	case pLfo2Speed:
		vst_strncpy (label, "Hz", kVstMaxParamStrLen);
		break;
	case pOsc1Pw:	case pOsc2Pw:	case pOsc3Pw:	case pLfo1Pw:
	case pLfo2Pw:	case pHfoPw:
		vst_strncpy (label, "%", kVstMaxParamStrLen);
		break;
	case pArpSpeed:
		vst_strncpy (label, "ms", kVstMaxParamStrLen);
		break;
	}
}

void CCetoneSynth::getParameterDisplay(VstInt32 index, char* text)
{
	SynthProgram* p = &Programs[this->CurrentProgram];

	switch (index)
	{
	default:			vst_strncpy(text, "Unknown", kVstMaxParamStrLen); break;
	case pMidiChan:		int2string(this->MidiChannel + 1, text, kVstMaxParamStrLen); break;
	
	case pVolume:		myfloat2string(p->Volume, text);	break;
	case pPanning:		myfloat2string(p->Panning, text); break;

	case pCoarse:		int2string(p->Coarse, text, kVstMaxParamStrLen); break;
	case pFine:			int2string(p->Fine, text, kVstMaxParamStrLen); break;

	case pFilterType:	ftype2str(p->FilterType, text); break;
	case pFilterMode:	fmode2str(p->FilterMode, text); break;

	case pCutoff:		myfloat2string(p->Cutoff, text); break;
	case pResonance:	myfloat2string(p->Resonance, text); break;

	case pPortaMode:	bool2string(p->PortaMode, text); break;
	case pPortaSpeed:	myfloat2string(p->PortaSpeed, text); break;

	case pArpMode:		arp2str(p->ArpMode, text); break;
	case pArpSpeed:		int2string(p->ArpSpeed, text, kVstMaxParamStrLen); break;

	case pOsc1Coarse:	int2string(p->Voice[0].Coarse, text, kVstMaxParamStrLen); break;
	case pOsc2Coarse:	int2string(p->Voice[1].Coarse, text, kVstMaxParamStrLen); break;
	case pOsc3Coarse:	int2string(p->Voice[2].Coarse, text, kVstMaxParamStrLen); break;
	case pHfoCoarse:	int2string(p->Voice[3].Coarse, text, kVstMaxParamStrLen); break;

	case pOsc1Fine:		int2string(p->Voice[0].Fine, text, kVstMaxParamStrLen); break;
	case pOsc2Fine:		int2string(p->Voice[1].Fine, text, kVstMaxParamStrLen); break;
	case pOsc3Fine:		int2string(p->Voice[2].Fine, text, kVstMaxParamStrLen); break;
	case pHfoFine:		int2string(p->Voice[3].Fine, text, kVstMaxParamStrLen); break;

	case pOsc1Wave:		owave2str(p->Voice[0].Wave, text); break;
	case pOsc2Wave:		owave2str(p->Voice[1].Wave, text); break;
	case pOsc3Wave:		owave2str(p->Voice[2].Wave, text); break;
	case pHfoWave:		owave2str(p->Voice[3].Wave, text); break;

	case pOsc1Pw:		int2string(p->Voice[0].Pw, text, kVstMaxParamStrLen); break;
	case pOsc2Pw:		int2string(p->Voice[1].Pw, text, kVstMaxParamStrLen); break;
	case pOsc3Pw:		int2string(p->Voice[2].Pw, text, kVstMaxParamStrLen); break;
	case pHfoPw:		int2string(p->Voice[3].Pw, text, kVstMaxParamStrLen); break;

	case pOsc1Volume:	myfloat2string(p->Voice[0].Volume, text); break;
	case pOsc2Volume:	myfloat2string(p->Voice[1].Volume, text); break;
	case pOsc3Volume:	myfloat2string(p->Voice[2].Volume, text); break;

	case pOsc1Sync:		bool2string(p->Voice[1].Sync, text); break;
	case pOsc2Sync:		bool2string(p->Voice[2].Sync, text); break;
	case pOsc3Sync:		bool2string(p->Voice[0].Sync, text); break;

	case pOsc1Ring:		bool2string(p->Voice[0].Ring, text); break;
	case pOsc2Ring:		bool2string(p->Voice[1].Ring, text); break;
	case pOsc3Ring:		bool2string(p->Voice[2].Ring, text); break;

	case pEnv1A:		myfloat2string(p->Attack[0], text); break;
	case pEnv2A:		myfloat2string(p->Attack[1], text); break;
	case pEnv3A:		myfloat2string(p->Attack[2], text); break;

	case pEnv1H:		myfloat2string(p->Hold[0], text); break;
	case pEnv2H:		myfloat2string(p->Hold[1], text); break;
	case pEnv3H:		myfloat2string(p->Hold[2], text); break;

	case pEnv1D:		myfloat2string(p->Decay[0], text); break;
	case pEnv2D:		myfloat2string(p->Decay[1], text); break;
	case pEnv3D:		myfloat2string(p->Decay[2], text); break;

	case pEnv1S:		myfloat2string(p->Sustain[0], text); break;
	case pEnv2S:		myfloat2string(p->Sustain[1], text); break;
	case pEnv3S:		myfloat2string(p->Sustain[2], text); break;

	case pEnv1R:		myfloat2string(p->Release[0], text); break;
	case pEnv2R:		myfloat2string(p->Release[1], text); break;
	case pEnv3R:		myfloat2string(p->Release[2], text); break;

	case pLfo1Speed:	myfloat2string(p->LfoSpeed[0], text); break;
	case pLfo2Speed:	myfloat2string(p->LfoSpeed[1], text); break;

	case pLfo1Wave:		wave2str(p->LfoWave[0], text); break;
	case pLfo2Wave:		wave2str(p->LfoWave[1], text); break;

	case pLfo1Pw:		int2string(p->LfoPw[0], text, kVstMaxParamStrLen); break;
	case pLfo2Pw:		int2string(p->LfoPw[1], text, kVstMaxParamStrLen); break;

	case pLfo1Trig:		bool2string(p->LfoTrigger[0], text); break;
	case pLfo2Trig:		bool2string(p->LfoTrigger[1], text); break;

	case pMod1Src:		msrc2str(p->Modulations[0].Source, text); break;
	case pMod2Src:		msrc2str(p->Modulations[1].Source, text); break;
	case pMod3Src:		msrc2str(p->Modulations[2].Source, text); break;
	case pMod4Src:		msrc2str(p->Modulations[3].Source, text); break;
	case pMod5Src:		msrc2str(p->Modulations[4].Source, text); break;
	case pMod6Src:		msrc2str(p->Modulations[5].Source, text); break;
	case pMod7Src:		msrc2str(p->Modulations[6].Source, text); break;
	case pMod8Src:		msrc2str(p->Modulations[7].Source, text); break;

	case pMod1Dest:		mdest2str(p->Modulations[0].Destination, text); break;
	case pMod2Dest:		mdest2str(p->Modulations[1].Destination, text); break;
	case pMod3Dest:		mdest2str(p->Modulations[2].Destination, text); break;
	case pMod4Dest:		mdest2str(p->Modulations[3].Destination, text); break;
	case pMod5Dest:		mdest2str(p->Modulations[4].Destination, text); break;
	case pMod6Dest:		mdest2str(p->Modulations[5].Destination, text); break;
	case pMod7Dest:		mdest2str(p->Modulations[6].Destination, text); break;
	case pMod8Dest:		mdest2str(p->Modulations[7].Destination, text); break;

	case pMod1Amount:	int2string((int)p->Modulations[0].Amount, text, kVstMaxParamStrLen); break;
	case pMod2Amount:	int2string((int)p->Modulations[1].Amount, text, kVstMaxParamStrLen); break;
	case pMod3Amount:	int2string((int)p->Modulations[2].Amount, text, kVstMaxParamStrLen); break;
	case pMod4Amount:	int2string((int)p->Modulations[3].Amount, text, kVstMaxParamStrLen); break;
	case pMod5Amount:	int2string((int)p->Modulations[4].Amount, text, kVstMaxParamStrLen); break;
	case pMod6Amount:	int2string((int)p->Modulations[5].Amount, text, kVstMaxParamStrLen); break;
	case pMod7Amount:	int2string((int)p->Modulations[6].Amount, text, kVstMaxParamStrLen); break;
	case pMod8Amount:	int2string((int)p->Modulations[7].Amount, text, kVstMaxParamStrLen); break;

	case pMod1Mul:		int2string((int)p->Modulations[0].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod2Mul:		int2string((int)p->Modulations[1].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod3Mul:		int2string((int)p->Modulations[2].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod4Mul:		int2string((int)p->Modulations[3].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod5Mul:		int2string((int)p->Modulations[4].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod6Mul:		int2string((int)p->Modulations[5].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod7Mul:		int2string((int)p->Modulations[6].Multiplicator, text, kVstMaxParamStrLen); break;
	case pMod8Mul:		int2string((int)p->Modulations[7].Multiplicator, text, kVstMaxParamStrLen); break;

	case pFilterMod:	myfloat2string(p->EnvMod, text); break;
	}
}

void CCetoneSynth::getParameterName(VstInt32 index, char* text)
{
	switch (index)
	{
	default:			vst_strncpy(text, "Unknown", kVstMaxParamStrLen);	break;
	case pMidiChan:		vst_strncpy(text, "Channel", kVstMaxParamStrLen);	break;

	case pVolume:		vst_strncpy(text, "Volume", kVstMaxParamStrLen);	break;
	case pPanning:		vst_strncpy(text, "Panning", kVstMaxParamStrLen);	break;

	case pCoarse:		vst_strncpy(text, "Coarse", kVstMaxParamStrLen);	break;
	case pFine:			vst_strncpy(text, "Fine", kVstMaxParamStrLen);		break;

	case pFilterType:	vst_strncpy(text, "F.Type", kVstMaxParamStrLen);	break;
	case pFilterMode:	vst_strncpy(text, "F.Mode", kVstMaxParamStrLen);	break;

	case pCutoff:		vst_strncpy(text, "Cutoff", kVstMaxParamStrLen);	break;
	case pResonance:	vst_strncpy(text, "Q", kVstMaxParamStrLen);			break;

	case pPortaMode:	vst_strncpy(text, "G.Mode", kVstMaxParamStrLen);	break;
	case pPortaSpeed:	vst_strncpy(text, "G.Speed", kVstMaxParamStrLen);	break;

	case pArpMode:		vst_strncpy(text, "A.Mode", kVstMaxParamStrLen);	break;
	case pArpSpeed:		vst_strncpy(text, "A.Speed", kVstMaxParamStrLen);	break;

	case pOsc1Coarse:	vst_strncpy(text, "Coarse 1", kVstMaxParamStrLen);	break;
	case pOsc2Coarse:	vst_strncpy(text, "Coarse 2", kVstMaxParamStrLen);	break;
	case pOsc3Coarse:	vst_strncpy(text, "Coarse 3", kVstMaxParamStrLen);	break;
	case pHfoCoarse:	vst_strncpy(text, "Coarse H", kVstMaxParamStrLen);	break;

	case pOsc1Fine:		vst_strncpy(text, "Fine 1", kVstMaxParamStrLen);	break;
	case pOsc2Fine:		vst_strncpy(text, "Fine 2", kVstMaxParamStrLen);	break;
	case pOsc3Fine:		vst_strncpy(text, "Fine 3", kVstMaxParamStrLen);	break;
	case pHfoFine:		vst_strncpy(text, "Fine H", kVstMaxParamStrLen);	break;

	case pOsc1Wave:		vst_strncpy(text, "Wave 1", kVstMaxParamStrLen);	break;
	case pOsc2Wave:		vst_strncpy(text, "Wave 2", kVstMaxParamStrLen);	break;
	case pOsc3Wave:		vst_strncpy(text, "Wave 3", kVstMaxParamStrLen);	break;
	case pHfoWave:		vst_strncpy(text, "Wave H", kVstMaxParamStrLen);	break;

	case pOsc1Pw:		vst_strncpy(text, "Pw 1", kVstMaxParamStrLen);		break;
	case pOsc2Pw:		vst_strncpy(text, "Pw 2", kVstMaxParamStrLen);		break;
	case pOsc3Pw:		vst_strncpy(text, "Pw 3", kVstMaxParamStrLen);		break;
	case pHfoPw:		vst_strncpy(text, "Pw H", kVstMaxParamStrLen);		break;

	case pOsc1Volume:	vst_strncpy(text, "Volume 1", kVstMaxParamStrLen);	break;
	case pOsc2Volume:	vst_strncpy(text, "Volume 2", kVstMaxParamStrLen);	break;
	case pOsc3Volume:	vst_strncpy(text, "Volume 3", kVstMaxParamStrLen);	break;

	case pOsc1Ring:		vst_strncpy(text, "Ring 1", kVstMaxParamStrLen);	break;
	case pOsc2Ring:		vst_strncpy(text, "Ring 2", kVstMaxParamStrLen);	break;
	case pOsc3Ring:		vst_strncpy(text, "Ring 2", kVstMaxParamStrLen);	break;

	case pOsc1Sync:		vst_strncpy(text, "Sync 1", kVstMaxParamStrLen);	break;
	case pOsc2Sync:		vst_strncpy(text, "Sync 2", kVstMaxParamStrLen);	break;
	case pOsc3Sync:		vst_strncpy(text, "Sync 3", kVstMaxParamStrLen);	break;

	case pEnv1A:		vst_strncpy(text, "Env1 A", kVstMaxParamStrLen);	break;
	case pEnv2A:		vst_strncpy(text, "MEnv1 A", kVstMaxParamStrLen);	break;
	case pEnv3A:		vst_strncpy(text, "MEnv2 A", kVstMaxParamStrLen);	break;

	case pEnv1H:		vst_strncpy(text, "Env1 H", kVstMaxParamStrLen);	break;
	case pEnv2H:		vst_strncpy(text, "MEnv1 H", kVstMaxParamStrLen);	break;
	case pEnv3H:		vst_strncpy(text, "MEnv2 H", kVstMaxParamStrLen);	break;

	case pEnv1D:		vst_strncpy(text, "Env1 D", kVstMaxParamStrLen);	break;
	case pEnv2D:		vst_strncpy(text, "MEnv1 D", kVstMaxParamStrLen);	break;
	case pEnv3D:		vst_strncpy(text, "MEnv2 D", kVstMaxParamStrLen);	break;

	case pEnv1S:		vst_strncpy(text, "Env1 S", kVstMaxParamStrLen);	break;
	case pEnv2S:		vst_strncpy(text, "MEnv1 S", kVstMaxParamStrLen);	break;
	case pEnv3S:		vst_strncpy(text, "MEnv2 S", kVstMaxParamStrLen);	break;

	case pEnv1R:		vst_strncpy(text, "Env1 R", kVstMaxParamStrLen);	break;
	case pEnv2R:		vst_strncpy(text, "MEnv1 R", kVstMaxParamStrLen);	break;
	case pEnv3R:		vst_strncpy(text, "MEnv2 R", kVstMaxParamStrLen);	break;

	case pLfo1Speed:	vst_strncpy(text, "L1 Speed", kVstMaxParamStrLen);	break;
	case pLfo2Speed:	vst_strncpy(text, "L2 Speed", kVstMaxParamStrLen);	break;

	case pLfo1Wave:		vst_strncpy(text, "L1 Wave", kVstMaxParamStrLen);	break;
	case pLfo2Wave:		vst_strncpy(text, "L2 Wave", kVstMaxParamStrLen);	break;

	case pLfo1Pw:		vst_strncpy(text, "L1 Pw", kVstMaxParamStrLen);		break;
	case pLfo2Pw:		vst_strncpy(text, "L2 Pw", kVstMaxParamStrLen);		break;

	case pLfo1Trig:		vst_strncpy(text, "L1 Trig.", kVstMaxParamStrLen);	break;
	case pLfo2Trig:		vst_strncpy(text, "L2 Trig.", kVstMaxParamStrLen);	break;

	case pMod1Src:		vst_strncpy(text, "M1 Src.", kVstMaxParamStrLen);	break;
	case pMod2Src:		vst_strncpy(text, "M2 Src.", kVstMaxParamStrLen);	break;
	case pMod3Src:		vst_strncpy(text, "M3 Src.", kVstMaxParamStrLen);	break;
	case pMod4Src:		vst_strncpy(text, "M4 Src.", kVstMaxParamStrLen);	break;
	case pMod5Src:		vst_strncpy(text, "M5 Src.", kVstMaxParamStrLen);	break;
	case pMod6Src:		vst_strncpy(text, "M6 Src.", kVstMaxParamStrLen);	break;
	case pMod7Src:		vst_strncpy(text, "M7 Src.", kVstMaxParamStrLen);	break;
	case pMod8Src:		vst_strncpy(text, "M8 Src.", kVstMaxParamStrLen);	break;

	case pMod1Dest:		vst_strncpy(text, "M1 Dest.", kVstMaxParamStrLen);	break;
	case pMod2Dest:		vst_strncpy(text, "M2 Dest.", kVstMaxParamStrLen);	break;
	case pMod3Dest:		vst_strncpy(text, "M3 Dest.", kVstMaxParamStrLen);	break;
	case pMod4Dest:		vst_strncpy(text, "M4 Dest.", kVstMaxParamStrLen);	break;
	case pMod5Dest:		vst_strncpy(text, "M5 Dest.", kVstMaxParamStrLen);	break;
	case pMod6Dest:		vst_strncpy(text, "M6 Dest.", kVstMaxParamStrLen);	break;
	case pMod7Dest:		vst_strncpy(text, "M7 Dest.", kVstMaxParamStrLen);	break;
	case pMod8Dest:		vst_strncpy(text, "M8 Dest.", kVstMaxParamStrLen);	break;

	case pMod1Amount:	vst_strncpy(text, "M1 Amnt.", kVstMaxParamStrLen);	break;
	case pMod2Amount:	vst_strncpy(text, "M2 Amnt.", kVstMaxParamStrLen);	break;
	case pMod3Amount:	vst_strncpy(text, "M3 Amnt.", kVstMaxParamStrLen);	break;
	case pMod4Amount:	vst_strncpy(text, "M4 Amnt.", kVstMaxParamStrLen);	break;
	case pMod5Amount:	vst_strncpy(text, "M5 Amnt.", kVstMaxParamStrLen);	break;
	case pMod6Amount:	vst_strncpy(text, "M6 Amnt.", kVstMaxParamStrLen);	break;
	case pMod7Amount:	vst_strncpy(text, "M7 Amnt.", kVstMaxParamStrLen);	break;
	case pMod8Amount:	vst_strncpy(text, "M8 Amnt.", kVstMaxParamStrLen);	break;

	case pMod1Mul:		vst_strncpy(text, "M1 Mul.", kVstMaxParamStrLen);	break;
	case pMod2Mul:		vst_strncpy(text, "M2 Mul.", kVstMaxParamStrLen);	break;
	case pMod3Mul:		vst_strncpy(text, "M3 Mul.", kVstMaxParamStrLen);	break;
	case pMod4Mul:		vst_strncpy(text, "M4 Mul.", kVstMaxParamStrLen);	break;
	case pMod5Mul:		vst_strncpy(text, "M5 Mul.", kVstMaxParamStrLen);	break;
	case pMod6Mul:		vst_strncpy(text, "M6 Mul.", kVstMaxParamStrLen);	break;
	case pMod7Mul:		vst_strncpy(text, "M7 Mul.", kVstMaxParamStrLen);	break;
	case pMod8Mul:		vst_strncpy(text, "M8 Mul.", kVstMaxParamStrLen);	break;

	case pFilterMod:	vst_strncpy(text, "F.Param.", kVstMaxParamStrLen);	break;
	}
}

void CCetoneSynth::setParameter(VstInt32 index, float value)
{
	SynthProgram* p = &Programs[this->CurrentProgram];

	switch (index)
	{
	default:	break;
	case pMidiChan:	this->MidiChannel = (int)(value * 15.f + 0.5f); break;

	case pVolume:	this->Volume = p->Volume = value * 5.f; break;
	case pPanning:	this->Panning = p->Panning = value; break;

	case pCoarse:	this->MainCoarse = p->Coarse = c_val2coarse(value); break;
	case pFine:		this->MainFine = p->Fine = c_val2fine(value); break;

	case pFilterType:	this->FilterType = p->FilterType = pf2i(value, FTYPE_MAX); break;
	case pFilterMode:	this->SetFilterMode(pf2i(value, FMODE_MAX)); break;

	case pCutoff:		p->Cutoff = c_val2cutoff(value); this->SetCutoffSave(p->Cutoff); break;
	case pResonance:	this->Resonance = p->Resonance = value; break;

	case pPortaMode:	this->PortaMode = p->PortaMode = c_val2bool(value); break;
	case pPortaSpeed:	this->PortaSpeed = p->PortaSpeed = value * 5.f; this->SetPortaSpeed(this->PortaSpeed); break;

	case pArpMode:		this->ArpMode = p->ArpMode = pf2i(value, ARP_MAX + 1) - 1; break;
	case pArpSpeed:		this->ArpSpeed = p->ArpSpeed = (int)(value * 500.f + 0.5f); this->SetArpSpeed(this->ArpSpeed); break;

	case pOsc1Coarse:	this->Voice[0].Coarse = p->Voice[0].Coarse = c_val2coarse(value); break;
	case pOsc2Coarse:	this->Voice[1].Coarse = p->Voice[1].Coarse = c_val2coarse(value); break;
	case pOsc3Coarse:	this->Voice[2].Coarse = p->Voice[2].Coarse = c_val2coarse(value); break;
	case pHfoCoarse:	this->Voice[3].Coarse = p->Voice[3].Coarse = c_val2coarse(value); break;

	case pOsc1Fine:		this->Voice[0].Fine = p->Voice[0].Fine = c_val2fine(value); break;
	case pOsc2Fine:		this->Voice[1].Fine = p->Voice[1].Fine = c_val2fine(value); break;
	case pOsc3Fine:		this->Voice[2].Fine = p->Voice[2].Fine = c_val2fine(value); break;
	case pHfoFine:		this->Voice[3].Fine = p->Voice[3].Fine = c_val2fine(value); break;

	case pOsc1Wave:		this->Voice[0].Wave = p->Voice[0].Wave = pf2i(value, OWAVE_MAX); break;
	case pOsc2Wave:		this->Voice[1].Wave = p->Voice[1].Wave = pf2i(value, OWAVE_MAX); break;
	case pOsc3Wave:		this->Voice[2].Wave = p->Voice[2].Wave = pf2i(value, OWAVE_MAX); break;
	case pHfoWave:		this->Voice[3].Wave = p->Voice[3].Wave = pf2i(value, OWAVE_MAX); break;
	case pLfo1Wave:		this->LfoWave[0] = p->LfoWave[0] = pf2i(value, WAVE_MAX); this->Lfos[0]->SetWave(this->LfoWave[0]); break;
	case pLfo2Wave:		this->LfoWave[1] = p->LfoWave[1] = pf2i(value, WAVE_MAX); this->Lfos[1]->SetWave(this->LfoWave[1]);  break;

	case pOsc1Pw:		this->Voice[0].Pw = p->Voice[0].Pw = c_val2pw(value); break;
	case pOsc2Pw:		this->Voice[1].Pw = p->Voice[1].Pw = c_val2pw(value); break;
	case pOsc3Pw:		this->Voice[2].Pw = p->Voice[2].Pw = c_val2pw(value); break;
	case pHfoPw:		this->Voice[3].Pw = p->Voice[3].Pw = c_val2pw(value); break;
	case pLfo1Pw:		this->LfoPw[0] = p->LfoPw[0] = c_val2pw(value); this->Lfos[0]->SetPw(this->LfoPw[0]); break;
	case pLfo2Pw:		this->LfoPw[1] = p->LfoPw[1] = c_val2pw(value); this->Lfos[1]->SetPw(this->LfoPw[1]);  break;

	case pOsc1Volume:	this->Voice[0].Volume = p->Voice[0].Volume = value * 5.f; break;
	case pOsc2Volume:	this->Voice[1].Volume = p->Voice[1].Volume = value * 5.f; break;
	case pOsc3Volume:	this->Voice[2].Volume = p->Voice[2].Volume = value * 5.f; break;

	case pOsc1Ring:		this->Voice[0].Ring = p->Voice[0].Ring = c_val2bool(value); break;
	case pOsc2Ring:		this->Voice[1].Ring = p->Voice[1].Ring = c_val2bool(value); break;
	case pOsc3Ring:		this->Voice[2].Ring = p->Voice[2].Ring = c_val2bool(value); break;

	case pOsc1Sync:		this->Voice[1].Sync = p->Voice[1].Sync = c_val2bool(value); break;
	case pOsc2Sync:		this->Voice[2].Sync = p->Voice[2].Sync = c_val2bool(value); break;
	case pOsc3Sync:		this->Voice[0].Sync = p->Voice[0].Sync = c_val2bool(value); break;

	case pLfo1Speed:	this->LfoSpeed[0] = p->LfoSpeed[0] = value * 50.f; this->Lfos[0]->SetSpeed(this->LfoSpeed[0]); break;
	case pLfo2Speed:	this->LfoSpeed[1] = p->LfoSpeed[1] = value * 50.f; this->Lfos[1]->SetSpeed(this->LfoSpeed[1]); break;

	case pLfo1Trig:		this->LfoTrigger[0] = p->LfoTrigger[0] = c_val2bool(value); this->Lfos[0]->SetTrigger(this->LfoTrigger[0]); break;
	case pLfo2Trig:		this->LfoTrigger[1] = p->LfoTrigger[1] = c_val2bool(value); this->Lfos[1]->SetTrigger(this->LfoTrigger[1]);  break;

	case pEnv1A:		this->EnvAttack[0] = p->Attack[0] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv2A:		this->EnvAttack[1] = p->Attack[1] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv3A:		this->EnvAttack[2] = p->Attack[2] = value * 10.f; this->UpdateEnvelopes(); break;

	case pEnv1H:		this->EnvHold[0] = p->Hold[0] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv2H:		this->EnvHold[1] = p->Hold[1] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv3H:		this->EnvHold[2] = p->Hold[2] = value * 10.f; this->UpdateEnvelopes(); break;

	case pEnv1D:		this->EnvDecay[0] = p->Decay[0] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv2D:		this->EnvDecay[1] = p->Decay[1] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv3D:		this->EnvDecay[2] = p->Decay[2] = value * 10.f; this->UpdateEnvelopes(); break;

	case pEnv1S:		this->EnvSustain[0] = p->Sustain[0] = value; this->UpdateEnvelopes(); break;
	case pEnv2S:		this->EnvSustain[1] = p->Sustain[1] = value; this->UpdateEnvelopes(); break;
	case pEnv3S:		this->EnvSustain[2] = p->Sustain[2] = value; this->UpdateEnvelopes(); break;

	case pEnv1R:		this->EnvRelease[0] = p->Release[0] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv2R:		this->EnvRelease[1] = p->Release[1] = value * 10.f; this->UpdateEnvelopes(); break;
	case pEnv3R:		this->EnvRelease[2] = p->Release[2] = value * 10.f; this->UpdateEnvelopes(); break;

	case pMod1Src:		this->Modulations[0].Source = p->Modulations[0].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod2Src:		this->Modulations[1].Source = p->Modulations[1].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod3Src:		this->Modulations[2].Source = p->Modulations[2].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod4Src:		this->Modulations[3].Source = p->Modulations[3].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod5Src:		this->Modulations[4].Source = p->Modulations[4].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod6Src:		this->Modulations[5].Source = p->Modulations[5].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod7Src:		this->Modulations[6].Source = p->Modulations[6].Source = pf2i(value, MOD_SRC_MAX); break;
	case pMod8Src:		this->Modulations[7].Source = p->Modulations[7].Source = pf2i(value, MOD_SRC_MAX); break;

	case pMod1Dest:		this->Modulations[0].Destination = p->Modulations[0].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod2Dest:		this->Modulations[1].Destination = p->Modulations[1].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod3Dest:		this->Modulations[2].Destination = p->Modulations[2].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod4Dest:		this->Modulations[3].Destination = p->Modulations[3].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod5Dest:		this->Modulations[4].Destination = p->Modulations[4].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod6Dest:		this->Modulations[5].Destination = p->Modulations[5].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod7Dest:		this->Modulations[6].Destination = p->Modulations[6].Destination = pf2i(value, MOD_DEST_MAX); break;
	case pMod8Dest:		this->Modulations[7].Destination = p->Modulations[7].Destination = pf2i(value, MOD_DEST_MAX); break;

	case pMod1Amount:	this->Modulations[0].Amount = p->Modulations[0].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod2Amount:	this->Modulations[1].Amount = p->Modulations[1].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod3Amount:	this->Modulations[2].Amount = p->Modulations[2].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod4Amount:	this->Modulations[3].Amount = p->Modulations[3].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod5Amount:	this->Modulations[4].Amount = p->Modulations[4].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod6Amount:	this->Modulations[5].Amount = p->Modulations[5].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod7Amount:	this->Modulations[6].Amount = p->Modulations[6].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;
	case pMod8Amount:	this->Modulations[7].Amount = p->Modulations[7].Amount = floorf(value * 200.f + 0.5f) - 100.f; break;

	case pMod1Mul:		this->Modulations[0].Multiplicator = p->Modulations[0].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod2Mul:		this->Modulations[1].Multiplicator = p->Modulations[1].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod3Mul:		this->Modulations[2].Multiplicator = p->Modulations[2].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod4Mul:		this->Modulations[3].Multiplicator = p->Modulations[3].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod5Mul:		this->Modulations[4].Multiplicator = p->Modulations[4].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod6Mul:		this->Modulations[5].Multiplicator = p->Modulations[5].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod7Mul:		this->Modulations[6].Multiplicator = p->Modulations[6].Multiplicator = floorf(value * 100.f + 0.5f); break;
	case pMod8Mul:		this->Modulations[7].Multiplicator = p->Modulations[7].Multiplicator = floorf(value * 100.f + 0.5f); break;

	case pFilterMod:	this->EnvMod = p->EnvMod = (value - 0.5f) * 2.f; break;
	}

	if(this->getEditor())
	{
		if(this->getEditor()->isOpen())
			((AEffGUIEditor*)this->getEditor())->setParameter(index, value);
	}
}

float CCetoneSynth::getParameter(VstInt32 index)
{
	SynthProgram* p = &Programs[this->CurrentProgram];

	float ret = 0.f;
	switch (index)
	{
	default:	break;
	case pMidiChan:		ret = this->MidiChannel / 15.f; break;
		
	case pVolume:		ret = p->Volume / 5.f; break;
	case pPanning:		ret = p->Panning; break;

	case pCoarse:		ret = c_coarse2val(p->Coarse); break;
	case pFine:			ret = c_fine2val(p->Fine); break;

	case pFilterType:	ret = pi2f(p->FilterType, FTYPE_MAX); break;
	case pFilterMode:	ret = pi2f(p->FilterMode, FMODE_MAX); break;

	case pCutoff:		ret = c_cutoff2val(p->Cutoff); break;
	case pResonance:	ret = p->Resonance;	break;

	case pPortaMode:	ret = c_bool2val(p->PortaMode); break;
	case pPortaSpeed:	ret = p->PortaSpeed / 5.f; break;

	case pArpMode:		ret = pi2f(p->ArpMode + 1, ARP_MAX + 1); break;
	case pArpSpeed:		ret = p->ArpSpeed / 500.f; break;

	case pOsc1Coarse:	ret = c_coarse2val(p->Voice[0].Coarse); break;
	case pOsc2Coarse:	ret = c_coarse2val(p->Voice[1].Coarse); break;
	case pOsc3Coarse:	ret = c_coarse2val(p->Voice[2].Coarse); break;
	case pHfoCoarse:	ret = c_coarse2val(p->Voice[3].Coarse); break;

	case pOsc1Fine:		ret = c_fine2val(p->Voice[0].Fine); break;
	case pOsc2Fine:		ret = c_fine2val(p->Voice[1].Fine); break;
	case pOsc3Fine:		ret = c_fine2val(p->Voice[2].Fine); break;
	case pHfoFine:		ret = c_fine2val(p->Voice[3].Fine); break;

	case pOsc1Wave:		ret = pi2f(p->Voice[0].Wave, OWAVE_MAX); break;
	case pOsc2Wave:		ret = pi2f(p->Voice[1].Wave, OWAVE_MAX); break;
	case pOsc3Wave:		ret = pi2f(p->Voice[2].Wave, OWAVE_MAX); break;
	case pHfoWave:		ret = pi2f(p->Voice[3].Wave, OWAVE_MAX); break;
	case pLfo1Wave:		ret = pi2f(p->LfoWave[0], WAVE_MAX); break;
	case pLfo2Wave:		ret = pi2f(p->LfoWave[1], WAVE_MAX); break;

	case pOsc1Pw:		ret = p->Voice[0].Pw / 65536.f; break;
	case pOsc2Pw:		ret = p->Voice[1].Pw / 65536.f; break;
	case pOsc3Pw:		ret = p->Voice[2].Pw / 65536.f; break;
	case pHfoPw:		ret = p->Voice[3].Pw / 65536.f; break;
	case pLfo1Pw:		ret = p->LfoPw[0] / 65536.f; break;
	case pLfo2Pw:		ret = p->LfoPw[1] / 65536.f; break;

	case pOsc1Volume:	ret = p->Voice[0].Volume / 5.f; break;
	case pOsc2Volume:	ret = p->Voice[1].Volume / 5.f; break;
	case pOsc3Volume:	ret = p->Voice[2].Volume / 5.f; break;

	case pOsc1Ring:		ret = c_bool2val(p->Voice[0].Ring); break;
	case pOsc2Ring:		ret = c_bool2val(p->Voice[1].Ring); break;
	case pOsc3Ring:		ret = c_bool2val(p->Voice[2].Ring); break;

	case pOsc1Sync:		ret = c_bool2val(p->Voice[1].Sync); break;
	case pOsc2Sync:		ret = c_bool2val(p->Voice[2].Sync); break;
	case pOsc3Sync:		ret = c_bool2val(p->Voice[0].Sync); break;

	case pLfo1Speed:	ret = p->LfoSpeed[0] / 50.f; break;
	case pLfo2Speed:	ret = p->LfoSpeed[1] / 50.f; break;

	case pLfo1Trig:		ret = c_bool2val(p->LfoTrigger[0]); break;
	case pLfo2Trig:		ret = c_bool2val(p->LfoTrigger[1]); break;

	case pEnv1A:		ret = p->Attack[0] / 10.f; break;
	case pEnv2A:		ret = p->Attack[1] / 10.f; break;
	case pEnv3A:		ret = p->Attack[2] / 10.f; break;

	case pEnv1H:		ret = p->Hold[0] / 10.f; break;
	case pEnv2H:		ret = p->Hold[1] / 10.f; break;
	case pEnv3H:		ret = p->Hold[2] / 10.f; break;

	case pEnv1D:		ret = p->Decay[0] / 10.f; break;
	case pEnv2D:		ret = p->Decay[1] / 10.f; break;
	case pEnv3D:		ret = p->Decay[2] / 10.f; break;

	case pEnv1S:		ret = p->Sustain[0]; break;
	case pEnv2S:		ret = p->Sustain[1]; break;
	case pEnv3S:		ret = p->Sustain[2]; break;

	case pEnv1R:		ret = p->Release[0] / 10.f; break;
	case pEnv2R:		ret = p->Release[1] / 10.f; break;
	case pEnv3R:		ret = p->Release[2] / 10.f; break;

	case pMod1Src:		ret = pi2f(p->Modulations[0].Source, MOD_SRC_MAX); break;
	case pMod2Src:		ret = pi2f(p->Modulations[1].Source, MOD_SRC_MAX); break;
	case pMod3Src:		ret = pi2f(p->Modulations[2].Source, MOD_SRC_MAX); break;
	case pMod4Src:		ret = pi2f(p->Modulations[3].Source, MOD_SRC_MAX); break;
	case pMod5Src:		ret = pi2f(p->Modulations[4].Source, MOD_SRC_MAX); break;
	case pMod6Src:		ret = pi2f(p->Modulations[5].Source, MOD_SRC_MAX); break;
	case pMod7Src:		ret = pi2f(p->Modulations[6].Source, MOD_SRC_MAX); break;
	case pMod8Src:		ret = pi2f(p->Modulations[7].Source, MOD_SRC_MAX); break;

	case pMod1Dest:		ret = pi2f(p->Modulations[0].Destination, MOD_DEST_MAX); break;
	case pMod2Dest:		ret = pi2f(p->Modulations[1].Destination, MOD_DEST_MAX); break;
	case pMod3Dest:		ret = pi2f(p->Modulations[2].Destination, MOD_DEST_MAX); break;
	case pMod4Dest:		ret = pi2f(p->Modulations[3].Destination, MOD_DEST_MAX); break;
	case pMod5Dest:		ret = pi2f(p->Modulations[4].Destination, MOD_DEST_MAX); break;
	case pMod6Dest:		ret = pi2f(p->Modulations[5].Destination, MOD_DEST_MAX); break;
	case pMod7Dest:		ret = pi2f(p->Modulations[6].Destination, MOD_DEST_MAX); break;
	case pMod8Dest:		ret = pi2f(p->Modulations[7].Destination, MOD_DEST_MAX); break;

	case pMod1Amount:	ret = (p->Modulations[0].Amount + 100) / 200.f; break;
	case pMod2Amount:	ret = (p->Modulations[1].Amount + 100) / 200.f; break;
	case pMod3Amount:	ret = (p->Modulations[2].Amount + 100) / 200.f; break;
	case pMod4Amount:	ret = (p->Modulations[3].Amount + 100) / 200.f; break;
	case pMod5Amount:	ret = (p->Modulations[4].Amount + 100) / 200.f; break;
	case pMod6Amount:	ret = (p->Modulations[5].Amount + 100) / 200.f; break;
	case pMod7Amount:	ret = (p->Modulations[6].Amount + 100) / 200.f; break;
	case pMod8Amount:	ret = (p->Modulations[7].Amount + 100) / 200.f; break;

	case pMod1Mul:		ret = p->Modulations[0].Multiplicator / 100.f; break;
	case pMod2Mul:		ret = p->Modulations[1].Multiplicator / 100.f; break;
	case pMod3Mul:		ret = p->Modulations[2].Multiplicator / 100.f; break;
	case pMod4Mul:		ret = p->Modulations[3].Multiplicator / 100.f; break;
	case pMod5Mul:		ret = p->Modulations[4].Multiplicator / 100.f; break;
	case pMod6Mul:		ret = p->Modulations[5].Multiplicator / 100.f; break;
	case pMod7Mul:		ret = p->Modulations[6].Multiplicator / 100.f; break;
	case pMod8Mul:		ret = p->Modulations[7].Multiplicator / 100.f; break;
	case pFilterMod:	ret = (p->EnvMod + 1.f) / 2.f; break;
	}

	return ret;
}
