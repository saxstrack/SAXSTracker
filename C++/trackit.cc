//#include "opencv/cv.h"
//#include "opencv/highgui.h"
//#include "opencv2/opencv.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "stdio.h"

#include <iostream>
#include <fstream>
//#include <sstream>
//#include <string>



// Sudden changes should be dampend, if line sticks to same place for several frames, then quick jumps should
// be either ignored or moved partly towards. I.e. believe the line that got most frames more.. 
// (like a second delay or something)

// I want to be able to call the C++ function from python.. starting the C++ tracking..
// then monitor the line location from python.. by calling a C++ function.

// As soon as the C function is called it will block.. how do I then monitor the internal
// line location values?



int find_top()
{
	
	//ssn
}


int main()  
{        
    int key = 0;
	int seek_line_idx = 120;
	
	CvPoint pt1 = cvPoint(0,seek_line_idx);
	CvPoint pt2 = cvPoint(350,seek_line_idx);
		
	CvPoint pt1_beam_right = cvPoint(180,0);
	CvPoint pt2_beam_right = cvPoint(180,250);
	
	CvPoint pt1_beam_left = cvPoint(160,0);
	CvPoint pt2_beam_left = cvPoint(160,250);
	
	cv::Scalar green(0,250,0);
	cv::Scalar red(0,0,250); // BGR
	cv::Scalar white(255,255,255);
	
	//std::vector<Point2f> circles;
	//CvScalar red = CV_RGB(250,0,0);
	
	int thickness = 1;
	int connectivity = 8;
	int sub;
	
    // Initialize camera and OpenCV image  
    //CvCapture* capture = cvCaptureFromCAM( 0 );  
    //CvCapture* capture = cvCaptureFromAVI( "sample_plug.avi" );    

	//******* Read background file: **************
	std::string strline;
	std::vector<double> record;
	//std::vector <std::vector <std::string> > data;	
	std::ifstream bgfile("example.txt");
	std::getline(bgfile, strline); //reads until a '\n' character
	
	//std::cout << strline << "\n";
	std::stringstream ss( strline );
	std::string field;
	std::getline(ss, field, ','); //reads until a ',' character
	
	// Convert read numbers to double:
	while (std::getline( ss, field, ','))
	{
		std::stringstream fs( field );
		double f = 0.0;
		fs >> f;
		
		// store the background values in record:
		record.push_back( f );
	}
	// ********************************************	
	//std::cout << record.front() << "\n";

	for(int i = 0; i < record.size(); i++)
	{
		sub = record.at(i);
		std::cout << sub << "\n";
		printf("%d\n", uchar(sub));
		std::cout << record.size() << "\n";
	}
	
	cv::VideoCapture capture("sample_plug.avi");
	cv::Mat frame, gray_frame;
	//cv::Vec3f circles;
	std::vector<cv::Vec3f> circles2;
	
	//IplImage* frame = cvQueryFrame( capture );
	//IplImage *grey_frame;
	
    // Check   
    if ( !capture.isOpened())   
    {  
        fprintf( stderr, "Cannot open AVI!\n" );  
        return 1;  
    }  
  
    // Get the fps, needed to set the delay  
    //int fps = ( int )cvGetCaptureProperty( capture, CV_CAP_PROP_FPS ); 
	int fps = capture.get(CV_CAP_PROP_FPS);
	
	capture >> frame; // Get first frame CV_8UC3
	
	printf("\nFPS: %f\n", float(fps));
	printf("Channels: %d\n", int(frame.channels()));
	
    // Create a window to display the video  
    cv::namedWindow("video", CV_WINDOW_AUTOSIZE );
	cv::namedWindow("Plot", CV_WINDOW_AUTOSIZE );
   
	cv::Mat plt(300, frame.cols, CV_8UC1);
	
	printf("Cols: %d\n", int(frame.cols));
	printf("Rows: %d\n", int(frame.rows));
	printf("Channels: %d\n", int(frame.channels()));

	int frame_count = 0;
	int frame_step = 1;
	int line_location = -1;
	//int window_length = 15;
	
	float old_frame[frame.cols];
	float subtract[frame.cols];
	float clean_signal[frame.cols];
	float filtered_signal[frame.cols];
	
	int step_key = 0;
	
    while( key != 'x' )   
    {  
        // get the image frame
        //frame = cvQueryFrame( capture );
		
		// Get a new frame
		capture >> frame; 

		// if( frame_count == frame_step)
		// 	frame_count = 0;
		// else
		// 	frame_count++;
			
	    // exit if unsuccessful
	    if( frame.empty() ) break;
	
		//Convert frame to gray scale
		cv::cvtColor(frame, gray_frame, CV_RGB2GRAY);
		
		//smooth the gray_scale frame
		cv::GaussianBlur(gray_frame, gray_frame, cv::Size (11, 11), 0);
		
		// Add ability to pause and step through frames
		if(step_key == 1)
			key = 'p';
		
		if( key == 'p' ){
			key = 0;
			while( key != 'p' && key != 27 ){
				key = cvWaitKey( 250 );
				
				if (key == 's'){
					// Write background to file:
					printf("Save check!!\n");
					// std::ofstream myfile("example.txt", std::ios::app);
					// 
					// for(int i = 0; i <frame.cols; i++){
					// 	uchar val = gray_frame.at<uchar>(seek_line_idx, i);
					// 	myfile << int(val);
					// 	myfile << ",";
					// }
					// myfile << int(gray_frame.at<uchar>(seek_line_idx, frame.cols));
					// 
					// myfile << "\n";
					// myfile.close();
					key = 0;
				}
					
				if (key == 'n'){
					printf("Step!\n");
					step_key = 1;
					break;
				}
				else
					step_key = 0;				
			}
		}

	
		// *********** Plot background subtracted seek_line ****************	 
		int res;
		for(int i = gray_frame.cols-1; i >= 0 ; i--)
		{
			//Get image intensity on seek_line:
			uchar val = gray_frame.at<uchar>(seek_line_idx, i);
			
			//Get background intensity:
			sub = record.at(i);
			
			//Avoid chaos if itensity-bg < 0
			res = (255-val) + uchar(sub)-250;
			if(res < 0)
				res = 1;
			
			//Save itensity-bg value
			clean_signal[i] = res;
			
			// plot curve:
			plt.at<uchar>(res, i) = 250;
			
			//std::cout << res << "\n";	
		}
		
		// Plot tracking line:
		for(int i = gray_frame.cols; i >= 0; i--)
		{
			if(double(clean_signal[i]) > 80.0)
			{
				CvPoint pt1_plug = cvPoint(i, 0);
				CvPoint pt2_plug = cvPoint(i, 250);
				
				cv:line(gray_frame, pt1_plug, pt2_plug, white, thickness, connectivity);
				line_location = i;
				
				//std::cout << line_location << "\n";
				break;
			}
		}
		
		//cvCvtColor(frame, &grey_frame, CV_BGR2GRAY); 
		//Draw horizontal line across the image to indicate seek_line
		cv::line(gray_frame, pt1, pt2, red, thickness,connectivity);
		cv::line(gray_frame, pt1_beam_right, pt2_beam_right, red, thickness, connectivity);
		cv::line(gray_frame, pt1_beam_left, pt2_beam_left, red, thickness, connectivity);
	
		//cv::line(gray_frame, pt1_beam, pt2_beam, green, thickness,connectivity);

        // display current frame   
        cv::imshow( "video", gray_frame ); 
		cv::imshow("Plot", plt);
		
		//cvReleaseImage(&grey_frame);
        // exit if user presses 'x'          
        key = cv::waitKey( 1000 / fps );  

		// Clear plot:
		plt = plt * 0;
    }  
   
    // Tidy up  
    cv::destroyWindow( "video" );  
	frame.release();
	gray_frame.release();
    capture.release();
    return 0;
}