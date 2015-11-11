set ns "C202"
namespace eval $ns {
variable commandID 202


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P202Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 202
------------------------------
Puts the guider and telescope 
in alignment mode. 
Takes the RA and DEC as input."


variable RA 0.0
variable DEC 0.0 

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Alignment Mode TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to enter\nAlignment Mode\nwith:\n    RA=$RA\n    DEC=$DEC"

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

    set commandQuestion "Do you want to enter\nAlignment mode\nwith:\n    RA=$RA\n    DEC=$DEC"

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

frame $tab.fAlignmentModeTC \
    -relief ridge \
    -borderwidth 2 \
    -height 100



if { $positions::P202On } { 
grid $tab.fAlignmentModeTC \
    -in $tab\
    -column $positions::P202X\
    -columnspan 2\
    -row $positions::P202Y\
    -rowspan 2\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fAlignmentModeTC.refRA \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "RA:"\
    -textvariable "${ns}::RA" \
    -width 5

iwidgets::regexpfield $tab.fAlignmentModeTC.refDEC \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "DEC:"\
    -textvariable "${ns}::DEC"\
    -width 5
    

button $tab.fAlignmentModeTC.bAlignmentModeTC \
    -text "Alignment Mode" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.fAlignmentModeTC.bAlignmentModeTC \
    -side right
pack $tab.fAlignmentModeTC.refRA $tab.fAlignmentModeTC.refDEC \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fAlignmentModeTC.bAlignmentModeTC $tthelp


}

