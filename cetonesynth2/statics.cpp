#include <math.h>
#include <windows.h>
#include "defines.h"
#include "statics.h"

static  float g_sampleRate = 0.0f;

float	g_sine[WAVETABLE_LENGTH];
float	g_freqtab[PITCH_MAX + 1];

float	g_saw[(NOTE_MAX + 25) * WAVETABLE_LENGTH * 2];
float	g_parabola[(NOTE_MAX + 25) * WAVETABLE_LENGTH * 2];
__int32	g_step[PITCH_MAX];
__int32	g_lookup[65536];
double	g_fsin[32768];
double	g_fcos[32768];

float	g_lfo_saw[WAVETABLE_LENGTH * 2];
float	g_lfo_tri[WAVETABLE_LENGTH * 2];
__int32	g_lfo_step[4096];

__int32 g_semi[128];
__int32 g_fine[128];
float	g_m2f0[128];
float	g_m2f1[128];
float	g_m2co[128];
float	g_m2vol[128];
float	g_m2fgain[128];
float	g_m2fenv[128];
float	g_m2mod[128];
int		g_osc_pw[128 * 16];
float	g_osc_pwo[128 * 16];
float	g_osc_tris[128 * 16];

unsigned __int32 _RND;
__int32 _RND1, _RND2;


const float g_syncSpeed[132] = {0.000000f,576.000000f,384.000000f,256.000000f,558.000000f,372.000000f,248.000000f,540.000000f,
 360.000000f,240.000000f,522.000000f,348.000000f,232.000000f,504.000000f,336.000000f,224.000000f,
 486.000000f,324.000000f,216.000000f,468.000000f,312.000000f,208.000000f,450.000000f,300.000000f,
 200.000000f,432.000000f,288.000000f,192.000000f,414.000000f,276.000000f,184.000000f,396.000000f,
 264.000000f,176.000000f,378.000000f,252.000000f,168.000000f,360.000000f,240.000000f,160.000000f,
 342.000000f,228.000000f,152.000000f,324.000000f,216.000000f,144.000000f,306.000000f,204.000000f,
 136.000000f,288.000000f,192.000000f,128.000000f,270.000000f,180.000000f,120.000000f,252.000000f,
 168.000000f,112.000000f,234.000000f,156.000000f,104.000000f,216.000000f,144.000000f,96.000000f,
 198.000000f,132.000000f,88.000000f,180.000000f,120.000000f,80.000000f,162.000000f,108.000000f,
 72.000000f,144.000000f,96.000000f,64.000000f,126.000000f,84.000000f,56.000000f,108.000000f,
 72.000000f,48.000000f,90.000000f,60.000000f,40.000000f,72.000000f,48.000000f,32.000000f,
 54.000000f,36.000000f,24.000000f,36.000000f,24.000000f,16.000000f,18.000000f,12.000000f,
 8.000000f,9.000000f,6.000000f,4.000000f,4.500000f,3.000000f,2.000000f,2.250000f,
 1.500000f,1.000000f,1.125000f,0.750000f,0.500000f,0.562500f,0.375000f,0.250000f,
 0.281250f,0.187500f,0.125000f,0.140625f,0.093750f,0.062500f,0.062500f,0.062500f,
 0.062500f,0.062500f,0.062500f,0.062500f,0.062500f,0.062500f,0.062500f,0.062500f,
 0.062500f,0.062500f,0.062500f,0.062500f};


