set ns "C163"
namespace eval $ns {
variable commandID 163


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P163Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Switches to the small window 
view from full chip, 
using the same coordinates as 
the large. This is 
meant primarily when already
tracking."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Switch Small Window"
variable commandQuestion "Do you want to switch to Small Window?"
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

button $tab.fWindow.bSmall \
    -text "Small" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

if { $positions::P163On } { 
pack $tab.fWindow.bSmall \
    -side right\
    -fill x
}


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fWindow.bSmall $tthelp


}

