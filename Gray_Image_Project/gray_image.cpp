#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

void convert(string directory, string imageName)
{
    Mat image;
    image = imread(directory + "/" +imageName);
    if (! image.data)
    {
       // printf("Could not open or find the image\n");
        return;
    }
    //cout<< imageName <<endl;
    Mat image_BW ;
    cvtColor(image, image_BW, CV_RGB2GRAY, 0);

    imwrite(directory + "/GRAY_" + imageName, image_BW);

}



int main()
{

    int fd[2];
    if (pipe(fd) == -1)
    {
        cout<<"Can't create the pipe"<<endl;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        cout<<"Can't fork process"<<endl;

    }
    else if (pid == 0)
    {
        //in child process
        dup2(fd[1], 1); //standard output
        close(fd[0]);
        if (system("ls -R \"`zenity --file-selection --directory`\"") == -1)
        {
            cout<< "Can't run script" <<endl;
        }
        return 0;
    }
    else{
        //in parent process
        dup2(fd[0], 0); //standard input
        close(fd[1]);   

        
        string imageName;

    
            string directory;
            while(getline(cin, directory))
            {
                directory.erase(directory.end() - 1);
                string name;
                while(getline(cin, name))
                {
                    if (name == "\n")
                        break;
                    else
                    {
                        //cout << directory << name <<endl;
                        convert(directory, name);
                    }
                }
            }
            


    }

    return 0;
}