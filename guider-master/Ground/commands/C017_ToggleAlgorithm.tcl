set ns "C017"
namespace eval $ns {
variable commandID 17


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P017Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Toggles the spot finding algorithm
between the 2009 and 2007 ones."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Toggle Spot Finding"
variable commandQuestion "Do you want toggle the spot algorithm?"
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

ttk::labelframe $tab.fToggleSpotAlgorithm \
    -borderwidth 2 \
    -relief ridge \
    -text "Spot Algorithm"

if { $positions::P017On } { 
grid $tab.fToggleSpotAlgorithm\
    -in $tab\
    -column $positions::P017X\
    -row $positions::P017Y\
    -sticky ew\
    -pady $positions::padY\
    -padx $positions::padX
}

button $tab.fToggleSpotAlgorithm.bToggleSpotAlgorithm \
    -text "Toggle Spot Algorithm" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fToggleSpotAlgorithm.bToggleSpotAlgorithm \
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fToggleSpotAlgorithm.bToggleSpotAlgorithm $tthelp


}

