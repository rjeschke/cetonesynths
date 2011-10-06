#include <windows.h>
#include "CetoneOrg.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

#pragma warning(disable: 4996)

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new CO(audioMaster);
}

CO::CO(audioMasterCallback audioMaster)
: AudioEffectX(audioMaster, 128, pNumParameters)
{
	int i;
	setNumOutputs(2);
	setUniqueID('cOrG');
	canProcessReplacing();
	programsAreChunks();
	isSynth();

	this->mStack = new MidiStack();
	this->_Synth = new Synth();

	cdelta = 0;

	this->path[0] = '\0';
	HMODULE hm = GetModuleHandle("CetoneOrg.dll");
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

	for(i = 0; i < 128; i++)
		this->CreateEmptyPrg(&(this->_prg[i]), i + 1);
	
	this->curProgram = 0;
	this->LoadPresets("autoload.fxb");
	this->ReadProgram();

	suspend();
}

CO::~CO()
{
	delete this->mStack;
	delete this->_Synth;
}

VstInt32 CO::getVendorVersion ()
{
	return VERSION_NUMBER; 
}

bool CO::getEffectName(char* name)
{
	vst_strncpy(name, EFFECT_NAME, kVstMaxEffectNameLen);
	return true;
}

bool CO::getVendorString(char* text)
{
	vst_strncpy(text, VENDOR_NAME, kVstMaxVendorStrLen);
	return true;
}

bool CO::getProductString(char* text)
{
	vst_strncpy(text, PRODUCT_NAME, kVstMaxProductStrLen);
	return true;
}

VstInt32 CO::canDo(char *text)
{
	if(!strcmp(text, "receiveVstEvents")) return 1;
	if(!strcmp (text, "receiveVstMidiEvent")) return 1;
	return -1;
}

void CO::setSampleRate(float sampleRate)
{
	initStatics(sampleRate);
	this->_Synth->setSampleRate(sampleRate);
	AudioEffectX::setSampleRate(sampleRate);
}

void CO::setBlockSize(VstInt32 blockSize)
{
	this->_Synth->setBlockSize(blockSize);
	AudioEffectX::setBlockSize(blockSize);
}

VstInt32 CO::processEvents(VstEvents *events)
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

void CO::HandleMidi(int p0, int p1, int p2)
{
	int status = p0 & 0xf0;

	switch (status)
	{
	case 0x80:			// Note off
		p1 -= 24;
		if(p1 >= 0 && p1 < 61)
			this->_Synth->NoteOff(p1);
		break;
	case 0x90:			// Note on
		p1 -= 24;
		if(p1 >= 0 && p1 < 61)
		{
			if (p2 > 0)
				this->_Synth->NoteOn(p1, p2);
			else
				this->_Synth->NoteOff(p1);
		}
		break;
	case 0xc0:			// Program change
		this->setProgram(p1);
		break;
	}
}


void CO::myProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace)
{
	int i, sf = sampleFrames;
	float* ol = outputs[0];
	float* or = outputs[1];
	int p0, p1, p2, delta, tdelta = 0;

	this->_Synth->InitRender();

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
			this->_Synth->Render(sampleFrames);
			sampleFrames = 0;
		}
		else if(cdelta >= sf)
		{
			this->_Synth->Render(sampleFrames);
			sampleFrames = 0;
			cdelta -= sf;
		}
		else
		{
			int len = cdelta - tdelta;
			this->_Synth->Render(len);
			tdelta += len;
			sampleFrames -= len;
			cdelta = 0;
			this->HandleMidi(np0, np1, np2);
		}
	}

	this->_Synth->FinishRender(sf);

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
}

void CO::process(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->myProcess(inputs, outputs, sampleFrames, false);
}

void CO::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	this->myProcess(inputs, outputs, sampleFrames, true);
}

#define TOINT(v,x) (truncate(v * (float)x))
#define FROMINT(v,x) ((float)(v) / (float)x)

