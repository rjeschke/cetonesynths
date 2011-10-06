#pragma once

class CFilterMoog
{
public:
	CFilterMoog(void);
	~CFilterMoog(void);

	void	Set(float cutoff, float q);
	void	SetMode(int mode);
	int		GetMode();
	float	Run(float val);
	void    Reset();

private:
    float CutOff;
    float Q;

	float p, k, r;

    float y1;
    float y2;
    float y3;
    float y4;
    float oldx;
    float oldy1;
    float oldy2;
    float oldy3;
};
