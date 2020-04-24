//
// Audio.h
//
#pragma once

#include "M5.h"

extern "C" {
  #include "esp_timer.h"
}

#define MAX_CHANNEL 16 // only 15 are actually usable

#define MIN_VOLUME 0
#define MAX_VOLUME 11

//
// Audio class
//
class Audio {
public:
	Audio();

	void setVolume(int _volume);
	int getVolume() { return volume; }
	void setSampleRate(int _sampleRate);
	void play(const uint8_t *wave, int length, int duration = 0); // duration(ms), -1=infinite
	void stop();
	bool isPlaying() { return waveIndex != -1; }
	void setMute(bool b);
	bool isMute() { return _isMute; }
	void toggleMute() { setMute(!isMute()); }

	void playTone(int num);
	int getFrequency(int num);

	void setWaveData(const uint8_t **waveDataArray, const uint16_t *waveDataSizeArray, bool normalize = false);
	void play(int index, int duration = 0);

	static void stopAll();
	static void setMuteAll(bool b);
	static void toggleMuteAll();
	static void setVolumeAll(int volume);
	static void setMasterVolume(int masterVolume) {} // TODO

protected:
	Audio *pNextAudio;
	static Audio *pFirstAudio;
	static Audio *pPrevAudio;

	int channel;
	int volume;
	int sampleRate;
	const uint8_t *wave;
	int waveIndex;
	int waveLength;
	int writeBytes;
	int totalBytes;
	bool _isMute;
	const uint8_t **waveDataArray;
	const uint16_t *waveDataSizeArray;
	uint8_t sampleMin;
	uint8_t sampleMax;

	bool isTone;
	bool highlow;

	esp_timer_handle_t hTimer;
	static void timerCallback(void* arg);

	static int channelCount;
	static int masterVolume;
	static uint8_t values[MAX_CHANNEL];

	bool init();
	void startTimer(int microsec);
	void stopTimer();
	void onTimer();
	void writeValue(uint8_t value);
	void audioStop();
};
