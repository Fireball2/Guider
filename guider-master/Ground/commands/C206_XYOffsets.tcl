set ns "C206"
namespace eval $ns {
variable commandID 206


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P206Tab
set suffix XYOffsets

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Asks the MPF digital to offset 
the pointing by x and y arcsec."


variable x 0
variable y 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "X Y Offsets"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to send offsets:
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


    if { $x < -200 } { set x -200 }
    if { $x > 200 } { set x 200 }
    if { $y < -200 } { set y -200 }
    if { $y > 200 } { set y 200 }

set commandQuestion \
"Do you want to send offsets:
 (dx,dy) = ($x, $y)?"    

    
    

    set commandMessage "\[$commandID] $commandName dx= $x, dy=$y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cff $commandID $x $y]
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
    -text "X \ & Y Offsets"

if { $positions::P206On } { 
grid $tab.f$suffix \
    -in $tab \
    -column $positions::P206X \
    -row $positions::P206Y \
    -columnspan 2 \
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.f$suffix.refx \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "dx (as):"\
    -textvariable "${ns}::x" \
    -width 5

iwidgets::regexpfield $tab.f$suffix.refy \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "dy (as):"\
    -textvariable "${ns}::y"\
    -width 5
    

button $tab.f$suffix.b$suffix \
    -text "XY Offsets" \
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

