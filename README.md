# MML-Parser
A tiny MML(Music Macro Language) parser (device-independent) and M5Stack/M5StickC audio player.

https://www.youtube.com/watch?v=N5gfNE_dzz8
```
// Mozart Piano Concerto No.23 Movement 2
const char* music = "T96 KIf+ L8 @0"
	"O5 c.d16ccfa a<b>gr<b4^ 16a16>f4^8<g>d <f4L32e+fgfL8e+rr"
	"O5 d.e=16d16.d32e32f.e16d <b+4.>crr L16c<b=a+b=a+b=>a=gfe+dc L8<b4a+a=rr"
	"O4 g=.a16g=g=4g= g=b>dg=b>d <<f.g+16fa4g f4r"
	"@48 [O4 a4.^8gf e+fg:c4. >c4.^8<bagabe+4.]2 >dc<b^ 8>c<a4Q4bgQ8f4rr4r";
```

## MML Syntax
|Symbol|Function|Description|
|---|---|---|
|A - G|note on|C(C4) D4.(D4+8) C12C12C12(triplets)|
|+ or #|sharp||
|-|flat||
|=|natural||
|R|rest|R1 (whole rest)|
|O|octave|O0 ... O8 (O4 default)|
|>  <|octave up/down||
|L|length|L4 default|
|Q|note off ratio| n/8 (Q8 default)|
|^|tie|C4^16|
|&|no note off||
|T|tempo| T120 default|
|V|volume| V0 ... V127(max) V+10 V-10|
|KJ KI|transpose(ma**J**or /m**I**nor)|KJg(G major)|
|[ ]|loop| [...]4 (repeat 4 times)|
|:|skip on the last loop||
|@|program change||
|/\*...\*/|comment||

## Audio Player (M5Stack/M5StickC+SPK HAT)
- 8bit wave data (Java converter .wav -> .h)
- Tone sound for MML play.
- Max 15 channel mixer. (using [esp_timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html))

## Dependent libraries (M5Stack)
- [M5Stack-SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater)