#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// # include <opencv2/core/core.hpp>
 #include <opencv2/videoio.hpp>
 #include <opencv2/highgui/highgui.hpp>
// # include <opencv2/imgproc/imgproc.hpp> 
//#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp> 
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    char ch;
    bool pipe = false, video = false, image = false;
    char  *pipeName, *videoName, *imageName;
    while((ch = getopt(argc, argv, "p:v:i:")) != EOF)
    {
        switch (ch)
        {
            case 'p':
              
                pipe = true;
                pipeName = optarg;
                //cout<<"pipe: " << pipeName<<endl;
                break;
            case 'v':
                video = true;
                videoName = optarg;
                //cout<<"video: " << videoName<<endl;
                break;
            case 'i':
                image = true;
                imageName = optarg;
                //cout<<"image: " << imageName<<endl;
                break;
            default:
                fprintf(stderr, "Unknow option: '%s'\n", optarg);
                return 1;
        }
    }

    argc -= optind;
    argv += optind;    

    if (argc != 1)
    {
        fprintf(stderr, "capture: missing operand\nTry 'capture --help' for more information\n");
        return 1;
    }

    if (strcmp(argv[0], "--help") == 0)
    {
        printf("capture [-p PIPENAME] [-v VIDEONAME] [-i IMAGENAME] source\n");
        return 0;
    }

    VideoCapture capture(argv[1]);

    if (!capture.isOpened())
    {
        fprintf(stderr, "Can't open\n");
        return 1;
    }
    //cout<< "Open" << endl;

    //Video option
    VideoWriter videoFile;
    if (video)
    {
        Size S = Size((int) capture.get(CAP_PROP_FRAME_WIDTH),
                      (int) capture.get(CAP_PROP_FRAME_HEIGHT));
        
        int ex = static_cast<int>(capture.get(CAP_PROP_FOURCC));
        //int codec = CV_FOURCC('M','P','E','G');
        videoFile.open(videoName, ex,capture.get(CAP_PROP_FPS), S, true);
    }

    //Pipe option
    VideoWriter pipeFile;
    if (pipe)
    {
        if (access(pipeName, F_OK) == -1)
        {
            //Create fifo file
            if (mkfifo(pipeName, 0666) == -1)
            {
                fprintf(stderr, "Can't create fifo file\n");
                return 1;
            }
        }
        
        Size S = Size((int) capture.get(CAP_PROP_FRAME_WIDTH),
                      (int) capture.get(CAP_PROP_FRAME_HEIGHT));
        int ex = static_cast<int>(capture.get(CAP_PROP_FOURCC));
        pipeFile.open(pipeName, ex,capture.get(CAP_PROP_FPS), S, true);
    }

    Mat frame;
    
    while(capture.read(frame))
    {
        if (video)
        {
            videoFile << frame;
        }
        if (image)
        {
            imwrite(imageName, frame);
        }
        if (pipe)
        {
            pipeFile << frame;
        }
    }

    return 0;
}