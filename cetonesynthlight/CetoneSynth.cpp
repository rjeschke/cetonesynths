#include <stdio.h>
#include <math.h>

#include "cetonesynth.h"
#include "cetoneeditor.h"

#if NOTE_RANGE == 1
	#pragma message("CetoneSynth: Note range == 14400")
#elif NOTE_RANGE == 2
	#pragma message("CetoneSynth: Note range == 15600")
#endif

#if ANALOGUE_BEHAVIOR > 0
	#pragma message("CetoneSynth: Using analogue behaviour")
#else
	#pragma message("CetoneSynth: Using digital behaviour")
#endif

bool TablesBuilt = false;

// Static arrays

float	CCetoneSynth::Int2FloatTab[65536];
float	CCetoneSynth::Int2FloatTab2[65536];

float	CCetoneSynth::SineTable[65536]; 
float	CCetoneSynth::SmallSineTable[WAVETABLE_LENGTH];
float	CCetoneSynth::FreqTable[PITCH_MAX];
int		CCetoneSynth::FreqTableInt[PITCH_MAX];

int		CCetoneSynth::LookupTable[65536];
float	CCetoneSynth::SawTable[NOTE_MAX * WAVETABLE_LENGTH];
float	CCetoneSynth::ParabolaTable[NOTE_MAX * WAVETABLE_LENGTH];
int		CCetoneSynth::FreqStepInt[PITCH_MAX];
int		CCetoneSynth::FreqStepFrac[PITCH_MAX];

float	CCetoneSynth::Pw2Float[4096];
int		CCetoneSynth::Pw2Offset[4096];
float	CCetoneSynth::Pw2WaveOffset[4096];

float	CCetoneSynth::SampleRate;
float	CCetoneSynth::SampleRate2;
float	CCetoneSynth::SampleRate_1;
float	CCetoneSynth::SampleRate2_1;
float	CCetoneSynth::SampleRatePi;
float	CCetoneSynth::Pi;

int	CCetoneSynth::C64Arps[8][16] =	{
		{0,  3,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3},
		{0,  4,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3},
		{0,  3,  7, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4},
		{0,  4,  7, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4},
		{0, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2},
		{0, 12,-12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3},
		{0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2},
		{0,  7, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3}};

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new CCetoneSynth(audioMaster);
}

CCetoneSynth::CCetoneSynth(audioMasterCallback audioMaster) 
: AudioEffectX (audioMaster, 128, pParameters)
{
	CCetoneSynth::Pi				=	4.f * atanf(1.f);	
	CCetoneSynth::SampleRate		=	44100.f;
	CCetoneSynth::SampleRate2		=	22050.f;
	CCetoneSynth::SampleRate_1		=	1.f / 44100.f;
	CCetoneSynth::SampleRate2_1		=	1.f / 22050.f;
	CCetoneSynth::SampleRatePi		=	CCetoneSynth::Pi / 44100.f;
	this->ModChangeSamples			=   1.f / (this->SampleRate * MOD_CHANGE_SPEED);

	this->VelocityMod = 0.f;
	this->Ctrl1Mod = 0.f;
	this->VelocityModStep = 0.f;
	this->Ctrl1ModStep = 0.f;

	for(int i = 0; i < 3; i++)
		this->Oscs[i] = new CSynthOscillator();

	this->Oscs[1]->SetSyncDest(this->Oscs[0]);
	this->Oscs[2]->SetSyncDest(this->Oscs[1]);
	this->Oscs[0]->SetSyncDest(this->Oscs[2]);

	for(int i = 0; i < 2; i++)
		this->Envs[i] = new CSynthEnvelope();

	this->Envs[0]->SetPreAttack(0.02f);
	this->Envs[1]->SetPreAttack(0.002f);

	this->Lfo = new CSynthLfo();

	this->MidiStack		= new CMidiStack();

	this->FilterDirty		= new CFilterDirty();
	this->FilterCh12db		= new CFilterCh12db();
	this->FilterMoog		= new CFilterMoog();
	this->FilterMoog2		= new CFilterMoog2();
	this->Filter303			= new CFilter303();
	this->Filter8580		= new CFilter8580();
	this->FilterBiquad		= new CFilterBiquad();

	this->Filter8580->CalcClock();

	this->InitFreqTable();
	this->InitSineTable();

	this->InitParameters();

	if (audioMaster)
	{
		setNumInputs (0);
		setNumOutputs (2);
		canProcessReplacing ();
		isSynth ();
		setUniqueID ('NCSl');
		programsAreChunks();
	}
	
	this->setEditor(new CCetoneEditor(this));

	suspend ();
}

