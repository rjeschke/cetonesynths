#include "arp.h"
#include <math.h>

ARP::ARP()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		float f = (float)(i + 10) / 128.0f;
		f *= f;
		this->m2f[127 - i] = powf(2.0f, f * 3.0f) - 1.0f;
	}

	for(i = 0; i < 8; i++)
		this->Notes[i] = 0;

	this->SetLoop(1);
	this->SetSync(1);
	this->SetTempo(0);
	this->SetSpeed(0);
	this->SetLength(0);
	this->Retrigger();
}

ARP::~ARP()
{
}

void ARP::Retrigger()
{
	this->Samples = 0;
	this->Pos = -1;
	this->fPos = 0;
}

void ARP::SetTempo(float f)
{
	if(this->Tempo == f)
		return;

	this->Tempo = f;
	if(this->Sync)
		this->SetSpeed(this->Speed);
}

void ARP::SetSpeed(int speed)
{
	this->Speed = speed;
	if(this->Sync)
	{
		float t = this->Tempo * g_syncSpeed[speed];
		this->Delay = truncate(t);	
		this->fDelay = truncate((t - (float)this->Delay) * 65536.0f);
	}
	else
	{
		float t = this->FS * this->m2f[speed];	
		this->Delay = truncate(t);	
		this->fDelay = truncate((t - (float)this->Delay) * 65536.0f);
	}
}

void ARP::SetSync(int sync)
{
	this->Sync = (sync) ? true : false;
	this->SetSpeed(this->Speed);
}

void ARP::SetLoop(int loop)
{
	this->Loop = (loop) ? true : false;
}

void ARP::SetLength(int len)
{
	this->Length = len;
}

void ARP::SetNote(int idx, int note)
{
	this->Notes[idx] = note * 100;
}

void ARP::SetSampleRate(float fs)
{
	if(this->FS == fs)
		return;
	this->FS = fs;
	if(!this->Sync)
		this->SetSpeed(this->Speed);
}
