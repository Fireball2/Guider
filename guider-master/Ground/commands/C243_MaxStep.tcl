set ns "C243"
namespace eval $ns {
variable commandID 243


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P243Tab
set suffix MaxStep

# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
--------------------------------
Sets the size of the maximum 
offset that the guider can send to 
the MPF in window-drag mode."


variable fb 0


# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "Maximum DragStep"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
Max Drag Step to:
   mDS = $fb"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable fb

# some boundary checking
    if { $fb < 20 } { set fb 20 }
    if { $fb > 200 } { set fb 200 }


variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
Max Drag Step to:
   mDS = $fb"

    set commandMessage "\[$commandID] $commandName mDS= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cf $commandID $fb ]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -text "Max Drag Step"\

if { $positions::P243On } { 
grid $tab.f$suffix\
 -in $tab\
 -column $positions::P243X\
 -row $positions::P243Y\
 -sticky ew\
-padx $positions::padX\
-pady $positions::padY
}

iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[\.0-9]+$}\
    -labeltext "MaxStep:"\
    -textvariable "${ns}::fb" \
    -textfont {Helvetica 14 bold} \
    -width 7

button $tab.f$suffix.b$suffix \
    -text "Send" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.f$suffix.b$suffix \
    -side right

pack $tab.f$suffix.ref$suffix\
    -fill x \
    -side left\



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

