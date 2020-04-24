//
// DrawTone.cpp
//

#include "M5.h"
#include "DrawTone.h"

#define TONE_FONT 4
#define SHARP_FONT 1
#ifdef ARDUINO_M5Stick_C
#define DISH_SIZE 16
#else
#define DISH_SIZE 32
#endif
#define HIDE_DELAY 50
#define OCTAVE_COUNT 8

int DrawTone::trackCount = 0;

static const char* noteSymbols[] = { "C", "C", "D", "D", "E", "F", "F", "G", "G", "A", "A", "B" };
static const uint8_t sharpFlags[] = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
static const uint16_t noteColors[] = {
	TFT_RED, 0xFAC0 /*(255,90,0)*/, TFT_ORANGE, 0xFEC0 /*(255,216,0)*/, TFT_YELLOW, TFT_GREEN, 0x0692 /*(0,208,144)*/,
	0x041F/*(0,128,255)*/, 0x621C /*(96,64,224)*/, 0xC018 /*(192,0,192)*/, 0xE01C /*(224,0,224)*/, TFT_MAGENTA };

DrawTone::DrawTone() {
	track = trackCount++;
	initialized = false;
	hideTick = 0;
	dy = 0;
}

void DrawTone::showTone(int note) {
	if (!initialized) {
		initialized = true;
		init();
	}
	hideTick = 0;
#ifndef ARDUINO_M5Stick_C
	M5.Lcd.setTextSize(2);
#endif
	int h = M5.Lcd.fontHeight(TONE_FONT);
	int index = note % 12;
	int _dy = (5 - note / 12) * (M5.Lcd.height() / OCTAVE_COUNT);
	if (dy != _dy) {
		_hideTone();
		dy = _dy;
	}
	M5.Lcd.fillCircle(cx, cy + dy, DISH_SIZE, noteColors[index]);
	M5.Lcd.setTextColor(TFT_BLACK);
	M5.Lcd.drawCentreString(noteSymbols[index], cx, cy + dy - h / 2 + 4, TONE_FONT);
	if (sharpFlags[index]) {
		M5.Lcd.drawCentreString("#", cx + 22, cy + dy - h / 2 + 6, SHARP_FONT);
	}
}

void DrawTone::hideTone() {
	if (!initialized) return;
	hideTick = prevTick;
}

void DrawTone::_hideTone() {
	int w = DISH_SIZE * 2;
	M5.Lcd.fillRect(cx - w / 2, cy + dy - w / 2, w + 1, w + 1, BG_COLOR);
}

void DrawTone::init() {
	int w = M5.Lcd.width() / trackCount;
	cx = track * w + w / 2;
	cy = M5.Lcd.height() / 2;
}

void DrawTone::update(unsigned long tick) {
	if (hideTick && tick - hideTick >= HIDE_DELAY) {
		hideTick = 0;
		_hideTone();
	}
	prevTick = tick;
}