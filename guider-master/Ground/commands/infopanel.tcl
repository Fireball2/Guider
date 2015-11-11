namespace eval infopanel {

# -- ------ GPS
    set tab $infoTab
    
    ttk::labelframe $tab.gpsframe\
	-text "GPS Information"

    ttk::label $tab.gpsframe.latitudetxt\
	-text "Latitude:"\
	-justify left

    ttk::label $tab.gpsframe.longitudetxt\
	-text "Longitude:"\
	-justify left

    ttk::label $tab.gpsframe.altitudetxt\
	-text "Altitude:"\
	-justify left

    ttk::label $tab.gpsframe.latitude\
	-justify left\
	-width 9\
	-textvariable var::gpslat

    ttk::label $tab.gpsframe.longitude\
	-justify left\
	-width 9\
	-textvariable var::gpslon

    ttk::label $tab.gpsframe.altitude\
	-justify left\
	-width 9\
	-textvariable var::gpsalt

    grid $tab.gpsframe.latitudetxt\
	-in $tab.gpsframe\
	-column 1\
	-row 1\
	-sticky ew

    grid $tab.gpsframe.longitudetxt\
	-in $tab.gpsframe\
	-column 1\
	-row 2\
	-sticky ew

    grid $tab.gpsframe.altitudetxt\
	-in $tab.gpsframe\
	-column 1\
	-row 3\
	-sticky ew



    grid $tab.gpsframe.latitude\
	-in $tab.gpsframe\
	-column 2\
	-row 1

    grid $tab.gpsframe.longitude\
	-in $tab.gpsframe\
	-column 2\
	-row 2

    grid $tab.gpsframe.altitude\
	-in $tab.gpsframe\
	-column 2\
	-row 3

    grid $tab.gpsframe\
	-in $tab\
	-column 1\
	-row 1\
	-rowspan 3\
	-sticky nsew
    

# ----------------- Frame Information
    
    ttk::labelframe $tab.frameInfo\
	-text "Image Information"

    grid $tab.frameInfo\
	-in $tab\
	-column 2\
	-row 1\
	-rowspan 3\
	-sticky nsew

    #----x
    ttk::label $tab.frameInfo.xtxt\
	-text "x:"\
	-justify left

    ttk::label $tab.frameInfo.x\
	-textvariable var::framex\
	-justify right

    grid $tab.frameInfo.xtxt\
	-in $tab.frameInfo\
	-column 1\
	-row 1\
	-sticky ew

    grid $tab.frameInfo.x\
	-in $tab.frameInfo\
	-column 2\
	-row 1\
	-sticky ew

    #----y
    ttk::label $tab.frameInfo.ytxt\
	-text "y:"\
	-justify left

    ttk::label $tab.frameInfo.y\
	-textvariable var::framey\
	-justify right\
	-width 6

    grid $tab.frameInfo.ytxt\
	-in $tab.frameInfo\
	-column 1\
	-row 2\
	-sticky ew

    grid $tab.frameInfo.y\
	-in $tab.frameInfo\
	-column 2\
	-row 2\
	-sticky ew


    #----dx
    ttk::label $tab.frameInfo.dxtxt\
	-text "dx:"\
	-justify left

    ttk::label $tab.frameInfo.dx\
	-textvariable var::framedx\
	-justify right\
	-width 6

    grid $tab.frameInfo.dxtxt\
	-in $tab.frameInfo\
	-column 3\
	-row 1\
	-sticky ew

    grid $tab.frameInfo.dx\
	-in $tab.frameInfo\
	-column 4\
	-row 1\
	-sticky ew

    #----y
    ttk::label $tab.frameInfo.dytxt\
	-text "dy:"\
	-justify left

    ttk::label $tab.frameInfo.dy\
	-textvariable var::framedy\
	-justify right\
	-width 6

    grid $tab.frameInfo.dytxt\
	-in $tab.frameInfo\
	-column 3\
	-row 2\
	-sticky ew

    grid $tab.frameInfo.dy\
	-in $tab.frameInfo\
	-column 4\
	-row 2\
	-sticky ew

    #----tt
    ttk::label $tab.frameInfo.tttxt\
	-text "TT: "\
	-justify left

    ttk::label $tab.frameInfo.tt\
	-textvariable var::framett\
	-justify right\
	-width 6

    grid $tab.frameInfo.tttxt\
	-in $tab.frameInfo\
	-column 1\
	-row 3\
	-sticky ew

    grid $tab.frameInfo.tt\
	-in $tab.frameInfo\
	-column 2\
	-row 3\
	-sticky ew


    #----et
    ttk::label $tab.frameInfo.ettxt\
	-text "ET: "\
	-justify left

    ttk::label $tab.frameInfo.et\
	-textvariable var::frameet\
	-justify right

    grid $tab.frameInfo.ettxt\
	-in $tab.frameInfo\
	-column 3\
	-row 3\
	-sticky ew

    grid $tab.frameInfo.et\
	-in $tab.frameInfo\
	-column 4\
	-row 3\
	-sticky ew

#-------------------------- Tracking
    ttk::labelframe $tab.trackinfo\
	-text "Tracking Info"
       
    
    grid $tab.trackinfo\
	-in $tab\
	-column 1\
	-row 4\
	-rowspan 4

    #------- wx

    ttk::label $tab.trackinfo.wxtxt\
	-text "Wx: "\
	-justify left

    ttk::label $tab.trackinfo.wx\
	-textvariable var::wx\
	-justify right\
	-width 6

    grid $tab.trackinfo.wxtxt\
	-in $tab.trackinfo\
	-column 1\
	-row 1

    grid $tab.trackinfo.wx\
	-in $tab.trackinfo\
	-column 2\
	-row 1

    #------- wy

    ttk::label $tab.trackinfo.wytxt\
	-text "Wy: "\
	-justify left

    ttk::label $tab.trackinfo.wy\
	-textvariable var::wy\
	-justify right\
	-width 6

    grid $tab.trackinfo.wytxt\
	-in $tab.trackinfo\
	-column 1\
	-row 2

    grid $tab.trackinfo.wy\
	-in $tab.trackinfo\
	-column 2\
	-row 2



#------ wdx


    ttk::label $tab.trackinfo.wdxtxt\
	-text "Wdx: "\
	-justify left

    ttk::label $tab.trackinfo.wdx\
	-textvariable var::wdx\
	-justify right\
	-width 6

    grid $tab.trackinfo.wdxtxt\
	-in $tab.trackinfo\
	-column 3\
	-row 1

    grid $tab.trackinfo.wdx\
	-in $tab.trackinfo\
	-column 4\
	-row 1

#------ wdy


    ttk::label $tab.trackinfo.wdytxt\
	-text "Wdy: "\
	-justify left

    ttk::label $tab.trackinfo.wdy\
	-textvariable var::wdy\
	-justify right\
	-width 6

    grid $tab.trackinfo.wdytxt\
	-in $tab.trackinfo\
	-column 3\
	-row 2

    grid $tab.trackinfo.wdy\
	-in $tab.trackinfo\
	-column 4\
	-row 2

    #------- tx

    ttk::label $tab.trackinfo.txtxt\
	-text "Tx: "\
	-justify left

    ttk::label $tab.trackinfo.tx\
	-textvariable var::tx\
	-justify right\
	-width 6

    grid $tab.trackinfo.txtxt\
	-in $tab.trackinfo\
	-column 1\
	-row 3

    grid $tab.trackinfo.tx\
	-in $tab.trackinfo\
	-column 2\
	-row 3

    #------- wy

    ttk::label $tab.trackinfo.tytxt\
	-text "Ty: "\
	-justify left

    ttk::label $tab.trackinfo.ty\
	-textvariable var::ty\
	-justify right\
	-width 6

    grid $tab.trackinfo.tytxt\
	-in $tab.trackinfo\
	-column 3\
	-row 3

    grid $tab.trackinfo.ty\
	-in $tab.trackinfo\
	-column 4\
	-row 3



#----------------- modes tab

    ttk::labelframe $tab.mdframe\
	-text "Mode Information"

    ttk::label $tab.mdframe.mdtxt\
	-text "Mode: "\
	-justify left

    ttk::label $tab.mdframe.smtxt\
	-text "Submode: "\
	-justify left

    ttk::label $tab.mdframe.astxt\
	-text "Active Sensor:"\
	-justify left

    ttk::label $tab.mdframe.md\
	-justify left\
	-width 9\
	-textvariable var::md

    ttk::label $tab.mdframe.sm\
	-justify left\
	-width 9\
	-textvariable var::sm

    ttk::label $tab.mdframe.as\
	-justify left\
	-width 9\
	-textvariable var::as

    grid $tab.mdframe.mdtxt\
	-in $tab.mdframe\
	-column 1\
	-row 1\
	-sticky ew

    grid $tab.mdframe.smtxt\
	-in $tab.mdframe\
	-column 1\
	-row 2\
	-sticky ew

    grid $tab.mdframe.astxt\
	-in $tab.mdframe\
	-column 1\
	-row 3\
	-sticky ew



    grid $tab.mdframe.md\
	-in $tab.mdframe\
	-column 2\
	-row 1

    grid $tab.mdframe.sm\
	-in $tab.mdframe\
	-column 2\
	-row 2

    grid $tab.mdframe.as\
	-in $tab.mdframe\
	-column 2\
	-row 3

    grid $tab.mdframe\
	-in $tab\
	-column 1\
	-row 0\
	-sticky nsew


#status 
    ttk::labelframe $tab.fStatus\
	-text "Other Status"

    grid $tab.fStatus\
	-in $tab\
	-column 2\
	-row 5

    ttk::label $tab.fStatus.lDACtxt\
	-text "DAC: "\
	-justify left

    ttk::label $tab.fStatus.lDAC\
	-textvariable var::dac\
	-justify right

    grid $tab.fStatus.lDACtxt\
	-in $tab.fStatus\
	-column 1\
	-row 1\
	-sticky ew

    grid $tab.fStatus.lDAC\
	-in $tab.fStatus\
	-column 2\
	-row 1\
	-sticky ew

    ttk::label $tab.fStatus.lLEDtxt\
	-text "Light: "\
	-justify left

    ttk::label $tab.fStatus.lLED\
	-textvariable var::led\
	-justify right

    grid $tab.fStatus.lLEDtxt\
	-in $tab.fStatus\
	-column 1\
	-row 2\
	-sticky ew

    grid $tab.fStatus.lLED\
	-in $tab.fStatus\
	-column 2\
	-row 2\
	-sticky ew

#---- LVDT

    ttk::label $tab.fStatus.lLVDTtxt\
	-text "LVDT: "\
	-justify left

    ttk::label $tab.fStatus.lLVDT\
	-textvariable var::lvdt\
	-justify right\
	-width 8\
	-padding 10

    ttk::label $tab.fStatus.lLVDTVolt\
	-textvariable var::lvdtvolt\
	-justify right\
	-width 8\
	-padding 10

    grid $tab.fStatus.lLVDTtxt\
	-in $tab.fStatus\
	-column 1\
	-row 3\
	-sticky ew

    grid $tab.fStatus.lLVDT\
	-in $tab.fStatus\
	-column 2\
	-row 3\
	-sticky ew

    grid $tab.fStatus.lLVDTVolt\
	-in $tab.fStatus\
	-column 3\
	-row 3\
	-sticky ew

	
 
# -- blobs
    ttk::labelframe $tab.fblobs\
	-text "Blobs"
    grid $tab.fblobs\
	-in $tab\
	-column 1\
	-row 10\
	-columnspan 2\
	-sticky nsew


    ttk::label $tab.fblobs.tCounttxt\
	-text "Blob #"
    ttk::label $tab.fblobs.tCount\
	-textvariable var::bcnt
    grid $tab.fblobs.tCounttxt\
	-in $tab.fblobs\
	-column 1\
	-row 0\
	-columnspan 2\
	-sticky ew
    grid $tab.fblobs.tCount\
	-in $tab.fblobs\
	-column 3\
	-row 0\
	-sticky ew
  
    ttk::label $tab.fblobs.blob0x\
	-textvariable var::bx0\
	-width 5
    ttk::label $tab.fblobs.blob0y\
	-textvariable var::by0\
    	-width 5
    ttk::label $tab.fblobs.blob0f\
	-textvariable var::bf0\
    	-width 5

    grid $tab.fblobs.blob0x\
	-in $tab.fblobs\
	-column 1\
	-row 1\
	-sticky ew
    
    grid $tab.fblobs.blob0y\
	-in $tab.fblobs\
	-column 2\
	-row 1\
	-sticky ew

    grid $tab.fblobs.blob0f\
	-in $tab.fblobs\
	-column 3\
	-row 1\
	-sticky ew

    ttk::label $tab.fblobs.blob1x\
	-textvariable var::bx1
    ttk::label $tab.fblobs.blob1y\
	-textvariable var::by1
    ttk::label $tab.fblobs.blob1f\
	-textvariable var::bf1

    grid $tab.fblobs.blob1x\
	-in $tab.fblobs\
	-column 1\
	-row 2\
	-sticky ew
    grid $tab.fblobs.blob1y\
	-in $tab.fblobs\
	-column 2\
	-row 2\
	-sticky ew
    grid $tab.fblobs.blob1f\
	-in $tab.fblobs\
	-column 3\
	-row 2\
	-sticky ew

    ttk::label $tab.fblobs.blob2x\
	-textvariable var::bx2
    ttk::label $tab.fblobs.blob2y\
	-textvariable var::by2
    ttk::label $tab.fblobs.blob2f\
	-textvariable var::bf2

    grid $tab.fblobs.blob2x\
	-in $tab.fblobs\
	-column 1\
	-row 3\
	-sticky ew
    grid $tab.fblobs.blob2y\
	-in $tab.fblobs\
	-column 2\
	-row 3\
	-sticky ew
    grid $tab.fblobs.blob2f\
	-in $tab.fblobs\
	-column 3\
	-row 3\
	-sticky ew


    ttk::label $tab.fblobs.blob3x\
	-textvariable var::bx3
    ttk::label $tab.fblobs.blob3y\
	-textvariable var::by3
    ttk::label $tab.fblobs.blob3f\
	-textvariable var::bf3

    grid $tab.fblobs.blob3x\
	-in $tab.fblobs\
	-column 1\
	-row 4\
	-sticky ew
    grid $tab.fblobs.blob3y\
	-in $tab.fblobs\
	-column 2\
	-row 4\
	-sticky ew
    grid $tab.fblobs.blob3f\
	-in $tab.fblobs\
	-column 3\
	-row 4\
	-sticky ew

    ttk::label $tab.fblobs.blob4x\
	-textvariable var::bx4
    ttk::label $tab.fblobs.blob4y\
	-textvariable var::by4
    ttk::label $tab.fblobs.blob4f\
	-textvariable var::bf4

    grid $tab.fblobs.blob4x\
	-in $tab.fblobs\
	-column 1\
	-row 5\
	-sticky ew
    grid $tab.fblobs.blob4y\
	-in $tab.fblobs\
	-column 2\
	-row 5\
	-sticky ew
    grid $tab.fblobs.blob4f\
	-in $tab.fblobs\
	-column 3\
	-row 5\
	-sticky ew

    ttk::label $tab.fblobs.blob5x\
	-textvariable var::bx5
    ttk::label $tab.fblobs.blob5y\
	-textvariable var::by5
    ttk::label $tab.fblobs.blob5f\
	-textvariable var::bf5

    grid $tab.fblobs.blob5x\
	-in $tab.fblobs\
	-column 1\
	-row 6\
	-sticky ew
    grid $tab.fblobs.blob5y\
	-in $tab.fblobs\
	-column 2\
	-row 6\
	-sticky ew
    grid $tab.fblobs.blob5f\
	-in $tab.fblobs\
	-column 3\
	-row 6\
	-sticky ew

    ttk::label $tab.fblobs.blob6x\
	-textvariable var::bx6
    ttk::label $tab.fblobs.blob6y\
	-textvariable var::by6
    ttk::label $tab.fblobs.blob6f\
	-textvariable var::bf6

    grid $tab.fblobs.blob6x\
	-in $tab.fblobs\
	-column 1\
	-row 7\
	-sticky ew
    grid $tab.fblobs.blob6y\
	-in $tab.fblobs\
	-column 2\
	-row 7\
	-sticky ew
    grid $tab.fblobs.blob6f\
	-in $tab.fblobs\
	-column 3\
	-row 7\
	-sticky ew

    ttk::label $tab.fblobs.blob7x\
	-textvariable var::bx7
    ttk::label $tab.fblobs.blob7y\
	-textvariable var::by7
    ttk::label $tab.fblobs.blob7f\
	-textvariable var::bf7

    grid $tab.fblobs.blob7x\
	-in $tab.fblobs\
	-column 1\
	-row 8\
	-sticky ew
    grid $tab.fblobs.blob7y\
	-in $tab.fblobs\
	-column 2\
	-row 8\
	-sticky ew
    grid $tab.fblobs.blob7f\
	-in $tab.fblobs\
	-column 3\
	-row 8\
	-sticky ew

    ttk::label $tab.fblobs.blob8x\
	-textvariable var::bx8
    ttk::label $tab.fblobs.blob8y\
	-textvariable var::by8
    ttk::label $tab.fblobs.blob8f\
	-textvariable var::bf8

    grid $tab.fblobs.blob8x\
	-in $tab.fblobs\
	-column 1\
	-row 9\
	-sticky ew
    grid $tab.fblobs.blob8y\
	-in $tab.fblobs\
	-column 2\
	-row 9\
	-sticky ew
    grid $tab.fblobs.blob8f\
	-in $tab.fblobs\
	-column 3\
	-row 9\
	-sticky ew

    ttk::label $tab.fblobs.blob9x\
	-textvariable var::bx9
    ttk::label $tab.fblobs.blob9y\
	-textvariable var::by9
    ttk::label $tab.fblobs.blob9f\
	-textvariable var::bf9

    grid $tab.fblobs.blob9x\
	-in $tab.fblobs\
	-column 1\
	-row 10\
	-sticky ew
    grid $tab.fblobs.blob9y\
	-in $tab.fblobs\
	-column 2\
	-row 10\
	-sticky ew
    grid $tab.fblobs.blob9f\
	-in $tab.fblobs\
	-column 3\
	-row 10\
	-sticky ew
    

# solve information

    ttk::button $tab.fblobs.fSolve\
	-text "Solve Field"\
	-command starfield::SolveField

    grid $tab.fblobs.fSolve\
	-in $tab.fblobs \
	-row 1\
	-column 4\
	-sticky NS \
	-rowspan 10
	

# blob information

    ttk::labelframe $tab.fEL\
	-text "EL Info"

    ttk::label $tab.fEL.lMeanEL\
	-textvariable var::meanel \
	-width 10
    



    ttk::label $tab.fEL.lMeanRMSEL\
	-textvariable var::meanrmsel \
	-width 10

    ttk::label $tab.fEL.lmeanELtxt \
	-text "EL: " \
	-width  8


    ttk::label $tab.fEL.lmeanRMSELtxt \
	-text "RMS EL: " \
	-width 8
    

    grid $tab.fEL\
	-sticky EW\
	-in $tab\
	-row 25\
	-column 1\
	-columnspan 2



    grid $tab.fEL.lmeanELtxt \
	-in $tab.fEL \
	-row 1 \
	-column 1

    grid $tab.fEL.lMeanEL \
	-sticky ew\
	-in $tab.fEL \
	-row 1 \
	-column 2

    grid $tab.fEL.lmeanRMSELtxt \
	-in $tab.fEL \
	-row 1 \
	-column 3


    grid $tab.fEL.lMeanRMSEL \
	-sticky ew\
	-in $tab.fEL \
	-row 1 \
	-column 4


ttk::labelframe $tab.fCE\
	-text "CE Info"

grid $tab.fCE\
	-in $tab\
	-column 1\
	-row 26 \
	-columnspan 2\
	-sticky ew

ttk::label $tab.fCE.lMeanCEtxt\
	-text "CE: "\
	-width 8

ttk::label $tab.fCE.lMeanRMSCEtxt\
	-text "RMS CE: "\
	-width 8




    ttk::label $tab.fCE.lMeanCE\
	-textvariable var::meance\
	-width 10
    

    grid $tab.fCE.lMeanCE \
	-sticky ew\
	-in $tab.fCE \
	-row 1 \
	-column 2

    grid $tab.fCE.lMeanCEtxt \
	-sticky ew\
	-in $tab.fCE \
	-row 1 \
	-column 1

    ttk::label $tab.fCE.lMeanRMSCE\
	-textvariable var::meanrmsce\
	-width 10
    

    grid $tab.fCE.lMeanRMSCE \
	-sticky ew \
	-in $tab.fCE \
	-row 1 \
	-column 4

    grid $tab.fCE.lMeanRMSCEtxt \
	-sticky ew \
	-in $tab.fCE \
	-row 1 \
	-column 3


   


}