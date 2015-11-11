set ns "C200"
namespace eval $ns {
variable commandID 200


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::P200Tab
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Puts the guider into INIT mode. 
This can be sent from any other mode. 
In INIT mode we can only look at the display, 
but not do anything with it.
"

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "INIT Mode TC"
variable commandQuestion "Do you want to enter\nINIT mode?"
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
	puts "Mbody is $commandID $mbody"
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------
button $tab.bINITModeTC \
    -text "INIT Mode" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

if { $positions::P200On } { 
grid $tab.bINITModeTC\
    -in $tab\
    -column $positions::P200X\
    -row $positions::P200Y\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY
}



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.bINITModeTC $tthelp


}

