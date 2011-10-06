#include <stdio.h>
#include "statics.h"
#include "Gui.h"
#include "CetoneSynth2.h"
#include "parameters.h"

typedef struct
{
	char* name;
	int ctrl;
	int pval;
	int voffs;
	int div;
} VST_PARAM;

#define PARAM_COUNT 46

static VST_PARAM _params[] = 
{	//01234567
	{"Volume", 7, pMixVol, 0, 1},
	{"Panning", 10, pMixPan, 64, 1},
	{"Glide", 5, pGlide, 0, 1},
	{"FiltGain", 12, pFGain, 64, 1},
	{"FiltEnv2", 13, pFEnv2, 64, 1},
	{"Osc1Semi", 14, pOSemi1, 64, 1},
	{"Osc1Fine", 17, pOFine1, 64, 1},
	{"Osc1Mrph", 20, pOMorph1, 0, 1},
	{"Osc1Sprd", 23, pOSpread1, 0, 4},
	{"Osc1Vol", 26, pMixVol1, 0, 1},
	{"Osc2Semi", 15, pOSemi2, 64, 1},
	{"Osc2Fine", 18, pOFine2, 64, 1},
	{"Osc2Mrph", 21, pOMorph2, 0, 1},
	{"Osc2Sprd", 24, pOSpread2, 0, 4},
	{"Osc2Vol", 27, pMixVol2, 0, 1},
	{"Osc3Semi", 16, pOSemi3, 64, 1},
	{"Osc3Fine", 19, pOFine3, 64, 1},
	{"Osc3Mrph", 22, pOMorph3, 0, 1},
	{"Osc3Sprd", 25, pOSpread3, 0, 4},
	{"Osc3Vol", 28, pMixVol3, 0, 1},
	{"AInVol", 29, pAudioVol, 0, 1},
	{"AInPan", 30, pAudioPan, 64, 1},
	{"Mod1Amnt", 44, pM1Amount, 64, 1},
	{"Mod2Amnt", 45, pM2Amount, 64, 1},
	{"Mod3Amnt", 46, pM3Amount, 64, 1},
	{"Mod4Amnt", 47, pM4Amount, 64, 1},
	{"Mod5Amnt", 48, pM5Amount, 64, 1},
	{"Mod6Amnt", 49, pM6Amount, 64, 1},
	{"ArpSpeed", 50, pArpSpeed, 0, 1},
	{"ArpLen", 51, pArpLength, 0, 16},
	{"ArpNote1", 52, pArpNote1, 64, 1},
	{"ArpNote2", 53, pArpNote2, 64, 1},
	{"ArpNote3", 54, pArpNote3, 64, 1},
	{"ArpNote4", 55, pArpNote4, 64, 1},
	{"ArpNote5", 56, pArpNote5, 64, 1},
	{"ArpNote6", 57, pArpNote6, 64, 1},
	{"ArpNote7", 58, pArpNote7, 64, 1},
	{"ArpNote8", 59, pArpNote8, 64, 1},
	{"Cutoff", 74, pFCutoff, 0, 1},
	{"Q", 71, pFQ, 0, 1},
	{"Lfo1Spd", 80, pL1Speed, 0, 1},
	{"Lfo1Dly", 81, pL1Delay, 0, 1},
	{"Lfo1PW", 82, pL1PW, 0, 1},
	{"Lfo2Spd", 83, pL2Speed, 0, 1},
	{"Lfo2Dly", 84, pL2Delay, 0, 1},
	{"Lfo2PW", 85, pL2PW, 0, 1}
};

static float i2f(int val)
{
	return (float)val / 127.0f;
}

static int f2i(float val)
{
	return (int)floorf(val * 127.0f + 0.5f);
}

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new CS2(audioMaster);
}

