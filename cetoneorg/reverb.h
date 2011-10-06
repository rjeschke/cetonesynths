#pragma once
#include "defines.h"
#include "statics.h"
#include <memory.h>
#include <math.h>

#define FIXED_GAIN 0.015f
#define SCALE_WET 2.0f
#define SCALE_DAMP 0.4f
#define SCALE_ROOM 0.28f
#define OFFSET_ROOM 0.7f
#define INITIAL_ROOM 0.5f
#define INITIAL_DAMP 0.5f
#define INITIAL_WET (0.5f / SCALE_WET)
#define INITIAL_DRY 0.5f
#define INITIAL_WIDTH 1.0f
#define STEREO_SPREAD 23

class CombFilter
{
public:
	CombFilter()
	{
		this->Reset();
	}

	~CombFilter()
	{
	}

	void Reset()
	{
		this->_History = 0.0f;
		memset(this->_Buffer, 0, 8192 * sizeof(float));
	}

	void setSize(int size)
	{
		this->_Size = size;
		this->Reset();
	}

	void setFeedback(float value)
	{
		this->_Feedback = value;
	}

	void setDamp(float value)
	{
		this->_Damp1 = value;
		this->_Damp2 = 1.0f - value;
	}

    __forceinline float Process(float input)
    {
        float output;

		output = this->_Buffer[this->_Pos];

		this->_History = (output * this->_Damp2) + (this->_History * this->_Damp1);
		UNDENORM(this->_History);

		this->_Buffer[this->_Pos] = input + (this->_History * this->_Feedback);
		UNDENORM(this->_Buffer[this->_Pos]);

		this->_Pos = (this->_Pos + 1) % this->_Size;

        return output;
    }
private:
	float _Buffer[8192];
	int _Size;
	float _Damp1;
	float _Damp2;
	float _Feedback;
	int _Pos;
	float _History;
};

class AllPassFilter
{
public:
    AllPassFilter()
    {
        this->Reset();
    }

	~AllPassFilter()
    {
    }

	void setSize(int size)
	{
		this->_Size = size;
		this->Reset();
	}

    void Reset()
    {
		memset(this->_Buffer, 0, 3072 * sizeof(float));
    }

    __forceinline float Process(float input)
    {
        float output, buf;

        buf = this->_Buffer[this->_Pos];
        output = buf - input;

		this->_Buffer[this->_Pos] = input + (buf * 0.5f);
		UNDENORM(this->_Buffer[this->_Pos]);

		this->_Pos = (this->_Pos + 1) % this->_Size;

        return output;
    }
private:
    float _Buffer[3072];
    int _Size;
    int _Pos;
};

class Reverb
{
public:
	Reverb()
	{
		int i;
		for(i = 0; i < 16; i++)
		{
			this->_combs[i] = new CombFilter();
			if(i < 8)
				this->_alls[i] = new AllPassFilter();
		}

		this->setWet(INITIAL_WET);
		this->setDry(INITIAL_DRY);
		this->setDamp(INITIAL_DAMP);
		this->setWidth(INITIAL_WIDTH);
		this->setRoomsize(INITIAL_ROOM);

		this->Update();
	}

	~Reverb()
	{
		int i;
		for(i = 0; i < 16; i++)
		{
			delete this->_combs[i];
			if(i < 8)
				delete this->_alls[i];
		}
	}

	void Reset()
	{
		int i;
		for(i = 0; i < 16; i++)
		{
			this->_combs[i]->Reset();
			if(i < 8)
				this->_alls[i]->Reset();
		}
	}

	void setDamp(float val)
	{
		this->_Damp = val * SCALE_DAMP;
		this->Update();
	}

	void setRoomsize(float val)
	{
		this->_Roomsize = val * SCALE_ROOM + OFFSET_ROOM;
		this->Update();
	}

	void setWet(float val)
	{
		this->_Wet = val * SCALE_WET;
		this->Update();
	}

	void setDry(float val)
	{
		this->_Dry = val;
	}

	void setWidth(float val)
	{
		this->_Width = val;
		this->Update();
	}

	void setSampleRate(float fs)
	{
		int i;
		float fac = fs / 44100.0f;
		for(i = 0; i < 8; i++)
		{
			this->_combs[i * 2]->setSize(truncate((float)g_comb_sizes[i] * fac));
			this->_combs[i * 2 + 1]->setSize(truncate(((float)g_comb_sizes[i] + (float)STEREO_SPREAD) * fac));
		}
		for(i = 0; i < 4; i++)
		{
			this->_alls[i * 2]->setSize(truncate((float)g_all_sizes[i] * fac));
			this->_alls[i * 2 + 1]->setSize(truncate(((float)g_all_sizes[i] + (float)STEREO_SPREAD) * fac));
		}
	}

	void Render(float *left, float* right, int samples)
	{
		float ol, or;
		int i;
		for(i = 0; i < samples; i++)
		{

			float input = (left[i] + right[i]) * FIXED_GAIN;

			ol = this->_combs[0]->Process(input)
				+ this->_combs[2]->Process(input)
				+ this->_combs[4]->Process(input)
				+ this->_combs[6]->Process(input)
				+ this->_combs[8]->Process(input)
				+ this->_combs[10]->Process(input)
				+ this->_combs[12]->Process(input)
				+ this->_combs[14]->Process(input);
			
			or = this->_combs[1]->Process(input)
				+ this->_combs[3]->Process(input)
				+ this->_combs[5]->Process(input)
				+ this->_combs[7]->Process(input)
				+ this->_combs[9]->Process(input)
				+ this->_combs[11]->Process(input)
				+ this->_combs[13]->Process(input)
				+ this->_combs[15]->Process(input);

			ol = this->_alls[0]->Process(ol);
			ol = this->_alls[2]->Process(ol);
			ol = this->_alls[4]->Process(ol);
			ol = this->_alls[6]->Process(ol);
			or = this->_alls[1]->Process(or);
			or = this->_alls[3]->Process(or);
			or = this->_alls[5]->Process(or);
			or = this->_alls[7]->Process(or);

			left[i] = ol * this->_Wet1 + or * this->_Wet2 + left[i] * this->_Dry;
			right[i] = or * this->_Wet1 + ol * this->_Wet2 + right[i] * this->_Dry;
		}
	}
private:
	void Update()
	{
		int i;
        this->_Wet1 = this->_Wet * (this->_Width / 2.0f + 0.5f);
        this->_Wet2 = this->_Wet * ((1.0f - this->_Width) / 2.0f);

        for (i = 0; i < 16; i++)
        {
            this->_combs[i]->setFeedback(this->_Roomsize);
            this->_combs[i]->setDamp(this->_Damp);
        }
	}

	AllPassFilter* _alls[8];
	CombFilter* _combs[16];

	float _Roomsize;
	float _Dry;
	float _Wet;
	float _Wet1;
	float _Wet2;
	float _Damp;
	float _Width;
};