void CO::setParameter(VstInt32 index, float val)
{
	switch (index)
	{
	default:		return;
	case pDb0:		
		this->_prg[this->curProgram].bars[0] = TOINT(val, 8);
		this->_Synth->setDrawbars(0, this->_prg[this->curProgram].bars[0]);
		break;
	case pDb1:		
		this->_prg[this->curProgram].bars[1] = TOINT(val, 8);
		this->_Synth->setDrawbars(1, this->_prg[this->curProgram].bars[1]);
		break;
	case pDb2:		
		this->_prg[this->curProgram].bars[2] = TOINT(val, 8);
		this->_Synth->setDrawbars(2, this->_prg[this->curProgram].bars[2]);
		break;
	case pDb3:		
		this->_prg[this->curProgram].bars[3] = TOINT(val, 8);
		this->_Synth->setDrawbars(3, this->_prg[this->curProgram].bars[3]);
		break;
	case pDb4:		
		this->_prg[this->curProgram].bars[4] = TOINT(val, 8);
		this->_Synth->setDrawbars(4, this->_prg[this->curProgram].bars[4]);
		break;
	case pDb5:		
		this->_prg[this->curProgram].bars[5] = TOINT(val, 8);
		this->_Synth->setDrawbars(5, this->_prg[this->curProgram].bars[5]);
		break;
	case pDb6:		
		this->_prg[this->curProgram].bars[6] = TOINT(val, 8);
		this->_Synth->setDrawbars(6, this->_prg[this->curProgram].bars[6]);
		break;
	case pDb7:		
		this->_prg[this->curProgram].bars[7] = TOINT(val, 8);
		this->_Synth->setDrawbars(7, this->_prg[this->curProgram].bars[7]);
		break;
	case pDb8:		
		this->_prg[this->curProgram].bars[8] = TOINT(val, 8);
		this->_Synth->setDrawbars(8, this->_prg[this->curProgram].bars[8]);
		break;
	case pTone:
		this->_prg[this->curProgram].tone = TOINT(val, 127);
		this->_Synth->setTone(this->_prg[this->curProgram].tone);
		break;
	case pClick:
		this->_prg[this->curProgram].click = TOINT(val, 127);
		this->_Synth->setClickAmount(this->_prg[this->curProgram].click);
		break;
	case pVolume:
		this->_prg[this->curProgram].volume = TOINT(val, 127);
		this->_Synth->setVolume(this->_prg[this->curProgram].volume);
		break;
	case pLfoSpeed:
		this->_prg[this->curProgram].lfoSpeed = TOINT(val, 127);
		this->_Synth->setLfoSpeed(this->_prg[this->curProgram].lfoSpeed);
		break;
	case pVibDepth:
		this->_prg[this->curProgram].vibDepth = TOINT(val, 127);
		this->_Synth->setVibDepth(this->_prg[this->curProgram].vibDepth);
		break;
	case pPanPhase:
		this->_prg[this->curProgram].panPhase = TOINT(val, 127);
		this->_Synth->setLfoPhase(this->_prg[this->curProgram].panPhase);
		break;
	case pPanDepth:
		this->_prg[this->curProgram].panDepth = TOINT(val, 127);
		this->_Synth->setPanDepth(this->_prg[this->curProgram].panDepth);
		break;
	case pChDelayL:
		this->_prg[this->curProgram].c_delayl = TOINT(val, 127);
		this->_Synth->setChorusDelayL(this->_prg[this->curProgram].c_delayl);
		break;
	case pChDelayR:
		this->_prg[this->curProgram].c_delayr = TOINT(val, 127);
		this->_Synth->setChorusDelayL(this->_prg[this->curProgram].c_delayr);
		break;
	case pChDepth:
		this->_prg[this->curProgram].c_depth = TOINT(val, 127);
		this->_Synth->setChorusDepth(this->_prg[this->curProgram].c_depth);
		break;
	case pChSpeed:
		this->_prg[this->curProgram].c_speed = TOINT(val, 127);
		this->_Synth->setChorusSpeed(this->_prg[this->curProgram].c_speed);
		break;
	case pChFeedback:
		this->_prg[this->curProgram].c_feedback = TOINT(val, 127);
		this->_Synth->setChorusFeedback(this->_prg[this->curProgram].c_feedback);
		break;
	case pChMix:
		this->_prg[this->curProgram].c_mix = TOINT(val, 127);
		this->_Synth->setChorusMix(this->_prg[this->curProgram].c_mix);
		break;
	case pRevDamp:
		this->_prg[this->curProgram].r_damp = TOINT(val, 127);
		this->_Synth->setRevDamp(this->_prg[this->curProgram].r_damp);
		break;
	case pRevWidth:
		this->_prg[this->curProgram].r_width = TOINT(val, 127);
		this->_Synth->setRevWidth(this->_prg[this->curProgram].r_width);
		break;
	case pRevRoom:
		this->_prg[this->curProgram].r_room = TOINT(val, 127);
		this->_Synth->setRevRoom(this->_prg[this->curProgram].r_room);
		break;
	case pRevDry:
		this->_prg[this->curProgram].r_dry = TOINT(val, 127);
		this->_Synth->setRevDry(this->_prg[this->curProgram].r_dry);
		break;
	case pRevWet:
		this->_prg[this->curProgram].r_wet = TOINT(val, 127);
		this->_Synth->setRevWet(this->_prg[this->curProgram].r_wet);
		break;
	case pOutVol:
		this->_prg[this->curProgram].outvol = TOINT(val, 127);
		this->_Synth->setOutVol(this->_prg[this->curProgram].outvol);
		break;
	}
}

