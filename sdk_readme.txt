
// ====================================================
//
// Enemy Territory: QUAKE Wars(TM) 1.5 SDK
//
// ====================================================


Miscellaneous Coding Notes:
 - You can use Visual Studio 2005 Express edition to compile the SDK.

 - You can find the final gamex86.dll in source\build\win32\<configuration name>\
   Copy it to the same folder as your etqw.exe and the game will automatically load your new dll.

 - We recommend setting up a post-build event to automatically copy the dll to the game folder.
 
 

Miscellaneous Mapping Notes:

 - You must add your custom map script to base/scripts/main.script (look for the #includes before game_main)
 
 - You must add your custom map or campaign metadata to base/pakmeta.conf while developing
   This file will only be loaded when using the SDK version of ETQW.
   
 - To include your metadata for use with the full game, you must create an addon.conf and package your map in a PK4.
   See sample_addon.conf for an example of how to create an addon.conf file.
