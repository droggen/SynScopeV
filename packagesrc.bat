@echo off
rd /s /q SynScopev-build-desktop
zip -r SynScopeV-src.zip source\* -x *.svn* -x source\ffmpeg_lib_win32\* -x source\QTFFmpegWrapper\*
zip -r SynScopeV-src.zip examples\* -x *.svn*
zip -r SynScopeV-src.zip LICENSE
zip -r SynScopeV-src.zip README.txt
