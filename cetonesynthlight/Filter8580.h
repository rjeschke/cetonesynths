#pragma once

class CFilter8580
{
public:
	CFilter8580(void);
	~CFilter8580(void);

	void	Set(float cutoff, float q);
	void	SetMode(int mode);
	int		GetMode();
	float	Run(float val);
	void    Reset();

	void	CalcClock();

private:
    float CutOff;
    float Q;
	int	Mode;
	int Clock;

	float w0;
	float q_par;

	float Vhp;
	float Vbp;
	float Vlp;
};
