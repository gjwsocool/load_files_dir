#pragma once
#include<string>
#include"Gun.h"
using namespace std;
class Solder{
private:
    string solderName;
    Gun* _ptr_gun;
public:
    Solder(string name);
    ~Solder();
    void addBulletToGun(int num);
    void addGun(Gun* _ptr_gun);//声明枪
    bool fire();//开火
};