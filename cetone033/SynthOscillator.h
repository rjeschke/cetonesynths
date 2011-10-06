#pragma once

#include "Cetone033.h"

class CSynthOscillator
{
public:
	CSynthOscillator(void);
	~CSynthOscillator(void);

	void	Set(float pw, int wave);
	void	SetPitch(int pitch);
	void	SetPw(float pw);
	void	SetWave(int wave);
	
	float	Run();
	
	void    Reset();

private:
	int Waveform;

	int IndexInt;
	int IndexFrac;

	int StepInt;
	int StepFrac;

	int   PwOffset;
	float PwWaveOffset;
	float TriWaveScale;

	float* SawPtr;
	float* ParabolaPtr;
};
