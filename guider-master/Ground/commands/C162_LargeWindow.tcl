set ns "C162"
namespace eval $ns {
variable commandID 162


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P161Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Switches to the full chip 
view from small window, 
using the same coordinates as 
the small window. This is 
meant primarily when already
tracking."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Switch Large Window"
variable commandQuestion "Do you want to switch to Large Window?"
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

ttk::labelframe $tab.fWindow \
    -borderwidth 2 \
    -relief ridge \
    -text "Window"

if { $positions::P162On } { 
grid $tab.fWindow\
    -in $tab\
    -row $positions::P162Y\
    -column $positions::P162X\
    -columnspan 2\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

button $tab.fWindow.bLarge \
    -text "Large" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fWindow.bLarge \
    -side left\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fWindow.bLarge $tthelp


}

