# following https://willhaley.com/blog/custom-debian-live-environment/ by Will Haley

# Prerequisites
# Install applications we need to build the environment.

sudo apt-get -y install \
    debootstrap \
    squashfs-tools \
    xorriso \
    grub-pc-bin \
    grub-efi-amd64-bin \
    mtools



cd ../iso
mkdir debian
cd debian
mkdir debian_cosleep
cd debian_cosleep
# rm -r *

## choose an architecture, arm64, i386, amd64
#sudo debootstrap \
#    --arch=amd64 \
#    --variant=minbase \
#    stretch \
#    ${PWD}/chroot \
#    http://ftp.us.debian.org/debian/

# OR debian live image, copy the filesystem.squashfs from the iso and extract in a rooted chroot folder
# sudo mkdir chroot


# copy those files to folder cosleep/iso/debian/debian_cosleep/chroot/usr/share/applications
cosleep/iso/debian/debian_cosleep/additional/datafiles.desktop
cosleep/iso/debian/debian_cosleep/additional/edfView.desktop
cosleep/iso/debian/debian_cosleep/additional/hearing_threshold.desktop
cosleep/iso/debian/debian_cosleep/additional/recording_stimulator.desktop

# to get DNS up and running
sudo touch ${PWD}/chroot/etc/resolv.conf
sudo mount --bind /etc/resolv.conf ${PWD}/chroot/etc/resolv.conf

# chroot to the Debian environment we just bootstrapped and us bash as the shell in there
sudo chroot ${PWD}/chroot /bin/bash

# [chroot] Set a custom hostname for your Debian environment.
echo "debian-live-cosleep" > /etc/hostname

apt-get update

# [chroot] Figure out which Linux Kernel you want in the live environment.
apt-cache search linux-image
# I chose linux-image-rt-amd64 as it is fastest response and works stable on modern machines
#apt-get update && \
#apt-get -y install --no-install-recommends \
#    linux-image-rt-amd64 \
#    live-boot \
#    systemd-sysv

apt-get -y install \
    linux-image-rt-amd64

# now install the rest
echo "KERNEL==\"ttyUSB[0-9]*\",MODE=\"0666\"" | tee /etc/udev/rules.d/50-ttyusb.rules

apt-get -y --allow-unauthenticated install sox
apt-get -y --allow-unauthenticated install libsox-fmt-mp3

# optional software and setup
## for python to run and compile and recompile the packages
apt-get -y install cmake git qt5-default
apt-get -y install python python-pip python-qt4 python-alsaaudio

apt-get -y install libcanberra-gtk*
apt-get -y install python-qt4-gl
apt-get -y install libqt4-opengl
apt-get -y install python-opengl


# control audio
#apt-get -y install jackd2

# some useful apps etc.
apt-get -y install ssh rsync nano vim tmux p7zip wget htop audacity curl
apt-get -y install wicd net-tools wireless-tools wpagui
#apt-get install network-manager net-tools wireless-tools wpagui wicd

apt-get update
apt-get -y install exfat-fuse exfat-utils


## apt-get -y install --no-install-recommends \
#apt-get install \
#    network-manager net-tools wireless-tools wpagui \
#    curl openssh-client \
#    blackbox xserver-xorg-core xserver-xorg xinit xterm \
#    nano vim tmux p7zip wget htop audacity\

pip install --upgrade setuptools
pip install pyinstaller


#apt-get install xfce4
#apt-get install xfce4-goodies
#apt-get install task-xfce-desktop
## issues with installing
## openjdk-8-jre-headless:amd64
## openjdk-8-jre:amd64
## default-jre-headless
## ca-certificates-java
## default-jre
## ant
## libapache-poi-java
## libmsv-java
## libpentaho-reporting-flow-engine-java
## libdom4j-java
## liblayout-java
## libsaxonhe-java
## libxmlbeans-java
#
#apt-get install xfce4-session
#apt-cache search xfce4
## To install all xfce4 packages do:
## apt-get install `apt-cache search xfce | cut -d ' ' -f1`

#(deprecated) apt-get install xfce4-goodies

apt-get clean

# [chroot] set password of only user root, the password is cosleep
passwd root

### (deprecated) copy https://cdimage.debian.org/debian-cd/current-live/amd64/iso-hybrid/debian-live-9.8.0-amd64-xfce.packages to root folder
### (deprecated) copy requirements.txt from the source to chroot/root

mkdir cosleep
#copy the cosleep files in there including the source, basicall the github repository

cd /cosleep/software/installation/source_python
pip install -r requirements.txt
bash compile.sh
#(deprecated) awk '{FS=" "} {print $1; print " install"}' debian-live-9.8.0-amd64-xfce.packages | xargs -n 1 apt-get -y --allow-unauthenticated -q install


