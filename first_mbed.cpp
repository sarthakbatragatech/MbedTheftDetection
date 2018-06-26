//Import necessary modules
#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include "MMA8452.h"
#include "ultrasonic.h"

//Initializa variables
Serial pc(USBTX,USBRX); 
Serial blue(p13,p14); 
DigitalOut led4(LED4); 
DigitalOut led3(LED3); 
DigitalOut led2(LED2); 
DigitalOut led1(LED1); 
AnalogIn tmp(p15); 
uLCD_4DGL uLCD(p9, p10, p11); 
MMA8452 acc(p28, p27, 100000); 
PwmOut speaker(p23); 
ultrasonic mu(p6, p7, .1, 1); 
Thread thread1; Thread thread2; 
Thread thread3; Thread thread4; 
Thread thread5; Thread thread6; 
Mutex m; DigitalOut camera_trigger(p18); 
DigitalOut wifi_trigger(p18); 
DigitalIn pb1(p20); 
DigitalIn pb2(p19); 
DigitalIn pb3(p17);

float tempC, tempF; 
double x, y, z, dist_accel; 
int dist_sonar; int i; 
volatile bool sound = false; 
volatile bool arm = false;

void temp_thread() {
	while(1) {
		float vin = 5.0 * LM61 / 1024.0;
		tempC = (1.8663 - vin) / 0.01169; tempF = 1.8 * tempC + 32.0;
		tempC = ((tmp * 3.3) * 100 - 50); tempF = 1.8 * tempC + 32.0; 

		//mutex lock
		m.lock(); 
		uLCD.locate(1,1); 
		uLCD.printf("%5.2F C %5.2F F \n\r", tempC, tempF); 
		m.unlock(); 

		//Adjusted Thread wait time
		Thread::wait(900); 
	}
}

//Accelerometer Thread
void accel_thread() {
	while(1) {
		acc.readXYZGravity(&x,&y,&z); 
		m.lock(); 
		uLCD.locate(0,3); 
		uLCD.printf("x:%lf\n",x); 
		uLCD.locate(0,4); 
		uLCD.printf("y:%lf\n",y); 
		uLCD.locate(0,5); 
		uLCD.printf("z:%lf\n",z); 
		m.unlock(); 
		dist_accel = (x*x + y*y + z*z)^0.5; 
		uLCD.printf("dist:%lf\n", dist_accel); 
		Thread::wait(800); 
	}
}

void dist(int distance) { 
	//put code here to execute when the distance has changed 
	uLCD.printf("Distance %d mm\r\n", distance); 
	//if less than 50mm, alarm activated 
}

ultrasonic mu(p6, p7, .1, 1, &dist);

//Sonar Thread
void sonar_thread() {
	while(1) { 
		mu.startUpdates();
		//start measuring the distance --SONAR 
		mu.checkDistance(); 
		m.lock(); 
		uLCD.locate(0,10); 
		dist_sonar = mu.getCurrentDistance(); 
		uLCD.printf("Distance: %d", dist_sonar); 
		wait(0.2); uLCD.printf(" "); 
		m.unlock(); 
		Thread::wait(700); 
	}
}

//Speaker Thread
void speaker_thread() {
	while(1) {
		led3 = !led3; 
		if(sound) {
			for (i=0; i<10; i=i+2) { 
				//--SPEAKER 
				speaker.period(1.0/969.0); 
				speaker = float(i)/50.0; 
				wait(.5); 
				speaker.period(1.0/800.0); 
				wait(.5); 
			} 
		} 
		Thread::wait(600); 
	} 
}

//Thresholds for Alarm Trigger
void checker_thread() { 
	while(1) { 
		if(arm) {
			if(tempC > 27 || x > 0.3 || y < -0.5 || dist_sonar < 50) { 
				sound = true; 
				camera_trigger = 1; 
				m.lock(); 
				uLCD.printf("DONE!"); 
				m.unlock(); 

				for (i=0; i<10; i=i+2) { 
					//--SPEAKER 
					speaker.period(1.0/969.0); 
					speaker = float(i)/50.0; 
					wait(.5); 
					speaker.period(1.0/800.0); 
					wait(.5); 
				} 
				led2 = !led2; 
			}
			if(tempC < 27 || x > 0.3 || y < -0.5 || dist_sonar > 50) { 
				sound = false; 
				speaker = 0.0; 
				camera_trigger = 0; 
			} 
			else { 
				sound = false; 
			} 
		} 
		Thread::wait(200); 
	}
}

int main() { 
	pb1.mode(PullUp); 
	pb2.mode(PullUp); 
	pb3.mode(PullUp);

	uLCD.locate(0,0); 
	uLCD.printf("Alarm System Menu"); 
	uLCD.locate(0,3); 
	uLCD.printf("1: Boot System"); 
	uLCD.locate(0,5); 
	uLCD.printf("2: How to Guide");

	while(1) { 
		if(pb1 == 0) { 
			uLCD.cls(); 
			break; 
		} if(pb2 == 0) {
			uLCD.cls(); 
			uLCD.locate(0,0); 
			uLCD.printf("Connect via BT"); 
			uLCD.locate(0,3); 
			uLCD.printf("'1' to arm"); 
			uLCD.locate(0,5); 
			uLCD.printf("'2' to disarm"); 
			uLCD.locate(0,7); 
			uLCD.printf("'PB1 to exit"); 
		} 
	}

	//Begin the threads
	thread1.start(temp_thread); 
	thread2.start(accel_thread); 
	thread3.start(sonar_thread); 
	thread4.start(speaker_thread); 
	thread5.start(checker_thread); 
	thread6.start(camera_thread);

	char bnum=0; 
	char bhit=0; 
	while(1) {
		m.lock(); 
		//Connect with the Bluetooth Module
		if(blue.readable()) {
			if (blue.getc()=='!') { 
				if (blue.getc()=='B') { 
					button data packet bnum = blue.getc(); 
					button number bhit = blue.getc(); 
					//1=hit, 0=release

					switch (bnum) { 
						case '1': // Arm the system					
								  if (bhit=='1') { 						  	
									arm = true; 
									pin = 0; 
									led1 = 1; 
								} break; 
						case '2': // Disarm the system 
								  if (bhit=='1') { 
								  	arm = false; 
								  	led1 = 0; 
								  } break; 
						case '3' : // Email for help
								  if (bhit = '1') {
								  } break; 
						default: break; 
					} 
				} 
			} 
		} 
		m.unlock(); 
		Thread::wait(100); 
	}
}