set ns "C210"
namespace eval $ns {
variable commandID 210


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P210Tab


# -----------------------------
# Help Variables
# -----------------------------
    set tthelp {Command ID 210
------------------------------
Sets the Region of Interest 
for the camera. Specifies
the (x,y) coordinats of the
top left corner of the box in
chip coordinates, and the 
box size (dx,dy).}



variable x 0
variable y 0
variable dx 0
variable dy 0
variable mod 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set ROI TC"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion "Do you want to set the ROI to:
(x,y) = ($x, $y)
(dx,dy) = ($dx, $dy)"


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
    variable dx
    variable y
    variable dy
    variable mod

    set md(INIT) 1
    set md(AUTOC) 2
    set md(ALIGN) 3
    set md(SLEW) 4
    set md(POINT) 5


    if { $dx < 20 } { set dx 20 }
    if { $dx > 200 } { set dx 200 } 
    if { $dy < 20 } { set dy 20 } 
    if { $dy > 200 } { set dy 200 } 
    if { [expr $x + $dx] > 1359 } { set x [expr 1359-$dx] }
    if { [expr $x - $dx] < 0 } { set x [expr $dx] }
    if { [expr $y + $dy] > 1035 } { set y [expr 1035-$dx] }
    if { [expr $y - $dy] < 0 } { set y [expr $dx] }


    puts $mod

    set mdid $md($mod)

   
    set commandQuestion "Do you want to set the ROI 
for mode:
MODE = $mod \($mdid) 
to:
\(x,y) = \($x, $y)
\(dx,dy) = \($dx, $dy)"
    
    set commandMessage "\[$commandID] $commandName mode= $mod \($mdid) x= $x y= $y dx= $dx dy= $dy"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccssss $commandID $mdid $x $y $dx $dy]
	message $mbody $commandMessage $commandLog
    } else {
# do we do anything if the message is not sent? 
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.fSetROITC \
    -relief ridge \
    -borderwidth 2 \
    -height 100 \
    -text "Set Region of Interest TC"


frame $tab.fSetROITC.lF 

frame $tab.fSetROITC.rF \



if { $positions::P210On } { 
grid $tab.fSetROITC \
    -in $tab\
    -column $positions::P210X\
    -columnspan 2\
    -row $positions::P210Y\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}


pack $tab.fSetROITC.lF \
    -side left \
    -expand 1


pack $tab.fSetROITC.rF \
    -side right \
    -expand 1


iwidgets::regexpfield $tab.fSetROITC.lF.refX \
    -regexp {^[0-9]+$}\
    -labeltext "x (pix):"\
    -textvariable "${ns}::x" \
    -textfont {Helvetica 14 bold} \
    -width 5

set x 768


iwidgets::regexpfield $tab.fSetROITC.lF.refDX \
    -regexp {^[0-9]+$}\
    -labeltext "dx(pix):"\
    -textvariable "${ns}::dx"\
    -textfont {Helvetica 14 bold} \
    -width 5
set dx 100

iwidgets::regexpfield $tab.fSetROITC.lF.refY \
    -regexp {^[0-9]+$}\
    -labeltext "y (pix):"\
    -textvariable "${ns}::y" \
    -textfont {Helvetica 14 bold} \
    -width 5
set y 550

iwidgets::regexpfield $tab.fSetROITC.lF.refDY \
    -regexp {^[0-9]+$}\
    -labeltext "dy(pix):"\
    -textvariable "${ns}::dy"\
    -textfont {Helvetica 14 bold} \
    -width 5
set dy 100

button $tab.fSetROITC.rF.bSetROITC \
    -text "Set ROI" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\
    -width 15



ttk::combobox $tab.fSetROITC.rF.comboMode \
    -values "INIT AUTOC ALIGN SLEW POINT" \
    -textvariable "${ns}::mod" \
    -width 10\
    -justify center\
    -font {Helvetica 12 bold} \

set mod INIT






pack $tab.fSetROITC.rF.comboMode \
    -side top \
    -fill x 


pack $tab.fSetROITC.rF.bSetROITC \
    -side bottom


    


pack $tab.fSetROITC.lF.refX $tab.fSetROITC.lF.refDX $tab.fSetROITC.lF.refY $tab.fSetROITC.lF.refDY \
    -fill x \
    -side top\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSetROITC.rF.bSetROITC $tthelp


}

