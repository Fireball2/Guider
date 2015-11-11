set ns "C249"
namespace eval $ns {
variable commandID 249


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $alignTab
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
-------------------------------
Calculates the mean offsets that 
the tracked on object is from the 
tracking point and adjusts the 
voltages accordingly to center 
it."

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "FixOffset"
variable commandQuestion "Do you want to issue Fix Offset"
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

ttk::labelframe $tab.fFixOffset \
    -borderwidth 2 \
    -relief ridge \
    -text "Fix Offset"

if { $positions::P249On } { 
grid $tab.fFixOffset\
    -in $tab\
    -row $positions::P249Y\
    -column $positions::P249X\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

button $tab.fFixOffset.bFixOffset \
    -text "Fix Offset" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fFixOffset.bFixOffset



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fFixOffset.bFixOffset $tthelp


}

