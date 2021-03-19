######################################################################
# Automatically generated by qmake (3.0) ?? 2? 4 17:49:37 2017
######################################################################

QT += dtkgui
QT += dtkwidget
TEMPLATE = app
TARGET = deepin-screen-recorder
INCLUDEPATH += .

include(accessibility/accessible.pri)


QMAKE_CXX += -Wl,--as-need -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,-O1
QMAKE_CXXFLAGS += -Wl,--as-need -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,-O1
QMAKE_LFLAGS += -Wl,--as-needed -pie

ARCH = $$QMAKE_HOST.arch
isEqual(ARCH, mips64) {
    QMAKE_CXX += -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi
    QMAKE_CXXFLAGS+= -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi
}

CONFIG += link_pkgconfig c++11
PKGCONFIG += xcb xcb-util dframeworkdbus

RESOURCES = ../assets/image/deepin-screen-recorder.qrc \
    ../assets/resources/resources.qrc \
    ../assets/icons/icons.qrc

# Input
HEADERS += main_window.h \
    record_process.h \
    utils.h \
    countdown_tooltip.h \
    constant.h \
    event_monitor.h \
    button_feedback.h \
    #process_tree.h \
    show_buttons.h \
    keydefine.h   \
    utils/audioutils.h \
    menucontroller/menucontroller.h \
    utils/baseutils.h \
    utils/configsettings.h \
    utils/shortcut.h \
    utils/tempfile.h \
    utils/calculaterect.h \
    utils/saveutils.h \
    utils/shapesutils.h \
    #widgets/colorbutton.h \
    widgets/zoomIndicator.h \
    widgets/textedit.h \
    widgets/toptips.h \
    widgets/toolbar.h \
    #widgets/textbutton.h \
    widgets/shapeswidget.h \
    widgets/toolbutton.h \
    widgets/maintoolwidget.h \
    widgets/subtoolwidget.h \
    widgets/keybuttonwidget.h \
    widgets/sidebar.h \
    widgets/shottoolwidget.h \
    widgets/colortoolwidget.h \
    dbusinterface/dbusnotify.h \
    dbusservice/dbusscreenshotservice.h \
    #dbusservice/dbusscreenshot.h \
    widgets/camerawidget.h \
    screenshot.h \
    utils/voicevolumewatcher.h \
    utils/camerawatcher.h \
    widgets/tooltips.h \
    widgets/filter.h \
    utils/desktopinfo.h \
    utils/screengrabber.h \
    #dbusinterface/drawinterface.h \
    screen_shot_event.h\
    RecorderRegionShow.h \
    recordertablet.h

SOURCES += main.cpp \
    main_window.cpp \
    record_process.cpp \
    utils.cpp \
    countdown_tooltip.cpp \
    constant.cpp \
    event_monitor.cpp \
    button_feedback.cpp \
    #process_tree.cpp \
    show_buttons.cpp  \
    utils/audioutils.cpp \
    menucontroller/menucontroller.cpp \
    utils/shapesutils.cpp \
    utils/tempfile.cpp \
    utils/calculaterect.cpp \
    utils/shortcut.cpp \
    utils/configsettings.cpp \
    utils/baseutils.cpp \
    widgets/toptips.cpp \
    widgets/shapeswidget.cpp \
    #widgets/textbutton.cpp \
    #widgets/colorbutton.cpp \
    widgets/textedit.cpp \
    widgets/zoomIndicator.cpp \
    widgets/toolbar.cpp \
    widgets/maintoolwidget.cpp \
    widgets/subtoolwidget.cpp \
    widgets/keybuttonwidget.cpp \
    widgets/sidebar.cpp \
    widgets/shottoolwidget.cpp \
    widgets/colortoolwidget.cpp \
    dbusinterface/dbusnotify.cpp \
    dbusservice/dbusscreenshotservice.cpp \
    #dbusservice/dbusscreenshot.cpp \
    widgets/camerawidget.cpp \
    screenshot.cpp \
    utils/voicevolumewatcher.cpp \
    utils/camerawatcher.cpp \
    widgets/tooltips.cpp \
    widgets/filter.cpp \
    utils/desktopinfo.cpp \
    utils/screengrabber.cpp \
    #dbusinterface/drawinterface.cpp \
    screen_shot_event.cpp\
    RecorderRegionShow.cpp \
    recordertablet.cpp

QT += core
QT += widgets
QT += gui
QT += x11extras
QT += dbus
QT += multimedia
QT += multimediawidgets
QT += concurrent
LIBS += -lX11 -lXext -lXtst -lXfixes
LIBS += -L"libprocps" -lprocps
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -Wno-error=deprecated-declarations -Wno-deprecated-declarations

