#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>

#include <wiringPi.h>
#include <softPwm.h>

using namespace std;
using namespace cv;

void detectAndDisplay( Mat frame );
String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String frontalFace_cascade_name = "haarcascade_frontalface_alt.xml";
String profileFace_cascade_name ="haarcascade_profileface.xml";
CascadeClassifier body_cascade;
CascadeClassifier frontalFace_cascade;
CascadeClassifier profileFace_cascade;
String window_name = "Capture - Face detection";

void RunServo(int pos){
wiringPiSetup();
pinMode(1,PWM_OUTPUT);
int c=0;
while(c<pos)
{
pwmWrite(1,1);
}

}
Mat3b binding(Mat img1,Mat img2){

    // Get dimension of final image
    int rows = max(img1.rows, img2.rows);
    int cols = img1.cols + img2.cols;

    // Create a black image
    Mat3b res(rows, cols);

    // Copy images in correct position
    img1.copyTo(res(Rect(0, 0, img1.cols, img1.rows)));
    img2.copyTo(res(Rect(img1.cols, 0, img2.cols, img2.rows)));
return res;
}
void printcenters(vector<Point>centers){
for(int u=0;u<centers.size();u++){
cout<<centers.get(u);
}
}
int main( void )
{
    Mat frame,frame2;
    //-- 1. Load the cascades
    if( !body_cascade.load( body_cascade_name ) ){ printf("--(!)Error loading body cascade\n"); return -1; };
    if( !frontalFace_cascade.load( frontalFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    if( !profileFace_cascade.load( profileFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };

    VideoCapture capture;
    while(1){
        capture.open(0);
	capture.read(frame);
 	capture.open(1);
	capture.read(frame2);

        detectAndDisplay( binding(frame,frame2) );
        waitKey(250);
    }
    return 0;
}

void detectAndDisplay( Mat frame )
{
	vector<Point>Centers = new vector<Point>;

    std::vector<Rect> bodies;
    Mat frame_gray=Mat::zeros( frame.size(), frame.type() );


     /// Do the operation new_image(i,j) = alpha*image(i,j) + beta
/* for( int y = 0; y < frame.rows; y++ )
    { for( int x = 0; x < frame.cols; x++ )
         { for( int c = 0; c < 3; c++ )
              {
      frame_gray.at<Vec3b>(y,x)[c] =
         saturate_cast<uchar>( 2*( frame.at<Vec3b>(y,x)[c] ) +30 );
             }
    }
    }
    imshow("Modified",frame_gray);*/


    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    //imshow("Modified_gray",frame_gray);
    equalizeHist( frame_gray, frame_gray );
    //imshow("Modified_equalized",frame_gray);

    //-- Detect bodies
    body_cascade.detectMultiScale( frame_gray, bodies, 1.2, 4, 0|CASCADE_SCALE_IMAGE, Size(70, 300) );
    for ( size_t i = 0; i < bodies.size(); i++ )
    {
        Point center( bodies[i].x + bodies[i].width/2, bodies[i].y + bodies[i].height/2 );
	Centers.push(center);
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
        }
        if(check){
        profileFace_cascade.detectMultiScale( faceROI, faces, 1.1, 6, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
        for ( size_t j = 0; j < faces.size(); j++ )
        {
            Point face_center( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
            int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
            circle( frame, face_center, radius, Scalar( 255, 255, 0 ), 4, 8, 0 );
        }
        }

    }
    //-- Show what you got
	printcenters(Centers);
    imshow( window_name, frame );
}

