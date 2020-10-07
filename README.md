# Talent VST

MVP Purpose: Create a VST that makes it easier for a producer to communicate with the talent in a studio situation.

# TODO list

Initial version:

- A single VST plugin that allows the producer to enter the lyrics of a song into the VST
- The lyrics are visualized in a karaoke-like fashion
- The lyrics are entered as separate pages, either using JUCE tabs or Markdown-like syntax
- Swapping pages is done using MIDI note events (C2 for page 1, D2 for page 2, etc)
- It should also be possible to add pitch bend to describe how far along the current page the timing has come

# Sandbox

There is a JUCE project in `src/sandbox` that does crude and rudimentary visualization of lyrics