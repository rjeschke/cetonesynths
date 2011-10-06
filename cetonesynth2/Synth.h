#pragma once

#include "defines.h"
#include "svoice.h"

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

class Synth
{
public:
	Synth(sPRG* ps);
	~Synth();
	void InitRender();
	void Render(int frames, float **inputs);
	void SetBlockSize(int size);
	void SetSampleRate(float fs);
	void NoteOn(int note, int vel);
	void NoteOff(int note);
	void Reset();

	void SetParameter(int id, int val);
	int GetParameter(int id);
	void NotifyVoices(int id, int val);

	void SetGuiFlag(bool flag) { this->guiOpen = flag; };

	void ChangeVoices(int nr);
	int GetVoices();
	int GetLFOPos(int lfo);

	void SetTempo(float bpm);

	void SetProgram(int prg);
	void Store();
	void Recall();
	bool HasPrgChanged();
	void ReadProgram();

	float* fRight;
	float* fLeft;
	sPRG sPrg;
	int _CPU;
private:
	void VoiceOn(int v, int note, int vel);
	float spq, FS, oBpm;
	SVoice* Voices[16];
	int iBlockSize, lastNote;
	int midiMap[128];
	int iFramePos;
	int curProgram;
	sPRG* programs;
	LFO* rlfo[2];
	bool guiOpen;
};
