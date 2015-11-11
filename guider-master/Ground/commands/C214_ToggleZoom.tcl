set ns "C214"
namespace eval $ns {
variable commandID 214


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P214Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Toggles the zoom window in 
autocollimation mode."


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Toggle Zoom"
variable commandQuestion "Do you want toggle Autocollimation zoom?"
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

ttk::labelframe $tab.fToggleZoom \
    -borderwidth 2 \
    -relief ridge \
    -text "Autocollimation Zoom"

if { $positions::P214On } { 
grid $tab.fToggleZoom\
    -in $tab\
    -column $positions::P214X\
    -row $positions::P214Y\
    -sticky ew\
    -pady $positions::padY\
    -padx $positions::padX
}

button $tab.fToggleZoom.bToggleZoom \
    -text "Toggle Zoom" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fToggleZoom.bToggleZoom \
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fToggleZoom.bToggleZoom $tthelp


}

