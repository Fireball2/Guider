set ns "C203"
namespace eval $ns {
variable commandID 203


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $generalTab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 203
--------------------------------
Puts the guider and telescope in
in slewing mode. 
Takes the RA and DEC as input."


variable RA 0.0
variable DEC 0.0 

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Slewing Mode TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to enter\nSlewing Mode\nwith:\n    RA=$RA\n    DEC=$DEC"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable RA
    variable DEC

    if { $RA > 360. } { set RA 360.0 }
    if { $RA < 0. } { set RA 0.0 }
    if { $DEC > 90. } { set DEC 90.0 }
    if { $DEC < -90. } { set DEC -90.0 }

    set commandQuestion "Do you want to enter\nAutoCollimation mode\nwith:\n    RA=$RA\n    DEC=$DEC"

    set commandMessage "\[$commandID] $commandName RA= $RA DEC=$DEC"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cdd $commandID $RA $DEC]
	message $mbody $commandMessage $commandLog
    } else {
	.stars create oval 100 200 300 200 -outline green -fill red
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

frame $tab.fSlewingModeTC \
    -relief ridge \
    -borderwidth 2 \
    -height 100

pack $tab.fSlewingModeTC \
    -side top \
    -fill x

iwidgets::regexpfield $tab.fSlewingModeTC.refRA \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "RA:"\
    -textvariable "${ns}::RA" \
    -width 5

iwidgets::regexpfield $tab.fSlewingModeTC.refDEC \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "DEC:"\
    -textvariable "${ns}::DEC"\
    -width 5
    

button $tab.fSlewingModeTC.bSlewingModeTC \
    -text "Slewing Mode" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.fSlewingModeTC.bSlewingModeTC \
    -side right
pack $tab.fSlewingModeTC.refRA $tab.fSlewingModeTC.refDEC \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSlewingModeTC.bSlewingModeTC $tthelp


}

