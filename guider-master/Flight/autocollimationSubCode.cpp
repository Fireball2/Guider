void QCAMAPI AutocollimationModeCallback(void *cam, unsigned long info, QCam_Err err, unsigned long){
  int qID;
  static short x,y,dx,dy;
  qID = info;
  static QCamera *camera;
  camera = (QCamera *) cam;
  static unsigned short  currentblob;
  static frameblob blob;
  static bloblist *pblob;
  static double rsq, rx, ry, rsqx, rsqy;
  static float targetx, targety, mean;
  static short xm,ym;
  static int i,j;
  static double val;
  unsigned char *buffer;
  static int count;
  static unsigned int fmnbr;

  if(qID > FRAMES) {
    WriteToLog("Invalid frame number.");
    qID = 0;
  };

  if(err){
    GtoT_CameraError(err);
  };


  fmnbr =  camera->getFrame();
  StatusToDetector(false, err);
  FrameNumberToDetector(fmnbr);


  x = camera->getRoiX();
  y = camera->getRoiY();
  dx = camera->getRoiDX();
  dy = camera->getRoiDY();
  buffer = camera->buffer[qID];

  camera->setFrameBlobXpix(CHIPWIDTH);
  camera->setFrameBlobYpix(CHIPHEIGHT);
  camera->fblob[qID].clearblobs();
  camera->fblob[qID].calc_mapstat();
  //    camera->fblob[qID].fix_badpix(camera->fblob[qID].get_mapmean());
  camera->fblob[qID].calc_mapstat();
  camera->fblob[qID].calc_searchgrid();
  
  camera->fblob[qID].calc_centroid();
  camera->fblob[qID].calc_flux();
  
  camera->fblob[qID].fix_multiple();
  camera->fblob[qID].sortblobs();
  
  pblob = camera->fblob[qID].getblobs();
  
    targetx = CHIPWIDTH/2.;
    targety = CHIPHEIGHT/2.;

    //    currentblob = 0;
    //    targetblob = camera->getTargetBlob();
    if(pblob!=NULL){
      autocollimationMD.offsetx =  (pblob->getx() - targetx);
      autocollimationMD.offsety =  (pblob->gety() - targety);
      //if(targetblob == currentblob){
      // these are the coordinates of the zoom point. 
      // would prefer different ones. 
      //     autocollimationMD.ypos =  (pblob->gety());
      //      autocollimationMD.xpos =  (pblob->getx());
      //      };
      currentblob++;
      xm = (short) (pblob->getx());
      ym = (short) (pblob->gety());


      
      rx = ry = rsq  = rsqx = rsqy = mean =0;
      
      count = 0;
      if(xm>BOXHALFSIZE && xm<CHIPWIDTH-BOXHALFSIZE-1 && ym>BOXHALFSIZE && ym<CHIPHEIGHT-BOXHALFSIZE-1){
	for(i=-BOXHALFSIZE; i<=BOXHALFSIZE; i++){
	  for(j=-BOXHALFSIZE; j<=BOXHALFSIZE; j++){
	    val = buffer[xm+i+(ym+j)*CHIPWIDTH];
	    mean+= (1-RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE])*val;
	    if(1-RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]) count++;
	    //	    count ++;
	  }
	};
	mean = mean/count;
	count = 0;
	for(i=-BOXHALFSIZE; i<=BOXHALFSIZE; i++){
	  for(j=-BOXHALFSIZE; j<=BOXHALFSIZE; j++){
	    val = buffer[xm+i+(ym+j)*CHIPWIDTH]-mean;
	    if(val<0) val=0;
	    rx+= i*RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]*val;
	    ry+= j*RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]*val;
	    rsq+= RSQUARED[i+BOXHALFSIZE][j+BOXHALFSIZE]*val;
	    rsqx += i * i * RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]*val;
	    rsqy += j * j * RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]*val;

	    if(RMASK[i+BOXHALFSIZE][j+BOXHALFSIZE]) count++;

	  }; //for j
	}; //for i
	rx = rx/count;
	ry = ry/count;
	rsqx = rsqx/count - rx*rx;
	if(rsqx<0) {
	  rsqx = 0;
	} else {
		rsqx = sqrt(rsqx);
	};
	rsqy = rsqy/count - ry*ry;
	if(rsqy < 0){
	  rsqy = 0;
	} else {
	  rsqy = sqrt(rsqy);
	};
		rsq = sqrt(rsqx*rsqx + rsqy*rsqy)/2;
	//	rsq = rsqx;
		if(!isnan(rsq)){
		  autocollimationMD.rmsr = rsq;
		} else {
		  autocollimationMD.rmsr = -1;
		};
      }
      GtoT_AutocollimationOffsets( autocollimationMD.offsetx*PIXSIZE, autocollimationMD.offsety*PIXSIZE, rsq);
      FInfo.send(fmnbr,leakylvdt, 3000.0, 3000.0, camera->getExpT(), err);  
    } else {
            autocollimationMD.offsetx = targetx;
            autocollimationMD.offsety = targety;
	    //            GtoT_AutocollimationOffsets( qID*PIXSIZE, -qID*PIXSIZE, 1000.);
    };
    
    

    PlaceFullChip(camera, camera->buffer[qID], camera->fblob[qID], rsq ,err);  

    if(!err){
      static unsigned int frmnb;
      frmnb = camera->onlyGetFrame();
      if(!(frmnb%10))ExtraFrameToLog(frmnb, x,y,dx,dy, xm,ym,camera->getExpT(),camera->buffer[qID]);
      if(!(frmnb%20))FrameToLog(frmnb,dx,dy,camera->buffer[qID]);
      if(!(frmnb%10))GtoT_FrameBlobs(&(camera->fblob[qID]));
    };

};
