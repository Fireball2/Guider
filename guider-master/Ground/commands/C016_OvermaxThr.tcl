set ns "C016"
namespace eval $ns {
variable commandID 16


# -----------------------------
# pathname for the controls
# -----------------------------
    set tab $positions::P016Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
--------------------------------
"


variable STEPS 0.5


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Spot Threshold TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to change the overmax Threshhold to $STEPS\n?"

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

    if { $STEPS > 1.0 } { set STEPS 1.0 }
    if { $STEPS < 0.0 } { set STEPS 0.0 }

    set commandQuestion "Do you want to change the overmax Threshold to $STEPS\n?"

    set commandMessage "\[$commandID] $commandName OVERMAX Threshold = $STEPS"
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

ttk::labelframe $tab.fOvermaxThrTC \
    -relief ridge \
    -borderwidth 2 \
    -text "OVERMAX Threshold"

if { $positions::P016On } { 
grid $tab.fOvermaxThrTC\
    -in $tab\
    -column $positions::P016X\
    -row $positions::P016Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fOvermaxThrTC.refSteps \
    -regexp {^[.\-0-9]+$}\
    -labeltext "Overmax Thr:"\
    -textvariable "${ns}::STEPS" \
    -textfont {Helvetica 14 bold} \
    -width 6

button $tab.fOvermaxThrTC.bOvermaxThrTC \
    -text "Overmax Thr" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.fOvermaxThrTC.bOvermaxThrTC \
    -side right

pack $tab.fOvermaxThrTC.refSteps\
    -side left


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fOvermaxThrTC.bOvermaxThrTC $tthelp


}

