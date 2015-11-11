namespace eval C141 {}


set tab $positions::P141Tab


# ---------------------------------------------------------------------------
# command 141: Reset Guider Display 
# ---------------------------------------------------------------------------
#procedure to reset the display on the guider
proc C141::ResetDisplay {} { 

#--- frontmatter
    set commandID 141
    set commandQuestion "About to rest the Guider Display. Is this OK?"
    set commandName "Reset Display"
    set commandMessage "\[$commandID] $commandName"
    set commandLog $commandMessage
    set commandTooltip {Sometimes the onboard display does something funky, like appear double. This resets this desplay.}
#--- rear end
   set  resp [tk_messageBox  -type yesno -message $commandQuestion]
    if {$resp} {
	set mbody [binary format c $commandID]
	message $mbody $commandMessage $commandLog
    } else {

    }
}



# ---------------------------------------
# Button for Message 141
# ---------------------------------------
button $tab.bResetDisplay \
    -text "ResetDisplay" \
    -font {Helvetica 16 bold} \
    -command C141::ResetDisplay

if { $positions::P141On } { 
pack $tab.bResetDisplay 
}

# ---------------------------------------
# Tooltip for Message 141
# ---------------------------------------