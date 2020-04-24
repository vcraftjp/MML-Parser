//
// DrawTone.h
//
#pragma once

#include <stddef.h>

#define BG_COLOR TFT_NAVY

//
// DrawTone class
//
class DrawTone {
public:
	DrawTone();

	void showTone(int note);
	void hideTone();
	void update(unsigned long tick);

private:
	bool initialized;
	int cx, cy;
	int track;
	int dy;
	unsigned long prevTick;
	unsigned long hideTick; // for delayed hide

	static int trackCount;

	void init();
	void _hideTone();
};
