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

#include <JuceHeader.h>
#include "PatchBrowserSelector.h"
#include "../PluginProcessor.h"

#define DEFAULT_SOUNDBANK_LOCATION_STRING (ODIN_STORAGE_PATH + File::getSeparatorString() + "Soundbanks")


class PatchBrowser    : public Component,
        // CMDEBUG>
        public Timer
        // CMDEBUG<
{
public:
    PatchBrowser(OdinAudioProcessor &p_processor, AudioProcessorValueTreeState &p_vts);
    ~PatchBrowser();

    void paint (Graphics&) override;
    void paintOverChildren (Graphics&) override;

    void setGUIBig();
    void setGUISmall();

	std::function<void()> forceValueTreeLambda;

private:
    // CMDEBUG>
    void midiProgChangeTimerInit();
    void timerCallback() override ;
    void categoryChanger();
    void bankChanger();
    void updateGuiAfterMidiPatchChange();
    // CMDEBUG<

    void loadPatchWithFileBrowserAndCopyToCategory(String p_directory);
    void loadPatchFromOpenedFileStream(juce::FileInputStream &p_file_stream);
    void savePatchInOpenedFileStream(FileOutputStream &p_file_stream);
    void loadSoundbankWithFileBrowser(String p_directory);

    bool checkForBiggerVersion(FileInputStream &p_file_stream, std::string &p_version_string);
    bool checkForSmallerVersion(FileInputStream &p_file_stream, std::string &p_version_string);

    bool usesWavedraw(int p_osc);
    bool usesChipdraw(int p_osc);
    bool usesSpecdraw(int p_osc);

    void setFirstSoundbankActive();


	std::unique_ptr<FileChooser> m_filechooser;

    Image m_background;

    bool m_GUI_big = true;

	OdinAudioProcessor &m_audio_processor;
	AudioProcessorValueTreeState &m_value_tree;

    PatchBrowserSelector m_soundbank_selector;
    PatchBrowserSelector m_category_selector;
    PatchBrowserSelector m_patch_selector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchBrowser)
};
