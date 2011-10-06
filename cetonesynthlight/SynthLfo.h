#pragma once

class CSynthLfo
{
public:
	CSynthLfo(void);
	~CSynthLfo(void);

	void	Set(float speed, int pw, int wave, bool trig);

	void	SetSpeed(float speed);
	float	GetSpeed();
	
	void	SetPw(int pw);
	int		GetPw();
	
	void	SetWave(int wave);
	int		GetWave();
	
	void	SetTrigger(bool trig);
	bool	GetTrigger();

	float	GetValue();

	float	Run();
	void	Trigger();

	void    Reset();

private:
	int OscNow;
	int OscStep;
	int Waveform;
	int PulseWidth;
	float Value;
	float Pitch;
	bool DoTrigger;
	int ShiftRegister;
};
