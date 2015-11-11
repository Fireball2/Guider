set ns "C164"
namespace eval $ns {
variable commandID 164


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P164Tab
set suffix WindowTrack

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
------------------------------
Changes the tracking window and
the tracking point while in one
of the two window guiding 
alignment submodes."

    variable xc 600
    variable yc 500
    variable d 100
    variable tx 500
    variable ty 500
    
# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "WindowTrack"
variable commandMessage "\[$commandID] $commandName, $xc, $yc, $d, $tx, $ty"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change 
window tracking to:
\(x,y,d) = \( $xc, $yc, $d )
\(tx,ty) = \( $tx, $ty )"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable xc
    variable yc
    variable d
    variable tx
    variable ty


    if { $d > 100 } { set d 100 }
    if { $d < 20 } { set d 20 }

    if { $xc < 100 } {set xc 100 }
    if { $xc > 1259 } {set xc 1259 } 

    if { $yc < 100 } { set yc 100 } 
    if { $yc > 935 } { set yc 935 } 

    if { $tx < 100 } { set tx 100 } 
    if { $tx > 1259 } { set tx 1259 } 

    if { $ty < 100 } { set ty 100 } 
    if { $ty > 935 } { set ty 935 }


    set commandQuestion \
"Do you want to change 
window tracking to:
\(x,y,d) = \( $xc, $yc, $d )
\(tx,ty) = \( $tx, $ty )"


set commandMessage "\[$commandID] $commandName, $xc, $yc, $d, $tx, $ty"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format csssss $commandID $xc $yc $d $tx $ty ]
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
    -text "Window Track"

if { $positions::P164On  } { 
grid $tab.f$suffix -in $tab \
-row $positions::P164Y\
-column $positions::P164X\
-columnspan 2\
-rowspan 2\
-sticky ew\
-padx $positions::padX\
-pady $positions::padY

}


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

iwidgets::regexpfield $tab.f$suffix.refTX \
    -regexp {^[0-9]+$}\
    -labeltext "TX:"\
    -textvariable "${ns}::tx"\
    -width 5

iwidgets::regexpfield $tab.f$suffix.refTY \
    -regexp {^[0-9]+$}\
    -labeltext "TY:"\
    -textvariable "${ns}::ty"\
    -width 5
    

button $tab.f$suffix.b$suffix \
    -text "WindowTrack" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"


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

grid $tab.f$suffix.refTX\
-in $tab.f$suffix\
-column 3\
-row 1\
-sticky ew

grid $tab.f$suffix.refTY\
-in $tab.f$suffix\
-column 3\
-row 2\
-sticky ew




# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