float CO::getParameter(VstInt32 index)
{
	switch (index)
	{
	default:		return 0.0f;
	case pDb0:		return FROMINT(this->_prg[this->curProgram].bars[0], 8);
	case pDb1:		return FROMINT(this->_prg[this->curProgram].bars[1], 8);
	case pDb2:		return FROMINT(this->_prg[this->curProgram].bars[2], 8);
	case pDb3:		return FROMINT(this->_prg[this->curProgram].bars[3], 8);
	case pDb4:		return FROMINT(this->_prg[this->curProgram].bars[4], 8);
	case pDb5:		return FROMINT(this->_prg[this->curProgram].bars[5], 8);
	case pDb6:		return FROMINT(this->_prg[this->curProgram].bars[6], 8);
	case pDb7:		return FROMINT(this->_prg[this->curProgram].bars[7], 8);
	case pDb8:		return FROMINT(this->_prg[this->curProgram].bars[8], 8);
	case pTone:		return FROMINT(this->_prg[this->curProgram].tone, 127);
	case pVolume:	return FROMINT(this->_prg[this->curProgram].volume, 127);
	case pLfoSpeed:	return FROMINT(this->_prg[this->curProgram].lfoSpeed, 127);
	case pVibDepth:	return FROMINT(this->_prg[this->curProgram].vibDepth, 127);
	case pPanPhase:	return FROMINT(this->_prg[this->curProgram].panPhase, 127);
	case pPanDepth:	return FROMINT(this->_prg[this->curProgram].panDepth, 127);
	case pClick:	return FROMINT(this->_prg[this->curProgram].click, 127);
	case pChDelayL:	return FROMINT(this->_prg[this->curProgram].c_delayl, 127);
	case pChDelayR:	return FROMINT(this->_prg[this->curProgram].c_delayr, 127);
	case pChDepth:	return FROMINT(this->_prg[this->curProgram].c_depth, 127);
	case pChSpeed:	return FROMINT(this->_prg[this->curProgram].c_speed, 127);
	case pChFeedback:	return FROMINT(this->_prg[this->curProgram].c_feedback, 127);
	case pChMix:	return FROMINT(this->_prg[this->curProgram].c_mix, 127);
	case pRevDamp:	return FROMINT(this->_prg[this->curProgram].r_damp, 127);
	case pRevWidth:	return FROMINT(this->_prg[this->curProgram].r_width, 127);
	case pRevRoom:	return FROMINT(this->_prg[this->curProgram].r_room, 127);
	case pRevWet:	return FROMINT(this->_prg[this->curProgram].r_wet, 127);
	case pRevDry:	return FROMINT(this->_prg[this->curProgram].r_dry, 127);
	case pOutVol:	return FROMINT(this->_prg[this->curProgram].outvol, 127);
	}
}

void CO::getParameterLabel(VstInt32 index, char* label)
{
	label[0] = '\0';
}

void CO::getParameterDisplay(VstInt32 index, char* text)
{
	text[0] = '\0';
}

