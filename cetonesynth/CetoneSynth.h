#pragma once
#pragma warning (disable: 4996)

#include "audioeffectx.h"

#include "defines.h"
#include "structures.h"
#include "globalfunctions.h"

#include "synthoscillator.h"
#include "synthenvelope.h"
#include "synthlfo.h"

#include "filtermoog.h"
#include "filtermoog2.h"
#include "filterdirty.h"
#include "filterch12db.h"
#include "filter303.h"
#include "filter8580.h"
#include "filterbutterworth24db.h"
#include "midistack.h"

class CCetoneSynth : public AudioEffectX
{
public:
	CCetoneSynth(audioMasterCallback audioMaster);
	~CCetoneSynth(void);

	// Plugin interface

	virtual void		resume();

	virtual VstInt32	processEvents(VstEvents* events);
	virtual void		process(float **inputs, float **outputs, VstInt32 sampleFrames); 	
	virtual void		processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);

	virtual VstInt32	getChunk(void** data, bool isPreset = false); 
	virtual VstInt32	setChunk(void* data, VstInt32 byteSize, bool isPreset = false);

	virtual void		setProgram(VstInt32 program);
	virtual VstInt32	getProgram();
	virtual void		setProgramName(char* name);
	virtual void		getProgramName(char* name);
	virtual bool		getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text);

	virtual VstInt32	getNumMidiInputChannels();
	virtual VstInt32	getNumMidiOutputChannels();

	virtual void		setSampleRate (float sampleRate);
	virtual void		setBlockSize (VstInt32 blockSize);

	virtual VstInt32	getVendorVersion ();
	virtual bool		getEffectName(char* name);
	virtual bool		getVendorString(char* text);
	virtual bool		getProductString(char* text);

	virtual VstInt32	canDo(char* text);

	virtual void		setParameter(VstInt32 index, float value);
	virtual float		getParameter(VstInt32 index);
	virtual void		getParameterLabel(VstInt32 index, char* label);
	virtual void		getParameterDisplay(VstInt32 index, char* text);
	virtual void		getParameterName(VstInt32 index, char* text);

	// Public statics

	static float		SineTable[65536]; 
	static float		SmallSineTable[WAVETABLE_LENGTH];
	static float		FreqTable[PITCH_MAX];
	static int			FreqTableInt[PITCH_MAX];

	static int			LookupTable[65536];
	static float		SawTable[NOTE_MAX * WAVETABLE_LENGTH];
	static float		ParabolaTable[NOTE_MAX * WAVETABLE_LENGTH];
	static int			FreqStepInt[PITCH_MAX];
	static int			FreqStepFrac[PITCH_MAX];

	static float		Pw2Float[4096];
	static int			Pw2Offset[4096];
	static float		Pw2WaveOffset[4096];

	static float		Int2FloatTab[65536];
	static float		Int2FloatTab2[65536];

	static float		SampleRate;
	static float		SampleRate2;
	static float		SampleRate_1;
	static float		SampleRate2_1;
	static float		SampleRatePi;
	static float		Pi;
	static int			C64Arps[8][16];

private:

	// Classes

	CSynthOscillator*	Oscs[4];
	CSynthEnvelope*		Envs[3];
	CSynthLfo*			Lfos[2];

	CMidiStack*			MidiStack;
	CFilterDirty*		FilterDirty;
	CFilterMoog*		FilterMoog;
	CFilterMoog2*		FilterMoog2;
	CFilterCh12db*		FilterCh12db;
	CFilter303*			Filter303;
	CFilter8580*		Filter8580;
	CFilterButterworth24db*		FilterBuddawert;

	// Variables

	int					MidiChannel;

	SynthProgram		Programs[129];

	// Program variables

	float				Volume;
	float				Panning;

	int					MainCoarse;
	int					MainFine;

	int					FilterType;
	int					FilterMode;
	float				Cutoff;
	float				Resonance;

	int					ArpMode;
	int					ArpSpeed;

	bool				PortaMode;
	float				PortaSpeed;

	SynthVoice			Voice[4];

	float				EnvAttack[3];
	float				EnvHold[3];
	float				EnvDecay[3];
	float				EnvSustain[3];
	float				EnvRelease[3];

	float				LfoSpeed[2];
	int					LfoWave[2];
	int					LfoPw[2];
	bool				LfoTrigger[2];
	
	SynthModulation		Modulations[8];

	float				EnvMod;

	// Runtime variables

	float				ModChangeSamples;

	int					CurrentNote;
	int					CurrentVelocity;
	int					CurrentDelta;
	int					CurrentCtrl1;
	int					CurrentProgram;
	
	float				VelocityMod;
	float				Ctrl1Mod;

	float				VelocityModStep;
	float				Ctrl1ModStep;

	float				VelocityModEnd;
	float				Ctrl1ModEnd;

	int					LastP0;
	int					NextP0;
	int					NextP1;
	int					NextP2;

	int					FilterCounter;

	int					ArpPos;
	int					ArpDelay;
	int					ArpCounter;

	int					CurrentPitch;
	int					PortaStep;
	int					PortaPitch;
	int					PortaFrac;
	float				PortaSamples;
	bool				DoPorta;

	float				CutoffDest;
	float				CutoffStep;

    int					VoicePulsewidth[4];
	
	float				LfoPitch[2];

	// Init functions
	
	void				InitSineTable();
	void				InitFreqTable();
	void				InitParameters();

	// Programs

	void				ReadProgram(int prg);
	void				WriteProgram(int prg);

	// Synthesizer functions	(CetoneSynthMain.cpp)

	void				SynthProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace);
	void				Run(float* left, float* right);
	void				HandleMidi(int p0, int p1, int p2);

	void				NoteOn(int note, int vel);
	void				NoteOff(int note, int vel);

	void				TriggerFilterCounter();
	void				UpdateFilters();
	void				UpdateFilters(float cutoff, float q, float mode);
	void				SetFilterMode(int mode);

	void				SetArpSpeed(int ms);
	void				SetPortaSpeed(float speed);
	void				UpdateEnvelopes();
	void				SetCutoffSave(float value);
};
