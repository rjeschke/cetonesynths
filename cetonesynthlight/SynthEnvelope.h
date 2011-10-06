#pragma once

class CSynthEnvelope
{
public:
	CSynthEnvelope(void);
	~CSynthEnvelope(void);

	void	Gate(bool gate);
	float	Run();

	void	Set(float a, float h, float d, float s, float r);

	float	GetAttack();
	float	GetHold();
	float	GetDecay();
	float	GetSustain();
	float	GetRelease();

	float	TimeValue(float val);

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
