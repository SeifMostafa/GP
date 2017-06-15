#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>    // std::sort
#include <stdlib.h>     /* abs */
#include <stdio.h>
#include <vector>
using namespace std;
using namespace cv;

static const int DELAY_PER_1ANGLE =40 ; // in msec
static const int ORG_SOUNDFILE_LENGTH = 5000; // in msec
static const int ANGLE_THRESHOL=10; // if two persons are in range of this threshold will meet them up and play the ad only one.
std::vector<double> RunFaceDetection( Mat frame );
int GetWaitTime(int stepAngleMotor, int soundFileLength, std::vector<double> AnglesVector,std::vector<double>WellSortedAnglesVector,std::vector<pair<int,int> > & GroupOfPeopleInRange);

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

class Dbrw
{
public:
    static void write(double angle)
    {
        ofstream outfile;
        outfile.open("/home/azizax/Documents/fci/GP/CODE/GP/dbrw",ios::out | ios::trunc );
        outfile<<angle<<endl;
        outfile.close();
    };
    static  double read()
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
};
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
        std::vector<int,int>GroupOfPeopleInRange;
        TimeToWait = GetWaitTime(DELAY_PER_1ANGLE,ORG_SOUNDFILE_LENGTH,AnglesVector,WellSortedAnglesVector,std::vector<std::pair<int,int> >GroupOfPeopleInRange);

        /// call motor , sound files
            for(int i=0;i<WellSortedAnglesVector.size();i++){

            }
        /// thread sleep to change the stopFaceDetection back to false

    }
    return 0;
}

// all numbers in msec
int GetWaitTime(int stepAngleMotor, int soundFileLength, std::vector<double> AV,std::vector<double>WellSortedAnglesVector,std::vector<std::pair<int,int>>GroupOfPeopleInRange)
{
    std::sort (AV.begin(), AV.end());
    int soundtime =0;
    int motortime =0;
    int currentPosition = Dbrw::read();
    // the current -- > min .. max
    if(AV.at(0)>currentPosition && AV.at(AV.size()-1)> currentPosition)
    {

        motortime+=abs(AV.at(AV.size()-1)-currentPosition);
        for(int i=0; i<AV.size(); i++)
        {
            WellSortedAnglesVector.push_back(AV.at(i));
        }

    }
    //  min .. max -- > current

    else if(AV.at(0)< currentPosition && AV.at(AV.size()-1)< currentPosition)
    {

        motortime+=abs(currentPosition - AV.at(0));
        for(int i=AV.size()-1; i>0; i--)
        {
            WellSortedAnglesVector.push_back(AV.at(i));
        }

    }
    // min --> the current -- > max
    else
    {

        int indexNearest=0;

        int diff = 180,temp=0,temp2=0;
        for(int i=0; i<AV.size(); i++)
        {
            temp =  currentPosition - AV.at(i);
            temp2 =  abs(temp);
            if(temp2<diff)
            {
                diff = temp2;
                indexNearest=i;
            }
        }
        if(temp>0)
        {
            // current to min
            motortime+=abs(currentPosition - AV.at(0));
            for(int j=indexNearest; j>=0; j--)
            {
                WellSortedAnglesVector.push_back(AV.at(j));

            }
            // min to max
            motortime+=abs(AV.at(0) - AV.at(AV.size()-1));

            for(int j=indexNearest+1; j<AV.size(); j++)
            {
                WellSortedAnglesVector.push_back(AV.at(j));

            }

        }
        else
        {
            // current to max
            motortime+=abs(currentPosition - AV.at(AV.size()-1));

            for(int j=indexNearest+1; j<AV.size(); j++)
            {
                WellSortedAnglesVector.push_back(AV.at(j));

            }
            // max to min
            motortime+=abs(AV.at(0) - AV.at(AV.size()-1));

            for(int j=indexNearest; j>=0; j--)
            {
                WellSortedAnglesVector.push_back(AV.at(j));
            }
        }
    }

/// thresholding how many times ad will be played and count times
    int counterOfplayedTimes =0;
    for(int i=0; i<AV.size()-1; i++)
    {
        if(abs(AV.at(i)-AV.at(i+1))<=ANGLE_THRESHOL)
        {
            counterOfplayedTimes++;
            GroupOfPeopleInRange.push_back(std::make_pair(i,i+1));
        }
    }
    soundtime =soundtime+ (AV.size()-counterOfplayedTimes)*ORG_SOUNDFILE_LENGTH;

    return soundtime+motortime;

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
    // read prev
    int prev = Dbrw::read();

    int val = x*dpp - prev;

    int temp_prev = prev;
    prev= prev+val;

    if(val<0)
    {
        val=val*-1;
    }
    if(prev>180)
    {
        prev=prev-temp_prev;
        // write prev
        Dbrw::write(prev);

    }
    return val;
}
