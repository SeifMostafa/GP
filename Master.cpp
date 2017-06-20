#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/videoio.hpp"
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
#include <exception>
using namespace std;
using namespace cv;

static const int ORG_SOUNDFILE_LENGTH = 5*1000000; // in microseconds
static const int ANGLE_THRESHOL=10; // if two persons are in range of this threshold will meet them up and play the ad only one.

std::vector<double> DetectFacesInFrame( Mat frame );
Mat RunFaceDetection();
std::vector<double> SortAngles(std::vector<double> AnglesVector);  // to get shortest and best path
void ExeAngles(std::vector<double>InCurrent, std::vector<double>others);

int ThresholdingGroupPeople(std::vector<double>&v,std::vector<double>&InCurrent);
double angle(double x,bool flipped);
int ConfigFaceDetection( void );

String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String frontalFace_cascade_name = "haarcascade_frontalface_alt.xml";
String profileFace_cascade_name ="haarcascade_profileface.xml";
CascadeClassifier body_cascade;
CascadeClassifier frontalFace_cascade;
CascadeClassifier profileFace_cascade;
String window_name = "Capture - Face detection";

int width=1280;
double FOV=154,dpp=FOV/((double)width);
Mat frame1,frame2;


bool stopFaceDetection = false;
std::vector<double> AnglesVector;
VideoCapture capture;


void remove(std::vector<double>& vec, size_t pos)
{
    try
    {
        std::vector<double>::iterator it = vec.begin();
        std::advance(it, pos);
        vec.erase(it);
    }
    catch(std::exception  const &exc)
    {
        std::cerr <<"Remove from vector: "<< exc.what()<<endl;
    }

}
void printVector(std::vector<double> vec)
{
    try
    {
        for (int i = 0; i < vec.size(); i++)
            cout << vec.at(i) <<" ";

        cout<<endl;
    }
    catch(std::exception  const &exc)
    {
        std::cerr<<"Print vector: "<<exc.what()<<endl;
    }

}

