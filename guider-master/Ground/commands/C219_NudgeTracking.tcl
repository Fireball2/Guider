set ns "C219"
namespace eval $ns {
variable commandID 219


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P219Tab
set suffix NudgeTracking

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Nudges the tracking point a given
number of pixels in x and y."


variable x 0
variable y 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Nudge Tracking"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to nudge the the tracking point by:
 (dx,dy) = ($x, $y)?"

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


    if { $x < -100 } { set x -100 }
    if { $x > 100 } { set x 100 }
    if { $y < -100 } { set y -100 }
    if { $y > 100 } { set y 100 }

set commandQuestion \
"Do you want to nudge  the tracking point by:
 (dx,dy) = ($x, $y)?"    

    
    

    set commandMessage "\[$commandID] $commandName dx= $x, dy=$y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccc $commandID $y $x]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -text "Nudge Tracking Point"

if { $positions::P219On } {
grid $tab.f$suffix \
    -in $tab \
    -column $positions::P219X \
    -row $positions::P219Y \
    -columnspan 2 \
    -sticky ew\
-padx $positions::padX\
-pady $positions::padY
}

iwidgets::regexpfield $tab.f$suffix.refx \
    -regexp {^[\-0-9]+$}\
    -labeltext "dx (pix):"\
    -textvariable "${ns}::x" \
    -width 5

iwidgets::regexpfield $tab.f$suffix.refy \
    -regexp {^[\-0-9]+$}\
    -labeltext "dy (pix):"\
    -textvariable "${ns}::y"\
    -width 5
    

button $tab.f$suffix.b$suffix \
    -text "Nudge Tracking" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.f$suffix.b$suffix \
    -side right
pack $tab.f$suffix.refx $tab.f$suffix.refy \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

