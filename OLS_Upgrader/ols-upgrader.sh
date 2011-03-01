#!/bin/bash

# Written by Jack Gassett
# 10/25/2010
# www.gadgetfactory.net

outfile=winbin/outfile
comport=winbin/comport
romdir=FPGAROM/
firmwaredir=PIC_firmware/
bindir=linbin
defaultcom=/dev/ttyACM0


#Determine if we are in Windows on cygwin
if [ "$OSTYPE" == 'cygwin' ]; then
	export PATH=winbin\:$PATH
	defaultcom=COM6
fi

#Determine if we are in Windows on cygwin
if [ "$OSTYPE" == 'linux-gnu' ]; then
	if ["$(arch)" == 'x86-64' ]; then
		bindir=linbin64
		export PATH=linbin64\:$PATH
	else
		bindir=linbin
		export PATH=linbin\:$PATH
	fi
	chmod a+x $bindir/*
fi

dialog --title "OpenBench Logic Sniffer Upgrader" \
		--msgbox "This will step you through upgrading the OpenBench Logic Sniffer to the latest version of the PIC Firmware and the FPGA ROM bitstream." 15 55


dialog --title "OpenBench Logic Sniffer FPGA Bitstream Loader" \
        --menu "Please choose the OLS bitstream to load:" 15 80 5 \
		"logic_sniffer_3.07-Demon-Core.bit" "FPGA ROM Demon Core 3.07" \
        "Logic_Sniffer_dynamic_depth.bit" "FPGA ROM 2.12" 2> $outfile
return_value=$?

# $bindir/dialog --title "Com Port" \
        # --menu "Please choose the Com Port the OLS is connected to:" 15 55 5 \
        # "COM5" "32 Channels with 4k Sample Depth" \
        # "COM6" "16 Channels with 8k Sample Depth" \
        # "COM3" "8 Channels with 16k Sample Depth" 2> $comport
# return_value=$?

dialog --title "Com Port" \
        --inputbox "Please choose the Com Port the OLS is connected to:" 15 55 $defaultcom 2> $comport
		
#bindir/ols-loader -status -p:`cat $comport` -boot	
#read -n1 -r -p "Press any key to continue..." key
		
dialog --title "Ensure OLS is ready for loading FPGA bitstream." \
		--msgbox "Please ensure the OLS is in update mode by holding down the update button and pressing reset. Both ACT and PWR LED's should be lit." 15 55

clear
ols-loader -write -erase -p:`cat $comport` -wB:$romdir`cat $outfile`
ols-loader -boot -p:`cat $comport`
read -n1 -r -p "Press any key to continue..." key

dialog --title "OpenBench Logic Sniffer PIC Firmware Loader" \
        --menu "Please choose the OLS Firmware to load:" 15 80 5 \
		"OLSv1.firmware.v3.0.hex" "PIC Firmware Demon Core 3.0" \
		"OLSv1.firmware.v2.6.hex" "PIC Firmware Demon Core 2.6" \
		"OLSv1.04-firmware-v2.3.hex" "PIC Firmware 2.3" \
        "OLSv1-firmware-v2.1.hex" "PIC Firmware 2.1" 2> $outfile
return_value=$?

dialog --title "Ensure OLS is ready for loading PIC firmware." \
		--msgbox "The OLS should be ready to load the PIC firmware, please verify that ACT and PWR are both lit. If they are not, place the PIC into update mode by placing a jumper between the PGC and PCD pins of the ICSP header and pressing reset. The ACT LED will light and the OLS will enumerate as a USB HID device (no driver needed)." 15 55

clear
ols-fw-update -e -w -m flash -vid 0x04D8 -pid 0xFC90 -ix $firmwaredir`cat $outfile`
read -n1 -r -p "Press any key to continue..." key

dialog --title "Upgrade Finished." \
		--msgbox "If both the portions of the upgrade process finished succesfully then you should be upgraded to the latest version." 15 55
clear