isEmpty(PREFIX){
    PREFIX = /usr
}

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(ICONDIR):ICONDIR=/usr/share/icons/hicolor/scalable/apps
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-screen-recorder
isEmpty(DOCDIR):DOCDIR=/usr/share/dman/deepin-screen-recorder
isEmpty(ETCDIR):ETCDIR=/etc
isEmpty(TABCONDIR):TABCONDIR=/etc/due-shell/json

target.path = $$INSTROOT$$BINDIR
icon.path = $$INSTROOT$$ICONDIR
desktop.path = $$INSTROOT$$APPDIR
translations.path = $$INSTROOT$$DSRDIR/translations
#manual.path = $$INSTROOT$$DOCDIR
#shotShell.path = $$INSTROOT$$BINDIR
#modprobe.path = $$ETCDIR/modprobe.d
#modload.path = $$ETCDIR/modules-load.d

#icon.files = image/deepin-screen-recorder.svg deepin-screenshot.svg
#desktop.files = deepin-screen-recorder.desktop deepin-screenshot.desktop
icon.files = ../assets/image/deepin-screen-recorder.svg ../assets/image/deepin-screenshot.svg
desktop.files = ../deepin-screen-recorder.desktop
#manual.files = ../manual/*
#shotShell.files = ../deepin-screenshot
#modprobe.files = ../modinfo/modprobe.d/deepin-screen-recorder.conf
#modload.files = ../modinfo/modulesload.d/deepin-screen-recorder.conf

dbus_service.files = $$PWD/../com.deepin.ScreenRecorder.service $$PWD/../com.deepin.Screenshot.service
dbus_service.path = $$PREFIX/share/dbus-1/services

#平板图标资源
tablet_resources.path = $$INSTROOT$$DSRDIR/tablet_resources
tablet_resources.files = ../assets/tabletUI/fast-icon_recording_active.svg ../assets/tabletUI/fast-icon_recording_normal.svg \
            ../assets/tabletUI/counting1.svg ../assets/tabletUI/counting2.svg ../assets/tabletUI/counting3.svg \
            ../assets/tabletUI/recording0.svg ../assets/tabletUI/recording1.svg
#平板配置资源
tablet_config.path = $$TABCONDIR
tablet_config.files = ../assets/screenRecorder.json


#INSTALLS += target icon desktop manual dbus_service shotShell
#INSTALLS += target icon desktop manual dbus_service modload modprobe
#INSTALLS += target icon desktop dbus_service tablet_resources

isEmpty(TRANSLATIONS) {
     include(../translations.pri)
}

TRANSLATIONS_COMPILED = $$TRANSLATIONS
TRANSLATIONS_COMPILED ~= s/\.ts/.qm/g

translations.files = $$TRANSLATIONS_COMPILED
#INSTALLS += translations

manual_dir.files = $$PWD/../assets/deepin-screen-recorder
manual_dir.path= /usr/share/deepin-manual/manual-assets/application/
INSTALLS += target icon desktop dbus_service tablet_resources tablet_config translations manual_dir

CONFIG *= update_translations release_translations

CONFIG(update_translations) {
    isEmpty(lupdate):lupdate=lupdate
    system($$lupdate -no-obsolete -locations none $$_PRO_FILE_)
}
CONFIG(release_translations) {
    isEmpty(lrelease):lrelease=lrelease
    system($$lrelease $$_PRO_FILE_)
}

DSR_LANG_PATH += $$DSRDIR/translations
DEFINES += "DSR_LANG_PATH=\\\"$$DSR_LANG_PATH\\\""

DISTFILES += \
    ../assets/image/newUI/focus/close-focus.svg



# 检查集成测试标签
AC_FUNC_ENABLE = true
# 获取标签系统设置
#AC_FUNC_ENABLE = $$(ENABLE_AC_FUNC)
equals(AC_FUNC_ENABLE, true ){
    DEFINES += ENABLE_ACCESSIBILITY
    message("deepin-screen-recorder enabled accessibility function with set: " $$AC_FUNC_ENABLE)
}

#内存检测标签
TSAN_TOOL_ENABLE = false
equals(TSAN_TOOL_ENABLE, true ){
    #DEFINES += TSAN_THREAD #互斥
    DEFINES += ENABLE_TSAN_TOOL
    message("deepin-screen-recorder enabled TSAN function with set: " $$TSAN_TOOL_ENABLE)
    contains(DEFINES, TSAN_THREAD){
       QMAKE_CXXFLAGS+="-fsanitize=thread"
       QMAKE_CFLAGS+="-fsanitize=thread"
       QMAKE_LFLAGS+="-fsanitize=thread"
    } else {
       QMAKE_CXXFLAGS+="-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
       QMAKE_CFLAGS+="-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
       QMAKE_LFLAGS+="-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
    }
}
