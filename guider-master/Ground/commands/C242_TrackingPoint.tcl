set ns "C242"
namespace eval $ns {
variable commandID 242


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P242Tab
set suffix TrackingPoint

# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID 242
---------------------------------
Changes the tracking point
on the chip."



variable x 0
variable y 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Tracking Point"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change the tracking point to:
 (x,y) = ($x, $y)?"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable x
    variable y


    if { $x < 100 } { set x 100 }
    if { $x > 1259 } { set x 1259 }
    if { $y < 100 } { set y 100 }
    if { $y > 935 } { set y 935 }

set commandQuestion \
"Do you want to change the tracking point to:
 (x,y) = ($x, $y)?"    

    
    

    set commandMessage "\[$commandID] $commandName x= $x, y=$y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format css $commandID $x $y]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


proc myfunction2 {} {
    variable x
    variable y 
    
    set x  806
    set y  641
    myfunction 



}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

frame $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -height 100

if { $positions::P242On } { 
grid $tab.f$suffix \
    -in $tab \
    -column $positions::P242X \
    -row $positions::P242Y \
    -columnspan 2 \
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

iwidgets::regexpfield $tab.f$suffix.refx \
    -regexp {^[0-9]+$}\
    -labeltext "x (pix):"\
    -textvariable "${ns}::x" \
    -width 5

iwidgets::regexpfield $tab.f$suffix.refy \
    -regexp {^[0-9]+$}\
    -labeltext "y (pix):"\
    -textvariable "${ns}::y"\
    -width 5
    

button $tab.f$suffix.b$suffix \
    -text "Track Pt" \
    -font {Helvetica 12 bold} \
    -command "${ns}::myfunction"\
    -width 10

button $tab.f$suffix.b2$suffix \
    -text "Fiber Ctr" \
    -font {Helvetica 12 bold} \
    -command "${ns}::myfunction2"\
    -width 10

pack $tab.f$suffix.b$suffix \
    -side right


pack $tab.f$suffix.b2$suffix \
    -side right

pack $tab.f$suffix.refx $tab.f$suffix.refy \
    -fill x \
    -side left\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

