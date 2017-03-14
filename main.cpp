#include "mbed.h"
#include "CMPS03.h"
#include "C12832.h"
#include "GPS.h"
#include "MSCFileSystem.h"
using namespace std;

MSCFileSystem msc("usb"); // Mount flash drive under the name "msc"
Serial pc(USBTX,USBRX);

C12832 lcd(p5, p7, p6, p8, p11);
CMPS03 compass(p28, p27, CMPS03_DEFAULT_I2C_ADDRESS);
GPS gps(p9, p10);//tx and rx ports p9 and p10
DigitalIn press(p14);//joystick middle click
//DigitalOut led(LED1);//for testing the loops are working
Timer t;


int main()
{
    while(1){
    if(!gps.sample())
    {lcd.printf("Wait for a lock then press joy\n\n");}
    else if(press){ 
    
    //gps data file creation and collection of the first 10 coords to be averaged
    FILE*gpsFile=fopen("/usb/GPS_Data.csv","w");
    int i=9;
    lcd.printf("Starting Data Collection v1.4\n\n\n");
    wait(5.0);
    lcd.cls();

    while(i>=0) {
        if(gps.sample()) {
            lcd.cls();
            lcd.printf("%i\n\n\n",i);
            fprintf(gpsFile,"%f,%f\n",gps.latitude,gps.longitude);
            i--;
            wait(0.5);
        } else {
            lcd.printf("NO LOCK PLEASE WAIT\n\n\n");
        }
    }
    fclose(gpsFile);

    wait(0.1);
    lcd.cls();
    
    bearingloop:
if(press){
    //bearing data file creation and data collection
    lcd.printf("Creating Bearing data file");
    FILE*bearingFile=fopen("/usb/Bearing_Data.csv","w");
    t.start();
    while(t.read()<=180) {
        lcd.cls();
        fprintf(bearingFile,"%f\n",compass.readBearing()/10.0);
        wait(0.1);
        lcd.printf("%f/ mins remaining",t.read()/60.0);
    }
    fclose(bearingFile);
    t.stop();
}
else{
    goto bearingloop;
    }
//Collection of the final 10 coords to get the difference in GPS position
    wait(0.1);
    lcd.cls();


    lcd.printf("Collecting final set of 10 coords\n\n\n");
    fopen("/usb/GPS_Data.csv","a");
    wait(1);
    lcd.cls();

    int l=9;
    while(l>=0) {
        if(gps.sample()) {
            lcd.cls();
            lcd.printf("%i\n\n\n",l);
            fprintf(gpsFile,"%f,%f\n",gps.latitude,gps.longitude);
            l--;
            wait(0.5);
        } else {
            lcd.printf("NO LOCK PLEASE WAIT\n\n\n");
        }
    }
    fclose(gpsFile);
    lcd.cls();
    lcd.printf("Turn off and on again to restart");
}
}

}


