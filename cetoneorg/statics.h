#pragma once

typedef struct
{
	int offset[16];
} GENERATOR;

extern float g_sine[SINE_SIZE * 3 * 128];
extern GENERATOR g_generators[61];
extern int g_all_sizes[4];
extern int g_comb_sizes[8];

int irand();
void initStatics(float fs);

__forceinline float spline(float t, float* ps)
{
    return (
		(((-t + 3.0f) * t - 3.0f) * t + 1.0f) * ps[0]
		+ (((3.0f * t - 6.0f) * t) * t + 4.0f) * ps[1]
		+ (((-3.0f * t + 3.0f) * t + 3.0f) * t + 1.0f) * ps[2]
		+ (t * t * t) * ps[3]
		) * (1.0f / 6.0f);
}
