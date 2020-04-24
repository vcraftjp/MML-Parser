//
// MMLParser.cpp
//

#include "MMLParser.h"

#define TIMEBASE 192
#define MAX_OCTAVE 8
#define MAX_GATE 8

static const int8_t noteIndices[] = { 9, 11, 0, 2, 4, 5, 7 }; // A B C D E F G

#define MAX_KEY_TABLE 6

static const int8_t sharpIndices[MAX_KEY_TABLE] = { 5, 0, 7, 2, 9, 4 };
static const int8_t flatIndices[MAX_KEY_TABLE] = { 11, 4, 9, 2, 7, 0 };

int16_t MMLParser::tempo = 120;
MMLParser *MMLParser::pFirstMMLParser = NULL;
MMLParser *MMLParser::pPrevMMLParser = NULL;

MMLParser::MMLParser(int _channel) {
	if (!pFirstMMLParser) {
		pFirstMMLParser = this;
	}
	if (pPrevMMLParser) {
		pPrevMMLParser->pNextMMLParser = this;
	}
	pNextMMLParser = NULL;
	pPrevMMLParser = this;

	channel = _channel;
	pfnCallback = NULL;
	maxVelocity = 127;
	init();
}

void MMLParser::init() {
	_isPlaying = false;
	_isPaused = false;
	p = NULL;
}

void MMLParser::startup() {
	p = pStart;
	startTick = prevTick = 0;
	steps = totalSteps = 0;
	loopNest = 0;
	octave = 4;
	gate = MAX_GATE;
	length = 4;
	prog = 0;
	velocity = 127;
	setKeyTable(0);
}

void MMLParser::play(MMLPTR mml, bool _isLoop) {
	pStart = mml;
	isLoop = _isLoop;
	startup();
//	tempo = 120;
	_isPlaying = true;
}

void MMLParser::noteOn(int num, int velocity) {
	if (pfnCallback) {
		(*pfnCallback)(MML_NOTE_ON, channel, num, velocity);
	}
	prevNum = num;
}

void MMLParser::noteOff() {
	if (pfnCallback) {
		(*pfnCallback)(MML_NOTE_OFF, channel, prevNum, 0);
	}
}

void MMLParser::programChange(int prog) {
	if (pfnCallback) {
		(*pfnCallback)(MML_PROGRAM_CHANGE, channel, prog, 0);
	}
}

void MMLParser::stop() {
	noteOff();
	init();
}

void MMLParser::pause(bool b) {
	_isPaused = b;
	if (b) {
		pauseTick = prevTick;
		noteOff();
	} else if (pauseTick) {
		startTick += prevTick - pauseTick;
	}
}

void MMLParser::stopAll() {
	for (MMLParser *p = pFirstMMLParser; p; p = p->pNextMMLParser) {
		p->stop();
	}
}

void MMLParser::pauseAll(bool b) {
	for (MMLParser *p = pFirstMMLParser; p; p = p->pNextMMLParser) {
		p->pause(b);
	}
}

bool MMLParser::updateAll(unsigned long tick) {
	for (MMLParser *p = pFirstMMLParser; p; p = p->pNextMMLParser) {
		if (!p->update(tick)) return false;
	}
	return true;
}

bool MMLParser::update(unsigned long tick) {
	prevTick = tick;
	if (!p || _isPaused) return true;
	if (startTick == 0) {
		startTick = tick;
	}
	//	return (60 * 1000 / tempo) * steps / 48;
	if (steps) {
		int e = (int)(tick - startTick) * 48 * tempo / (60 * 1000) - totalSteps;
		if (stepsGate) {
			if (e >= stepsGate) {
				noteOff();
				stepsGate = 0;
			}
		}
		if (e >= steps) {
			totalSteps += steps;
			steps = 0;
		}
	}
	if (steps == 0) {
		if (*p == '\0') {
			if (isLoop) {
				startup();
			} else {
				stop();
			}
		} else {
			while (steps == 0 && *p != '\0') {
				if (!parse()) return false;
			}
		}
	}
	return true;
}

bool MMLParser::parse() {
	skipSpace();
	char c = *p++;
	if (c == '\0') {
		p--;
		return true;
	}

	if (c >= 'a' && c <= 'z') {
		c -= 'a' - 'A';
	}
	if (c >= 'A' && c <= 'G') {
		return parseTone(c);
	} else {
		switch (c) {
			case 'R':
				return parseRest();
			case 'O':
				return (octave = parseValue(octave)) != -1;
			case 'L':
				return (length = parseValue()) != -1;
			case 'Q':  // note off rate (n/8)
				return (gate = parseValue()) != -1;
			case 'T':
				return (tempo = parseValue(tempo)) != -1;
			case 'V':
				return (velocity = parseValue(velocity)) != -1;
			case 'K': {
				char c = *p;
				if (c == 'J' || c == 'I') {
					p++;
					return parseKeyChange(c);
				}
				return false;
			}
			case '<':
				octave--;
				return true;
			case '>':
				octave++;
				return true;
			case '[':
				return loopStart();
			case ']':
				return loopEnd();
			case ':':
				return loopSkip();
			case '&':
				return true;
			case '@':
				if ((prog = parseValue()) != -1) {
					noteOff();
					programChange(prog);
					return true;
				}
				break;
			case '/':
				return skipComment();
		}
	}
	return false;
}

