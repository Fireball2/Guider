set ns "C230"
namespace eval $ns {
variable commandID 230


# -----------------------------
# pathname for the controls
# -----------------------------
    set tab $positions::P230Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 230
--------------------------------
Moves the focal plane stage 
a fixed number of steps."


variable STEPS 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Focus TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to move the\nFocal plane stage\n    $STEPS\nsteps?"

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

    if { $STEPS > 10000. } { set STEPS 10000.0 }
    if { $STEPS < -10000. } { set STEPS 10000.0 }


    

    set h [expr $var::lvdt - $STEPS * 3.937e-5 ]
    set dh [expr ( $h - 5.78 )/10.*25.4]

    if { $h > 6.57 || $h < 5.00 } {
	set outofrangeQ "This will take you outside the 2mm safety buffer to $dh mm  away from nominal focus.\n\n\nAre you sure you want to do this?"
	set yesIdo [verify $outofrangeQ]

    } 
    set commandQuestion "Do you want to move the\nFocal plane stage\n    $STEPS\nsteps?"

    set commandMessage "\[$commandID] $commandName STEPS= $STEPS"
    set commandLog $commandMessage
    if { $yesIdo } { 
	if { [verify "$commandQuestion"]} {
	    set mbody [binary format cs $commandID $STEPS ]
	    message $mbody $commandMessage $commandLog
	} else {
	    
	}
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fFocusTC \
    -relief ridge \
    -borderwidth 2 \
    -text "Move Focal Plane Stage"

if { $positions::P230On } { 
grid $tab.fFocusTC\
    -in $tab\
    -column $positions::P230X\
    -row $positions::P230Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fFocusTC.refSteps \
    -regexp {^[\-0-9]+$}\
    -labeltext "Motor Steps:"\
    -textvariable "${ns}::STEPS" \
    -textfont {Helvetica 14 bold} \
    -width 6

button $tab.fFocusTC.bFocusTC \
    -text "Move" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.fFocusTC.bFocusTC \
    -side right

pack $tab.fFocusTC.refSteps\
    -side left


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fFocusTC.bFocusTC $tthelp


}

