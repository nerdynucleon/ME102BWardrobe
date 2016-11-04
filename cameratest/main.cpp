#include "main.hpp"
using namespace cv;
using namespace std;

#define WIDTH 1280
#define HEIGHT 720
#define FRAMERATE 60

#define CAM_CONFIG_LENGTH 512
static const char * CAM_CONFIG_TEMPLATE = "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)%d, height=(int)%d,format=(string)I420, framerate=(fraction)%d/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";

String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

int main(int argc, char** argv)
{ 
	printf("OPENCV:%d.%d\n",CV_MAJOR_VERSION,CV_MINOR_VERSION);
	char CAM_CONFIG [CAM_CONFIG_LENGTH];
	sprintf(CAM_CONFIG, CAM_CONFIG_TEMPLATE, WIDTH, HEIGHT, FRAMERATE);

	// Select Camera
	VideoCapture cap;
	if(argc == 2)
		cap = VideoCapture(CAM_CONFIG);
	else
		cap = VideoCapture(1);
	
	if(!cap.isOpened()) {
		printf("Fail to open camera\n");
		return -1;
	}
	//cap.set(CAP_PROP_FPS, 30);
	printf("framerate:%f\n",cap.get(CAP_PROP_FPS));

	/* //Face Detection Nonsense
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
   	if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	*/
	for(;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		imshow("hi!",frame);
		//detectAndDisplay(frame);
		waitKey(1);
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	cap.release();
}

/* //Face Detection Nonsense
void detectAndDisplay( Mat frame )
{
  std::vector<Rect> faces;
  Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //-- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

  for( size_t i = 0; i < faces.size(); i++ )
  {
    Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
    ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    Mat faceROI = frame_gray( faces[i] );
    std::vector<Rect> eyes;

    //-- In each face, detect eyes
    eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    for( size_t j = 0; j < eyes.size(); j++ )
     {
       Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
       int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
       circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
     }
  }
  //-- Show what you got
  imshow( "face detection", frame );
 }
*/


