#pragma once

enum
{
	pOsc1Coarse,
	pOsc1Fine,
	pOsc1Wave,
	pOsc1Morph,
	pOsc1Volume,

	pOsc2Coarse,
	pOsc2Fine,
	pOsc2Wave,
	pOsc2Morph,
	pOsc2Volume,

	pEnv1Attack,
	pEnv1Decay,

	pEnv2Attack,
	pEnv2Decay,

	pModEnv,
	pModVel,
	pModRes,

	pCutoff,
	pResonance,

	pGlideState,
	pGlideSpeed,

	pClipState,
	pVolume,

	pFilterType,

	pParameters
};

struct SynthProgramOld
{
	char Name[128];

	int Coarse[2];
	int Fine[2];
	int Wave[2];
	float Morph[2];
	float Volume[2];

	float Attack[2];
	float Decay[2];

	float ModEnv;
	float ModVel;
	float ModRes;

	float Cutoff;
	float Resonance;

	bool GlideState;
	float GlideSpeed;

	bool ClipState;
	float MainVolume;
};

struct SynthProgram
{
	char Name[128];

	int Coarse[2];
	int Fine[2];
	int Wave[2];
	float Morph[2];
	float Volume[2];

	float Attack[2];
	float Decay[2];

	float ModEnv;
	float ModVel;
	float ModRes;

	float Cutoff;
	float Resonance;
	int FilterType;

	bool GlideState;
	float GlideSpeed;

	bool ClipState;
	float MainVolume;
};