cd /cosleep/deploy/
bash install_edfView.sh

chmod -R a+rwx /cosleep

cd /home
# ArmA -r USERFOLDER



# [chroot] Exit the chroot.
exit





sudo umount ${PWD}/chroot/etc/resolv.conf


##### (deprecated) copy cosleep rec_stim folder into chroot/root and grant access to everyone with chmod 777 recursively

# Create directories that will contain files for our live environment files and scratch files.
# rm -r image
# rm -r scratch
# rm debian-custom-cosleep.iso
mkdir -p ${PWD}/{scratch,image/live}

# Compress the chroot environment into a Squash filesyste
sudo mksquashfs \
    ${PWD}/chroot \
    ${PWD}/image/live/filesystem.squashfs \
    -e boot

# Copy the kernel and initramfs from inside the chroot to the live directory.
cp ${PWD}/chroot/boot/vmlinuz-*rt* \
    ${PWD}/image/vmlinuz && \
cp ${PWD}/chroot/boot/initrd.img-*rt* \
    ${PWD}/image/initrd

# Create a menu configuration file for grub.
# Please note that the insmod all_video line was needed in my testing to deal with a bug in UEFI booting for one of my machines.
# Perhaps not everyone will need that line, but I did.
# This config instructs Grub to use the search command to infer which device contains our live environment.
# This seems like the most portable solution considering the various ways we may write our live environment to bootable media.
cat <<'EOF' >${PWD}/scratch/grub.cfg

search --set=root --file /DEBIAN_CUSTOM

insmod all_video

set default="0"
set timeout=10

menuentry "Debian Live COsleep (rt kernel)" {
    linux /vmlinuz boot=live components splash quiet
    initrd /initrd
}

