#include<iostream>
using namespace std;

int main(){

int width=640,centerOfFace;
double FOV=180,dpp=FOV/((double)width),motorPosition=FOV/2;
while(1){
cout<<"\nEnter the Face's center:\t";
cin>>centerOfFace;
double angle=((double)centerOfFace)*dpp; /*face position by angle starting form the left side of the image
                                        that the most left side angle position=0 degree and the most right angle position=FOV*/
double movingAngle=angle-motorPosition; //Negative result==> moving left  Positive result==> moving right
bool isPositive= movingAngle>0;
if(isPositive)
    cout<<"\nMoving motor to the right by angle: "<<movingAngle<<endl;
else
    cout<<"\nMoving motor to the left by angle: "<<movingAngle<<endl;
motorPosition=angle;
cout<<"Motor position: "<<motorPosition<<endl;
}
return 0;
}
