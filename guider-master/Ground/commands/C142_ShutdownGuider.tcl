namespace eval C142 {}

set tab $positions::P142Tab


# ---------------------------------------------------------------------------
# command 142 Shutdown Guider
# ---------------------------------------------------------------------------
# Procedure to shut down the guider computer
proc C142::ShutdownGuider {} { 
    global serial
    #need to verify that the port is open!
    set commandID 142
    set commandQuestion "Are you sure you want to Shutdown the Guider?"
    set commandMessage {[142] Shutdown Guider}
    set commandLog $commandMessage
    set  resp [tk_messageBox  -type yesno -message $commandQuestion]
    if {$resp} {

	set mbody [binary format c $commandID]
	puts $mbody
	message $mbody $commandMessage $commandLog
    } else {

    }
}
# ---------------------------------------
# Button for Message 142
# ---------------------------------------
# the shutdown guider button goes in the computer tab
button $tab.bShutdownGuider \
    -text "Shutdown Guider" \
    -font {Helvetica 16 bold} \
    -command C142::ShutdownGuider 

if { $positions::P142On } { 
pack $tab.bShutdownGuider
}

tooltip::tooltip $tab.bShutdownGuider "Command ID 142\n-------------------\nSends a shutdown command\nto the guider computer."