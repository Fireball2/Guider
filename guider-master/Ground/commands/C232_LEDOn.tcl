set ns "C232"
namespace eval $ns {
variable commandID 232


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P232Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Turns ON the focal plane 
lights."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "FP Lights ON TC"
variable commandQuestion "Do you want to turn ON the\nfocal plane lights?"
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

ttk::labelframe $tab.fLEDs \
    -borderwidth 2 \
    -relief ridge \
    -text "Focal Plane Lights"

if { $positions::P232On } {
grid  $tab.fLEDs\
    -in $tab\
    -row $positions::P232Y\
    -column $positions::P232X\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

button $tab.fLEDs.bLEDsON \
    -text "ON" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fLEDs.bLEDsON \
    -side left\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLEDs.bLEDsON $tthelp


}

