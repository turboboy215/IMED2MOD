# IMED2MOD
##Climax/Images Software to MOD converter
This tool converts music from Game Boy, Game Boy Color, and Game Gear games using Climax's "IMEDGBoy" and "IMEDGear" module formats to standard ProTracker MOD. Unlike my other converters, rather than working with ROMs, this requires each individual "module" to be extracted from the ROM image. Note that some games, including Warriors of Might and Magic, store all or most of the music compressed using RNC/ProPack, so they need to be decompressed first.
These module formats are heavily based on the standard MOD format, and use the same numbers for effects. However, they are more compact using a sort of binary mask as "compression", and obviously use different instruments. This tool was created with the assistance of the official source code for Deadly Skies.
All effects are handled properly by the converter. Similar to GHXM, the resulting MOD files don't sound "great" since all the instruments are represented by the same sine waveform. Drums in particular sound "annoying".

Examples:
* IMED2MOD "LEGO Alpha Team - 1.mod"
* IMED2MOD "Bubsy II - 2.mod"
* IMED2MOD "The Smurfs Travel the World - 4.mod"


Supported games:
Game Boy/Game Boy Color:
* Bubsy II
* Deadly Skies
* LEGO Alpha Team
* LEGO Racers
* NHL Blades of Steel
* NHL Blades of Steel 2000
* Warriors of Might and Magic

Game Gear:
* Chicago Syndicate
* The Smurfs Travel the World (also on Master System)

## To do:
  * Support for the SNES (IMEDSNES) and MegaDrive (IMEDMega) module formats
