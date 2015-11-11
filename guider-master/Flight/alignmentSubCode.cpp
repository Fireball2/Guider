

//#define RASTER

statistic statposx(100);
statistic statposbgnd(100);
statistic statposy(100);
statistic statposf(100);
statistic statposrmsx(100);
statistic statposrmsy(100);


void QCAMAPI AlignmentModeCallback(void *cam, unsigned long info, QCam_Err err, unsigned long){
  int qID;
  static short x,y,dx,dy;
  static int i,j; // counters
  static double sum, sumsq;
  static double leakyrmsx, leakyrmsy;
  static unsigned short idx;



#ifdef  RASTER
  static short rasterx, rastery;
#endif

  static unsigned char val;
  static unsigned short loststar;
  qID = info;
  if(qID > FRAMES){
    WriteToLog("Invalid frame number");
    qID = 0;
  };
  QCamera *camera;
  camera = (QCamera *)cam;
  //  static frameblob fblob;
  unsigned char *buffer;
  static unsigned char max, max2;
  static unsigned short maxx, maxy;
  static unsigned short targetx, targety;
  //  static double targetx_d, targety_d;
  static float correctedtargetx, correctedtargety;
  static double sumx, sumy, sumtot;
  static double rsq,rsqx,rsqy;
  static bool extended;
  static double centx, centy, dist;
  static float distx, disty;
  static float el, ce;
  static double totalflux, lastcentx, lastcenty, lastel, lastce;
  static double bx, by;
  static bloblist *pblob;
  static unsigned short targetblob, currentblob;
  static clock_t startt, endt;
  static unsigned int frmnumber;
  static unsigned char submodeid;
  static unsigned short wx, wdx, wy, wdy;
  static bool guidtrack;
  static bool existblob;
  startt = clock();
  static float trkX, trkY;



  frmnumber = camera->onlyGetFrame();
  idx = frmnumber % 100;
  submodeid = camera->getSubModeID();

  WriteToLog("ALGN","(%d), Processing %ld",submodeid, frmnumber);
  
  //frmnumber = camera->getFrame();
  guidtrack = false;
  existblob = false;
  if(MPFS.PointControl == 0) guidtrack = true;
  StatusToDetector(guidtrack, err);
  FrameNumberToDetector(frmnumber);
  //  ptus = (unsigned char *) &frmnumber;
  


 


  

  //   GtoD_PortB(*ptus);
  //    GtoD_PortA(*ptus);

  if(err){
    GtoT_CameraError(err);
    WriteToLog("ALGN","Error %d",err);
  };

  x = camera->getRoiX();
  y = camera->getRoiY();
  dx = camera->getRoiDX();
  dy = camera->getRoiDY();
  buffer = camera->buffer[qID];


  // XXX hot pixels. should be loaded from a file!


  //  camera->ClearBadPixels(buffer,mean);
    sum = sumsq = 0;
    max = 0;


    alignmentMD.submodeID = submodeid;
    // no matter what submode we are in, so long as it's not a small ROI 
  if(submodeid !=ALIGNTRACKROI){
    buffer[77876] = 0;
    buffer[521476] = 0;
    buffer[562827]=0;
    buffer[533746] = 0;
    
 
    camera->setFrameBlobXpix(CHIPWIDTH);
    camera->setFrameBlobYpix(CHIPHEIGHT);
    camera->fblob[qID].clearblobs();
    camera->fblob[qID].calc_mapstat();
    camera->fblob[qID].calc_mapstat();
    camera->fblob[qID].calc_searchgrid();
    camera->fblob[qID].calc_centroid();
    camera->fblob[qID].calc_flux();
    camera->fblob[qID].fix_multiple();
    camera->fblob[qID].sortblobs();
    pblob = camera->fblob[qID].getblobs();
    targetx = trackpointCE;
    targety = trackpointEL;

    if (dither::isDithering) dither::add(targetx, targety);
    bx = -1000;
    by = -1000;

    if(camera->feellucky && pblob!=NULL){
      float x,y;
      x = pblob->getx();
      y = pblob->gety();
      camera->setNowWinX((unsigned short)x);
      camera->setNowWinY((unsigned short)y);
      camera->setNowWinDX(100);
      camera->setNowWinDY(100);
      trackpointEL = (unsigned short) y;
      trackpointCE = (unsigned short) x;
      
      GtoT_MessageRcvd(GG_SWITCHINGGUIDER);
      WriteToLog("LUCKY","Switching Guider");
      GtoM_SwitchingGuiderRequest();
      camera->setActiveSensor(GUIDERSENSOR);
      camera->feellucky = false;
	
    };


    currentblob = 0;
    targetblob = camera->getTargetBlob();

    wx = camera->getWinX();
    wy = camera->getWinY();
    wdx = camera->getWinDX();
    wdy = camera->getWinDY();

    //    WriteToLog("ALGN","Got the window %d %d %d %d",wx,wy,wdx,wdy);

    static bool trackblobnotthere;
    trackblobnotthere = true;


    if(pblob == NULL){
      
      // for full chip we actually want to send out a zero if we lose
      //      the pointing.  
      //      SendOutEL(-camera->userRoiEL(lastel));
      //      SendOutCE(-camera->userRoiCE(lastce));
      SendOutEL(0);
      SendOutCE(0);
      statposx.insert(0,idx);
      statposy.insert(0,idx);
      // XXX 
      GtoT_PointingErrors(0,0);
      FInfo.send(frmnumber, leakylvdt/4096., 5000.0,-5000.0,camera->getExpT(),err);	
      WriteToLog("ALGN"," (%d): N=0:  %ld, T(%d, %d) , W(%d, %d, %d, %d)  R(%d, %d, %d, %d)",submodeid, frmnumber,targetx+x, targety+y,wx, wy, wdx, wdy, x, y, dx, dy);

    } else {
      while(pblob!=NULL){

	// alright. we want to track on different things if we 
	// are in different submodes. 
	
	// if we are in ALIGNFULLCHIP then we track on what 
	// the software perceives to be the brightest blob

	// if we are in ALIGNTRACKFULLCHIP then we track on a window
	// surrounding the blob that was in question

	// in ALIGNDRAGFULLCHIP we use a window around the blob and
	// slowly bring the spot closer to the tracking point

	//first lets get the blob coordinates
	
	bx = pblob->getx();
	by = pblob->gety();

	// okay. great. now the easy part :P

	if(submodeid == ALIGNTRACKFULLCHIP && trackblobnotthere==true){
	  if(bx>wx-wdx && bx<wx+wdx && by>wy-wdy && by<wy+wdy){
	    // that's our blob!

	    alignmentMD.centX = bx;
	    alignmentMD.centY = by;
	    targetx = wx;
	    targety = wy;

	    if(dither::isDithering) dither::add(targetx, targety);


#ifdef RASTER
	    
	    rasterx = 708 + (frmnumber % (625*20))%25;
	    rastery = 542 + (frmnumber % (625 * 20))/25;
			     
	    targetx = rasterx;
	    targety = rastery;
			     
		
	    printf("Yeah... I was here\n");
			    

#endif 	    
	    


	    correctedtargetx = targetx+camera->gondolaoffsetx;
	    correctedtargety = targety+camera->gondolaoffsety;
	    distx = bx - correctedtargetx;
	    disty = by - correctedtargetx;
	    camera->distybuffer[qID]=disty;
	    camera->distxbuffer[qID]=distx;
	    //	    el = disty;
	    //	    ce = distx;
	    ce = (bx-targetx)*camera->getPlateScale();
	    el = (by-targety)*camera->getPlateScale();
	    FInfo.send(frmnumber, leakylvdt/4096., el,ce,camera->getExpT(),err);
	    GtoT_PointingErrors(el, ce);
	    WriteToLog("ALGN"," (%d):  %ld, T(%d, %d) , W(%d, %d, %d, %d)  R(%d, %d, %d, %d) B(%lf, %lf)",submodeid, frmnumber, targetx+x, targety+y,wx, wy, wdx, wdy, x, y, dx, dy, bx, by);
	    // we don't want to find anymore blobs to track on.
	    goto processedblob;
	  };
	} else	if(submodeid == ALIGNDRAGFULLCHIP && trackblobnotthere==true){
	  targetx = trackpointCE;
	  targety = trackpointEL;
	  if(dither::isDithering) dither::add(targetx, targety);
	  // first check if our star is within the bounding box we
	  // care about:
	  if(bx>wx-wdx && bx<wx+wdx && by>wy-wdy && by<wy+wdy){
	    //groovy, it is.
	    //now see if it is within 40 pixels of the target spot:
	    alignmentMD.centX = bx;
	    alignmentMD.centY = by;
	    correctedtargetx = targetx+camera->gondolaoffsetx;
	    correctedtargety = targety+camera->gondolaoffsety;
	    if(fabs(bx-targetx)<camera->MoveStepSize && fabs(by-targety)<camera->MoveStepSize){
	      distx = bx-correctedtargetx;
	      disty = by-correctedtargety;
	      camera->distybuffer[qID]=disty;
	      camera->distxbuffer[qID]=distx;
	      el = (by-targety)*camera->getPlateScale();
	      ce = (bx-targetx)*camera->getPlateScale();
	      FInfo.send(frmnumber,leakylvdt/4096., el,ce,camera->getExpT(),err);
	      GtoT_PointingErrors(el*camera->getPlateScale(), ce*camera->getPlateScale());
	      camera->setNowWinX((unsigned short) floor(bx+0.5));
	      camera->setNowWinY((unsigned short) floor(by+0.5));
	      //	      camera->setNowWinX(trackpointCE);
	      
	      //	      camera->setNowWinY(trackpointEL);
	      //	      camera->setSubModeID(ALIGNTRACKFULLCHIP);
	      alignmentMD.winx = trackpointCE;
	      alignmentMD.winy = trackpointEL;
	      alignmentMD.winx = (unsigned short) floor(targetx+0.5);
	      alignmentMD.winy = (unsigned short) floor(targety+0.5);
	      WriteToLog("ALGN"," (%d):  %ld, T(%d, %d) , W(%d, %d, %d, %d)  R(%d, %d, %d, %d) B(%lf, %lf)",submodeid, frmnumber, targetx+x, targety+y,wx, wy, wdx, wdy, x, y, dx, dy, bx, by);
	    } else {
	      distx = bx-correctedtargetx;
	      disty = by-correctedtargety;
	      camera->distybuffer[qID]=disty;
	      camera->distxbuffer[qID]=distx;
	      el = by-targety;
	      ce = bx-targetx;

	      dist=sqrt(ce*ce+el*el);
	      FInfo.send(frmnumber, leakylvdt/4096., el,ce,camera->getExpT(),err);	
	      GtoT_PointingErrors(el*camera->getPlateScale(), ce*camera->getPlateScale());
	      if(dist > camera->MoveStepSize){
		distx = distx/dist*camera->MoveStepSize; 
		disty = disty/dist*camera->MoveStepSize;
	      };
	      camera->setNowWinX((unsigned short) floor(bx+0.5));
	      camera->setNowWinY((unsigned short) floor(by+0.5));
	      alignmentMD.winx = (unsigned short) floor(bx+0.5);
	      alignmentMD.winy = (unsigned short) floor(by+0.5);
	      WriteToLog("ALGN"," (%d):  %ld, T(%d, %d) , W(%d, %d, %d, %d)  R(%d, %d, %d, %d) B(%lf, %lf)",submodeid, frmnumber, targetx+x, targety+y,wx, wy, wdx, wdy, x, y, dx, dy, bx, by);
	    };
	    goto processedblob;
	  } else {
	    // there was no blob in our window.
	    WriteToLog("ALGN"," (%d): No Blob :  %ld, T(%d, %d) , W(%d, %d, %d, %d)  R(%d, %d, %d, %d) B(-1000, -1000)",submodeid, frmnumber, targetx+x, targety+y,wx, wy, wdx, wdy, x, y, dx, dy);
	    
	  };
	} else	if(submodeid == ALIGNFULLCHIP){
	  // and finally, here we track on the n-th brightest blob in
	  // the field, without discriminating against whether the blobs 
	  // switch or not.
	  currentblob = 0;
	  targetblob = 0;
	  if(currentblob == targetblob){
	    alignmentMD.centX = bx;
	    alignmentMD.centY = by;


	    //	    if(dither::isDithering){
	    //	      dither::add(targetx, targety);
	    //	    }; 

#ifdef RASTER
	    
	    //	    rastery = 542 + (frmnumber % (625*27))/(625);
	    //	    rasterx = 708 + (frmnumber % (625*27))%25;

	    //	    static int inloop;
	    //	    inloop = (frmnumber/27) % 625;

	    //	    rastery = 530 + 7.5* (inloop/25);
	    //	    rasterx = 694 + 7.5* (inloop % 25);
			     
	    // targetx = rasterx;
	    //	    targety = rastery;
			     
		
	    //	    printf("Yeah... I was here 2 \n");
			    

#endif 	    
	    



	    correctedtargetx = targetx+camera->gondolaoffsetx;
	    correctedtargety = targety+camera->gondolaoffsety;
	    //	    targetx = trackpointCE;
	    //	    targety = trackpointEL;
	    //	   	    id(dither::isDithering) dither::add(targetx, targety);
	    distx = (bx - correctedtargetx);
	    disty = (by - correctedtargety);
	    camera->distybuffer[qID]=disty;
	    camera->distxbuffer[qID]=distx;
	    //	    el = by-targety;
	    //	    ce = bx-targetx;
	    FInfo.send(frmnumber, leakylvdt/4096, disty,distx,camera->getExpT(),err);	
	    GtoT_PointingErrors(disty, distx);
	    WriteToLog("ALGN"," (%d):  %ld, T(%d, %d), B(%lf, %lf)",submodeid,frmnumber,targetx+x, targety+y, bx+x, by+y);
	    goto processedblob;
	  }
	};
	currentblob++;
	pblob=pblob->getnextblob();
      };

      processedblob:

    
      
      

 
     static double tmpel, tmpce;
     
      tmpel = disty*camera->getPlateScale();
      tmpce = distx*camera->getPlateScale();
      
 
      //      dist = sqrt(el*el + ce*ce);
      
      distx = camera->cosangle * tmpce + camera->sinangle*tmpel;
      disty = -camera->sinangle * tmpce + camera->cosangle*tmpel;
      
      
      //	  printf("%lf %lf\n",camera->cosangle, camera->sinangle);
      
      lastel = disty;
      lastce = distx;



      

      disty = camera->userFCEL(disty);
      distx = camera->userFCCE(distx);
     
      SendOutEL(-disty);
      SendOutCE(-distx);
      statposx.insert(distx,idx);
      statposy.insert(disty,idx);

      //      WriteToLog("ALFC","Sending out %lf %lf",disty,distx);

    }

    PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], 0, err);

    if(!err){
      static unsigned int frmnb;
      frmnb = camera->onlyGetFrame();
      //      if(!(frmnb%10))FrameToLog(frmnb,dx,dy,camera->buffer[qID]);
      if(filesaving) {
	if(!(frmnb%10))ExtraFrameToLog(frmnb, x,y,dx,dy, bx,by,camera->getExpT(),camera->buffer[qID]);
	
	// added in Ft Sumner, to have a little more information on 
	// the frames.

	InfoToLog(camera, &camera->fblob[qID], frmnumber, targetx, targety, -1.0, -1.0 );
	

      };
      if(!(frmnb%10))GtoT_FrameBlobs(&(camera->fblob[qID]));

    };
    
    //ROI MODE
  } else {  // ok. now, what do we do if we are in ROI mode? 

    if (!ctralgorithm) { 
    // first we find the brightest pixel.  This isn't necesserily optimal,
    // but it is what we do.
    max = 0;maxx=0; maxy=0;
    for(j=0;j<dy;j++){
      for(i=0;i<dx;i++){
	val = buffer[i+j*dx];
	sum += val;
	sumsq += val*val;
	if(val>max){
	  maxx = i; maxy = j; max = val;
	};
      };
    };



    // now calculate a better centroid and a flux. 
    sumx = sumy = sumtot = rsq = rsqx = rsqy =0;
    for(j=-BOXHALFSIZE; j<BOXHALFSIZE+1;j++){
      for(i=-BOXHALFSIZE; i<BOXHALFSIZE+1; i++){
	if(maxx+i >= 0 && maxx+i<dx && maxy+j>=0 && maxy+j<dy){
	  //	  val = (double) buffer[(maxx+i) + (maxy+j)*dx];
	  // Ft Sumner 
	  val =  1.0*buffer[(maxx+i) + (maxy+j)*dx];
	  sumx += i*val;
	  sumy += j*val;
	  sumtot += val;
	}
      }
    }
    
    if(sumtot < 1) sumtot = 1;
    sumx = sumx/sumtot;
    sumy = sumy/sumtot;

    maxx = maxx+sumx+0.5;
    maxy = maxy+sumy+0.5;



    // that locates the maximum and gets us on the way to the 
    // mean and the sigma for the exposure.
    sum = sum/(dx*dy);
    sumsq = sqrt(sumsq/(dx*dy)-sum*sum);
    if(sumsq <1) sumsq = 1;
    // these should possibly be calculated better, but for now
    // let's leave it like this.
    //    targetx = dx/2;
    //    targety = dy/2;

    targetx = trackpointCE-x;
    targety = trackpointEL-y;


    if (dither::isDithering) dither::add(targetx, targety);

    // we might have a problem here if the target point is outside of
    // the small window. What we're going to do is set the target
    // point to the center point of the box if the true target point
    // is outside of this box.
    
    if (targetx < 0 || targetx >=dx) targetx = dx/2.;
    if (targety < 0 || targety >=dy) targety = dy/2.;
				       
				      


    correctedtargetx = targetx+camera->gondolaoffsetx/2.;
    correctedtargety = targety+camera->gondolaoffsety/2.;

    sumx = sumy = sumtot = rsq = rsqx = rsqy =0;

    static int cnt,mx,my = 0;
    static double bgnd;
    static float bgcnt;
    static float bgxsq, bgysq;
    static int overmax;
    cnt = 0;
    bgnd = 0;
    bgcnt = 0;
    bgxsq = 0;
    bgysq = 0;
    overmax = 0;
    

    for(j=-BOXHALFSIZE; j<BOXHALFSIZE+1;j++){
      for(i=-BOXHALFSIZE; i<BOXHALFSIZE+1; i++){
	if(maxx+i >= 0 && maxx+i<dx && maxy+j>=0 && maxy+j<dy){
	  
	  val = (double) buffer[(maxx+i) + (maxy+j)*dx];

	  if (val > max/3.) overmax++;

	  if (BGDMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]){
	    bgcnt++;
	    bgnd += val;
	  };
	  
	  
	  sumx += 1.0*i * val;
	  sumy += 1.0*j * val;
	  sumtot += 1.0*val;
	  //	  rsq += RSQUARED[i+BOXHALFSIZE][j+BOXHALFSIZE]*val;
	  rsqx += 1.0*i*i*val;
	  rsqy += 1.0*j*j*val;
	  bgxsq += 1.0*i*i;
	  bgysq += 1.0*j*j;
	  cnt++;
	};
      };
    };
    
    // find the mean background in the image. 
    if(bgcnt<1) { 
      bgcnt = 1;
      bgnd = 0;
    };
    bgnd = bgnd/bgcnt;
    
    //    printf("test A: Bx=%f\tBy=%f\n",bgxsq, bgysq);


    //    printf("test B: rx=%f\try=%f\n",rsqx, rsqy);

    // correct for the background
    rsqx = rsqx - bgxsq*bgnd;
    rsqy = rsqy - bgysq*bgnd;

    //    printf("test C: rx=%f\try=%f\n",rsqx, rsqy);

    if (rsqx <0 ) rsqx = 900000.;  // if variance is negative, we have no star..
    if(rsqy < 0) rsqy = 900000.;


    
    sumtot -= cnt * bgnd;
    
    if(sumtot < 1) sumtot = 1;

    // what if there are no counts?
    if(cnt<1) cnt = 1;

    // RMSx and RMSy sizes 

    rsqx = sqrt(rsqx/sumtot - sumx*sumx/(sumtot*sumtot)); 
    rsqy = sqrt(rsqy/sumtot - sumy*sumy/(sumtot*sumtot)); 

    // added in Ft Sumner May 7, 2009
    statposrmsx.insert(rsqx,idx);
    statposrmsy.insert(rsqy,idx);

    //    printf("Test H: rx=%f, ry=%f\n",rsqx,rsqy);
    
    // added leaky average May 2, 2009, Ft Sumner NM. 
    //    if (!isnan(rsqx) && ! isnan(rsqy) ) {
    //leakyrmsx = 0.95 * leakyrmsx + 0.05 * rsqx;
    //      leakyrmsy = 0.95 * leakyrmsy + 0.05 * rsqy;
    //    }
    

    //    alignmentMD.rsqx = leakyrmsx;
    //    alignmentMD.rsqy = leakyrmsy;

    alignmentMD.rsqx = statposrmsx.getMean();
    alignmentMD.rsqy = statposrmsy.getMean();
    statposf.insert(sumtot,idx);
    alignmentMD.flux = statposf.getMean();
    rsq = sqrt(rsqx*rsqx + rsqy*rsqy);
    mx = (int) rsqx;
    my = (int) rsqy;
    
   
    totalflux = sumtot;
    extended = true;

    // warning : extended is referring to a non-hot-pixels, etc.

    // fort sumner, april 13,2009.
    // now we have some checks whether the source is a nice extended blob 
    // or a single pixel or what. 


    //    printf("Test I: S=%f RSQ=%f BG=%f \n",sumtot,rsq,bgnd);
    if (sumtot < 3*rsq*rsq*bgnd) extended = false; // the total flux should be some fixed amout over the background. 

    if ( overmax < 5 )   extended = false; // the total number of pixels that are high enough should be pretty large to make sure it's not a single hot pixel. 
    
    if (isnan(rsq)) extended = false;


    if(extended){
      centx = ((double) sumx)/sumtot + maxx;
      centy = ((double) sumy)/sumtot + maxy;
      
      lastcentx = centx;
      lastcenty = centy;
      loststar = 0;
    } else {
      centx = correctedtargetx;
      centy = correctedtargety;
      loststar++;
    };


    if(loststar == 90) { 
      WriteToLog("I LOST THE STAR!!!");
      GtoM_SwitchingDTURequest();
      loststar = 0;
      
    };
    
    Point maxpt;
    


    if (zoom_type) { 
      maxpt.x = targetx;
      maxpt.y = targety;
    } else { 
      maxpt.x = centx;
      maxpt.y = centy;
    }    
    
    //    maxpt.x = dx/2;
    //    maxpt.y = dy/2;

    
   
    
    alignmentMD.centX = centx-targetx;
    alignmentMD.centY = centy-targety;
    if(!(qID % 15))alignmentMD.rsq = 2* rsq;
    
    
    
    distx = (centx - correctedtargetx);
    //distx *= camera->getPlateScale();
    disty = (centy - correctedtargety);
    //disty *= camera->getPlateScale();
    camera->distybuffer[qID]=disty;
    camera->distxbuffer[qID]=distx;
    dist = sqrt(distx*distx + disty*disty);

    //    if(max > 5 * sumx && extended){
      //    };
    ce = (centx-targetx)*camera->getPlateScale();
    el = (centy-targety)*camera->getPlateScale();
    FInfo.send(frmnumber,leakylvdt/4096., el,ce,camera->getExpT(),err);	
    GtoT_PointingErrors(el,ce); //errors are in pixels
    static unsigned int frmnbr;
    frmnbr = camera->onlyGetFrame();
    WriteToLog("ALGN"," (%d): %ld,  T(%d, %d), R(%d, %d, %d, %d), W(%d, %d, %d, %d), T(%d, %d), C(%f, %f), OE(%f, %f)",  submodeid, frmnbr, targetx, targety, x,y,dx,dy, wx,wy,wdx, wdy, targetx+x, targety+y, centx+x, centy+y, ce, el);    

    lastel = el;
    lastce = ce;
    
    //    GtoT_PointingErrors(disty,distx);
    
    
    
    
    ce = camera->cosangle * distx - camera->sinangle*disty;
    el = -camera->sinangle * distx + camera->cosangle*disty;
    el = camera->userRoiEL(el);
    ce = camera->userRoiCE(ce);
    //if(!(qID%3))GtoT_PointingErrors(el,ce);
    //      GtoT_PointingErrors(el,ce);
    
    
    
    
    SendOutEL(-el);
    SendOutCE(-ce);
    statposx.insert(ce,idx);
    statposy.insert(el,idx);
  
    PlaceSmallBox(camera, camera->buffer[qID], (void*) &maxpt, err, qID);
    
    InfoToLog(camera, &camera->fblob[qID], frmnumber, targetx, targety, centx, centy );



    if(!err){
      if(!(frmnumber%5))ExtraFrameToLog(frmnumber, x,y,dx,dy, centx,centy,camera->getExpT(),camera->buffer[qID]);
	    //    if(!(frmnbr%5))FrameToLog(frmnbr,dx,dy,camera->buffer[qID]);
    };
    } else { // procede different value ... in case I want to use a different procedure to centrod... which I probably will want to do...
      
      // the 200x200 box (or smaller) is going to get divided into 4x4 80 pixel squares, offset by 40 pixels from one another. 

      //#define BOXES 361
      //#define DIVISOR 20
      
      static float subsum[400];
      static float subsqsum[400];
      static short maxx[400];
      static short maxy[400];
      static float  maxv[400];
      static float centx[400];
      static float centy[400];
      static float sumsqx[400];
      static float sumsqy[400];
      static float sumx[400];
      static float sumy[400];
      static float val;
      static short idx;
      static short sortind[400];       
      static short tmpshort;
      static float bgnd;
      static float max;
      static float lastel, lastce;

      static unsigned short  stepdx;
      static unsigned short stepdy; 

      Point maxpt;      

      // OK. first order of business. Where are we supposed to track?!
    targetx = trackpointCE-x;
    targety = trackpointEL-y;




    if (dither::isDithering) dither::add(targetx, targety);

    // we might have a problem here if the target point is outside of
    // the small window. What we're going to do is set the target
    // point to the center point of the box if the true target point
    // is outside of this box.
    
    if (targetx < 0 || targetx >=dx) targetx = dx/2.;
    if (targety < 0 || targety >=dy) targety = dy/2.;
				       
				      


    correctedtargetx = targetx+camera->gondolaoffsetx/2.;
    correctedtargety = targety+camera->gondolaoffsety/2.;

    // we be done with that...

      stepdx = dx/DIVISOR;
      stepdy = dy/DIVISOR;

      
      static short i,j,k,l;
      
      // zero the relevant quantities.
      max = 0;
      for ( i = 0 ; i< BOXES; i++) {
	subsum[i] = 0;
	subsqsum[i] = 0;
	maxx[i] = maxy[i] = maxv[i] = 0;
	centx[i] = centy[i]= 0;
	sumsqx[i] = sumsqy[i] = 0;
	sortind[i] = i;
	
	// and anything else that needs to be zeroed. 
      };

      for (j = 0; j < DIVISOR-1; j++){  // loop over subsum square y direction
	for (i=0; i< DIVISOR-1; i++){ // loop over subsum square x direction 

	  idx = i+j*(DIVISOR-1); // the index within each box. 
	  
	  for (l = 0; l< 2*stepdy; l++){ // loop over individual pixels, y 
	    for (k = 0; k<2*stepdx; k++){ // loop over individual pixels, x
	      val = 1.0*buffer[(i*stepdx+k)+((j*stepdy+l)*dx)];
	      
	      subsum[idx] += val;
	      subsqsum[idx] += val*val;
	      sumsqx[idx] = k*k*val;
	      sumsqy[idx] = l*l*val;
	      sumx[idx] = k*val;
	      sumy[idx] = l*val;
	      if (val > maxv[idx]) { 
		maxv[idx] = 1.0*val;
		maxx[idx] = k;
		maxy[idx] = l;		
	      } ;
	      if (val > max){
		max = val;
	      };
	    }; // for (k)
	  }; // for(l);






	  if (subsum[idx] < 1) subsum[idx] = 1;
	  centx[idx] = sumx[idx]/subsum[idx];
	  centy[idx] = sumy[idx]/subsum[idx];
	  sumsqx[idx] /= subsum[idx];
	  sumsqy[idx] /= subsum[idx];
	  subsqsum[idx] = subsqsum[idx]/(1.0*stepdx*4.0*stepdy)-subsum[idx]*subsum[idx]/(16.0*stepdx*stepdx*stepdy*1.0*stepdy);
	  
	}  // for (i);
      } // for (j)





      // now want to sort the tiles by the total number of counts in each

 
      
      // now we gnomesort...
      i = 1 ;
      j = 2 ;

      while (i < BOXES) {
	if ( subsum[sortind[i-1]] >= subsum[sortind[i]] ){
	  i = j;
	  j++;
	} else {
	  tmpshort = sortind[i-1];
	  sortind[i-1] = sortind[i];
	  sortind[i] = tmpshort;
	  i--;
	  if (i == 0) {
	    i = j;
	    j++;
	  };
	};
      }; //while()








      // we will use the 4 dimmest squares as background 
      // --- and --- 
      // we will assume that the brightest tile has the star. 

      bgnd = 0;
      
      for(i=0;i<4;i++){
	bgnd += subsum[sortind[BOXES-1-i]];
      };
      
      bgnd /= (16*stepdx*stepdy) ;


      static bool star;
      static short loststar;

 
      star = true;
      /*
      if ( subsum[sortind[0]] < 1.00 * subsum[sortind[BOXES-1]]){
	star = false;
      } else { 
	star = true;
      };
      */



      
      static unsigned short mx, my;
      static short cnt, overmax;
      static  float mv, cx, cy, flux, smx, smy, smsqx, smsqy,sumtot, rmsx, rmsy;

      //      if(star) { 
      cx = cy = flux = smx = smy = smsqx = smsqy = 0.0;
      cnt = overmax = 0;
      idx = sortind[0];
      mx = (maxx[idx]+stepdx*(idx%(DIVISOR-1)));
      my = (maxy[idx]+stepdy*(idx/(DIVISOR-1)));
      mv = maxv[idx];








      // mx , my and mv is the max pixel in the brightest square. 
      // now we need to do a centroid around that pixel. we also have the background to subtract!

      cx = cy = flux = smx = smy = smsqx = smsqy = sumtot = 0.0;

      for(j=-BOXHALFSIZE; j<BOXHALFSIZE; j++){
	for(i=-BOXHALFSIZE; i<BOXHALFSIZE; i++){
	if(mx+i >= 0 && mx+i<dx && my+j>=0 && my+j<dy){

	  // background subtraction. 
	  val =  1.0*buffer[(mx+i) + (my+j)*dx] - bgnd;
	  cnt++;

	  if (val > ctrovermaxthr * mv) overmax++;
	  cx += i*val;
	  cy += j*val;
	  smsqx += i*i*val;
	  smsqy += j*j*val;
	  sumtot += val;
	};
	  
	}; // for(i)
      }; // for(j);

      // we should have the centroid, and the rms value at this point... I think...  
      // referenced to (mx,my) = (0,0);

      cx /= sumtot;
      cy /= sumtot;



    if (zoom_type) { 
      maxpt.x = targetx;
      maxpt.y = targety;
    } else { 
      maxpt.x = cx+mx;
      maxpt.y = cy+my;
    }



    //    maxpt.x = maxv[sortind[0]];
    //    maxpt.y = subsqsum[sortind[0]];
    


      smsqx /= sumtot;
      smsqy /= sumtot;



      rmsx = smsqx - cx*cx;
      rmsy = smsqy - cy*cy;
      
      if (rmsx < 0) rmsx = 90000000.0;
      if (rmsy < 0) rmsy = 90000000.0;

      rmsx = sqrt(rmsx);
      rmsy = sqrt(rmsy);



      

      if (overmax < ctrovermaxcnt) star = false;
      if (rmsx > ctrspotrms || rmsy > ctrspotrms) star = false;

 
      if (!star) maxpt.x = 555.0;
     
      static short idx2;
      idx2 = frmnumber % 100;
      

      statposx.insert(cx+mx-targetx,idx2);
      statposy.insert(cy+my-targety,idx2);

      statposf.insert(sumtot,idx2);

      statposrmsx.insert(rmsx,idx2);
      statposrmsy.insert(rmsy,idx2);
      
      statposbgnd.insert(sumtot/sqrt(sumtot+cnt*bgnd),idx2);


      alignmentMD.centX = statposx.getMean();
      alignmentMD.centY = statposy.getMean();      
      alignmentMD.rmsx = statposx.getRMS();
      alignmentMD.rmsy = statposy.getRMS();      


      alignmentMD.bgnd = sumtot/sqrt(sumtot+cnt*bgnd);
      //      alignmentMD.bgnd = mv/maxv[sortind[BOXES-1]];

      
      alignmentMD.rsqx = statposrmsx.getMean();
      alignmentMD.rsqy = statposrmsy.getMean(); 

      alignmentMD.flux = statposf.getMean();;
      


      
      ce = (cx+mx)-targetx;
      el = (cy+my)-targety;
      

      el = camera->userRoiEL(el);
      ce = camera->userRoiCE(ce);
      

      if(isnan(ce)) ce = 0;
      if(isnan(el)) el = 0;

      // store the values.

      if(star){
	loststar = 0; // we clearly didn't lose the star. 
	lastel = el;
	lastce = ce;
	
      };
      
      //      } else { // so that was what we did if there was a star
      
      if(!star){
	loststar++;
	if (loststar > 90){ 
	  WriteToLog("I LOST THE STAR!!! Switching to DTU");
	  GtoM_SwitchingDTURequest();
	  loststar = 0;
	}
	
	el = lastel;
	ce = lastce;
      };
	
	//      }; // and this is what we do when there is no star.
      
      
      // now that we know where we are, we send out the data to the MPF
      SendOutEL(-el);
      SendOutCE(-ce);           

      // then to the ground
      GtoT_PointingErrors(el,ce); //errors are in pixels

      // then to the detector
      FInfo.send(frmnumber,leakylvdt/4096., el,ce,camera->getExpT(),err);
      
      // and now we display. :)
      PlaceSmallBox(camera, camera->buffer[qID], (void*) &maxpt, err, qID);
      
      // next we log what we see. 
    WriteToLog("ALGN"," (%d): %ld,  T(%d, %d), R(%d, %d, %d, %d), W(%d, %d, %d, %d), T(%d, %d), C(%f, %f), OE(%f, %f)",  submodeid, frmnumber, targetx, targety, x,y,dx,dy, wx,wy,wdx, wdy, targetx+x, targety+y, cx+x+mx, cy+y+my, ce, el);    

      // then we log some of the frame information to the frame logfile
      InfoToLog(camera, &camera->fblob[qID], frmnumber, targetx, targety, cx+x+mx, cy+y+my );


      // and we save every fifth image. 
      if(!err){	
	if(!(frmnumber%5))ExtraFrameToLog(frmnumber, x,y,dx,dy, cx+mx+x,cy+my+y,camera->getExpT(),camera->buffer[qID]);

      };


      


    }; // different centroid calculation option. 
  }; // roi mode

  frmnumber = camera->getFrame();
  return;

};