CCetoneSynth::~CCetoneSynth()
{
	for(int i = 0; i < 3; i++)
		delete this->Oscs[i];

	for(int i = 0; i < 2; i++)
		delete this->Envs[i];

	delete this->Lfo;

	delete this->MidiStack;

	delete this->FilterDirty;
	delete this->FilterCh12db;
	delete this->FilterMoog;
	delete this->FilterMoog2;
	delete this->Filter303;
	delete this->Filter8580;
	delete this->FilterBiquad;
}

void CCetoneSynth::InitParameters()
{
	this->CurrentProgram	= 0;

	this->Volume			=	1.f;
	this->Panning			=	0.5f;

	this->MainCoarse		=	0;
	this->MainFine			=	0;

	this->FilterType		=	FTYPE_NONE;
	this->FilterMode		=	FMODE_LOW;

	this->Cutoff			=	1.f;
	this->Resonance			=	0.f;

	this->ArpMode			=	-1;
	this->ArpSpeed			=	20;

	this->PortaMode			=	false;
	this->PortaSpeed		=	0.1f;

	this->EnvMod			=	0.f;

	for (int i = 0; i < 3; i++)
	{
		this->Voice[i].Coarse	=	0;
		this->Voice[i].Fine		=	0;
		this->Voice[i].Wave		=	WAVE_SAW;
		this->Voice[i].Pw		=	32768;
		this->Voice[i].Volume	=	1.f;
		this->Voice[i].Sync		=	false;
		this->Voice[i].Ring		=	false;
	}

	this->Voice[1].Coarse	=	12;
	this->Voice[2].Coarse	=	-12;

	for(int i = 0; i < 2; i++)
	{
		this->EnvAttack[i]		=	0.f;
		this->EnvHold[i]		=	0.f;
		this->EnvDecay[i]		=	0.f;
		this->EnvSustain[i]		=	0.f;
		this->EnvRelease[i]		=	0.f;
	}

	this->EnvAttack[0]			=	0.01f;
	this->EnvHold[0]			=	0.02f;
	this->EnvDecay[0]			=	0.23f;
	this->EnvSustain[0]			=	0.75f;
	this->EnvRelease[0]			=	0.5f;

	for(int i = 0; i < 4; i++)
	{
		this->Modulations[i].Source			=	0;
		this->Modulations[i].Destination	=	0;
		this->Modulations[i].Amount			=	0.f;
		this->Modulations[i].Multiplicator	=	1.f;
	}

	this->LfoSpeed		=	0.05f;
	this->LfoWave		=	WAVE_SINE;
	this->LfoPw			=	32768;
	this->LfoTrigger	=	false;

	for(int i = 0; i < 128; i++)
	{
		char tmp[64], tmp2[64];

		vst_strncpy(tmp, "CetoneLight #", 63);
		sprintf(tmp2, "%u", i + 1);
		vst_strncat(tmp, tmp2, 63);
		
		vst_strncpy(this->Programs[i].Name, tmp, 63);

		this->WriteProgram(i);
	}

	this->ReadProgram(0);

	// Runtime

	this->CurrentDelta = 0;
	this->CurrentNote = -1;
	this->CurrentVelocity = 0;
	this->FilterCounter = 0;
}

