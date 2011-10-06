#pragma once

class CFilterCh12db
{
public:
	CFilterCh12db(void);
	~CFilterCh12db(void);

	void	Set(float cutoff, float q);
	void	SetMode(int mode);
	int		GetMode();
	float	Run(float val);
	void    Reset();

private:
    float CutOff;
    float Q;
    int Mode;

    float f;
    float Low;
    float Band;
    float High;
    float Notch;
};
