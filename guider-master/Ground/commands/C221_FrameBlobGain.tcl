set ns "C221"
namespace eval $ns {
variable commandID 221


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBGain

# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
--------------------------------
Sets the gain parameter for the 
frameblob procedures. See 
blobs.h and FrameBlob.h."


variable fb 0.0


# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Gain"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Gain to:
   Gain = $fb"

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
    if { $fb < 0.001 } { set fb 0.001 }
    if { $fb > 10 } { set fb 10 }


variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Gain to:
   Gain = $fb"

    set commandMessage "\[$commandID] $commandName Gain= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format d $commandID $fb ]
	message $mbody $commandMessage $commandLog
    } else {
#	.stars create oval 100 200 300 200 -outline green -fill red
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -text "FrameBlob Gain"\


grid $tab.f$suffix -in $tab -column 1 -row 1 -sticky ew


iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[\.0-9]+$}\
    -labeltext "Gain:"\
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

