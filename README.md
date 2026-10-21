Daisy Seed Generative Ambient Synth
Generative ambient synthesizer built on the Daisy Seed microcontroller using
DaisyDuino. Randomly walks through a quantized scale, filtered and degraded
into texture. No input needed.

Demo/Article
https://ritchieschool.du.edu/news/ritchie-school-students-interdisciplinary-path-through-engineering-and-music


Signal Chain
Sawtooth oscillator → Moog ladder filter → Sample rate reducer → Reverb

Controls
A0 — Amplitude
A1 — Pitch (scales frequency down to 0 Hz)
A2 — Filter cutoff
A3 — Sample rate reduction
A4 — Random pitch change rate
A5 — Reverb feedback
A6 — Mood (selects scale: minor pentatonic in lower half, two-note drone in upper half)

Hardware
- Daisy Seed
- 7 potentiometers on A0–A6
- Breadboard
- Wire


Building
Setup hardware on breadboard
Install the DaisyDuino library in the Arduino IDE
Select Daisy Seed as your board
Upload via microUSB

References
Electrosmith DaisyDuino — github.com/electro-smith/DaisyDuino
