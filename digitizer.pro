# Use this file to build Engauge Digitizer from source code,
# according to the instructions in the INSTALL file. For 
# hints on modifying this file, examine the .qmake.cache
# in the qt installation directory

TEMPLATE	= app
CONFIG		= qt warn_on thread debug
OBJECTS_DIR     = src/.objs
unix {
# hints for LIBS entry:
# 1) to fix solaris-g++ link errors, try appending "-lSM -lICE -ldl"
# 2) to fix ugly fonts and blocky widgets, try appending "-lfreetype -lXft -lrt"
LIBS            += -lfftw3
}
macx {
DEFINES  	+= Q_OS_MACX
LIBS            = -framework CoreFoundation -lfftw3
QMAKE_CXXFLAGS_DEBUG += -Wno-non-virtual-dtor
QMAKE_CXXFLAGS_RELEASE += -Wno-non-virtual-dtor
}
win32 {
DEFINES  	+= WIN32
INCLUDEPATH	= $$(FFTW_HOME)/include
LIBS            += -L$$(FFTW_HOME)/lib -lfftw3
OBJECTS_DIR     = src/.objs
}
HEADERS		= src/axispointdlg.h \
		  src/clipboard.h \
                  src/colorchooser.h \
		  src/coordsysdlg.h \
		  src/curvecmb.h \
		  src/defaultsettings.h \
		  src/digitdebug.h \
		  src/digitdef.h \
		  src/digitdoc.h \
		  src/digitmain.h \
		  src/digittool.h \
		  src/digitview.h \
		  src/discretize.h \
		  src/discretizedlg.h \
		  src/exportdlg.h \
		  src/geometrydlg.h \
		  src/geometrywindowitem.h \
		  src/grid.h \
		  src/gridmesh.h \
		  src/gridmeshdlg.h \
		  src/gridremoval.h \
		  src/gridremovaldlg.h \
		  src/helpwindow.h \
		  src/highlight.h \
		  src/main.h \
		  src/match.h \
		  src/matchset.h \
                  src/measurecmb.h \
		  src/mmsubs.h \
		  src/mrudocuments.h \
		  src/osxsubs.h \
		  src/point.h \
		  src/pointmatch.h \
		  src/pointmatchdlg.h \
		  src/pointmatchthread.h \
		  src/pointset.h \
		  src/pointsetdlg.h \
		  src/pointsets.h \
                  src/pointsetnamedlg.h \
                  src/pointsetsdlg.h \
		  src/pointsetstyles.h \
                  src/scalebardlg.h \
                  src/scanforaxes.h \
		  src/segment.h \
		  src/segmentdlg.h \
		  src/segments.h \
		  src/sessionsdlg.h \
		  src/transform.h \
		  src/zoomcmb.h
SOURCES		= src/axispointdlg.cpp \
		  src/clipboard.cpp \
                  src/colorchooser.cpp \
		  src/coordsysdlg.cpp \
		  src/curvecmb.cpp \
		  src/defaultsettings.cpp \
		  src/digitdebug.cpp \
		  src/digitdef.cpp \
		  src/digitdoc.cpp \
		  src/digitmain.cpp \
		  src/digittool.cpp \
		  src/digitview.cpp \
		  src/discretize.cpp \
		  src/discretizedlg.cpp \
		  src/exportdlg.cpp \
		  src/geometrydlg.cpp \
		  src/geometrywindowitem.cpp \
		  src/grid.cpp \
		  src/gridmesh.cpp \
		  src/gridmeshdlg.cpp \
		  src/gridremoval.cpp \
		  src/gridremovaldlg.cpp \
		  src/helpwindow.cpp \
		  src/highlight.cpp \
		  src/main.cpp \
		  src/match.cpp \
		  src/matchset.cpp \
                  src/measurecmb.cpp \
		  src/mmsubs.cpp \
		  src/mrudocuments.cpp \
		  src/osxsubs.cpp \
		  src/point.cpp \
		  src/pointmatch.cpp \
		  src/pointmatchdlg.cpp \
		  src/pointmatchthread.cpp \
		  src/pointset.cpp \
		  src/pointsetdlg.cpp \
                  src/pointsetnamedlg.cpp \
		  src/pointsets.cpp \
                  src/pointsetsdlg.cpp \
		  src/pointsetstyles.cpp \
                  src/scalebardlg.cpp \
                  src/scanforaxes.cpp \
		  src/segment.cpp \
		  src/segmentdlg.cpp \
		  src/segments.cpp \
		  src/sessionsdlg.cpp \
		  src/transform.cpp \
		  src/zoomcmb.cpp
TARGET		= bin/engauge
# pt_br is portugese brazilian
# tr is turkish
TRANSLATIONS	= engauge_pt_br.ts \
		  engauge_tr.ts
MOC_DIR = src/.moc
#qt3support was inserted by qt3to4. qt3support in turn requires network
QT +=  qt3support network
