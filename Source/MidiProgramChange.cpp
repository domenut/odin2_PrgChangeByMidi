// For IDE Only ->:
#include "PluginProcessor.h"
#include "gui/PatchBrowser.h"
// <- For IDE Only END:
//

// CMDEBUG>

void OdinAudioProcessor::selectBankOrCategory(int index){
    if((index >= 0) && (index <= 63)){
        selectCategory(index);
    }
    if((index >= 64) && (index <= 127)){
        selectSoundbank(index - 63);
    }
}

void OdinAudioProcessor::selectProgram(int index){
    if(current_category == ""){
        selectCategory(1);
        category_change_dir = current_category ;
    }
    String wildcard = (String(index) + "_*.odin");
    File current_dir(DEFAULT_SOUNDBANK_LOCATION_STRING + File::getSeparatorString() +
                     current_bank + File::getSeparatorString() + current_category);
    Array<File> file_array = current_dir.findChildFiles(2, false, wildcard);

    if( ! (file_array[0].getFileName()=="")){
        String absolute_path = current_dir.getFullPathName() +
                File::getSeparatorString() + file_array[0].getFileName() ;
        File file_to_open(absolute_path);
        FileInputStream file_stream(file_to_open);
        if (file_stream.openedOk()) {
            file_stream.setPosition(0);
            readPatch(ValueTree::readFromStream(file_stream).createCopy());
            program_change_path = current_dir.getFullPathName() + File::getSeparatorString() + file_array[0].getFileName() ;
            program_change_trigger = 1 ;
        }
    }
}

void OdinAudioProcessor::selectCategory(int index){
    if(current_bank == ""){
        selectSoundbank(1);
    }
    String wildcard = (String(index) + "_*");
    File category_dir(DEFAULT_SOUNDBANK_LOCATION_STRING + File::getSeparatorString() + current_bank);
    Array<File> file_array = category_dir.findChildFiles(1, false, wildcard);
    current_category = file_array[0].getFileName();
    category_change_dir = current_category ;
    program_change_trigger = 2 ;
}

void OdinAudioProcessor::selectSoundbank(int index){
    String indxstr = String(index);
    String wildcard = (indxstr + "_*");
    File category_dir(DEFAULT_SOUNDBANK_LOCATION_STRING);
    Array<File> file_array = category_dir.findChildFiles(1, false, wildcard);
    current_bank = file_array[0].getFileName();
    bank_change_dir = current_bank;
    program_change_trigger = 3 ;
}







// CMDEBUG<





