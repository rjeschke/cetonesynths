#include "defines.h"
#include "statics.h"
#include <math.h>

#define RANDa 1664525
#define RANDc 1013904223

int g_all_sizes[4] = {556, 441, 341, 225};
int g_comb_sizes[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
float g_sine[SINE_SIZE * 3 * 128];
GENERATOR g_generators[61];

unsigned __int32 _RND;

static float g_sampleRate = 0.0f;

int irand()
{
	_RND = (_RND * RANDa) + RANDc;
	return _RND >> 16;
}

void initStatics(float fs)
{
	int i, n, m;
	float sine[SINE_SIZE];
	float para[SINE_SIZE];
	float tri[SINE_SIZE];
	int offs[9] = { -12, -5, 0, 12, 19, 24, 28, 31, 36 };

	if(g_sampleRate == fs)
		return;

	g_sampleRate = fs;

	for(i = 0; i < 61; i++)
	{
		for(n = 0; n < 9; n++)
		{
			int v = i + 12 + offs[n];
			while(v > 88)
				v -= 12;
			g_generators[i].offset[n] = v;
		}
	}

	for(i = 0; i < SINE_SIZE; i++)
	{
		sine[i] = sinf(2.0f * PI * (float)i / SINE_SIZEf);
		para[i] = (PI * PI / 3.0f);
	}

	int harms = truncate((fs * 0.49f) / 5273.6f);
	float tharm = (PI * 0.5f) / (float)harms;

	float sign = -1.f;
    for (n = 0; n < harms; n++)
    {
        float t;
        int harmonic = n + 1;
        t = cosf((float)n * tharm);
        t *= t;
        t /= (float)(harmonic * harmonic);
        t *= 4.f * sign;

        for (m = 0; m < SINE_SIZE; m++)
            para[m] += t * sine[(m * harmonic + (SINE_SIZE / 4)) & (SINE_SIZE - 1)];
        sign = -sign;
    }
	float max = 0.0f;

    for (i = 0; i < SINE_SIZE; i++)
    {
        if (fabs(para[i]) > max)
            max = fabs(para[i]);
    }

    max /= 2.f;

    for (i = 0; i < SINE_SIZE; i++)
    {
        para[i] /= max;
        para[i] -= 1.f;
    }

	max = 1.0f / 2.25f;
	for(i = 0; i < SINE_SIZE; i++)
		tri[i] = (para[i] - para[(i + (SINE_SIZE / 2)) & (SINE_SIZE - 1)]) * max;

	for(i = 0; i < 128; i++)
	{
		float mix1 = (float)i / 127.0f;
		float mix0 = 1.0f - mix1;
		float *ptr = g_sine + i * SINE_SIZE * 3;

		for(n = 0; n < SINE_SIZE; n++)
		{
			ptr[n + SINE_SIZE] = 
				ptr[n + 2 * SINE_SIZE] = 
				ptr[n] = sine[n] * mix0 + tri[n] * mix1;
		}
	}
}
