# Crash Bash Extractor

This is a quickly-written utility to extract the data file from Crash Bash.

Currently it only supports unpacking.

## Usage

```batch
CrashBashEx InExeName InDataName OutFolder TableStart TableEnd
```

For example, for the July 26 demo (found on US Demo Disc 38), the table is located at the file offset 0x335A0 and ends at 0x343F0. This can be discovered through tools like IDA or Ghidra. 

More specifically - the first file load will attempt to load the last file in the table, which is the code overlay with most of the game code. To find the loading function, look for a string like "load file start" and cross-reference its earliest call (should be close to main() ).

Its first argument leads to the last entry in the table. That should lead you to the actual table. Simply visually inspect where it starts and ends, it should be surrounded by zeroes.

The following command will result in extracted files:

```batch
CrashBashEx CRASHBSH.EXE CRASHBSH.DAT output 0x335A0 0x343F0
```

## Extra

I've also included a ripped song that was unused in Crash Bash (from the early July 26th demo).

You can play it either with a SF2+MIDI player (XMPlay, foobar2000, Falcosoft MIDI Player), or with Winamp (through MS Synth DLS) or through the PSF. PSF was created with the Davironica generic driver, so no looping on the PSF for now.

To hear it ingame in the Warp Room, apply the following code (prefereably AFTER entering the game, not in the Demo Disc menu):

```
BETA MUSIC IN WARP ROOM
80042FD8 1084
80042FDC 5F90
80042FDE 0002
```
