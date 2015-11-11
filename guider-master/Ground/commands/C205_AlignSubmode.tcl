set ns "C205"
namespace eval $ns {
variable commandID 205


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P205Tab
set suffix AlignSubmode

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
------------------------------
Selects the submode for alignment 
mode, including the window location
and exposure times."

    variable sm {FullChipTrack}
    variable xc 600
    variable yc 500
    variable d 100
    variable tt 110
    variable et 100000
    
# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Alignment SubMode"
variable commandMessage "\[$commandID] $commandName, $sm, $xc, $yc, $d, $tt, $et"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change 
the submode to:
sm = $sm
\(x,y,d) = \( $xc, $yc, $d )
\(tt,et) = \( $tt, $et )"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable sm
    variable xc
    variable yc
    variable d
    variable tt
    variable et


    if { $d > 100 } { set d 100 }
    if { $d < 20 } { set d 20 }

    if { $xc < 100 } {set xc 100 }
    if { $xc > 1259 } {set xc 1259 } 

    if { $yc < 100 } { set yc 100 } 
    if { $yc > 935 } { set yc 935 } 

    if { $tt < 33 } { set tt 33 } 
    if { $tt > 255 } { set tt 255 } 

    if { $et < 100 } { set et 100 } 
    if { $et > [expr { ($tt-3)*1000 } ] } { set et [expr ($tt-3)*1000 ] } 


    set commandQuestion \
	"Do you want to change the submode to 
 sm = $sm
\(x,y,d) = \( $xc, $yc, $d )
\(tt,et) = \( $tt ms, $et us )"


    set val(FullChipTrack) 0
    set val(WindowTrack) 1
    set val(WindowDrag) 3
    set val(ROITrack) 2
    

set commandMessage "\[$commandID] $commandName, $sm, $xc, $yc, $d, $tt, $et"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccsssci $commandID $val($sm) $xc $yc $d $tt $et ]
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
    -text "Alignment Sub Mode TC"


if { $positions::P205On } { 
grid $tab.f$suffix -in $tab \
-row $positions::P205Y\
-column $positions::P205X\
-columnspan 2\
-rowspan 2\
-sticky ew\
-padx $positions::padX\
-pady $positions::padY
}

    ttk::combobox $tab.f$suffix.c$suffix\
	-values "{FullChipTrack} {WindowTrack} {WindowDrag} {ROITrack}"\
	-textvariable "${ns}::sm"\
	-width 15\
	-justify  center\
	-font {Helvetica 12 bold}


iwidgets::regexpfield $tab.f$suffix.refXC \
    -regexp {^[0-9]+$}\
    -labeltext "x:"\
    -textvariable "${ns}::xc" \
    -width 5

iwidgets::regexpfield $tab.f$suffix.refYC \
    -regexp {^[0-9]+$}\
    -labeltext "y:"\
    -textvariable "${ns}::yc" \
    -width 5

iwidgets::regexpfield $tab.f$suffix.refD \
    -regexp {^[0-9]+$}\
    -labeltext "d:"\
    -textvariable "${ns}::d"\
    -width 5

iwidgets::regexpfield $tab.f$suffix.refTT \
    -regexp {^[0-9]+$}\
    -labeltext "TrigT:"\
    -textvariable "${ns}::tt"\
    -width 5

iwidgets::regexpfield $tab.f$suffix.refET \
    -regexp {^[0-9]+$}\
    -labeltext "ExpT:"\
    -textvariable "${ns}::et"\
    -width 7
    

button $tab.f$suffix.b$suffix \
    -text "Align SubMode" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15


grid $tab.f$suffix.b$suffix \
-in $tab.f$suffix\
-column 4\
-row 2\
-sticky ew

grid $tab.f$suffix.refXC\
-in $tab.f$suffix\
-column 1\
-row 1\
-sticky ew

grid $tab.f$suffix.refYC\
-in $tab.f$suffix\
-column 2\
-row 1\
-sticky ew

grid $tab.f$suffix.refD\
-in $tab.f$suffix\
-column 1\
-row 2\
-sticky ew

grid $tab.f$suffix.refTT\
-in $tab.f$suffix\
-column 3\
-row 1\
-sticky ew

grid $tab.f$suffix.refET\
-in $tab.f$suffix\
-column 3\
-row 2\
-sticky ew

grid $tab.f$suffix.c$suffix\
-in $tab.f$suffix\
-column 4\
-row 1\
-sticky ew





# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

