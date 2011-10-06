#pragma once

#include "CetoneSynth.h"

class CSynthOscillator
{
public:
	CSynthOscillator(void);
	~CSynthOscillator(void);

	void	Set(int pw, int wave, bool sync);

	void	SetPitch(int pitch);
	int		GetPitch();
	
	void	SetPw(int pw);
	int		GetPw();
	
	void	SetWave(int wave);
	int		GetWave();
	
	float	Run();
	
	void	Sync();
	void	SetSync(bool sync);
	void	SetSyncDest(CSynthOscillator* osc);

	void    Reset();

private:
	int Waveform;
	int PulseWidth;
	int Value;
	int Pitch;
	int ShiftRegister;

	int IndexInt;
	int IndexFrac;

	int StepInt;
	int StepFrac;

	float PwFloat;
	float PwWaveOffset;
	int   PwOffset;

	float* SawPtr;
	float* ParabolaPtr;

	bool DoSync;
	CSynthOscillator* SyncOsc;
};
