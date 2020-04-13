   SynScopeV

   Copyright (C) 2009,2010:
         Daniel Roggen, droggen@gmail.com


History
-------

V 2.10 - 27.10.2010: Added app icon

V 2.00 - 19.10.2010: New visualizations: stickman, label plot, scatter plot

V 1.00 - 23.03.2010: Video export function


V 0.91 - 03.09.2009: Support for signal files with 'NaN' values (interpreted as 0)

V 0.90 - 24.08.2009: Initial beta




License
-------

This software is licensed under the terms of the GPL v.2 license.


Dependencies
------------

- QT and QT/Phonon are required.
- DirectShow (Windows), Quicktime (OSX), or Xine/gstreamer (Linux).

Phonon is available by default on OSX and Linux in the QT distribution. 
Windows distributions of QT before version 2009.05 need to be re-compiled enabling phonon and direct-show backend. This includes the following steps:
- updating to mingw with gcc version 4.4
- installing directx sdk
- patching include files in the mingw directory
- configuring and compiling QT with phonon and the directshow backend.
See http://labs.trolltech.com/blogs/2009/07/15/phonon-and-mingw-a-story-about-true-windows-love/ for more informations.

Note: this seems unnecessary since Qt 2009.05 - phonon is now precompiled.

- QtFFmpegWrapper: a BSD licensed QT wrapper for FFmpeg. See the script source/getgetffmpeg to pull the sources from the mercurial repository.