CS2::CS2(audioMasterCallback audioMaster)
: AudioEffectX(audioMaster, 128, PARAM_COUNT)
{
	int i;
	
	setNumInputs(2);
	setNumOutputs(2);
	setUniqueID('cSC2');
	canMono();
	canProcessReplacing();
	programsAreChunks();
	isSynth();

	for(i = 0; i < 128; i++)
		this->CreateEmptyPrg(&(this->programs[i]), i + 1);
	
	this->curProgram = 0;
	
	QueryPerformanceFrequency(&(this->perfFreq));

	cdelta = 0;
	this->mStack = new MidiStack();
	this->_Synth = new Synth(this->programs);

	this->path[0] = '\0';
	HMODULE hm = GetModuleHandle("CetoneSynth2.dll");
	if(hm)
	{
		int len = GetModuleFileName(hm, this->path, 511);
		while(len)
		{
			if(path[len] == '\\')
			{
				path[len + 1] = '\0';
				break;
			}
			len--;
		}
	}

	if(audioMaster)
		this->setEditor(new Gui(this, this->_Synth, this->GetFileName("gui.dat")));

#ifdef CS2DEBUG
	logOpen(this->GetFileName("log.txt"));
	logPrintf("*** CS2 created\n");
	logPrintf("receiveVstEvents: %d\n", this->canHostDo("receiveVstEvents"));
	logPrintf("receiveVstMidiEvent: %d\n", this->canHostDo("receiveVstMidiEvent"));
#endif

	this->LoadPresets("autoload.fxb");
	this->_Synth->SetProgram(0);
	//this->updateDisplay();
	suspend();
}

CS2::~CS2()
{
	delete this->mStack;
	delete this->_Synth;
#ifdef CS2DEBUG
	logPrintf("*** CS2 destroyed\n");
	logClose();
#endif
}

void CS2::myProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace)
{
	int i, sf = sampleFrames;
	float* ol = outputs[0];
	float* or = outputs[1];
	int p0, p1, p2, delta, tdelta = 0;

	LARGE_INTEGER ts, te;
	QueryPerformanceCounter(&ts);
	this->_Synth->InitRender();

	VstTimeInfo* t = this->getTimeInfo(kVstTempoValid);
	if(t)
	{
		if(t->flags & kVstTempoValid)
			this->_Synth->SetTempo((float)t->tempo);
	}

	while(sampleFrames > 0)
	{
		if(cdelta == 0)
		{
			while(!this->mStack->IsEmpty() && (cdelta == 0))
			{
				this->mStack->Pop(&p0, &p1, &p2, &delta);
				
				if(delta != 0)
				{
					np0 = p0;
					np1 = p1;
					np2 = p2;
					cdelta = delta;
				}
				else
					this->HandleMidi(p0, p1, p2);
			}
		}

		if(cdelta == 0)
		{
			this->_Synth->Render(sampleFrames, inputs);
			sampleFrames = 0;
		}
		else if(cdelta >= sf)
		{
			this->_Synth->Render(sampleFrames, inputs);
			sampleFrames = 0;
			cdelta -= sf;
		}
		else
		{
			int len = cdelta - tdelta;
			this->_Synth->Render(len, inputs);
			tdelta += len;
			sampleFrames -= len;
			cdelta = 0;
			this->HandleMidi(np0, np1, np2);
		}
	}

	if(replace)
	{
		for(i = 0; i < sf; i++)
		{
			ol[i] = this->_Synth->fLeft[i];
			or[i] = this->_Synth->fRight[i];
		}
	}
	else
	{
		for(i = 0; i < sf; i++)
		{
			ol[i] += this->_Synth->fLeft[i];
			or[i] += this->_Synth->fRight[i];
		}
	}

	QueryPerformanceCounter(&te);

	float v = (float)(te.QuadPart - ts.QuadPart) / (float)this->perfFreq.QuadPart;
	float z = (float)sf / this->sampleRate;
	this->_Synth->_CPU = truncate(v / z * 100.0f);
}

