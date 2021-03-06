//
// MMLTest.cpp
//

#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

#include "MMLParser.h"

#define MULTI_TRACK

#ifdef MULTI_TRACK
#include "music_the_bear.h"
// #include "music_cordarn.h"

MMLParser* pMMLs[TRACK_COUNT];

#else // !MULTI_TRACK
MMLParser mml;

// Mozart Piano Concerto No.23 Movement 2
const char* music = "T96 KIf+ L8 @0"
    "O5 c.d16ccfa a<b>gr<b4^ 16a16>f4^8<g>d <f4L32e+fgfL8e+rr"
    "O5 d.e=16d16.d32e32f.e16d <b+4.>crr L16c<b=a+b=a+b=>a=gfe+dc L8<b4a+a=rr"
    "O4 g=.a16g=g=4g= g=b>dg=b>d <<f.g+16fa4g f4r"
    "@48 [O4 a4.^8gf e+fg:c4. >c4.^8<bagabe+4.]2 >dc<b^ 8>c<a4Q4bgQ8f4rr4r";

#endif // MULTI_TRACK

#define MIDIMSG(status, ch, data1, data2) ((DWORD)((status << 4) | ch | (data1 << 8) | (data2 << 16)) )

HMIDIOUT h;

void mmlCallback(MMLEvent event, int ch, int num, int velocity) {
    switch (event) {
    case MML_NOTE_ON:
        midiOutShortMsg(h, MIDIMSG(0x9, ch, num, velocity));
        printf("%d ", num);
        // printf("%d (%d)\r\n", num, mml.getTotalSteps());
        break;
    case MML_NOTE_OFF:
        midiOutShortMsg(h, MIDIMSG(0x8, ch, num, 0));
        printf("- ");
        break;
    case MML_PROGRAM_CHANGE:
        midiOutShortMsg(h, MIDIMSG(0xC, ch, num, 0));
        break;
    }
}

int main() {

    midiOutOpen(&h, MIDI_MAPPER, 0, 0, 0);

#ifndef MULTI_TRACK
    midiOutShortMsg(h, MIDIMSG(0xC, 0, 26, 0)); // #26 Electric Guitar (jazz)

    mml.setCallback(mmlCallback);
    mml.play(music, true);

    while (mml.isPlaying()) {
        if (!mml.update(::GetTickCount())) {
            puts("Error\r\n");
            return 1;
        }
        ::Sleep(10);
    }

#else // MULTI_TRACK
    for (int i = 0; i < TRACK_COUNT; i++) {
        pMMLs[i] = new MMLParser((i == DRUM_TRACK) ? 9 : i);
        pMMLs[i]->setCallback(mmlCallback);
        pMMLs[i]->play(tracks[i], true);
    }

    for (;;) {
        bool isPlaying = false;
        DWORD tick = ::GetTickCount();
        for (int i = 0; i < TRACK_COUNT; i++) {
            if (pMMLs[i]->isPlaying()) {
                isPlaying = true;
                if (!pMMLs[i]->update(tick)) {
                    puts("Error\r\n");
                    return 1;
                }
            }
            else {
                printf("%d(%d) end\r\n", i, pMMLs[i]->getTotalSteps());
            }
        }
        if (!isPlaying) break;
        ::Sleep(10);
    }
#endif // MULTI_TRACK
    puts("\r\ndone\r\n");

    midiOutReset(h);
    midiOutClose(h);

    return 0;
}

