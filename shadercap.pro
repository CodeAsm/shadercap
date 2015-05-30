TEMPLATE = app
CONFIG += debug_and_release
DESTDIR = ./

CONFIG(debug, debug|release){
  TARGET = shadercap_d
  OBJECTS_DIR = obj/release
  MOC_DIR = moc/release
}

CONFIG(release, debug|release){
  TARGET = shadercap
  OBJECTS_DIR = obj/debug
  MOC_DIR = moc/debug
}

HEADERS += MainWindow.h \
           RenderSurface.h \
           CodeEditor.h

SOURCES += Shadercap.cpp \
           MainWindow.cpp \
           RenderSurface.cpp \
           CodeEditor.cpp \
           VideoEncoder.cpp \
           EbmlWriter.cpp \
           EbmlSupport.cpp

INCLUDEPATH += $$PWD

unix: LIBS += -lvpx
win32: LIBS += -lvpx

QT += core gui widgets

gcc {
  QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
  QMAKE_CXXFLAGS_RELEASE -= -O
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3
}
