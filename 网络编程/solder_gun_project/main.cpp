#include"Gun.h"
#include"Solder.h"
void test(){
    Solder sanduo("xvsanduo");
    sanduo.addGun(new Gun("AK47"));
    sanduo.addBulletToGun(20);
    sanduo.fire();
}
int main(){
    cout<<"this is a test string"<<endl;
    cout<<"this is a test string"<<endl;
    test();
    return 0;
}