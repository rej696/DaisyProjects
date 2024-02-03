#include "daisy_seed.h"
#include "daisysp.h"
#include "flt.h"

using namespace daisy;
using namespace daisysp;

enum AdcChannel {
    POT_BOT_LEFT = 0,
    POT_TOP_FAR_LEFT,
    POT_TOP_MID_LEFT,
    POT_TOP_MID_RIGHT,
    POT_TOP_FAR_RIGHT,
    POT_BOT_RIGHT,
    SLIDER_LEFT,
    SLIDER_RIGHT,

    ADC_CHANNEL_COUNT
};

static const daisy::Pin pinmap[ADC_CHANNEL_COUNT] {
    [POT_BOT_LEFT] = seed::A0,
    [POT_TOP_FAR_LEFT] = seed::A1,
    [POT_TOP_MID_LEFT] = seed::A2,
    [POT_TOP_MID_RIGHT] = seed::A3,
    [POT_TOP_FAR_RIGHT] = seed::A4,
    [POT_BOT_RIGHT] = seed::A5,
    [SLIDER_LEFT] = seed::A6,
    [SLIDER_RIGHT] = seed::A7
};

DaisySeed hw;

int const oscCount = 6;
constexpr float volume = 3.0 / oscCount;
Oscillator oscs[oscCount];

Filter flt;

/******************
 * Audio Callback
 ******************/
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        float output = 0;

        for(Oscillator &o : oscs)
        {
            output += o.Process();
        }
        output = flt.Process(output) * volume;

        out[0][i] = output;
        out[1][i] = output;
    }
}


void init_adcs() {
    /* Setup Pins */
    AdcChannelConfig adc_config[ADC_CHANNEL_COUNT];
    for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        adc_config[i].InitSingle(pinmap[i]);
    }
    hw.adc.Init(&adc_config[0], ADC_CHANNEL_COUNT);
}

float init_audio() {
    /* Setup Audio */
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    return hw.AudioSampleRate();
}

void read_adc_values(float values[ADC_CHANNEL_COUNT]) {
    for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        values[i] = hw.adc.GetFloat(i);
    }
}

int main(void)
{
    float adc_values[ADC_CHANNEL_COUNT] = {0.0};

    /* Setup */
    hw.Init();
    hw.StartLog();

    init_adcs();
    float sample_rate = init_audio();

    /* TODO Init any objects */
    for (Oscillator &o: oscs) {
        o.Init(sample_rate);
        o.SetWaveform(Oscillator::WAVE_SAW);
    }

    flt.Init(sample_rate);

    hw.adc.Start();
    hw.StartAudio(AudioCallback);

    while(1)
    {
        read_adc_values(adc_values);

        for (int i = 0; i <= POT_BOT_RIGHT; i++) {
            hw.PrintLine("ADC %d: %f", i, adc_values[i]);
            oscs[i].SetFreq(fmap(adc_values[i], 0, 500));
        }

        flt.SetFreq(adc_values[SLIDER_LEFT]);
        flt.SetRes(adc_values[SLIDER_RIGHT]);

        daisy::System::Delay(4);
    }
}