void CO::getParameterName(VstInt32 index, char* text)
{
	switch (index)
	{
	default:		vst_strncpy(text, "Unknown", kVstMaxParamStrLen);	break;
	case pDb0:		vst_strncpy(text, "16'", kVstMaxParamStrLen);	break;
	case pDb1:		vst_strncpy(text, "5 1/3'", kVstMaxParamStrLen);	break;
	case pDb2:		vst_strncpy(text, "8'", kVstMaxParamStrLen);	break;
	case pDb3:		vst_strncpy(text, "4'", kVstMaxParamStrLen);	break;
	case pDb4:		vst_strncpy(text, "2 2/3'", kVstMaxParamStrLen);	break;
	case pDb5:		vst_strncpy(text, "2'", kVstMaxParamStrLen);	break;
	case pDb6:		vst_strncpy(text, "1 3/5'", kVstMaxParamStrLen);	break;
	case pDb7:		vst_strncpy(text, "1 1/3'", kVstMaxParamStrLen);	break;
	case pDb8:		vst_strncpy(text, "1'", kVstMaxParamStrLen);	break;
	case pTone:		vst_strncpy(text, "Tone", kVstMaxParamStrLen);	break;
	case pClick:	vst_strncpy(text, "Keyclick", kVstMaxParamStrLen);	break;
	case pVolume:	vst_strncpy(text, "Volume", kVstMaxParamStrLen);	break;
	case pLfoSpeed:	vst_strncpy(text, "LfoSpeed", kVstMaxParamStrLen);	break;
	case pVibDepth:	vst_strncpy(text, "Vibrato", kVstMaxParamStrLen);	break;
	case pPanPhase:	vst_strncpy(text, "PanPhase", kVstMaxParamStrLen);	break;
	case pPanDepth:	vst_strncpy(text, "PanDepth", kVstMaxParamStrLen);	break;
	case pChDelayL:	vst_strncpy(text, "ChDelayL", kVstMaxParamStrLen);	break;
	case pChDelayR:	vst_strncpy(text, "ChDelayR", kVstMaxParamStrLen);	break;
	case pChDepth:	vst_strncpy(text, "ChDepth", kVstMaxParamStrLen);	break;
	case pChSpeed:	vst_strncpy(text, "ChSpeed", kVstMaxParamStrLen);	break;
	case pChFeedback:	vst_strncpy(text, "ChFeedbk", kVstMaxParamStrLen);	break;
	case pChMix:	vst_strncpy(text, "ChMix", kVstMaxParamStrLen);	break;
	case pRevRoom:	vst_strncpy(text, "RevRoom", kVstMaxParamStrLen);	break;
	case pRevWidth:	vst_strncpy(text, "RevWidth", kVstMaxParamStrLen);	break;
	case pRevDamp:	vst_strncpy(text, "RevDamp", kVstMaxParamStrLen);	break;
	case pRevWet:	vst_strncpy(text, "RevWet", kVstMaxParamStrLen);	break;
	case pRevDry:	vst_strncpy(text, "RevDry", kVstMaxParamStrLen);	break;
	case pOutVol:	vst_strncpy(text, "OutVol", kVstMaxParamStrLen);	break;
	}
}

void CO::ReadProgram()
{
	PROGRAM *p = &(this->_prg[this->curProgram]);

	this->_Synth->setVolume(p->volume);
	this->_Synth->setDrawbars(0, p->bars[0]);
	this->_Synth->setDrawbars(1, p->bars[1]);
	this->_Synth->setDrawbars(2, p->bars[2]);
	this->_Synth->setDrawbars(3, p->bars[3]);
	this->_Synth->setDrawbars(4, p->bars[4]);
	this->_Synth->setDrawbars(5, p->bars[5]);
	this->_Synth->setDrawbars(6, p->bars[6]);
	this->_Synth->setDrawbars(7, p->bars[7]);
	this->_Synth->setDrawbars(8, p->bars[8]);
	this->_Synth->setClickAmount(p->click);
	this->_Synth->setTone(p->tone);

	this->_Synth->setVibDepth(p->vibDepth);
	this->_Synth->setLfoSpeed(p->lfoSpeed);
	this->_Synth->setLfoPhase(p->panPhase);
	this->_Synth->setPanDepth(p->panDepth);

	this->_Synth->setChorusDelayL(p->c_delayl);
	this->_Synth->setChorusDelayR(p->c_delayr);
	this->_Synth->setChorusDepth(p->c_depth);
	this->_Synth->setChorusSpeed(p->c_speed);
	this->_Synth->setChorusMix(p->c_mix);
	this->_Synth->setChorusFeedback(p->c_feedback);

	this->_Synth->setRevRoom(p->r_room);
	this->_Synth->setRevDamp(p->r_damp);
	this->_Synth->setRevWidth(p->r_width);
	this->_Synth->setRevWet(p->r_wet);
	this->_Synth->setRevDry(p->r_dry);

	this->_Synth->setOutVol(p->outvol);

	this->_Synth->Reset();
}

