#ifndef Lieu_h
#define Lieu_h
#include "Arduino.h"

class Lieu {
  private:
    String proprio;
    bool estAchete;
    String uidLieu;

  public:
    Lieu(String uidLieu);
    String getProprio();
    bool getEstAchete();
    String getUid();
    void setAchete(String newProprio);
    void setVendre();
};
#endif