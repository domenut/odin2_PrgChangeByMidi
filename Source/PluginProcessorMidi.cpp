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

//this file is included from PluginProcessor.cpp to split the class implementation

// For IDE Only ->:
#include "PluginProcessor.h"
// <- For IDE Only END:

// CMDEBUG>
#define MIDI_CONTROLLER_NUMBER_BANK_SELECT 0
//#define MIDI_CONTROLLER_NUMBER_CATEGORY_SELECT 32
// CMDEBUG<

#define MIDI_CONTROLLER_NUMBER_BREATH 2

bool OdinAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool OdinAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool OdinAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

void OdinAudioProcessor::handleMidiNoteOn(int p_midi_note, int p_midi_velocity) {
	if (m_arpeggiator_on) {
		m_arpeggiator.midiNoteOn(p_midi_note, p_midi_velocity);
	} else {
		midiNoteOn(p_midi_note, p_midi_velocity);
	}
}

void OdinAudioProcessor::handleMidiNoteOff(int p_midi_note) {
	if (m_arpeggiator_on) {
		m_arpeggiator.midiNoteOff(p_midi_note);
	} else {
		midiNoteOff(p_midi_note);
	}
}

void OdinAudioProcessor::midiNoteOn(
    int p_midi_note, int p_midi_velocity, float p_arp_mod_1, float p_arp_mod_2, bool p_add_to_mono_list) {

	if (m_mono_poly_legato != PlayModes::Poly && p_add_to_mono_list) {
		//append note to list, we ignore sustain here for simplicity
		m_playmode_mono_note_list.push_back(std::make_pair(p_midi_note, p_midi_velocity));
		//DBG("List:");
		//for (auto const &i : m_playmode_mono_note_list) {
		//	DBG(i.first);
		//}
	}

	if (m_mono_poly_legato != PlayModes::Retrig) {
		m_global_env.restartEnvelopeLegato();
	} else {
		m_global_env.restartEnvelopeRetrig();
	}

	if (*m_lfo4_reset) {
		m_global_lfo.voiceStart(false);
	}

	if (*m_phaser_reset) {
		m_phaser.resetLFO();
	}
	if (*m_flanger_reset) {
		m_flanger[0].resetLFO();
		m_flanger[1].resetLFO();
	}
	if (*m_chorus_reset) {
		m_chorus[0].resetLFO();
		m_chorus[1].resetLFO();
	}

	int unison_counter = 0;
	int unison_voices  = m_value_tree.state.getChildWithName("misc")["unison_voices"];
	auto voice_numbers = m_voice_manager.getVoices(p_midi_note, unison_voices);
	if (m_last_midi_note == -1) {
		// first time glide - dont glide
		m_last_midi_note = p_midi_note;
	}
	for (int new_voice : voice_numbers) {
		m_voice[new_voice].start(
		    p_midi_note,
		    p_midi_velocity,
		    m_last_midi_note,
		    m_unison_pan_positions[unison_voices][unison_counter],
		    //we "shuffle" pan positions around to get detune positions:
		    m_unison_pan_positions[unison_voices][m_unison_detune_positions[unison_voices][unison_counter]],
		    m_unison_gain_factors[unison_voices],
		    unison_voices > 1,
		    p_arp_mod_1,
		    p_arp_mod_2);
		m_voice[new_voice].amp.setMIDIVelocity(p_midi_velocity);
		m_mod_matrix.setMostRecentVoice(new_voice);
		++unison_counter;
	}
	m_last_midi_note = p_midi_note;

	// set values to filters
	for (int stereo = 0; stereo < 2; ++stereo) {
		m_ladder_filter[stereo].m_MIDI_note = p_midi_note;
		m_diode_filter[stereo].m_MIDI_note  = p_midi_note;
		m_korg_filter[stereo].m_MIDI_note   = p_midi_note;
		m_SEM_filter_12[stereo].m_MIDI_note = p_midi_note;
		m_comb_filter[stereo].m_MIDI_note   = p_midi_note;
		m_ring_mod[stereo].m_MIDI_note      = p_midi_note;

		m_ladder_filter[stereo].m_MIDI_velocity = p_midi_velocity;
		m_diode_filter[stereo].m_MIDI_velocity  = p_midi_velocity;
		m_korg_filter[stereo].m_MIDI_velocity   = p_midi_velocity;
		m_SEM_filter_12[stereo].m_MIDI_velocity = p_midi_velocity;
		m_comb_filter[stereo].m_MIDI_velocity   = p_midi_velocity;
		m_ring_mod[stereo].m_MIDI_velocity      = p_midi_velocity;
	}
}

