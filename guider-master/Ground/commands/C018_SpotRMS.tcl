set ns "C018"
namespace eval $ns {
variable commandID 18


# -----------------------------
# pathname for the controls
# -----------------------------
    set tab $positions::P018Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
--------------------------------
"


variable STEPS 10.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Spot RMS Threshold TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to change the spot RMS threshold to $STEPS\n?"

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

    if { $STEPS > 10.0 } { set STEPS 10.0 }
    if { $STEPS < 0.0 } { set STEPS 0.0 }

    set commandQuestion "Do you want to change the spot RMS threshold to $STEPS\n?"


    set commandMessage "\[$commandID] $commandName Spot RMS Threshhold = $STEPS"
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

ttk::labelframe $tab.fSpotRMSThrTC \
    -relief ridge \
    -borderwidth 2 \
    -text "Spot RMS Threshold"

if { $positions::P018On } { 
grid $tab.fSpotRMSThrTC\
    -in $tab\
    -column $positions::P018X\
    -row $positions::P018Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fSpotRMSThrTC.refSteps \
    -regexp {^[.\-0-9]+$}\
    -labeltext "Pattern:"\
    -textvariable "${ns}::STEPS" \
    -textfont {Helvetica 14 bold} \
    -width 6

button $tab.fSpotRMSThrTC.bSpotRMSThrTC \
    -text "RMS Thr." \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.fSpotRMSThrTC.bSpotRMSThrTC \
    -side right

pack $tab.fSpotRMSThrTC.refSteps\
    -side left


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSpotRMSThrTC.bSpotRMSThrTC $tthelp


}

