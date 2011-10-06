#pragma once

#include "filtermoog.h"
#include "filterbiquad.h"

class CCetoneLpFilter
{
public:
	CCetoneLpFilter(void);
	~CCetoneLpFilter(void);
	
	void Reset();
	
	void Set(float cutoff, float q);
	void SetSampleRate(float fs);
	void Name(int type, char* text);
	
	float Run(float inputout);
	
	void SetType(int type);
	int GetType();

private:
	int Type;
	float Cutoff, Resonance;

	CFilterMoog*	FilterMoog;
	CFilterBiquad*	FilterBiquad;
};
