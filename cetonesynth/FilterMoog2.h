#pragma once

class CFilterMoog2
{
public:
	CFilterMoog2(void);
	~CFilterMoog2(void);

	void	Set(float cutoff, float q);
	void	SetMode(int mode);
	int		GetMode();
	float	Run(float val);
	void    Reset();

private:
    float CutOff;
    float Q;
	int Mode;

	float f, p, q2;
    float b0, b1, b2, b3, b4;
};
