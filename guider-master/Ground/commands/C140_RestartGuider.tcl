namespace eval C140 {}

# -------------------------------------------------------------------------
# Message 140 : Reboot Guider
# -------------------------------------------------------------------------
set tab $positions::P140Tab

proc C140::RebootGuider {} { 
# --- Frontmatter
    set commandID 140
    puts $commandID
    set commandName "Guider Reboot"
    set commandQuestion "Are you sure you want to Reboot the Guider?"
    set commandMessage "\[$commandID]  $commandName"
    set commandLog $commandMessage
# --- the Guts.    
    if { [verify "$commandQuestion" ] } {

	set mbody [binary format c $commandID]
	message $mbody $commandMessage $commandLog
    } else {

    }
}

# ---------------------------------------
# Button for Message 140
# ---------------------------------------
button $tab.bRebootGuider \
    -text "RebootGuider" \
    -font {Helvetica 16 bold} \
    -command C140::RebootGuider

if { $positions::P140On } { 
pack $tab.bRebootGuider
}

# ---------------------------------------
# Tooltip for Message 140
# ---------------------------------------
tooltip::tooltip $tab.bRebootGuider "Command ID 140\n-------------------\nSends a reboot command\nto the guider computer."