void initStatics(float fs)
{
	int i, n, m;
	float fs2 = fs * 0.5f;
	float upper = fs2;

	_RND = (unsigned __int32) GetTickCount();
	_RND1 = 0x67452301;
	_RND2 = 0xefcdab89;

	if((g_sampleRate != 0.0f) && (fs == g_sampleRate))
		return;
	else
		g_sampleRate = fs;

	double fupper = (double)fs2;
	if(fupper > 22000.0)
		fupper = 22000.0;
	for(i = 0; i < 32768; i++)
	{
		double f = (double)i / 32768.0;
		if(f < 0.001)
			f = 0.001;
		double v = (PId * f * fupper) / (double)fs;
		g_fsin[i] = sin(v);
		g_fcos[i] = cos(v);
	}

	for(i = 0; i < 128; i++)
	{
		float f = (float)(i - 64) / 64.0f;
		float g = (float)i / 127.0f;
		g_semi[i] = (i - 64) * 100;
		g_fine[i] = i - 64;
		g_m2f0[i] = g;
		g_m2f1[i] = f;
		
		if(i < 64)
			g_m2fenv[i] = -(f * f + 1.0f / 256.0f);
		else if(i > 64)
			g_m2fenv[i] =(f * f + 1.0f / 256.0f);
		else
			g_m2fenv[i] = 0;

		if(i < 64)
			g_m2mod[i] = -(f * f + 1.0f / 256.0f);
		else if(i > 64)
			g_m2mod[i] =(f * f + 1.0f / 256.0f);
		else
			g_m2mod[i] = 0;

		g_m2co[i] = g * g;
		g_m2vol[i] = g * g * 2.0f;
		g_m2fgain[i] = (f < 0.0f) ? powf(2.0, f * 4.0f) : powf(2.0, f * 2.0f);
	}

	for(i = 0; i < 2048; i++)
	{
		n = i;
		if(n < 5)
			n = 5;
		else if(n > 2042)
			n = 2042;

		float f = (float)n / 2048.0f;
		g_osc_pw[i] = i * (WAVETABLE_LENGTH / 2048);
		g_osc_pwo[i] = 1.f - (2.f * f);
		g_osc_tris[i] = 1.f / (9.f * (f - f * f));
	}

	for (i = 0; i < WAVETABLE_LENGTH; i++)
		g_sine[i] = sinf(2.f * PI * (float)i / WAVETABLE_LENGTHf);

	for(i = 0; i < WAVETABLE_LENGTH; i++)
	{
		int n = (i + WAVETABLE_LENGTH4) & WAVETABLE_MASK;
		float f = (float)n / (float)(WAVETABLE_LENGTH1);
		g_lfo_saw[i] = (f - 0.5f) * 2.0f;

		if(f <= 0.5f)
			g_lfo_tri[i] = (f - 0.25f) * 4.0f;
		else
			g_lfo_tri[i] = (0.75f - f) * 4.0f;

		g_lfo_saw[i + WAVETABLE_LENGTH] = g_lfo_saw[i];
		g_lfo_tri[i + WAVETABLE_LENGTH] = g_lfo_tri[i];
	}

	for (i = 0; i <= PITCH_MAX; i++)
        g_freqtab[i] = 440.0f * powf(2.f, (float)(i - 6900) / 1200.f);

    int harmonicsIndex = 0;
    int lastHarmonics = -1;
    int lookupIndex = 0;

    for (i = 0; i < (NOTE_MAX + 25); i++)
    {
		float freq = 440.0f * powf(2.f, (float)(i - 69) / 12.f);
        int harmonics = truncate(upper / freq);
		if(!harmonics)
			harmonics = 1;

        if (harmonics != lastHarmonics)
        {
            float* ptr = &(g_saw[harmonicsIndex * WAVETABLE_LENGTH * 2]);
			
            for (n = 0; n < WAVETABLE_LENGTH; n++)
                ptr[n] = 0.f;

			float tharm = (PI * 0.5f) / (float)harmonics;

            for (n = 0; n < harmonics; n++)
            {
                int harmonic = n + 1;

                float t = cosf((float)n * tharm);
                t *= t;
                t /= (float)harmonic;

                for (m = 0; m < WAVETABLE_LENGTH; m++)
                    ptr[m] += t * g_sine[(m * harmonic) & WAVETABLE_MASK];
            }
            for (m = 0; m < WAVETABLE_LENGTH; m++)
                ptr[m + WAVETABLE_LENGTH] = ptr[m];
            lastHarmonics = harmonics;

            int tmp = truncate(floorf(2.f * freq));

            for (n = lookupIndex; n <= tmp; n++)
                g_lookup[n] = harmonicsIndex * WAVETABLE_LENGTH * 2;

            lookupIndex = tmp + 1;
            harmonicsIndex++;
        }
    }

    for (i = lookupIndex; i < 65536; i++)
        g_lookup[i] = (harmonicsIndex - 1) * WAVETABLE_LENGTH * 2;

    float max = 0.0;

    for (i = 0; i < WAVETABLE_LENGTH; i++)
    {
        if (fabs(g_saw[i]) > max)
            max = fabs(g_saw[i]);
    }

    for (i = WAVETABLE_LENGTH; i < harmonicsIndex * WAVETABLE_LENGTH * 2; i++)
        g_saw[i] /= max;

    harmonicsIndex = 0;
    lastHarmonics = -1;
    float sign;

	float pi3 = (float)(PI * PI / 3.0);

    for (i = 0; i < (NOTE_MAX + 25); i++)
    {
        int harmonics = truncate(upper / (440.0f * powf(2.f, (float)(i - 69) / 12.f)));
		if(!harmonics)
			harmonics = 1;

        if (harmonics != lastHarmonics)
        {
            float* ptr = &(g_parabola[harmonicsIndex * WAVETABLE_LENGTH * 2]);

            for (n = 0; n < WAVETABLE_LENGTH; n++)
                ptr[n] = pi3;

			float tharm = (PI * 0.5f) / (float)harmonics;

			sign = -1.f;
            for (n = 0; n < harmonics; n++)
            {
                float t;
                int harmonic = n + 1;

                t = cosf((float)n * tharm);
                t *= t;
                t /= (float)(harmonic * harmonic);
                t *= 4.f * sign;

                for (m = 0; m < WAVETABLE_LENGTH; m++)
                    ptr[m] += t * g_sine[(m * harmonic + WAVETABLE_LENGTH4) & WAVETABLE_MASK];
                sign = -sign;
            }
            for (m = 0; m < WAVETABLE_LENGTH; m++)
                ptr[m + WAVETABLE_LENGTH] = ptr[m];
            lastHarmonics = harmonics;
            harmonicsIndex++;
        }
    }

    max = 0.0;

    for (i = 0; i < WAVETABLE_LENGTH; i++)
    {
        if (fabs(g_parabola[i]) > max)
            max = fabs(g_parabola[i]);
    }

    max /= 2.f;

    for (i = 0; i < harmonicsIndex * WAVETABLE_LENGTH * 2; i++)
    {
        g_parabola[i] /= max;
        g_parabola[i] -= 1.f;
    }

	for (i = 0; i < PITCH_MAX; i++)
	{
		float rate = g_freqtab[i] * WAVETABLE_LENGTHf / fs;
		g_step[i] = truncate(rate * 65536.0f + 0.5f);
	}

	for (i = 0; i < 4096; i++)
	{
		float rate = powf(2.0, (float)(i - 2048) / 384.0f) * WAVETABLE_LENGTHf / fs;
		g_lfo_step[i] = truncate(rate * 65536.0f + 0.5f);
	}
}

// Interpolates between ps[1] and ps[2]
float spline(float t, float* ps)
{
    return (
		(((-t + 3.0f) * t - 3.0f) * t + 1.0f) * ps[0]
		+ (((3.0f * t - 6.0f) * t) * t + 4.0f) * ps[1]
		+ (((-3.0f * t + 3.0f) * t + 3.0f) * t + 1.0f) * ps[2]
		+ (t * t * t) * ps[3]
		) / 6.0f;
}

int irand()
{
	_RND = (_RND * RANDa) + RANDc;
	return _RND >> 16;
}

