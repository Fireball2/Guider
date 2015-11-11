set ns "C222"
namespace eval $ns {
variable commandID 222


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBSatVal


# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
-----------------------------------
Sets the Saturation Value parameter 
for the frameblob procedures. See 
blobs.h and FrameBlob.h."


variable fb 0


# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Saturatin Value"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Saturation Value to:
   SatVal = $fb"

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
    if { $fb < 1 } { set fb 1 }
    if { $fb > 255 } { set fb 255 }


variable commandQuestion \
"Command ID $commandID
---------------------------
Do you want to set the
FrameBlob Saturation Value to:

   Satval = $fb"

    set commandMessage "\[$commandID] $commandName SatVal= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format c $commandID $fb ]
	message $mbody $commandMessage $commandLog
    } else {
	.stars create oval 100 200 300 200 -outline green -fill red
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -text "FrameBlob Saturation Value"\


grid $tab.f$suffix -in $tab -column 2 -row 1 -sticky ew


iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[0-9]+$}\
    -labeltext "SatVal:"\
    -textvariable "${ns}::fb" \
    -textfont {Helvetica 14 bold} \
    -width 5

button $tab.f$suffix.b$suffix \
    -text "Send" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.f$suffix.b$suffix \
    -side right

pack $tab.f$suffix.ref$suffix\
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