class Dbrw
{
public:
    static void WriteAngle(double angle)
    {
        try
        {
            ofstream outfile;
            outfile.open("/home/azizax/Documents/fci/GP/CODE/GP/dbrw",ios::out | ios::trunc );
            outfile<<angle<<endl;
            outfile.close();
        }
        catch(std::exception  const &exc)
        {
            std::cerr<<"WriteAngle into file: "<<exc.what()<<endl;
        }
    };
    static  double ReadAngle()
    {
        try
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
            double result="9999";
            ss<<buffer;
            ss>>result;
            infile.close();
            return result;
        }
        catch(std::exception  const &exc)
        {
            std::cerr<<"ReadAngle from file: "<<exc.what()<<endl;
        }
    };
    static int ReadReached()
    {
        try
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
        catch(std::exception  const &exc)
        {
            std::cerr<<"Read Reached from file: "<<exc.what()<<endl;
        }
    }
    static int ClearReached()
    {
        try
        {
            std::ofstream ofs;
            ofs.open("/home/azizax/Documents/fci/GP/CODE/GP/reached", std::ofstream::out | std::ofstream::trunc);
            ofs.close();
        }
        catch(std::exception  const &exc)
        {
            std::cerr<<"Clear Reached file: "<<exc.what()<<endl;
        }
    }

};
void MoveMotor(double angle)
{
    try
    {

        char str_steps[4];
        char cmd [91]= "python /home/azizax/Documents/fci/GP/CODE/GP/DeliveryBoy_DeliverAngleFromPi2Arduino.py "; // 91 is the total size of path
        int steps =0;
        int prev = Dbrw::ReadAngle();
        cout<<"Angle: " <<angle <<" prev: "<< prev<< endl;
        steps = angle - prev;

        stringstream ss;
        ss<<steps;
        ss>>str_steps;
        strcat(cmd,str_steps);
        cout<<cmd<<endl;
        /*   system(cmd);

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
         */
    }
    catch(std::exception  const &exc)
    {
        cerr<<"MoveMotor: "<<exc.what()<<endl;
    }
}
void PlaySound()
{
    try
    {

//   system("python /home/azizax/Documents/fci/GP/CODE/GP/playsound.py");
        cout<<"DO U HEAR ME?!\n";
    }
    catch(std::exception  const &exc)
    {
        cerr<<"PlaySound: "<<exc.what()<<endl;
    }
}
int main()
{
    try
    {
        std::vector<double>WellSortedAnglesVector ;
        std::vector<double>InCurrent ;
        try
        {

            if(ConfigFaceDetection()!=0)
                return 0 ;
        }
        catch(std::exception  const &exc)
        {
            cerr<<"ConfigFaceDetection from Main: "<<exc.what()<<endl;
        }
        while(1)
        {
            /// catch faces
            try
            {

                while (!stopFaceDetection)
                {

                    Mat frame = RunFaceDetection();
                    AnglesVector=  DetectFacesInFrame( frame );
                    if(AnglesVector.size()>0)
                    {
                        stopFaceDetection= true;
                    }
                    waitKey(250);
                }
            }
            catch(std::exception  const &exc)
            {
                cerr<<"while FaceDetection from Main: "<<exc.what()<<endl;
            }
            /// sort faces
            try
            {



                /*AnglesVector.push_back(55.0);
                AnglesVector.push_back(22.0);
                AnglesVector.push_back(85.0);
                AnglesVector.push_back(160.0);
                AnglesVector.push_back(35.0);*/

                ThresholdingGroupPeople(AnglesVector,InCurrent);
            }
            catch(std::exception  const &exc)
            {
                cerr<<"ThresholdingGroupPeople from Main: "<<exc.what()<<endl;
            }

            try
            {
                WellSortedAnglesVector = SortAngles(AnglesVector);
            }
            catch(std::exception  const &exc)
            {
                cerr<<"WellSortedAnglesVector - sortAngles from Main: "<<endl;
            }
            //printVector(InCurrent);
            // printVector(WellSortedAnglesVector);

            /// call motor , sound files
            try
            {
                ExeAngles(InCurrent,WellSortedAnglesVector);
            }
            catch(std::exception  const &exc)
            {
                cerr<<"ExeAngles from Main: "<<exc.what()<<endl;
            }

        }
        return 0;
    }
    catch(std::exception  const &exc)
    {
        cerr<<"PROGRAM: "<<exc.what()<<endl;
    }
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
bool CheckNextTripIsExis( std::vector<double> NewFaces, double NextTripAt)
{
    for(int i=0; i<NewFaces.size(); i++)
    {
        if(abs(NewFaces.at(i)-NextTripAt)<=ANGLE_THRESHOL)return true;
    }
    return false;
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
    if(others.size()>0)
    {

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
                /// check next group is still exist or not !
                Mat frame =  RunFaceDetection();
                std::vector<double> NewFaces = DetectFacesInFrame(frame);
                if(NewFaces.size()>0)
                {

                    if(!CheckNextTripIsExis(NewFaces,others.at(i+1))) return;
                }
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
    stopFaceDetection= false;
}
std::vector<double> SortAngles(std::vector<double> AV)
{
    std::vector<double>WellSortedAnglesVector;
    std::sort (AV.begin(), AV.end());
    int currentPosition = Dbrw::ReadAngle();
    currentPosition = 80;
    //cout<<"currentPosition: "<< currentPosition<<endl;
    if(AV.size()>0)
    {

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
    try
    {
        if(v.size()>1)
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
            if(v.size()==groups) return 1;
            else return v.size()-groups;
        }
        else return 1;
    }
    catch(std::exception  const &exc)
    {
        cerr<<" //////////// ThresholdingGroupPeople: "<< exc.what()<<endl;
    }
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
std::vector<double> DetectFacesInFrame( Mat frame )
{
    std::vector<Rect> faces;
    std::vector<Rect> profile_faces;
    std::vector<Rect> bodies;
    std::vector<Rect> profile_faces_flip;
    std::vector<double>facesAngles;

    Mat frame_gray=Mat::zeros( frame.size(), frame.type() );
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    // Detect faces
    frontalFace_cascade.detectMultiScale(frame_gray, faces, 1.2, 3,0|CASCADE_SCALE_IMAGE, Size(30, 30));

    // Iterate over all of the faces
    size_t s=0;

    if(faces.size()!=s)
    {
        cout<<" frontal = "<<faces.size()<<endl;
        for ( size_t j = 0; j < faces.size(); j++ )
        {
            Point center( faces[j].x + faces[j].width/2, faces[j].y + faces[j].height/2 );
            //int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
            ellipse( frame, center, Size(faces[j].width/2, faces[j].height/2),0,0,360, Scalar( 255,0,255 ), 4, 8, 0 );
            facesAngles.push_back(angle(center.x,0));
            cout << angle(center.x,1) << endl;

        }
    }
    if(faces.size()==s)
    {
        profileFace_cascade.detectMultiScale(frame_gray, profile_faces, 1.1,6,0|CASCADE_SCALE_IMAGE, Size(10, 30));
        cout <<" profile faces"<<profile_faces.size()<<endl;
        // cout <<"frontal "<<faces.size()<<endl;

        for( size_t i = 0; i < profile_faces.size(); i++ )
        {
            // Find center of faces
            Point center(profile_faces[i].x +profile_faces[i].width/2, profile_faces[i].y + profile_faces[i].height/2);
            ellipse(frame, center, Size(profile_faces[i].width/2, profile_faces[i].height/2),
                    0, 0, 360, Scalar( 255,255,0 ), 4, 8, 0 );
            facesAngles.push_back(angle(center.x,0));
            cout << angle(center.x,1) << endl;

        }
        Mat flip_frame_gray;
        flip(frame_gray,flip_frame_gray, 1);
        profileFace_cascade.detectMultiScale(flip_frame_gray, profile_faces_flip, 1.1,6,0|CASCADE_SCALE_IMAGE, Size(10, 30));
        cout <<" profile_faces_flip"<<profile_faces_flip.size()<<endl;
        // cout <<"frontal "<<faces.size()<<endl;

        for( size_t i = 0; i < profile_faces_flip.size(); i++ )
        {
            // Find center of faces
            Point center(profile_faces_flip[i].x +profile_faces_flip[i].width/2, profile_faces_flip[i].y + profile_faces_flip[i].height/2);
            ellipse(frame, center, Size(profile_faces_flip[i].width/2, profile_faces_flip[i].height/2),
                    0, 0, 360, Scalar( 255,0,0 ), 4, 8, 0 );
            facesAngles.push_back(angle(center.x,1));
            cout << angle(center.x,1) << endl;
        }

    }
    if(faces.size()==s&&profile_faces.size()==s)
    {
        body_cascade.detectMultiScale( frame_gray, bodies, 1.1,6, 0|CASCADE_SCALE_IMAGE, Size(50, 50) );
        cout<<" bodies " << bodies.size() <<endl;
        for ( size_t i = 0; i< bodies.size(); i++ )
        {
            Point center( bodies[i].x + bodies[i].width/2, bodies[i].y + bodies[i].height/2 );
            ellipse( frame, center, Size( bodies[i].width/2, bodies[i].height/2 ), 0, 0, 360, Scalar( 0,255, 255 ), 4, 8, 0 );
            facesAngles.push_back(angle(center.x,0));
            cout << angle(center.x,1) << endl;

        }
    }
    // Show what you got
    imshow( window_name, frame );
    return facesAngles;
}
double angle(double x,bool flipped)
{
    int positionAngle = x*dpp;
    if(flipped) positionAngle=154-positionAngle; //154 is the whole field of view it may differ
    return positionAngle;
}
Mat RunFaceDetection()
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
    return frame;
}

