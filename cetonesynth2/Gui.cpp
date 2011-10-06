#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "defines.h"
#include "Gui.h"
#include "resource.h"

#pragma warning(disable: 4244 4312) 

extern void* hInstance;
static int g_refCount = 0;
static int g_finit = 0;
static int g_copyShown = 0;

static char* g_modSrc[] = 	
	{
		" off", 
		" Vel", "KTrk", "LFO1", "LFO2", 
		"Env1", "Env2", "L1E1", "L1L2",
		"ModW", "MwL2", "Aftr", "AInL",
		"AInR"
	};

#define MAX_SRC 13
#define MUL_SRC (127/MAX_SRC)

static char* g_modDst[] =	
	{
		" FCo", "  FQ", "FVol", " Vol", 
		" Pan", "Vol1", "Vol2", "Vol3",
		"Ptc1", "Ptc2", "Ptc3", "Mrp1",
		"Mrp2", "Mrp3", "Spr1", "Spr2",
		"Spr3",	"L1Sp", "L2Sp", "Ptch",
		"Amt6", "AVol", "APan"
	};

#define MAX_DEST 22
#define MUL_DEST (127/MAX_DEST)

static char* g_syncNames[129] =	{
 " off","32b.","32b ","32bt","31b.","31b ","31bt","30b.",
 "30b ","30bt","29b.","29b ","29bt","28b.","28b ","28bt",
 "27b.","27b ","27bt","26b.","26b ","26bt","25b.","25b ",
 "25bt","24b.","24b ","24bt","23b.","23b ","23bt","22b.",
 "22b ","22bt","21b.","21b ","21bt","20b.","20b ","20bt",
 "19b.","19b ","19bt","18b.","18b ","18bt","17b.","17b ",
 "17bt","16b.","16b ","16bt","15b.","15b ","15bt","14b.",
 "14b ","14bt","13b.","13b ","13bt","12b.","12b ","12bt",
 "11b.","11b ","11bt","10b.","10b ","10bt"," 9b."," 9b ",
 " 9bt"," 8b."," 8b "," 8bt"," 7b."," 7b "," 7bt"," 6b.",
 " 6b "," 6bt"," 5b."," 5b "," 5bt"," 4b."," 4b "," 4bt",
 " 3b."," 3b "," 3bt"," 2b."," 2b "," 2bt"," 1b."," 1b ",
 " 1bt","  2.","  2 ","  2t","  4.","  4 ","  4t","  8.",
 "  8 ","  8t"," 16."," 16 "," 16t"," 32."," 32 "," 32t",
 " 64."," 64 "," 64t","128.","128 ","128t","128t","128t",
 "128t","128t","128t","128t","128t","128t","128t","128t", "128t"};

inline HINSTANCE GetInstance() { return (HINSTANCE)hInstance; }
LONG_PTR WINAPI WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static int Read32(FILE* f)
{
	int ret = fgetc(f);
	ret |= fgetc(f) << 8;
	ret |= fgetc(f) << 16;
	ret |= fgetc(f) << 24;
	return ret;
}

static int ReadS16(FILE* f)
{
	int ret = fgetc(f);
	ret |= fgetc(f) << 8;
	if(ret > 32767)
		ret -= 65536;
	return ret;
}

static HBITMAP ReadBitmap(FILE *f)
{
	int width, height, x, y;
	HBITMAP bmp;
	BITMAPINFO bi;
	unsigned __int8* data;
	
	width = Read32(f);
	height = Read32(f);

	memset(&bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;

	int pitch = (width * 3 + 3) & (~3);
	data = (unsigned __int8*)malloc(pitch * height);
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			data[(height - 1 - y) * pitch + x * 3 + 2] = (BYTE)fgetc(f);
			data[(height - 1 - y) * pitch + x * 3 + 1] = (BYTE)fgetc(f);
			data[(height - 1 - y) * pitch + x * 3] = (BYTE)fgetc(f);
		}
	}

	HDC hdc = GetDC(GetDesktopWindow());
	HDC chdc = CreateCompatibleDC(hdc);
	bmp = CreateCompatibleBitmap(hdc, width, height);

	SelectObject(chdc, bmp);
	SetDIBits(chdc, bmp, 0, height, data, &bi, DIB_RGB_COLORS);
	DeleteDC(chdc);
	ReleaseDC(GetDesktopWindow(), hdc);
	
	free(data);

	return bmp;
}

void Gui::InitGui(char *name)
{
	int i, offs;
	int knob[4], button[2], slider[2];
	FILE* f;
	fopen_s(&f, name, "rb");

	offs = Read32(f);

	fseek(f, offs, SEEK_SET);

	this->bBkg = ReadBitmap(f);
	this->bButtons = ReadBitmap(f);
	this->bKnobs = ReadBitmap(f);
	this->bSliders = ReadBitmap(f);
	this->bLed = ReadBitmap(f);
	this->bChars = ReadBitmap(f);
	this->bArch = ReadBitmap(f);

	fseek(f, 4, SEEK_SET);

	knob[0] = ReadS16(f);
	knob[1] = ReadS16(f);
	knob[2] = ReadS16(f);
	knob[3] = ReadS16(f);
	button[0] = ReadS16(f);
	button[1] = ReadS16(f);
	slider[0] = ReadS16(f);
	slider[1] = ReadS16(f);
	this->ArchPos[0] = ReadS16(f);
	this->ArchPos[1] = ReadS16(f);
	this->ArchPos[2] = ReadS16(f);
	this->ArchPos[3] = ReadS16(f);
	this->Chars[0] = ReadS16(f);
	this->Chars[1] = ReadS16(f);
	this->LedPos[0] = ReadS16(f);
	this->LedPos[1] = ReadS16(f);
	this->LedPos[2] = ReadS16(f);
	this->LedPos[3] = ReadS16(f);
	this->LedPos[4] = ReadS16(f);
	this->LedPos[5] = ReadS16(f);
	this->StatusPos[0] = ReadS16(f);
	this->StatusPos[1] = ReadS16(f);
	this->StatusPos[2] = ReadS16(f);
	this->StatusPos[3] = ReadS16(f);

	for(i = 0; i < OBJ_COUNT; i++)
	{
		int in[8], n;
		for(n = 0; n < 8; n++)
			in[n] = ReadS16(f);

		switch(in[0])
		{
		case 0:		// BUTTON
			this->objects[i] = new GButton(this, i, in[1], in[2], button[0], button[1], in[3], 0, this->bButtons);
			break;
		case 1:		// RBUTTON
			this->objects[i] = new GButton(this, i, in[1], in[2], button[0], button[1], in[3], -1, this->bButtons);
			break;
		case 2:		// TRIGGER
			this->objects[i] = new GTrigger(this, i, in[1], in[2], button[0], button[1], in[3], this->bButtons);
			break;
		case 3:		// KNOB
			this->objects[i] = new GKnob(this, i, in[1], in[2], knob[0], knob[1], in[3], this->bKnobs);
			this->objects[i]->SetMinMax(in[4], in[5], in[6]);
			this->objects[i]->SetMultiplier(in[7]);
			this->objects[i]->SetTextPos(knob[2], knob[3]);
			break;
		case 4:		// SLIDER
			this->objects[i] = new GKnob(this, i, in[1], in[2], slider[0], slider[1], in[3], this->bSliders);
			this->objects[i]->SetMinMax(in[4], in[5]);
			break;
		}
	}

	fclose(f);
}