void CS2::process(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->myProcess(inputs, outputs, sampleFrames, false);
}

void CS2::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->myProcess(inputs, outputs, sampleFrames, true);
}

VstInt32 CS2::getVendorVersion ()
{
	return VERSION_NUMBER; 
}

bool CS2::getEffectName(char* name)
{
	vst_strncpy(name, EFFECT_NAME, kVstMaxEffectNameLen);
	return true;
}

bool CS2::getVendorString(char* text)
{
	vst_strncpy(text, VENDOR_NAME, kVstMaxVendorStrLen);
	return true;
}

bool CS2::getProductString(char* text)
{
	vst_strncpy(text, PRODUCT_NAME, kVstMaxProductStrLen);
	return true;
}

VstInt32 CS2::canDo(char *text)
{
    if(!strcmp(text, "sendVstEvents")) return 1;
    if(!strcmp(text, "sendVstMidiEvent")) return 1;
	if(!strcmp(text, "receiveVstEvents")) return 1;
	if(!strcmp (text, "receiveVstMidiEvent")) return 1;
	if(!strcmp (text, "receiveVstTimeInfo")) return 1;
	return -1;
}

void CS2::getParameterName(VstInt32 index, char *text)
{
	if(index < 0 || index >= PARAM_COUNT)
		vst_strncpy(text, "Unknown", kVstMaxParamStrLen);
	else
		vst_strncpy(text, _params[index].name, kVstMaxParamStrLen);
}

void CS2::getParameterLabel(VstInt32 index, char *label)
{
	label[0] = '\0';
}

void CS2::getParameterDisplay(VstInt32 index, char *text)
{
	if(index < 0 || index >= PARAM_COUNT)
		int2string(0, text, kVstMaxParamStrLen);
	else
		int2string(this->_Synth->GetParameter(_params[index].pval) - _params[index].voffs, text, kVstMaxParamStrLen);
}

void CS2::setParameter(VstInt32 index, float value)
{
	if(index < 0 || index >= PARAM_COUNT)
		return;
	else
		this->CCDispatcher(_params[index].ctrl, f2i(value));
}

float CS2::getParameter(VstInt32 index)
{
	if(index < 0 || index >= PARAM_COUNT)
		return 0.0f;
	else
		return i2f(this->_Synth->GetParameter(_params[index].pval) * _params[index].div);
}

void CS2::resume()
{
	//this->suspended = false;
	AudioEffectX::resume();
}

void CS2::suspend()
{
	//this->suspended = true;
	this->_Synth->Reset();
	AudioEffectX::suspend();
}

void CS2::getProgramName(char* name)
{
	vst_strncpy(name, this->programs[this->curProgram].name, kVstMaxProgNameLen);
	//this->updateDisplay();
}

bool CS2::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* name)
{
	if(index < 0 || index > 127)
		return false;

	vst_strncpy(name, this->programs[index].name, kVstMaxProgNameLen);
	return true;
}

void CS2::setProgramName(char* name)
{
	vst_strncpy(this->programs[this->curProgram].name, name, kVstMaxProgNameLen);
}

void CS2::setProgram(VstInt32 program)
{
	AudioEffectX::setProgram(program);
	this->_Synth->SetProgram(program);
	if(this->editor->isOpen())
		((Gui*)this->editor)->ReadProgram();
}

VstInt32 CS2::getChunk(void **data, bool isPreset)
{
	if(isPreset)
	{
		if(this->_Synth->HasPrgChanged())
			*data = &(this->_Synth->sPrg);
		else
			*data = &(this->programs[this->curProgram]);
		return (VstInt32)sizeof(sPRG);
	}
	else
	{
		*data = &(this->programs[0]);
		return (VstInt32)sizeof(sPRG) * 128;
	}
}

