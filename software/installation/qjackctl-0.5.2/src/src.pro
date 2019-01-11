# qjackctl.pro
#
TARGET = qjackctl

TEMPLATE = app
INCLUDEPATH += .

include(src.pri)

#DEFINES += DEBUG

HEADERS += config.h \
	qjackctlAbout.h \
	qjackctlAlsaConnect.h \
	qjackctlAlsaGraph.h \
	qjackctlConnect.h \
	qjackctlConnectAlias.h \
	qjackctlGraph.h \
	qjackctlInterfaceComboBox.h \
	qjackctlJackConnect.h \
	qjackctlJackGraph.h \
	qjackctlPatchbay.h \
	qjackctlPatchbayFile.h \
	qjackctlPatchbayRack.h \
	qjackctlSession.h \
	qjackctlSetup.h \
	qjackctlStatus.h \
	qjackctlSystemTray.h \
	qjackctlAboutForm.h \
	qjackctlConnectionsForm.h \
	qjackctlGraphForm.h \
	qjackctlMainForm.h \
	qjackctlMessagesStatusForm.h \
	qjackctlPatchbayForm.h \
	qjackctlSessionForm.h \
	qjackctlSetupForm.h \
	qjackctlSocketForm.h

SOURCES += \
	qjackctl.cpp \
	qjackctlAlsaConnect.cpp \
	qjackctlAlsaGraph.cpp \
	qjackctlConnect.cpp \
	qjackctlConnectAlias.cpp \
	qjackctlGraph.cpp \
	qjackctlInterfaceComboBox.cpp \
	qjackctlJackConnect.cpp \
	qjackctlJackGraph.cpp \
	qjackctlPatchbay.cpp \
	qjackctlPatchbayFile.cpp \
	qjackctlPatchbayRack.cpp \
	qjackctlSession.cpp \
	qjackctlSetup.cpp \
	qjackctlSystemTray.cpp \
	qjackctlAboutForm.cpp \
	qjackctlConnectionsForm.cpp \
	qjackctlGraphForm.cpp \
	qjackctlMainForm.cpp \
	qjackctlMessagesStatusForm.cpp \
	qjackctlPatchbayForm.cpp \
	qjackctlSessionForm.cpp \
	qjackctlSetupForm.cpp \
	qjackctlSocketForm.cpp

FORMS += \
	qjackctlAboutForm.ui \
	qjackctlConnectionsForm.ui \
	qjackctlGraphForm.ui \
	qjackctlMainForm.ui \
	qjackctlMessagesStatusForm.ui \
	qjackctlPatchbayForm.ui \
	qjackctlSessionForm.ui \
	qjackctlSetupForm.ui \
	qjackctlSocketForm.ui

RESOURCES += \
	qjackctl.qrc


TRANSLATIONS += \
	translations/qjackctl_cs.ts \
	translations/qjackctl_de.ts \
	translations/qjackctl_es.ts \
	translations/qjackctl_fr.ts \
	translations/qjackctl_it.ts \
	translations/qjackctl_ja.ts \
	translations/qjackctl_nl.ts \
	translations/qjackctl_ru.ts


unix {

	# variables
	OBJECTS_DIR = .obj
	MOC_DIR     = .moc
	UI_DIR      = .ui

	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	isEmpty(BINDIR) {
		BINDIR = $${PREFIX}/bin
	}

	isEmpty(DATADIR) {
		DATADIR = $${PREFIX}/share
	}

	#DEFINES += DATADIR=\"$${DATADIR}\"

	# make install
	INSTALLS += target desktop icon appdata

	target.path = $${BINDIR}

	desktop.path = $${DATADIR}/applications
	desktop.files += $${TARGET}.desktop

	icon.path = $${DATADIR}/icons/hicolor/32x32/apps
	icon.files += images/$${TARGET}.png 

	appdata.path = $${DATADIR}/metainfo
	appdata.files += appdata/$${TARGET}.appdata.xml
}

# XML/DOM support
QT += xml

# QT5 support
!lessThan(QT_MAJOR_VERSION, 5) {
	QT += widgets
}

win32 {
	CONFIG += static
}
