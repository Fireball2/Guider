set ns "C207"
namespace eval $ns {
variable commandID 207


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P207Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command $commandID
---------------------------------
Tells the guider what coordinates to
zoom in on."



variable x 680
variable y 518

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Autocollimation Zoom XY"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to send X \& Y zoom \nwith:\n    X=$x\n    Y=$y"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable x
    variable y


    if { $y > 986 } { set y 986 }
    if { $y < 50 } { set y 50 }
    if { $x < 50 } { set x 50 }
    if { $x > 1310 } { set x 1310 }
    

    
set commandQuestion "Do you want to send X \& Y zoom \nwith:\n    X=$x\n    Y=$y"

    
    set commandMessage "\[$commandID] $commandName x= $x y= $y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format css $commandID $x $y]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

set suffix ACZoomXY

frame $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -height 100

if { $positions::P207On } { 
pack $tab.f$suffix\
    -side top \
    -fill x
}

iwidgets::regexpfield $tab.f$suffix.refX \
    -regexp {^[0-9]+$}\
    -labeltext "X:"\
    -textvariable "${ns}::x" \
    -width 5

iwidgets::regexpfield $tab.f$suffix.refY \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "Y (arcsec):"\
    -textvariable "${ns}::y"\
    -width 5
    

button $tab.f$suffix.b$suffix \
    -text "XY Zoom" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.f$suffix.b$suffix \
    -side right
pack $tab.f$suffix.refX $tab.f$suffix.refY \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

