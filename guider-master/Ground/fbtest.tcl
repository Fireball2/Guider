#!/usr/bin/wish

# ground station implementation for the FIREBALL guider. 
# February 2009
# Matt Matuszewski
# matmat@srl.caltech.edu
# not pretty code, but supposed to work.



package require tooltip
package require Iwidgets
package require tile



#puts [ttk::themes]
#ttk::setTheme alt


puts "Hello"

# -------------------------------------------------------------------
# serial handling
# -------------------------------------------------------------------
puts "Starting Serial"
set serial [open "/dev/ttyS0" r+]
puts "Serial supposedly set"
fconfigure $serial -mode "9600,n,8,1"
puts "now supposedly configured" 
fconfigure $serial -blocking 0 -buffering none -translation binary
puts "now configured even more"


# ------------------------------------------------------------------
# Resets the serial 
# ------------------------------------------------------------------
proc ResetSerial { } { 
    global serial
    after 1000
    close $serial 
    set serial [open "/dev/ttyS0" r+]
    puts "Serial supposedly set"
    fconfigure $serial -mode "9600,n,8,1"
    puts "now supposedly configured" 
    fconfigure $serial -blocking 0 -buffering none -translation binary
    puts "now configured even more"
}


proc FlushSerial { } { 
    global serial
    flush $serial
}


# -------------------------------------------------------------------
# time handling 
# -------------------------------------------------------------------
proc starttime {} {
    set nowtime [clock format [clock seconds] \
		     -format "%T on %a, %b %d %G" -gmt 1]
    return "Log started at $nowtime GMT"
}

proc timenow {} {
    set nowtime [clock format [clock seconds] \
		     -format "\[%D %T]" -gmt 1]
    return $nowtime
}


# -------------------------------------------------------------------
# log handling 
# -------------------------------------------------------------------
set logfilenumber 0
set logfile [open "gse.log" a]
puts $logfile [starttime]


proc writelog { mesg } { 
    global logfile
    set tmn [timenow]
    puts  $logfile "\[$tmn] : $mesg"
    flush $logfile
}

proc closelog {}  {
    global logfile
    writelog "GSE : Log Closed\n\n"
    close $logfile
}

set count 0


proc properexit {} {
    global serial
    close $serial
    closelog
    exit
}








#main matter
wm title . "FIREBall Guider Ground Control"

frame .mbar -borderwidth 1 -width 1280   -relief raised
pack .mbar -fill x -side top


menubutton .mbar.file -text "File" -menu .mbar.file.m
pack .mbar.file -side left
menu .mbar.file.m
.mbar.file.m add command -label "Reset Serial" -command ResetSerial
.mbar.file.m add command -label "Flush Serial" -command FlushSerial
.mbar.file.m add command -label "Exit" -command properexit

menubutton .mbar.edit -text "Edit" -menu .mbar.edit.m
pack .mbar.edit -side left
menu .mbar.edit.m
.mbar.edit.m add command -label "Clear" -command clear


frame .style -borderwidth 1 -relief sunken 
pack .style -fill x  -side bottom

label .style.readout -text "x: 0.00 y: 0.00" 
pack .style.readout -side right



iwidgets::tabnotebook .tn \
	-tabpos s \
	-width 500 \
	-height 800 \
	-angle 0 \
	-bevelamount 4 \
	-gap 3 \
	-margin 6 \
	-tabborders 0 


set generalTab [.tn add -label "Gen."]
set autocTab [.tn add -label "AutoC"]
set alignTab [.tn add -label "Align"]
set computerTab [.tn add  -label "CPU"]
set infoTab [.tn add -label "Info"]
set miscTab [.tn add -label "Misc."]
set blobTab [.tn add -label "Blobs" ]








.tn select 2

pack .tn -side left




listbox .lb -width 85 -height 4
pack .lb -side bottom

listbox .lb2 -width 85 
pack .lb2 -side bottom









# message counter. Gets increased every time we try to send a message
proc get_counter {  } {
    global count
    set cnt $count
    incr count
    if {$count > 65535} { 
	set count 0
    }
    return $cnt
}


#whenever a command is to be sent to the guider the following things
#need to happen:
# 1. Verify that the port is open
# 2. Confirm that the command is to be sent. 
# 3. Send the command
# 4. Write the command sent to a log-file in csv format
# 5. Write the command to the on-screen log in a legible form
# 6. Remember to write the resultant echo message from the guider into
#    logfile.
# 7. document what the command is, the options, etc. 



