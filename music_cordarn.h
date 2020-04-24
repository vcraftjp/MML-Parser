#pragma once

// Game music "Cordarn" from PC-9801 "ƒfƒBƒKƒ“‚Ì–‚Î/Digan No Maseki" 1988
// Composer: Yo Ooyama (ASTURIAS)

const char* trackA =
	"v80 l16 KJg Q8 t124"
	"@82 /* Hall */ v96 o4e2f2g2>d2<b2g2f2.d8.ef<b8.b8a8b4>d8.ef<b8.b8a8b4>d4c2e4.ded2a2e2f2g2>d2<b2g2f2.>d8.ef<b8.b8a8b4>d8.ef<b8.b8a8b4>d4c2e4.fgf2..o7r8"
	"r1r1r1r2/* oct1 */ @54 /* Hall */ v120 o6Q2g8Q8fd&r4"
	"[[@30 /* Harpsic */ /* oct-1 */ v80 o3a>ea>eae<ae]2]2[o3a>da>dad<ad]2[v80 o3b>fb>fbf<bf]2"
	"@81 /* Clarinette */ /* oct0 */ v96 >Q4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4a8Q8cg4f8efedQ4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4f8Q8cg8.efga8b8"
	"@81 /* Hall */ /* oct0 */ v112 <e8>e>Q4c8<Q8eb4a8gagfQ4g8Q8e>Q4c8<Q8eb4a8gagfQ4g8Q8e>Q4c8<Q8eb4a8gagfQ4g8Q8eQ4a8Q8ef8.c8.<b8a8";

const char* trackB =
	"v80 l16 KJg @34 /* b_dr */ v96"
	"[[o2e4]4]2[[o2d4]4]2[[o1b4]4]2[o2c4]4[o2d4]3v+30ddd8v-30"
	"[[o2e4]4]2[[o2d4]4]2[[o1b4]4]2[o2c4]4[o2d4]4"
	"[[o1a4]4]7a4a4 @36 /* Snaredrum */ v127 aaaaaaaa[[@34 /* b_dr */ v112 o2e4@118 /* Snaredrum */ v127 a4]2]8";

const char* trackC =
	"v80 l16 KJg"
	"[[Q8@37 /* Ebass1 */ v80 o2e8bbeba>e]2]2[[o2d8aadag>d]2]2[[o1b8>ff<b>feb]2]2[o2c8ggc>c<g>c]2[o2d8aad>d<a>d]2[[o2e8bbeba>e]2]2[[o2d8aadag>d]2]2[[o1b8>ff<b>feb]2]2[o2c8ggc>c<g>c]2[o2d8aad>d<a>d]2"
	"@02 /* DeepGrand */ v120 o2a2.&r8b8>c1d1<d1a2.&r8b8>c1d1d+2<b2"
	"@37 o1 /* Ebass1 */ v96 >e8bbeba>e<e8b>e<d>d<a>d<c8ggc>c<g>c<c8g>c<d>d<a>d<e8bbeba>e<e8b>e<d>d<a>d<c8ggc>c<g>c<c8g>c<d>d<a>d<e8bbeba>e<e8b>e<d>d<a>d<c8ggc>c<g>c<c8g>c<d>d<a>d<e8bbeba>e<e8b>e<d>d<a>d<c8ggc>c<g>co7ro3d8.d8d8";

const char* trackD =
	"v80 l16 KJg @48 /* PSG Normal */ "
	"r1r1r1r1 r1r1r1r1"
	"Q8v96 o4g2a2b2>f2d2<b2a2.d8.ef<b8.b8a8b4>d8.ef<b8.b8a8b4>d4v112 c2e4.v127 ded2a4..o7r"
	"@81 [v96 o5g8fg8<b8>c8d8e8f8d]3Q4g8Q8fd2.o7r[o5g8fg8<b8>c8d8e8f8d]2Q4g8Q8fQ8a2o7Q8ro5f4Q4g8Q8fQ8a2o7Q8ro5d+4"
	"@91 /* PSG Down */ v64 o7ro5Q4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4a8Q8cg4f8efedQ4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4f8Q8cg8.efga8b8"
	"@48 /* PSG Normal */ v96 Q4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4a8Q8cg4f8efedQ4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4f8Q8cd8.<a8.g8f16";

const char* trackE =
	"v80 l16 KJg @91 /* PSG Down */ "
	"r1r1r1r1 r1r1r1r1"
	"v64 r8o4g2a4.&r8b2>f4.&r8d2<b4.&r8a8&r2d8.ef<b8.b8a8b4>d8.ef<b8.b8a8b4>d4v96 c2e4.v112 ded2a4..o7r"
	"[/* oct0 */ o7rv96 o5g8fg8<b8>c8d8e8f8]3Q4g8Q8fd2.o7r[o7ro5g8fg8<b8>c8d8e8f8]2Q4g8Q8fQ8a2o7Q8ro5f4Q4g8Q8fQ8a2o7Q8ro5d+4"
	"o7r1r1r1r1 @91 /* PSG Down */ v80 o5e8<b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4a8Q8cg4f8efedQ4e8<Q8b>Q4a8<Q8b>g4f8efedQ4e8Q8cQ4f8Q8cd8.<a8.g8f8";

const char* trackR =
	"l16 KJg Q1"
	"v64 [[o2c4]4]7c4c4c4 v112 edd8"
	"v64 [[o2c4]4]8"
	"v64 [[o2c4]4]7c4c4 v112 eddddddd"
	"v80 [o2c4d4]15 v112 rd8.d4";

#define TRACK_COUNT 6
#define DRUM_TRACK 5

const char* tracks[TRACK_COUNT] = { trackA, trackB, trackC, trackD, trackE, trackR };

