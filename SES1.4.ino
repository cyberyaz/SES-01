#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;
static SampleRateReducer sampleRate;
static Oscillator osc, lfo;
static MoogLadder flt;
static ReverbSc reverb;
static SmoothRandomGenerator smoothPitch, smoothRate, smoothCut;

float randRate;  // Smooth random rate from knob
float amp, pitchIn;  // Pitch input information from knob
float cutoff, cutIn; // Filter cutoff point, and cutoff input information from knob
float samprate, samprateIn;
float verbLevel, verbFeedback;  // Reverb level and feedback from corresponding knobs
float moodIn; // Mood knob input

// Define the minor pentatonic scale (e.g., A minor pentatonic)

void moodScale(float moodKnob, float *scale, int *numNotes) {
    if (moodKnob >= 1 && moodKnob <= 4) {
        scale[0] = 220.0f;
        scale[1] = 261.6f;
        scale[2] = 293.6f;
        scale[3] = 329.6f;
        scale[4] = 392.0f;
        *numNotes = 5; // Set the number of notes directly
    } else {
        scale[0] = 220.0f;
        scale[1] = 440.0f;
        *numNotes = 2; // Set the number of notes directly
    }
}

void setup() {
    Serial.begin(9600);  // Sets incoming bits

    // Initialize Daisy
    hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    num_channels = hw.num_channels;
    float sample_rate = DAISY.get_samplerate();

    // INITIALIZE AUDIO COMPONENTS ///////////////////////////////////////////////////////////////////////////////////////
    osc.Init(sample_rate);
    osc.SetWaveform(osc.WAVE_SAW);

    flt.Init(sample_rate);
    flt.SetRes(0.9);

    sampleRate.Init();

    reverb.Init(sample_rate);
    reverb.SetFeedback(0.85f);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // INITIALIZE MODULATION COMPONENTS ///////////////////////////////////////////////////////////////////////////////////
    smoothPitch.Init(sample_rate);
    smoothRate.Init(sample_rate);
    smoothRate.SetFreq(0.2f);
    smoothCut.Init(sample_rate);
    smoothCut.SetFreq(2.0f);

    lfo.Init(sample_rate);
    lfo.SetFreq(0.005);
    lfo.SetAmp(0.5);
    lfo.SetWaveform(osc.WAVE_SIN);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    DAISY.begin(MyCallback);
}

void MyCallback(float **in, float **out, size_t size) {
    osc.SetAmp(amp);
    reverb.SetFeedback(verbFeedback);
    float scale[5]; // Frequencies for A, C, D, E, G
    int numNotes;
    moodScale(moodIn, scale, &numNotes);

    for (size_t i = 0; i < size; i++) {

        // PITCH MODULATION AND QUANTIZER //////////////////////////////////////////////////////////////////////////////////
        // Update rate from the Rate knob, minimum at 0.005 Hz
        smoothPitch.SetFreq(0.005 + 9.995 * randRate);  // Range: 0.005 to 10.0 Hz

        // Modulate frequency w/ random
        float modulation = smoothPitch.Process();
        int index = static_cast<int>((modulation * numNotes)) % numNotes;
        if (index < 0){
            index += numNotes;  // Ensure positive index
        }

        // Uses knob info to transpose randomized freq
        float basePitch = scale[index];
        int transposeIndex = static_cast<int>((pitchIn * numNotes)) % numNotes;
        float transposedPitch = basePitch * pow(2.0, transposeIndex / 12.0);

        // Apply pitchIn modulation to bring pitch down to 0 Hz
        float finalPitch = transposedPitch * pitchIn;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        // FILTER AND SAMPLERATE MODULATION ///////////////////////////////////////////////////////////////////////////////////
        cutoff = (cutIn + fabsf(smoothCut.Process()) / 2) * 5000;
        samprate = (samprateIn + fabsf(smoothRate.Process())) / 8;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        // APPLICATION OF MODULATION //////////////////////////////////////////////////////////////////////////////////////////
        osc.SetFreq(finalPitch);
        flt.SetFreq(cutoff);
        sampleRate.SetFreq(samprate);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




        // AUDIO SIGNAL CHAIN /////////////////////////////////////////////////////////////////////////////////////////////////
        float drySig = sampleRate.Process(flt.Process(osc.Process()));
        float wetSig;
        reverb.Process(drySig, &wetSig);
        for (size_t chn = 0; chn < num_channels; chn++) {
            out[chn][i] = wetSig;
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
    }
}

void loop() {
    analogReadResolution(16);
    pitchIn = analogRead(A1) / 65535.0;  // Controls pitch down to 0 Hz
    amp = analogRead(A0) / 65535.0;
    cutIn = analogRead(A2) / 6553500.0;
    samprateIn = analogRead(A3) / 65535.0;
    randRate = analogRead(A4) / 65535.0;  // Controls rate of pitch change
    verbLevel = analogRead(A5) / 65535.0;
    verbFeedback = 0.2 + (analogRead(A5) / 65535.0) * 0.75;
    moodIn = 1 + (analogRead(A6) / 65535.0) * 7;
}

