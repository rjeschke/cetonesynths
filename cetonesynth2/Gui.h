#pragma once

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "Synth.h"
#include "aeffeditor.h"
#include "parameters.h"

#define DRAG_IDLE 50

#define GMB_LEFT 1
#define GMB_MIDDLE 2
#define GMB_RIGHT 4

typedef struct
{
	int len;
	int id[15];
} sGROUP;

class Gui;
class GKnob;

class GObject
{
public:
	GObject(Gui* gui, int id, int x, int y, int w, int h)
	{
		this->_gui = gui;
		this->ID = id;
		this->X = x;
		this->Y = y;
		this->Width = w;
		this->Height = h;
		this->Value = this->OldValue = 0;
		this->Min = 0;
		this->Max = 127;
		this->ValOffset = 0;
		this->Multi = 1;
	};
	~GObject() { };

	virtual int GetValue() { return this->Value / this->Multi; };
	virtual void SetValue(int val) { this->Value = val; this->CheckValue();	};
	virtual void _SetValue(int val) { this->Value = this->OldValue = val  * this->Multi; };
	virtual void ValueChanged() { };
	virtual void RenderTextDisplay() { };
	virtual void Reset()
	{
		this->Value = this->OldValue = 0;
		this->Redraw();
	};

	virtual void CheckValue()
	{
		if(this->Value < this->Min * this->Multi)
			this->Value = this->Min * this->Multi;
		else if(this->Value > this->Max * this->Multi)
			this->Value = this->Max * this->Multi;

		if(this->OldValue != this->Value)
		{
			this->OldValue = this->Value;
			this->Redraw();
			this->ValueChanged();
		}
	};

	virtual void SetMultiplier(int mul)
	{
		this->Multi = mul;
	}

	virtual void SetMinMax(int min, int max)
	{
		this->Min = min;
		this->Max = max;
		this->CheckValue();
	};

	virtual void SetMinMax(int min, int max, int offs)
	{
		this->Min = min;
		this->Max = max;
		this->ValOffset = offs;
		this->CheckValue();
	};

	virtual bool InBounds(int x, int y) 
	{ 
		return 
			((x >= this->X )
			&& (y >= this->Y)
			&& (x < (this->X + this->Width ))
			&& (y < (this->Y + this->Height))) ? true : false;
	};
	
	virtual void Mouse(int x, int y, int b) { };
	virtual void Drag(int dx, int dy) { };
	virtual void DragStart(int x, int y) { this->DragX = x; this->DragY = y; };
	virtual void DragMove(int x, int y) 
	{ 
		int dx = x - this->DragX;
		int dy = y - this->DragY;
		if(dx != 0 || dy != 0)
		{
			this->DragX = x;
			this->DragY = y;
			this->Drag(dx, dy);
		}
	};
	
	virtual void Wheel(int delta)
	{
		int v = this->Value + delta * this->Multi;
		if(this->Multi > 1)
			v /= this->Multi;
		this->SetValue(v * this->Multi);
	};

	virtual void Redraw() { };

	virtual void SetTextPos(int x, int y) {this->TX = x; this->TY = y;};

	int GetDelta(int dx, int dy)
	{
		dy /= 2;
		int x = (dx < 0) ? -dx : dx;
		int y = (dy < 0) ? -dy : dy;
		return (x > y) ? dx : -dy;
	};

	int ID, X, Y, Width, Height, Min, Max, ValOffset, OldValue, Multi;
	int DragX, DragY, isRadio;
	bool bHasTextDisplay;
	Gui* _gui;
protected:
	int Value, TX, TY;
};


class Gui : public AEffEditor
{
public:
	Gui(AudioEffect* effect, Synth* synth, char* gname);
	~Gui();
	bool getRect (ERect** rect);
	bool open (void* ptr);
	void close ();
	VstInt16 Width() { return this->guiSize.right - this->guiSize.left; };
	VstInt16 Height() { return this->guiSize.bottom - this->guiSize.top; };
	void Blit(int sx, int sy, int w, int h, int dx, int dy, HBITMAP bmp);
	void DrawString(int x, int y, char* text);
	void DoIdle();
	bool GetMouseState(int* x, int* y, int* b);
	void ValueChanged(int id, int val);
	void Value2String(int id, int val, char* text);
	void Int2String(int val, char *text);
	void Int2String(int val, char *text, char *ztext);
	void Int2String(int val, char *text, char p0, char p1, char *ztext);
	void CheckGroup(int grp, int id, int val);
	void GroupRadio(int grp, int radio);
	void CheckOSC1(int id, int val);
	void CheckOSC2(int id, int val);
	void CheckOSC3(int id, int val);
	int CheckLFO1();
	int CheckLFO2();
	void ToLocal(int *x, int *y);
	void UpdateObject(int id);
	void ReadProgram();
	void Chaosize();
	void SendCC(int num, int val);
	//bool isOpen ()				{ return systemWindow != 0; }		///< Returns true if editor is currently open
	GObject* objects[OBJ_COUNT];
	HBITMAP bKnobs;
	HBITMAP bSliders;
	HBITMAP bBkg;
	HBITMAP bButtons;
	HBITMAP bChars;
	HBITMAP bOffScreen;
	HBITMAP bLed;
	HBITMAP bArch;
	HBITMAP bCopy;
	bool	bSwapButtons;
	volatile bool bDragging;
	volatile bool bInIdle;
	volatile int dCount;
	Synth*	_Synth;
	int		LedPos[6];
	int		StatusPos[4];
	int		copyLeft;
private:
	int		ArchPos[4];
	int		Chars[2];
	void	InitGui(char* name);
	void	ReadGraphics(char* name);
	bool	prgRead;
	ERect	guiSize;
	HWND	hWnd;
	sGROUP	groups[32];
};

