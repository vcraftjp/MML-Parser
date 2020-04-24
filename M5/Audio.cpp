//
// Audio.cpp
//

#include "Audio.h"

#define DEFAULT_SAMPLE_RATE 8000

#define OCTAVE_STEPS 12
#define MAX_OCTAVE 8

Audio *Audio::pFirstAudio = NULL;
Audio *Audio::pPrevAudio = NULL;
int Audio::channelCount = 0;
int Audio::masterVolume = 0;
uint8_t Audio::values[MAX_CHANNEL];

static uint8_t valueTable[MAX_VOLUME + 1] = { 1, 2, 3, 4, 8, 12, 20, 32, 56, 92, 160, 255 };

static const float noteFrequencies[] = {
	4186.009f, 4434.922f, 4698.636f, 4978.032f, 5274.041f, 5587.652f, 5919.911f, 6271.927f, 6644.875f, 7040.000f, 7458.620f, 7902.133f
};

inline void audioWrite(uint8_t b) {
#ifdef ARDUINO_M5Stick_C
	dacWrite(26, b);
#else
	M5.Speaker.write(b);
#endif
}

void Audio::timerCallback(void* arg) {
	((Audio*)arg)->onTimer();
}

void Audio::writeValue(uint8_t value) {
	if (channelCount == 1) {
		audioWrite(value);
	} else {
		values[channel] = value;
		int totalValue = 0;
		for (int i = 0; i < channelCount; i++) {
			totalValue += values[i];
		}
		if (totalValue >= 255) totalValue = 255;
		audioWrite((uint8_t)totalValue);
	}
}

void Audio::onTimer() {
	if (isTone) {
		highlow = !highlow;
//		writeValue(highlow ? (1 << volume) : 0);
		writeValue(highlow ? valueTable[volume] : 0);
	} else if (waveIndex >= 0) {
		uint8_t b = (uint16_t)(pgm_read_byte(wave + waveIndex) - sampleMin) * 255 / (sampleMax - sampleMin) / (MAX_VOLUME + 1 - volume);
		writeValue(b);
		waveIndex++;
		writeBytes++;
		if (waveIndex >= waveLength) {
			if (totalBytes) {
				if (totalBytes == -1) { // infinite loop
					waveIndex = 0;
				} else if (writeBytes < totalBytes) {
					waveIndex = 0;
				} else {
					audioStop();
				}
			} else {
				audioStop();
			}
		}
	}
}

Audio::Audio() {
	if (!pFirstAudio) {
		pFirstAudio = this;
	}
	if (pPrevAudio) {
		pPrevAudio->pNextAudio = this;
	}
	pNextAudio = NULL;
	pPrevAudio = this;

	if (channelCount == 0) {
		for (int i = 0; i < MAX_CHANNEL; i++) {
			values[i] = 0;
		}
	}
	channel = channelCount++;
	hTimer = NULL;
	volume = MIN_VOLUME;
	sampleRate = DEFAULT_SAMPLE_RATE;
	waveIndex = -1;
	_isMute = false;
	waveDataArray = NULL;
	waveDataSizeArray = NULL;
}


void Audio::setVolume(int _volume) {
	volume = min(_volume, MAX_VOLUME);
}

void Audio::setSampleRate(int _sampleRate) {
	if (_sampleRate != sampleRate) {
		sampleRate = _sampleRate;
	}
}

void Audio::stopAll() {
	for (Audio *p = pFirstAudio; p; p = p->pNextAudio) {
		p->stop();
	}
}

void Audio::setMuteAll(bool b) {
	for (Audio *p = pFirstAudio; p; p = p->pNextAudio) {
		p->setMute(b);
	}
}

void Audio::toggleMuteAll() {
	for (Audio *p = pFirstAudio; p; p = p->pNextAudio) {
		p->toggleMute();
	}
}

void Audio::setVolumeAll(int volume) {
	for (Audio *p = pFirstAudio; p; p = p->pNextAudio) {
		p->setVolume(volume);
	}
}

bool Audio::init() {
	if (hTimer) return true;
	if (channel >= 15) return false;

	esp_timer_create_args_t args = {
		.callback = &timerCallback,
		.arg =  reinterpret_cast<void*>(this),
		.dispatch_method = ESP_TIMER_TASK,
		.name = "timer0"
	};
	if (esp_timer_create(&args, &hTimer) != ESP_OK) {
		Serial.println("create timer failed.");
		return false;
	}
	return true;
}

void Audio::startTimer(int microsec) {
	stopTimer();
    esp_timer_start_periodic(hTimer, microsec);
}

void Audio::stopTimer() {
	if (hTimer) {
	    esp_timer_stop(hTimer);
	}
}

void Audio::play(const uint8_t *_wave, int _length, int _duration) {
	if (_isMute) return;
	wave = _wave;
	waveLength = _length;
	waveIndex = 0;
	writeBytes = 0;
	totalBytes = _duration;
	if (_duration > 0) {
		totalBytes = _duration * (sampleRate / 1000);
	}
	init();
	startTimer(1000000 / sampleRate);
}

void Audio::playTone(int num) {
	if (_isMute) return;
	isTone = true;
	if (num == 0) {
		stopTimer();
		writeValue(0);
	} else {
		init();
		int microsec = 1000000 / getFrequency(num) / 2;
		startTimer(microsec);
	}
}

void Audio::stop() {
	audioStop();
}

void Audio::audioStop() {
	waveIndex = -1;
	stopTimer();
	writeValue(0);
}

void Audio::setMute(bool b) {
	_isMute = b;
	if (isPlaying()) {
		stop();
	}
}

void Audio::setWaveData(const uint8_t **_waveDataArray, const uint16_t *_waveDataSizeArray,  bool normalize) {
	waveDataArray = _waveDataArray;
	waveDataSizeArray = _waveDataSizeArray;
	if (normalize) {
		sampleMin = 255;
		sampleMax = 0;
		for (int i = 0; i < 13; i++) {
			const uint8_t *p = waveDataArray[i];
			const uint16_t len = waveDataSizeArray[i];
			for (int j = 0; j < len; j++) {
				uint8_t b = *p++;
				if (b < sampleMin) sampleMin = b;
				if (b > sampleMax) sampleMax = b;
			}
		}
	} else {
		sampleMin = 0;
		sampleMax = 255;
	}
}

void Audio::play(int index, int _duration) {
	play(waveDataArray[index], waveDataSizeArray[index], _duration);
}

int Audio::getFrequency(int num) {
	int noteIndex = num % OCTAVE_STEPS;
	int octave = num / OCTAVE_STEPS - 1;
	return (int)round(noteFrequencies[noteIndex] / (1 << (MAX_OCTAVE - octave)));
}
