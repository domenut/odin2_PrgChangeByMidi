
Work In Progress!

 This fork adds:

  CHANGING BANK/CATEGORY/PATCH VIA MIDI MESSAGES: (not working for VST3 version)
  
    Name banks/categories/patches (in gui preset library) with number and underscore "0_" to "63_" "0_My first bank" , "1_ Bank Two" , "45_ song 46" 
    
      Bank changes:
        Midi cc:0 , values 64 to 127 Select Soundbank folder (selects named bank "0_*" to "63_*" as seen on gui)
      Category changes:
        Midi cc:0 , values 0 to 63 Select category in current soundbank
      Patch changes:
        Midi program-change values 0 to 127 Select patch in current category

-------------------------------------------------------------------------------------------
  
  SAVING MIDI LEARNS PER PATCH:
  
    Midi learn as usual on Odin2 gui.
    Save patch. (in gui preset library). Save will include midi map.
    Mapped controls from loaded patches will have a blue outline on gui,
      to help differentiate them from new, unsaved mappings.
      
    Problem:
        To have multiple Odin2 GUI instances not interfere with one another,
          I had to isolate them inside carla-patchbay instances.
     
-------------------------------------------------------------------------------------------
    
  AUDIO-LINE-IN MODULE: (LV2 version has problems with some hosts)
  
    In Oscillator dropdown, select "Audio input".
      Add a signal... 
        Use it like an oscillator.
      Module includes the same high-pass, low-pass and volume controls as the noise oscillator.
        
     Problem:   
      LV2 version, in carla/jalv, audio in is not passed to synth (It works in ardour7 and juce 'AudioPluginHost')
      CLAP version works. (vst3 has other problems, (midi patch change))
    
-------------------------------------------------------------------------------------------
   
Current caveats:

     I had to add: #include <utility>
        to top of file: ./libs/JUCELV2/modules/juce_gui_basics/windows/juce_ComponentPeer.h
        to build juiceade.
    
     I am obviously winging it here.


