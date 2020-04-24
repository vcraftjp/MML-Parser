#include "M5.h"

#ifndef ARDUINO_M5Stick_C
#include <M5StackUpdater.h>
#endif

#include "Audio.h"

#ifndef ARDUINO_M5Stick_C
int volume = MIN_VOLUME;
#else
int volume = 8;
#endif

// #define CHANNEL_TEST

#ifndef CHANNEL_TEST

//
// Play music
//
#include "MMLParser.h"
#include "DrawTone.h"
#include "music_the_bear.h"
//#include "music_cordarn.h"
#include "sounds/sounds.h"

MMLParser mml[TRACK_COUNT];
Audio audio[TRACK_COUNT];
Audio audioFX;
DrawTone drawTone[TRACK_COUNT];

void mmlCallback(MMLEvent event, int ch, int num, int velocity) {
	switch (event) {
	case MML_NOTE_ON:
		audio[ch].playTone(num);
		drawTone[ch].showTone(num);
		break;
	case MML_NOTE_OFF:
		audio[ch].playTone(0);
		drawTone[ch].hideTone();
		break;
	case MML_PROGRAM_CHANGE:
		break;
	}
}

void drawPauseMessage() {
	M5.Lcd.fillScreen(TFT_BLACK);
	M5.Lcd.setTextColor(TFT_GREEN);
#ifndef ARDUINO_M5Stick_C
	M5.Lcd.setTextSize(2);
#endif
	M5.Lcd.drawCentreString("Press Btn-A to Play", M5.Lcd.width() / 2, M5.Lcd.height() / 2 - M5.Lcd.fontHeight(2) / 2, 1);
}

void setup() {
	M5.begin();

#ifdef ARDUINO_M5Stick_C
	M5.Lcd.setRotation(3);
#else
	if (digitalRead(BUTTON_A_PIN) == 0) {
		Serial.println("Will Load menu binary");
		updateFromFS(SD);
		ESP.restart();
	}
#endif
	audioFX.setSampleRate(SAMPLE_RATE);
	audioFX.setWaveData(waveDataArray, waveDataSizeArray);
	audioFX.setVolume(volume);

	for (int i = 0; i < TRACK_COUNT; i++) {
		audio[i].setVolume(volume);

		mml[i].setCallback(mmlCallback);
		mml[i].setChannel(i);
		mml[i].play(tracks[i], true);
		mml[i].pause(true);
	}
	drawPauseMessage();
}

void loop() {
	M5.update();

	if (M5.BtnA.wasPressed()) { // toggle pause
		bool isPaused = !mml[0].isPaused();
		MMLParser::pauseAll(isPaused);
		if (isPaused) {
			drawPauseMessage();
		} else {
			M5.Lcd.fillScreen(BG_COLOR);
		}
	}

#ifndef ARDUINO_M5Stick_C
	if (M5.BtnB.wasPressed()) {
		audioFX.play(SND_DOG);
	}

	if (M5.BtnC.wasPressed()) {
		audioFX.play(SND_COW);
	}
#endif
/*
	if (M5.BtnB.wasPressed()) { // volume down
		if (volume > MIN_VOLUME) {
			Audio::setVolumeAll(--volume);
		}
	}

	if (M5.BtnC.wasPressed()) { // volume up
		if (volume < MAX_VOLUME) {
			Audio::setVolumeAll(++volume);
		}
	}
*/
	unsigned long tick = millis();
	for (int i = 0; i < TRACK_COUNT; i++) {
		if (mml[i].isPlaying()) {
			if (!mml[i].update(tick)) {
				M5.Lcd.print("Error");
				break;
			}
			if (!mml[i].isPaused()) {
				drawTone[i].update(tick);
			}
		}
	}
	delay(10);
}

#else // CHANNEL_TEST

//
// Multi channel Test
//
#define CHANNEL_COUNT 15 // less than MAX_CHANNEL (Audio.h)

#define START_TONE 60 // C4

Audio audio[CHANNEL_COUNT];

int channelIndex = -1;
int tone;
unsigned long tick;

void setup() {
	M5.begin();

#ifndef ARDUINO_M5Stick_C
	M5.Lcd.setTextSize(2);
#else
	M5.Lcd.setRotation(3);
#endif

	Audio::setVolumeAll(volume);
	M5.Lcd.println("Channel test");
}

void loop() {
	M5.update();

	if (M5.BtnA.wasPressed()) {
		if (channelIndex == -1) {
			channelIndex = 0;
			tone = START_TONE;
			tick = 0;
		} else {
			for (int i = 0; i < channelIndex; i++) {
				audio[i].stop();
			}
			channelIndex = -1;
			M5.Lcd.println("");
		}
	}

	if (channelIndex >= 0 && channelIndex < CHANNEL_COUNT) {
		if (tick == 0 || millis() - tick >= 500) {
			tick = millis();
			audio[channelIndex].playTone(tone);
			int m = tone % 12;
			if (m == 0) {
				tone += 4; // E
			} else if (m == 4) {
				tone += 3; // G
			} else {
				tone += 5; // C
			}
			M5.Lcd.printf("%d ", channelIndex);
			channelIndex++;
		}
	}

	delay(20);
}

#endif // CHANNEL_TEST
