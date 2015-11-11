set ns "C201"
namespace eval $ns {
variable commandID 201


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $generalTab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Puts the guider and telescope into
the Autocollimation mode."

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "AutoCollimation Mode TC"
variable commandQuestion "Do you want to enter\nAutoCollimation mode?"
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
button $tab.bAutoCollimationModeTC \
    -text "AutoCollimation Mode" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

if { $positions::P201On } { 

grid $tab.bAutoCollimationModeTC\
    -in $tab\
    -column $positions::P201X\
    -row $positions::P201Y\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY


}

# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.bINITModeTC $tthelp


}