class GKnob : public GObject
{
public:
	GKnob(Gui* gui, int id, int x, int y, int w, int h, int xoffs, HBITMAP img)
	: GObject(gui, id, x, y, w, h)
	{
		this->bBkg = img;
		this->XOffs = xoffs;
		this->bHasTextDisplay = (h == 40) ? true : false;
	};
	~GKnob() { };

	void RenderTextDisplay()
	{
		char text[4];
		this->_gui->Value2String(this->ID, this->GetValue(), text);
		this->_gui->DrawString(this->X + this->TX, this->Y + this->TY, text);
	}

	void ValueChanged()
	{
		this->_gui->ValueChanged(this->ID, this->GetValue());
	}

	void Redraw()
	{
		this->_gui->Blit(
			this->XOffs, 
			(this->Value + this->ValOffset) * this->Height, 
			this->Width,
			this->Height,
			this->X,
			this->Y,
			this->bBkg);
		if(this->bHasTextDisplay)
			this->RenderTextDisplay();
	};

	void Drag(int dx, int dy)
	{
		this->SetValue(this->Value + this->GetDelta(dx, dy));
	};

	void Mouse(int x, int y, int b)
	{
		if(b == GMB_LEFT)
		{
			this->_gui->bDragging = true;
			this->_gui->GetMouseState(&x, &y, &b);
			this->DragStart(x, y);
			while(b & GMB_LEFT)
			{
				this->_gui->GetMouseState(&x, &y, &b);
				if(b & GMB_LEFT)
					this->DragMove(x, y);
				this->_gui->DoIdle();
			}
			this->_gui->bDragging = false;
		}
	};

	HBITMAP bBkg;
	int XOffs;
};

class GButton : public GObject
{
public:
	GButton(Gui* gui, int id, int x, int y, int w, int h, int xoffs, int isRadio, HBITMAP img)
	: GObject(gui, id, x, y, w, h)
	{
		this->bBkg = img;
		this->XOffs = xoffs;
		this->isRadio = isRadio;
	};
	~GButton() { };

	void ValueChanged()
	{
		this->_gui->ValueChanged(this->ID, this->Value);
	}

	void Redraw()
	{
		this->_gui->Blit(
			this->XOffs, 
			this->Value * this->Height, 
			this->Width,
			this->Height,
			this->X,
			this->Y,
			this->bBkg);
	};

	void CheckValue()
	{
		if (this->Value)
			this->Value = 1;

		if(this->OldValue != this->Value)
		{
			this->OldValue = this->Value;
			this->Redraw();
			this->ValueChanged();
		}
	};

	void SetValue(int val) 
	{ 
		this->Value = (val) ? 1 : 0; 
		this->CheckValue();
	};

	void _SetValue(int val) 
	{ 
		this->Value = this->OldValue = ((val) ? 1 : 0); 
	};

	int GetValue() 
	{ 
		return this->Value; 
	};

	void Wheel(int delta) {	};

	void Mouse(int x, int y, int b)
	{
		if(b == GMB_LEFT)
		{
			if(this->Value)
			{
				if(!this->isRadio)
					this->Value = 0;
			}
			else
				this->Value = 1;
			this->CheckValue();
		}
	};

	HBITMAP bBkg;
	int XOffs;
};

class GTrigger : public GObject
{
public:
	GTrigger(Gui* gui, int id, int x, int y, int w, int h, int xoffs, HBITMAP img)
	: GObject(gui, id, x, y, w, h)
	{
		this->bBkg = img;
		this->XOffs = xoffs;
	};
	~GTrigger() { };

	void ValueChanged()
	{
		this->_gui->ValueChanged(this->ID, this->Value);
	}

	void Redraw()
	{
		this->_gui->Blit(
			this->XOffs, 
			this->Value * this->Height, 
			this->Width,
			this->Height,
			this->X,
			this->Y,
			this->bBkg);
	};

	void CheckValue()
	{
		if (this->Value)
			this->Value = 1;
	};

	void SetValue(int val) 
	{ 
		this->Value = (val) ? 1 : 0; 
	};

	void _SetValue(int val) 
	{ 
		this->Value = this->OldValue = ((val) ? 1 : 0); 
	};

	int GetValue() 
	{ 
		return this->Value; 
	};

	void Wheel(int delta) {	};

	void Mouse(int x, int y, int b)
	{
		if((b == GMB_LEFT) && !this->Value)
		{
			this->_SetValue(1);
			this->Redraw();
			this->_gui->bDragging = true;
			this->_gui->GetMouseState(&x, &y, &b);
			this->DragStart(x, y);
			while(b & GMB_LEFT)
			{
				this->_gui->GetMouseState(&x, &y, &b);
				this->_gui->DoIdle();
			}
			this->_gui->bDragging = false;
			this->_SetValue(0);
			this->Redraw();
			this->_gui->ToLocal(&x, &y);
			if(this->InBounds(x, y))
				this->ValueChanged();
		}
	};

	HBITMAP bBkg;
	int XOffs;
};
