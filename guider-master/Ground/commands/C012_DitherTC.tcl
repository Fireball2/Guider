set ns "C012"
namespace eval $ns {
variable commandID 12


# -----------------------------
# pathname for the controls
# -----------------------------
    set tab $positions::P012Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
--------------------------------
Initiates \(or stops) a dither
pattern."


variable STEPS 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Dither TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to dither the with the  \n    $STEPS\npattern?"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable STEPS

    if { $STEPS > 10 } { set STEPS 10 }
    if { $STEPS < 0 } { set STEPS 0 }

    set commandQuestion "Do you want to dither the with the  \n    $STEPS\npattern?"

    set commandMessage "\[$commandID] $commandName pattern = $STEPS"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID $STEPS ]
	message $mbody $commandMessage $commandLog
    } else {
	
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fFocusTC \
    -relief ridge \
    -borderwidth 2 \
    -text "Dither Pattern"

if { $positions::P012On } { 
grid $tab.fFocusTC\
    -in $tab\
    -column $positions::P012X\
    -row $positions::P012Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.fFocusTC.refSteps \
    -regexp {^[\-0-9]+$}\
    -labeltext "Pattern:"\
    -textvariable "${ns}::STEPS" \
    -textfont {Helvetica 14 bold} \
    -width 6

button $tab.fFocusTC.bFocusTC \
    -text "Dither" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.fFocusTC.bFocusTC \
    -side right

pack $tab.fFocusTC.refSteps\
    -side left


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fFocusTC.bFocusTC $tthelp


}