void OdinAudioProcessor::midiNoteOff(int p_midi_note) {
	//DBG("NoteOff, key " + std::to_string(p_midi_note));
	if (m_mono_poly_legato != PlayModes::Poly) {

		//remove note from mono note list
		bool note_killed_is_most_recent = false;
		for (auto note_it = m_playmode_mono_note_list.begin(); note_it != m_playmode_mono_note_list.end(); note_it++) {
			if (note_it->first == p_midi_note) {
				note_killed_is_most_recent = (note_it->first == m_playmode_mono_note_list.back().first);
				m_playmode_mono_note_list.erase(note_it);
				break;
			}
		}

		//DBG("List:");
		//for (auto const &i : m_playmode_mono_note_list) {
		//	DBG(i.first);
		//}

		//if mono/legato && killed note is the one playing && note list not empty, we play the most recent pressed key again
		if (!m_playmode_mono_note_list.empty() && note_killed_is_most_recent) {
			midiNoteOn(m_playmode_mono_note_list.back().first, m_playmode_mono_note_list.back().second, 0, 0, false);
		}
	}

	if (!m_voice_manager.getSustainActive()) {
		for (int voice = 0; voice < VOICES; ++voice) {
			if (m_voice[voice].keyUp(p_midi_note)) {
				//DBG("KeyUp on voice " + std::to_string(voice));
			}
		}
	} else {
		for (int voice = 0; voice < VOICES; ++voice) {
			if (m_voice[voice].usesThisMIDIKey(p_midi_note)) {
				m_voice_manager.addToKillList(voice, p_midi_note);
			}
		}
	}

	checkEndGlobalEnvelope();
}

void OdinAudioProcessor::handleMidiMessage(const MidiMessage &p_midi_message ) {
//    DBG(p_midi_message.getDescription());
	// apply midi message
	if (p_midi_message.isNoteOn()) {
		handleMidiNoteOn(p_midi_message.getNoteNumber(), p_midi_message.getVelocity());
	} else if (p_midi_message.isNoteOff()) {
		handleMidiNoteOff(p_midi_message.getNoteNumber());
	} else if (p_midi_message.isAllNotesOff() || p_midi_message.isAllSoundOff()) {
		allNotesOff();
	} else if (p_midi_message.isPitchWheel()) {
		setPitchWheelValue(p_midi_message.getPitchWheelValue());
	} else if (p_midi_message.isController() && p_midi_message.getControllerNumber() == 1) { // modwheel
		setModWheelValue(p_midi_message.getControllerValue());
	} else if (p_midi_message.isSustainPedalOn()) {
		if (m_arpeggiator_on) {
			m_arpeggiator.setSustainActive(true);
        } else {
			m_voice_manager.setSustainActive(true);
		}
		DBG("Sustain pedal pressed");
	} else if (p_midi_message.isSustainPedalOff()) {
		DBG("Sustain pedal released");
		m_voice_manager.setSustainActive(false);
		for (int voice = 0; voice < VOICES; ++voice) {
			if (m_voice_manager.isOnKillList(voice)) {
				m_voice[voice].startRelease();
			}
		}
		m_arpeggiator.setSustainActive(false);
		m_voice_manager.clearKillList();
		checkEndGlobalEnvelope();
	} else if (p_midi_message.isAftertouch()) {
		// todo this is untested
		DBG("receiving afterouch, note: " + std::to_string(p_midi_message.getNoteNumber()) +
		    ", value: " + std::to_string((float)p_midi_message.getAfterTouchValue() / 127.f));
		for (int voice = 0; voice < VOICES; ++voice) {
			m_voice[voice].setAftertouch(p_midi_message.getNoteNumber(),
			                             (float)p_midi_message.getAfterTouchValue() / 127.f);
		}
	} else if (p_midi_message.isChannelPressure()) {
		m_MIDI_channel_pressure = (float)p_midi_message.getChannelPressureValue() / 127.f;
	} else if (p_midi_message.isSoftPedalOn()) {
		m_soft_pedal = 1.f;
	} else if (p_midi_message.isSoftPedalOff()) {
		m_soft_pedal = 0.f;
	} else if(p_midi_message.isController() && p_midi_message.getControllerNumber() == MIDI_CONTROLLER_NUMBER_BREATH) {
		m_midi_breath = p_midi_message.getControllerValue() / 127.f;
    } else if(p_midi_message.isController() && p_midi_message.getControllerNumber() == MIDI_CONTROLLER_NUMBER_BANK_SELECT){
        selectBankOrCategory(p_midi_message.getControllerValue());
    } else if(p_midi_message.isProgramChange()) {
        selectProgram(p_midi_message.getProgramChangeNumber());
    }
    else {
		if (!p_midi_message.isMidiClock()) {
			//DBG("UNHANDELED MIDI MESSAGE: " + p_midi_message.getDescription());
            if (p_midi_message.isController()) {
//                DBG("Controller with number: " + std::to_string(p_midi_message.getControllerNumber()));
            }
		}
	}

	if ((p_midi_message.isController() && !(p_midi_message.isAllNotesOff()))) {

		// midi learn
		if (m_midi_learn_parameter_active) {
			m_midi_control_param_map.emplace(p_midi_message.getControllerNumber(),
			                                 m_value_tree.getParameter(m_midi_learn_parameter_ID));
            m_midi_learn_control->setMidiControlActive();
//            m_midi_learn_control->setMidiPatchControlActive();
			m_midi_learn_parameter_active = false;
			m_midi_learn_control          = nullptr;
			//add control to value tree
			m_value_tree.state.getChildWithName("midi_learn")
			    .setProperty(m_midi_learn_parameter_ID, p_midi_message.getControllerNumber(), nullptr);

//			DBG(m_value_tree.state.toXmlString());

            DBG("Added MIDI control for parameter " +
                m_midi_control_param_map.find(p_midi_message.getControllerNumber())->second->paramID +
			    " on controller number " + std::to_string(p_midi_message.getControllerNumber()));
			m_midi_learn_parameter_ID = "";

#ifdef ODIN_DEBUG
			int counter = 1;
			DBG("=========");
			for (auto const &control : m_midi_control_param_map) {
				DBG(std::to_string(counter++) + ": " + control.second->paramID.toStdString());
			}
			DBG("=========");
#endif
		}

		// do midi control
        for (auto const &control : m_midi_control_param_map) {
            if (control.first == p_midi_message.getControllerNumber()) {
                //todo replace lock by async MM call
                //const MessageManagerLock mmLock;
                control.second->setValueNotifyingHost(/*control.second->convertFrom0to1(*/
                                                      (float)p_midi_message.getControllerValue() / 127.f); //));
            }
        }
	}
}

