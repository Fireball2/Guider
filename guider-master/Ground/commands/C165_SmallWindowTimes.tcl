set ns "C165"
namespace eval $ns {
variable commandID 165


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P165Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
--------------------------------
Sets the trigger and integration
times for the small window
display in alignment mode. "


variable tt 0
variable et 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Small Window Times"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to switch the trigger
and exposure times for the small
window to:\n    tt=$tt\n    et= $et"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable tt
    variable et

    if { $tt < 33 } { set tt 33 }
    if { $tt > 255 } { set tt 255 }
    if { $et > ($tt-3)*1000 } { set et [expr {($tt-3)*1000}] }
    if { $et < 1 } { set et 1 }

    variable commandQuestion \
	"Do you want to switch the trigger
and exposure times for the small
window to:\n    tt=$tt ms\n    et= $et us"


    set commandMessage "\[$commandID] $commandName tt= $tt ms et= $et us"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cci $commandID $tt $et]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fSmallWindowTime \
    -relief ridge \
    -borderwidth 2 \
    -height 100\
    -text "Times for Small Window"

if { $positions::P165On } { 
grid $tab.fSmallWindowTime\
    -in $tab\
    -row $positions::P165Y\
    -column $positions::P165X\
    -columnspan 2\
    -sticky ew\
-padx $positions::padX\
-pady $positions::padY
    
}

iwidgets::regexpfield $tab.fSmallWindowTime.refTT \
    -regexp {^[0-9]+$}\
    -labeltext "TT (ms): "\
    -textvariable "${ns}::tt" \
    -width 7

iwidgets::regexpfield $tab.fSmallWindowTime.refET \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "ET (us): "\
    -textvariable "${ns}::et"\
    -width 7
    

button $tab.fSmallWindowTime.bSmallWindowTime \
    -text "SW Times" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.fSmallWindowTime.bSmallWindowTime \
    -side right
pack $tab.fSmallWindowTime.refTT $tab.fSmallWindowTime.refET \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSmallWindowTime.bSmallWindowTime $tthelp


}

