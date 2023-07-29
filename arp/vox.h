#ifndef ARP_VOX_H_
#define ARP_VOX_H_
#include "daisysp.h"
#include "daisy_seed.h"
#include <cstdlib.h>

class Vox
{
  public:
    void Init(float sample_rate)
    {
        // Oscillator Setup
        this->_osc.Init(sample_rate);
        this->_osc.SetWaveform(daisysp::Oscillator::WAVE_SAW);

        // Lfo Setup
        this->_lfo.Init(sample_rate);
        this->_lfo.SetFreq(this->randFloat() / 10.0);
        this->_lfo.SetWaveform(daisysp::Oscillator::WAVE_SIN);
        this->_lfo.SetAmp(0.005);

        this->_env.Init(sample_rate);
        this->_env.SetTime(daisysp::ADSR_SEG_ATTACK, 0.02);
        this->_env.SetTime(daisysp::ADSR_SEG_RELEASE, 0.02);
    }

    void NoteOn(float freq, float amp)
    {
        this->_osc_freq = freq;
        this->_osc_amp  = amp;
        this->openGate();
    }

    void NoteOff() { this->closeGate(); }

    float Process()
    {
        float env_amp = this->_env.Process(this->_gate);
        if(!this->_env.IsRunning())
        {
            return 0;
        }

        this->_osc.SetFreq(this->_osc_freq * (1.0f + this->_lfo.Process()));
        this->_osc.SetAmp(env_amp * this->_osc_amp);
        return this->_osc.Process();
    }

  private:
    float randFloat()
    {
        int range = this->_randMax - this->_randMin + 1;
        return static_cast<float>(std::rand() % range + this->_randMin);
    }

    void openGate() { this->_gate = true; }

    void closeGate() { this->_gate = false; }

    const int _randMin = 50;
    const int _randMax = 150;

    daisysp::Oscillator _osc;
    daisysp::Oscillator _lfo;
    daisysp::Adsr       _env;
    float               _osc_freq;
    float               _osc_amp;
    bool                _gate = false;
};

#endif /* ARP_VOX_H_ */
