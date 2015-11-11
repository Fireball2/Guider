set ns "C160"
namespace eval $ns {
variable commandID 160


# -----------------------------
# pathname for the controls
# -----------------------------

set tab $positions::P160Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Requests that the tracking be
switched to guider."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Switching to guider Request"
variable commandQuestion "Do you want to switch to Guider tracking?"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    if { [verify "$commandQuestion"]} {
	set mbody [binary format c $commandID]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fSensor \
    -borderwidth 2 \
    -relief ridge \
    -text "Tracking Sensor"

if { $positions::P160On } { 
grid $tab.fSensor\
    -in $tab\
    -column $positions::P160X\
    -row $positions::P160Y\
    -sticky ew\
    -pady $positions::padY\
    -padx $positions::padX
}


button $tab.fSensor.bGuider \
    -text "Guider" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fSensor.bGuider \
    -side left\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSensor.bGuider $tthelp


}

