@echo off

echo Downloading QTFFmpegWrapper

wget http://qtffmpegwrapper.googlecode.com/files/QTFFmpegWrapper-20100323.zip
unzip QTFFmpegWrapper-20100323.zip QTFFmpegWrapper/*
del QTFFmpegWrapper-20100323.zip

echo Downloading FFmpeg libraries

wget http://qtffmpegwrapper.googlecode.com/files/ffmpeg_lib_win32-latest.zip
unzip ffmpeg_lib_win32-latest.zip
del ffmpeg_lib_win32-latest.zip


echo Done