proc message {mbody commandMessage commandLog} {
    global serial

    set syncstart \x55
    set counter [binary format s [get_counter]] 
    set length [binary format  s [string length $mbody]]
    puts "Length is [string length $mbody]"
    set crc a

    set syncend \xAA
    set mess0  $syncstart$counter$length$mbody
    set length [string length $mess0]
    set crcstart [string index $mess0 1]
    binary scan $crcstart c crc
    for {set i 2} { $i < $length } {incr i} { 
	set currchar   [string index $mess0 $i]
	binary scan $currchar c charval
	set crc2 [expr $crc ^ $charval]
	set crc $crc2
	set len [string length $crc]
    }
    set crc [binary format c $crc]
    set len [string length $crc]
#    puts "length of crc $len. crc is $crc"

    set mess $mess0$crc$syncend

#    binary scan  $mess ccccccccc a0 a1 a2 a3 a4 a5 a6 a7 a8
#    puts "mess: $a0 $a1 $a2 $a3 $a4 $a5 $a6 $a7 $a8"
    

#    puts [string length $crc]
#    puts [string length $mess0]
#    puts [string length $mess]
#    puts "the message is \[  $mess  ]"
    puts -nonewline $serial $mess

    #update the listbox. 
    .lb insert end $commandMessage    
    .lb itemconfigure [expr [.lb index end] -1] -foreground black 
    .lb see [expr [.lb index end]]

    #write to the log
    binary scan $counter c ctr
    set logstring "$ctr : $commandLog"
    writelog $logstring
}


proc verify {commandQuestion } {
    set resp [tk_messageBox  -type yesno -message $commandQuestion]
    return $resp
}


#    set syncstart [binary format c 85]
#    set syncend [binary format c 170]


set MAINPATH .



source "$MAINPATH/commands/variables.tcl"


source "$MAINPATH/commands/positions.tcl"

source "$MAINPATH/commands/C011_FeelLucky.tcl"
source "$MAINPATH/commands/C012_DitherTC.tcl"
source "$MAINPATH/commands/C140_RestartGuider.tcl"
source "$MAINPATH/commands/C141_ResetDisplay.tcl"
source "$MAINPATH/commands/C142_ShutdownGuider.tcl"
source "$MAINPATH/commands/C160_SwitchGuider.tcl"
source "$MAINPATH/commands/C161_SwitchDTU.tcl"
source "$MAINPATH/commands/C162_LargeWindow.tcl"
source "$MAINPATH/commands/C163_SmallWindow.tcl"
source "$MAINPATH/commands/C164_WindowTrackTC.tcl"
source "$MAINPATH/commands/C165_SmallWindowTimes.tcl"
source "$MAINPATH/commands/C166_LargeWindowTimes.tcl"
source "$MAINPATH/commands/C200_INITModeTC.tcl"
source "$MAINPATH/commands/C201_AUTOModeTC.tcl"
source "$MAINPATH/commands/C202_ALGNModeTC.tcl"
#source "./commands/C203_SLEWModeTC.tcl"
source "$MAINPATH/commands/C204_ELCEOffsetsTC.tcl"
source "$MAINPATH/commands/C205_AlignSubmode.tcl"
source "$MAINPATH/commands/C206_XYOffsets.tcl"
source "$MAINPATH/commands/C207_AutoCollimationZoomXY.tcl"
source "$MAINPATH/commands/C210_SetROITC.tcl"
source "$MAINPATH/commands/C211_ExposureTime.tcl"
source "$MAINPATH/commands/C214_ToggleZoom.tcl"
source "$MAINPATH/commands/C014_ToggleZoom.tcl"
source "$MAINPATH/commands/C219_NudgeTracking.tcl"
source "$MAINPATH/commands/C221_FrameBlobGain.tcl"
source "$MAINPATH/commands/C222_FrameBlobSatVal.tcl"
source "$MAINPATH/commands/C223_FrameBlobThreshhold.tcl"
source "$MAINPATH/commands/C224_FrameBlobDistanceTolerance.tcl"
source "$MAINPATH/commands/C225_FrameBlobGridSize.tcl"
source "$MAINPATH/commands/C226_FrameBlobCenBox.tcl"
source "$MAINPATH/commands/C227_FrameBlobApBox.tcl"
source "$MAINPATH/commands/C228_FrameBlobReadoutOffset.tcl"
source "$MAINPATH/commands/C229_FrameBlobReadoutNoise.tcl"
source "$MAINPATH/commands/C230_FocusTC.tcl"
source "$MAINPATH/commands/C231_AutoFocusTC.tcl"
source "$MAINPATH/commands/C232_LEDOn.tcl"
source "$MAINPATH/commands/C233_LEDOff.tcl"
source "$MAINPATH/commands/C234_TagLog.tcl"
source "$MAINPATH/commands/C238_DACToggle.tcl"
source "$MAINPATH/commands/C242_TrackingPoint.tcl"
source "$MAINPATH/commands/C243_MaxStep.tcl"
source "$MAINPATH/commands/C249_FixOffset.tcl"
source "$MAINPATH/commands/C215_ChangeDisplaySettings.tcl"
source "$MAINPATH/commands/C015_Overmax.tcl"
source "$MAINPATH/commands/C017_ToggleAlgorithm.tcl"
source "$MAINPATH/commands/C016_OvermaxThr.tcl"
source "$MAINPATH/commands/C018_SpotRMS.tcl"
source "$MAINPATH/commands/C019_BoxSize.tcl"
source "$MAINPATH/commands/starfield.tcl"

source "$MAINPATH/commands/infopanel.tcl"










