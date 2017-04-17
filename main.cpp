#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace cv;
void detectAndDisplay( Mat frame );
String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String body_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier body_cascade;
CascadeClassifier face_cascade;
String window_name = "Capture - Face detection";
int main( void )
{
    Mat frame;
    //-- 1. Load the cascades
    if( !body_cascade.load( body_cascade_name ) ){ printf("--(!)Error loading body cascade\n"); return -1; };
    if( !face_cascade.load( body_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    VideoCapture capture;
    capture.open(0);
    while(1){
		capture.read(frame);
        detectAndDisplay( frame );
        waitKey(100);
    }
    return 0;
}

void detectAndDisplay( Mat frame )
{
    std::vector<Rect> faces;
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
    //equalizeHist( frame_gray, frame_gray );
    //imshow("Modified_equalized",frame_gray);

    //-- Detect faces
    body_cascade.detectMultiScale( frame_gray, faces, 1.1, 4, 0|CASCADE_SCALE_IMAGE, Size(10, 30) );
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;
        //-- In each face, detect eyes
        face_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
        for ( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        }
    }
    //-- Show what you got
    imshow( window_name, frame );
}
