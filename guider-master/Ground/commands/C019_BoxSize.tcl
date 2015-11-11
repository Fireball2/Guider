set ns "C019"
namespace eval $ns {
variable commandID 19


# -----------------------------
# pathname for the controls
# -----------------------------
    set tab $positions::P019Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 019
--------------------------------
Sets the number of boxes to divide 
the 200x200 window into. Recommended
numbers are either 5, 10 or 20."


variable STEPS 10


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Box Count TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to set the number of boxes to\n    $STEPS\nsteps?" 

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable STEPS

    if { $STEPS > 20 } { set STEPS 20 }
    if { $STEPS < 5 } { set STEPS 5 }


    set commandQuestion "Do you want to set the number of boxes to\n    $STEPS\nsteps?" 
    set commandMessage "\[$commandID] $commandName BOXES = $STEPS"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID $STEPS ]
	message $mbody $commandMessage $commandLog
    } else {
	
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fBoxesTC \
    -relief ridge \
    -borderwidth 2 \
    -text "Subwindow Boxes"

if { $positions::P019On } { 
grid $tab.fBoxesTC\
    -in $tab\
    -column $positions::P019X\
    -row $positions::P019Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fBoxesTC.refSteps \
    -regexp {^[\-0-9]+$}\
    -labeltext "Box Count:"\
    -textvariable "${ns}::STEPS" \
    -textfont {Helvetica 14 bold} \
    -width 6

button $tab.fBoxesTC.bBoxesTC \
    -text "Set Boxes" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.fBoxesTC.bBoxesTC \
    -side right

pack $tab.fBoxesTC.refSteps\
    -side left


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fBoxesTC.bBoxesTC $tthelp


}

