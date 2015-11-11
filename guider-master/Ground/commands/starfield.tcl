namespace eval starfield {


#--------------------
#

    proc SolveField {} {


	#--------------------------------------------------------------
	# Solve Field Handling
	#--------------------------------------------------------------
	# no error checking yet. Let's see how that goes... 
	
	
	set sffile [open "./starfields/number.dat" r]
	set val [read $sffile]
	set var::fbcnt $val
	puts "The number is $var::fbcnt"
	close $sffile
	
	if {$var::bcnt < 4 } { 
	    tk_messageBox  -message "I cowardly refuse to try to solve\nthe field with fewer than 4 stars" 
	} else { 

	

	

	    set starfilename "field$var::fbcnt.dat"
	    set starfitsname "field$var::fbcnt.fits"
	    set staroutfile "$var::fbcnt.sol"
	    set starfile [open "./starfields/$starfilename" w]




	    for { set i 0 } { $i < $var::bcnt } { incr i } { 

		
		set bx var::bx$i
		set bx [expr $$bx]
		
		set by var::by$i
		set by [expr $$by]
		
		set bf var::bf$i
		set bf [expr $$bf]
		
		puts "$i $var::bcnt"
		puts "$bx $by $bf "
		puts  $starfile "$bx\t$by\t$bf" 
	    }

	    #------
	    # first we turn the x y f file into a fits file.


	    close $starfile



	    exec /usr/local/astrometry/bin/xylist2fits "./starfields/$starfilename" "./starfields/$starfitsname"
	    exec /usr/local/astrometry/bin/solve-field --depth 10 --no-fits2fits --no-plots --height 1036 --width 1360 "./starfields/$starfitsname" ">./starfields/$staroutfile" -E 2 &
	    


      


	}

	set var::fbcnt [expr $var::fbcnt+1]
	set fn [open "./starfields/number.dat" w]
	puts -nonewline $fn $var::fbcnt 
	puts "The number now is $var::fbcnt" 
	close $fn
	
    }


}