namespace eval var {


#error count 
    variable errors 0

# stage variables
# these are for reference only. 
    variable stagepos 0.0

    #averages 
    variable meanel 0.0
    variable meance 0.0
    variable meanrmsel 0.0
    variable meanrmsce 0.0

#gps variables
    variable gpslat 0.0
    variable gpslon 0.0
    variable gpsalt 0.0

#frame
    variable number 0
    variable framex 0
    variable framey 0
    variable framedx 0
    variable framedy 0
    variable frameet 0
    variable framett 0

 #window
    variable wx 0
    variable wy 0 
    variable wdx 0
    variable wdy 0
    variable tx 0
    variable ty 0 

    #mode 
    variable md 0
    variable sm 0 
    variable as 0

#status
    variable dac 0
    variable led 0 
    variable lvdt 0 
    variable lvdtvolt 0

#autoollimation
    variable acx 0
    variable acy 0
    variable acr 0

#blobs
    variable bcnt 0

    variable bx0 0
    variable by0 0
    variable bf0 0
    variable bx1 0
    variable by1 0
    variable bf1 0
    variable bx2 0
    variable by2 0
    variable bf2 0
    variable bx3 0
    variable by3 0
    variable bf3 0
    variable bx4 0
    variable by4 0
    variable bf4 0
    variable bx5 0
    variable by5 0
    variable bf5 0
    variable bx6 0
    variable by6 0
    variable bf6 0
    variable bx7 0
    variable by7 0
    variable bf7 0
    variable bx8 0
    variable by8 0
    variable bf8 0
    variable bx9 0
    variable by9 0
    variable bf9 0
    
    variable fbcnt 0


    proc getblobx { n } {
	set x var::bx${n}
	set x [expr $$x]
	return $x
    }

    proc getbloby { n } {
	set x var::by${n}
	set x [expr $$x]
	return $x
    }

    proc getblobf { n } {
	set x var::bf${n}
	set x [expr $$x]
	return $x
    }
    
    


    
}