Gui::Gui(AudioEffect *effect, Synth* synth, char* gname)
{
	this->effect = effect;
	this->_Synth = synth;

	memset(this->objects, 0, sizeof(GObject*) * OBJ_COUNT);

	this->InitGui(gname);

	this->bCopy = LoadBitmap(GetInstance(), MAKEINTRESOURCE(IDB_BITMAP1));

	BITMAP bm;
	GetObject(this->bBkg, sizeof(BITMAP), &bm);
	this->guiSize.left = this->guiSize.top = 0;
	this->guiSize.bottom = (VstInt16) bm.bmHeight;
	this->guiSize.right = (VstInt16) bm.bmWidth;
	this->hWnd = 0;
	this->bInIdle = false;
	this->bDragging = false;
	this->dCount = 0;
	this->prgRead = false;

	this->groups[0].len = 2;
	this->groups[0].id[0] = kF12dB;
	this->groups[0].id[1] = kF24dB;

	this->groups[1].len = 4;
	this->groups[1].id[0] = kFLP;
	this->groups[1].id[1] = kFBP;
	this->groups[1].id[2] = kFHP;
	this->groups[1].id[3] = kFBR;

	this->groups[2].len = 5;
	this->groups[2].id[0] = kOOff1;
	this->groups[2].id[1] = kOSaw1;
	this->groups[2].id[2] = kOPulse1;
	this->groups[2].id[3] = kOTri1;
	this->groups[2].id[4] = kONoise1;
	this->groups[3].len = 4;
	this->groups[3].id[0] = kONormal1;
	this->groups[3].id[1] = kOAdd1;
	this->groups[3].id[2] = kOMul1;
	this->groups[3].id[3] = kOSuper1;

	this->groups[4].len = 5;
	this->groups[4].id[0] = kOOff2;
	this->groups[4].id[1] = kOSaw2;
	this->groups[4].id[2] = kOPulse2;
	this->groups[4].id[3] = kOTri2;
	this->groups[4].id[4] = kONoise2;
	this->groups[5].len = 4;
	this->groups[5].id[0] = kONormal2;
	this->groups[5].id[1] = kOAdd2;
	this->groups[5].id[2] = kOMul2;
	this->groups[5].id[3] = kOSuper2;

	this->groups[6].len = 5;
	this->groups[6].id[0] = kOOff3;
	this->groups[6].id[1] = kOSaw3;
	this->groups[6].id[2] = kOPulse3;
	this->groups[6].id[3] = kOTri3;
	this->groups[6].id[4] = kONoise3;
	this->groups[7].len = 4;
	this->groups[7].id[0] = kONormal3;
	this->groups[7].id[1] = kOAdd3;
	this->groups[7].id[2] = kOMul3;
	this->groups[7].id[3] = kOSuper3;

	this->groups[8].len = 5;
	this->groups[8].id[0] = kL1Off;
	this->groups[8].id[1] = kL1Saw;
	this->groups[8].id[2] = kL1Pulse;
	this->groups[8].id[3] = kL1Tri;
	this->groups[8].id[4] = kL1Noise;

	this->groups[9].len = 5;
	this->groups[9].id[0] = kL2Off;
	this->groups[9].id[1] = kL2Saw;
	this->groups[9].id[2] = kL2Pulse;
	this->groups[9].id[3] = kL2Tri;
	this->groups[9].id[4] = kL2Noise;

	this->objects[kM1Source]->SetMinMax(0, MAX_SRC);
	this->objects[kM1Source]->SetMultiplier(MUL_SRC);
	this->objects[kM2Source]->SetMinMax(0, MAX_SRC);
	this->objects[kM2Source]->SetMultiplier(MUL_SRC);
	this->objects[kM3Source]->SetMinMax(0, MAX_SRC);
	this->objects[kM3Source]->SetMultiplier(MUL_SRC);
	this->objects[kM4Source]->SetMinMax(0, MAX_SRC);
	this->objects[kM4Source]->SetMultiplier(MUL_SRC);
	this->objects[kM5Source]->SetMinMax(0, MAX_SRC);
	this->objects[kM5Source]->SetMultiplier(MUL_SRC);
	this->objects[kM6Source]->SetMinMax(0, MAX_SRC);
	this->objects[kM6Source]->SetMultiplier(MUL_SRC);

	this->objects[kM1Dest]->SetMinMax(0, MAX_DEST);
	this->objects[kM1Dest]->SetMultiplier(MUL_DEST);
	this->objects[kM2Dest]->SetMinMax(0, MAX_DEST);
	this->objects[kM2Dest]->SetMultiplier(MUL_DEST);
	this->objects[kM3Dest]->SetMinMax(0, MAX_DEST);
	this->objects[kM3Dest]->SetMultiplier(MUL_DEST);
	this->objects[kM4Dest]->SetMinMax(0, MAX_DEST);
	this->objects[kM4Dest]->SetMultiplier(MUL_DEST);
	this->objects[kM5Dest]->SetMinMax(0, MAX_DEST);
	this->objects[kM5Dest]->SetMultiplier(MUL_DEST);
	this->objects[kM6Dest]->SetMinMax(0, MAX_DEST);
	this->objects[kM6Dest]->SetMultiplier(MUL_DEST);

	
}

Gui::~Gui()
{
	int i;
	DeleteObject(this->bBkg);
	DeleteObject(this->bButtons);
	DeleteObject(this->bKnobs);
	DeleteObject(this->bChars);
	DeleteObject(this->bSliders);
	DeleteObject(this->bLed);
	DeleteObject(this->bArch);
	DeleteObject(this->bCopy);

	for(i = 0; i < OBJ_COUNT; i++)
	{
		if(this->objects[i] != 0)
			delete this->objects[i];
	}
}

void Gui::ToLocal(int *x, int *y)
{
	RECT r;
	GetWindowRect(this->hWnd, &r);
	*x -= r.left;
	*y -= r.top;
}

bool Gui::getRect(ERect **rect)
{
	*rect = &(this->guiSize);
	return true;
}

bool Gui::open(void *ptr)
{
	this->systemWindow = ptr;

	if(!g_copyShown)
		this->copyLeft = 200;
	else
		this->copyLeft = 0;

	g_refCount++;
	if (g_refCount == 1)
	{
		WNDCLASS windowClass;
		windowClass.style = CS_GLOBALCLASS;
		windowClass.lpfnWndProc = WindowProc; 
		windowClass.cbClsExtra  = 0; 
		windowClass.cbWndExtra  = 0; 
		windowClass.hInstance   = GetInstance(); 
		windowClass.hIcon = 0; 
		windowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName  = 0; 
		windowClass.lpszClassName = CLASS_NAME; 
		RegisterClass(&windowClass);
		this->bSwapButtons = GetSystemMetrics(SM_SWAPBUTTON) > 0;
	}

	this->hWnd = CreateWindowEx
		(0, 
		CLASS_NAME, 
		"Window",
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
		0, 
		0, 
		this->Width(), 
		this->Height(), 
		(HWND)this->systemWindow, 
		NULL, 
		GetInstance(), 
		NULL);

	SetWindowLongPtr(this->hWnd, GWLP_USERDATA, (LONG_PTR)this);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);

	// Create offscreen surface and blit background onto it
	this->bOffScreen = CreateCompatibleBitmap(GetDC(this->hWnd), this->Width(), this->Height());
	HDC hdc = GetDC(this->hWnd);
	HDC hdc1 = CreateCompatibleDC(hdc);
	HDC hdc2 = CreateCompatibleDC(hdc);
	SelectObject(hdc1, this->bOffScreen);
	SelectObject(hdc2, this->bBkg);
	BitBlt(hdc1, 0, 0, this->Width(), this->Height(), hdc2, 0, 0, SRCCOPY);
	DeleteDC(hdc1);
	DeleteDC(hdc2);
	ReleaseDC(this->hWnd, hdc);

	this->bInIdle = false;
	this->bDragging = false;

	SetTimer(this->hWnd, 1, 10, (TIMERPROC) NULL);


	this->_Synth->SetGuiFlag(true);
	this->ReadProgram();
#if _M_IX86_FP == 1
	this->Blit(0, this->ArchPos[1] * 1, this->ArchPos[0], this->ArchPos[1], this->ArchPos[2], this->ArchPos[3], this->bArch);
#elif _M_IX86_FP == 2
	this->Blit(0, this->ArchPos[1] * 2, this->ArchPos[0], this->ArchPos[1], this->ArchPos[2], this->ArchPos[3], this->bArch);
#else
	this->Blit(0, 0, this->ArchPos[0], this->ArchPos[1], this->ArchPos[2], this->ArchPos[3], this->bArch);
#endif
	
	if(this->_Synth->HasPrgChanged())
	{
		this->objects[kPatchStore]->SetValue(0);
		this->objects[kPatchStore]->Redraw();
	}
	else
	{
		this->objects[kPatchStore]->SetValue(1);
		this->objects[kPatchStore]->Redraw();
	}
	
	return true;
}

void Gui::close()
{
	KillTimer(this->hWnd, 1);

	this->systemWindow = 0;
	
	if(this->hWnd)
	{
		SetWindowLongPtr(this->hWnd, GWLP_USERDATA, (long)NULL);
		DestroyWindow(this->hWnd);
		DeleteObject(this->bOffScreen);
		this->hWnd = 0;
	}

	if(g_refCount)
	{
		g_refCount--;
		if(!g_refCount)
			UnregisterClass (CLASS_NAME, GetInstance ());
	}

	this->_Synth->SetGuiFlag(false);
}

void Gui::DrawString(int x, int y, char* text)
{
	RECT r;
	HDC hdc = GetDC(this->hWnd);
	HDC hdc1 = CreateCompatibleDC(hdc);
	HDC hdc2 = CreateCompatibleDC(hdc);
	int i;

	SelectObject(hdc1, this->bOffScreen);
	SelectObject(hdc2, this->bChars);
	for(i = 0; i < 4; i++)
		BitBlt(hdc1, x + i * this->Chars[0], y, this->Chars[0], this->Chars[1], hdc2, (int)text[i] * this->Chars[0], 0, SRCCOPY);

	DeleteDC(hdc1);
	DeleteDC(hdc2);
	ReleaseDC(this->hWnd, hdc);

	r.top = y;
	r.bottom = y + this->Chars[1];
	r.left = x;
	r.right = x + this->Chars[1] * 4;
	
	if(!this->prgRead)
		InvalidateRect(this->hWnd, &r, false); 
}

void Gui::Blit(int sx, int sy, int w, int h, int dx, int dy, HBITMAP bmp)
{
	RECT r;
	HDC hdc = GetDC(this->hWnd);
	HDC hdc1 = CreateCompatibleDC(hdc);
	HDC hdc2 = CreateCompatibleDC(hdc);
	
	SelectObject(hdc1, this->bOffScreen);
	SelectObject(hdc2, bmp);
	BitBlt(hdc1, dx, dy, w, h, hdc2, sx, sy, SRCCOPY);
	DeleteDC(hdc1);
	DeleteDC(hdc2);
	ReleaseDC(this->hWnd, hdc);
	
	r.top = dy;
	r.bottom = dy + h;
	r.left = dx;
	r.right = dx + w;

	if(!this->prgRead)
		InvalidateRect(this->hWnd, &r, false); 
}

