set ns "C211"
namespace eval $ns {
variable commandID 211


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P211Tab
set suffix ExposureTime

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
------------------------------
Sets the exposure time for the 
full chip exposures for the 
specified mode."

    variable sm {INIT}
    variable tt 110
    variable et 100000
    
# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Alignment SubMode"
variable commandMessage "\[$commandID] $commandName, $sm, $tt, $et"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change 
the exposure and trigger times
for mode = $sm
to:
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
    variable tt
    variable et


    if { $tt < 33 } { set tt 33 } 
    if { $tt > 255 } { set tt 255 } 

    if { $et < 100 } { set et 100 } 
    if { $et > [expr { ($tt-3)*1000 } ] } { set et [expr ($tt-3)*1000 ] } 


    set commandQuestion \
"Do you want to change 
the exposure and trigger times
for mode = $sm
to:
\(tt,et) = \( $tt ms, $et us)"



    set val(INIT) 1
    set val(AUTOC) 2
    set val(ALGN) 3

    

set commandMessage "\[$commandID] $commandName, $sm, $tt, $et"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccci $commandID $val($sm) $tt $et ]
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
    -text "Exposure Time TC"


if { $positions::P211On } { 
grid $tab.f$suffix -in $tab \
    -row $positions::P211Y\
    -column $positions::P211X\
    -columnspan 2\
    -rowspan 1\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

ttk::combobox $tab.f$suffix.c$suffix\
    -values "{INIT} {AUTOC} {ALGN}"\
    -textvariable "${ns}::sm"\
    -justify  center\
    -width 6\
    -font {Helvetica 12 bold}


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
    -text "ET/TT" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"



grid $tab.f$suffix.b$suffix \
-in $tab.f$suffix\
-column 4\
-row 1\
-sticky ew

grid $tab.f$suffix.refTT\
-in $tab.f$suffix\
-column 3\
-row 1\
-sticky ew

grid $tab.f$suffix.refET\
-in $tab.f$suffix\
-column 2\
-row 1\
-sticky ew

grid $tab.f$suffix.c$suffix\
-in $tab.f$suffix\
-column 1\
-row 1\
-sticky ew





# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

