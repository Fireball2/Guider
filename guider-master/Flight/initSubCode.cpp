void QCAMAPI InitModeCallback(void *cam, unsigned long info, QCam_Err err, unsigned long){
  static int qID;
  qID = info;
  static QCamera *camera;
  camera = (QCamera *) cam;
  static frameblob blob;
  unsigned short dx, dy, x, y;
  unsigned char *buffer;
  static unsigned int fmnbr;


  fmnbr =  camera->getFrame();
  StatusToDetector(false, err);
  FrameNumberToDetector(fmnbr);


  x = camera->getRoiX();
  y = camera->getRoiY();
  dx = camera->getRoiDX();
  dy = camera->getRoiDY();
  buffer = camera->buffer[qID];




  //  if(!(qID%10))FrameToLog(camera->onlyGetFrame(),dx,dy,camera->buffer[qID]);

  PlaceFullChip(camera, camera->buffer[qID], blob, 0 ,err);
  if(err){
    GtoT_CameraError(err);
  };


  FInfo.send(fmnbr,leakylvdt, 3000.0, 3000., camera->getExpT(), err);  

};
