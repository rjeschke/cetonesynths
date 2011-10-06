#include "CetoneEditor.h"

CLabel::CLabel (CRect &size, char *text): CParamDisplay (size)
{
	strcpy (label, "");
	setLabel (text);
}

void CLabel::setLabel (char *text)
{
	if (text)
	{
		if(text[0] == 0)
		{
			label[0] = '-';
			label[1] = '-';
			label[2] = '-';
			label[3] = 0;
		}
		else
		{
			int i = 0;

			while(i < 7)
			{
				label[i] = text[i];
				if(text[i] == 0)
					break;
				i++;
			}
			label[i] = 0;
		}
	}
	else
		label[0] = 0;

	setDirty ();
}

void CLabel::draw (CDrawContext *context)
{
	CColor black = {0, 0, 0, 255};
	CColor white = {255, 255, 255, 255};

	context->setFillColor(black);
	context->fillRect(this->size);
	context->setFont(kNormalFontSmaller);
	context->setFontColor(white);
	context->drawString(this->label, this->size, false, kCenterText);
	setDirty (false);
}

CCetoneEditor::CCetoneEditor(AudioEffect* effect)
: AEffGUIEditor(effect)
{
	this->BmpBackground = new CBitmap(rBackground);

	this->GuiSize.top = 0;
	this->GuiSize.left = 0;
	this->GuiSize.right = (VstInt16)this->BmpBackground->getWidth();
	this->GuiSize.bottom = (VstInt16)this->BmpBackground->getHeight();

	this->BmpKnob = 0;
	this->BmpButton = 0;
	frame = 0;
}

CCetoneEditor::~CCetoneEditor(void)
{
	if(this->BmpBackground)
		this->BmpBackground->forget();
	this->BmpBackground = 0;
}

bool CCetoneEditor::getRect(ERect **rect)
{
	*rect = &(this->GuiSize);

	return true;
}

