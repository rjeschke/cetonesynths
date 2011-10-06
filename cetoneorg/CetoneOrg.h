#pragma once

#include "defines.h"
#include "audioeffectx.h"
#include "Synth.h"

class MidiStack
{
public:
	MidiStack()
	{
		this->StackLow = 0;
		this->StackHigh = 0;
	};
	~MidiStack() { };

	bool IsEmpty()
	{
		return (this->StackLow == this->StackHigh) ? true : false;
	};

	void Push(int p0, int p1, int p2, int delta)
	{
		this->P0[this->StackHigh] = p0;
		this->P1[this->StackHigh] = p1;
		this->P2[this->StackHigh] = p2;
		this->Delta[this->StackHigh] = delta;

		this->StackHigh++;
		this->StackHigh &= 4095;
	};
	
	void Pop(int* p0, int* p1, int* p2, int* delta)
	{
		*p0 = this->P0[this->StackLow];
		*p1 = this->P1[this->StackLow];
		*p2 = this->P2[this->StackLow];
		*delta = this->Delta[this->StackLow];

		this->StackLow++;
		this->StackLow &= 4095;
	};
private:
	int P0[4096];
	int P1[4096];
	int P2[4096];
	int Delta[4096];
	int StackLow;
	int StackHigh;
};

class CO : public AudioEffectX
{
public:
	CO(audioMasterCallback audioMaster);
	~CO();
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
	VstInt32	getNumMidiOutputChannels() { return 0; };
	VstInt32	canDo(char* text);
	VstInt32	processEvents(VstEvents *events);

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
	void		suspend() { AudioEffectX::suspend(); };
	void		resume() { AudioEffectX::resume(); };

private:
	void		CreateEmptyPrg(PROGRAM* p, int nr = -1);
	void		LoadPresets(char* name);
	char*		GetFileName(char* name);
	void		SetVersion();
	void		SetVersion(int i);
	void		ReadProgram();
	void		HandleMidi(int p0, int p1, int p2);
	void		myProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace); 	
	int			np0, np1, np2, cdelta;
	Synth*		_Synth;
	MidiStack*	mStack;
	PROGRAM		_prg[128];
	char		path[512];
	char		tempname[512];

};