VstInt32 CS2::setChunk(void *data, VstInt32 byteSize, bool isPreset)
{
	if(isPreset)
	{
		if(byteSize == (VstInt32)sizeof(sPRG))
		{
			memcpy_s(&(this->programs[this->curProgram]), sizeof(sPRG), data, sizeof(sPRG)); 
			this->_Synth->Recall();
			if(this->editor->isOpen())
				((Gui*)this->editor)->ReadProgram();
			this->CheckPrg(this->curProgram);
			this->SetVersion(this->curProgram);
			//this->updateDisplay();
			return (VstInt32)sizeof(sPRG);
		}
	}
	else
	{
		if(byteSize == (VstInt32)sizeof(sPRG) * 128)
		{
			memcpy_s(&(this->programs[0]), sizeof(sPRG) * 128, data, sizeof(sPRG) * 128); 
			this->_Synth->Recall();
			if(this->editor->isOpen())
				((Gui*)this->editor)->ReadProgram();
			this->CheckPrg();
			this->SetVersion();
			//this->updateDisplay();
			return (VstInt32)sizeof(sPRG) * 128;
		}
	}

	return 0;
}

void CS2::setSampleRate(float sampleRate)
{
	initStatics(sampleRate);
	this->_Synth->SetSampleRate(sampleRate);
	AudioEffectX::setSampleRate(sampleRate);
	this->_Synth->ReadProgram();
}

