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
           CodeEditor.h \
           VideoOptions.h \
           VideoProgress.h \
           VideoParameters.h

SOURCES += Shadercap.cpp \
           MainWindow.cpp \
           RenderSurface.cpp \
           CodeEditor.cpp \
           VideoEncoder.cpp \
           EbmlWriter.cpp \
           EbmlSupport.cpp \
           ShaderParameter.cpp \
           VideoOptions.cpp \
           VideoProgress.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += "./glslang/glslang/MachineIndependent/"
INCLUDEPATH += "./glslang/glslang/"
INCLUDEPATH += "./glslang/OGLCompilersDLL/"
unix: INCLUDEPATH += "./glslang/glslang/OSDependent/Linux/"
win32: INCLUDEPATH += "./glslang/glslang/OSDependent/Windows/"

unix: LIBS += -lvpx
unix: LIBS += -L"./glslang/glslang/" -lglslang
unix: LIBS += -L"./glslang/glslang/OSDependent/Linux/" -lOSDependent 
unix: LIBS += -L"./glslang/OGLCompilersDLL/" -lOGLCompiler

# Windows libs must be configured manually in Visual Studio
win32: LIBS += -lvpx

QT += core gui widgets

gcc {
  QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-unused-variable -Wunused-parameter
  QMAKE_CXXFLAGS_RELEASE -= -O
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3
}
