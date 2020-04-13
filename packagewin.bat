@echo off
rd /q /s windows
del SynScopeV-win.zip
md windows
rem md windows\phonon_backend
copy README.txt windows
copy LICENSE windows
copy SynScopev-build-desktop\release\synscopev.exe windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\QtCore4.dll windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\QtGui4.dll windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\QtOpenGL4.dll windows
rem copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\QtNetwork4.dll windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\QtXml4.dll windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\phonon4.dll windows
rem copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\QtSvg4.dll windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\mingwm10.dll windows
copy C:\QtSDK\Desktop\Qt\4.8.0\mingw\bin\libgcc_s_dw2-1.dll windows

copy ..\qtffmpegwrapper_trunk\ffmpeg_lib_win32\*.dll windows

cd windows
zip -r ..\SynScopeV-win.zip * -x *.svn*
cd ..
zip -r SynScopeV-win.zip examples\* -x *.svn*




