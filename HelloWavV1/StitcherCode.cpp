#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"

using namespace std;
using namespace cv;

bool try_use_gpu = false;
string result_name = "result.jpg";

int main()
{
    vector<Mat> imgs;
    // add images...
    Mat right=imread("R.jpg");
    Mat left=imread("L.jpg");
    imgs.push_back(right);
    imgs.push_back(left);
    Mat pano;
    Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
    Stitcher::Status status = stitcher.stitch(imgs, pano);
    if (status != Stitcher::OK)
    {
        cout << "Can't stitch images, error code = " << int(status) << endl;
        return -1;
    }
    cout<< pano.size();
    imwrite(result_name, pano);
}
