# Nehrim Steam Plugin

This is the source code for an OBSE plugin that enables the use of Steam for the TES IV Oblivion Total Conversion Mod Nehrim. The plugin does the following:

 - On startup, reinitializes the Steam API for the Steam version of Nehrim. Steam will now track In-Game time for Nehrim
 - In a new thread, run the Steam event loop to enable callbacks
 - Register a new script function `UnlockAchievements` that takes one string parameter and unlocks the Steam Nehrim achievement with the given code
 
Note that the reinitialization will only happen after the game has started, so Steam will check that Oblivion is owned as usual.

# OBSE Plugin

This is an OBSE plugin. Most of the code (i.e. all code in `common/` and `obse/`) is part of the OBSE project, and only the sources in `nehrim_steam/` are written by us.

# References

 - [OBSE](https://obse.silverlock.org/)
 - [Nehrim](https://sureai.net/games/nehrim/)
 - [Nehrim on Steam](https://store.steampowered.com/app/1014940/Nehrim_At_Fates_Edge/)
 - [Oblivion Game of the Year Edition on Steam](https://store.steampowered.com/app/22330/)