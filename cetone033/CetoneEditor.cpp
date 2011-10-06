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
	size.offset (8, 68);
	this->Knobs[0] = new CAnimKnob(size, this, 0, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[0]);
	size (0, 0, 44, 14);
	size.offset (10, 118);
	this->Labels[0] = new CLabel(size, 0);
	frame->addView(this->Labels[0]);
	size (0, 0, 48, 48);
	size.offset (56, 68);
	this->Knobs[1] = new CAnimKnob(size, this, 1, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[1]);
	size (0, 0, 44, 14);
	size.offset (58, 118);
	this->Labels[1] = new CLabel(size, 0);
	frame->addView(this->Labels[1]);
	size (0, 0, 48, 48);
	size.offset (104, 68);
	this->Knobs[2] = new CAnimKnob(size, this, 2, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[2]);
	size (0, 0, 44, 14);
	size.offset (106, 118);
	this->Labels[2] = new CLabel(size, 0);
	frame->addView(this->Labels[2]);
	size (0, 0, 48, 48);
	size.offset (152, 68);
	this->Knobs[3] = new CAnimKnob(size, this, 3, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[3]);
	size (0, 0, 44, 14);
	size.offset (154, 118);
	this->Labels[3] = new CLabel(size, 0);
	frame->addView(this->Labels[3]);
	size (0, 0, 48, 48);
	size.offset (200, 68);
	this->Knobs[4] = new CAnimKnob(size, this, 4, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[4]);
	size (0, 0, 44, 14);
	size.offset (202, 118);
	this->Labels[4] = new CLabel(size, 0);
	frame->addView(this->Labels[4]);
	size (0, 0, 48, 48);
	size.offset (272, 68);
	this->Knobs[5] = new CAnimKnob(size, this, 5, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[5]);
	size (0, 0, 44, 14);
	size.offset (274, 118);
	this->Labels[5] = new CLabel(size, 0);
	frame->addView(this->Labels[5]);
	size (0, 0, 48, 48);
	size.offset (320, 68);
	this->Knobs[6] = new CAnimKnob(size, this, 6, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[6]);
	size (0, 0, 44, 14);
	size.offset (322, 118);
	this->Labels[6] = new CLabel(size, 0);
	frame->addView(this->Labels[6]);
	size (0, 0, 48, 48);
	size.offset (368, 68);
	this->Knobs[7] = new CAnimKnob(size, this, 7, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[7]);
	size (0, 0, 44, 14);
	size.offset (370, 118);
	this->Labels[7] = new CLabel(size, 0);
	frame->addView(this->Labels[7]);
	size (0, 0, 48, 48);
	size.offset (416, 68);
	this->Knobs[8] = new CAnimKnob(size, this, 8, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[8]);
	size (0, 0, 44, 14);
	size.offset (418, 118);
	this->Labels[8] = new CLabel(size, 0);
	frame->addView(this->Labels[8]);
	size (0, 0, 48, 48);
	size.offset (464, 68);
	this->Knobs[9] = new CAnimKnob(size, this, 9, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[9]);
	size (0, 0, 44, 14);
	size.offset (466, 118);
	this->Labels[9] = new CLabel(size, 0);
	frame->addView(this->Labels[9]);
	size (0, 0, 48, 18);
	size.offset (536, 68);
	this->Buttons[21] = new COnOffButton(size, this, 21, this->BmpButton);
	frame->addView(this->Buttons[21]);
	size (0, 0, 48, 48);
	size.offset (584, 68);
	this->Knobs[22] = new CAnimKnob(size, this, 22, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[22]);
	size (0, 0, 44, 14);
	size.offset (586, 118);
	this->Labels[22] = new CLabel(size, 0);
	frame->addView(this->Labels[22]);
	size (0, 0, 48, 48);
	size.offset (8, 178);
	this->Knobs[10] = new CAnimKnob(size, this, 10, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[10]);
	size (0, 0, 44, 14);
	size.offset (10, 228);
	this->Labels[10] = new CLabel(size, 0);
	frame->addView(this->Labels[10]);
	size (0, 0, 48, 48);
	size.offset (56, 178);
	this->Knobs[11] = new CAnimKnob(size, this, 11, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[11]);
	size (0, 0, 44, 14);
	size.offset (58, 228);
	this->Labels[11] = new CLabel(size, 0);
	frame->addView(this->Labels[11]);
	size (0, 0, 48, 48);
	size.offset (116, 178);
	this->Knobs[12] = new CAnimKnob(size, this, 12, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[12]);
	size (0, 0, 44, 14);
	size.offset (118, 228);
	this->Labels[12] = new CLabel(size, 0);
	frame->addView(this->Labels[12]);
	size (0, 0, 48, 48);
	size.offset (164, 178);
	this->Knobs[13] = new CAnimKnob(size, this, 13, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[13]);
	size (0, 0, 44, 14);
	size.offset (166, 228);
	this->Labels[13] = new CLabel(size, 0);
	frame->addView(this->Labels[13]);
	size (0, 0, 48, 48);
	size.offset (224, 178);
	this->Knobs[14] = new CAnimKnob(size, this, 14, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[14]);
	size (0, 0, 44, 14);
	size.offset (226, 228);
	this->Labels[14] = new CLabel(size, 0);
	frame->addView(this->Labels[14]);
	size (0, 0, 48, 48);
	size.offset (272, 178);
	this->Knobs[15] = new CAnimKnob(size, this, 15, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[15]);
	size (0, 0, 44, 14);
	size.offset (274, 228);
	this->Labels[15] = new CLabel(size, 0);
	frame->addView(this->Labels[15]);
	size (0, 0, 48, 48);
	size.offset (320, 178);
	this->Knobs[16] = new CAnimKnob(size, this, 16, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[16]);
	size (0, 0, 44, 14);
	size.offset (322, 228);
	this->Labels[16] = new CLabel(size, 0);
	frame->addView(this->Labels[16]);
	size (0, 0, 48, 48);
	size.offset (380, 178);
	this->Knobs[17] = new CAnimKnob(size, this, 17, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[17]);
	size (0, 0, 44, 14);
	size.offset (382, 228);
	this->Labels[17] = new CLabel(size, 0);
	frame->addView(this->Labels[17]);
	size (0, 0, 48, 48);
	size.offset (428, 178);
	this->Knobs[18] = new CAnimKnob(size, this, 18, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[18]);
	size (0, 0, 44, 14);
	size.offset (430, 228);
	this->Labels[18] = new CLabel(size, 0);
	frame->addView(this->Labels[18]);
	size (0, 0, 48, 48);
	size.offset (476, 178);
	this->Knobs[23] = new CAnimKnob(size, this, 23, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[23]);
	size (0, 0, 44, 14);
	size.offset (478, 228);
	this->Labels[23] = new CLabel(size, 0);
	frame->addView(this->Labels[23]);
	size (0, 0, 48, 18);
	size.offset (536, 178);
	this->Buttons[19] = new COnOffButton(size, this, 19, this->BmpButton);
	frame->addView(this->Buttons[19]);
	size (0, 0, 48, 48);
	size.offset (584, 178);
	this->Knobs[20] = new CAnimKnob(size, this, 20, 270, 48, this->BmpKnob, point);
	frame->addView(this->Knobs[20]);
	size (0, 0, 44, 14);
	size.offset (586, 228);
	this->Labels[20] = new CLabel(size, 0);
	frame->addView(this->Labels[20]);

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
	case pGlideState:
	case pClipState:
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
	case pGlideState:
	case pClipState:
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
