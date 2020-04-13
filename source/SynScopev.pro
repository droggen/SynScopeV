# -------------------------------------------------
# Project created by QtCreator 2009-06-24T17:22:33
# -------------------------------------------------
QT += phonon opengl
TARGET = synscopev
TEMPLATE = app
INCLUDEPATH += NPlot SSView
SOURCES += main.cpp \
    mainwindow.cpp \
    cio.cpp \
    referencesdialog.cpp \
    data.cpp \
    sourcedialog.cpp \
    resampledialog.cpp \
    matlabcodedialog.cpp \
    savesyncfiledialog.cpp \
    helpdialog.cpp \
    exportvideo.cpp \
    exportvideodialog.cpp \
    exportmosaicvideodialog.cpp \
    precisetimer.cpp \
    mainwindow_video.cpp \
    videopreviewdialog.cpp \
    guihelper.cpp \
    parse.cpp \
    mainwindow_video_mosaic.cpp \
    NPlot/ScopeBase.cpp \
    NPlot/Scope.cpp \
    NPlot/PlotStickman_p.cpp \
    NPlot/NPlotWidgetStickman.cpp \
    NPlot/NPlotWidgetScopeExt.cpp \
    NPlot/NPlotWidgetScope.cpp \
    NPlot/NPlotWidgetScatter.cpp \
    NPlot/NPlotWidgetLabel.cpp \
    NPlot/NPlotWidgetBase.cpp \
    NPlot/glhelper.cpp \
    NPlot/GfxLib.cpp \
    SSView/SSViewVideo.cpp \
    SSView/SSViewStickman.cpp \
    SSView/SSViewScope.cpp \
    SSView/SSViewScatter.cpp \
    SSView/SSViewLabel.cpp \
    SSView/SSViewAbstract.cpp \
    NPlot/ScopeType.cpp \
    NPlot/videowidgetext.cpp \
    SSView/SeekSlider2.cpp \
    PlotHelper.cpp \
    parsetextmatrix.cpp
HEADERS += mainwindow.h \
    cio.h \
    referencesdialog.h \
    data.h \
    sourcedialog.h \
    resampledialog.h \
    matlabcodedialog.h \
    savesyncfiledialog.h \
    helpdialog.h \
    exportvideo.h \
    exportvideodialog.h \
    exportmosaicvideodialog.h \
    precisetimer.h \
    videopreviewdialog.h \
    guihelper.h \
    parse.h \
    NPlot/SDL_gfxPrimitives_font.h \
    NPlot/ScopeBase.h \
    NPlot/Scope.h \
    NPlot/PlotStickman_p.h \
    NPlot/NPlotWidgetStickman.h \
    NPlot/NPlotWidgetScopeExt.h \
    NPlot/NPlotWidgetScope.h \
    NPlot/NPlotWidgetScatter.h \
    NPlot/NPlotWidgetLabel.h \
    NPlot/NPlotWidgetBase.h \
    NPlot/glhelper.h \
    NPlot/GfxLib.h \
    SSView/SSViewVideo.h \
    SSView/SSViewStickman.h \
    SSView/SSViewScope.h \
    SSView/SSViewScatter.h \
    SSView/SSViewLabel.h \
    SSView/SSViewAbstract.h \
    NPlot/ScopeType.h \
    NPlot/videowidgetext.h \
    SSView/SeekSlider2.h \
    PlotHelper.h \
    parsetextmatrix.h
FORMS += mainwindow.ui \
    referencesdialog.ui \
    sourcedialog.ui \
    resampledialog.ui \
    matlabcodedialog.ui \
    savesyncfiledialog.ui \
    helpdialog.ui \
    exportvideodialog.ui \
    exportmosaicvideodialog.ui \
    videopreviewdialog.ui
RESOURCES += icon.qrc

QMAKE_CXXFLAGS += -std=c++0x -std=gnu++0x

# DEFINES += LINUX
DEFINES += WIN32
DEFINES += DEVELMODE

# icon
win32: RC_FILE = synscopev.rc


# ##############################################################################
# ##############################################################################
# FFMPEG: START OF CONFIGURATION BELOW ->
# Copy these lines into your own project
# Make sure to set the path variables for:
# 1) QTFFmpegWrapper sources (i.e. where the QVideoEncoder.cpp and QVideoDecoder.cpp lie),
# 2) FFMPEG include path (i.e. where the directories libavcodec, libavutil, etc. lie),
# 3) the binary FFMPEG libraries (that must be compiled separately).
# Under Linux path 2 and 3 may not need to be set as these are usually in the standard include and lib path.
# Under Windows, path 2 and 3 must be set to the location where you placed the FFMPEG includes and compiled binaries
# Note that the FFMPEG dynamic librairies (i.e. the .dll files) must be in the PATH
# ##############################################################################
# ##############################################################################

# ##############################################################################
# Modify here: set FFMPEG_LIBRARY_PATH and FFMPEG_INCLUDE_PATH
# ##############################################################################

# Set QTFFMPEGWRAPPER_SOURCE_PATH to point to the directory containing the QTFFmpegWrapper sources
QTFFMPEGWRAPPER_SOURCE_PATH = ../../qtffmpegwrapper_trunk/QTFFmpegWrapper

# Set FFMPEG_LIBRARY_PATH to point to the directory containing the FFmpeg import libraries (if needed - typically for Windows), i.e. the dll.a files
FFMPEG_LIBRARY_PATH = ../../qtffmpegwrapper_trunk/ffmpeg_lib_win32

# Set FFMPEG_INCLUDE_PATH to point to the directory containing the FFMPEG includes (if needed - typically for Windows)
FFMPEG_INCLUDE_PATH = ../../qtffmpegwrapper_trunk/QTFFmpegWrapper

# ##############################################################################
# Do not modify: FFMPEG default settings
# ##############################################################################
# Sources for QT wrapper
SOURCES += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.cpp \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.cpp
HEADERS += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.h \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.h

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale

# Add the path
LIBS += -L$$FFMPEG_LIBRARY_PATH
INCLUDEPATH += QVideoEncoder
INCLUDEPATH += $$FFMPEG_INCLUDE_PATH

# Requied for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################

OTHER_FILES += \
    help.html \
    synscopev.rc \
    BUG.txt


