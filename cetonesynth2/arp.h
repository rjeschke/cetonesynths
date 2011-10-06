#pragma once

#include "defines.h"
#include "statics.h"

class ARP
{
public:
	ARP();
	~ARP();
	void Retrigger();
	void SetTempo(float f);
	void SetSampleRate(float fs);
	void SetSpeed(int speed);
	void SetSync(int sync);
	void SetLoop(int loop);
	void SetLength(int len);
	void SetNote(int idx, int note);
	
	__forceinline int Process() 
	{
		if(!this->Speed)
			return 0;
		
		if((this->Pos > this->Length) && !this->Loop)
			return 0;

		if(this->Samples-- <= 0)
		{
			this->Pos++;
			if(this->Pos > this->Length)
			{
				if(this->Loop)
					this->Pos = 0;
				else
					return 0;
			}
			this->Samples = this->Delay;
			this->fPos += this->fDelay;
			if(this->fPos > 0xffff)
			{
				this->Samples++;
				this->fPos &= 0xffff;
			}
		}

		return this->Notes[this->Pos]; 
	};
private:
	float m2f[128];
	int Notes[8];
	float FS;
	int Length;
	int Speed;
	bool Loop;
	bool Sync;
	int Pos;
	int fPos;
	int Delay;
	int fDelay;
	int Samples;
	float Tempo;
};