void CCetoneSynth::InitFreqTable()
{
	if(TablesBuilt)
		return;

	this->FilterBiquad->SetSampleRate(this->SampleRate);

	float pi = 4.f * atanf(1.f);
	float halfpi = pi * 0.5f;

	for (int i = 0; i < PITCH_MAX; i++)
        this->FreqTable[i] = BASE_FREQUENCE * powf(2.f, (float)i / 1200.f);

	for (int i = 0; i < WAVETABLE_LENGTH; i++)
		this->SmallSineTable[i] = sinf(2.f * pi * (float)i / (float)WAVETABLE_LENGTH);

    int harmonicsIndex = 0;
    int lastHarmonics = -1;
    int lookupIndex = 0;

    for (int i = 0; i < NOTE_MAX; i++)
    {
        int harmonics = (int)(this->SampleRate2 / this->FreqTable[i * 100]);

        if (harmonics != lastHarmonics)
        {
            float* ptr = &(this->SawTable[harmonicsIndex * WAVETABLE_LENGTH]);

            for (int n = 0; n < WAVETABLE_LENGTH; n++)
                ptr[n] = 0.f;

			float tharm = halfpi / (float)harmonics;

            for (int n = 0; n < harmonics; n++)
            {
                int harmonic = n + 1;

                float t = cosf((float)n * tharm);
                t *= t;
                t /= (float)harmonic;

                for (int m = 0; m < WAVETABLE_LENGTH; m++)
                    ptr[m] += t * this->SmallSineTable[(m * harmonic) & WAVETABLE_MASK];
            }
            lastHarmonics = harmonics;

            int tmp = (int)(2.f * this->FreqTable[i * 100]);

            for (int n = lookupIndex; n <= tmp; n++)
                this->LookupTable[n] = harmonicsIndex;

            lookupIndex = tmp + 1;
            harmonicsIndex++;
        }
    }

    for (int i = lookupIndex; i < 65536; i++)
        this->LookupTable[i] = harmonicsIndex - 1;

    float max = 0.0;

    for (int i = 0; i < WAVETABLE_LENGTH; i++)
    {
        if (fabs(this->SawTable[i]) > max)
            max = fabs(this->SawTable[i]);
    }

    for (int i = 0; i < harmonicsIndex * WAVETABLE_LENGTH; i++)
        this->SawTable[i] /= max;

    harmonicsIndex = 0;
    lastHarmonics = -1;
    float sign;

	float pi3 = (float)(pi * pi / 3.0);

    for (int i = 0; i < NOTE_MAX; i++)
    {
        int harmonics = (int)(this->SampleRate2 / this->FreqTable[i * 100]);

        if (harmonics != lastHarmonics)
        {
            float* ptr = &(this->ParabolaTable[harmonicsIndex * WAVETABLE_LENGTH]);

            for (int n = 0; n < WAVETABLE_LENGTH; n++)
                ptr[n] = pi3;

			float tharm = halfpi / (float)harmonics;

			sign = -1.0;
            for (int n = 0; n < harmonics; n++)
            {
                float t;
                int harmonic = n + 1;

                t = cosf((float)n * tharm);
                t *= t;
                t /= (float)(harmonic * harmonic);
                t *= 4.f * sign;

                for (int m = 0; m < WAVETABLE_LENGTH; m++)
                    ptr[m] += t * this->SmallSineTable[(m * harmonic + WAVETABLE_LENGTH4) & WAVETABLE_MASK];
                sign = -sign;
            }
            lastHarmonics = harmonics;
            harmonicsIndex++;
        }
    }

    max = 0.0;

    for (int i = 0; i < WAVETABLE_LENGTH; i++)
    {
        if (fabs(this->ParabolaTable[i]) > max)
            max = fabs(this->ParabolaTable[i]);
    }

    max /= 2.f;

    for (int i = 0; i < harmonicsIndex * WAVETABLE_LENGTH; i++)
    {
        this->ParabolaTable[i] /= max;
        this->ParabolaTable[i] -= 1.f;
    }

	for (int i = 0; i < PITCH_MAX; i++)
	{
		float rate = this->FreqTable[i] * (float) WAVETABLE_LENGTH / this->SampleRate;

		this->FreqStepInt[i] = (int)rate;
		this->FreqStepFrac[i] = (int)((rate - (float)this->FreqStepInt[i]) * 65536.f);

		this->FreqTableInt[i] = (int)(this->FreqTable[i] * 2.f);
	}

	for (int i = 0; i < 4096; i++)
	{
		int pw = (i << 4);
		
		float f = 1.f - ((float)pw / 65536.f);

		int f0 = (int)(f * WAVETABLE_LENGTHf);

		float f1 = (float)f0 / WAVETABLE_LENGTHf;
		float f2 = 1.f - (2.f * f1);

		this->Pw2Float[i] = f1;
		this->Pw2Offset[i] = f0;
		this->Pw2WaveOffset[i] = f2;
	}

	for (int i = 0; i < 65536; i++)
	{
		this->Int2FloatTab[i] = (float)i / 65536.f;
		this->Int2FloatTab2[i] = (float)(i - 32768) / 32768.f;
	}

	TablesBuilt = true;
}

void CCetoneSynth::InitSineTable()
{
	float pi = 4.f * atanf(1.f);
	float st = pi / 32768.f;

	for (int i = 0; i < 65536; i++)
		this->SineTable[i] = sinf((float)i * st);
}

