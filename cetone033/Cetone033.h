#pragma once

#include "audioeffectx.h"

#include "defines.h"
#include "structures.h"
#include "globalfunctions.h"
#include "midistack.h"
#include "synthoscillator.h"

#include "cetonelpfilter.h"

class CCetone033 : public AudioEffectX
{
public:
	CCetone033(audioMasterCallback audioMaster);
	~CCetone033(void);

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

	static float		SampleRate;
	static float		SampleRate2;
	static float		SampleRateEnv;
	static float		SampleRateVel;
	static float		Pi;

	static float		SmallSineTable[WAVETABLE_LENGTH];
	static float		FreqTable[PITCH_MAX];
	static int			FreqTableInt[PITCH_MAX];

	static int			LookupTable[65536];
	static float		SawTable[NOTE_MAX * WAVETABLE_LENGTH];
	static float		ParabolaTable[NOTE_MAX * WAVETABLE_LENGTH];
	static int			FreqStepInt[PITCH_MAX];
	static int			FreqStepFrac[PITCH_MAX];

	static float		Int2FloatTab[65536];
	static float		Int2FloatTab2[65536];

private:
	CMidiStack*			MidiStack;
	CCetoneLpFilter*	Filter;

	CSynthOscillator*	Oscs[2];

	SynthProgram		Programs[128];
	SynthProgramOld		OldPrograms[128];

	int					CurrentNote;
	int					CurrentVelocity;
	int					CurrentDelta;
	int					CurrentProgram;
	int					CurrentPitch;
	
	float				VelocityMod;
	float				VelocityModStep;
	float				VelocityModEnd;

	int					LastP0;
	int					NextP0;
	int					NextP1;
	int					NextP2;

	int					FilterCounter;
	
	float				GlideSamples;

	// Program

	int					Coarse[2];
	int					Fine[2];
	int					Wave[2];
	float				Morph[2];
	float				Volume[2];

	float				Attack[2];
	float				Decay[2];

	float				ModEnv;
	float				ModVel;
	float				ModRes;

	float				Cutoff;
	float				Resonance;
	int					FilterType;

	bool				GlideState;
	float				GlideSpeed;

	bool				ClipState;
	float				MainVolume;

	float				CutoffDest;
	float				CutoffStep;

	float				ResonanceDest;
	float				ResonanceStep;

	//

	float				AttackFactor[2];
	float				DecayFactor[2];
	float				VoiceVolume[2];
	int					EnvPos[2];
	float				DecayResonance;

	bool				DoGlide;
	int					GlidePitch;
	int					GlideStep;
	int					GlideFrac;
	float				ModChangeSamples;
	float				ModResValue;

	void				ReadProgram(int prg);
	void				WriteProgram(int prg);

	void				SynthProcess(float **inputs, float **outputs, VstInt32 sampleFrames, bool replace);
	void				HandleMidi(int p0, int p1, int p2);

	void				NoteOn(int note, int vel);
	void				NoteOff(int note, int vel);
	void				InitFreqTables(float fs);
	void				InitParameters();
	void				UpdateEnvelopes();
	void				SetGlideSpeed(float speed);
	void				SetGlideState(bool state);
	void				SetModRes(float value);

	void				SetCutoffSave(float value);
	void				SetResonanceSave(float value);

	void				ImportProgram(SynthProgramOld* src, SynthProgram* dest);
};
