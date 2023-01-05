#include "Lieu.h"

Lieu::Lieu(String uidLieu){
  Lieu::proprio = "Aucun";
  Lieu::estAchete = false;
  Lieu::uidLieu = uidLieu;
}

String Lieu::getProprio(){
  return proprio;
}

bool Lieu::getEstAchete(){
  return estAchete;
}

String Lieu::getUid(){
  return uidLieu;
}

void Lieu::setAchete(String newProprio){
  estAchete = true;
  proprio = newProprio;
}

void Lieu::setVendre(){
  estAchete = false;
  proprio = "Aucun";
}