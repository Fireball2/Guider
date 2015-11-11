set ns "C227"
namespace eval $ns {
variable commandID 227


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBApBox


# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
-----------------------------------
Sets the Aperture Box parameter 
for the frameblob procedures. See 
blobs.h and FrameBlob.h."


variable fb 0


# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Aperture Box"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Aperture Box to:
   AB = $fb"

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
    if { $fb > 2000 } { set fb 2000 }


variable commandQuestion \
"Command ID $commandID
---------------------------
Do you want to set the
FrameBlob Aperture Box to:

   CB = $fb"

    set commandMessage "\[$commandID] $commandName CB= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format i $commandID $fb ]
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
    -text "FrameBlob Aperure Box"\


grid $tab.f$suffix -in $tab -column 2 -row 3 -sticky ew


iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[0-9]+$}\
    -labeltext "ApBox:"\
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

