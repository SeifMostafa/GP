#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <windows.h>
#include "mmsystem.h"
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
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
String window_name = "Capture - Face detection";

void *SayHello(void *GuestsNo) {
   long n;
   n = (long)GuestsNo;
   switch(n){
        case 0:
               cout << "\nNo one is Here "<< endl;
            break;
        case 1:
        cout << "\nHello ..."<< n <<" Guests " << endl;
           PlaySound(TEXT("Hello-SoundBible.com-218208532.wav"),NULL,SND_SYNC);
            break;
        default:
            cout << "\nHello ..."<< n <<" Guests ........ 'CAN PLAY DIFFERENT .wav'" << endl;
           PlaySound(TEXT("Hello-SoundBible.com-218208532.wav"),NULL,SND_SYNC);
            break;

    }
   pthread_exit(NULL);
}

int main( void )
{
    Mat frame;
    //-- 1. Load the cascades
    if( !body_cascade.load( body_cascade_name ) ){ printf("--(!)Error loading body cascade\n"); return -1; };
    if( !frontalFace_cascade.load( frontalFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    if( !profileFace_cascade.load( profileFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };

    VideoCapture capture;
    capture.open(2);
    while(1){
		capture.read(frame);
        detectAndDisplay( frame );
        waitKey(250);
    }
    return 0;
}


void detectAndDisplay( Mat frame )
{
    int ss ;
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
    pthread_t threads;
    int rc ;

       if (bodies.size()==0 || bodies.size() == NULL){
               cout<<"no One is here\n";
               rc = pthread_create(&threads, NULL, SayHello, (void *)bodies.size());
        }
        else if(bodies.size()==1){
                rc = pthread_create(&threads, NULL, SayHello, (void *)bodies.size());
        }
        else if (bodies.size()>1){

                rc = pthread_create(&threads, NULL, SayHello, (void *)bodies.size());
        }
    //-- Show what you got

    imshow( window_name, frame );



}
