#define BLYNK_TEMPLATE_ID "TMPLtruvy1yH" // According to Blynk's documentation this has to be the first line in the code https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made


#define SET_RTC 0
/* #define BLYNK_TEMPLATE_ID "TMPLtruvy1yH" */
#define BLYNK_DEVICE_NAME "Flowrate Monitoring and Control"
#define BLYNK_AUTH_TOKEN "qMvcc9VEGKnMT8Exsy2s_6I8SRSs0jPN"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <Arduino.h>
#include <Controllino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

/***************************BEGIN GLOBAL VARIABLES****************************/
char timeStartUp[20];
char auth[] = BLYNK_AUTH_TOKEN;
/***************************END GLOBAL VARIABLES****************************/

BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (0).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, millis() / 1000);
}

int i = 0;
void setup()
{
  // put your setup code here, to run once:
  // Initialize RTC
  Serial.begin(921600);

  Controllino_RTC_init();
  delay(10); // give time for RTC to initialzie

// Only run if date/time needs to be set.
// Otherwise on every reset the rtc will take the time of the last time a script was uploaded
#if SET_RTC
  Controllino_SetTimeDateStrings(__DATE__, __TIME__); /* set compilation time to the RTC chip */
#endif

  // Get controller start up time
  sprintf(timeStartUp, "%i,%i,%i,%i,%i,%i", // Day,Month,Year,Hour,Minute,Second
          Controllino_GetDay(),
          Controllino_GetMonth(),
          Controllino_GetYear(),
          Controllino_GetHour(),
          Controllino_GetMinute(),
          Controllino_GetSecond());

  Serial.println(timeStartUp);
  pinMode(CONTROLLINO_D0, OUTPUT);
  Blynk.begin(auth);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  i = i + 1;

  /*   // put your main code here, to run repeatedly:
    digitalWrite(CONTROLLINO_D0, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000);                        // wait for 100 milliseconds which is 1/10 of a second
    digitalWrite(CONTROLLINO_D0, LOW);  // turn the LED off by making the voltage LOW
    delay(1000); */
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}