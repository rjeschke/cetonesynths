#pragma once

#include <windows.h>
#include "aeffeditor.h"
#include "audioeffectx.h"
#include "Synth.h"

class CS2 : public AudioEffectX
{
public:
	CS2(audioMasterCallback audioMaster);
	~CS2();
	void		process(float **inputs, float **outputs, VstInt32 sampleFrames); 	
	void		processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);
	VstInt32	getVendorVersion ();
	bool		getEffectName(char* name);
	bool		getVendorString(char* text);
	bool		getProductString(char* text);
	void		setSampleRate(float sampleRate);
	void		setBlockSize(VstInt32 blockSize);
	void		setBlockSizeAndSampleRate(VstInt32 blockSize, float sampleRate) { this->setSampleRate(sampleRate); this->setBlockSize(blockSize); }
	VstInt32	getNumMidiInputChannels() { return 1; };
	VstInt32	getNumMidiOutputChannels() { return 1; };
	VstInt32	canDo(char* text);
	VstInt32	processEvents(VstEvents *events);
	void		HandleMidi(int p0, int p1, int p2);

	void		setParameter(VstInt32 index, float value);
	float		getParameter(VstInt32 index);
	void		getParameterLabel(VstInt32 index, char* label);
	void		getParameterDisplay(VstInt32 index, char* text);
	void		getParameterName(VstInt32 index, char* text);
	void		getProgramName(char* name);
	bool		getProgramNameIndexed(VstInt32 category, VstInt32 index, char* name);
	void		setProgramName(char* name);
	void		setProgram(VstInt32 program);
	VstInt32	getChunk(void **data, bool isPreset = false);
	VstInt32	setChunk(void *data, VstInt32 byteSize, bool isPreset = false);
	void		CreateEmptyPrg(sPRG* p, int nr = -1);
	void		resume();
	void		suspend();
	void		LoadPresets(char* name);
	char*		GetFileName(char* name);
	void		GuiUpdater(int ctrl);
	void		CCDispatcher(int ctrl, int val);
private:
	void		CheckPrg();
	void		CheckPrg(int i);
	void		SetVersion();
	void		SetVersion(int i);
	char		path[512];
	char		tempname[512];
	LARGE_INTEGER perfFreq;
	void		myProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace); 	
	int			np0, np1, np2, cdelta;
	Synth*		_Synth;
	MidiStack*	mStack;
	sPRG		programs[128];
	sPRG		emptyPrg;
};