void OdinAudioProcessor::startMidiLearn(const String &p_parameter_ID, OdinMidiLearnBase *p_GUI_control) {
    DBG(">>CMD>> " + String( 0 ) );

	DBG("MIDI LEARN FOR PARAMETER " + p_parameter_ID + " WAS SIGNALED!");
	if (m_midi_learn_parameter_active) {
		m_midi_learn_control->stopMidiLearn();
	}
	m_midi_learn_parameter_ID     = p_parameter_ID;
	m_midi_learn_parameter_active = true;
	m_midi_learn_control          = p_GUI_control;
    loaded_new_map = false;
}

void OdinAudioProcessor::midiForget(const String &p_parameter_ID, OdinMidiLearnBase *p_GUI_control) {
	for (std::multimap<int, RangedAudioParameter *>::iterator iter = m_midi_control_param_map.begin();
	     iter != m_midi_control_param_map.end();) {
		std::multimap<int, RangedAudioParameter *>::iterator erase_iter = iter++;
		if (erase_iter->second == m_value_tree.getParameter(p_parameter_ID)) {
			m_midi_control_param_map.erase(erase_iter);

			//remove the control from the list in valuetree
			if (m_value_tree.state.getChildWithName("midi_learn").hasProperty(p_parameter_ID)) {
				m_value_tree.state.getChildWithName("midi_learn").removeProperty(p_parameter_ID, nullptr);
			}

//			DBG(m_value_tree.state.toXmlString());

#ifndef ODIN_DEBUG
			return;
#endif
		}
	}

#ifdef ODIN_DEBUG
	int counter = 1;
	DBG("=========");
	for (auto const &control : m_midi_control_param_map) {
        DBG("PPMidi: " + std::to_string(counter++) + ": " + control.second->paramID.toStdString());
	}
	DBG("=========");
#endif
}

void OdinAudioProcessor::stopMidiLearn() {

	m_midi_learn_parameter_active = false;
	m_midi_learn_parameter_ID     = "";
	m_midi_learn_control          = nullptr;
}

WavetableContainer *OdinAudioProcessor::getWavetableContainerPointer() {
	return &m_WT_container;
}

void OdinAudioProcessor::allNotesOff() {
	for (int voice = 0; voice < VOICES; ++voice) {
		m_voice[voice].forceKeyUp();
	}
	m_arpeggiator.endPlayingNotes();
}

void OdinAudioProcessor::midiForgetAll(){
    // m_midi_control_param_map.clear();

    for (std::multimap<int, RangedAudioParameter *>::iterator iter = m_midi_control_param_map.begin();
         iter != m_midi_control_param_map.end();) {
        std::multimap<int, RangedAudioParameter *>::iterator erase_iter = iter++;
        m_midi_control_param_map.erase(erase_iter);

        // remove all controls from the list in valuetree
        m_value_tree.state.getChildWithName("midi_learn").removeAllProperties(nullptr);
    }
}

