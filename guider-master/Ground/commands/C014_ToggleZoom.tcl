set ns "C014"
namespace eval $ns {
variable commandID 14


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P014Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Toggles the zoom window type
between centered on spot
and centered on target point"


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Toggle Zoom Type"
variable commandQuestion "Do you want toggle zoom type?"
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

ttk::labelframe $tab.fToggleZoomType \
    -borderwidth 2 \
    -relief ridge \
    -text "Zoom Type"

if { $positions::P014On } { 
grid $tab.fToggleZoomType\
    -in $tab\
    -column $positions::P014X\
    -row $positions::P014Y\
    -sticky ew\
    -pady $positions::padY\
    -padx $positions::padX
}

button $tab.fToggleZoomType.bToggleZoomType \
    -text "Toggle Zoom Type" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

pack $tab.fToggleZoomType.bToggleZoomType \
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fToggleZoomType.bToggleZoomType $tthelp


}

