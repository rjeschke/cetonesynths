#pragma once

class CFilter303
{
public:
	CFilter303(void);
	~CFilter303(void);

	void	Set(float cutoff, float q, float mod);
	void	SetMode(int mode);
	int		GetMode();
	float	Run(float val);
	void    Reset();

private:
    float CutOff;
    float Q;
	float env_mod;
	float res_coef;

	float a, b, c, d1, d2;
};
