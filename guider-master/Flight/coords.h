#ifndef COORDS_H_
#define COORDS_H_

class Coords{
 public:
  
  double RA, DEC;


  Coords();

  Coords::Coords(double ra, double dec);
  ~Coords();

  void Set(double ra, double dec);

  void SetRA(double ra);
  void SetDEC(double dec);

  void GetFloat(double &ra, double &dec);
  double GetFloatRA();
  double GetFloatDEC();

  void operator=(Coords c);
  
  char operator==(Coords c);
    
};


#endif
