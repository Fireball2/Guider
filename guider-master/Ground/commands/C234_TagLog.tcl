set ns "C234"
namespace eval $ns {
variable commandID 234


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P234Tab
set suffix tagLog

# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
---------------------------------
Tags the logs of both the ground 
and flight computers with a numbered
tag. The ground side log also gets a 
string."



variable logNumber 0

    if { [ file exists "logNumber.dat" ] } { 
	
	set fpln [open "logNumber.dat" r]
	set data [read $fpln]
	close $fpln
	
	set logNumber $data
	
    } else { 
	set fpln [open "logNumber.dat" w]
	puts -nonewline $fpln 0
	close $fpln
	set logNumber 0
    }
    

variable logString " "

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Tag Logs"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to tag the logs?"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable logNumber
    variable logString

set commandQuestion \
"Do you want to tag the logs?"

    
    

    set commandMessage "\[$commandID] $commandName, $logNumber, $logString"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cs $commandID $logNumber]
	message $mbody $commandMessage $commandLog
	set logString " "
	set logNumber [expr $logNumber+1]
	set fpln [open "logNumber.dat" w]
	puts -nonewline $fpln $logNumber 
	close $fpln
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

frame $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -height 100

if { $positions::P234On } { 
grid $tab.f$suffix \
    -in $tab \
    -column $positions::P234X \
    -row  $positions::P234Y \
    -columnspan 2 \
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

ttk::label $tab.f$suffix.l$suffix\
    -textvariable "${ns}::logNumber"\
    -justify left

ttk::entry $tab.f$suffix.e$suffix\
    -textvariable "${ns}::logString"

button $tab.f$suffix.b$suffix \
    -text "Send Tag" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 7

pack $tab.f$suffix.b$suffix \
    -side right
pack $tab.f$suffix.l$suffix $tab.f$suffix.e$suffix \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

