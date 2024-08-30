#include"Solder.h"
Solder::Solder(string name){
    this->solderName=name;
    this->_ptr_gun=nullptr;
}
void Solder::addGun(Gun* _ptr_gun){
    this->_ptr_gun=_ptr_gun;
}
void Solder::addBulletToGun(int num){
    this->_ptr_gun->addBollet(num);
}
bool Solder::fire(){
    return
     _ptr_gun->shoot();
}
Solder::~Solder()
{
  if(this->_ptr_gun==nullptr){
    return;
  }
  delete _ptr_gun;
  _ptr_gun=nullptr;
}