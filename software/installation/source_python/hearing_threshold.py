import sys
import pygame
#import time
from pygame.locals import *
import datetime
#import subprocess

from PyQt4 import QtGui, QtCore # (the example applies equally well to PySide)
#import pyqtgraph as pg

import alsaaudio
from Dialogs import Dialogs
import numpy as np

soundBufferSize = 4096  # power of 2, divided by the sampling rate of 44100 Hz gives the delay 1024 is possible with good drivers


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    dialogApp = Dialogs()

    #set the master volume to max
    masterVolumePercent = int(100)
    # if (masterVolumePercent <= 100) and (masterVolumePercent >= 0):
    #      subprocess.call(["amixer", "-D", "pulse", "sset", "Master", str(masterVolumePercent) + "%"])
    # get PCH output card and Master Control for mixer selection
    alsaaudio.Mixer(control="Master", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).setvolume(
        masterVolumePercent)
    # get PCH output card and Master Control for mixer selection
    alsaaudio.Mixer(control="PCM", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).setvolume(
        masterVolumePercent)

    # alsaaudio.Mixer("Master").setvolume(masterVolumePercent)
    # alsaaudio.Mixer("PCM").setvolume(masterVolumePercent)
    # mpcm.setvolume(masterVolumePercent)


    # print alsaaudio.mixers()

    subject, okpressed = dialogApp.getText("Provide Subject ID","Subject","subj1")
    subject = str(subject).strip()

    if not okpressed or subject=="":
        sys.exit(0)

    volumestep0 = 0.01
    volumestep1 = 0.05
    volumestep2 = 0.1

    def changeVolume(vol, doIncNotDec, volumestep):
        vol = vol + volumestep if doIncNotDec else vol - volumestep
        #vol = round(vol,5)
        if vol > 0.9:
            vol = 0.9
        if vol < 0.1:
            vol = 0.1
        return(vol)


    def changeMasterVolume(vol, ch):
        vol = vol + ch
        # vol = round(vol,5)
        if vol > 100:
            vol = 100
        if vol < 1:
            vol = 1
        return (int(vol))


    sound_base_levels_dB = (-72, -60, -48, -36, -24)

    sound_base_level_index = 0

    def changeSoundBaseLevel(sbli, doIncNotDec):
        sbli = sbli + 1 if doIncNotDec else sbli - 1
        #vol = round(vol,5)
        if sbli > len(sound_base_levels_dB)-1:
            sbli = len(sound_base_levels_dB) - 1
        if sbli < 0:
            sbli = 0
        return(sbli)


    togglePlay = True


    pygame.init()
    screen_width = 800
    screen = pygame.display.set_mode((screen_width, 600))
    screen.fill((0, 0, 0))

    pygame.display.set_caption('Hearing threshold test - by Frederik D. Weber')
    pygame.mouse.set_visible(0)

    clock = pygame.time.Clock()

    # reduce the buffer from 4096 to 1024 (32 does not work properly on thinkpad x220 to be below 100 ms buffer delay
    # pygame.mixer.pre_init(frequency=441000, size=-16, channels=1, buffer=4096)
    pygame.mixer.pre_init(frequency=44100, size=-16, channels=2, buffer=soundBufferSize)
    # init() and pre_init() channels refers to mono vs stereo, not playback Channel object

    pygame.mixer.init()


    # create separate Channel objects for simultaneous playback

    # channel_sound_trigger = pygame.mixer.Channel(0) # argument must be int
    channel_sound = pygame.mixer.Channel(0)

    sound_full_volume = pygame.mixer.Sound(file="stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav.44.1kHz.16bit.integer.full.db.minus24.db.wav")
    sound_high_volume = pygame.mixer.Sound(file="stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav.44.1kHz.16bit.integer.full.db.minus36.db.wav")
    sound_medium_volume = pygame.mixer.Sound(file="stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav.44.1kHz.16bit.integer.full.db.minus48.db.wav")
    sound_low_volume = pygame.mixer.Sound(file="stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav.44.1kHz.16bit.integer.full.db.minus60.db.wav")
    sound_lowest_volume = pygame.mixer.Sound(file="stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav.44.1kHz.16bit.integer.full.db.minus72.db.wav")
    #sound_floor_volume = pygame.mixer.Sound(file="stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav.44.1kHz.16bit.integer.full.db.minus84.db.wav")

    # channel_sound_trigger.set_volume(0.2)
    channel_sound.set_volume(0.2)

    #pygame.mixer.music.set_volume(0.0)

    pygame.font.init()
    myfont = pygame.font.SysFont('monospace', 30)

    textsurfaceInstructions00 = myfont.render("Turn up system volume to Maximum!", True, (120, 120, 120))
    textsurfaceInstructions0 = myfont.render("I or J key: " + str(volumestep0), True, (120, 120, 120))
    textsurfaceInstructions1 = myfont.render("UP or DOWN key: " + str(volumestep1), True, (120, 120, 120))
    textsurfaceInstructions2 = myfont.render("H or U key: " + str(volumestep2), True, (120, 120, 120))



    done = False

    #try:
    while not done:

        # vol = pygame.mixer.music.get_volume()
        vol = channel_sound.get_volume()
        masterVolumePercent = alsaaudio.Mixer(control="Master", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).getvolume()
        masterVolumePercent = int(masterVolumePercent[0])
        masterVolumePercent = changeMasterVolume(masterVolumePercent,0)
        alsaaudio.Mixer(control="Master", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).setvolume(masterVolumePercent)


        screen.fill((0, 0, 0))

        screen.blit(textsurfaceInstructions00,
                    (screen_width/2 - textsurfaceInstructions00.get_width() // 2, 50 - textsurfaceInstructions00.get_height() // 2))
        screen.blit(textsurfaceInstructions0,
                    (screen_width/2 - textsurfaceInstructions0.get_width() // 2, 100 - textsurfaceInstructions0.get_height() // 2))
        screen.blit(textsurfaceInstructions1,
                    (screen_width/2 - textsurfaceInstructions1.get_width() // 2, 150 - textsurfaceInstructions1.get_height() // 2))
        screen.blit(textsurfaceInstructions2,
                    (screen_width/2 - textsurfaceInstructions2.get_width() // 2, 200 - textsurfaceInstructions2.get_height() // 2))

        textsurfaceInstructions3 = myfont.render(
            "sound base level, Q or A key: " + str(sound_base_levels_dB[sound_base_level_index]) + " dB", True,
            (120, 120, 120))
        textsurfaceInstructions4 = myfont.render(
            "sound Play, Enter key: " + ("ON" if togglePlay else "OFF"), True,
            (120, 120, 120))

        screen.blit(textsurfaceInstructions3,
                    (screen_width/2  - textsurfaceInstructions3.get_width() // 2, 250 - textsurfaceInstructions3.get_height() // 2))
        screen.blit(textsurfaceInstructions4,
                    (screen_width/2  - textsurfaceInstructions4.get_width() // 2, 300 - textsurfaceInstructions4.get_height() // 2))

        textsurfaceMaster = myfont.render("Master Volume, W or S key: " + str(masterVolumePercent) + "%", True, (200, 200, 200))
        screen.blit(textsurfaceMaster, (screen_width/2  - textsurfaceMaster.get_width() // 2, 450 - textsurfaceMaster.get_height() // 2))

        textsurface = myfont.render("Volume: " + str(round(vol, 5)), True, (200, 200, 200))
        screen.blit(textsurface, (screen_width / 2 - textsurface.get_width() // 2, 500 - textsurface.get_height() // 2))

        textExit = myfont.render("Play with SPACE (exit & save with ESC)", True, (66, 66, 66))
        screen.blit(textExit, (screen_width / 2 - textExit.get_width() // 2, 550 - textExit.get_height() // 2))

        pygame.display.update()
        clock.tick(60)

        for event in pygame.event.get():
            if event.type == KEYDOWN:
                play=False

                if event.key == pygame.K_i:
                    #pygame.mixer.music.set_volume(changeVolume(vol, True, volumestep0))
                    channel_sound.set_volume(changeVolume(vol, True, volumestep0))
                    play = togglePlay
                if event.key == pygame.K_j:
                    channel_sound.set_volume(changeVolume(vol, False, volumestep0))
                    play = togglePlay
                elif event.key == pygame.K_UP:
                    channel_sound.set_volume(changeVolume(vol, True, volumestep1))
                    # pygame.mixer.music.set_volume(changeVolume(vol, True, volumestep1))
                    play = togglePlay
                elif event.key == pygame.K_DOWN:
                    channel_sound.set_volume(changeVolume(vol, False, volumestep1))
                    play = togglePlay
                elif event.key == pygame.K_u:
                    channel_sound.set_volume(changeVolume(vol, True, volumestep2))
                    play = togglePlay
                elif event.key == pygame.K_h:
                    channel_sound.set_volume(changeVolume(vol, False, volumestep2))
                    play = togglePlay
                elif event.key == pygame.K_w:
                    alsaaudio.Mixer(control="Master", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).setvolume(changeMasterVolume(masterVolumePercent, 2))
                    play = togglePlay
                elif event.key == pygame.K_s:
                    alsaaudio.Mixer(control="Master", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).setvolume(changeMasterVolume(masterVolumePercent, -2))
                    play = togglePlay
                elif event.key == pygame.K_q:
                    sound_base_level_index = changeSoundBaseLevel(sound_base_level_index, True)
                    channel_sound.set_volume(changeVolume(vol, False, 1))
                    play = togglePlay
                    # channel_sound.set_volume(0.0)
                elif event.key == pygame.K_a:
                    sound_base_level_index = changeSoundBaseLevel(sound_base_level_index, False)
                    channel_sound.set_volume(changeVolume(vol, True, 1))
                    play = togglePlay
                    # channel_sound.set_volume(0.0)
                elif event.key == pygame.K_RETURN:
                    togglePlay = not togglePlay
                elif event.key == pygame.K_SPACE:
                    play = togglePlay
                elif (event.key == pygame.K_ESCAPE) or (event.key == pygame.QUIT):
                    done = True
                #pygame.mixer.music.set_volume(0.00782) # lowest possible volume
                #pygame.mixer.music.play()
                if play:
                    # if sound_base_level_index==0:
                    #     channel_sound.play(sound_floor_volume, loops=0)
                    if sound_base_level_index==0:
                        channel_sound.play(sound_lowest_volume, loops=0)
                    elif sound_base_level_index==1:
                        channel_sound.play(sound_low_volume, loops=0)
                    elif sound_base_level_index == 2:
                        channel_sound.play(sound_medium_volume, loops=0)
                    elif sound_base_level_index == 3:
                        channel_sound.play(sound_high_volume, loops=0)
                    elif sound_base_level_index == 4:
                        channel_sound.play(sound_full_volume, loops=0)

                        #channel_sound_trigger.play(sound_trigger, loops=0)

                        #time.sleep(0.1)

                    #except Exception as e: print(e)
                    #finally:

    masterVolumePercent = int(alsaaudio.Mixer(control="Master", cardindex=np.where(np.array(alsaaudio.cards()) == u'PCH')[0][0]).getvolume()[0])

    result_text = \
    "subject: " + subject + "\n" +\
    "final sound base level was: " + str(sound_base_levels_dB[sound_base_level_index]) + " dB" + "\n" +\
    "final volume was          : " + str(channel_sound.get_volume()) + "\n" +\
    "master volume is          : " + str(masterVolumePercent) + "%\n"

    dialogApp.showMessageBox("Results:", result_text, False, False, False, True)

    print(result_text)

    datetimenow = datetime.datetime.today()
    with open("data/subject/" + subject + ".volume_settings.txt", "a") as f:
        f.write("subject,datetime,sound_base_level_dB,volume_final,"+\
                "final_master_volume_of_system"+ "\n")
        writeLine = subject + "," + str(datetimenow) + "," + \
                    str(sound_base_levels_dB[sound_base_level_index]) + "," + \
                    str(channel_sound.get_volume()) + "," + \
                    str(masterVolumePercent)

        f.write(writeLine + "\n")
    f.close()
    print("saved in file " + "data/subject/" + subject + ".volume_settings.txt")

    # sys.exit(app.exec_())
    pygame.quit()
    sys.exit(0)
