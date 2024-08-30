#include"Gun.h"
#include<iostream>
using namespace std;
void Gun::addBollet(int bulletNum){
    this->_bulletCount+=bulletNum;
}
bool Gun::shoot(){
    //先检查有无子弹
    if(_bulletCount<=0){
    cout<<"there is no bullet!"<<endl;
    return false;
    }
    this->_bulletCount-=1;
    cout<<"shoot successfully!"<<endl;
    return true;
}