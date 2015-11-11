namespace eval positions {

set padX 5 
set padY 5

# Feel Lucky
set P011On 1
set P011Tab $alignTab
set P011X 0 
set P011Y 0

# Dither
set P012On 1
set P012Tab $computerTab
set P012X 0 
set P012Y 15

#Switching guider
set P160On 1
set P160Tab $alignTab
set P160X 1
set P160Y 0

#Switching DTU
set P161On 1
set P161Tab $alignTab
set P161X 1
set P161Y 0

#AlignSubMode
set P205On 1
set P205Tab $alignTab
set P205X 0
set P205Y 1

#window track
set P164On 1 
set P164Tab $alignTab
set P164X 0 
set P164Y 3

#nudge tracking
set P219On 1
set P219Tab $alignTab
set P219X 0 
set P219Y 6

#trackpoint
set P242On 1
set P242Tab $alignTab
set P242X 0
set P242Y 5


#max step
set P243On 1
set P243Tab $alignTab
set P243X 0
set P243Y 7

#fix offset 
set P249On 1
set P249Tab $alignTab
set P249X 1
set P249Y 7



#XY Offsets TC
set P206On 1
set P206Tab $alignTab
set P206X 0
set P206Y 8

#switch to Large Window
set P162On 1
set P162Tab $alignTab
set P162X 0
set P162Y 9

#switch to Small Window
set P163On 1
set P163Tab $alignTab
set P163X 0
set P163Y 9

#small window times
set P165On 1 
set P165Tab $alignTab
set P165X 0
set P165Y 10

#large window times
set P166On 1 
set P166Tab $alignTab
set P166X 0
set P166Y 11





#Reboot guider

set P140On 1
set P140Tab $computerTab
set P140X 0
set P149Y 1 

#Reset Display 
set P141On 1
set P141Tab $computerTab
set P141X 0
set P141Y 1

#Shutdown Guider 
set P142On 1
set P142Tab $computerTab
set P142X 0
set P142Y 1






#init mode TC
set P200On 1
set P200Tab $generalTab
set P200X 0
set P200Y 0

#autocollimation mode TC 
set P201On 1 
set P201Tab $generalTab
set P201X 0
set P201Y 1

#alignment mode TC 
set P202On 1 
set P202Tab $generalTab
set P202X 0
set P202Y 2

#set ROI 
set P210On 1
set P210Tab $generalTab
set P210X 0
set P210Y 5

#exposure time
set P211On 1
set P211Tab $generalTab
set P211X 0
set P211Y 7

#focus TC
set P230On 1
set P230Tab $generalTab
set P230X 0
set P230Y 10

#home stage
set P231On 0
set P231Tab $generalTab
set P231X 1
set P231Y 10

#tag log 
set P234On 1 
set P234Tab $generalTab 
set P234X 0
set P234Y 12





#ELCE offsets TC
set P204On 1
set P204Tab $autocTab
set P204X 0
set P204Y 0




#autocollim xy
set P207On 1
set P207Tab $autocTab
set P207X 0
set P207Y 0


#AC zoom
set P214On 1
set P214Tab $autocTab
set P214X 0
set P214Y 0 

#AC zoom
set P014On 1
set P014Tab $autocTab
set P014X 1
set P014Y 0 


#display settings
set P215On 1
set P215Tab $computerTab
set P215X 0
set P215Y 0

#leds
set P232On 1
set P232Tab $autocTab
set P232X 1
set P232Y 1


#DAC output
set P238On 1
set P238Tab $computerTab
set P238X 0
set P238Y 11

#Overmax Count
set P015On 1
set P015Tab $computerTab
set P015X 0
set P015Y 16

#Overmax Threshold
set P016On 1
set P016Tab $computerTab
set P016X 0
set P016Y 17

#Toggle Algorithm
set P017On 1
set P017Tab $computerTab
set P017X 0
set P017Y 21

#Boxes
set P019On 1
set P019Tab $computerTab
set P019X 0
set P019Y 20

#RMS SPot Threshold
set P018On 1
set P018Tab $computerTab
set P018X 0
set P018Y 19












}
