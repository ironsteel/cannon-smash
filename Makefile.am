pkgdatadir = $(datadir)/games/@PACKAGE@
localedir = $(prefix)/share/locale

bin_PROGRAMS = csmash
csmash_SOURCES = main.cpp Launcher.cpp LobbyClient.cpp Ball.cpp Event.cpp \
		Control.cpp View.cpp BaseView.cpp FieldView.cpp BallView.cpp \
		Player.cpp comPlayer.cpp parts.cpp PlayerView.cpp HitMark.cpp \
		PenAttack.cpp ShakeCut.cpp PenDrive.cpp comPenAttack.cpp \
		comShakeCut.cpp comPenDrive.cpp ComTrainingPenAttack.cpp \
		TrainingPenAttack.cpp ComTrainingPenDrive.cpp \
		TrainingPenDrive.cpp NetPenAttack.cpp NetPenDrive.cpp \
		NetShakeCut.cpp PlayerSelect.cpp PlayerSelectView.cpp \
		TrainingSelect.cpp TrainingSelectView.cpp PlayGame.cpp \
		SoloPlay.cpp PracticePlay.cpp PlayGameView.cpp Title.cpp \
		TitleView.cpp Howto.cpp HowtoView.cpp Training.cpp \
		TrainingView.cpp Opening.cpp OpeningView.cpp Sound.cpp \
		LoadImage.cpp z.cpp MultiPlay.cpp Network.cpp MenuItem.cpp \
		MenuItemView.cpp Logging.cpp affine float matrix xerror.cpp \
		PracticeSelect.cpp PracticeSelectView.cpp BaseView2D.cpp \
		FieldView2D.cpp PlayerView2D.cpp BallView2D.cpp \
		PlayerSelectView2D.cpp TitleView2D.cpp MenuItemView2D.cpp \
		RCFile.cpp loadparts.cpp LobbyClientView.cpp LauncherView.cpp \
		MultiPlayerSelect.cpp ttinc.cpp \
		ttinc.h Launcher.h LobbyClient.h Ball.h Event.h Control.h \
		View.h BaseView.h FieldView.h BallView.h Player.h comPlayer.h \
		parts.h PlayerView.h HitMark.h PenAttack.h ShakeCut.h \
		PenDrive.h comPenAttack.h comShakeCut.h comPenDrive.h \
		ComTrainingPenAttack.h TrainingPenAttack.h \
		ComTrainingPenDrive.h TrainingPenDrive.h NetPenAttack.h \
		NetPenDrive.h NetShakeCut.h PlayerSelect.h PlayerSelectView.h \
		TrainingSelect.h TrainingSelectView.h PlayGame.h SoloPlay.h \
		PracticePlay.h PlayGameView.h Title.h TitleView.h Howto.h \
		HowtoView.h Training.h TrainingView.h Opening.h OpeningView.h \
		Sound.h LoadImage.h z.h MultiPlay.h Network.h MenuItem.h \
		MenuItemView.h Logging.h PracticeSelect.h \
		PracticeSelectView.h BaseView2D.h FieldView2D.h \
		PlayerView2D.h BallView2D.h PlayerSelectView2D.h \
		TitleView2D.h MenuItemView2D.h RCFile.h loadparts.h \
		LobbyClientView.h LauncherView.h MultiPlayerSelect.h 

DEFS = -DLOCALEDIR=\"$(localedir)\" -DCANNONSMASH_DATADIR=\"$(pkgdatadir)\" @DEFS@
LIBS = @LIBINTL@ @LIBS@
#INCLUDES = $(INCLUDES) -I../intl -I$(top_srcdir)/intl

SUBDIRS = m4  Parts images wav win32 po
EXTRA_DIST = config.rpath  config.rpath  README.en README.nt Makefile.nt CREDITS csmash.spec

ACLOCAL_AMFLAGS = -I m4
