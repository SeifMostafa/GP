#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;
using namespace cv;
std::vector<double> runDetection( Mat frame );
double angle(double x,bool flipped);
String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String frontalFace_cascade_name = "haarcascade_frontalface_alt.xml";
String profileFace_cascade_name ="haarcascade_profileface.xml";
CascadeClassifier body_cascade;
CascadeClassifier frontalFace_cascade;
CascadeClassifier profileFace_cascade;
String window_name = "Capture - Face detection";
    int width=640;
    double FOV=180,dpp=FOV/((double)width);

int main( void )
{
    Mat frame1,frame2;

    //Load the cascades
    if( !body_cascade.load( body_cascade_name ) ){ printf("--(!)Error loading body cascade\n"); return -1; };
    if( !frontalFace_cascade.load( frontalFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    if( !profileFace_cascade.load( profileFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };

    VideoCapture cap(1);// Open default camera
    VideoCapture cap1(0);
    cap.read(frame1);
    cap1.read(frame2);
    while(cap.read(frame1))
    {
        cap.read(frame1);
        cap1.read(frame2);
        //Binding
        int rows = max(frame1.rows, frame2.rows);
        int cols = frame1.cols + frame2.cols;
        Mat3b frame(rows, cols, Vec3b(0,0,0));
        frame1.copyTo(frame(Rect(0, 0, frame1.cols, frame1.rows)));
        frame2.copyTo(frame(Rect(frame1.cols, 0, frame2.cols, frame2.rows)));

        runDetection(frame); // Call function to detect faces
        imshow( window_name, frame );
        if( waitKey(30) >= 0)    // pause
            break;
    }
    return 0;
}
std::vector<double> runDetection( Mat frame )
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
        }

    }
    if(faces.size()==s&&profile_faces.size()==s)
    {
        body_cascade.detectMultiScale( frame_gray, bodies, 1.1,6, 0|CASCADE_SCALE_IMAGE, Size(50, 50) );
        cout<<" bodies " <<bodies.size();
        for ( size_t i = 0; i< bodies.size(); i++ )
        {
            Point center( bodies[i].x + bodies[i].width/2, bodies[i].y + bodies[i].height/2 );
            ellipse( frame, center, Size( bodies[i].width/2, bodies[i].height/2 ), 0, 0, 360, Scalar( 0,255, 255 ), 4, 8, 0 );
            facesAngles.push_back(angle(center.x,0));
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
