# Vcheck_Patcher

This source will build a DXGI shim or F4SE/SKSE64 plugin that when used will patch the plugin header version check. The game EXE will check the version header and if it is too high it skips loading the plugin. This comes with a risk if you are using a game EXE that does not actually know how to handle the plugin odd things may happen.

This was originally developed because Bethesda changed the version header in the November 2019 Fallout 4 patch. This prevented using older game EXEs while waiting on other desired F4SE plugins to be updated.

## Build Instructions

This project has 3 build styles.
1) A DXGI shim which does not require F4SE or SKSE64. It still supports either Skyrim Special Edition or Fallout 4 based on build selection within the solution.
2) A F4SE plugin.
3) A SKSE64 plugin.

Developed with Visual Studio 2019 (Community Edition is fine)

NOTE: When loading the solution you will receive an error from Visual Studio that one or more project files could not be opened. This happens if you do not have one or more of the F4SE or SKSE project files. Since the solution included tries to load both and this is atypical, this is normal. Verify that the projects you do have, are loaded.

For building a DXGI shim:
1) Clone or extract this repository somewhere.
2) Load the solution and choose one of the build targets (<Debug/Release>_<game>_Dxgi).
3) Retarget for your SDK and build tools as necessary.
4) Build the solution.

For building an SKSE64 or F4SE plugin

1) Download the clean f4se or skse64 source from their respective website [f4se's website](http://f4se.silverlock.org/) [skse64's website](http://skse.silverlock.org/)
2) Extract or clone this repository into the f4se/skse64 folder of the source. The xSE solution and the one from this project should be side by side. You can safely leave or delete f4se_loader, f4se_steam_loader, f4se_loader_common, and the f4se64 solution.
3) Load the solutionand choose one of the build targets for the game desired (<Debug/Release>_<game>_<xSE>).
4) Retarget for your SDK and build tools as necessary.
5) Build the solution.

A more manual method:

If you wish to do something different there are a few DEFINES that are important.

_SKYRIM64  -  Specifies to use things relevant to Skryim Special Edition
_FALLOUT4  -  Specified to use things relevant to Fallout 4
SKSE64     -  Will use includes and other settings for SKSE64 plugin development
F4SE       -  Will use includes and other settings for F4SE plugin development
_DXGI_SHIM -  Will enable and disable large chunks of code for use or not as a DXGI shim.

## Pre-Built Binaries

You can find a pre-built binary for an F4SE plugin from the Nexus download page at https://www.nexusmods.com/fallout4/mods/42497

## Credits

Jonathan Ostrus - original concept and main developer

Eckserah - putting up with me

Nukem - old memory search/patch function code

reg2k - RVA/Utils code. Basis from mixed repos https://github.com/reg2k

The Pattern matching code is from an unknown author. I found it in reg2k's repos.