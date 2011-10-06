#pragma once

class CFilterBiquad
{
public:
	CFilterBiquad();
	~CFilterBiquad();
	void Reset();
	void Set(float cutoff, float q);
	void SetSampleRate(float fs);
	float Run(float inputout);
	
private:
	float t0, t1;
	float min_fc;
	float a1, a2;
	float b0, b1, b2;
	float buffer[6];
};
