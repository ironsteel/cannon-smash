pkgdatadir = $(datadir)/games/@PACKAGE@/images

pkgdata_ppm = Bad.ppm Keyboard.ppm Nice.ppm Title.ppm \
		eight.ppm five.ppm four.ppm nine.ppm one.ppm seven.ppm \
		six.ppm three.ppm two.ppm zero.ppm Max.ppm \
		EasyTutorial.ja.ppm EasyTutorial.de.ppm EasyTutorial.fr.ppm EasyTutorial.ppm \
		NormalTutorial.ja.ppm NormalTutorial.de.ppm NormalTutorial.fr.ppm NormalTutorial.ppm \
		HardTutorial.ja.ppm HardTutorial.de.ppm HardTutorial.fr.ppm HardTutorial.ppm \
		11point.ja.pbm 11point.de.pbm 11point.fr.pbm 11point.pbm \
		21point.ja.pbm 21point.de.pbm 21point.fr.pbm 21point.pbm \
		5point.ja.pbm 5point.de.pbm 5point.fr.pbm 5point.pbm \
		ArctanX.ja.pbm ArctanX.de.pbm ArctanX.fr.pbm ArctanX.pbm \
		Config.ja.pbm Config.de.pbm Config.fr.pbm Config.pbm \
		Easy.ja.pbm Easy.de.pbm Easy.fr.pbm Easy.pbm \
		Hard.ja.pbm Hard.de.pbm Hard.fr.pbm Hard.pbm \
		Howto.ja.pbm Howto.de.pbm Howto.fr.pbm Howto.pbm \
		Howto1.ja.pbm Howto1.de.pbm Howto1.fr.pbm Howto1.pbm \
		Howto2.ja.pbm Howto2.de.pbm Howto2.fr.pbm Howto2.pbm \
		Howto3.ja.pbm Howto3.de.pbm Howto3.fr.pbm Howto3.pbm \
		Howto4.ja.pbm Howto4.de.pbm Howto4.fr.pbm Howto4.pbm \
		Howto5.ja.pbm Howto5.de.pbm Howto5.fr.pbm Howto5.pbm \
		Howto6.ja.pbm Howto6.de.pbm Howto6.fr.pbm Howto6.pbm \
		LevelSelect.ja.pbm LevelSelect.de.pbm LevelSelect.fr.pbm LevelSelect.pbm \
		ModeSelect.ja.pbm ModeSelect.de.pbm ModeSelect.fr.pbm ModeSelect.pbm \
		Normal.ja.pbm Normal.de.pbm Normal.fr.pbm Normal.pbm \
		Pause.ja.pbm Pause.de.pbm Pause.fr.pbm Pause.pbm \
		Practice.ja.pbm Practice.de.pbm Practice.fr.pbm Practice.pbm \
		Quit.ja.pbm Quit.de.pbm Quit.fr.pbm Quit.pbm \
		SelectPlayer.ja.pbm SelectPlayer.de.pbm SelectPlayer.fr.pbm SelectPlayer.pbm \
		StartGame.ja.pbm StartGame.de.pbm StartGame.fr.pbm StartGame.pbm \
		Training.ja.pbm Training.de.pbm Training.fr.pbm Training.pbm \
		Transparent.ja.pbm Transparent.de.pbm Transparent.fr.pbm Transparent.pbm \
		Tsuborish.ja.pbm Tsuborish.de.pbm Tsuborish.fr.pbm Tsuborish.pbm \
		WireFrame.ja.pbm WireFrame.de.pbm WireFrame.fr.pbm WireFrame.pbm \
		ArmOnly.ja.pbm ArmOnly.de.pbm ArmOnly.fr.pbm ArmOnly.pbm \
		downArrow.ja.pbm downArrow.de.pbm downArrow.fr.pbm downArrow.pbm \
		leftArrow.ja.pbm leftArrow.de.pbm leftArrow.fr.pbm leftArrow.pbm \
		lose.ja.pbm lose.de.pbm lose.fr.pbm lose.pbm \
		rightArrow.ja.pbm rightArrow.de.pbm rightArrow.fr.pbm rightArrow.pbm \
		upArrow.ja.pbm upArrow.de.pbm upArrow.fr.pbm upArrow.pbm \
		win.ja.pbm win.de.pbm win.fr.pbm win.pbm

pkgdata_jpg = Floor.jpg Mouse1.jpg Mouse2.jpg Mouse3.jpg Mouse4.jpg \
		PenDrive.jpg PenAttack.jpg ShakeCut.jpg \
		Front.jpg Back.jpg Left.jpg Right.jpg \
		FastRally.jpg Footwork.jpg

pkgdata_bmp = BG.bmp BGAlpha.bmp Title.bmp PenAttack.bmp PenDrive.bmp \
		ShakeCut.bmp Triangle.bmp \
		ConfigW.bmp ConfigY.bmp HowtoW.bmp HowtoY.bmp PracticeW.bmp \
		PracticeY.bmp QuitW.bmp QuitY.bmp StartGameW.bmp \
		StartGameY.bmp TrainingW.bmp TrainingY.bmp

pkgdata_DATA = $(pkgdata_ppm:=@PKGDATA@) $(pkgdata_jpg) $(pkgdata_bmp)
EXTRA_DIST = $(pkgdata_ppm) $(pkgdata_jpg) $(pkgdata_bmp)

%.ppm.gz: %.ppm
	@GZIP@ -c $< > $<.gz

%.pbm.gz: %.pbm
	@GZIP@ -c $< > $<.gz