void CO::SetVersion()
{
	int i;
	for(i = 0; i < 128; i++)
		this->_prg[i].version = REAL_VERSION;
}

void CO::SetVersion(int i)
{
	this->_prg[i].version = REAL_VERSION;
}

char* CO::GetFileName(char *name)
{
	if(this->path[0] == '\0')
		return NULL;
	strncpy(this->tempname, this->path, 511);
	strncat(this->tempname, name, 511);
	return this->tempname;
}

void CO::LoadPresets(char *name)
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
			if(id != 0x47724f63) { fclose(f); return; }	// cOrG
			fread(&id, 4, 1, f); fread(&id, 4, 1, f);
			if(id != 0x80000000) { fclose(f); return; }	// 128 programs
			fseek(f, 0xa0, SEEK_SET);
			fread(&(this->_prg[0]), sizeof(PROGRAM), 0x80, f);
			fclose(f);
			this->SetVersion();
		}
	}
}

void CO::getProgramName(char* name)
{
	vst_strncpy(name, this->_prg[this->curProgram].name, kVstMaxProgNameLen);
	//this->updateDisplay();
}

bool CO::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* name)
{
	if(index < 0 || index > 127)
		return false;

	vst_strncpy(name, this->_prg[index].name, kVstMaxProgNameLen);
	return true;
}

void CO::setProgramName(char* name)
{
	vst_strncpy(this->_prg[this->curProgram].name, name, kVstMaxProgNameLen);
}

void CO::setProgram(VstInt32 program)
{
	AudioEffectX::setProgram(program);
	this->ReadProgram();
}

VstInt32 CO::getChunk(void **data, bool isPreset)
{
	if(isPreset)
	{
		*data = &(this->_prg[this->curProgram]);
		return (VstInt32)sizeof(PROGRAM);
	}
	else
	{
		*data = &(this->_prg[0]);
		return (VstInt32)sizeof(PROGRAM) * 128;
	}
}

VstInt32 CO::setChunk(void *data, VstInt32 byteSize, bool isPreset)
{
	if(isPreset)
	{
		if(byteSize == (VstInt32)sizeof(PROGRAM))
		{
			memcpy_s(&(this->_prg[this->curProgram]), sizeof(PROGRAM), data, sizeof(PROGRAM)); 
			this->SetVersion(this->curProgram);
			this->ReadProgram();
			return (VstInt32)sizeof(PROGRAM);
		}
	}
	else
	{
		if(byteSize == (VstInt32)sizeof(PROGRAM) * 128)
		{
			memcpy_s(&(this->_prg[0]), sizeof(PROGRAM) * 128, data, sizeof(PROGRAM) * 128); 
			this->SetVersion();
			this->ReadProgram();
			return (VstInt32)sizeof(PROGRAM) * 128;
		}
	}

	return 0;
}

void CO::CreateEmptyPrg(PROGRAM* p, int nr)
{
	char name[kVstMaxProgNameLen];
	memset(p, 0, sizeof(PROGRAM));
	if(nr > -1)
		sprintf_s(name, kVstMaxProgNameLen, "CetoneOrg #%d", nr);
	else
		sprintf_s(name, kVstMaxProgNameLen, "CetoneOrg", nr);
	p->version = REAL_VERSION;
	vst_strncpy(p->name, name, kVstMaxProgNameLen);

	p->bars[0] = 7;
	p->bars[1] = 7;
	p->bars[2] = 7;
	p->bars[3] = 7;
	p->bars[4] = 7;
	p->bars[5] = 7;
	p->bars[6] = 7;
	p->bars[7] = 7;
	p->bars[8] = 7;
	p->volume = 32;
	p->click = 127;
	p->tone = 127;
	p->lfoSpeed = 64;
	p->panDepth = 48;
	p->vibDepth = 32;
	p->c_delayl = 34;
	p->c_delayr = 39;
	p->c_depth = 20;
	p->c_speed = 30;
	p->c_mix = 48;
	p->c_feedback = 64;
	p->r_damp = 64;
	p->r_room = 96;
	p->r_width = 64;
	p->r_wet = 16;
	p->r_dry = 64;
	p->outvol = 80;
}

#pragma warning(default: 4996)
