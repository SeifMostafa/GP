#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <stack>




using namespace std;
using namespace cv;

String body_cascade_name = "haarcascade_mcs_upperbody.xml";
String frontalFace_cascade_name = "haarcascade_frontalface_alt.xml";
String profileFace_cascade_name ="haarcascade_profileface.xml";
CascadeClassifier body_cascade;
CascadeClassifier frontalFace_cascade;
CascadeClassifier profileFace_cascade;
String window_name = "Capture - Face detection";

stack<double> faces_x ;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t detect_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t motor_cond = PTHREAD_COND_INITIALIZER;

double dpp;
double angle(int x){
return x*dpp;
}
void* PlayAudio(void *SizeOfFaces){
long f = *(long *) SizeOfFaces;
cout<<"THIS IS PLATAUDIO FUNCTION HERE TO SYSTME CALL OMXPLAYER "<<endl;
   cout<<"WITH NUMBER OF FACES DETECTED = "<<f<<endl;
    // here we should system call the omxplayer to play our audio file
    // system("omxplayer [FILENAME.WAV]");
pthread_exit(NULL);
}
void* MoveMotor(void *){
    while(1){
           //pthread_mutex_trylock(&mut);



if(!faces_x.empty()){

   cout<<"Motor Moving To Position  : "<<(double)faces_x.top()<<endl;
cout<<"Faces left in stak : "<<faces_x.size()<<endl;
    faces_x.pop();

}else{
    cout<<"NO POSITIONS TO MOVE----------- "<<endl;
}

pthread_cond_signal(&detect_cond);
pthread_cond_wait(&motor_cond,&mut);
    }

// send faces points to arduino through serial
// in a loop send point by point
// serial.write()


// we can let the arduino send a feedback through serial when it finish moving to all faces
pthread_exit(NULL);
}
void* detectAndDisplay( void* )
{

    Mat frame1,frame2;
    int width=640;
    double FOV=180,dpp=FOV/((double)width);
    //-- 1. Load the cascades
    if( !body_cascade.load( body_cascade_name ) ){ printf("--(!)Error loading body cascade\n"); pthread_exit(NULL); };
    if( !frontalFace_cascade.load( frontalFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); pthread_exit(NULL); };
    if( !profileFace_cascade.load( profileFace_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); pthread_exit(NULL); };

    VideoCapture capture;
    capture.open(0);
    capture.open(1);
    while(faces_x.empty()){

		capture.read(frame1);
		capture.read(frame2);
        int rows = max(frame1.rows, frame2.rows);
        int cols = frame1.cols + frame2.cols;
        Mat3b frame(rows, cols, Vec3b(0,0,0));
        frame1.copyTo(frame(Rect(0, 0, frame1.cols, frame1.rows)));
        frame2.copyTo(frame(Rect(frame1.cols, 0, frame2.cols, frame2.rows)));

    std::vector<Rect> bodies;
    Mat frame_gray=Mat::zeros( frame.size(), frame.type() );
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect bodies
    body_cascade.detectMultiScale( frame_gray, bodies, 1.2, 4, 0|CASCADE_SCALE_IMAGE, Size(70, 300) );
    for ( size_t i = 0; i < bodies.size(); i++ )
    {

        std::vector<Rect> faces;
        Point face_center;
        Point center( bodies[i].x + bodies[i].width/2, bodies[i].y + bodies[i].height/2 );
        ellipse( frame, center, Size( bodies[i].width/2, bodies[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        Mat faceROI = frame_gray( bodies[i] );
        //-- In each body, detect faces
        bool check=true;
        frontalFace_cascade.detectMultiScale( faceROI, faces, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
        for ( size_t j = 0; j < faces.size(); j++ )
        {
            check=false;
             face_center = Point( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
            int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
            circle( frame, face_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            cout<<"angle 1 : "<<angle(face_center.x)<<endl;
        }
        if(check){
        profileFace_cascade.detectMultiScale( faceROI, faces, 1.1, 6, 0 |CASCADE_SCALE_IMAGE, Size(10, 30) );
        for ( size_t j = 0; j < faces.size(); j++ )
        {
             face_center=Point( bodies[i].x + faces[j].x + faces[j].width/2, bodies[i].y + faces[j].y + faces[j].height/2 );
            int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
            circle( frame, face_center, radius, Scalar( 255, 255, 0 ), 4, 8, 0 );
            cout<<"angle 2 : "<<angle(face_center.x)<<endl;
        }
        }
        faces_x.push(angle(face_center.x));
        cout<<"NUMBER OF FACES DETECTED = "<<faces_x.size()<<endl;
    }

       //pthread_cond_wait(&detect_cond,&mut);

        waitKey(250);
    //-- Show what you got
        imshow( window_name,frame );

        pthread_cond_signal(&motor_cond);
        pthread_cond_wait(&detect_cond,&mut);
  }
        pthread_cond_signal(&motor_cond);
        pthread_cond_wait(&detect_cond,&mut);
   //Mat frame = *(Mat *)framee;

pthread_exit(NULL);

}




int main( void )
{

    pthread_t Audio_t;
    pthread_t Detect_t;
    pthread_t Motor_t;
    long FacesSize;




 pthread_create(&Detect_t,NULL,detectAndDisplay,NULL);
     //   pthread_join(Detect_t,NULL);
        FacesSize = faces_x.size();

pthread_create(&Motor_t, NULL,MoveMotor,NULL);
//pthread_join(Motor_t,NULL);

        //pthread_join(Audio_t,NULL);
       //detectAndDisplay( frame );

pthread_exit(NULL);
}
