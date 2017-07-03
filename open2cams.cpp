#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

String window_name = "Face Detection";

Mat frame1,frame2;
VideoCapture capture,capture1;
Mat RunFaceDetection()
{
    capture.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    capture.open(0);
    capture.read(frame1);

    capture1.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    capture1.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    capture1.open(1);
    capture1.read(frame2);

    int rows = max(frame1.rows, frame2.rows);
    int cols = frame1.cols + frame2.cols;
    Mat3b frame(rows, cols, Vec3b(0,0,0));
    frame1.copyTo(frame(Rect(0, 0, frame1.cols, frame1.rows)));
    frame2.copyTo(frame(Rect(frame1.cols, 0, frame2.cols, frame2.rows)));
    Size size(1280,720);
    //resize(frame,frame,size);
	//imwrite("open2cams",frame);
    return frame;
}

int main() {

  while(1) {
    Mat frame = RunFaceDetection();
    imshow( window_name, frame );
    if( waitKey(30) >= 0)    // pause
      break;
  }
  return 0;
}


