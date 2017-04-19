#include <opencv2\opencv.hpp>
using namespace cv;
int main()
{
    // Load images
    Mat3b img1 = imread("1.jpg");
    Mat3b img2 = imread("2.jpg");

    // Get dimension of final image
    int rows = max(img1.rows, img2.rows);
    int cols = img1.cols + img2.cols;

    // Create a black image
    Mat3b res(rows, cols, Vec3b(0,0,0));

    // Copy images in correct position
    img1.copyTo(res(Rect(0, 0, img1.cols, img1.rows)));
    img2.copyTo(res(Rect(img1.cols, 0, img2.cols, img2.rows)));

    // Show result
    imshow("Img 1", img1);
    imshow("Img 2", img2);
    imshow("Result", res);
    waitKey();

    return 0;
}
