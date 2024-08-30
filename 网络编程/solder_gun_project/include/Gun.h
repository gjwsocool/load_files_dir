#pragma once//防止头文件重复包含
#include<string>
#include<iostream>
using namespace std;
class Gun{
private:
   int _bulletCount;
   string _type;
public:
   Gun(string type):_type(type){this->_bulletCount=0;}
   //装子弹
   void addBollet(int bulletNum);
   //发射子弹
   bool shoot();
};