TEMPLATE	= app
CONFIG		= qt warn_on debug thread
macx {
DEFINES  	+= Q_OS_MACX
LIBS            += -framework CoreFoundation
}
win32 {
DEFINES  	+= WIN32
CONFIG          += console
}
HEADERS		= matrix.h
SOURCES		= transpose.cpp
TARGET		= transpose
TRANSLATIONS	= 
