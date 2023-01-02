// According to Blynk's documentation, the template ID must be the first line in the code 
// https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made

#define BLYNK_TEMPLATE_ID "TMPLtruvy1yH" 
#define BLYNK_DEVICE_NAME "Flowrate Monitoring and Control"
#define BLYNK_AUTH_TOKEN "qMvcc9VEGKnMT8Exsy2s_6I8SRSs0jPN"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

// Blynk virtual pin declarations
#define BLYNK_DATE_SYSTEM_START V0
#define BLYNK_BLEACH_FLOWRATE V1
#define BLYNK_FLOWRATE_SETPOINT V2
#define BLYNK_SYSTEM_RUNTIME V3

// System configuration values
#define CONTROLLINO_BAUD_RATE 921600

// Set this true if the date and time of the compiled sketch should be transferred to the Controllino
#define SET_RTC 0

#include <Arduino.h>
#include <Controllino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

/***************************BEGIN GLOBAL VARIABLES****************************/
char onlineSinceDate[50];
char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;
/***************************END GLOBAL VARIABLES****************************/



// This function sends Arduino's up time every second to Virtual Pin (0).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(BLYNK_DATE_SYSTEM_START, onlineSinceDate);
}

BLYNK_WRITE(BLYNK_FLOWRATE_SETPOINT) // this command is listening when something is written to V1
{
  int pinValue = param.asFloat(); // assigning incoming value from pin V1 to a variable

  
  Serial.print("V1 button value is: "); // printing value to serial monitor
  Serial.println(pinValue);
  // map pinValue to current output

}

void setup()
{

  // put your setup code here, to run once:
  // Initialize RTC
  

  Controllino_RTC_init();
  delay(10); // give time for RTC to initialzie

  // Only run if date/time needs to be set.
  // Otherwise on every reset the rtc will take the time of the last time a script was uploaded
  #if SET_RTC
    Controllino_SetTimeDateStrings(__DATE__, __TIME__); /* set compilation time to the RTC chip */
  #endif

  // Get controller start up time
  sprintf(onlineSinceDate, "%02i-%02i-20%i • %02i: %02i: %02i",
          Controllino_GetDay(),
          Controllino_GetMonth(),
          Controllino_GetYear(),
          Controllino_GetHour(),
          Controllino_GetMinute(),
          Controllino_GetSecond()
        );

  Serial.begin(CONTROLLINO_BAUD_RATE);
  Serial.println(onlineSinceDate);

  // Physical pin and relay configuration
  pinMode(CONTROLLINO_D0, OUTPUT);
  
  Blynk.begin(auth);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  Blynk.logEvent("low_flow_-_10_minutes");
  /*Blynk.logEvent("event_code", String("High TemperatureDetected! Tº: ") + temp);*/
}

void loop()
{

/*   digitalWrite(CONTROLLINO_D0, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                        // wait for 100 milliseconds which is 1/10 of a second
  digitalWrite(CONTROLLINO_D0, LOW);  // turn the LED off by making the voltage LOW
  delay(1000); */
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

/* #include <Arduino.h>
#include <Controllino.h>

// the setup function runs once when you press reset (CONTROLLINO RST button) or connect the CONTROLLINO to the PC
void setup() {
 // initialize all used digital output pins as outputs
 pinMode(CONTROLLINO_AO0, OUTPUT);
 pinMode(CONTROLLINO_AO1, OUTPUT);                            
}

// the loop function runs over and over again forever
void loop() {
 int analogOut0 = 51;                     // 0 - 255 to be set (0 - 10 000 mV, or 0 - 20 000 uA)
 int analogOut1 = 255;                     // 0 - 255 to be set (0 - 10 000 mV, or 0 - 20 000 uA)
 analogWrite(CONTROLLINO_AO0, analogOut0); // set the analog output 0 to 5V or 10mA
 analogWrite(CONTROLLINO_AO1, analogOut1); // set the analog output 1 to 10V or 20mA                      
} */