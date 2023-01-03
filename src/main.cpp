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

// Pipe dimensions for flow meter
#define PIPE_INTERNAL_DIAMETER 0.0508      // 2" = 0.0508m
#define PIPE_INTERNAL_RADIUS 0.0254        // 2" = 0.0508m
#define PIPE_CROSS_SECTION_AREA 2.02683e-3 // πr2
#define m_CUBED_PER_SEC_TO_m_CUBED_PER_HOUR 3600

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
float flowrate = 0.0;
int time[6] = {0, 0, 0, 0, 0, 0};
BlynkTimer timer;
/***************************END GLOBAL VARIABLES****************************/

// This function sends Arduino's up time every second to Virtual Pin (0).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

void getControllerUptime()
{
  const int day = Controllino_GetDay();
  const int mth = Controllino_GetMonth();
  const int yrs = Controllino_GetYear();
  const int hrs = Controllino_GetHour();
  const int min = Controllino_GetMinute();
  const int sec = Controllino_GetSecond();

  const long unsigned int startSecs = time[3] * 3600 + time[4] * 60 + time[5]; 
  const long unsigned int endSecs = hrs * 3600 + min * 60 + sec; 
  long unsigned int diff = endSecs-startSecs;

  int systemUpTimeMn = int((diff / (60)) % 60);
  int systemUpTimeHr = int((diff / (60 * 60)) % 24);
  int systemUpTimeDy = int((diff / (60 * 60 * 24)) % 365);

  Serial.print(systemUpTimeDy);
  Serial.print(",");
  Serial.print(systemUpTimeHr);
  Serial.print(",");
  Serial.println(systemUpTimeMn);
}

void myTimerEvent()
{

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(BLYNK_DATE_SYSTEM_START, onlineSinceDate);
  Blynk.virtualWrite(BLYNK_BLEACH_FLOWRATE, flowrate);
}

BLYNK_WRITE(BLYNK_FLOWRATE_SETPOINT) // this command is listening when something is written to V1
{
  int pinValue = param.asFloat(); // assigning incoming value from pin V1 to a variable

  Serial.print("V1 button value is: "); // printing value to serial monitor
  Serial.println(pinValue);
  const int adcVal = map(pinValue,0,100,51,252);
  analogWrite(CONTROLLINO_AO0, adcVal);
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
  time[0] = Controllino_GetDay();
  time[1] = Controllino_GetMonth();
  time[2] = Controllino_GetYear();
  time[3] = Controllino_GetHour();
  time[4] = Controllino_GetMinute();
  time[5] = Controllino_GetSecond();
  sprintf(onlineSinceDate, "%02i-%02i-20%i • %02i: %02i: %02i",
          time[0], time[1], time[2], time[3], time[4], time[5]

  );

  Serial.begin(CONTROLLINO_BAUD_RATE);
  Serial.println(onlineSinceDate);

  // Physical pin and relay configuration
  pinMode(CONTROLLINO_D0, OUTPUT);
  pinMode(CONTROLLINO_AO1, OUTPUT); // 4-20mA out to control valve for air supply
  pinMode(CONTROLLINO_AI12, INPUT); // 0-10V input from INOR transmitter. Used to convert 4-20mA from flowmeter to 0-10V input to flow meter

  //
  analogWrite(CONTROLLINO_AO0, 51); //set analog output to 4ma

  //TODO: Then after blynk boots up, synchronize the value of the app to the Controllino
   Blynk.begin(auth);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  Blynk.logEvent("low_flow_-_10_minutes");
  /*Blynk.logEvent("event_code", String("High TemperatureDetected! Tº: ") + temp);*/
}

void loop()
{

  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  float sensorValue = analogRead(CONTROLLINO_AI12);
  Serial.print(sensorValue);
  sensorValue = sensorValue * (5.0 / 1023.0);
  Serial.print(",");
  Serial.print(sensorValue);
  flowrate = (sensorValue * PIPE_CROSS_SECTION_AREA) * m_CUBED_PER_SEC_TO_m_CUBED_PER_HOUR;
  Serial.print(",");
  Serial.println(flowrate);
  //getControllerUptime();
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
 int analogOut0 = 127;                     // 0 - 255 to be set (0 - 10 000 mV, or 0 - 20 000 uA)
 int analogOut1 = 255;                     // 0 - 255 to be set (0 - 10 000 mV, or 0 - 20 000 uA)
 analogWrite(CONTROLLINO_AO0, analogOut0); // set the analog output 0 to 5V or 10mA
 analogWrite(CONTROLLINO_AO1, analogOut1); // set the analog output 1 to 10V or 20mA
} */