#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
using namespace cv;
void detectAndDisplay( Mat frame );
String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String frontalFace_cascade_name = "haarcascade_frontalface_alt.xml";
String profileFace_cascade_name ="haarcascade_profileface.xml";
CascadeClassifier body_cascade;
CascadeClassifier frontalFace_cascade;
CascadeClassifier profileFace_cascade;
double angle(double x);
String window_name = "Capture - Face detection";
int width=640;
double FOV=180,dpp=FOV/((double)width);
class Dbrw
{
public:
    static void write(double angle)
    {
        ofstream outfile;
        outfile.open("/home/pi/Documents/REPO/GP/dbrw",ios::out | ios::trunc );
        outfile<<angle<<endl;
        outfile.close();
    };
    static  double read()
    {
        ifstream infile;
        infile.open ("/home/pi/Documents/REPO/GP/dbrw",  ios::out | ios::in );
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

int main( void )
{
     Mat frame1,frame2;

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

     VideoCapture capture;
 
     while(1)
     {
             capture.open(0);
		capture.read(frame1);
   	  capture.open(1);
         capture.read(frame2);
         int rows = max(frame1.rows, frame2.rows);
         int cols = frame1.cols + frame2.cols;
         Mat3b frame(rows, cols, Vec3b(0,0,0));
         frame1.copyTo(frame(Rect(0, 0, frame1.cols, frame1.rows)));
         frame2.copyTo(frame(Rect(frame1.cols, 0, frame2.cols, frame2.rows)));

         detectAndDisplay( frame );
         waitKey(250);
     }
     return 0;

}
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> bodies;
    Mat frame_gray=Mat::zeros( frame.size(), frame.type() );
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect bodies
    body_cascade.detectMultiScale( frame_gray, bodies, 1.2, 4, 0|CASCADE_SCALE_IMAGE, Size(70, 300) );
    for ( size_t i = 0; i < bodies.size(); i++ )
    {
        Point center( bodies[i].x + bodies[i].width/2, bodies[i].y + bodies[i].height/2 );
        ellipse( frame, center, Size( bodies[i].width/2, bodies[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        Mat faceROI = frame_gray( bodies[i] );
        std::vector<Rect> faces;
        //-- In each body, detect faces
        bool check=true;
        frontalFace_cascade.detectMultiScale( faceROI, faces, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
        for ( size_t j = 0; j < faces.size(); j++ )
        {
            check=false;
            Point face_center( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
            int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
            circle( frame, face_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            angle(face_center.x);
        }
        if(check)
        {
            profileFace_cascade.detectMultiScale( faceROI, faces, 1.1, 6, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
            for ( size_t j = 0; j < faces.size(); j++ )
            {
                Point face_center( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
                int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
                circle( frame, face_center, radius, Scalar( 255, 255, 0 ), 4, 8, 0 );
                angle(face_center.x);
            }
        }
    }
    //-- Show what you got
    imshow( window_name, frame );
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