void CCetoneSynth::ReadProgram(int prg)
{
	if(prg < 0 || prg > 127)
		return;

	this->CurrentProgram = prg;
	SynthProgram* p = &Programs[this->CurrentProgram];

	this->Volume		=	p->Volume;
	this->Panning		=	p->Panning;

	this->MainCoarse	=	p->Coarse;
	this->MainFine		=	p->Fine;

	this->FilterType	=	p->FilterType;
	this->FilterMode	=	p->FilterMode;

	this->Cutoff		=	p->Cutoff;
	this->Resonance		=	p->Resonance;

	this->ArpMode		=	p->ArpMode;
	this->ArpSpeed		=	p->ArpSpeed;

	this->SetArpSpeed(this->ArpSpeed);

	this->PortaMode		=	p->PortaMode;
	this->PortaSpeed	=	p->PortaSpeed;

	this->SetPortaSpeed(this->PortaSpeed);

	this->EnvMod		=	p->EnvMod;

	for(int i = 0; i < 3; i++)
	{
		this->Voice[i].Coarse	=	p->Voice[i].Coarse;
		this->Voice[i].Fine		=	p->Voice[i].Fine;
		this->Voice[i].Wave		=	p->Voice[i].Wave;
		this->Voice[i].Pw		=	p->Voice[i].Pw;
		this->Voice[i].Volume	=	p->Voice[i].Volume;
		this->Voice[i].Sync		=	p->Voice[i].Sync;
		this->Voice[i].Ring		=	p->Voice[i].Ring;
	}

	for(int i = 0; i < 2; i++)
	{
		this->EnvAttack[i]		=	p->Attack[i];
		this->EnvHold[i]		=	p->Hold[i];
		this->EnvDecay[i]		=	p->Decay[i];
		this->EnvSustain[i]		=	p->Sustain[i];
		this->EnvRelease[i]		=	p->Release[i];
	}

	this->LfoSpeed		=	p->LfoSpeed;
	this->LfoWave		=	p->LfoWave;
	this->LfoPw			=	p->LfoPw;
	this->LfoTrigger	=	p->LfoTrigger;

	this->Lfo->Set(this->LfoSpeed, this->LfoPw, this->LfoWave, this->LfoTrigger);

	for(int i = 0; i < 4; i++)
	{
		this->Modulations[i].Source			=	p->Modulations[i].Source;
		this->Modulations[i].Destination	=	p->Modulations[i].Destination;
		this->Modulations[i].Amount			=	p->Modulations[i].Amount;
		this->Modulations[i].Multiplicator	=	p->Modulations[i].Multiplicator;
	}

	this->UpdateEnvelopes();
}

void CCetoneSynth::WriteProgram(int prg)
{
	SynthProgram* p = &Programs[prg];

	p->Volume		=	this->Volume;
	p->Panning		=	this->Panning;

	p->Coarse		=	this->MainCoarse;
	p->Fine			=	this->MainFine;

	p->FilterType	=	this->FilterType;
	p->FilterMode	=	this->FilterMode;

	p->Cutoff		=	this->Cutoff;
	p->Resonance	=	this->Resonance;

	p->ArpMode		=	this->ArpMode;
	p->ArpSpeed		=	this->ArpSpeed;
	
	p->PortaMode	=	this->PortaMode;
	p->PortaSpeed	=	this->PortaSpeed;

	p->EnvMod		=	this->EnvMod;

	for(int i = 0; i < 3; i++)
	{
		p->Voice[i].Coarse	=	this->Voice[i].Coarse;
		p->Voice[i].Fine	=	this->Voice[i].Fine;
		p->Voice[i].Wave	=	this->Voice[i].Wave;
		p->Voice[i].Pw		=	this->Voice[i].Pw;
		p->Voice[i].Volume	=	this->Voice[i].Volume;
		p->Voice[i].Sync	=	this->Voice[i].Sync;
		p->Voice[i].Ring	=	this->Voice[i].Ring;
	}

	for(int i = 0; i < 2; i++)
	{
		p->Attack[i]		=	this->EnvAttack[i];
		p->Hold[i]			=	this->EnvHold[i];
		p->Decay[i]			=	this->EnvDecay[i];
		p->Sustain[i]		=	this->EnvSustain[i];
		p->Release[i]		=	this->EnvRelease[i];
	}

	p->LfoSpeed		=	this->LfoSpeed;
	p->LfoWave		=	this->LfoWave;
	p->LfoPw		=	this->LfoPw;
	p->LfoTrigger	=	this->LfoTrigger;

	for(int i = 0; i < 4; i++)
	{
		p->Modulations[i].Source		=	this->Modulations[i].Source;
		p->Modulations[i].Destination	=	this->Modulations[i].Destination;
		p->Modulations[i].Amount		=	this->Modulations[i].Amount;
		p->Modulations[i].Multiplicator	=	this->Modulations[i].Multiplicator;
	}
}
