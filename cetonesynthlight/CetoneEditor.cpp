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
	size.offset (260, 40);
	this->Knobs[0] = new CAnimKnob(size, this, 0, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[0]);
	size (0, 0, 44, 14);
	size.offset (262, 90);
	this->Labels[0] = new CLabel(size, 0);
	frame->addView(this->Labels[0]);
	size (0, 0, 48, 48);
	size.offset (308, 40);
	this->Knobs[1] = new CAnimKnob(size, this, 1, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[1]);
	size (0, 0, 44, 14);
	size.offset (310, 90);
	this->Labels[1] = new CLabel(size, 0);
	frame->addView(this->Labels[1]);
	size (0, 0, 48, 48);
	size.offset (356, 40);
	this->Knobs[16] = new CAnimKnob(size, this, 16, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[16]);
	size (0, 0, 44, 14);
	size.offset (358, 90);
	this->Labels[16] = new CLabel(size, 0);
	frame->addView(this->Labels[16]);
	size (0, 0, 48, 48);
	size.offset (404, 40);
	this->Knobs[23] = new CAnimKnob(size, this, 23, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[23]);
	size (0, 0, 44, 14);
	size.offset (406, 90);
	this->Labels[23] = new CLabel(size, 0);
	frame->addView(this->Labels[23]);
	size (0, 0, 48, 48);
	size.offset (452, 40);
	this->Knobs[30] = new CAnimKnob(size, this, 30, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[30]);
	size (0, 0, 44, 14);
	size.offset (454, 90);
	this->Labels[30] = new CLabel(size, 0);
	frame->addView(this->Labels[30]);
	size (0, 0, 48, 48);
	size.offset (512, 40);
	this->Knobs[4] = new CAnimKnob(size, this, 4, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[4]);
	size (0, 0, 44, 14);
	size.offset (514, 90);
	this->Labels[4] = new CLabel(size, 0);
	frame->addView(this->Labels[4]);
	size (0, 0, 48, 48);
	size.offset (560, 40);
	this->Knobs[5] = new CAnimKnob(size, this, 5, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[5]);
	size (0, 0, 44, 14);
	size.offset (562, 90);
	this->Labels[5] = new CLabel(size, 0);
	frame->addView(this->Labels[5]);
	size (0, 0, 48, 48);
	size.offset (608, 40);
	this->Knobs[6] = new CAnimKnob(size, this, 6, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[6]);
	size (0, 0, 44, 14);
	size.offset (610, 90);
	this->Labels[6] = new CLabel(size, 0);
	frame->addView(this->Labels[6]);
	size (0, 0, 48, 48);
	size.offset (656, 40);
	this->Knobs[7] = new CAnimKnob(size, this, 7, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[7]);
	size (0, 0, 44, 14);
	size.offset (658, 90);
	this->Labels[7] = new CLabel(size, 0);
	frame->addView(this->Labels[7]);
	size (0, 0, 48, 48);
	size.offset (704, 40);
	this->Knobs[63] = new CAnimKnob(size, this, 63, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[63]);
	size (0, 0, 44, 14);
	size.offset (706, 90);
	this->Labels[63] = new CLabel(size, 0);
	frame->addView(this->Labels[63]);
	size (0, 0, 48, 48);
	size.offset (764, 40);
	this->Knobs[2] = new CAnimKnob(size, this, 2, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[2]);
	size (0, 0, 44, 14);
	size.offset (766, 90);
	this->Labels[2] = new CLabel(size, 0);
	frame->addView(this->Labels[2]);
	size (0, 0, 48, 48);
	size.offset (812, 40);
	this->Knobs[3] = new CAnimKnob(size, this, 3, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[3]);
	size (0, 0, 44, 14);
	size.offset (814, 90);
	this->Labels[3] = new CLabel(size, 0);
	frame->addView(this->Labels[3]);
	size (0, 0, 48, 48);
	size.offset (8, 150);
	this->Knobs[12] = new CAnimKnob(size, this, 12, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[12]);
	size (0, 0, 44, 14);
	size.offset (10, 200);
	this->Labels[12] = new CLabel(size, 0);
	frame->addView(this->Labels[12]);
	size (0, 0, 48, 48);
	size.offset (56, 150);
	this->Knobs[13] = new CAnimKnob(size, this, 13, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[13]);
	size (0, 0, 44, 14);
	size.offset (58, 200);
	this->Labels[13] = new CLabel(size, 0);
	frame->addView(this->Labels[13]);
	size (0, 0, 48, 48);
	size.offset (104, 150);
	this->Knobs[14] = new CAnimKnob(size, this, 14, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[14]);
	size (0, 0, 44, 14);
	size.offset (106, 200);
	this->Labels[14] = new CLabel(size, 0);
	frame->addView(this->Labels[14]);
	size (0, 0, 48, 48);
	size.offset (152, 150);
	this->Knobs[15] = new CAnimKnob(size, this, 15, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[15]);
	size (0, 0, 44, 14);
	size.offset (154, 200);
	this->Labels[15] = new CLabel(size, 0);
	frame->addView(this->Labels[15]);
	size (0, 0, 48, 18);
	size.offset (200, 150);
	this->Buttons[17] = new COnOffButton(size, this, 17, this->BmpButton);
	frame->addView(this->Buttons[17]);
	size (0, 0, 48, 18);
	size.offset (200, 198);
	this->Buttons[18] = new COnOffButton(size, this, 18, this->BmpButton);
	frame->addView(this->Buttons[18]);
	size (0, 0, 48, 48);
	size.offset (260, 150);
	this->Knobs[19] = new CAnimKnob(size, this, 19, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[19]);
	size (0, 0, 44, 14);
	size.offset (262, 200);
	this->Labels[19] = new CLabel(size, 0);
	frame->addView(this->Labels[19]);
	size (0, 0, 48, 48);
	size.offset (308, 150);
	this->Knobs[20] = new CAnimKnob(size, this, 20, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[20]);
	size (0, 0, 44, 14);
	size.offset (310, 200);
	this->Labels[20] = new CLabel(size, 0);
	frame->addView(this->Labels[20]);
	size (0, 0, 48, 48);
	size.offset (356, 150);
	this->Knobs[21] = new CAnimKnob(size, this, 21, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[21]);
	size (0, 0, 44, 14);
	size.offset (358, 200);
	this->Labels[21] = new CLabel(size, 0);
	frame->addView(this->Labels[21]);
	size (0, 0, 48, 48);
	size.offset (404, 150);
	this->Knobs[22] = new CAnimKnob(size, this, 22, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[22]);
	size (0, 0, 44, 14);
	size.offset (406, 200);
	this->Labels[22] = new CLabel(size, 0);
	frame->addView(this->Labels[22]);
	size (0, 0, 48, 18);
	size.offset (452, 150);
	this->Buttons[24] = new COnOffButton(size, this, 24, this->BmpButton);
	frame->addView(this->Buttons[24]);
	size (0, 0, 48, 18);
	size.offset (452, 198);
	this->Buttons[25] = new COnOffButton(size, this, 25, this->BmpButton);
	frame->addView(this->Buttons[25]);
	size (0, 0, 48, 48);
	size.offset (512, 150);
	this->Knobs[26] = new CAnimKnob(size, this, 26, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[26]);
	size (0, 0, 44, 14);
	size.offset (514, 200);
	this->Labels[26] = new CLabel(size, 0);
	frame->addView(this->Labels[26]);
	size (0, 0, 48, 48);
	size.offset (560, 150);
	this->Knobs[27] = new CAnimKnob(size, this, 27, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[27]);
	size (0, 0, 44, 14);
	size.offset (562, 200);
	this->Labels[27] = new CLabel(size, 0);
	frame->addView(this->Labels[27]);
	size (0, 0, 48, 48);
	size.offset (608, 150);
	this->Knobs[28] = new CAnimKnob(size, this, 28, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[28]);
	size (0, 0, 44, 14);
	size.offset (610, 200);
	this->Labels[28] = new CLabel(size, 0);
	frame->addView(this->Labels[28]);
	size (0, 0, 48, 48);
	size.offset (656, 150);
	this->Knobs[29] = new CAnimKnob(size, this, 29, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[29]);
	size (0, 0, 44, 14);
	size.offset (658, 200);
	this->Labels[29] = new CLabel(size, 0);
	frame->addView(this->Labels[29]);
	size (0, 0, 48, 18);
	size.offset (704, 150);
	this->Buttons[31] = new COnOffButton(size, this, 31, this->BmpButton);
	frame->addView(this->Buttons[31]);
	size (0, 0, 48, 18);
	size.offset (704, 198);
	this->Buttons[32] = new COnOffButton(size, this, 32, this->BmpButton);
	frame->addView(this->Buttons[32]);
	size (0, 0, 48, 48);
	size.offset (8, 260);
	this->Knobs[33] = new CAnimKnob(size, this, 33, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[33]);
	size (0, 0, 44, 14);
	size.offset (10, 310);
	this->Labels[33] = new CLabel(size, 0);
	frame->addView(this->Labels[33]);
	size (0, 0, 48, 48);
	size.offset (56, 260);
	this->Knobs[34] = new CAnimKnob(size, this, 34, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[34]);
	size (0, 0, 44, 14);
	size.offset (58, 310);
	this->Labels[34] = new CLabel(size, 0);
	frame->addView(this->Labels[34]);
	size (0, 0, 48, 48);
	size.offset (104, 260);
	this->Knobs[35] = new CAnimKnob(size, this, 35, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[35]);
	size (0, 0, 44, 14);
	size.offset (106, 310);
	this->Labels[35] = new CLabel(size, 0);
	frame->addView(this->Labels[35]);
	size (0, 0, 48, 48);
	size.offset (152, 260);
	this->Knobs[36] = new CAnimKnob(size, this, 36, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[36]);
	size (0, 0, 44, 14);
	size.offset (154, 310);
	this->Labels[36] = new CLabel(size, 0);
	frame->addView(this->Labels[36]);
	size (0, 0, 48, 48);
	size.offset (200, 260);
	this->Knobs[37] = new CAnimKnob(size, this, 37, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[37]);
	size (0, 0, 44, 14);
	size.offset (202, 310);
	this->Labels[37] = new CLabel(size, 0);
	frame->addView(this->Labels[37]);
	size (0, 0, 48, 48);
	size.offset (260, 260);
	this->Knobs[38] = new CAnimKnob(size, this, 38, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[38]);
	size (0, 0, 44, 14);
	size.offset (262, 310);
	this->Labels[38] = new CLabel(size, 0);
	frame->addView(this->Labels[38]);
	size (0, 0, 48, 48);
	size.offset (308, 260);
	this->Knobs[39] = new CAnimKnob(size, this, 39, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[39]);
	size (0, 0, 44, 14);
	size.offset (310, 310);
	this->Labels[39] = new CLabel(size, 0);
	frame->addView(this->Labels[39]);
	size (0, 0, 48, 48);
	size.offset (356, 260);
	this->Knobs[40] = new CAnimKnob(size, this, 40, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[40]);
	size (0, 0, 44, 14);
	size.offset (358, 310);
	this->Labels[40] = new CLabel(size, 0);
	frame->addView(this->Labels[40]);
	size (0, 0, 48, 48);
	size.offset (404, 260);
	this->Knobs[41] = new CAnimKnob(size, this, 41, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[41]);
	size (0, 0, 44, 14);
	size.offset (406, 310);
	this->Labels[41] = new CLabel(size, 0);
	frame->addView(this->Labels[41]);
	size (0, 0, 48, 48);
	size.offset (452, 260);
	this->Knobs[42] = new CAnimKnob(size, this, 42, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[42]);
	size (0, 0, 44, 14);
	size.offset (454, 310);
	this->Labels[42] = new CLabel(size, 0);
	frame->addView(this->Labels[42]);
	size (0, 0, 48, 18);
	size.offset (764, 150);
	this->Buttons[8] = new COnOffButton(size, this, 8, this->BmpButton);
	frame->addView(this->Buttons[8]);
	size (0, 0, 48, 48);
	size.offset (812, 150);
	this->Knobs[9] = new CAnimKnob(size, this, 9, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[9]);
	size (0, 0, 44, 14);
	size.offset (814, 200);
	this->Labels[9] = new CLabel(size, 0);
	frame->addView(this->Labels[9]);
	size (0, 0, 48, 48);
	size.offset (764, 260);
	this->Knobs[10] = new CAnimKnob(size, this, 10, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[10]);
	size (0, 0, 44, 14);
	size.offset (766, 310);
	this->Labels[10] = new CLabel(size, 0);
	frame->addView(this->Labels[10]);
	size (0, 0, 48, 48);
	size.offset (812, 260);
	this->Knobs[11] = new CAnimKnob(size, this, 11, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[11]);
	size (0, 0, 44, 14);
	size.offset (814, 310);
	this->Labels[11] = new CLabel(size, 0);
	frame->addView(this->Labels[11]);
	size (0, 0, 48, 48);
	size.offset (8, 370);
	this->Knobs[47] = new CAnimKnob(size, this, 47, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[47]);
	size (0, 0, 44, 14);
	size.offset (10, 420);
	this->Labels[47] = new CLabel(size, 0);
	frame->addView(this->Labels[47]);
	size (0, 0, 48, 48);
	size.offset (56, 370);
	this->Knobs[48] = new CAnimKnob(size, this, 48, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[48]);
	size (0, 0, 44, 14);
	size.offset (58, 420);
	this->Labels[48] = new CLabel(size, 0);
	frame->addView(this->Labels[48]);
	size (0, 0, 48, 48);
	size.offset (104, 370);
	this->Knobs[49] = new CAnimKnob(size, this, 49, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[49]);
	size (0, 0, 44, 14);
	size.offset (106, 420);
	this->Labels[49] = new CLabel(size, 0);
	frame->addView(this->Labels[49]);
	size (0, 0, 48, 48);
	size.offset (152, 370);
	this->Knobs[50] = new CAnimKnob(size, this, 50, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[50]);
	size (0, 0, 44, 14);
	size.offset (154, 420);
	this->Labels[50] = new CLabel(size, 0);
	frame->addView(this->Labels[50]);
	size (0, 0, 48, 48);
	size.offset (228, 370);
	this->Knobs[51] = new CAnimKnob(size, this, 51, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[51]);
	size (0, 0, 44, 14);
	size.offset (230, 420);
	this->Labels[51] = new CLabel(size, 0);
	frame->addView(this->Labels[51]);
	size (0, 0, 48, 48);
	size.offset (276, 370);
	this->Knobs[52] = new CAnimKnob(size, this, 52, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[52]);
	size (0, 0, 44, 14);
	size.offset (278, 420);
	this->Labels[52] = new CLabel(size, 0);
	frame->addView(this->Labels[52]);
	size (0, 0, 48, 48);
	size.offset (324, 370);
	this->Knobs[53] = new CAnimKnob(size, this, 53, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[53]);
	size (0, 0, 44, 14);
	size.offset (326, 420);
	this->Labels[53] = new CLabel(size, 0);
	frame->addView(this->Labels[53]);
	size (0, 0, 48, 48);
	size.offset (372, 370);
	this->Knobs[54] = new CAnimKnob(size, this, 54, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[54]);
	size (0, 0, 44, 14);
	size.offset (374, 420);
	this->Labels[54] = new CLabel(size, 0);
	frame->addView(this->Labels[54]);
	size (0, 0, 48, 48);
	size.offset (448, 370);
	this->Knobs[55] = new CAnimKnob(size, this, 55, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[55]);
	size (0, 0, 44, 14);
	size.offset (450, 420);
	this->Labels[55] = new CLabel(size, 0);
	frame->addView(this->Labels[55]);
	size (0, 0, 48, 48);
	size.offset (496, 370);
	this->Knobs[56] = new CAnimKnob(size, this, 56, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[56]);
	size (0, 0, 44, 14);
	size.offset (498, 420);
	this->Labels[56] = new CLabel(size, 0);
	frame->addView(this->Labels[56]);
	size (0, 0, 48, 48);
	size.offset (544, 370);
	this->Knobs[57] = new CAnimKnob(size, this, 57, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[57]);
	size (0, 0, 44, 14);
	size.offset (546, 420);
	this->Labels[57] = new CLabel(size, 0);
	frame->addView(this->Labels[57]);
	size (0, 0, 48, 48);
	size.offset (592, 370);
	this->Knobs[58] = new CAnimKnob(size, this, 58, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[58]);
	size (0, 0, 44, 14);
	size.offset (594, 420);
	this->Labels[58] = new CLabel(size, 0);
	frame->addView(this->Labels[58]);
	size (0, 0, 48, 48);
	size.offset (668, 370);
	this->Knobs[59] = new CAnimKnob(size, this, 59, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[59]);
	size (0, 0, 44, 14);
	size.offset (670, 420);
	this->Labels[59] = new CLabel(size, 0);
	frame->addView(this->Labels[59]);
	size (0, 0, 48, 48);
	size.offset (716, 370);
	this->Knobs[60] = new CAnimKnob(size, this, 60, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[60]);
	size (0, 0, 44, 14);
	size.offset (718, 420);
	this->Labels[60] = new CLabel(size, 0);
	frame->addView(this->Labels[60]);
	size (0, 0, 48, 48);
	size.offset (764, 370);
	this->Knobs[61] = new CAnimKnob(size, this, 61, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[61]);
	size (0, 0, 44, 14);
	size.offset (766, 420);
	this->Labels[61] = new CLabel(size, 0);
	frame->addView(this->Labels[61]);
	size (0, 0, 48, 48);
	size.offset (812, 370);
	this->Knobs[62] = new CAnimKnob(size, this, 62, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[62]);
	size (0, 0, 44, 14);
	size.offset (814, 420);
	this->Labels[62] = new CLabel(size, 0);
	frame->addView(this->Labels[62]);
	size (0, 0, 48, 48);
	size.offset (532, 260);
	this->Knobs[43] = new CAnimKnob(size, this, 43, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[43]);
	size (0, 0, 44, 14);
	size.offset (534, 310);
	this->Labels[43] = new CLabel(size, 0);
	frame->addView(this->Labels[43]);
	size (0, 0, 48, 48);
	size.offset (580, 260);
	this->Knobs[44] = new CAnimKnob(size, this, 44, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[44]);
	size (0, 0, 44, 14);
	size.offset (582, 310);
	this->Labels[44] = new CLabel(size, 0);
	frame->addView(this->Labels[44]);
	size (0, 0, 48, 48);
	size.offset (628, 260);
	this->Knobs[45] = new CAnimKnob(size, this, 45, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[45]);
	size (0, 0, 44, 14);
	size.offset (630, 310);
	this->Labels[45] = new CLabel(size, 0);
	frame->addView(this->Labels[45]);
	size (0, 0, 48, 18);
	size.offset (676, 260);
	this->Buttons[46] = new COnOffButton(size, this, 46, this->BmpButton);
	frame->addView(this->Buttons[46]);

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
