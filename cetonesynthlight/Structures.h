#pragma once

struct SynthVoice
{
	float			Volume;
	int				Coarse;
	int				Fine;
	int				Wave;
	int				Pw;
	bool			Ring;
	bool			Sync;
};

struct SynthModulation
{
	int				Source;
	int				Destination;
	float			Amount;
	float			Multiplicator;
};

struct SynthProgram
{
	char			Name[64];

	// Global

	float			Volume;
	float			Panning;

	int				Coarse;
	int				Fine;

	// Filter

	float			Cutoff;
	float			Resonance;
	int				FilterType;
	int				FilterMode;

	// C64er Arpeggio

	int				ArpMode;
	int				ArpSpeed;

	// Portamento

	bool			PortaMode;
	float			PortaSpeed;

	// Envelopes

	float			Attack[2];
	float			Hold[2];
	float			Decay[2];
	float			Sustain[2];
	float			Release[2];
	
	// LFO's

	float			LfoSpeed;
	int				LfoWave;
	int				LfoPw;
	bool			LfoTrigger;

	// Voices

	SynthVoice		Voice[4];
	
	// Modulations

	SynthModulation	Modulations[4];

	float			EnvMod;
};

enum PARAMETERS
{
	pVolume = 0,
	pPanning,
	pCoarse,
	pFine,

	pFilterType,	// 4
	pFilterMode,
	pCutoff,
	pResonance,

	pPortaMode,		// 8
	pPortaSpeed,

	pArpMode,		// 10
	pArpSpeed,

	pOsc1Coarse,	// 12
	pOsc1Fine,
	pOsc1Wave,
	pOsc1Pw,
	pOsc1Volume,
	pOsc1Ring,
	pOsc1Sync,

	pOsc2Coarse,	// 19
	pOsc2Fine,
	pOsc2Wave,
	pOsc2Pw,
	pOsc2Volume,
	pOsc2Ring,
	pOsc2Sync,

	pOsc3Coarse,	// 26
	pOsc3Fine,
	pOsc3Wave,
	pOsc3Pw,
	pOsc3Volume,
	pOsc3Ring,
	pOsc3Sync,

	pEnv1A,			// 34
	pEnv1H,
	pEnv1D,
	pEnv1S,
	pEnv1R,

	pEnv2A,			// 38
	pEnv2H,
	pEnv2D,
	pEnv2S,
	pEnv2R,

	pLfo1Speed,		// 43
	pLfo1Wave,
	pLfo1Pw,
	pLfo1Trig,

	pMod1Src,		// 47
	pMod1Dest,
	pMod1Amount,
	pMod1Mul,

	pMod2Src,
	pMod2Dest,
	pMod2Amount,
	pMod2Mul,

	pMod3Src,
	pMod3Dest,
	pMod3Amount,
	pMod3Mul,

	pMod4Src,
	pMod4Dest,
	pMod4Amount,
	pMod4Mul,

	pFilterMod,

	pParameters
};