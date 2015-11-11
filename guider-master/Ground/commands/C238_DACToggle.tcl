set ns "C238"
namespace eval $ns {
variable commandID 238


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P238Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Toggles the DAC output ON or
OFF."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "DACOutput"
variable commandQuestion "Do you want to toggle the DAC output?"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage

# -----------------------------
# Function
# -----------------------------
proc myfunction {status} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    if { $status == 0} {
	set commandQuestion "Do you want to toggle the DAC output OFF?"
	set commandMessage "\[$commandID] $commandName OFF"
    } else {
	set commandQuestion "Do you want to toggle the DAC output ON?"
	set commandMessage "\[$commandID] $commandName ON"
    }

    variable commandLog $commandMessage

    if { [verify "$commandQuestion"]} {
	if {$status} { 
	    set mbody [binary format cc $commandID 1]
	} else {
	    set mbody [binary format cc $commandID 0]
	}
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fDACOutput \
    -borderwidth 2 \
    -relief ridge \
    -text "DAC Output"

if { $positions::P238On } { 
grid $tab.fDACOutput\
    -in $tab\
    -column $positions::P238X\
    -row $positions::P238Y\
    -sticky ew
}


button $tab.fDACOutput.bDACON \
    -text "ON" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction 1"

button $tab.fDACOutput.bDACOFF \
    -text "OFF" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction 0"

pack $tab.fDACOutput.bDACOFF \
    -side right\


pack $tab.fDACOutput.bDACON \
    -side left\




# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fDACOutput.bDACON $tthelp
tooltip::tooltip $tab.fDACOutput.bDACOFF $tthelp


}

