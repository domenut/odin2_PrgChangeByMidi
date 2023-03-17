/*
** Odin 2 Synthesizer Plugin
** Copyright (C) 2020 - 2021 TheWaveWarden
**
** Odin 2 is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Odin 2 is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#include "AudioInput.h"
#include "stdlib.h"


AudioInput::AudioInput() {
    m_lowpass.m_freq_base = FILTER_FC_MAX;
    m_lowpass.setLP();

    m_highpass.m_freq_base = FILTER_FC_MIN;
    m_highpass.setHP();

    m_lowpass.reset();
    m_highpass.reset();
}

AudioInput::~AudioInput() {
}

float AudioInput::doAudioInput(float audio_sample) {

    m_lowpass.update();
    m_highpass.update();

//    float audio = (float)rand();
//    audio       = 2 * (audio / (float)RAND_MAX ) - 1;

	// do 2nd order like this?
    audio_sample = m_lowpass.doFilter(audio_sample);
    audio_sample = m_highpass.doFilter(audio_sample);

    return audio_sample;
}

void AudioInput::setFilterFreqs(float p_lowpass_freq, float p_highpass_freq) {
	m_lowpass.m_freq_base  = p_lowpass_freq;
	m_highpass.m_freq_base = p_highpass_freq;
}

void AudioInput::setHPFreq(float p_freq) {
	m_highpass.m_freq_base = p_freq;
}

void AudioInput::setLPFreq(float p_freq) {
    DBG("AudioIn..setLPFreq: " + String(p_freq) );
	m_lowpass.m_freq_base = p_freq;
}
