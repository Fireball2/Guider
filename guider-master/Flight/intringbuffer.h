class UnsignedIntFILO{
 private:
  unsigned int buffer[8092];
  unsigned int size;
  int start, end;
 public:
  UnsignedIntRingBuffer(unsigned int size);
  ~UnsignedInt();
  
  unsigned int Take();
  void Put(unsigned int val);
  void Reset();

};
