#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>    // std::sort,swap
#include <stdlib.h>     /* abs , sys*/
#include <stdio.h>
#include <vector>
#include <string.h>
#include <unistd.h>
using namespace std;
using namespace cv;

static const int ORG_SOUNDFILE_LENGTH = 5*1000000; // in microseconds
static const int ANGLE_THRESHOL=10; // if two persons are in range of this threshold will meet them up and play the ad only one.

std::vector<double> RunFaceDetection( Mat frame );
std::vector<double> SortAngles(std::vector<double> AnglesVector);  // to get shortest and best path
void ExeAngles(std::vector<double>InCurrent, std::vector<double>others);

int ThresholdingGroupPeople(std::vector<double>&v,std::vector<double>&InCurrent);

double angle(double x);
int ConfigFaceDetection( void );

String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String frontalFace_cascade_name = "haarcascade_frontalface_alt.xml";
String profileFace_cascade_name ="haarcascade_profileface.xml";
CascadeClassifier body_cascade;
CascadeClassifier frontalFace_cascade;
CascadeClassifier profileFace_cascade;
String window_name = "Capture - Face detection";

int width=640;
double FOV=180,dpp=FOV/((double)width);
Mat frame1,frame2;


bool stopFaceDetection = false;
std::vector<double> AnglesVector;
VideoCapture capture;


void remove(std::vector<double>& vec, size_t pos)
{
    std::vector<double>::iterator it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}
void printVector(std::vector<double> vec)
{
    for (int i = 0; i < vec.size(); i++)
        cout << vec.at(i) <<" ";

    cout<<endl;
}