bool CCetoneEditor::open(void *ptr)
{
	AEffGUIEditor::open(ptr);

	CRect r;
	r.left = 0;
	r.top = 0;
	r.right = (VstInt16)this->BmpBackground->getWidth();
	r.bottom = (VstInt16)this->BmpBackground->getHeight();

	frame = new CFrame(r, ptr, this);
	frame->setBackground(this->BmpBackground);

	this->setKnobMode(kLinearMode);

	if(!this->BmpKnob)
		this->BmpKnob = new CBitmap(rKnobs);

	if(!this->BmpButton)
		this->BmpButton = new CBitmap(rButtons);

	for(int i = 0; i < pParameters; i++)
	{
		this->Buttons[i] = 0;
		this->Knobs[i] = 0;
	}

	CRect size;
	CPoint point;
	// Create:
	point (0, 0);
	size (0, 0, 48, 48);
	size.offset (212, 40);
	this->Knobs[1] = new CAnimKnob(size, this, 1, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[1]);
	size (0, 0, 44, 14);
	size.offset (214, 90);
	this->Labels[1] = new CLabel(size, 0);
	frame->addView(this->Labels[1]);
	size (0, 0, 48, 48);
	size.offset (260, 40);
	this->Knobs[2] = new CAnimKnob(size, this, 2, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[2]);
	size (0, 0, 44, 14);
	size.offset (262, 90);
	this->Labels[2] = new CLabel(size, 0);
	frame->addView(this->Labels[2]);
	size (0, 0, 48, 48);
	size.offset (308, 40);
	this->Knobs[17] = new CAnimKnob(size, this, 17, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[17]);
	size (0, 0, 44, 14);
	size.offset (310, 90);
	this->Labels[17] = new CLabel(size, 0);
	frame->addView(this->Labels[17]);
	size (0, 0, 48, 48);
	size.offset (356, 40);
	this->Knobs[24] = new CAnimKnob(size, this, 24, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[24]);
	size (0, 0, 44, 14);
	size.offset (358, 90);
	this->Labels[24] = new CLabel(size, 0);
	frame->addView(this->Labels[24]);
	size (0, 0, 48, 48);
	size.offset (404, 40);
	this->Knobs[31] = new CAnimKnob(size, this, 31, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[31]);
	size (0, 0, 44, 14);
	size.offset (406, 90);
	this->Labels[31] = new CLabel(size, 0);
	frame->addView(this->Labels[31]);
	size (0, 0, 48, 48);
	size.offset (464, 40);
	this->Knobs[5] = new CAnimKnob(size, this, 5, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[5]);
	size (0, 0, 44, 14);
	size.offset (466, 90);
	this->Labels[5] = new CLabel(size, 0);
	frame->addView(this->Labels[5]);
	size (0, 0, 48, 48);
	size.offset (512, 40);
	this->Knobs[6] = new CAnimKnob(size, this, 6, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[6]);
	size (0, 0, 44, 14);
	size.offset (514, 90);
	this->Labels[6] = new CLabel(size, 0);
	frame->addView(this->Labels[6]);
	size (0, 0, 48, 48);
	size.offset (560, 40);
	this->Knobs[7] = new CAnimKnob(size, this, 7, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[7]);
	size (0, 0, 44, 14);
	size.offset (562, 90);
	this->Labels[7] = new CLabel(size, 0);
	frame->addView(this->Labels[7]);
	size (0, 0, 48, 48);
	size.offset (608, 40);
	this->Knobs[8] = new CAnimKnob(size, this, 8, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[8]);
	size (0, 0, 44, 14);
	size.offset (610, 90);
	this->Labels[8] = new CLabel(size, 0);
	frame->addView(this->Labels[8]);
	size (0, 0, 48, 48);
	size.offset (656, 40);
	this->Knobs[93] = new CAnimKnob(size, this, 93, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[93]);
	size (0, 0, 44, 14);
	size.offset (658, 90);
	this->Labels[93] = new CLabel(size, 0);
	frame->addView(this->Labels[93]);
	size (0, 0, 48, 48);
	size.offset (716, 40);
	this->Knobs[3] = new CAnimKnob(size, this, 3, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[3]);
	size (0, 0, 44, 14);
	size.offset (718, 90);
	this->Labels[3] = new CLabel(size, 0);
	frame->addView(this->Labels[3]);
	size (0, 0, 48, 48);
	size.offset (764, 40);
	this->Knobs[4] = new CAnimKnob(size, this, 4, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[4]);
	size (0, 0, 44, 14);
	size.offset (766, 90);
	this->Labels[4] = new CLabel(size, 0);
	frame->addView(this->Labels[4]);
	size (0, 0, 48, 48);
	size.offset (8, 150);
	this->Knobs[13] = new CAnimKnob(size, this, 13, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[13]);
	size (0, 0, 44, 14);
	size.offset (10, 200);
	this->Labels[13] = new CLabel(size, 0);
	frame->addView(this->Labels[13]);
	size (0, 0, 48, 48);
	size.offset (56, 150);
	this->Knobs[14] = new CAnimKnob(size, this, 14, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[14]);
	size (0, 0, 44, 14);
	size.offset (58, 200);
	this->Labels[14] = new CLabel(size, 0);
	frame->addView(this->Labels[14]);
	size (0, 0, 48, 48);
	size.offset (104, 150);
	this->Knobs[15] = new CAnimKnob(size, this, 15, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[15]);
	size (0, 0, 44, 14);
	size.offset (106, 200);
	this->Labels[15] = new CLabel(size, 0);
	frame->addView(this->Labels[15]);
	size (0, 0, 48, 48);
	size.offset (152, 150);
	this->Knobs[16] = new CAnimKnob(size, this, 16, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[16]);
	size (0, 0, 44, 14);
	size.offset (154, 200);
	this->Labels[16] = new CLabel(size, 0);
	frame->addView(this->Labels[16]);
	size (0, 0, 48, 18);
	size.offset (200, 150);
	this->Buttons[18] = new COnOffButton(size, this, 18, this->BmpButton);
	frame->addView(this->Buttons[18]);
	size (0, 0, 48, 18);
	size.offset (200, 198);
	this->Buttons[19] = new COnOffButton(size, this, 19, this->BmpButton);
	frame->addView(this->Buttons[19]);
	size (0, 0, 48, 48);
	size.offset (8, 260);
	this->Knobs[20] = new CAnimKnob(size, this, 20, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[20]);
	size (0, 0, 44, 14);
	size.offset (10, 310);
	this->Labels[20] = new CLabel(size, 0);
	frame->addView(this->Labels[20]);
	size (0, 0, 48, 48);
	size.offset (56, 260);
	this->Knobs[21] = new CAnimKnob(size, this, 21, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[21]);
	size (0, 0, 44, 14);
	size.offset (58, 310);
	this->Labels[21] = new CLabel(size, 0);
	frame->addView(this->Labels[21]);
	size (0, 0, 48, 48);
	size.offset (104, 260);
	this->Knobs[22] = new CAnimKnob(size, this, 22, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[22]);
	size (0, 0, 44, 14);
	size.offset (106, 310);
	this->Labels[22] = new CLabel(size, 0);
	frame->addView(this->Labels[22]);
	size (0, 0, 48, 48);
	size.offset (152, 260);
	this->Knobs[23] = new CAnimKnob(size, this, 23, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[23]);
	size (0, 0, 44, 14);
	size.offset (154, 310);
	this->Labels[23] = new CLabel(size, 0);
	frame->addView(this->Labels[23]);
	size (0, 0, 48, 18);
	size.offset (200, 260);
	this->Buttons[25] = new COnOffButton(size, this, 25, this->BmpButton);
	frame->addView(this->Buttons[25]);
	size (0, 0, 48, 18);
	size.offset (200, 308);
	this->Buttons[26] = new COnOffButton(size, this, 26, this->BmpButton);
	frame->addView(this->Buttons[26]);
	size (0, 0, 48, 48);
	size.offset (8, 370);
	this->Knobs[27] = new CAnimKnob(size, this, 27, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[27]);
	size (0, 0, 44, 14);
	size.offset (10, 420);
	this->Labels[27] = new CLabel(size, 0);
	frame->addView(this->Labels[27]);
	size (0, 0, 48, 48);
	size.offset (56, 370);
	this->Knobs[28] = new CAnimKnob(size, this, 28, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[28]);
	size (0, 0, 44, 14);
	size.offset (58, 420);
	this->Labels[28] = new CLabel(size, 0);
	frame->addView(this->Labels[28]);
	size (0, 0, 48, 48);
	size.offset (104, 370);
	this->Knobs[29] = new CAnimKnob(size, this, 29, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[29]);
	size (0, 0, 44, 14);
	size.offset (106, 420);
	this->Labels[29] = new CLabel(size, 0);
	frame->addView(this->Labels[29]);
	size (0, 0, 48, 48);
	size.offset (152, 370);
	this->Knobs[30] = new CAnimKnob(size, this, 30, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[30]);
	size (0, 0, 44, 14);
	size.offset (154, 420);
	this->Labels[30] = new CLabel(size, 0);
	frame->addView(this->Labels[30]);
	size (0, 0, 48, 18);
	size.offset (200, 370);
	this->Buttons[32] = new COnOffButton(size, this, 32, this->BmpButton);
	frame->addView(this->Buttons[32]);
	size (0, 0, 48, 18);
	size.offset (200, 418);
	this->Buttons[33] = new COnOffButton(size, this, 33, this->BmpButton);
	frame->addView(this->Buttons[33]);
	size (0, 0, 48, 48);
	size.offset (260, 150);
	this->Knobs[34] = new CAnimKnob(size, this, 34, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[34]);
	size (0, 0, 44, 14);
	size.offset (262, 200);
	this->Labels[34] = new CLabel(size, 0);
	frame->addView(this->Labels[34]);
	size (0, 0, 48, 48);
	size.offset (308, 150);
	this->Knobs[35] = new CAnimKnob(size, this, 35, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[35]);
	size (0, 0, 44, 14);
	size.offset (310, 200);
	this->Labels[35] = new CLabel(size, 0);
	frame->addView(this->Labels[35]);
	size (0, 0, 48, 48);
	size.offset (356, 150);
	this->Knobs[36] = new CAnimKnob(size, this, 36, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[36]);
	size (0, 0, 44, 14);
	size.offset (358, 200);
	this->Labels[36] = new CLabel(size, 0);
	frame->addView(this->Labels[36]);
	size (0, 0, 48, 48);
	size.offset (404, 150);
	this->Knobs[37] = new CAnimKnob(size, this, 37, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[37]);
	size (0, 0, 44, 14);
	size.offset (406, 200);
	this->Labels[37] = new CLabel(size, 0);
	frame->addView(this->Labels[37]);
	size (0, 0, 48, 48);
	size.offset (452, 150);
	this->Knobs[38] = new CAnimKnob(size, this, 38, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[38]);
	size (0, 0, 44, 14);
	size.offset (454, 200);
	this->Labels[38] = new CLabel(size, 0);
	frame->addView(this->Labels[38]);
	size (0, 0, 48, 48);
	size.offset (260, 260);
	this->Knobs[39] = new CAnimKnob(size, this, 39, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[39]);
	size (0, 0, 44, 14);
	size.offset (262, 310);
	this->Labels[39] = new CLabel(size, 0);
	frame->addView(this->Labels[39]);
	size (0, 0, 48, 48);
	size.offset (308, 260);
	this->Knobs[40] = new CAnimKnob(size, this, 40, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[40]);
	size (0, 0, 44, 14);
	size.offset (310, 310);
	this->Labels[40] = new CLabel(size, 0);
	frame->addView(this->Labels[40]);
	size (0, 0, 48, 48);
	size.offset (356, 260);
	this->Knobs[41] = new CAnimKnob(size, this, 41, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[41]);
	size (0, 0, 44, 14);
	size.offset (358, 310);
	this->Labels[41] = new CLabel(size, 0);
	frame->addView(this->Labels[41]);
	size (0, 0, 48, 48);
	size.offset (404, 260);
	this->Knobs[42] = new CAnimKnob(size, this, 42, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[42]);
	size (0, 0, 44, 14);
	size.offset (406, 310);
	this->Labels[42] = new CLabel(size, 0);
	frame->addView(this->Labels[42]);
	size (0, 0, 48, 48);
	size.offset (452, 260);
	this->Knobs[43] = new CAnimKnob(size, this, 43, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[43]);
	size (0, 0, 44, 14);
	size.offset (454, 310);
	this->Labels[43] = new CLabel(size, 0);
	frame->addView(this->Labels[43]);
	size (0, 0, 48, 48);
	size.offset (260, 370);
	this->Knobs[44] = new CAnimKnob(size, this, 44, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[44]);
	size (0, 0, 44, 14);
	size.offset (262, 420);
	this->Labels[44] = new CLabel(size, 0);
	frame->addView(this->Labels[44]);
	size (0, 0, 48, 48);
	size.offset (308, 370);
	this->Knobs[45] = new CAnimKnob(size, this, 45, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[45]);
	size (0, 0, 44, 14);
	size.offset (310, 420);
	this->Labels[45] = new CLabel(size, 0);
	frame->addView(this->Labels[45]);
	size (0, 0, 48, 48);
	size.offset (356, 370);
	this->Knobs[46] = new CAnimKnob(size, this, 46, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[46]);
	size (0, 0, 44, 14);
	size.offset (358, 420);
	this->Labels[46] = new CLabel(size, 0);
	frame->addView(this->Labels[46]);
	size (0, 0, 48, 48);
	size.offset (404, 370);
	this->Knobs[47] = new CAnimKnob(size, this, 47, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[47]);
	size (0, 0, 44, 14);
	size.offset (406, 420);
	this->Labels[47] = new CLabel(size, 0);
	frame->addView(this->Labels[47]);
	size (0, 0, 48, 48);
	size.offset (452, 370);
	this->Knobs[48] = new CAnimKnob(size, this, 48, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[48]);
	size (0, 0, 44, 14);
	size.offset (454, 420);
	this->Labels[48] = new CLabel(size, 0);
	frame->addView(this->Labels[48]);
	size (0, 0, 48, 18);
	size.offset (716, 150);
	this->Buttons[9] = new COnOffButton(size, this, 9, this->BmpButton);
	frame->addView(this->Buttons[9]);
	size (0, 0, 48, 48);
	size.offset (764, 150);
	this->Knobs[10] = new CAnimKnob(size, this, 10, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[10]);
	size (0, 0, 44, 14);
	size.offset (766, 200);
	this->Labels[10] = new CLabel(size, 0);
	frame->addView(this->Labels[10]);
	size (0, 0, 48, 48);
	size.offset (716, 260);
	this->Knobs[11] = new CAnimKnob(size, this, 11, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[11]);
	size (0, 0, 44, 14);
	size.offset (718, 310);
	this->Labels[11] = new CLabel(size, 0);
	frame->addView(this->Labels[11]);
	size (0, 0, 48, 48);
	size.offset (764, 260);
	this->Knobs[12] = new CAnimKnob(size, this, 12, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[12]);
	size (0, 0, 44, 14);
	size.offset (766, 310);
	this->Labels[12] = new CLabel(size, 0);
	frame->addView(this->Labels[12]);
	size (0, 0, 48, 48);
	size.offset (740, 370);
	this->Knobs[0] = new CAnimKnob(size, this, 0, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[0]);
	size (0, 0, 44, 14);
	size.offset (742, 420);
	this->Labels[0] = new CLabel(size, 0);
	frame->addView(this->Labels[0]);
	size (0, 0, 48, 48);
	size.offset (8, 480);
	this->Knobs[61] = new CAnimKnob(size, this, 61, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[61]);
	size (0, 0, 44, 14);
	size.offset (10, 530);
	this->Labels[61] = new CLabel(size, 0);
	frame->addView(this->Labels[61]);
	size (0, 0, 48, 48);
	size.offset (56, 480);
	this->Knobs[62] = new CAnimKnob(size, this, 62, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[62]);
	size (0, 0, 44, 14);
	size.offset (58, 530);
	this->Labels[62] = new CLabel(size, 0);
	frame->addView(this->Labels[62]);
	size (0, 0, 48, 48);
	size.offset (104, 480);
	this->Knobs[63] = new CAnimKnob(size, this, 63, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[63]);
	size (0, 0, 44, 14);
	size.offset (106, 530);
	this->Labels[63] = new CLabel(size, 0);
	frame->addView(this->Labels[63]);
	size (0, 0, 48, 48);
	size.offset (152, 480);
	this->Knobs[64] = new CAnimKnob(size, this, 64, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[64]);
	size (0, 0, 44, 14);
	size.offset (154, 530);
	this->Labels[64] = new CLabel(size, 0);
	frame->addView(this->Labels[64]);
	size (0, 0, 48, 48);
	size.offset (8, 590);
	this->Knobs[65] = new CAnimKnob(size, this, 65, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[65]);
	size (0, 0, 44, 14);
	size.offset (10, 640);
	this->Labels[65] = new CLabel(size, 0);
	frame->addView(this->Labels[65]);
	size (0, 0, 48, 48);
	size.offset (56, 590);
	this->Knobs[66] = new CAnimKnob(size, this, 66, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[66]);
	size (0, 0, 44, 14);
	size.offset (58, 640);
	this->Labels[66] = new CLabel(size, 0);
	frame->addView(this->Labels[66]);
	size (0, 0, 48, 48);
	size.offset (104, 590);
	this->Knobs[67] = new CAnimKnob(size, this, 67, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[67]);
	size (0, 0, 44, 14);
	size.offset (106, 640);
	this->Labels[67] = new CLabel(size, 0);
	frame->addView(this->Labels[67]);
	size (0, 0, 48, 48);
	size.offset (152, 590);
	this->Knobs[68] = new CAnimKnob(size, this, 68, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[68]);
	size (0, 0, 44, 14);
	size.offset (154, 640);
	this->Labels[68] = new CLabel(size, 0);
	frame->addView(this->Labels[68]);
	size (0, 0, 48, 48);
	size.offset (212, 480);
	this->Knobs[69] = new CAnimKnob(size, this, 69, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[69]);
	size (0, 0, 44, 14);
	size.offset (214, 530);
	this->Labels[69] = new CLabel(size, 0);
	frame->addView(this->Labels[69]);
	size (0, 0, 48, 48);
	size.offset (260, 480);
	this->Knobs[70] = new CAnimKnob(size, this, 70, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[70]);
	size (0, 0, 44, 14);
	size.offset (262, 530);
	this->Labels[70] = new CLabel(size, 0);
	frame->addView(this->Labels[70]);
	size (0, 0, 48, 48);
	size.offset (308, 480);
	this->Knobs[71] = new CAnimKnob(size, this, 71, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[71]);
	size (0, 0, 44, 14);
	size.offset (310, 530);
	this->Labels[71] = new CLabel(size, 0);
	frame->addView(this->Labels[71]);
	size (0, 0, 48, 48);
	size.offset (356, 480);
	this->Knobs[72] = new CAnimKnob(size, this, 72, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[72]);
	size (0, 0, 44, 14);
	size.offset (358, 530);
	this->Labels[72] = new CLabel(size, 0);
	frame->addView(this->Labels[72]);
	size (0, 0, 48, 48);
	size.offset (212, 590);
	this->Knobs[73] = new CAnimKnob(size, this, 73, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[73]);
	size (0, 0, 44, 14);
	size.offset (214, 640);
	this->Labels[73] = new CLabel(size, 0);
	frame->addView(this->Labels[73]);
	size (0, 0, 48, 48);
	size.offset (260, 590);
	this->Knobs[74] = new CAnimKnob(size, this, 74, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[74]);
	size (0, 0, 44, 14);
	size.offset (262, 640);
	this->Labels[74] = new CLabel(size, 0);
	frame->addView(this->Labels[74]);
	size (0, 0, 48, 48);
	size.offset (308, 590);
	this->Knobs[75] = new CAnimKnob(size, this, 75, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[75]);
	size (0, 0, 44, 14);
	size.offset (310, 640);
	this->Labels[75] = new CLabel(size, 0);
	frame->addView(this->Labels[75]);
	size (0, 0, 48, 48);
	size.offset (356, 590);
	this->Knobs[76] = new CAnimKnob(size, this, 76, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[76]);
	size (0, 0, 44, 14);
	size.offset (358, 640);
	this->Labels[76] = new CLabel(size, 0);
	frame->addView(this->Labels[76]);
	size (0, 0, 48, 48);
	size.offset (416, 480);
	this->Knobs[77] = new CAnimKnob(size, this, 77, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[77]);
	size (0, 0, 44, 14);
	size.offset (418, 530);
	this->Labels[77] = new CLabel(size, 0);
	frame->addView(this->Labels[77]);
	size (0, 0, 48, 48);
	size.offset (464, 480);
	this->Knobs[78] = new CAnimKnob(size, this, 78, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[78]);
	size (0, 0, 44, 14);
	size.offset (466, 530);
	this->Labels[78] = new CLabel(size, 0);
	frame->addView(this->Labels[78]);
	size (0, 0, 48, 48);
	size.offset (512, 480);
	this->Knobs[79] = new CAnimKnob(size, this, 79, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[79]);
	size (0, 0, 44, 14);
	size.offset (514, 530);
	this->Labels[79] = new CLabel(size, 0);
	frame->addView(this->Labels[79]);
	size (0, 0, 48, 48);
	size.offset (560, 480);
	this->Knobs[80] = new CAnimKnob(size, this, 80, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[80]);
	size (0, 0, 44, 14);
	size.offset (562, 530);
	this->Labels[80] = new CLabel(size, 0);
	frame->addView(this->Labels[80]);
	size (0, 0, 48, 48);
	size.offset (416, 590);
	this->Knobs[81] = new CAnimKnob(size, this, 81, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[81]);
	size (0, 0, 44, 14);
	size.offset (418, 640);
	this->Labels[81] = new CLabel(size, 0);
	frame->addView(this->Labels[81]);
	size (0, 0, 48, 48);
	size.offset (464, 590);
	this->Knobs[82] = new CAnimKnob(size, this, 82, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[82]);
	size (0, 0, 44, 14);
	size.offset (466, 640);
	this->Labels[82] = new CLabel(size, 0);
	frame->addView(this->Labels[82]);
	size (0, 0, 48, 48);
	size.offset (512, 590);
	this->Knobs[83] = new CAnimKnob(size, this, 83, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[83]);
	size (0, 0, 44, 14);
	size.offset (514, 640);
	this->Labels[83] = new CLabel(size, 0);
	frame->addView(this->Labels[83]);
	size (0, 0, 48, 48);
	size.offset (560, 590);
	this->Knobs[84] = new CAnimKnob(size, this, 84, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[84]);
	size (0, 0, 44, 14);
	size.offset (562, 640);
	this->Labels[84] = new CLabel(size, 0);
	frame->addView(this->Labels[84]);
	size (0, 0, 48, 48);
	size.offset (620, 480);
	this->Knobs[85] = new CAnimKnob(size, this, 85, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[85]);
	size (0, 0, 44, 14);
	size.offset (622, 530);
	this->Labels[85] = new CLabel(size, 0);
	frame->addView(this->Labels[85]);
	size (0, 0, 48, 48);
	size.offset (668, 480);
	this->Knobs[86] = new CAnimKnob(size, this, 86, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[86]);
	size (0, 0, 44, 14);
	size.offset (670, 530);
	this->Labels[86] = new CLabel(size, 0);
	frame->addView(this->Labels[86]);
	size (0, 0, 48, 48);
	size.offset (716, 480);
	this->Knobs[87] = new CAnimKnob(size, this, 87, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[87]);
	size (0, 0, 44, 14);
	size.offset (718, 530);
	this->Labels[87] = new CLabel(size, 0);
	frame->addView(this->Labels[87]);
	size (0, 0, 48, 48);
	size.offset (764, 480);
	this->Knobs[88] = new CAnimKnob(size, this, 88, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[88]);
	size (0, 0, 44, 14);
	size.offset (766, 530);
	this->Labels[88] = new CLabel(size, 0);
	frame->addView(this->Labels[88]);
	size (0, 0, 48, 48);
	size.offset (620, 590);
	this->Knobs[89] = new CAnimKnob(size, this, 89, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[89]);
	size (0, 0, 44, 14);
	size.offset (622, 640);
	this->Labels[89] = new CLabel(size, 0);
	frame->addView(this->Labels[89]);
	size (0, 0, 48, 48);
	size.offset (668, 590);
	this->Knobs[90] = new CAnimKnob(size, this, 90, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[90]);
	size (0, 0, 44, 14);
	size.offset (670, 640);
	this->Labels[90] = new CLabel(size, 0);
	frame->addView(this->Labels[90]);
	size (0, 0, 48, 48);
	size.offset (716, 590);
	this->Knobs[91] = new CAnimKnob(size, this, 91, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[91]);
	size (0, 0, 44, 14);
	size.offset (718, 640);
	this->Labels[91] = new CLabel(size, 0);
	frame->addView(this->Labels[91]);
	size (0, 0, 48, 48);
	size.offset (764, 590);
	this->Knobs[92] = new CAnimKnob(size, this, 92, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[92]);
	size (0, 0, 44, 14);
	size.offset (766, 640);
	this->Labels[92] = new CLabel(size, 0);
	frame->addView(this->Labels[92]);
	size (0, 0, 48, 48);
	size.offset (512, 150);
	this->Knobs[49] = new CAnimKnob(size, this, 49, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[49]);
	size (0, 0, 44, 14);
	size.offset (514, 200);
	this->Labels[49] = new CLabel(size, 0);
	frame->addView(this->Labels[49]);
	size (0, 0, 48, 48);
	size.offset (560, 150);
	this->Knobs[50] = new CAnimKnob(size, this, 50, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[50]);
	size (0, 0, 44, 14);
	size.offset (562, 200);
	this->Labels[50] = new CLabel(size, 0);
	frame->addView(this->Labels[50]);
	size (0, 0, 48, 48);
	size.offset (608, 150);
	this->Knobs[51] = new CAnimKnob(size, this, 51, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[51]);
	size (0, 0, 44, 14);
	size.offset (610, 200);
	this->Labels[51] = new CLabel(size, 0);
	frame->addView(this->Labels[51]);
	size (0, 0, 48, 18);
	size.offset (656, 150);
	this->Buttons[52] = new COnOffButton(size, this, 52, this->BmpButton);
	frame->addView(this->Buttons[52]);
	size (0, 0, 48, 48);
	size.offset (512, 260);
	this->Knobs[53] = new CAnimKnob(size, this, 53, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[53]);
	size (0, 0, 44, 14);
	size.offset (514, 310);
	this->Labels[53] = new CLabel(size, 0);
	frame->addView(this->Labels[53]);
	size (0, 0, 48, 48);
	size.offset (560, 260);
	this->Knobs[54] = new CAnimKnob(size, this, 54, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[54]);
	size (0, 0, 44, 14);
	size.offset (562, 310);
	this->Labels[54] = new CLabel(size, 0);
	frame->addView(this->Labels[54]);
	size (0, 0, 48, 48);
	size.offset (608, 260);
	this->Knobs[55] = new CAnimKnob(size, this, 55, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[55]);
	size (0, 0, 44, 14);
	size.offset (610, 310);
	this->Labels[55] = new CLabel(size, 0);
	frame->addView(this->Labels[55]);
	size (0, 0, 48, 18);
	size.offset (656, 260);
	this->Buttons[56] = new COnOffButton(size, this, 56, this->BmpButton);
	frame->addView(this->Buttons[56]);
	size (0, 0, 48, 48);
	size.offset (512, 370);
	this->Knobs[57] = new CAnimKnob(size, this, 57, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[57]);
	size (0, 0, 44, 14);
	size.offset (514, 420);
	this->Labels[57] = new CLabel(size, 0);
	frame->addView(this->Labels[57]);
	size (0, 0, 48, 48);
	size.offset (560, 370);
	this->Knobs[58] = new CAnimKnob(size, this, 58, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[58]);
	size (0, 0, 44, 14);
	size.offset (562, 420);
	this->Labels[58] = new CLabel(size, 0);
	frame->addView(this->Labels[58]);
	size (0, 0, 48, 48);
	size.offset (608, 370);
	this->Knobs[59] = new CAnimKnob(size, this, 59, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[59]);
	size (0, 0, 44, 14);
	size.offset (610, 420);
	this->Labels[59] = new CLabel(size, 0);
	frame->addView(this->Labels[59]);
	size (0, 0, 48, 48);
	size.offset (656, 370);
	this->Knobs[60] = new CAnimKnob(size, this, 60, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[60]);
	size (0, 0, 44, 14);
	size.offset (658, 420);
	this->Labels[60] = new CLabel(size, 0);
	frame->addView(this->Labels[60]);

	for(int i = 0; i < pParameters; i++)
	{
		if(this->Knobs[i])
			this->Knobs[i]->setZoomFactor(2.5f);

		this->setParameter(i, effect->getParameter(i));
	}

	return true;
}

