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

#include "DrawableSlider.h"
#include "../GlobalIncludes.h"
#include "../PluginProcessor.h"

OdinAudioProcessor *DrawableSlider::m_processor;

DrawableSlider::DrawableSlider() {
	setLookAndFeel(&m_feels);

	setPopupDisplayEnabled(true, false, nullptr);
	setNumDecimalPlacesToDisplay(3);

	m_handle = ImageCache::getFromMemory(BinaryData::slider_handle_png, BinaryData::slider_handle_pngSize);

	setVelocityModeParameters(1.0, 1, 0.0, true, ModifierKeys::shiftModifier);

	setTextBoxStyle(NoTextBox, true, 0, 0);

    midiLearnIndicateTimerInit();
}

DrawableSlider::~DrawableSlider() {
	setLookAndFeel(nullptr);
    Timer::stopTimer();
}

void DrawableSlider::paint(Graphics &g) {
	SET_INTERPOLATION_QUALITY(g)
	//g.setColour(Colours::grey);
	//g.drawRect(getLocalBounds(), 1); // draw an outline around the component

	//DBG(getValue());
	g.drawImageAt(m_handle, 0, (1.f - valueToProportionOfLength(getValue())) * (getHeight() - m_handle.getHeight()));

	//DBG(getValue());
	if (m_midi_learn) {
		g.setColour(Colours::red);
		g.drawRoundedRectangle(getLocalBounds().getX(),
		                       getLocalBounds().getY(),
		                       getLocalBounds().getWidth(),
		                       getLocalBounds().getHeight(),
		                       5,
		                       2); // draw an outline around the component
    } else if (m_midi_control && ! m_midi_loaded_patch_control) {
		g.setColour(Colours::green);
		g.drawRoundedRectangle(getLocalBounds().getX(),
		                       getLocalBounds().getY(),
		                       getLocalBounds().getWidth(),
		                       getLocalBounds().getHeight(),
		                       5,
                               2); // draw an outline around the component
    } else if (m_midi_loaded_patch_control) {
        g.setColour(Colours::blue);
        g.drawRoundedRectangle(getLocalBounds().getX(),
                               getLocalBounds().getY(),
                               getLocalBounds().getWidth(),
                               getLocalBounds().getHeight(),
                               5,
                               2); // draw an outline around the component
	}
}



void DrawableSlider::mouseDown(const MouseEvent &event) {
	if (event.mods.isRightButtonDown() && m_midi_learn_possible) {
		//DBG("RIGHT");
		PopupMenu midi_learn_menu;
		if (m_midi_learn) {
			midi_learn_menu.addItem(2, "Stop MIDI learn");
			if (midi_learn_menu.show() == 2) {
				stopMidiLearn();
				m_processor->stopMidiLearn();
			}
		} else {
			midi_learn_menu.addItem(2, "MIDI learn");
			if (m_midi_control) {
				midi_learn_menu.addItem(3, "MIDI forget");
			}
			int menu = midi_learn_menu.show();
			if (menu == 2) {
				if (m_midi_control) {
					m_processor->midiForget(m_parameter_ID, this);
				}
				m_processor->startMidiLearn(m_parameter_ID, this);
				m_midi_learn   = true;
				m_midi_control = false;
				repaint();
			} else if (menu == 3) {
				m_processor->midiForget(m_parameter_ID, this);
				m_midi_control = false;
                m_midi_loaded_patch_control = false;
                m_processor->midi_learned.removeProperty(m_parameter_ID, nullptr);
                repaint();
			}
		}
		return;
	}
	Slider::mouseDown(event);
}

String DrawableSlider::getTextFromValue(double value) {
	//apparently there's a better solution:
	//https://forum.juce.com/t/setnumdecimalplacestodisplay-not-behaving-solved/33686/2
	return String(value, getNumDecimalPlacesToDisplay()) + getTextValueSuffix();
}

void DrawableSlider::midiLearnIndicateTimerInit(){
    if(m_midi_learn_possible){
    Timer::startTimer(2000);
    }
}

// CMDEBUG>
void DrawableSlider::timerCallback(){
    if(m_parameter_ID == ""){
        Timer::stopTimer();
        return;
    }
    if(m_processor->midi_learned.hasProperty(m_parameter_ID)){
        m_midi_loaded_patch_control = true;
        repaint();
    }else{
        m_midi_loaded_patch_control = false;
        if(m_processor->loaded_new_map){
            m_midi_control = false;
        }
        repaint();
    }
}
// CMDEBUG<






