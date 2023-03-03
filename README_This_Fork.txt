This fork to allow:

  Changing: Bank/Category/Patch via Midi messages.
  
  Current config (testing usability)
  
  Bank changes:
    Midi cc:0 , values 64 to 127 Select Soundbank folder (selects bank '0_*' to '63_*' as seen on gui)
    
  Category changes:
    Midi cc:0 , values 0 to 63 Select category in current soundbank
    
  Patch changes:
    Midi program-change values 0 to 127 Select patch in current category

  Also adding midi-map per patch...
   
Current caveats:
    1) Bank/Category/Patch names MUST start with number followed by an underscore for this to work.
   
   eg: 3_ *        - [midi cc 0 , value 67] selects this soundbank. (Starts in bank named: "0_*")
        0_ *       - [midi cc 0 , value 0 ] selects this category. (IN SELECTED SOUNDBANK!, Starts in category: "0_*")
          6_ *     - [midi program change value: 6]  selects this patch. (IN SELECTED CATEGORY!)
          
    2) Although it works, when gui is showing and reflecting changes, the parameters are being changed twice.
            ( need to go deeper to fix )
                This problem goes away when running headless in carla.

    3) I had to add: #include <utility>
        to top of file: ./libs/JUCELV2/modules/juce_gui_basics/windows/juce_ComponentPeer.h
        to build juiceade.

    4) I am obviously winging it here.