class Dbrw
{
public:
    static void WriteAngle(double angle)
    {
        ofstream outfile;
        outfile.open("/home/azizax/Documents/fci/GP/CODE/GP/dbrw",ios::out | ios::trunc );
        outfile<<angle<<endl;
        outfile.close();
    };
    static  double ReadAngle()
    {
        ifstream infile;
        infile.open ("/home/azizax/Documents/fci/GP/CODE/GP/dbrw",  ios::out | ios::in );
        string buffer="";
        while (infile.good())
        {
            // cout<<(char) infile.get();
            buffer+=(char) infile.get();
        }
        stringstream ss;
        double result;
        ss<<buffer;
        ss>>result;
        infile.close();
        return result;
    };
    static int ReadReached()
    {
        ifstream infile;
        infile.open ("/home/azizax/Documents/fci/GP/CODE/GP/reached",  ios::out | ios::in );
        string buffer="";
        while (infile.good())
        {
            // cout<<(char) infile.get();
            buffer+=(char) infile.get();
        }
        stringstream ss;
        int result;
        ss<<buffer;
        ss>>result;
        infile.close();
        return result;
    }
    static int ClearReached()
    {
        std::ofstream ofs;
        ofs.open("/home/azizax/Documents/fci/GP/CODE/GP/reached", std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }

};
void MoveMotor(double angle)
{
    char str_steps[3];
    char cmd []= "python /home/azizax/Documents/fci/GP/CODE/GP/DeliveryBoy_DeliverAngleFromPi2Arduino.py ";
    int steps =0;
    int prev = Dbrw::ReadAngle();
    steps = angle - prev;

    stringstream ss;
    ss<<steps;
    ss>>str_steps;
    strcat(cmd,str_steps);
   // cout<<cmd<<endl;
      system(cmd);

      // check reached
      bool reached=false;
      while(!reached)
      {
          if(Dbrw::ReadReached()==1)
          {
              reached=true;
              Dbrw::ClearReached();
          }
      }
    Dbrw::WriteAngle(angle);
}
void PlaySound()
{
   system("python /home/azizax/Documents/fci/GP/CODE/GP/playsound.py");
//    cout<<"DO U HEAR ME?!\n";
}
int main()
{

      if(ConfigFaceDetection()!=0)
          return 0 ;
      while(1)
      {
          /// catch faces
          while (!stopFaceDetection)
          {
              capture.open(0);
              capture.read(frame1);
              capture.release();
              capture.open(1);
              capture.read(frame2);
              capture.release();
              int rows = max(frame1.rows, frame2.rows);
              int cols = frame1.cols + frame2.cols;
              Mat3b frame(rows, cols, Vec3b(0,0,0));
              frame1.copyTo(frame(Rect(0, 0, frame1.cols, frame1.rows)));
              frame2.copyTo(frame(Rect(frame1.cols, 0, frame2.cols, frame2.rows)));

              AnglesVector=  RunFaceDetection( frame );
              if(AnglesVector.size()>0)
              {
                  stopFaceDetection= true;
              }
              waitKey(250);
          }
          /// calculate waited time

    int TimeToWait =0;
    std::vector<double>WellSortedAnglesVector ;
    std::vector<double>InCurrent ;

    /*AnglesVector.push_back(55.0);
    AnglesVector.push_back(22.0);
    AnglesVector.push_back(85.0);
    AnglesVector.push_back(160.0);
    AnglesVector.push_back(35.0);*/

    ThresholdingGroupPeople(AnglesVector,InCurrent);
    WellSortedAnglesVector = SortAngles(AnglesVector);

    //printVector(InCurrent);
   // printVector(WellSortedAnglesVector);

    /// call motor , sound files
    ExeAngles(InCurrent,WellSortedAnglesVector);

}
    return 0;
}
void ExeAnglesForInternalGroup(std::vector<double>Group)
{
    int PeriodAmongInCurrentMoves =  ORG_SOUNDFILE_LENGTH/Group.size();
    /// play sound for Group
    for(int i=0; i<Group.size(); i++)
    {
        MoveMotor(Group.at(i));
        if(i==0)
        {
            PlaySound();
        }
        usleep(PeriodAmongInCurrentMoves);
    }
}
void ExeAngles(std::vector<double>InCurrent, std::vector<double>others)
{
    /**
        play sound and move motor for InCurrent As Internal Group
        Grouping others into internals
        play sound and move motor for internals
    **/
    if(InCurrent.size()>0)ExeAnglesForInternalGroup(InCurrent);

    std::vector<double>InternalGroup;
    int c=0;
    for(int i=0; i<others.size()-1; i++)
    {
        InternalGroup.push_back(others.at(i));
        c++;
        if(abs(others.at(i)-others.at(i+1))<=ANGLE_THRESHOL)
        {
            InternalGroup.push_back(others.at(i+1));
            c++;
        }
        else
        {
            ExeAnglesForInternalGroup(InternalGroup);
            InternalGroup.clear();
        }
    }
    /// LAST ANGLE
    if(c<others.size())
    {
        std::vector<double>InternalGroup;
        InternalGroup.push_back(others.at(others.size()-1));
        ExeAnglesForInternalGroup(InternalGroup);
    }
}
std::vector<double> SortAngles(std::vector<double> AV)
{
    std::vector<double>WellSortedAnglesVector;
    std::sort (AV.begin(), AV.end());
    int currentPosition = Dbrw::ReadAngle();
     currentPosition = 80;
    //cout<<"currentPosition: "<< currentPosition<<endl;

    /// the current -- > min .. max
    if(AV.at(0)>currentPosition && AV.at(AV.size()-1)> currentPosition)
    {
        // cout<<"the current -- > min .. max\n";
        std::copy(AV.begin(),AV.end(),std::back_inserter(WellSortedAnglesVector));
    }
    //  min .. max -- > current

    else if(AV.at(0)< currentPosition && AV.at(AV.size()-1)< currentPosition)
    {
        //cout<<"min .. max -- > current\n";
        std::copy(AV.end(),AV.begin(),std::back_inserter(WellSortedAnglesVector));
    }
    // min --> the current -- > max
    else
    {
        // cout<<"min --> the current -- > max\n";
        // SPLIT TWO VECTORS
        std::vector<double> Bigger;
        std::vector<double> Smaller;
        for(int i=0; i<AV.size(); i++)
        {
            if(AV.at(i)>currentPosition)Bigger.push_back(AV.at(i));
            else Smaller.push_back(AV.at(i));
        }

        int tripBigger = abs(Bigger.at(Bigger.size()-1) - currentPosition);
        int tripSmaller = abs(Smaller.at(0) - currentPosition);
        if(tripBigger>tripSmaller&&abs(Bigger.at(0)-currentPosition)>abs(Smaller.at(Smaller.size()-1)-currentPosition))
        {
            // smaller first
            // then bigger
            //  cout<<"LEFT -> RIGHT1\n";
            std::reverse(Smaller.begin(),Smaller.end());
            std::copy(Smaller.begin(),Smaller.end(),std::back_inserter(WellSortedAnglesVector));
            std::copy(Bigger.begin(),Bigger.end(),std::back_inserter(WellSortedAnglesVector));
        }
        else if(tripBigger<tripSmaller&&abs(Bigger.at(0)-currentPosition)<abs(Smaller.at(Smaller.size()-1)-currentPosition))
        {
            // bigger first
            // then smaller
            //  cout<<"RIGHT -> LEFT1\n";
            std::copy(Bigger.begin(),Bigger.end(),std::back_inserter(WellSortedAnglesVector));
            std::reverse(Smaller.begin(),Smaller.end());
            std::copy(Smaller.begin(),Smaller.end(),std::back_inserter(WellSortedAnglesVector));
        }
        else
        {
            if(tripBigger>tripSmaller)
            {
                // smaller first
                // then bigger
                //        cout<<"LEFT -> RIGHT2\n";
                std::reverse(Smaller.begin(),Smaller.end());
                std::copy(Smaller.begin(),Smaller.end(),std::back_inserter(WellSortedAnglesVector));
                std::copy(Bigger.begin(),Bigger.end(),std::back_inserter(WellSortedAnglesVector));
            }
            else
            {
                // bigger first
                // then smaller
                //    cout<<"RIGHT -> LEFT2\n";
                std::copy(Bigger.begin(),Bigger.end(),std::back_inserter(WellSortedAnglesVector));
                std::reverse(Smaller.begin(),Smaller.end());
                std::copy(Smaller.begin(),Smaller.end(),std::back_inserter(WellSortedAnglesVector));
            }
        }
    }

    /*  for(int i=0; i<WellSortedAnglesVector.size(); i++)
      {
          cout<<WellSortedAnglesVector.at(i)<<endl;
      }
      */
    return WellSortedAnglesVector;

}
int ThresholdingGroupPeople(std::vector<double>&v,std::vector<double>&InCurrent)
{
    std::sort (v.begin(), v.end());
    int currentPosition = Dbrw::ReadAngle();
    currentPosition = 80;
    /**
        groups:  may contain 1-v.size() people
        best case == ruch , people stand front of the product
        and the distance among them <10degrees
        worst case == singles around the product

        result will contain org vector
        After removing the current position and in range with
    **/

    int groups=0;
    int TempEndOfPrevGroup=777;   /// to bind groups

    /// LAST ELEMENT
     if(abs(v.at(v.size()-1)-currentPosition)<=ANGLE_THRESHOL)
        {
            InCurrent.push_back(v.at(v.size()-1));
            remove(v,v.size()-1);
        }

    for(int i=0; i<v.size()-1; i++)
    {
        if(abs(v.at(i)-currentPosition)<=ANGLE_THRESHOL)
        {
            InCurrent.push_back(v.at(i));
            remove(v,i);
        }
        else if(abs(v.at(i)-v.at(i+1))<=ANGLE_THRESHOL)
        {
            if(TempEndOfPrevGroup!=i)
            {
                groups++;
                TempEndOfPrevGroup=i+1;
            }
        }
    }
    return v.size()-groups;
}
int ConfigFaceDetection( void )
{

    //-- 1. Load the cascades
    if( !body_cascade.load( body_cascade_name ) )
    {
        printf("--(!)Error loading body cascade\n");
        return -1;
    };
    if( !frontalFace_cascade.load( frontalFace_cascade_name ) )
    {
        printf("--(!)Error loading face cascade\n");
        return -1;
    };
    if( !profileFace_cascade.load( profileFace_cascade_name ) )
    {
        printf("--(!)Error loading face cascade\n");
        return -1;
    };

    return 0;
}
std::vector<double> RunFaceDetection(Mat frame)
{
    std::vector<Rect> bodies;
    Mat frame_gray=Mat::zeros( frame.size(), frame.type() );
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    std::vector<double>facesAngles;
    //-- Detect bodies
    body_cascade.detectMultiScale( frame_gray, bodies, 1.1, 4, 0|CASCADE_SCALE_IMAGE, Size(40,60) );
    for ( size_t i = 0; i < bodies.size(); i++ )
    {
        Point center( bodies[i].x + bodies[i].width/2, bodies[i].y + bodies[i].height/2 );
        ellipse( frame, center, Size( bodies[i].width/2, bodies[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        Mat faceROI = frame_gray( bodies[i] );
        std::vector<Rect> faces;
        facesAngles.push_back(angle(center.x));
        //-- In each body, detect faces
        bool check=true;
        frontalFace_cascade.detectMultiScale( faceROI, faces, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
        if(faces.size()>0) facesAngles.pop_back();
        for ( size_t j = 0; j < faces.size(); j++ )
        {
            check=false;
            Point face_center( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
            int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
            circle( frame, face_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            facesAngles.push_back(angle(face_center.x));
        }
        if(check)
        {
            profileFace_cascade.detectMultiScale( faceROI, faces, 1.1, 6, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
            if(faces.size()>0) facesAngles.pop_back();
            for ( size_t j = 0; j < faces.size(); j++ )
            {
                Point face_center( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
                int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
                circle( frame, face_center, radius, Scalar( 255, 255, 0 ), 4, 8, 0 );
                facesAngles.push_back(angle(face_center.x));
            }
        }
    }
    //-- Show what you got
    imshow( window_name, frame );
    return facesAngles;
}
double angle(double x)
{
 return x*dpp;
}