VstInt32 CS2::processEvents(VstEvents *events)
{
	for (VstInt32 i = 0; i < events->numEvents; i++)
	{
		if ((events->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* _event = (VstMidiEvent*)events->events[i];
		char* midiData = _event->midiData;
		
		this->mStack->Push(midiData[0] & 0xff, midiData[1] & 0x7f, midiData[2] & 0x7f, _event->deltaFrames);
	}
	return 1;
}

void CS2::HandleMidi(int p0, int p1, int p2)
{
	int status = p0 & 0xf0;

	switch (status)
	{
	case 0x80:			// Note off
		this->_Synth->NoteOff(p1);
		break;
	case 0x90:			// Note on
		if (p2 > 0)
			this->_Synth->NoteOn(p1, p2);
		else
			this->_Synth->NoteOff(p1);
		break;
	case 0xb0:			// CC
		this->CCDispatcher(p1, p2);
		break;
	case 0xc0:			// Program change
		this->setProgram(p1);
		break;
	case 0xd0:			// Aftertouch
		this->_Synth->SetParameter(pAftertouch, p1);
		break;
	}
}

void CS2::setBlockSize(VstInt32 blockSize)
{ 
	this->_Synth->SetBlockSize(blockSize); 
	AudioEffectX::setBlockSize(blockSize); 
}
 
void CS2::CreateEmptyPrg(sPRG* p, int nr)
{
	int i;
	char name[kVstMaxProgNameLen];
	memset(p, 0, sizeof(sPRG));
	if(nr > -1)
		sprintf_s(name, kVstMaxProgNameLen, "CetoneSynth2 #%d", nr);
	else
		sprintf_s(name, kVstMaxProgNameLen, "CetoneSynth2", nr);
	p->version = REAL_VERSION;
	vst_strncpy(p->name, name, kVstMaxProgNameLen);

	p->osc[0].fine = 64;
	p->osc[0].semi = 64;
	p->osc[0].wave = OSCW_SAW;
	p->osc[0].mode = OSCM_NORMAL;
	p->osc[0].morph = 64;
	p->osc[0].flags = 0x80;

	p->osc[1].fine = 64;
	p->osc[1].semi = 64 - 12;
	p->osc[1].wave = OSCW_SAW;
	p->osc[1].mode = OSCM_NORMAL;
	p->osc[1].morph = 64;
	p->osc[1].flags = 0x80;

	p->osc[2].fine = 64;
	p->osc[2].semi = 64 + 12;
	p->osc[2].wave = OSCW_SAW;
	p->osc[2].mode = OSCM_NORMAL;
	p->osc[2].morph = 64;
	p->osc[2].flags = 0x80;

	p->mixer.volume = 64;
	p->mixer.panning = 64;
	p->mixer.osc1 = 64;
	p->mixer.osc2 = 64;
	p->mixer.osc3 = 64;
	p->mixer.velsens = 64;

	p->env[0].a = 10;
	p->env[0].h = 10;
	p->env[0].d = 10;
	p->env[0].s = 90;
	p->env[0].r = 40;

	p->filter.cutoff = 127;
	p->filter.velsens = 64;
	p->filter.gain = 64;
	p->filter.env2 = 64;
	p->filter.db = FILTM_12DB;
	p->filter.mode = FILTT_LP;

	for(i = 0; i < 8; i++)
		p->arp.note[i] = 64;
	p->arp.sync = 255;

	p->mod[0].amount = 64;
	p->mod[1].amount = 64;
	p->mod[2].amount = 64;
	p->mod[3].amount = 64;
	p->mod[4].amount = 64;
	p->mod[5].amount = 64;

	p->lfo[0].keytrack = 64;
	p->lfo[0].pw = 64;
	p->lfo[1].keytrack = 64;
	p->lfo[1].pw = 64;
}

char* CS2::GetFileName(char *name)
{
	if(this->path[0] == '\0')
		return NULL;
	strncpy(this->tempname, this->path, 511);
	strncat(this->tempname, name, 511);
	return this->tempname;
}

void CS2::LoadPresets(char *name)
{
	char* fn = this->GetFileName(name);
	if(fn)
	{
		FILE* f;
		fopen_s(&f, fn, "rb");
		if(f)
		{
			__int32 id;
			fread(&id, 4, 1, f);
			if(id != 0x4b6e6343) { fclose(f); return; }	// CcnK
			fread(&id, 4, 1, f); fread(&id, 4, 1, f);
			if(id != 0x68434246) { fclose(f); return; }	// FBCh
			fread(&id, 4, 1, f); fread(&id, 4, 1, f);
			if(id != 0x32435363) { fclose(f); return; }	// cSC2
			fread(&id, 4, 1, f); fread(&id, 4, 1, f);
			if(id != 0x80000000) { fclose(f); return; }	// 128 programs
			fseek(f, 0xa0, SEEK_SET);
			fread(&(this->programs[0]), sizeof(sPRG), 0x80, f);
			fclose(f);
			this->CheckPrg();
			this->SetVersion();
		}
	}
}


void CS2::GuiUpdater(int ctrl)
{
	Gui* gui = (Gui*)this->editor;
	
	switch(ctrl)
	{
	case 5:		gui->UpdateObject(pGlide);		break;
	case 7:		gui->UpdateObject(pMixVol);		break;
	case 10:	gui->UpdateObject(pMixPan);		break;
	case 12:	gui->UpdateObject(pFGain);		break;
	case 13:	gui->UpdateObject(pFEnv2);		break;
	case 14:	gui->UpdateObject(pOSemi1);		break;
	case 15:	gui->UpdateObject(pOSemi2);		break;
	case 16:	gui->UpdateObject(pOSemi3);		break;
	case 17:	gui->UpdateObject(pOFine1);		break;
	case 18:	gui->UpdateObject(pOFine2);		break;
	case 19:	gui->UpdateObject(pOFine3);		break;
	case 20:	gui->UpdateObject(pOMorph1);	break;
	case 21:	gui->UpdateObject(pOMorph2);	break;
	case 22:	gui->UpdateObject(pOMorph3);	break;
	case 23:	gui->UpdateObject(pOSpread1);	break;
	case 24:	gui->UpdateObject(pOSpread2);	break;
	case 25:	gui->UpdateObject(pOSpread3);	break;
	case 26:	gui->UpdateObject(pMixVol1);	break;
	case 27:	gui->UpdateObject(pMixVol2);	break;
	case 28:	gui->UpdateObject(pMixVol3);	break;
	case 29:	gui->UpdateObject(pAudioVol);	break;
	case 30:	gui->UpdateObject(pAudioPan);	break;
	case 44:	gui->UpdateObject(pM1Amount);	break;
	case 45:	gui->UpdateObject(pM2Amount);	break;
	case 46:	gui->UpdateObject(pM3Amount);	break;
	case 47:	gui->UpdateObject(pM4Amount);	break;
	case 48:	gui->UpdateObject(pM5Amount);	break;
	case 49:	gui->UpdateObject(pM6Amount);	break;
	case 50:	gui->UpdateObject(pArpSpeed);	break;
	case 51:	gui->UpdateObject(pArpLength);	break;
	case 52:	gui->UpdateObject(pArpNote1);	break;
	case 53:	gui->UpdateObject(pArpNote2);	break;
	case 54:	gui->UpdateObject(pArpNote3);	break;
	case 55:	gui->UpdateObject(pArpNote4);	break;
	case 56:	gui->UpdateObject(pArpNote5);	break;
	case 57:	gui->UpdateObject(pArpNote6);	break;
	case 58:	gui->UpdateObject(pArpNote7);	break;
	case 59:	gui->UpdateObject(pArpNote8);	break;
	case 71:	gui->UpdateObject(pFQ);			break;
	case 74:	gui->UpdateObject(pFCutoff);	break;
	case 80:	gui->UpdateObject(pL1Speed);	break;
	case 81:	gui->UpdateObject(pL1Delay);	break;
	case 82:	gui->UpdateObject(pL1PW);		break;
	case 83:	gui->UpdateObject(pL2Speed);	break;
	case 84:	gui->UpdateObject(pL2Delay);	break;
	case 85:	gui->UpdateObject(pL2PW);		break;
	}
}

void CS2::CCDispatcher(int ctrl, int val)
{
	switch(ctrl)
	{
	case 1:		// Modwheel
		this->_Synth->SetParameter(pModwheel, val);
		break;
	case 5:		// Glide
		this->_Synth->SetParameter(pGlide, val);
		break;
	case 7:		// Volume
		this->_Synth->SetParameter(pMixVol, val);
		break;
	case 10:	// Panning
		this->_Synth->SetParameter(pMixPan, val);
		break;
	case 12:	// Filter gain
		this->_Synth->SetParameter(pFGain, val);
		break;
	case 13:	// Filter Env2
		this->_Synth->SetParameter(pFEnv2, val);
		break;
	case 14:	// OSC1 Semi
		this->_Synth->SetParameter(pOSemi1, val);
		break;
	case 15:	// OSC2 Semi
		this->_Synth->SetParameter(pOSemi2, val);
		break;
	case 16:	// OSC3 Semi
		this->_Synth->SetParameter(pOSemi3, val);
		break;
	case 17:	// OSC1 Fine
		this->_Synth->SetParameter(pOFine1, val);
		break;
	case 18:	// OSC2 Fine
		this->_Synth->SetParameter(pOFine2, val);
		break;
	case 19:	// OSC3 Fine
		this->_Synth->SetParameter(pOFine3, val);
		break;
	case 20:	// OSC1 Morph
		this->_Synth->SetParameter(pOMorph1, val);
		break;
	case 21:	// OSC2 Morph
		this->_Synth->SetParameter(pOMorph2, val);
		break;
	case 22:	// OSC3 Morph
		this->_Synth->SetParameter(pOMorph3, val);
		break;
	case 23:	// OSC1 Spread
		this->_Synth->SetParameter(pOSpread1, val >> 2);
		break;
	case 24:	// OSC2 Spread
		this->_Synth->SetParameter(pOSpread2, val >> 2);
		break;
	case 25:	// OSC3 Spread
		this->_Synth->SetParameter(pOSpread3, val >> 2);
		break;
	case 26:	// OSC1 Volume
		this->_Synth->SetParameter(pMixVol1, val);
		break;
	case 27:	// OSC2 Volume
		this->_Synth->SetParameter(pMixVol2, val);
		break;
	case 28:	// OSC3 Volume
		this->_Synth->SetParameter(pMixVol3, val);
		break;
	case 29:	// Audio input vol
		this->_Synth->SetParameter(pAudioVol, val);
		break;
	case 30:	// Audio input balance
		this->_Synth->SetParameter(pAudioPan, val);
		break;
	case 44:	// Mod slot 1 amount
		this->_Synth->SetParameter(pM1Amount, val);
		break;
	case 45:	// Mod slot 2 amount
		this->_Synth->SetParameter(pM2Amount, val);
		break;
	case 46:	// Mod slot 3 amount
		this->_Synth->SetParameter(pM3Amount, val);
		break;
	case 47:	// Mod slot 4 amount
		this->_Synth->SetParameter(pM4Amount, val);
		break;
	case 48:	// Mod slot 5 amount
		this->_Synth->SetParameter(pM5Amount, val);
		break;
	case 49:	// Mod slot 6 amount
		this->_Synth->SetParameter(pM6Amount, val);
		break;
	case 50:	// Arp speed
		this->_Synth->SetParameter(pArpSpeed, val);
		break;
	case 51:	// Arp length
		this->_Synth->SetParameter(pArpLength, val >> 4);
		break;
	case 52:	// Arp note 1
		this->_Synth->SetParameter(pArpNote1, val);
		break;
	case 53:	// Arp note 2
		this->_Synth->SetParameter(pArpNote2, val);
		break;
	case 54:	// Arp note 3
		this->_Synth->SetParameter(pArpNote3, val);
		break;
	case 55:	// Arp note 4
		this->_Synth->SetParameter(pArpNote4, val);
		break;
	case 56:	// Arp note 5
		this->_Synth->SetParameter(pArpNote5, val);
		break;
	case 57:	// Arp note 6
		this->_Synth->SetParameter(pArpNote6, val);
		break;
	case 58:	// Arp note 7
		this->_Synth->SetParameter(pArpNote7, val);
		break;
	case 59:	// Arp note 8
		this->_Synth->SetParameter(pArpNote8, val);
		break;
	case 71:	// HARMONIC CONTENT (Resonance)
		this->_Synth->SetParameter(pFQ, val);
		break;
	case 74:	// BRIGHTNESS (Cutoff)
		this->_Synth->SetParameter(pFCutoff, val);
		break;
	case 80:	// LFO1 speed
		this->_Synth->SetParameter(pL1Speed, val);
		break;
	case 81:	// LFO1 dleay
		this->_Synth->SetParameter(pL1Delay, val);
		break;
	case 82:	// LFO1 PW
		this->_Synth->SetParameter(pL1PW, val);
		break;
	case 83:	// LFO2 speed
		this->_Synth->SetParameter(pL2Speed, val);
		break;
	case 84:	// LFO2 dleay
		this->_Synth->SetParameter(pL2Delay, val);
		break;
	case 85:	// LFO2 PW
		this->_Synth->SetParameter(pL2PW, val);
		break;
	case 123:	// All notes off
		this->_Synth->Reset();
		break;
	}

	if(this->editor->isOpen())
		this->GuiUpdater(ctrl);
}

void CS2::SetVersion()
{
	int i;
	for(i = 0; i < 128; i++)
		this->programs[i].version = REAL_VERSION;
}

void CS2::SetVersion(int i)
{
	this->programs[i].version = REAL_VERSION;
}

void CS2::CheckPrg()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		if(this->programs[i].version < REAL_VERSION)
		{
			this->programs[i].audioVol = 0;
			this->programs[i].audioPan = 64;
		}
	}
}

void CS2::CheckPrg(int i)
{
	if(this->programs[i].version < REAL_VERSION)
	{
		this->programs[i].audioVol = 0;
		this->programs[i].audioPan = 64;
	}
}
