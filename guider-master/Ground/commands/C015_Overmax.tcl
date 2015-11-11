set ns "C015"
namespace eval $ns {
variable commandID 15


# -----------------------------
# pathname for the controls
# -----------------------------
    set tab $positions::P015Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
--------------------------------
Change the number of pixels needed
over the to consider something a star."


variable STEPS 3.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Overmax TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to set OVERMAX to   \n    $STEPS?"

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

    if { $STEPS > 10 } { set STEPS 10 }
    if { $STEPS < 0 } { set STEPS 0 }

    set commandQuestion "Do you want to set OVERMAX to   \n    $STEPS?"


    set commandMessage "\[$commandID] $commandName OVERMAX = $STEPS"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cf $commandID $STEPS ]
	message $mbody $commandMessage $commandLog
    } else {
	
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fOvermaxTC \
    -relief ridge \
    -borderwidth 2 \
    -text "Overmax"

if { $positions::P015On } { 
grid $tab.fOvermaxTC\
    -in $tab\
    -column $positions::P015X\
    -row $positions::P015Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fOvermaxTC.refSteps \
    -regexp {^[.\-0-9]+$}\
    -labeltext "Overmax Cnt:"\
    -textvariable "${ns}::STEPS" \
    -textfont {Helvetica 14 bold} \
    -width 6

button $tab.fOvermaxTC.bOvermaxTC \
    -text "Overmax Count" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.fOvermaxTC.bOvermaxTC \
    -side right

pack $tab.fOvermaxTC.refSteps\
    -side left


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fOvermaxTC.bOvermaxTC $tthelp


}

