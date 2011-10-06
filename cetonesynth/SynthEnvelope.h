#pragma once

class CSynthEnvelope
{
public:
	CSynthEnvelope(void);
	~CSynthEnvelope(void);

	void	Gate(bool gate);
	float	Run();

	void	Set(float a, float h, float d, float s, float r);

	void	SetAttack(float val);
	float	GetAttack();
	void	SetHold(float val);
	float	GetHold();
	void	SetDecay(float val);
	float	GetDecay();
	void	SetSustain(float val);
	float	GetSustain();
	void	SetRelease(float val);
	float	GetRelease();

	void	SetPreAttack(float val);

	void	Reset();

private:
	float	Attack, Hold, Decay, Sustain, Release, PreAttack;
	int		sAttack, sHold, sDecay, sRelease, sPreAttack;
	float	sAttackf, sHoldf, sDecayf, sReleasef, sPreAttackf;

    int Phase;
    int LastPhase;

    float VolStep;
    float VolNow;

    int Counter;
    int Samples;
};
