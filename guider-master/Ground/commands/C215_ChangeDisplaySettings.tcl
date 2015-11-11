namespace eval C215 {}


# ---------------------------------------------------------------------------
# Command 215 : Change Display Settings
# ---------------------------------------------------------------------------
# Sends display parameters to the guider computer, controlling the brightness,
# contrast and gamma correction


variable C215::dispBrightness 
variable C215::dispContrast 
variable C215::dispGamma 


set tab $positions::P215Tab

proc ChangeDisplaySettings {}  { 
    # frontmatter
    global dispBrightness
    global dispContrast
#    global dispGamma
    set commandID 215
    set commandName "Change Display Settings"
    




    if {$dispBrightness > 255} { 
	set dispBrightness 255
    }
    if {$dispContrast > 255} {
	set dispContrast 255
    }
    if { $C215::dispGamma >255} {
	set C215::dispGamma 255
    }
    

    set commandQuestion "Are you sure you want to change\n the display settings to:\n\tbrightness = $dispBrightness\n\tcontrast = $dispContrast\n\tgamma = $C215::dispGamma"

    set commandMessage "\[$commandID] $commandName B=$dispBrightness, C=$dispContrast, G=$C215::dispGamma"
    set commandLog $commandMessage
    #back end
    if { [verify "$commandQuestion" ] } {
	set mbody [binary format cccc $commandID $dispBrightness $dispContrast $C215::dispGamma]
	message $mbody $commandMessage $commandLog
	
    }


}




# ----------------------------------------
# Widgets for Message 215
# ----------------------------------------

ttk::labelframe $tab.f  \
    -text "Display Settings"\
    -relief ridge \
    -borderwidth 2 
  
#label $tab.f.labelDisplaySettings -text "Display Settings" -fg #DD7777


iwidgets::regexpfield $tab.f.refBrightness \
    -labeltext "Bright:"\
    -fixed 3\
    -regexp {^[0-9]+$}\
    -width 5 \
    -textvariable dispBrightness

set dispBrightness 128

#$displayTab.f.refBrightness insert 0 128


#puts [$displayTab.f.refBrightness get]

button $tab.f.bDisplaySettings \
    -text {Send} \
    -command "ChangeDisplaySettings"




iwidgets::regexpfield $tab.f.refContrast \
    -labeltext "Contrast:"\
    -fixed 3\
    -regexp {^[0-9]+$}\
    -width 5\
    -textvariable dispContrast

set dispContrast 64

iwidgets::regexpfield $tab.f.refGamma \
    -labeltext "Gamma:"\
    -fixed 3\
    -regexp {^[0-9]+$}\
    -width 5 \
    -textvariable C215::dispGamma
 
set C215::dispGamma 128

grid $tab.f  \
    -in $tab \
    -column $positions::P215X \
    -row $positions::P215Y \
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY


pack $tab.f.bDisplaySettings -side right 
pack $tab.f.refBrightness $tab.f.refContrast  $tab.f.refGamma -side left -fill x -expand 1


# ----------------------------------------
# Tooltips for Message 215
# ----------------------------------------

tooltip::tooltip $tab.f.refBrightness \
    "Command ID 215: 
-----------------------------------
Display Brightness (0 to 255)
Neutral Value = 128"

tooltip::tooltip $tab.f.refContrast \
    "Command ID 215: 
-----------------------------------
Display Contrast (0 to 255)
Neutral Value = 64"

tooltip::tooltip $tab.f.refGamma \
    "Command ID 215: 
-----------------------------------
Display Gamma (0 to 255)
Neutral Value = 128"



tooltip::tooltip $tab.f.bDisplaySettings \
    "Command ID 215: 
-----------------------------------
Sends the display settings to
the guider computer. 

newV = ((B-128)+(C/64)oldV)^(G/128)"


