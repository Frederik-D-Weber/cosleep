#!/bin/bash

INPUTFOLDER=input
STIMULIFOLDER=stimuli

read -p "Output folder name:" OUTPUTFOLDER

if [ $(dpkg-query -W -f='${Status}' sox 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  sudo apt-get --force-yes --yes install sox

fi

if [ $(dpkg-query -W -f='${Status}' libsox-fmt-mp3 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  sudo apt-get --force-yes --yes install libsox-fmt-mp3
fi



while true; do
  read -p "Need to re-add file endings (e.g. files miss a .wav or .mp3 ending)? (Y/N):" INPUT
  case $INPUT in
	[Yy]* ) 
		RENAMEWITHNEWENDING=true
		break
		;;
	[Nn]* )
		RENAMEWITHNEWENDING=false
		break
		;;
	*)
		#echo "wrong input, exiting"
		#exit
		;;
  esac
done


read -p "Enter file ending (e.g. mp3, wav, ...): *." FILEENDING
FILESEARCHSTRING=*.$FILEENDING

while true; do
  read -p "Hard-Normalize audio (Y/N):" INPUT
  case $INPUT in
	[Yy]* ) 
		DOAUDIONORMALIZE=true
		break
		;;
	[Nn]* )
		DOAUDIONORMALIZE=false
		break
		;;
	*)
		#echo "wrong input, exiting"
		#exit
		;;
  esac
done

while true; do
  read -p "Remove starting silence (Y/N):" INPUT
  case $INPUT in
	[Yy]* ) 
		DOREMOVESILENCE=true
		break
		;;
	[Nn]* )
		DOREMOVESILENCE=false
		break
		;;
	*)
		#echo "wrong input, exiting"
		#exit
		;;
  esac
done

mkdir $OUTPUTFOLDER

if $RENAMEWITHNEWENDING; then
	cd $INPUTFOLDER
	for FILE in *; do 
		[[ $FILE == *.* ]] && continue
		mv -- "$FILE" "$FILE.$FILEENDING"; 
	done
	cd ..
fi

STIMULUSLISTFILENAME=${OUTPUTFOLDER}_StimulusList.txt
rm $STIMULUSLISTFILENAME

for AUDIOFILE in $INPUTFOLDER/$FILESEARCHSTRING; do
	NEWAUDIOFILENAME=$OUTPUTFOLDER/$(basename $AUDIOFILE).44.1kHz.16bit.integer.full.db
	AUDIOFILENAME=$(basename $AUDIOFILE)
	echo "\"${AUDIOFILENAME%.*}\",\"$STIMULIFOLDER/$OUTPUTFOLDER/$(basename $AUDIOFILE)\"" >> $STIMULUSLISTFILENAME

	if $DOAUDIONORMALIZE && $DOREMOVESILENCE; then
		sox $AUDIOFILE -b 16 -e signed-integer $NEWAUDIOFILENAME.wav norm vol 0.99 remix - silence 1 0.0001 1% fade 0.005 rate 44100 dither
	elif $DOAUDIONORMALIZE && ! $DOREMOVESILENCE; then
		sox $AUDIOFILE -b 16 -e signed-integer $NEWAUDIOFILENAME.wav norm vol 0.99 remix - rate 44100 dither
	elif ! $DOAUDIONORMALIZE && $DOREMOVESILENCE; then
		sox $AUDIOFILE -b 16 -e signed-integer $NEWAUDIOFILENAME.wav vol 0.99 remix - silence 1 0.0001 1% fade 0.005 rate 44100 dither
	elif ! $DOAUDIONORMALIZE && ! $DOREMOVESILENCE; then
		sox $AUDIOFILE -b 16 -e signed-integer $NEWAUDIOFILENAME.wav vol 0.99 remix - rate 44100 dither
	fi
	
	for dB in {0..84..1}; do
		sox $NEWAUDIOFILENAME.wav -b 16 -e signed-integer $NEWAUDIOFILENAME.minus$dB.db.wav gain -$dB remix - rate 44100 dither
	done
done

#cp $INPUTFOLDER/$FILESEARCHSTRING $OUTPUTFOLDER

mv $OUTPUTFOLDER ../


