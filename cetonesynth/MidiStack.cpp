#include "MidiStack.h"

CMidiStack::CMidiStack(void)
{
	this->StackLow = 0;
	this->StackHigh = 0;
}

CMidiStack::~CMidiStack(void)
{
}

bool CMidiStack::IsEmpty()
{
	return (this->StackLow == this->StackHigh) ? true : false;
}

void CMidiStack::Push(int p0, int p1, int p2, int delta)
{
	this->P0[this->StackHigh] = p0;
	this->P1[this->StackHigh] = p1;
	this->P2[this->StackHigh] = p2;
	this->Delta[this->StackHigh] = delta;

	this->StackHigh++;
	this->StackHigh &= 4095;
}

void CMidiStack::Pop(int *p0, int *p1, int *p2, int *delta)
{
	*p0 = this->P0[this->StackLow];
	*p1 = this->P1[this->StackLow];
	*p2 = this->P2[this->StackLow];
	*delta = this->Delta[this->StackLow];

	this->StackLow++;
	this->StackLow &= 4095;
}
