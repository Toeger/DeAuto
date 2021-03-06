TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += no_lflags_merge
CONFIG += warn_off

SOURCES += \
    vardeclvisitor.cpp \
    file_edit.cpp \
    $$PWD/utility.cpp \
    $$PWD/assume.cpp

INCLUDEPATH += /usr/lib/llvm-3.9/include/

LIBS += -lboost_filesystem
LIBS += -lboost_system

LIBS += -lclangFrontend
LIBS += -lclangDriver
LIBS += -lclangCodeGen
LIBS += -lclangSema
LIBS += -lclangAnalysis
LIBS += -lclangRewrite
LIBS += -lclangParse
LIBS += -lclangLex
LIBS += -lclangARCMigrate
LIBS += -lclangEdit
LIBS += -lclangFrontendTool
LIBS += -lclangRewrite
LIBS += -lclangSerialization
LIBS += -lclangTooling
LIBS += -lclangStaticAnalyzerCheckers
LIBS += -lclangStaticAnalyzerCore
LIBS += -lclangStaticAnalyzerFrontend
LIBS += -lclangAST
LIBS += -lclangSema
LIBS += -lclangBasic
LIBS += -lclangRewriteFrontend
LIBS += -lclangASTMatchers
LIBS += -lclangToolingCore
LIBS += -lLLVM

LIBS += $$LIBS

QMAKE_CXXFLAGS += -std=c++1z
clang{
    QMAKE_CXXFLAGS += --system-header-prefix=clang #clang bugs out and assumes system files to not be system files
}
QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS #TODO: figure out why this is necessary
QMAKE_CXXFLAGS_DEBUG += -fno-omit-frame-pointer -Wall -ggdb -Wno-unused-parameter -Wno-unused-variable -Werror
QMAKE_CXXFLAGS_DEBUG += -fsanitize=undefined,address
QMAKE_CXXFLAGS_RELEASE += -Wunused-parameter -Wunused-variable -Werror
QMAKE_LFLAGS_DEBUG += -fsanitize=undefined,address
#QMAKE_LFLAGS_DEBUG += -v
QMAKE_CXXFLAGS_PROFILE += -DNDEBUG
QMAKE_CXXFLAGS_RELEASE += -O3 -DNDEBUG
gcc{
    clang{
        #clang is identified as gcc but doesn't support -flto
    }
    else{
        QMAKE_CXXFLAGS_RELEASE += -flto
        QMAKE_LFLAGS_RELEASE += -flto
    }
}

HEADERS += \
    vardeclvisitor.h \
    file_edit.h \
    $$PWD/utility.h \
    $$PWD/assume.h
