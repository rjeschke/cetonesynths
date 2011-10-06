#pragma once

#include "cetone033.h"

#include "aeffguieditor.h"

enum
{
	rBackground = 128,
	rKnobs,
	rButtons
};

class CLabel : public CParamDisplay
{
public:
	CLabel(CRect &size, char *text);
	void draw(CDrawContext *context);
	void setLabel(char *text);

protected:
	char label[256];
};

class CCetoneEditor : public AEffGUIEditor, public CControlListener
{
public:
	CCetoneEditor(AudioEffect* effect);
	virtual ~CCetoneEditor(void);

	virtual bool getRect(ERect **rect);
	virtual bool open(void *ptr);
	virtual void close();

	virtual void setParameter(VstInt32 index, float value);
	virtual void valueChanged(CDrawContext* context, CControl* control);

	virtual void idle();

private:
	ERect			GuiSize;
	CBitmap*		BmpBackground;
	CBitmap*		BmpKnob;
	CBitmap*		BmpButton;

	CAnimKnob*		Knobs[pParameters];
	COnOffButton*	Buttons[pParameters];
	CLabel*			Labels[pParameters];
 };
