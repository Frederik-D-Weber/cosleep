# COsleep
 
Closed- and Open-loop (Slow Oscillations) sleep stimulation (auditory) or recording in _full_-PSG using OpenBCI for sleep.

see more **[infos for settings here](https://www.spisop.org/cosleep/)**

**[A poster with pictures and infographics, lists'n'stuff](https://drive.google.com/open?id=15XmB4hGwDl6L_oVv34uby7bRQaTBXUhu)**


* Its a **fun project**, but with hard science behind in hardware, software and concept, usable for research.
* Different modes: e.g. Stimulation, Recording only (just PSG, no stimulation), record on SD card without real-time monitoring/stimulation
* Setup for OpenBCI sleep recording (125 or 250 Hz sampling rate, 8 or 16 channels, incl. EEG, EOG, EMG and ECG) [here](https://www.spisop.org/openbci/)
* Build your own sleep lab for under a $1000 and do research-grade recording and reactivation protocols.
* Its free, its open and its closed-loop too, at least for slow waves/slow oscillations.
* Works and tested with all Firmwares for OpenBCI device: v1 v2 v3
* Use your own Electrode Montage (for 8 and 16 channel) stored in a *.tsv to change channel setup, display in the GUI and algorithm
* Targeted & Untargeted Auditory Stimuli Presentation using Configurable Stimuli Playlists (easy to create and mix).
* Reproduce all the published research in this area, e.g. Targeted Memory Reactivation (TMR, e.g Rudoy et al. 2009) during sleep, or Slow wave enhancement (Ngo et al. 2013)
* Stimulate during a specific time point of a slow oscillation, and trigger another stimuli after that.
* Stimulation is activated manually, but disengaged automatically on arousal.
* See live ERPs from stimulation
* Latencies considered in stimulation (below 10 ms signal latency, auditory delay down to 24 ms with normal hardware under 3 ms with specialized sound card, default is 93 ms)
* 100% logging what you did, all markers, Lights-off on, checklist for eye movements
* Lost samples are imputed and logged.
* works with parallel microSD card recording (250 Hz)
* Warning on SD card problems on device
* Handles all kinds of auditory stimuli, peeps, noises, voices and baaaams! (as long as in wav or mp3 format, or what [_sox_ can handle](http://sox.sourceforge.net/soxformat.html))
* Detect accurately your **hearing threshold** on any PC with Linux (alsa) down to the deciBel, and stimulate accurately as well with online-adjustments of volume.
* Full unfiltered BDF+ (24bit) & CSV export or pre-filtered (High-pass, can choose cut-off frequency, Butterworth, order 1 sample) EDF+(16bit) (live recording, and including all markers)
* Obfuscate your Sham/Stim condition for blinding your experiment.
* Realtime-view, scalable channels, with spindle highlighting.
* Flexible re-referencing
* Simulation of previous Recording for testing and trying things (even without an OpenBCI device)
* Default settings for stimulation in healthy school children (5-12 yrs.) and adults (5-35 yrs.)
* ... lots of other stuff

**Tutorial in the making, otherwise ask the author.**
# Source
[software/installation/source_python](https://github.com/Frederik-D-Weber/cosleep/tree/master/software/installation/source_python)

# External Download
1. [Example Data](https://drive.google.com/open?id=1lG-Q-U_NJ-pon1OYjL4bfOzeRWntyBL9) for testing the simulation (FREDDY is 8 channel, JINGYI is 16 channel data, settings described in [here](https://www.spisop.org/openbci/)
1. [Example Stimuli (preprocessed)](https://drive.google.com/open?id=1LlGVS8i8-biWbdamWvwoxJH3bp0wo1Ch) for testing the open and closed loop stimulation of voice and tones. Example protocols for those stimuli see in the subfolder [stimulations](http://github.com/Frederik-D-Weber/cosleep/software/installation/source_python/stimulations)
1. [Binaries, Ubuntu 18.04 LTS x64](https://drive.google.com/open?id=1BkZig25DqL_edzowbonLJYWGkWO9WNGj) compiled using pyinstaller
1. [Proof of concept learning Chinese in sleep files for ANKI](https://drive.google.com/open?id=1tC-79FaSCWTRyq7YmemzdTrxar4xp6q6) if you like to learn more chinese.

# Requirements
## EEG
1. OpenBCI Cyton board (optionally a daisy module) v1 v2 v3
1. EEG electrodes
1. Battery/Power pack
1. Other 'stuff' to record polysomnography
1. Motivation to built a little (you can do it!)
...see tutorial [here](https://www.spisop.org/openbci/)

## PC (minimum recommendation)
* Dual core 1.2 Ghz
* 1.5 GB RAM
* 5 GByte free disk space
* USB 2.0 port
* Audio jack (wireless audio for stimulation not recommended, due to delay)
(e.g. a Rasperry Pi3, Pine64, Rock64, etc. are fine too)

## OS requirement
* Linux
    * Ubuntu 18.04 LTS (**tested**), recompiled also tested under Ubunutu 14.04 LTS
    * Debian 9 if source code is recompiled (**tested**)
* (optional, but recommended) _low-latency_ kernel preferred (will automatically installed with the _deploy_ scripts)
* [READY ON THE USB-STICK](https://github.com/Frederik-D-Weber/cosleep/releases/tag/v0.2.0-USB): setup free USB stick image for bootup in custom Debian live version on any PC soon to follow.

## Tested PCs, compiled (pyinstaller)
* Lenovo Thinkpad X220, Intel Core i7-2640M, 16 GByte RAM, fast SSD, Ubuntu 18.04 LTS
* Lenovo Thinkpad X220, Intel Core i5-2520M, 16 GByte RAM, fast SSD, Ubuntu 14.04 LTS
* Dell Laptitude, Ubuntu 18.04 LTS

## Tested PCs, deployed (recompiled (pyinstaller))
* Lenovo Thinkpad X220, Intel Core i7-2640M, 16 GByte RAM, fast SSD, Ubuntu 18.04 LTS
* Lenovo Thinkpad X220, Intel Core i5-2520M, 16 GByte RAM, fast SSD, Ubuntu 14.04 LTS
* Dell Laptitude, Ubuntu 18.04 LTS
* Debian stretch (9, stable)
