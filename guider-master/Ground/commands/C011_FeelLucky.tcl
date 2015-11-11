set ns "C011"
namespace eval $ns {
variable commandID 11


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::P011Tab
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 011
-------------------------------
Asks the guider to trap the 
brightest source in the frame
and start tracking on it.
"

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Feeling Lucky TC"
variable commandQuestion "Do you want to issue Feeling Lucky?"
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

# First we make a frame.
# ----------------------------

ttk::labelframe $tab.fFeelLucky \
    -borderwidth 2 \
    -relief ridge \
    -text "Feeling Lucky?"


if { $positions::P011On } {
    grid $tab.fFeelLucky -in $tab -row $positions::P011X -column $positions::P011Y -padx $positions::padX -pady $positions::padY
}

button $tab.fFeelLucky.bFeelLucky \
    -text "I am feeling lucky!" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fFeelLucky.bFeelLucky



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fFeelLucky.bFeelLucky $tthelp


}

