#define SAMPLE_RATE 8000

#include "cow.h"
#include "dog.h"

#define SND_COW 0
#define SND_DOG 1

#define SOUND_COUNT 2

const uint8_t *waveDataArray[] = {
	cow,
	dog,
};

const uint16_t waveDataSizeArray[] = {
	sizeof(cow),
	sizeof(dog),
};
