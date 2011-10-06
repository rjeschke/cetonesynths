#include "osc.h"

OSC::OSC()
{
	this->Pitch = 0;
	this->SetWave(OSCW_OFF);
	this->SetMode(OSCM_NORMAL);
	this->SetPw(1024);
	this->DoSync = false;
	this->IsSync = false;
	this->IsAM = false;
	this->SetSpread(0);
	this->SetPitch(6900);
	this->Reset();

	this->SIndex[0].fixed.index = irand() & WAVETABLE_MASK;
	this->SIndex[1].fixed.index = irand() & WAVETABLE_MASK;
	this->SIndex[2].fixed.index = irand() & WAVETABLE_MASK;
	this->SIndex[3].fixed.index = irand() & WAVETABLE_MASK;
	this->SIndex[4].fixed.index = irand() & WAVETABLE_MASK;
}

void OSC::SetWave(int wave)
{
	this->Waveform = wave & 15;
}

void OSC::SetMode(int mode)
{
	this->Modus = mode;
}

void OSC::SetSpread(int val)
{
	if(val < 0)
		val = 0;
	else if(val > 31)
		val = 31;
	val++;
	this->Spread[0] = val;
	this->Spread[1] = 2 * val;
	val = this->Pitch;
	this->Pitch = -1;
	this->SetPitch(val);
}

void OSC::AlertSlave(bool sync)
{
	this->IsSync = sync;
	this->SetTables();
}

void OSC::SetSync(int val)
{
	this->DoSync = (val) ? true : false;
	this->Slave->AlertSlave(this->DoSync);
}

void OSC::Reset()
{
	this->Sync();
}