void Gui::DoIdle()
{
	this->bInIdle = true;
	Sleep(20);
	this->getEffect()->masterIdle();

	MSG wm;
	if (PeekMessage (&wm, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
		DispatchMessage (&wm);

	this->bInIdle = false;
}

bool Gui::GetMouseState(int *x, int *y, int *b)
{
	POINT p;
	if(!GetCursorPos(&p))
		return false;

	*x = (int)p.x;
	*y = (int)p.y;
	*b = 0;

	if(GetAsyncKeyState(VK_LBUTTON) < 0)
		*b |= (this->bSwapButtons) ? GMB_RIGHT : GMB_LEFT;
	if(GetAsyncKeyState(VK_MBUTTON) < 0)
		*b |= GMB_MIDDLE;
	if(GetAsyncKeyState(VK_RBUTTON) < 0)
		*b |= (this->bSwapButtons) ? GMB_LEFT : GMB_RIGHT;

	return true;
}

void Gui::Int2String(int val, char *text)
{
	int i = 3;
	if(val < 0)
	{
		val = -val;
		while(i >= 0)
		{
			text[i] = (char)((val % 10) + 48);
			i--;
			val /= 10;
			if(val == 0)
				break;
		};
		if(i >= 0)
			text[i] = '-';
	}
	else
	{
		while(i >= 0)
		{
			text[i] = (char)((val % 10) + 48);
			i--;
			val /= 10;
			if(val == 0)
				break;
		};
	}
}

void Gui::Int2String(int val, char *text, char p0, char p1, char* ztext)
{
	int i = 3;
	if(val == 0)
	{
		text[0] = ztext[0];
		text[1] = ztext[1];
		text[2] = ztext[2];
		text[3] = ztext[3];
	}else if(val < 0)
	{
		val = -val;
		while(i >= 0)
		{
			text[i] = (char)((val % 10) + 48);
			i--;
			val /= 10;
			if(val == 0)
				break;
		};
		if(i >= 0)
			text[i] = p0;
	}
	else
	{
		while(i >= 0)
		{
			text[i] = (char)((val % 10) + 48);
			i--;
			val /= 10;
			if(val == 0)
				break;
		};
		if(i >= 0)
			text[i] = p1;
	}
}

void Gui::Int2String(int val, char *text, char* ztext)
{
	if(val == 0)
	{
		text[0] = ztext[0];
		text[1] = ztext[1];
		text[2] = ztext[2];
		text[3] = ztext[3];
	}
	else
	{
		int i = 3;
		while(i >= 0)
		{
			text[i] = (char)((val % 10) + 48);
			i--;
			val /= 10;
			if(val == 0)
				break;
		};
	}
}

void Gui::CheckGroup(int grp, int id, int val)
{
	if(val)
	{
		int i;
		for(i = 0; i < this->groups[grp].len; i++)
		{
			if(this->groups[grp].id[i] != id)
				this->objects[this->groups[grp].id[i]]->Reset();
		}
	}
}

void Gui::GroupRadio(int grp, int radio)
{
	int i;
	for(i = 0; i < this->groups[grp].len; i++)
		this->objects[this->groups[grp].id[i]]->isRadio = radio;
}


void Gui::CheckOSC1(int id, int val)
{
	int i;
	if(val)
	{
		switch(id)
		{
		case kOAdd1:
		case kOMul1:
			this->CheckGroup(3, id, val);
			if(this->objects[kOSaw1]->isRadio)
			{
				this->GroupRadio(2, 0);
				this->objects[kOOff1]->isRadio = -1;
			}
			break;
		case kONormal1:
		case kOSuper1:
			this->CheckGroup(3, id, val);
			if(!this->objects[kOSaw1]->isRadio)
			{
				this->GroupRadio(2, -1);
				this->objects[kOOff1]->_SetValue(1);
				this->objects[kOOff1]->Redraw();
				this->CheckGroup(2, kOOff1, 1);
			}
			break;
		}
	}

	if(id == kOOff1 || id == kOSaw1 || id == kOTri1 || id == kOPulse1 || id == kONoise1)
	{
		switch(this->_Synth->sPrg.osc[0].mode)
		{
		case OSCM_ADD:
		case OSCM_MUL:
			if(val)
			{
				if(id == kOOff1)
				{
					for(i = 1; i < 5; i++)
						this->objects[this->groups[2].id[i]]->Reset();
				}
				else
					this->objects[kOOff1]->Reset();
			}
			break;
		case OSCM_NORMAL:
		case OSCM_SUPER:
			this->CheckGroup(2, id, val);
			break;
		}
	}
	
	int w = 0;
	if(this->objects[kOSaw1]->GetValue())
		w |= OSCW_SAW;
	if(this->objects[kOTri1]->GetValue())
		w |= OSCW_TRI;
	if(this->objects[kOPulse1]->GetValue())
		w |= OSCW_PULSE;
	if(this->objects[kONoise1]->GetValue())
		w |= OSCW_NOISE;

	if(!w)
	{
		this->objects[kOOff1]->_SetValue(1);
		this->objects[kOOff1]->Redraw();
	}
	if(this->objects[kONormal1]->GetValue())
		this->_Synth->SetParameter(pOMode1, OSCM_NORMAL);
	else if(this->objects[kOAdd1]->GetValue())
		this->_Synth->SetParameter(pOMode1, OSCM_ADD);
	else if(this->objects[kOMul1]->GetValue())
		this->_Synth->SetParameter(pOMode1, OSCM_MUL);
	else if(this->objects[kOSuper1]->GetValue())
		this->_Synth->SetParameter(pOMode1, OSCM_SUPER);

	this->_Synth->SetParameter(pOWave1, w);

	w = 0;
	if(this->objects[kOSync1]->GetValue()) w |= 0x01;
	if(this->objects[kOAM1]->GetValue()) w |= 0x02;
	if(this->objects[kOKTrk1]->GetValue()) w |= 0x80;
	this->_Synth->SetParameter(pOFlags1, w);
}

void Gui::CheckOSC2(int id, int val)
{
	int i;
	if(val)
	{
		switch(id)
		{
		case kOAdd2:
		case kOMul2:
			this->CheckGroup(5, id, val);
			if(this->objects[kOSaw2]->isRadio)
			{
				this->GroupRadio(4, 0);
				this->objects[kOOff2]->isRadio = -1;
			}
			break;
		case kONormal2:
		case kOSuper2:
			this->CheckGroup(5, id, val);
			if(!this->objects[kOSaw2]->isRadio)
			{
				this->GroupRadio(4, -1);
				this->objects[kOOff2]->_SetValue(1);
				this->objects[kOOff2]->Redraw();
				this->CheckGroup(4, kOOff2, 1);
			}
			break;
		}
	}

	if(id == kOOff2 || id == kOSaw2 || id == kOTri2 || id == kOPulse2 || id == kONoise2)
	{
		switch(this->_Synth->sPrg.osc[1].mode)
		{
		case OSCM_ADD:
		case OSCM_MUL:
			if(val)
			{
				if(id == kOOff2)
				{
					for(i = 1; i < 5; i++)
						this->objects[this->groups[4].id[i]]->Reset();
				}
				else
					this->objects[kOOff2]->Reset();
			}
			break;
		case OSCM_NORMAL:
		case OSCM_SUPER:
			this->CheckGroup(4, id, val);
			break;
		}
	}
	
	int w = 0;
	if(this->objects[kOSaw2]->GetValue())
		w |= OSCW_SAW;
	if(this->objects[kOTri2]->GetValue())
		w |= OSCW_TRI;
	if(this->objects[kOPulse2]->GetValue())
		w |= OSCW_PULSE;
	if(this->objects[kONoise2]->GetValue())
		w |= OSCW_NOISE;

	if(!w)
	{
		this->objects[kOOff2]->_SetValue(1);
		this->objects[kOOff2]->Redraw();
	}

	if(this->objects[kONormal2]->GetValue())
		this->_Synth->SetParameter(pOMode2, OSCM_NORMAL);
	else if(this->objects[kOAdd2]->GetValue())
		this->_Synth->SetParameter(pOMode2, OSCM_ADD);
	else if(this->objects[kOMul2]->GetValue())
		this->_Synth->SetParameter(pOMode2, OSCM_MUL);
	else if(this->objects[kOSuper2]->GetValue())
		this->_Synth->SetParameter(pOMode2, OSCM_SUPER);

	this->_Synth->SetParameter(pOWave2, w);

	w = 0;
	if(this->objects[kOSync2]->GetValue()) w |= 0x01;
	if(this->objects[kOAM2]->GetValue()) w |= 0x02;
	if(this->objects[kOKTrk2]->GetValue()) w |= 0x80;
	this->_Synth->SetParameter(pOFlags2, w);
}

void Gui::CheckOSC3(int id, int val)
{
	int i;
	if(val)
	{
		switch(id)
		{
		case kOAdd3:
		case kOMul3:
			this->CheckGroup(7, id, val);
			if(this->objects[kOSaw3]->isRadio)
			{
				this->GroupRadio(6, 0);
				this->objects[kOOff3]->isRadio = -1;
			}
			break;
		case kONormal3:
		case kOSuper3:
			this->CheckGroup(7, id, val);
			if(!this->objects[kOSaw3]->isRadio)
			{
				this->GroupRadio(6, -1);
				this->objects[kOOff3]->_SetValue(1);
				this->objects[kOOff3]->Redraw();
				this->CheckGroup(6, kOOff3, 1);
			}
			break;
		}
	}

	if(id == kOOff3 || id == kOSaw3 || id == kOTri3 || id == kOPulse3 || id == kONoise3)
	{
		switch(this->_Synth->sPrg.osc[2].mode)
		{
		case OSCM_ADD:
		case OSCM_MUL:
			if(val)
			{
				if(id == kOOff3)
				{
					for(i = 1; i < 5; i++)
						this->objects[this->groups[6].id[i]]->Reset();
				}
				else
					this->objects[kOOff3]->Reset();
			}
			break;
		case OSCM_NORMAL:
		case OSCM_SUPER:
			this->CheckGroup(6, id, val);
			break;
		}
	}
	
	int w = 0;
	if(this->objects[kOSaw3]->GetValue())
		w |= OSCW_SAW;
	if(this->objects[kOTri3]->GetValue())
		w |= OSCW_TRI;
	if(this->objects[kOPulse3]->GetValue())
		w |= OSCW_PULSE;
	if(this->objects[kONoise3]->GetValue())
		w |= OSCW_NOISE;

	if(!w)
	{
		this->objects[kOOff3]->_SetValue(1);
		this->objects[kOOff3]->Redraw();
	}

	if(this->objects[kONormal3]->GetValue())
		this->_Synth->SetParameter(pOMode3, OSCM_NORMAL);
	else if(this->objects[kOAdd3]->GetValue())
		this->_Synth->SetParameter(pOMode3, OSCM_ADD);
	else if(this->objects[kOMul3]->GetValue())
		this->_Synth->SetParameter(pOMode3, OSCM_MUL);
	else if(this->objects[kOSuper3]->GetValue())
		this->_Synth->SetParameter(pOMode3, OSCM_SUPER);

	this->_Synth->SetParameter(pOWave3, w);

	w = 0;
	if(this->objects[kOSync3]->GetValue()) w |= 0x01;
	if(this->objects[kOAM3]->GetValue()) w |= 0x02;
	if(this->objects[kOKTrk3]->GetValue()) w |= 0x80;
	this->_Synth->SetParameter(pOFlags3, w);
}

int Gui::CheckLFO1()
{
	if(this->objects[kL1Saw]->GetValue())
		return OSCW_SAW;
	if(this->objects[kL1Pulse]->GetValue())
		return OSCW_PULSE;
	if(this->objects[kL1Tri]->GetValue())
		return OSCW_TRI;
	if(this->objects[kL1Noise]->GetValue())
		return OSCW_NOISE;
	return 0;
}

int Gui::CheckLFO2()
{
	if(this->objects[kL2Saw]->GetValue())
		return OSCW_SAW;
	if(this->objects[kL2Pulse]->GetValue())
		return OSCW_PULSE;
	if(this->objects[kL2Tri]->GetValue())
		return OSCW_TRI;
	if(this->objects[kL2Noise]->GetValue())
		return OSCW_NOISE;
	return 0;
}


#define CC(x,y) this->SendCC(x,y)
void Gui::ValueChanged(int id, int val)
{
	if(val)
	{
		switch(id)
		{
		case kF12dB: this->CheckGroup(0, id, val); this->_Synth->SetParameter(pFMode, FILTM_12DB); break;
		case kF24dB: this->CheckGroup(0, id, val); this->_Synth->SetParameter(pFMode, FILTM_24DB); break;
		case kFLP: this->CheckGroup(1, id, val); this->_Synth->SetParameter(pFType, FILTT_LP); break;
		case kFBP: this->CheckGroup(1, id, val); this->_Synth->SetParameter(pFType, FILTT_BP); break;
		case kFHP: this->CheckGroup(1, id, val); this->_Synth->SetParameter(pFType, FILTT_HP); break;
		case kFBR: this->CheckGroup(1, id, val); this->_Synth->SetParameter(pFType, FILTT_BR); break;
		case kL1Off: case kL1Saw: case kL1Pulse: case kL1Tri: case kL1Noise:
			this->CheckGroup(8, id, val);
			this->_Synth->SetParameter(pL1Wave, this->CheckLFO1());
			break;
		case kL2Off: case kL2Saw: case kL2Pulse: case kL2Tri: case kL2Noise:
			this->CheckGroup(9, id, val);
			this->_Synth->SetParameter(pL2Wave, this->CheckLFO2());
			break;
		}
	}
	switch(id)
	{
	case kVoices: this->_Synth->SetParameter(pVoices, val); break;
	case kGlide: this->_Synth->SetParameter(pGlide, val); CC(5, val); break;
	case kCutoff: this->_Synth->SetParameter(pFCutoff, val); CC(74, val); break;
	case kReso: this->_Synth->SetParameter(pFQ, val); CC(71, val); break;
	case kFGain: this->_Synth->SetParameter(pFGain, val + 64); CC(12, val + 64); break;
	case kFVel: this->_Synth->SetParameter(pFVelsens, val + 64); break;
	case kFEnv: this->_Synth->SetParameter(pFEnv2, val + 64); CC(13, val + 64); break;
	case kFExp: this->_Synth->SetParameter(pFExp, val); break;

	case kMixVol: this->_Synth->SetParameter(pMixVol, val); CC(7, val); break;
	case kMixPan: this->_Synth->SetParameter(pMixPan, val + 64); CC(10, val + 64); break;
	case kMixVel: this->_Synth->SetParameter(pMixVelsens, val + 64); break;
	case kMixVol1: this->_Synth->SetParameter(pMixVol1, val); CC(26, val); break;
	case kMixVol2: this->_Synth->SetParameter(pMixVol2, val); CC(27, val); break;
	case kMixVol3: this->_Synth->SetParameter(pMixVol3, val); CC(28, val); break;

	case kAttack1: this->_Synth->SetParameter(pAttack1, val); break;
	case kHold1: this->_Synth->SetParameter(pHold1, val); break;
	case kDecay1: this->_Synth->SetParameter(pDecay1, val); break;
	case kSustain1: this->_Synth->SetParameter(pSustain1, val); break;
	case kRelease1: this->_Synth->SetParameter(pRelease1, val); break;

	case kAttack2: this->_Synth->SetParameter(pAttack2, val); break;
	case kHold2: this->_Synth->SetParameter(pHold2, val); break;
	case kDecay2: this->_Synth->SetParameter(pDecay2, val); break;
	case kSustain2: this->_Synth->SetParameter(pSustain2, val); break;
	case kRelease2: this->_Synth->SetParameter(pRelease2, val); break;

	case kAttack3: this->_Synth->SetParameter(pAttack3, val); break;
	case kHold3: this->_Synth->SetParameter(pHold3, val); break;
	case kDecay3: this->_Synth->SetParameter(pDecay3, val); break;
	case kSustain3: this->_Synth->SetParameter(pSustain3, val); break;
	case kRelease3: this->_Synth->SetParameter(pRelease3, val); break;

	case kArpSpeed: this->_Synth->SetParameter(pArpSpeed, val); CC(50, val); break;
	case kArpLength: this->_Synth->SetParameter(pArpLength, val); CC(51, val << 4); break;
	case kArpNote1: this->_Synth->SetParameter(pArpNote1, val + 64); CC(52, val + 64); break;
	case kArpNote2: this->_Synth->SetParameter(pArpNote2, val + 64); CC(53, val + 64); break;
	case kArpNote3: this->_Synth->SetParameter(pArpNote3, val + 64); CC(54, val + 64); break;
	case kArpNote4: this->_Synth->SetParameter(pArpNote4, val + 64); CC(55, val + 64); break;
	case kArpNote5: this->_Synth->SetParameter(pArpNote5, val + 64); CC(56, val + 64); break;
	case kArpNote6: this->_Synth->SetParameter(pArpNote6, val + 64); CC(57, val + 64); break;
	case kArpNote7: this->_Synth->SetParameter(pArpNote7, val + 64); CC(58, val + 64); break;
	case kArpNote8: this->_Synth->SetParameter(pArpNote8, val + 64); CC(59, val + 64); break;

	case kSemi1:	this->_Synth->SetParameter(pOSemi1, val + 64); CC(14, val + 64); break;
	case kSemi2:	this->_Synth->SetParameter(pOSemi2, val + 64); CC(15, val + 64); break;
	case kSemi3:	this->_Synth->SetParameter(pOSemi3, val + 64); CC(16, val + 64); break;

	case kFine1:	this->_Synth->SetParameter(pOFine1, val + 64); CC(17, val + 64); break;
	case kFine2:	this->_Synth->SetParameter(pOFine2, val + 64); CC(18, val + 64); break;
	case kFine3:	this->_Synth->SetParameter(pOFine3, val + 64); CC(19, val + 64); break;

	case kMorph1:	this->_Synth->SetParameter(pOMorph1, val); CC(20, val); break;
	case kMorph2:	this->_Synth->SetParameter(pOMorph2, val); CC(21, val); break;
	case kMorph3:	this->_Synth->SetParameter(pOMorph3, val); CC(22, val); break;

	case kOSpread1:	this->_Synth->SetParameter(pOSpread1, val); CC(23, val << 2); break;
	case kOSpread2:	this->_Synth->SetParameter(pOSpread2, val); CC(24, val << 2); break;
	case kOSpread3:	this->_Synth->SetParameter(pOSpread3, val); CC(25, val << 2); break;

	case kOAdd1:		case kOMul1:
	case kONormal1:		case kOSuper1:
	case kOOff1:		case kOSaw1:		case kOPulse1:		case kOTri1:
	case kONoise1:		case kOAM1:			case kOSync1:		case kOKTrk1:
		this->CheckOSC1(id, val);
		break;
	case kOAdd2:		case kOMul2:
	case kONormal2:		case kOSuper2:
	case kOOff2:		case kOSaw2:		case kOPulse2:		case kOTri2:
	case kONoise2:		case kOAM2:			case kOSync2:		case kOKTrk2:
		this->CheckOSC2(id, val);
		break;
	case kOAdd3:		case kOMul3:
	case kONormal3:		case kOSuper3:
	case kOOff3:		case kOSaw3:		case kOPulse3:		case kOTri3:
	case kONoise3:		case kOAM3:			case kOSync3:		case kOKTrk3:
		this->CheckOSC3(id, val);
		break;
	case kArpSync:	
		this->_Synth->SetParameter(pArpSync, val); 
		this->objects[kArpSpeed]->Redraw(); 
		break;
	case kArpLoop:	this->_Synth->SetParameter(pArpLoop, val); break;

	case kL1Sync:	this->_Synth->SetParameter(pL1Sync, val); this->objects[kL1Speed]->Redraw(); break;
	case kL1SNH:	this->_Synth->SetParameter(pL1SNH, val); break;
	case kL1Retrig:	this->_Synth->SetParameter(pL1Retrig, val); break;
	case kL1Speed:	this->_Synth->SetParameter(pL1Speed, val); CC(80, val); break;
	case kL1Delay:	this->_Synth->SetParameter(pL1Delay, val); CC(81, val); break;
	case kL1PW:	this->_Synth->SetParameter(pL1PW, val); CC(82, val); break;
	case kL1KTrack: this->_Synth->SetParameter(pL1Keytrack, val + 64); break;

	case kL2Sync:	this->_Synth->SetParameter(pL2Sync, val); this->objects[kL2Speed]->Redraw(); break;
	case kL2SNH:	this->_Synth->SetParameter(pL2SNH, val); break;
	case kL2Retrig:	this->_Synth->SetParameter(pL2Retrig, val); break;
	case kL2Speed:	this->_Synth->SetParameter(pL2Speed, val); CC(83, val); break;
	case kL2Delay:	this->_Synth->SetParameter(pL2Delay, val); CC(84, val); break;
	case kL2PW:	this->_Synth->SetParameter(pL2PW, val); CC(85, val); break;
	case kL2KTrack: this->_Synth->SetParameter(pL2Keytrack, val + 64); break;

	case kM1Source:	this->_Synth->SetParameter(pM1Source, val); break;
	case kM1Amount:	this->_Synth->SetParameter(pM1Amount, val + 64); CC(44, val + 64); break;
	case kM1Dest:	this->_Synth->SetParameter(pM1Dest, val); break;

	case kM2Source:	this->_Synth->SetParameter(pM2Source, val); break;
	case kM2Amount:	this->_Synth->SetParameter(pM2Amount, val + 64); CC(45, val + 64); break;
	case kM2Dest:	this->_Synth->SetParameter(pM2Dest, val); break;

	case kM3Source:	this->_Synth->SetParameter(pM3Source, val); break;
	case kM3Amount:	this->_Synth->SetParameter(pM3Amount, val + 64); CC(46, val + 64); break;
	case kM3Dest:	this->_Synth->SetParameter(pM3Dest, val); break;

	case kM4Source:	this->_Synth->SetParameter(pM4Source, val); break;
	case kM4Amount:	this->_Synth->SetParameter(pM4Amount, val + 64); CC(47, val + 64); break;
	case kM4Dest:	this->_Synth->SetParameter(pM4Dest, val); break;

	case kM5Source:	this->_Synth->SetParameter(pM5Source, val); break;
	case kM5Amount:	this->_Synth->SetParameter(pM5Amount, val + 64); CC(48, val + 64); break;
	case kM5Dest:	this->_Synth->SetParameter(pM5Dest, val); break;

	case kM6Source:	this->_Synth->SetParameter(pM6Source, val); break;
	case kM6Amount:	this->_Synth->SetParameter(pM6Amount, val + 64); CC(49, val + 64); break;
	case kM6Dest:	this->_Synth->SetParameter(pM6Dest, val); break;

	case kPatchStore:	this->_Synth->Store();	break;
	case kPatchRecall:	this->_Synth->Recall(); this->ReadProgram(); break;
	case kPatchChaos:	this->Chaosize(); break;
	case kPanic:		this->_Synth->Reset(); break;
	case kAudioVol:		this->_Synth->SetParameter(pAudioVol, val); CC(29, val); break;
	case kAudioPan:		this->_Synth->SetParameter(pAudioPan, val + 64); CC(30, val + 64); break;
	}

	if(this->_Synth->HasPrgChanged())
	{
		this->objects[kPatchStore]->SetValue(0);
		this->objects[kPatchStore]->Redraw();
	}
	else
	{
		this->objects[kPatchStore]->SetValue(1);
		this->objects[kPatchStore]->Redraw();
	}
}
#undef CC

void Gui::Value2String(int id, int val, char* text)
{
	int i;
	*((unsigned __int32*)text) = 0x20202020;

	switch(id)
	{
	default:
		this->Int2String(val, text);
		break;
	case kVoices:	case kArpLength:
		this->Int2String(val + 1, text);
		break;
	case kGlide:	case kL1Delay:	case kL2Delay: case kAudioVol:
		this->Int2String(val, text, " off");
		break;
	case kArpSpeed:
		if(this->_Synth->sPrg.arp.sync)
		{
			for(i = 0; i < 4; i++)
				text[i] = g_syncNames[val][i];
		}
		else
			this->Int2String(val, text, " off");
		break;
	case kL1Speed:
		if(this->_Synth->sPrg.lfo[0].sync)
		{
			for(i = 0; i < 4; i++)
				text[i] = g_syncNames[val + 1][i];
		}
		else
			this->Int2String(val, text);
		break;
	case kL2Speed:
		if(this->_Synth->sPrg.lfo[1].sync)
		{
			for(i = 0; i < 4; i++)
				text[i] = g_syncNames[val + 1][i];
		}
		else
			this->Int2String(val, text);
		break;
	case kMixVel:
	case kSemi1:	case kFine1:	
	case kSemi2:	case kFine2:	
	case kSemi3:	case kFine3:
	case kFVel:		case kFEnv:		case kFGain:
	case kL1KTrack:	case kL2KTrack:
	case kM1Amount:	case kM2Amount:	case kM3Amount:
	case kM4Amount:	case kM5Amount:	case kM6Amount:
	case kArpNote1:	case kArpNote2:	case kArpNote3:	case kArpNote4:
	case kArpNote5:	case kArpNote6:	case kArpNote7:	case kArpNote8:
		this->Int2String(val, text, '-', '+', "   0");
		break;
	case kM1Source:	case kM2Source:	case kM3Source:
	case kM4Source:	case kM5Source:	case kM6Source:
		for(i = 0; i < 4; i++)
			text[i] = g_modSrc[val][i];
		break;
	case kM1Dest:	case kM2Dest:	case kM3Dest:
	case kM4Dest:	case kM5Dest:	case kM6Dest:
		for(i = 0; i < 4; i++)
			text[i] = g_modDst[val][i];
		break;
	case kMixPan:	case kAudioPan:
		this->Int2String(val, text, 'L', 'R', "Cntr");
		break;
	}
}

void _GetPos(int *x, int *y, HWND hwnd)
{
	RECT r;
	GetWindowRect(hwnd, &r);
	*x -= r.left;
	*y -= r.top;
}

void Gui::ReadProgram()
{
	sPRG* p = &(this->_Synth->sPrg);
	int i;
	this->prgRead = true;

	for(i = 0; i < OBJ_COUNT; i++)
	{
		if(this->objects[i])
			this->objects[i]->Reset();
	}

	//********************************************************************************
	//***** OSC                                                                  *****
	//********************************************************************************
	this->objects[kFine1]->_SetValue(p->osc[0].fine - 64);
	this->objects[kFine2]->_SetValue(p->osc[1].fine - 64);
	this->objects[kFine3]->_SetValue(p->osc[2].fine - 64);

	this->objects[kSemi1]->_SetValue(p->osc[0].semi - 64);
	this->objects[kSemi2]->_SetValue(p->osc[1].semi - 64);
	this->objects[kSemi3]->_SetValue(p->osc[2].semi - 64);

	this->objects[kMorph1]->_SetValue(p->osc[0].morph);
	this->objects[kMorph2]->_SetValue(p->osc[1].morph);
	this->objects[kMorph3]->_SetValue(p->osc[2].morph);

	this->objects[kOSpread1]->_SetValue(p->osc[0].spread);
	this->objects[kOSpread2]->_SetValue(p->osc[1].spread);
	this->objects[kOSpread3]->_SetValue(p->osc[2].spread);

	if(p->osc[0].flags & 0x80) this->objects[kOKTrk1]->_SetValue(1);
	if(p->osc[1].flags & 0x80) this->objects[kOKTrk2]->_SetValue(1);
	if(p->osc[2].flags & 0x80) this->objects[kOKTrk3]->_SetValue(1);

	if(p->osc[0].flags & 0x01) this->objects[kOSync1]->_SetValue(1);
	if(p->osc[1].flags & 0x01) this->objects[kOSync2]->_SetValue(1);
	if(p->osc[2].flags & 0x01) this->objects[kOSync3]->_SetValue(1);

	if(p->osc[0].flags & 0x02) this->objects[kOAM1]->_SetValue(1);
	if(p->osc[1].flags & 0x02) this->objects[kOAM2]->_SetValue(1);
	if(p->osc[2].flags & 0x02) this->objects[kOAM3]->_SetValue(1);

	if(!p->osc[0].wave)
		this->objects[kOOff1]->_SetValue(1);
	else
	{
		if(p->osc[0].wave & OSCW_SAW) this->objects[kOSaw1]->_SetValue(1);
		if(p->osc[0].wave & OSCW_TRI) this->objects[kOTri1]->_SetValue(1);
		if(p->osc[0].wave & OSCW_PULSE) this->objects[kOPulse1]->_SetValue(1);
		if(p->osc[0].wave & OSCW_NOISE) this->objects[kONoise1]->_SetValue(1);
	}

	if(!p->osc[1].wave)
		this->objects[kOOff2]->_SetValue(1);
	else
	{
		if(p->osc[1].wave & OSCW_SAW) this->objects[kOSaw2]->_SetValue(1);
		if(p->osc[1].wave & OSCW_TRI) this->objects[kOTri2]->_SetValue(1);
		if(p->osc[1].wave & OSCW_PULSE) this->objects[kOPulse2]->_SetValue(1);
		if(p->osc[1].wave & OSCW_NOISE) this->objects[kONoise2]->_SetValue(1);
	}

	if(!p->osc[2].wave)
		this->objects[kOOff3]->_SetValue(1);
	else
	{
		if(p->osc[2].wave & OSCW_SAW) this->objects[kOSaw3]->_SetValue(1);
		if(p->osc[2].wave & OSCW_TRI) this->objects[kOTri3]->_SetValue(1);
		if(p->osc[2].wave & OSCW_PULSE) this->objects[kOPulse3]->_SetValue(1);
		if(p->osc[2].wave & OSCW_NOISE)	this->objects[kONoise3]->_SetValue(1);
	}

	switch(p->osc[0].mode)
	{
	case OSCM_ADD:	case OSCM_MUL:
		if(p->osc[0].mode == OSCM_ADD)
			this->objects[kOAdd1]->_SetValue(1);
		else
			this->objects[kOMul1]->_SetValue(1);
		if(this->objects[kOSaw1]->isRadio)
		{
			this->GroupRadio(2, 0);
			this->objects[kOOff1]->isRadio = -1;
		}
		break;
	case OSCM_NORMAL:	case OSCM_SUPER:
		if(p->osc[0].mode == OSCM_NORMAL)
			this->objects[kONormal1]->_SetValue(1);
		else
			this->objects[kOSuper1]->_SetValue(1);
		if(!this->objects[kOSaw1]->isRadio)
			this->GroupRadio(2, -1);
		break;
	}

	switch(p->osc[1].mode)
	{
	case OSCM_ADD:	case OSCM_MUL:
		if(p->osc[1].mode == OSCM_ADD)
			this->objects[kOAdd2]->_SetValue(1);
		else
			this->objects[kOMul2]->_SetValue(1);
		if(this->objects[kOSaw2]->isRadio)
		{
			this->GroupRadio(4, 0);
			this->objects[kOOff2]->isRadio = -1;
		}
		break;
	case OSCM_NORMAL:	case OSCM_SUPER:
		if(p->osc[1].mode == OSCM_NORMAL)
			this->objects[kONormal2]->_SetValue(1);
		else
			this->objects[kOSuper2]->_SetValue(1);
		if(!this->objects[kOSaw2]->isRadio)
			this->GroupRadio(4, -1);
		break;
	}

	switch(p->osc[2].mode)
	{
	case OSCM_ADD:	case OSCM_MUL:
		if(p->osc[2].mode == OSCM_ADD)
			this->objects[kOAdd3]->_SetValue(1);
		else
			this->objects[kOMul3]->_SetValue(1);
		if(this->objects[kOSaw3]->isRadio)
		{
			this->GroupRadio(6, 0);
			this->objects[kOOff3]->isRadio = -1;
		}
		break;
	case OSCM_NORMAL:	case OSCM_SUPER:
		if(p->osc[2].mode == OSCM_NORMAL)
			this->objects[kONormal3]->_SetValue(1);
		else
			this->objects[kOSuper3]->_SetValue(1);
		if(!this->objects[kOSaw3]->isRadio)
			this->GroupRadio(6, -1);
		break;
	}

	//********************************************************************************
	//***** FILTER                                                               *****
	//********************************************************************************
	this->objects[kCutoff]->_SetValue(p->filter.cutoff);
	this->objects[kReso]->_SetValue(p->filter.q);
	this->objects[kFGain]->_SetValue(p->filter.gain - 64);
	this->objects[kFVel]->_SetValue(p->filter.velsens - 64);
	this->objects[kFEnv]->_SetValue(p->filter.env2 - 64);
	if(p->filter.exp) this->objects[kFExp]->_SetValue(1);
	if(p->filter.db == FILTM_12DB)
		this->objects[kF12dB]->_SetValue(1);
	else
		this->objects[kF24dB]->_SetValue(1);
	if(p->filter.mode == FILTT_LP)
		this->objects[kFLP]->_SetValue(1);
	else if(p->filter.mode == FILTT_BP)
		this->objects[kFBP]->_SetValue(1);
	else if(p->filter.mode == FILTT_HP)
		this->objects[kFHP]->_SetValue(1);
	else if(p->filter.mode == FILTT_BR)
		this->objects[kFBR]->_SetValue(1);

	//********************************************************************************
	//***** ENVELOPES                                                            *****
	//********************************************************************************
	this->objects[kAttack1]->_SetValue(p->env[0].a);
	this->objects[kHold1]->_SetValue(p->env[0].h);
	this->objects[kDecay1]->_SetValue(p->env[0].d);
	this->objects[kSustain1]->_SetValue(p->env[0].s);
	this->objects[kRelease1]->_SetValue(p->env[0].r);
	this->objects[kAttack2]->_SetValue(p->env[1].a);
	this->objects[kHold2]->_SetValue(p->env[1].h);
	this->objects[kDecay2]->_SetValue(p->env[1].d);
	this->objects[kSustain2]->_SetValue(p->env[1].s);
	this->objects[kRelease2]->_SetValue(p->env[1].r);
	this->objects[kAttack3]->_SetValue(p->env[2].a);
	this->objects[kHold3]->_SetValue(p->env[2].h);
	this->objects[kDecay3]->_SetValue(p->env[2].d);
	this->objects[kSustain3]->_SetValue(p->env[2].s);
	this->objects[kRelease3]->_SetValue(p->env[2].r);
	//********************************************************************************
	//***** MIXER                                                                *****
	//********************************************************************************
	this->objects[kMixVol]->_SetValue(p->mixer.volume);
	this->objects[kMixVel]->_SetValue(p->mixer.velsens - 64);
	this->objects[kMixPan]->_SetValue(p->mixer.panning - 64);
	this->objects[kMixVol1]->_SetValue(p->mixer.osc1);
	this->objects[kMixVol2]->_SetValue(p->mixer.osc2);
	this->objects[kMixVol3]->_SetValue(p->mixer.osc3);

	//********************************************************************************
	//***** GLOBAL                                                               *****
	//********************************************************************************
	this->objects[kVoices]->_SetValue(p->voices);
	this->objects[kGlide]->_SetValue(p->glide);

	//********************************************************************************
	//***** ARP                                                                  *****
	//********************************************************************************
	this->objects[kArpSpeed]->_SetValue(p->arp.speed);
	this->objects[kArpLength]->_SetValue(p->arp.length);
	this->objects[kArpNote1]->_SetValue(p->arp.note[0] - 64);
	this->objects[kArpNote2]->_SetValue(p->arp.note[1] - 64);
	this->objects[kArpNote3]->_SetValue(p->arp.note[2] - 64);
	this->objects[kArpNote4]->_SetValue(p->arp.note[3] - 64);
	this->objects[kArpNote5]->_SetValue(p->arp.note[4] - 64);
	this->objects[kArpNote6]->_SetValue(p->arp.note[5] - 64);
	this->objects[kArpNote7]->_SetValue(p->arp.note[6] - 64);
	this->objects[kArpNote8]->_SetValue(p->arp.note[7] - 64);
	this->objects[kArpSync]->_SetValue((p->arp.sync) ? 1 : 0);
	this->objects[kArpLoop]->_SetValue((p->arp.loop) ? 1 : 0);

	//********************************************************************************
	//***** LFOs                                                                 *****
	//********************************************************************************
	this->objects[kL1Speed]->_SetValue(p->lfo[0].speed);
	this->objects[kL1Delay]->_SetValue(p->lfo[0].delay);
	this->objects[kL1PW]->_SetValue(p->lfo[0].pw);
	this->objects[kL1KTrack]->_SetValue(p->lfo[0].keytrack - 64);
	this->objects[kL1Sync]->_SetValue((p->lfo[0].sync) ? 1 : 0);
	this->objects[kL1Retrig]->_SetValue((p->lfo[0].retrig) ? 1 : 0);
	this->objects[kL1SNH]->_SetValue((p->lfo[0].snh) ? 1 : 0);
	if(p->lfo[0].wave == OSCW_SAW) this->objects[kL1Saw]->_SetValue(1);
	else if(p->lfo[0].wave == OSCW_TRI) this->objects[kL1Tri]->_SetValue(1);
	else if(p->lfo[0].wave == OSCW_PULSE) this->objects[kL1Pulse]->_SetValue(1);
	else if(p->lfo[0].wave == OSCW_NOISE) this->objects[kL1Noise]->_SetValue(1);
	else this->objects[kL1Off]->_SetValue(1);

	this->objects[kL2Speed]->_SetValue(p->lfo[1].speed);
	this->objects[kL2Delay]->_SetValue(p->lfo[1].delay);
	this->objects[kL2PW]->_SetValue(p->lfo[1].pw);
	this->objects[kL2KTrack]->_SetValue(p->lfo[1].keytrack - 64);
	this->objects[kL2Sync]->_SetValue((p->lfo[1].sync) ? 1 : 0);
	this->objects[kL2Retrig]->_SetValue((p->lfo[1].retrig) ? 1 : 0);
	this->objects[kL2SNH]->_SetValue((p->lfo[1].snh) ? 1 : 0);
	if(p->lfo[1].wave == OSCW_SAW) this->objects[kL2Saw]->_SetValue(1);
	else if(p->lfo[1].wave == OSCW_TRI) this->objects[kL2Tri]->_SetValue(1);
	else if(p->lfo[1].wave == OSCW_PULSE) this->objects[kL2Pulse]->_SetValue(1);
	else if(p->lfo[1].wave == OSCW_NOISE) this->objects[kL2Noise]->_SetValue(1);
	else this->objects[kL2Off]->_SetValue(1);

	//********************************************************************************
	//***** MODs                                                                 *****
	//********************************************************************************
	this->objects[kM1Source]->_SetValue(p->mod[0].source);
	this->objects[kM1Amount]->_SetValue(p->mod[0].amount - 64);
	this->objects[kM1Dest]->_SetValue(p->mod[0].dest);
	this->objects[kM2Source]->_SetValue(p->mod[1].source);
	this->objects[kM2Amount]->_SetValue(p->mod[1].amount - 64);
	this->objects[kM2Dest]->_SetValue(p->mod[1].dest);
	this->objects[kM3Source]->_SetValue(p->mod[2].source);
	this->objects[kM3Amount]->_SetValue(p->mod[2].amount - 64);
	this->objects[kM3Dest]->_SetValue(p->mod[2].dest);
	this->objects[kM4Source]->_SetValue(p->mod[3].source);
	this->objects[kM4Amount]->_SetValue(p->mod[3].amount - 64);
	this->objects[kM4Dest]->_SetValue(p->mod[3].dest);
	this->objects[kM5Source]->_SetValue(p->mod[4].source);
	this->objects[kM5Amount]->_SetValue(p->mod[4].amount - 64);
	this->objects[kM5Dest]->_SetValue(p->mod[4].dest);
	this->objects[kM6Source]->_SetValue(p->mod[5].source);
	this->objects[kM6Amount]->_SetValue(p->mod[5].amount - 64);
	this->objects[kM6Dest]->_SetValue(p->mod[5].dest);

	this->objects[kAudioVol]->_SetValue(p->audioVol);
	this->objects[kAudioPan]->_SetValue(p->audioPan - 64);

	this->objects[kPatchStore]->SetValue(1);

	for(i = 0; i < OBJ_COUNT; i++)
	{
		if(this->objects[i])
			this->objects[i]->Redraw();
	}

	this->prgRead = false;

	RECT r;
	r.top = 0;
	r.bottom = this->Height();
	r.left = 0;
	r.right = this->Width();
	InvalidateRect(this->hWnd, &r, false);
}

#define OSU(x,y) this->objects[x]->_SetValue(y); this->objects[x]->Redraw()

void Gui::UpdateObject(int id)
{
	sPRG* p = &(this->_Synth->sPrg);

	switch(id)
	{
	case pGlide:	OSU(kGlide, p->glide);					break;
	case pMixVol:	OSU(kMixVol, p->mixer.volume);			break;
	case pMixPan:	OSU(kMixPan, p->mixer.panning - 64);	break;
	case pFCutoff:	OSU(kCutoff, p->filter.cutoff);			break;
	case pFQ:		OSU(kReso, p->filter.q);				break;
	case pFGain:	OSU(kFGain, p->filter.gain - 64);		break;
	case pFEnv2:	OSU(kFEnv, p->filter.env2 - 64);		break;
	case pOSemi1:	OSU(kSemi1, p->osc[0].semi - 64);		break;
	case pOSemi2:	OSU(kSemi2, p->osc[1].semi - 64);		break;
	case pOSemi3:	OSU(kSemi3, p->osc[2].semi - 64);		break;
	case pOFine1:	OSU(kFine1, p->osc[0].fine - 64);		break;
	case pOFine2:	OSU(kFine2, p->osc[1].fine - 64);		break;
	case pOFine3:	OSU(kFine3, p->osc[2].fine - 64);		break;
	case pOMorph1:	OSU(kMorph1, p->osc[0].morph);			break;
	case pOMorph2:	OSU(kMorph2, p->osc[1].morph);			break;
	case pOMorph3:	OSU(kMorph3, p->osc[2].morph);			break;
	case pOSpread1:	OSU(kOSpread1, p->osc[0].spread);		break;
	case pOSpread2:	OSU(kOSpread2, p->osc[1].spread);		break;
	case pOSpread3:	OSU(kOSpread3, p->osc[2].spread);		break;
	case pMixVol1:	OSU(kMixVol1, p->mixer.osc1);			break;
	case pMixVol2:	OSU(kMixVol2, p->mixer.osc2);			break;
	case pMixVol3:	OSU(kMixVol3, p->mixer.osc3);			break;
	case pAudioVol:	OSU(kAudioVol, p->audioVol);			break;
	case pAudioPan:	OSU(kAudioPan, p->audioPan - 64);		break;
	case pM1Amount:	OSU(kM1Amount, p->mod[0].amount - 64);	break;
	case pM2Amount:	OSU(kM1Amount, p->mod[1].amount - 64);	break;
	case pM3Amount:	OSU(kM1Amount, p->mod[2].amount - 64);	break;
	case pM4Amount:	OSU(kM1Amount, p->mod[3].amount - 64);	break;
	case pM5Amount:	OSU(kM1Amount, p->mod[4].amount - 64);	break;
	case pM6Amount:	OSU(kM1Amount, p->mod[5].amount - 64);	break;
	case pArpSpeed: OSU(kArpSpeed, p->arp.speed);			break;
	case pArpLength: OSU(kArpLength, p->arp.length);		break;
	case pArpNote1: OSU(kArpSpeed, p->arp.note[0] - 64);	break;
	case pArpNote2: OSU(kArpSpeed, p->arp.note[1] - 64);	break;
	case pArpNote3: OSU(kArpSpeed, p->arp.note[2] - 64);	break;
	case pArpNote4: OSU(kArpSpeed, p->arp.note[3] - 64);	break;
	case pArpNote5: OSU(kArpSpeed, p->arp.note[4] - 64);	break;
	case pArpNote6: OSU(kArpSpeed, p->arp.note[5] - 64);	break;
	case pArpNote7: OSU(kArpSpeed, p->arp.note[6] - 64);	break;
	case pArpNote8: OSU(kArpSpeed, p->arp.note[7] - 64);	break;
	case pL1Speed:	OSU(kL1Speed, p->lfo[0].speed);			break;
	case pL1Delay:	OSU(kL1Delay, p->lfo[0].delay);			break;
	case pL1PW:		OSU(kL1PW, p->lfo[0].pw);				break;
	case pL2Speed:	OSU(kL1Speed, p->lfo[1].speed);			break;
	case pL2Delay:	OSU(kL1Delay, p->lfo[1].delay);			break;
	case pL2PW:		OSU(kL1PW, p->lfo[1].pw);				break;
	}

	if(this->_Synth->HasPrgChanged())
	{
		this->objects[kPatchStore]->SetValue(0);
		this->objects[kPatchStore]->Redraw();
	}
	else
	{
		this->objects[kPatchStore]->SetValue(1);
		this->objects[kPatchStore]->Redraw();
	}
}

void Gui::Chaosize()
{
	int i, m, nc = 0;
	sPRG* p = &(this->_Synth->sPrg);
	
	this->_Synth->Reset();
	for(i = 0; i < 3; i++)
	{
		p->osc[i].semi = (irand() & 63) + 32;
		p->osc[i].fine = (irand() & 63) + 32;
		p->osc[i].morph = irand() & 127;
		p->osc[i].flags = irand() & 0x83;
		p->osc[i].spread = irand() & 31;
		m = p->osc[i].mode = irand() & 3;
		if(m == 0 || m == 3)
		{
			if(nc)
			{
				m = irand() % 3;
				if(!m)
					p->osc[i].wave = OSCW_SAW;
				else if(m == 1)
					p->osc[i].wave = OSCW_PULSE;
				else if(m == 2)
					p->osc[i].wave = OSCW_TRI;
			}
			else
			{
				m = irand() & 3;
				if(!m)
					p->osc[i].wave = OSCW_SAW;
				else if(m == 1)
					p->osc[i].wave = OSCW_PULSE;
				else if(m == 2)
					p->osc[i].wave = OSCW_TRI;
				else
				{
					p->osc[i].wave = OSCW_NOISE;
					nc++;
				}
			}
		}
		else
		{
			if(nc)
				p->osc[i].wave = irand() & 7;
			else
			{
				p->osc[i].wave = irand() & 15;
				if(p->osc[i].wave & OSCW_NOISE)
					nc++;
			}

		}
	}

	for(i = 0; i < 3; i++)
	{
		p->env[i].a = irand() & 63;
		p->env[i].h = irand() & 63;
		p->env[i].d = irand() & 63;
		p->env[i].s = irand() & 127;
		p->env[i].r = irand() & 63;
	}

	p->arp.loop = irand() & 1;
	p->arp.sync = irand() & 1;
	p->arp.speed = (irand() & 63) + 64;
	p->arp.length = irand() & 7;
	for(i = 0; i < 8; i++)
		p->arp.note[i] = irand() & 127;

	for(i = 0; i < 2; i++)
	{
		p->lfo[i].speed = (irand() & 63) + 64;
		p->lfo[i].delay = (irand() & 63);
		p->lfo[i].keytrack = (irand() & 63) + 32;
		p->lfo[i].pw = irand() & 127;
		p->lfo[i].retrig = irand() & 1;
		p->lfo[i].snh = irand() & 1;
		p->lfo[i].sync = irand() & 1;
		m = irand() & 3;
		if(!m)
			p->lfo[i].wave = OSCW_SAW;
		else if(m == 1)
			p->lfo[i].wave = OSCW_PULSE;
		else if(m == 2)
			p->lfo[i].wave = OSCW_TRI;
		else
			p->lfo[i].wave = OSCW_NOISE;
	}

	for(i = 0; i < 6; i++)
	{
		p->mod[i].source = irand() % (MAX_SRC + 1);
		p->mod[i].amount = irand() & 127;
		p->mod[i].dest = irand() % (MAX_DEST + 1);
	}

	p->mixer.osc1 = irand() & 127;
	p->mixer.osc2 = irand() & 127;
	p->mixer.osc3 = irand() & 127;
	p->mixer.panning = 64;
	p->mixer.velsens = 64;
	p->mixer.volume = 40;

	p->filter.cutoff = irand() & 127;
	p->filter.q = irand() & 127;
	p->filter.velsens = (irand() & 63) + 32;
	p->filter.env2 = irand() & 127;
	p->filter.db = irand() & 1;
	p->filter.exp = irand() & 1;

	m = irand() & 3;
	if(!m)
		p->filter.mode = FILTT_LP;
	else if(m == 1)
		p->filter.mode = FILTT_BP;
	else if(m == 2)
		p->filter.mode = FILTT_HP;
	else
		p->filter.mode = FILTT_BR;

	p->voices = irand() & 7;
	p->glide = irand() & 127;
	p->audioPan = 64;
	p->audioVol = 0;

	this->_Synth->ReadProgram();
	this->ReadProgram();
}

void Gui::SendCC(int num, int val)
{
	VstEvents e;
	VstMidiEvent m;

	memset(&e, 0, sizeof(VstEvents));
	memset(&m, 0, sizeof(VstMidiEvent));

	m.byteSize = sizeof(VstMidiEvent);
	m.type = kVstMidiType;
	m.midiData[0] = 0xb0;
	m.midiData[1] = (char)num;
	m.midiData[2] = (char)val;
	m.flags = kVstMidiEventIsRealtime; 

	e.numEvents = 1;
	e.events[0] = (VstEvent*) &m;

	((AudioEffectX*)this->effect)->sendVstEventsToHost(&e);
}

LONG_PTR WINAPI WindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Gui* gui = (Gui*)GetWindowLongPtr (hwnd, GWLP_USERDATA);

	switch (message)
	{
	case WM_TIMER:
		{
			if(gui)
			{
				if(gui->copyLeft)
				{
					gui->copyLeft--;
					if(!gui->copyLeft)
					{
						RECT r;
						r.top = 0;
						r.left = 0;
						r.right = gui->Width();
						r.bottom = gui->Height();
						InvalidateRect(hwnd, &r, FALSE);
						g_copyShown = -1;
					}
				}

				if(--gui->dCount <= 0)
				{
					char text[4];
					*((unsigned __int32*)text) = 0x20202020;
					gui->Int2String(gui->_Synth->_CPU, text);	
					gui->DrawString(gui->StatusPos[0], gui->StatusPos[1], text);
					
					*((unsigned __int32*)text) = 0x20202020;	
					gui->Int2String(gui->_Synth->GetVoices(), text);
					gui->DrawString(gui->StatusPos[2], gui->StatusPos[3], text);
					gui->dCount = 20;
				}
				gui->Blit(0, (gui->_Synth->GetLFOPos(0) >> 2) & 0xfff8, gui->LedPos[0], gui->LedPos[1], gui->LedPos[2], gui->LedPos[3], gui->bLed); 
				gui->Blit(0, (gui->_Synth->GetLFOPos(1) >> 2) & 0xfff8, gui->LedPos[0], gui->LedPos[1], gui->LedPos[4], gui->LedPos[5], gui->bLed); 
			}
			return 0;
			break;
		}
	case WM_PAINT:
		{
			// BitBlt offscreen portion to window
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HDC chdc = CreateCompatibleDC(hdc);
			SelectObject(chdc, gui->bOffScreen);
			
			BitBlt(hdc, 
				ps.rcPaint.left, 
				ps.rcPaint.top, 
				ps.rcPaint.right - ps.rcPaint.left, 
				ps.rcPaint.bottom - ps.rcPaint.top,
				chdc, 
				ps.rcPaint.left,
				ps.rcPaint.top, 
				SRCCOPY);

			if(gui->copyLeft)
			{
				SelectObject(chdc, gui->bCopy);
				
				BitBlt(hdc, 
					(gui->Width() / 2) - 128, 
					(gui->Height() / 2) - 48, 
					256, 
					96,
					chdc, 
					0,
					0, 
					SRCCOPY);
			}
			DeleteDC(chdc);
			EndPaint(hwnd, &ps);
			return 0;
			break;
		}
	case WM_MOUSEWHEEL:
		{
			if(!gui->bDragging)
			{
				int delta = (wParam & 0x80000000) ? -1 : 1;
				int i;
				int x = (int)(lParam & 0xffff); 
				int y = (int)((lParam >> 16) & 0xffff); 
				_GetPos(&x, &y, hwnd);
				
				for(i = 0; i < OBJ_COUNT; i++)
				{
					if(gui->objects[i] != 0)
					{
						if(gui->objects[i]->InBounds(x, y))
						{
							gui->objects[i]->Wheel(delta);
							break;
						}
					}
				}
			}
			return 0;
			break;
		}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
			if(!gui->bDragging)
			{
				if(GetFocus() != hwnd)
					SetFocus(hwnd);

				int i;
				int x = (int)(lParam & 0xffff); 
				int y = (int)((lParam >> 16) & 0xffff); 
				int b = 0;
				if(wParam & MK_LBUTTON)
					b |= (gui->bSwapButtons) ? GMB_RIGHT : GMB_LEFT;
				if(wParam & MK_MBUTTON)
					b |= GMB_MIDDLE;
				if(wParam & MK_RBUTTON)
					b |= (gui->bSwapButtons) ? GMB_LEFT : GMB_RIGHT;

				for(i = 0; i < OBJ_COUNT; i++)
				{
					if(gui->objects[i] != 0)
					{
						if(gui->objects[i]->InBounds(x, y))
						{
							gui->objects[i]->Mouse(x, y, b);
							break;
						}
					}
				}
			}
			return 0;
			break;
		}
	case WM_DESTROY:
		if (gui)
			gui->close();
		break;
	}
	return DefWindowProc (hwnd, message, wParam, lParam);
}

#pragma warning(default: 4244 4312) 
