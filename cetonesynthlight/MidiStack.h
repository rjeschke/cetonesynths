#pragma once

class CMidiStack
{
public:
	CMidiStack(void);
	~CMidiStack(void);

	bool IsEmpty();
	void Push(int p0, int p1, int p2, int delta);
	void Pop(int* p0, int* p1, int* p2, int* delta);

private:
	int P0[4096];
	int P1[4096];
	int P2[4096];
	int Delta[4096];
	int StackLow;
	int StackHigh;
};
