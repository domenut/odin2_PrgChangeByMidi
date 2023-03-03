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

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OdinMidiLearnBase.h"
#include "../GlobalIncludes.h"

class OdinAudioProcessor;

class OdinButton : public DrawableButton, public OdinMidiLearnBase,
                                          public Timer
{
public:
  OdinButton(const String &buttonName, ButtonStyle buttonStyle)
      : DrawableButton(buttonName, buttonStyle) {
      // CMDEBUG>
      midiLearnIndicateTimerInit();
      // CMDEBUG<
  }

  ~OdinButton() {
      Timer::stopTimer();
  }

  void paint(juce::Graphics &g) override {
    SET_INTERPOLATION_QUALITY(g)

    DrawableButton::paint(g);
    if (m_midi_learn) {
      g.setColour(Colours::red);
      g.drawRoundedRectangle(getLocalBounds().getX(), getLocalBounds().getY(),
                             getLocalBounds().getWidth(),
                             getLocalBounds().getHeight(), 5,
                             2); // draw an outline around the component
    } else if (m_midi_control && ! m_midi_loaded_patch_control) {
      g.setColour(Colours::green);
      g.drawRoundedRectangle(getLocalBounds().getX(), getLocalBounds().getY(),
                             getLocalBounds().getWidth(),
                             getLocalBounds().getHeight(), 5,
                             2); // draw an outline around the component
    } else if (m_midi_loaded_patch_control) {
        g.setColour(Colours::blue);
        g.drawRoundedRectangle(getLocalBounds().getX(), getLocalBounds().getY(),
                               getLocalBounds().getWidth(),
                               getLocalBounds().getHeight(), 5,
                               2); // draw an outline around the component
    }
  }

  void mouseDown(const MouseEvent &event) override;
  static void setOdinPointer(OdinAudioProcessor *p_pointer) {
    m_processor = p_pointer;
  }

  void stopMidiLearn() override {
    m_midi_learn = false;
    m_midi_control = false;
    repaint();
  }

  void setMidiControlActive() override {
    m_midi_learn = false;
    m_midi_control = true;
    repaint();
  }

private:

  // CMDEBUG>
  void midiLearnIndicateTimerInit();
  void timerCallback() override ;
  void midiLearnIndicator(bool set);
  // CMDEBUG<

  static OdinAudioProcessor *m_processor;
};
