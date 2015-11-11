set ns "C161"
namespace eval $ns {
variable commandID 161


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P161Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Requests that the tracking be
switched to the DTU."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Switching to DTU Request"
variable commandQuestion "Do you want to switch to DTU tracking?"
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

button $tab.fSensor.bDTU \
    -text "DTU" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fSensor.bDTU \
    -side right\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSensor.bDTU $tthelp


}

