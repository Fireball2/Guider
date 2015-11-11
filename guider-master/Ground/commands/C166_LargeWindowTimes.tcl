set ns "C166"
namespace eval $ns {
variable commandID 166


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P166Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
--------------------------------
Sets the trigger and integration
times for the large window
display in alignment mode. "


variable tt 0
variable et 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Large Window Times"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to switch the trigger
and exposure times for the large
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

    if { $tt < 110 } { set tt 110 }
    if { $tt > 255 } { set tt 255 }
    if { $et > [expr {($tt-10)*1000}] } { set et [expr {($tt-10)*1000}] }
    if { $et < 1 } { set et 1 }

    variable commandQuestion \
	"Do you want to switch the trigger
and exposure times for the large
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

ttk::labelframe $tab.fLargeWindowTime \
    -relief ridge \
    -borderwidth 2 \
    -height 100\
    -text "Times for Large Window"

if { $positions::P166On } {
grid $tab.fLargeWindowTime\
    -in $tab\
    -row $positions::P166Y\
    -column $positions::P166X\
    -columnspan 2\
    -sticky ew
}

iwidgets::regexpfield $tab.fLargeWindowTime.refTT \
    -regexp {^[0-9]+$}\
    -labeltext "TT(ms): "\
    -textvariable "${ns}::tt" \
    -width 7

iwidgets::regexpfield $tab.fLargeWindowTime.refET \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "ET(us): "\
    -textvariable "${ns}::et"\
    -width 7
    

button $tab.fLargeWindowTime.bLargeWindowTime \
    -text "LW Times" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15

pack $tab.fLargeWindowTime.bLargeWindowTime \
    -side right
pack $tab.fLargeWindowTime.refTT $tab.fLargeWindowTime.refET \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLargeWindowTime.bLargeWindowTime $tthelp


}

