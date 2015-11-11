set ns "C233"
namespace eval $ns {
variable commandID 233


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $autocTab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Turns OFF the focal plane 
lights."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "FP Lights OFF TC"
variable commandQuestion "Do you want to turn OFF the\nfocal plane lights?"
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


button $tab.fLEDs.bLEDsOFF \
    -text "OFF" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fLEDs.bLEDsOFF \
    -side right\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLEDs.bLEDsOFF $tthelp


}