void CCetoneEditor::close()
{
	if(frame)
		delete frame;
	frame = 0;

	if(this->BmpKnob)
	{
		this->BmpKnob->forget();
		this->BmpKnob = 0;
	}

	if(this->BmpButton)
	{
		this->BmpButton->forget();
		this->BmpButton = 0;
	}
}

void CCetoneEditor::setParameter(VstInt32 index, float value)
{
	if(!frame)
		return;

	if(index < 0 || index >= pParameters)
		return;

	char text[128];

	switch(index)
	{
	case pOsc1Ring:
	case pOsc2Ring:
	case pOsc3Ring:
	case pOsc1Sync:
	case pOsc2Sync:
	case pOsc3Sync:
	case pPortaMode:
	case pLfo1Trig:
	case pLfo2Trig:
		if(this->Buttons[index])
			this->Buttons[index]->setValue((value < 0.5f) ? 0.f : 1.f);
		break;
	default:
		if(this->Knobs[index])
		{
			this->Knobs[index]->setValue(value);
			effect->getParameterDisplay(index, text);
			this->Labels[index]->setLabel(text);
		}
		break;
	}

}

void CCetoneEditor::valueChanged(CDrawContext* context, CControl* control)
{
	if(!control)
		return;

	char text[128];

	float value = control->getValue();
	VstInt32 index = control->getTag();

	if(index < 0 || index >= pParameters)
		return;

	switch (index)
	{
	case pOsc1Ring:
	case pOsc2Ring:
	case pOsc3Ring:
	case pOsc1Sync:
	case pOsc2Sync:
	case pOsc3Sync:
	case pPortaMode:
	case pLfo1Trig:
	case pLfo2Trig:
		effect->setParameterAutomated(index, (value < 0.5f) ? 0.f : 1.f);
		break;
	default:
		effect->setParameterAutomated(index, value);
		effect->getParameterDisplay(index, text);
		this->Labels[index]->setLabel(text);
		break;
	}
}

void CCetoneEditor::idle()
{
	AEffGUIEditor::idle(); 	
}
