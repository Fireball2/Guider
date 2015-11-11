set ns "C204"
namespace eval $ns {
variable commandID 204


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P204Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 204
---------------------------------
Requests the siderostat be 
nudged a given number of arcsec
in elevation and cross-elevation."



variable EL 0.0
variable CE 0.0 

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "EL \& CE Offsets TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to send \nEL \& CE Offsets TC \nwith:\n    EL=$EL arcsec\n    CE=$CE arcsec"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable EL
    variable CE


    if { $EL > 1000. } { set EL 1000.0 }
    if { $EL < -1000. } { set EL 1000.0 }
    if { $CE > 1000. } { set CE 1000.0 }
    if { $CE < -1000. } { set CE -1000.0 }
    
    variable commandQuestion "Do you want to send \nEL \& CE Offsets TC \nwith:\n    EL=$EL arcsec\n    CE=$CE arcsec"
    
    
    set commandMessage "\[$commandID] $commandName EL= $EL CE=$CE"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cff $commandID $EL $CE]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

frame $tab.fELCEOffsetsTC \
    -relief ridge \
    -borderwidth 2 \
    -height 100

if { $positions::P204On } { 
pack $tab.fELCEOffsetsTC \
    -side top \
    -fill x
}

iwidgets::regexpfield $tab.fELCEOffsetsTC.refEL \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "EL (arcsec):"\
    -textvariable "${ns}::EL" \
    -width 5

iwidgets::regexpfield $tab.fELCEOffsetsTC.refCE \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "CE (arcsec):"\
    -textvariable "${ns}::CE"\
    -width 5
    

button $tab.fELCEOffsetsTC.bELCEOffsetsTC \
    -text "EL \& CE Offsets" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.fELCEOffsetsTC.bELCEOffsetsTC \
    -side right
pack $tab.fELCEOffsetsTC.refEL $tab.fELCEOffsetsTC.refCE \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fELCEOffsetsTC.bELCEOffsetsTC $tthelp


}