set poll_delay 10; #com port delay in ms



#variable v
#ttk::scale .scal -from 0 -to 255 -value 128 -length 255 -orient vertical -variable v
#ttk::label .lab -textvariable v

#xcpack .scal .lab -side right



    variable blob0, blob1, blob2, blob3, blob4, blob5, blob6, blob7, blob8, blob9

canvas .stars -background white -width 680 -height 518 -background darkgray
pack .stars -side top -expand no
set roi [.stars create rect 0 0 680 518 -outline tan  -fill black ] 





set boundbox [.stars create rect 100 100 200 200 -outline orange -fill {}]
set win [.stars create rect 100 100 200 200 -outline yellow -fill {}]
set trackpoint [.stars create polygon 200 200 210 190 220 200 210 210 -outline darkgreen -fill {}] 
set trackpoint2 [.stars create polygon 300 300 310 290 320 300 310 310 -outline green -fill {}]




for {set i 0} {$i < 10} {incr i} {
    set blob [.stars create oval -20 -20 -30 -30  -fill blue]
    set blob$i $blob
    puts "$blob  $blob0"
    

}


proc updateblobs {} {
    global blob0, blob1, blob2, blob3, blob4, blob5, blob6, blob7, blob8, blob9
    
    for {set i 0} { $i < 10 } { incr i } { 
	if { $i < $var::bcnt } {


	    set bx [ var::getblobx $i ]
	    set by [ var::getbloby $i ]
	    set bf [ var::getblobf $i ]


#	    set bl blob$i
#	    set bl [expr $$bl]


	    .stars coords [expr $i+6] [expr 0.5* $bx   -5] [expr $by/2-5] [expr 0.5*$bx+5] [expr 0.5*$by+5]
	    .stars itemconfigure [expr 6+$i] -fill red
#	    tooltip::tooltip .stars -item [expr $i+2] "($bx, $by) $bf"
	} else {
	    .stars coords [expr 6+$i] -20 -20 -20 -20
	}



    };

}


proc updatefltrackpoint { x y } {
    global trackpoint2
    set x [expr $x/2]
    set y [expr $y/2]
    .stars coords $trackpoint2 [expr $x-10] $y $x [expr $y+10] [expr $x+10] $y $x [expr $y-10]
}

proc updateroi {x y dx dy} {
    global roi

    set dx [expr $dx/2]
    set dy [expr $dy/2]    
    set x1 [expr $x/2]
    set y1 [expr $y/2]
    set x2 [expr $x/2+$dx]
    set y2 [expr $y/2+$dy]

    .stars coords $roi $x1 $y1 $x2 $y2
}

proc updatewin {x y dx dy} {
    global win
    
    set dx [expr $dx/2]
    set dy [expr $dy/2]    
    set x1 [expr $x/2-$dx]
    set y1 [expr $y/2-$dx]
    set x2 [expr $x/2+$dx]
    set y2 [expr $y/2+$dy]

    .stars coords $win $x1 $y1 $x2 $y2
}





bind .stars <Button-3> {
    if {$var::md !=2 } { 
    set x1 [expr %x-50]
    set x2 [expr %x+50]
    set y1 [expr %y-50]
    set y2 [expr %y+50]
    .stars coords $boundbox $x1 $y1 $x2 $y2
    set C164::xc [expr %x*2]
    set C164::yc [expr %y*2]
    set C164::d 100
    } else { 
    set C207::x [expr %x*2]
    set C207::y [expr %y*2]
    }

}

bind .stars <Double-Button-3> {
    if { $var::md != 2 } { 
	puts [.stars coords $boundbox]
	C164::myfunction
    } else {
	C207::myfunction
    };
}

bind .stars <Double-Button-1> {
    puts [.stars postscript -file outfile.ps -height 640 -width 480 -pageheight 4.8c -pagewidth 6.4c ]
    C242::myfunction
}

bind . <Control-Alt-Key-x> { 

properexit 

}


bind .stars <Button-1> {
    set x1 [expr %x-10]
    set x2 [expr %x]
    set x3 [expr %x+10]
    set x4 [expr %x]
    set y1 [expr %y]
    set y2 [expr %y+10]
    set y3 [expr %y]
    set y4 [expr %y-10]

    .stars coords $trackpoint $x1 $y1 $x2 $y2 $x3 $y3 $x4 $y4 

    set C164::tx [expr %x*2]
    set C164::ty [expr %y*2]
    set C242::x [expr %x*2]
    set C242::y [expr %y*2]

}


#set starone [.stars create oval 100 100 5 5 -outline red -fill green]
#canvas move $starone 200 200


source "$MAINPATH/commands/readserial.tcl"

# command for 


while {1} {
    set data [read $serial]             ;# read ALL incoming bytes
    set size [string length $data]      ;# number of received byte, may be 0
    if { $size } {
#	puts "received $size bytes: $data"
	readserial::parsebuffer $data
    } else {
#	puts "<no data>"
#	puts [get_counter]
	update      ;# Display output, allow to close wish-window
    }
    after $poll_delay
}