submenu "Debian Live COsleep (rt kernel) with Localisation Support" {
menuentry "Albanian (sq)" {
  linux  /live/vmlinuz boot=live components locales=sq_AL.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Amharic (am)" {
  linux  /live/vmlinuz boot=live components locales=am_ET quiet splash
  initrd /live/initrd.img
}
menuentry "Arabic (ar)" {
  linux  /live/vmlinuz boot=live components locales=ar_EG.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Asturian (ast)" {
  linux  /live/vmlinuz boot=live components locales=ast_ES.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Basque (eu)" {
  linux  /live/vmlinuz boot=live components locales=eu_ES.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Belarusian (be)" {
  linux  /live/vmlinuz boot=live components locales=be_BY.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Bangla (bn)" {
  linux  /live/vmlinuz boot=live components locales=bn_BD quiet splash
  initrd /live/initrd.img
}
menuentry "Bosnian (bs)" {
  linux  /live/vmlinuz boot=live components locales=bs_BA.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Bulgarian (bg)" {
  linux  /live/vmlinuz boot=live components locales=bg_BG.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Tibetan (bo)" {
  linux  /live/vmlinuz boot=live components locales=bo_IN quiet splash
  initrd /live/initrd.img
}
menuentry "C (C)" {
  linux  /live/vmlinuz boot=live components locales=C quiet splash
  initrd /live/initrd.img
}
menuentry "Catalan (ca)" {
  linux  /live/vmlinuz boot=live components locales=ca_ES.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Chinese (Simplified) (zh_CN)" {
  linux  /live/vmlinuz boot=live components locales=zh_CN.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Chinese (Traditional) (zh_TW)" {
  linux  /live/vmlinuz boot=live components locales=zh_TW.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Croatian (hr)" {
  linux  /live/vmlinuz boot=live components locales=hr_HR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Czech (cs)" {
  linux  /live/vmlinuz boot=live components locales=cs_CZ.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Danish (da)" {
  linux  /live/vmlinuz boot=live components locales=da_DK.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Dutch (nl)" {
  linux  /live/vmlinuz boot=live components locales=nl_NL.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Dzongkha (dz)" {
  linux  /live/vmlinuz boot=live components locales=dz_BT quiet splash
  initrd /live/initrd.img
}
menuentry "English (en)" {
  linux  /live/vmlinuz boot=live components locales=en_US.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Esperanto (eo)" {
  linux  /live/vmlinuz boot=live components locales=eo.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Estonian (et)" {
  linux  /live/vmlinuz boot=live components locales=et_EE.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Finnish (fi)" {
  linux  /live/vmlinuz boot=live components locales=fi_FI.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "French (fr)" {
  linux  /live/vmlinuz boot=live components locales=fr_FR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Galician (gl)" {
  linux  /live/vmlinuz boot=live components locales=gl_ES.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Georgian (ka)" {
  linux  /live/vmlinuz boot=live components locales=ka_GE.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "German (de)" {
  linux  /live/vmlinuz boot=live components locales=de_DE.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Greek (el)" {
  linux  /live/vmlinuz boot=live components locales=el_GR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Gujarati (gu)" {
  linux  /live/vmlinuz boot=live components locales=gu_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Hebrew (he)" {
  linux  /live/vmlinuz boot=live components locales=he_IL.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Hindi (hi)" {
  linux  /live/vmlinuz boot=live components locales=hi_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Hungarian (hu)" {
  linux  /live/vmlinuz boot=live components locales=hu_HU.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Icelandic (is)" {
  linux  /live/vmlinuz boot=live components locales=is_IS.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Indonesian (id)" {
  linux  /live/vmlinuz boot=live components locales=id_ID.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Irish (ga)" {
  linux  /live/vmlinuz boot=live components locales=ga_IE.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Italian (it)" {
  linux  /live/vmlinuz boot=live components locales=it_IT.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Japanese (ja)" {
  linux  /live/vmlinuz boot=live components locales=ja_JP.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Kazakh (kk)" {
  linux  /live/vmlinuz boot=live components locales=kk_KZ.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Khmer (km)" {
  linux  /live/vmlinuz boot=live components locales=km_KH quiet splash
  initrd /live/initrd.img
}
menuentry "Kannada (kn)" {
  linux  /live/vmlinuz boot=live components locales=kn_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Korean (ko)" {
  linux  /live/vmlinuz boot=live components locales=ko_KR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Kurdish (ku)" {
  linux  /live/vmlinuz boot=live components locales=ku_TR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Lao (lo)" {
  linux  /live/vmlinuz boot=live components locales=lo_LA quiet splash
  initrd /live/initrd.img
}
menuentry "Latvian (lv)" {
  linux  /live/vmlinuz boot=live components locales=lv_LV.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Lithuanian (lt)" {
  linux  /live/vmlinuz boot=live components locales=lt_LT.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Malayalam (ml)" {
  linux  /live/vmlinuz boot=live components locales=ml_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Marathi (mr)" {
  linux  /live/vmlinuz boot=live components locales=mr_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Macedonian (mk)" {
  linux  /live/vmlinuz boot=live components locales=mk_MK.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Burmese (my)" {
  linux  /live/vmlinuz boot=live components locales=my_MM quiet splash
  initrd /live/initrd.img
}
menuentry "Nepali (ne)" {
  linux  /live/vmlinuz boot=live components locales=ne_NP quiet splash
  initrd /live/initrd.img
}
menuentry "Northern Sami (se_NO)" {
  linux  /live/vmlinuz boot=live components locales=se_NO quiet splash
  initrd /live/initrd.img
}
menuentry "Norwegian Bokmaal (nb_NO)" {
  linux  /live/vmlinuz boot=live components locales=nb_NO.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Norwegian Nynorsk (nn_NO)" {
  linux  /live/vmlinuz boot=live components locales=nn_NO.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Persian (fa)" {
  linux  /live/vmlinuz boot=live components locales=fa_IR quiet splash
  initrd /live/initrd.img
}
menuentry "Polish (pl)" {
  linux  /live/vmlinuz boot=live components locales=pl_PL.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Portuguese (pt)" {
  linux  /live/vmlinuz boot=live components locales=pt_PT.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Portuguese (Brazil) (pt_BR)" {
  linux  /live/vmlinuz boot=live components locales=pt_BR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Punjabi (Gurmukhi) (pa)" {
  linux  /live/vmlinuz boot=live components locales=pa_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Romanian (ro)" {
  linux  /live/vmlinuz boot=live components locales=ro_RO.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Russian (ru)" {
  linux  /live/vmlinuz boot=live components locales=ru_RU.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Sinhala (si)" {
  linux  /live/vmlinuz boot=live components locales=si_LK quiet splash
  initrd /live/initrd.img
}
menuentry "Serbian (Cyrillic) (sr)" {
  linux  /live/vmlinuz boot=live components locales=sr_RS quiet splash
  initrd /live/initrd.img
}
menuentry "Slovak (sk)" {
  linux  /live/vmlinuz boot=live components locales=sk_SK.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Slovenian (sl)" {
  linux  /live/vmlinuz boot=live components locales=sl_SI.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Spanish (es)" {
  linux  /live/vmlinuz boot=live components locales=es_ES.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Swedish (sv)" {
  linux  /live/vmlinuz boot=live components locales=sv_SE.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Tagalog (tl)" {
  linux  /live/vmlinuz boot=live components locales=tl_PH.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Tamil (ta)" {
  linux  /live/vmlinuz boot=live components locales=ta_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Telugu (te)" {
  linux  /live/vmlinuz boot=live components locales=te_IN quiet splash
  initrd /live/initrd.img
}
menuentry "Tajik (tg)" {
  linux  /live/vmlinuz boot=live components locales=tg_TJ.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Thai (th)" {
  linux  /live/vmlinuz boot=live components locales=th_TH.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Turkish (tr)" {
  linux  /live/vmlinuz boot=live components locales=tr_TR.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Uyghur (ug)" {
  linux  /live/vmlinuz boot=live components locales=ug_CN quiet splash
  initrd /live/initrd.img
}
menuentry "Ukrainian (uk)" {
  linux  /live/vmlinuz boot=live components locales=uk_UA.UTF-8 quiet splash
  initrd /live/initrd.img
}
menuentry "Vietnamese (vi)" {
  linux  /live/vmlinuz boot=live components locales=vi_VN quiet splash
  initrd /live/initrd.img
}
menuentry "Welsh (cy)" {
  linux  /live/vmlinuz boot=live components locales=cy_GB.UTF-8 quiet splash
  initrd /live/initrd.img
}
}
EOF

# Create a special file in image named DEBIAN_CUSTOM.
# This file will be used to help Grub figure out which device contains our live filesystem.
# This file name must be unique and must match the file name in our grub.cfg config.

touch ${PWD}/image/DEBIAN_CUSTOM


# Your ${PWD} directory should now roughly look like this.
ls -lh
#${PWD}/chroot/*tons of chroot files*
#${PWD}/scratch/grub.cfg
#${PWD}/image/DEBIAN_CUSTOM
#${PWD}/image/initrd
#${PWD}/image/vmlinuz
#${PWD}/image/live/filesystem.squashfs


## create live .iso for dd to put on USB stick, runs on UEFI and BIOS
# Create a grub UEFI image.

grub-mkstandalone \
    --format=x86_64-efi \
    --output=${PWD}/scratch/bootx64.efi \
    --locales="" \
    --fonts="" \
    "boot/grub/grub.cfg=${PWD}/scratch/grub.cfg"

# Create a FAT16 UEFI boot disk image containing the EFI bootloader.
# Note the use of the mmd and mcopy commands to copy our UEFI boot loader named bootx64.efi

(cd ${PWD}/scratch && \
    dd if=/dev/zero of=efiboot.img bs=1M count=10 && \
    mkfs.vfat efiboot.img && \
    mmd -i efiboot.img efi efi/boot && \
    mcopy -i efiboot.img ./bootx64.efi ::efi/boot/
)


# Create a grub BIOS image.

grub-mkstandalone \
    --format=i386-pc \
    --output=${PWD}/scratch/core.img \
    --install-modules="linux normal iso9660 biosdisk memdisk search tar ls" \
    --modules="linux normal iso9660 biosdisk search" \
    --locales="" \
    --fonts="" \
    "boot/grub/grub.cfg=${PWD}/scratch/grub.cfg"

# Use cat to combine a bootable Grub cdboot.img bootloader with our boot image.

cat \
    /usr/lib/grub/i386-pc/cdboot.img \
    ${PWD}/scratch/core.img \
> ${PWD}/scratch/bios.img

#Generate the ISO file.

xorriso \
    -as mkisofs \
    -iso-level 3 \
    -full-iso9660-filenames \
    -volid "DEBIAN_CUSTOM" \
    -eltorito-boot \
        boot/grub/bios.img \
        -no-emul-boot \
        -boot-load-size 4 \
        -boot-info-table \
        --eltorito-catalog boot/grub/boot.cat \
    --grub2-boot-info \
    --grub2-mbr /usr/lib/grub/i386-pc/boot_hybrid.img \
    -eltorito-alt-boot \
        -e EFI/efiboot.img \
        -no-emul-boot \
    -append_partition 2 0xef ${PWD}/scratch/efiboot.img \
    -output "${PWD}/debian-custom-cosleep.iso" \
    -graft-points \
        "${PWD}/image" \
        /boot/grub/bios.img=${PWD}/scratch/bios.img \
        /EFI/efiboot.img=${PWD}/scratch/efiboot.img

# Now burn the ISO to a CD or dd to an USB stick and you should be ready to boot from it using either a UEFI or a BIOS system.
# (sudo) dd if=debian-custom.iso of=/dev/sdX bs=1M
# unmount and remove previous partition table then: sudo dd if=debian-custom-cosleep.iso of=/dev/sdc bs=4M; sync
