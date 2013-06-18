#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "stdio.h"
#include <iostream>
#include <fstream>

#define ARRAYSIZE(x)  (sizeof(x)/sizeof(*(x)))

int main(void)  
{        
    int key = 0;
	int seek_line_idx = 120;
	
	CvPoint pt1 = cvPoint(0,seek_line_idx);
	CvPoint pt2 = cvPoint(350,seek_line_idx);
		
	CvPoint pt1_beam_right = cvPoint(180,0);
	CvPoint pt2_beam_right = cvPoint(180,250);
	
	CvPoint pt1_beam_left = cvPoint(160,0);
	CvPoint pt2_beam_left = cvPoint(160,250);
	
	CvScalar red = CV_RGB(250,0,0);
	CvScalar green = CV_RGB(0,0,250);
	CvScalar white = CV_RGB(255,255,255);
	
	int thickness = 1;
	int connectivity = 8;
	int sub, res;
	int j;
	char buffer[BUFSIZ*2], *ptr;  // BUFSIZ = 1024
	int array[350]; 
	
	printf("Array Length: %d\n", (int)ARRAYSIZE(array));
	
	// Load background file:
	const char filename[] = "example.txt";
	FILE *file = fopen(filename, "r");
	if ( file ){	
		// Read newline terminated line:
	   //	for ( i = 0; fgets(buffer, sizeof(buffer) , file); ++i )
	   //	{
		fgets(buffer, sizeof(buffer) , file);
		
		printf("Buf Length: %d\n", (int)ARRAYSIZE(buffer));		
		//printf("%s\n\n", buffer);
		
		// Parse the comma-separated values from each line into 'array'	
		for (ptr = buffer, j = 0; j < ARRAYSIZE(array); ++j, ++ptr )
 		{
 		 	array[j] = (int)strtol(ptr, &ptr, 10);
			//printf("%d: %d\n", j, array[j]);

 		}
		// }
		fclose(file);
	}
	else{
		printf("Can't load example.txt");
		return 0;
	}
	
	// Initialize camera and OpenCV image  
    //CvCapture *capture = cvCaptureFromCAM( 0 );  
    CvCapture *capture = cvCaptureFromAVI( "sample_plug.avi" );    
    	
	IplImage *frame = cvQueryFrame( capture );
	IplImage *gray_frame = cvCreateImage( cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1 );
	
	int fps = ( int )cvGetCaptureProperty( capture, CV_CAP_PROP_FPS ); 
	
	printf("\nFPS: %f\n", float( fps ));
	printf("Image Width: %d\n", int( frame->width ));
	printf("Image Height: %d\n", int( frame->height ));

	cvNamedWindow("video", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("Plot", CV_WINDOW_AUTOSIZE );

	float clean_signal[frame->width];
	float subtract[frame->width];

	int step_key = 0;
	
	CvSize gauss_size = cvSize( 11, 11 );

	while( key != 'x' )
	{
		frame = cvQueryFrame( capture );
		
		if( !frame )
			break;
			
		cvCvtColor( frame, gray_frame, CV_RGB2GRAY ); 
		//cvGaussianBlur(gray_frame, gray_frame, gauss_size, 0);
		
		if( key == 'p'){
			key = 0;
			while( key != 'p' && key != 27 ){
				key = cvWaitKey( 250 );
			}
		}
		
		for( int i = gray_frame->width-1; i >= 0 ; i-- )
		{
			//Get image intensity on seek_line:
			//uchar val = gray_frame.at<uchar>(seek_line_idx, i);
			uchar val = CV_IMAGE_ELEM( gray_frame, uchar, seek_line_idx, i );
			
			//Get background intensity:
			sub = array[i];
			
			//Avoid chaos if itensity-bg < 0
			res = (255-val) + uchar( sub )-250;
			if(res < 0)
				res = 1;
			
			//Save itensity-bg value
			clean_signal[i] = res;
	
			// plot curve:
			//plt.at<uchar>(res, i) = 250;	
			//std::cout << res << "\n";	
		}
		
		for( int i = gray_frame->width; i >= 0; i-- )
		{
			if( double(clean_signal[i]) > 80.0 )
			{
				CvPoint pt1_plug = cvPoint( i, 0 );
				CvPoint pt2_plug = cvPoint( i, 250 );
				
				cvLine( gray_frame, pt1_plug, pt2_plug, white, thickness, connectivity );
				//line_location = i;
				break;
			}
		}
		
		cvLine(gray_frame, pt1, pt2, red, thickness,connectivity);
		//cvLine(gray_frame, pt1_beam_right, pt2_beam_right, red, thickness, connectivity);
		cvLine(gray_frame, pt1_beam_left, pt2_beam_left, red, thickness, connectivity);
		
		cvShowImage( "Plot", gray_frame );
		
		key = cvWaitKey( 1000 / fps ); 
		
	}

	cvReleaseCapture( &capture );
	//cvReleaseImage( &frame );  //This causes crash..why??
	//cvReleaseImage( &gray_frame );	
	cvDestroyWindow( "video" );
	cvDestroyWindow( "Plot" );
	
	return 0;
	
} //end