set ns "C231"
namespace eval $ns {
variable commandID 231


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P231Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Sends the focal plane stage 
to the home positon."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Home Stage TC"
variable commandQuestion "Do you want to home the stage?"
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

ttk::labelframe $tab.fAutoFocus \
    -borderwidth 2 \
    -relief ridge \
    -text "Home Focal Plane Stage"

if { $positions::P231On } { 
grid $tab.fAutoFocus\
    -in $tab\
    -column $positions::P231X\
    -row $positions::P231Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY

}

button $tab.fAutoFocus.bAutoFocusTC \
    -text "Home" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fAutoFocus.bAutoFocusTC



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fAutoFocus.bAutoFocusTC $tthelp


}