bool MMLParser::parseTone(char c) {
	int noteIndex = noteIndices[c - 'A'];
	int o = octave;
	bool noTranspose = false;
	noteIndex = parseNote(noteIndex, &o, &noTranspose);
	if (*p == '=') { // natural
		p++;
	} else if (!noTranspose) {
		noteIndex += keyTable[noteIndex];
	}
	steps = parseSteps();
	if (*p == '&') {
		p++;
		stepsGate = 0;
	} else {
		stepsGate = steps * gate / MAX_GATE;
	}
	int num = (o + 1) * OCTAVE_STEPS + noteIndex;
	int v = velocity;
	if (v < 0) {
		v = 0;
	} else if (v > maxVelocity) {
		v = maxVelocity;
	}
	noteOn(num, v);
	return true;
}

int MMLParser::parseNote(int noteIndex, int* pOctave, bool* pNoTranspose) {
	char c = *p;
	if (c == '+' || c == '#') {
		p++;
		if (pNoTranspose) {
			*pNoTranspose = true;
		}
		if (++noteIndex >= OCTAVE_STEPS) {
			noteIndex -= OCTAVE_STEPS;
			if (pOctave) {
				if (++(*pOctave) > MAX_OCTAVE) {
					*pOctave = MAX_OCTAVE;
				}
			}
		}
	}
	else if (c == '-') {
		p++;
		if (pNoTranspose) {
			*pNoTranspose = true;
		}
		if (--noteIndex < 0) {
			noteIndex += OCTAVE_STEPS;
			if (pOctave) {
				if (--(*pOctave) < 0) {
					*pOctave = 0;
				}
			}
		}
	}
	return noteIndex;
}

bool MMLParser::parseRest() {
	steps = parseSteps();
	stepsGate = 0;
	noteOff();
	return true;
}

int MMLParser::parseValue() {
	int value = -1;
	for (;;) {
		char c = *p;
		if (c < '0' || c > '9') break;
		if (value < 0) {
			value = 0;
		}
		value = value * 10 + (c - '0');
		p++;
	}
	return value;
}

int MMLParser::parseValue(int defaultValue) {
	int d = 0;
	while (*p == '+' || *p == '-') {
		if (*p == '+') {
			d++;
		} else {
			d--;
		}
		p++;
	}
	int value = parseValue();
	if (value == -1) {
		return defaultValue + d;
	}
	if (d > 0) {
		return defaultValue + value;
	}
	if (d < 0) {
		return defaultValue - value;
	}
	return value;
}

int MMLParser::parseSteps() {
	int steps = 0;
	for (;;) {
		int value = parseValue();
		int len = (value == -1) ? length : value;
		steps += TIMEBASE / len;
		while (*p == '.') {
			p++;
			steps += TIMEBASE / (len <<= 1);
		}
		if (*p == '^') { // tie
			p++;
			skipSpace();
			continue;
		}
		break;
	}
	return steps;
}

bool MMLParser::parseKeyChange(char c) {
	bool major = (c == 'J');
	c = *p++;
	if (c >= 'a' && c <= 'g') {
		c -= 'a' - 'A';
	}
	if (c < 'A' || c > 'G') return false;
	int noteIndex = parseNote(noteIndices[c - 'A']);
	setKeyTable(noteIndex, major);
	return true;
}

bool MMLParser::loopStart() {
	if (loopNest >= MAX_LOOP_NEST) return false;
	MMLLoop& mmlLoop = mmlLoops[loopNest];
	mmlLoop.index = p - pStart;
	mmlLoop.count = -1;
	loopNest++;
	return true;
}

bool MMLParser::loopEnd() {
	if (loopNest == 0) return false;
	int value = parseValue();
	MMLLoop& mmlLoop = mmlLoops[loopNest - 1];
	if (mmlLoop.count == -1) {
		mmlLoop.count = (value == -1) ? 2 : value;
	}
	if (--mmlLoop.count > 0) {
		p = pStart + mmlLoop.index;
	} else {
		loopNest--;
	}
	return true;
}

bool MMLParser::loopSkip() {
	if (loopNest == 0) return false;
	MMLLoop& mmlLoop = mmlLoops[loopNest - 1];
	if (mmlLoop.count == 1) {
		char c;
		while ((c = *p)) {
			if (c == ']') {
				break;
			}
			p++;
		}
	}
	return true;
}

void MMLParser::setKeyTable(int note, bool major) {
	for (int i = 0; i < OCTAVE_STEPS; i++) {
		keyTable[i] = 0;
	}
	int n = major ? 0 : 9;
	int s = -1; // circle of 5th (clockwise)
	for (; s < MAX_KEY_TABLE; s++) {
		if (n == note) break;
		if ((n += 7) >= OCTAVE_STEPS) {
			n -= OCTAVE_STEPS;
		}
	}
	if (s == -1) return;
	n = major ? 0 : 9;
	int f = -1; // circle of 5th (anticlockwise)
	for (; f < MAX_KEY_TABLE; f++) {
		if (n == note) break;
		if ((n -= 7) < 0) {
			n += OCTAVE_STEPS;
		}
	}
	if (s <= f) {
		for (int i = 0; i <= s; i++) {
			keyTable[sharpIndices[i]] = 1;
		}
	} else {
		for (int i = 0; i <= f; i++) {
			keyTable[flatIndices[i]] = -1;
		}
	}
}

void MMLParser::skipSpace() {
	for (;;) {
		char c = *p;
		if ((c > '\0' && c <= ' ') || c == '|') {
			p++;
			continue;
		}
		break;
	}
}

bool MMLParser::skipComment() {
	if (*p != '*') return false;
	char c;
	while ((c = *p)) {
		p++;
		if (c == '*' && *p == '/') {
			p++;
			break;
		}
	}
	return true;
}