#pragma once

class CFilterDirty
{
public:
	CFilterDirty(void);
	~CFilterDirty(void);

	void	Set(float cutoff, float q);
	void	SetMode(int mode);
	int		GetMode();
	float	Run(float val);
	void    Reset();

private:
    float CutOff;
    float Q;
    int Mode;

    float fb, f, buf0, buf1;
};
