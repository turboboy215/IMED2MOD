# IMED2MOD
## Climax/Images Software to MOD converter
This tool converts music from Game Boy, Game Boy Color, Game Gear, and Master System games using Climax's "IMEDGBoy" and "IMEDGear" module formats to standard ProTracker MOD. This converter allows you to convert either an individual "module" file (extracted from the ROM, for some games also need to be decompressed), or an entire ROM using a .CFG file (compressed modules automatically get decompressed). CFG files for all games are provided with the program.
These module formats are heavily based on the standard MOD format, and use the same numbers for effects. However, they are more compact using a sort of binary mask as "compression", and obviously use different instruments. This tool was created with the assistance of the official source code for Deadly Skies.
All effects are handled properly by the converter. Similar to GHXM, the resulting MOD files don't sound "great" since all the instruments are represented by the same sine waveform. Drums in particular sound "annoying".

Credit to Lab 313/Dr. MefistO for reverse-engineering the ProPack application to provide C code for decompression.

Examples:
* IMED2MOD "LEGO Alpha Team - 1.mod" m
* IMED2MOD "Bubsy II - 2.mod" m
* IMED2MOD "The Smurfs Travel the World - 4.mod" m
* IMED2MOD "LEGO Alpha Team (E) (M10) [C][!].gbc" r "LEGO Alpha Team (E) (M10) [C][!].cfg"
* IMED2MOD "Bubsy 2 (E).gb" r "Bubsy 2 (E).cfg"
* IMED2MOD "Smurfs 2, The (E) [!].gg" r "Smurfs 2, The (E) [!].cfg"


Supported games:

Game Boy/Game Boy Color:
* Bubsy II
* Deadly Skies
* LEGO Alpha Team
* LEGO Racers
* NHL Blades of Steel
* NHL Blades of Steel 2000
* Warriors of Might and Magic

Game Gear/Master System:
* Chicago Syndicate
* The Smurfs Travel the World

## To do:
  * Support for the SNES (IMEDSNES) and MegaDrive (IMEDMega) module formats
