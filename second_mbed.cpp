#include "mbed.h"
include "uLCD_4DGL.h"
include "JPEGCamera.h"
DigitalOut myled1(LED1); 
//show successful picture was taken 
DigitalOut myled2(LED2); 
//show end of sequence 
DigitalOut myled3(LED3); 
//show picture take failed 
DigitalOut myled4(LED4); 
//show camera is not ready 
uLCD_4DGL uLCD(p9, p10, p11); 
DigitalIn signal(p20); 
Serial pc(USBTX, USBRX);

int main() { 
	uLCD.printf("HeLLO"); 
	while(1) { 
		pc.printf("Signal is: %f \n", signal); 
		if(signal == 1) { 
			uLCD.printf("Hello"); 
			JPEGCamera camera(p13, p14); TX, RX LocalFileSystem local("local"); 
			//save images on mbed 
			camera.setPictureSize(JPEGCamera::SIZE640x480); 
			Timer timer; 
			timer.start(); 
			if(camera.setPictureSize(JPEGCamera::SIZE640x480)) { 
				myled1 = 1; wait(2.0); myled1 = 0; wait(2.0); 
			} 
			for (int i = 1; i < 5; i++) { 
				if (camera.isReady()) { 
					char filename[32]; sprintf(filename, "/local/pict%03d.jpg", i); uLCD.printf("Picture: %s \n", filename); 
					if (camera.takePicture(filename)) { 
						while (camera.isProcessing()) { 
							camera.processPicture(); 
						}
						myled1 = 1; 
						//show successful picture was taken 
						wait(2.0); myled1 = 0; 
					} else { 
						uLCD.printf("take picture failed\n"); 
						myled3 = 1; 
						//show picture take failed 
						wait(2.0); 
						myled3 = 0; 
					} 
				} else { 
					uLCD.printf("camera is not ready\n"); 
					myled4 = 1; 
					//show camera is not ready 
					wait(2.0); 
					myled4 = 0; 
				} 
			} 
			myled2 = 1; 
			//show end of sequence 
			wait(2.0); 
			myled2 = 0; 
			uLCD.printf("time = %f\n", timer.read()); 
		} 
	} 
}