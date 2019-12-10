# Vcheck_Patcher for Fallout 4

This source will build an F4SE plugin DLL that when used will patch the plugin header version check. The game EXE will check the version header and if it is too high it skips loading the plugin. This comes with a risk if you are using a game EXE that does not actually know how to handle the plugin odd things may happen.

This was originally developed because Bethesda changed the version header in the November 2019 patch. This prevented using old game EXEs while waiting on other desired F4SE plugins to be updated.

## Build Instructions

Requires Visual Studio 2019 (Community Edition is fine)

1) Download the clean f4se source from [f4se's website](http://f4se.silverlock.org/)
2) Extract or clone this repository into the f4se folder of the source. You can safely leave or delete f4se_loader, f4se_steam_loader, f4se_loader_common, and the f4se64 solution. You may also leave them if you wish.
3) Load and build the solution :)

## Pre-Built Binaries

You can find a pre-built binary from the Nexus download page at https://www.nexusmods.com/fallout4/mods/42497

## Credits

Jonathan Ostrus - original concept and main developer

Eckserah - putting up with me

Nukem - memory search/patch function code

