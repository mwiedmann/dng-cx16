# Tasks
- Level string creation in ldtk-convert.js assumes the levels are in the correct level order in the .json. They may not be so we need to sort the strings by level before creating the BIN file.
- There is still that bug where entities sometimes jsut move diagonally off the screen. Seems to be when you interact with them (either hit or touch).

# Fixes


# Questions

- We only check inputs periocially to make it easier to stay in a diagonal direction. We may need to check button input every tick or ranged attack firing rate can be limited. Also the issue with sticky direction above.