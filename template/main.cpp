#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

#define NUM_ADC_CHANNELS (3)
enum AdcChannel {
    P_RIGHT = 0,
    P_CENTRE,
    P_LEFT
};

typedef enum _SwitchState {
    S_RIGHT,
    S_CENTRE,
    S_LEFT,
} SwitchState_t;

DaisySeed hw;

SwitchState_t tri_switch = S_LEFT;

/******************
 * Audio Callback
 ******************/
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        float output = 0;

        // TODO process output based on switch state?
        switch (tri_switch) {
            case S_LEFT: break;
            case S_CENTRE: break;
            case S_RIGHT: break;
        }

        out[0][i] = output;
        out[1][i] = output;
    }
}


/* Debug Print Statement */
inline void debug(float left, float centre, float right) {
    hw.PrintLine("Left [" FLT_FMT3 "], Centre [" FLT_FMT3
                 "], Right [" FLT_FMT3 "], switch [%d]",
                 FLT_VAR3(left),
                 FLT_VAR3(centre),
                 FLT_VAR3(right),
                 tri_switch);
}

inline SwitchState_t read_tri_switch(bool left, bool right) {
    if (left && !right) {
        return S_LEFT;
    }
    if (right && !left) {
        return S_RIGHT;
    }
    return S_CENTRE;
}

int main(void)
{
    /* Setup */
    hw.Init();
    hw.StartLog();

    /* Setup Pins */
    AdcChannelConfig adc_config[NUM_ADC_CHANNELS];
    adc_config[P_RIGHT].InitSingle(seed::A0);
    adc_config[P_CENTRE].InitSingle(seed::A1);
    adc_config[P_LEFT].InitSingle(seed::A2);
    hw.adc.Init(&adc_config[0], NUM_ADC_CHANNELS);

    GPIO switch_left;
    GPIO switch_right;
    switch_left.Init(seed::D18, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    switch_right.Init(seed::D28, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    /* Setup Audio */
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    float sampleRate = hw.AudioSampleRate();

    /* TODO Init any objects */

    hw.adc.Start();
    hw.StartAudio(AudioCallback);

    while(1)
    {
        tri_switch = read_tri_switch(switch_left.Read(), switch_right.Read());
        float left = hw.adc.GetFloat(P_LEFT);
        float centre  = hw.adc.GetFloat(P_CENTRE);
        float right = hw.adc.GetFloat(P_RIGHT);

        hw.SetLed(S_RIGHT == tri_switch);

        /* TODO Read any objects */

        daisy::System::Delay(4);
    }
}
