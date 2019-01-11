# COsleep
 
Closed- and Open-loop (Slow Ocillations) sleep stimulation (auditory) or recording in _full_-PSG using OpenBCI for sleep.

* Its a **fun project**, but with hard science behind in hardware, software and concept, usable for research.
* Different modes: e.g. Stimulation, Recording only (just PSG, no stimulation), record on SD card without real-time monitoring/stimulation
* Setup for OpenBCI sleep recording (125 or 250 Hz sampling rate, 8 or 16 channels, incl. EEG, EOG, EMG and ECG) [here](https://www.spisop.org/openbci/)
* Build your own sleep lab for under a $1000 and do research-grade recording and reactivation protocols.
* Its free, its open and its closed-loop too, at least for slow waves/slow oscillations.
* Targeted & Untargeted Auditory Stimuli Presentation using Configurable Stimuli Playlists (easy to create and mix).
* Reproduce all the published research in this area, e.g. Targeted Memory Reactivation (TMR, e.g Rudoy et al. 2009) during sleep, or Slow wave enhancement (Ngo et al. 2013)
* Stimulate during a specific time point of a slow oscillation, and trigger another stimuli after that.
* Stimulation is activated manually, but disengaged automatically on arousal.
* See live ERPs from stimulation
* Latencies considered in stimulation (below 10 ms signal latency, auditory delay down to 24 ms, standard 93 ms)
* 100% logging what you did, all markers, Lights-off on, checklist for eye movements
* Lost samples are imputed and logged.
* works with parallel microSD card recording (250 Hz)
* Handles all kinds of auditory stimuli, peeps, noises, voices and baaaams! (as long as in wav or mp3 format, or what [_sox_ can handle](http://sox.sourceforge.net/soxformat.html))
* Detect accurately your **hearing threshold** on any PC with Linux (alsa) down to the deciBel, and stimulate accurately as well with online-adjustments of volume.
* FUll unfiltered BDF & CSV export (live, and including all markers)
* Obfuscate your sham/stim condition for blinding your experiment.
* Realtime-view, scalable channels, with spindle highlighting.
* Flexible rereferencing
* ... lots of stuff, see the poster:
**[A poster with pictures and infographics, lists'n'stuff](https://drive.google.com/open?id=15XmB4hGwDl6L_oVv34uby7bRQaTBXUhu)**

**Tutorial in the making, otherwise ask the author.**

#External Download
1. [Example Data](https://drive.google.com/open?id=1lG-Q-U_NJ-pon1OYjL4bfOzeRWntyBL9) for testing the simulation (FREDDY is 8 channel, JINGYI is 16 channel data, settings described in [here](https://www.spisop.org/openbci/)
1. [Example Stimuli (preprocessed)](https://drive.google.com/open?id=1LlGVS8i8-biWbdamWvwoxJH3bp0wo1Ch) for testing the open and closed loop stimulation of voice and tones. Example protocols for those stimuli see in the subfolder [stimulations](http://github.com/Frederik-D-Weber/cosleep/software/installation/source_python/stimulations)
1. [Binaries, Ubuntu 18.04 LTS x64](https://drive.google.com/open?id=1BkZig25DqL_edzowbonLJYWGkWO9WNGj) compiled using pyinstaller
1. [Proof of concept learning Chinese in sleep files for ANKI](https://drive.google.com/open?id=1tC-79FaSCWTRyq7YmemzdTrxar4xp6q6) if you like to learn more chinese.

# Requirements
## EEG
1. OpenBCI Cyton board (optionally a daisy module)
1. EEG electrodes
1. Battery/Power pack
1. Other stuff to record polysomnography
1. Motivation to built a little (you can do it!)
...see tutorial [here](https://www.spisop.org/openbci/)

## PC (minimum recommendation)
* Dual core 1.2 Ghz
* 1.5 GB RAM
* 5 GByte free disk space
* USB 2.0 port
* Audio jack (wireless audio due to delay not recommended)
(e.g. a Rasperry Pi3, Pine64, Rock64, etc. are fine too)

## OS (preferred)
* Linux, e.g. Ubuntu 12.XX and newer versions it was **tested** under Ubunutu 14.04 LTS and 18.04 LTS) 
* _low-latency_ kernel preferred (will automatically installed with the _deploy_ scripts)

## Tested PCs, deploy, run and compiled (pyinstaller)
* Lenovo Thinkpad X220, Intel Core i7-2640M, 16 GByte RAM, fast SSD, Ubuntu 18.04 LTS
* Lenovo Thinkpad X220, Intel Core i5-2520M, 16 GByte RAM, fast SSD, Ubuntu 14.04 LTS
* Dell Laptitude, Ubuntu 18.04 LTS
