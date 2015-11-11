namespace eval readserial  {

    variable buffer "."

    variable startb [ binary format c 0x55 ]
    variable endb [ binary format c 0xAA ]
    variable count 0
    variable lastcnt 0


    proc test {message} {
	variable startb
	variable endb
	

    }

    proc parse {message} {
	variable lastcnt 
#	puts "Process called with $message"
	binary scan $message cssc sb ct len mid
	# convert to unsigned values
	set ct [expr $ct & 0xFFFF ]
	set len [expr $len & 0xFFFF ]
	set mid [expr $mid & 0xFF ]

	set data [string range $message 6 end-2]
	set crc [string range $message end-1 end-1]
#	puts $crc

	if {$ct - $lastcnt != 1} {
	    
	    .lb insert end  "Some messages have been skipped. now=$ct, last=$lastcnt"
	}
	#update message counter.
	set lastcnt $ct


	switch -- $mid {

	    # list of up to the 10 brightest blobs.
	    128 {
		puts Hello
	    }


	    # guider status
	    252 {
		binary scan $data cccissssciccfffssssssf gm gsm as fn rx ry rdx rdy tt et dac fl glat glon galt wx wy wdx wdy tx ty lvdt
		set gm [expr $gm & 0xff]
		set gsm [expr $gsm & 0xff]
		set as [expr $as & 0xff]
		set fn [expr $fn & 0xffffffff]
		set rx [expr $rx & 0xffff]
		set ry [expr $ry & 0xffff]
		set rdx [expr $rdx & 0xffff]
		set rdy [expr $rdy & 0xffff]
		set tt [expr $tt & 0xff]
		set et [expr $et & 0xffffffff]
		set dac [expr $dac & 0xff]
		set fl [expr $fl & 0xff]
		set wx [expr $wx & 0xffff]
		set wy [expr $wy & 0xffff]
		set wdx [expr $wdx & 0xffff]
		set wdy [expr $wdy & 0xffff]
		set tx [expr $tx & 0xffff]
		set ty [expr $ty & 0xffff]

		
		
		set var::md $gm
		set var::sm $gsm
		set var::as $as

		set var::framex $rx
		set var::framey $ry
		set var::framedx $rdx
		set var::framedy $rdy
		
		set var::framett $tt
		set var::frameet $et
		
		set var::gpslat $glat
		set var::gpslon $glon
		set var::gpsalt $galt

		set gpsdat [open "gps.dat" "w"]
		puts $gpsdat "$glat $glon $galt"
		
		close $gpsdat


		set var::wx $wx
		set var::wy $wy
		set var::wdx $wdx
		set var::wdy $wdy

		set var::tx $tx
		set var::ty $ty

		set var::dac $dac
		set var::led $fl
		set var::lvdt $lvdt
		
		set var::lvdtvolt [expr 1.0 * $lvdt / 4096.0 * 10.0]

		 set str "RCVD $ct: 252: $gm, $gsm, $as, $fn, $rx, $ry, $rdx, $rdy, $tt, $et, $dac, $fl, $glat, $glon, $galt, $wx, $wy, $wdx, $wdy, $tx, $ty $lvdt"

		updatefltrackpoint $tx $ty
		updateroi $rx $ry $rdx $rdy
		updatewin $wx $wy $wdx $wdy
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }

	    254 { 
		binary scan $data c response
		set response [expr $response & 0xff]
		set str "RCVD $ct: 254: $response" 
		.lb insert end $str
		.lb itemconfigure [expr [.lb index end] -1] -foreground green
		.lb see [expr [.lb index end]]
		writelog $str
	    }


	    251 { 
		binary scan $data fff acx acy acr
		set var::acx $acx
		set var::acy $acy
		set var::acr $acr
	    }




	    #camera error.
	    253 {
		binary scan $data c err
		set err [expr $err & 0xff]
		set str "RCVD Camera error \# $err."
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end]-1] -foreground red
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }

	    #text error.
	    124 { 
		set str "RCVD \"$data\"."
#		puts $str
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground red
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }

	    250 {
		
		binary scan $data cs2is2is2is2is2is2is2is2is2is2i n b0 f0 b1 f1 b2 f2 b3 f3 b4 f4 b5 f5 b6 f6 b7 f7 b8 f8 b9 f9
		set str "RCVD $ct: 250: $n $b0 $f0 $b1 $f1 $b2 $f2 $b3 $f3 $b4 $f4 $b5 $f5 $b6 $f6 $b7 $f7 $b8 $f8 $b9 $f9"
		writelog $str

		writelog $str
		
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		if { $n > 0 } {
		    set var::bcnt $n
		    for {set i 0 } { $i < 10 } { incr i } {
			if {$i < $n} { 
			    set bx b$i
			    set bx [expr $$bx]
			    set f f$i
			    set f [expr $$f]
			    set var::bx$i [lindex $bx 0]
			    set var::by$i [lindex $bx 1]
			    set var::bf$i $f
			    

			} else {
			    set var::bx$i 0
			    set var::by$i 0
			    set var::bf$i 0
			}

		    };
		    
		} else {
		    set bcnt 0
		}
		updateblobs
	    }
	    
	    255 {
		binary scan $data ff el ce
		

		set str "RCVD $ct: 255: $el, $ce"
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		writelog $str


		set var::meanel [expr 0.98*$var::meanel + 0.02*$el]
		set var::meance [expr 0.98*$var::meance + 0.02*$ce]
		set var::meanrmsel [expr sqrt(0.98*pow($var::meanrmsel,2)+ 0.02*pow($var::meanel - $el,2)) ]
		set var::meanrmsce [expr sqrt(0.98*pow($var::meanrmsce,2)+ 0.02* pow($var::meance - $ce,2)) ]

	    }

	    default {
		set str "RCVD: unknown message ($mid)"
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }


	}
	
    }

    proc parsebuffer {data} { 
	variable buffer
	variable startb
	variable endb
	variable count


	#add the new bytes to the buffer
	set buffer $buffer$data

	set start [ string first $startb $buffer ]




	# if there is no start character, we can't possibly have a valid message. 
	if { $start == -1 } {
	    set buffer "."
	    return
	} 



	
	# let's get rid of the leading useless characters. 
	set buffer [string range $buffer $start end]
	set start 0



	# if the buffer size is less than 8 characters, we can't have a valid message. 
	# there are a total of 7 control bytes, and the smallest possible data 
	# message is 1 byte. In this case just sit there and wait for more. 
	if { [string length $buffer] < 8  } {
	    return
	}

	binary scan $buffer cssc sb count length id 
	#puts "$count $id"

	# if the buffer is shorter than the length indicated in the message, then we can't 
	# have a full message there. should exit and await more input.

#	puts "message length : $length"

	set lb [string length $buffer]

#	binary scan $buffer c$lb vals
#	puts $vals


	if { [string length $buffer] < [expr 8 + $length]} {
	    return
	}

	# so we have enough characters for this to be a message. Is the last character the 
	# appropriate end-byte synchbyte? If it is, cut it out to be a message and do some
	# more processing with it. If it isn't then we can't have a valid message starting
	# where 



	if { [string index $buffer [expr 6+$length]] == $endb } {
#	    puts "Here!"
	    set message [string range $buffer 0 [expr 6+$length]]
	    binary scan $message cssc aa bb cc mid
	    set bb [expr {$bb & 0xFFFF}]
	    set mid [expr {$mid & 0xFF}]
	    set buffer [string range $buffer [expr 6+$length] end]
	    parse $message
	    parsebuffer ""

	} else {
	    set buffer [string range $buffer 1 end]
	    parsebuffer ""
	}
	    
    }

}