#include <Wire.h>                // i2c library for arduino.           

// MACRO Definitions
#define BP_START_PIN (2)         // start button of the blood pressure monitor device. replaced a transistor.   
#define VALVE_PIN (5)            // checks if the measurement is done.             
#define MEASURE_BEGIN_PIN (4)    // indicates that a measurement should start. this can be connected to switch or another MCU or raspberry pi.
#define PUMP_PIN (6)             // checks if the pump starts putting pressure on the calf.

volatile byte i2c_data_rx;       // indicates there are available data from the i2c bus.
volatile uint16_t count;         // indicates the total number of data collected.
volatile uint8_t sys, dia, hr;   // stored the measure values: systolic, diastolic and heart rate.
uint8_t bp_measure_done = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(BP_START_PIN, OUTPUT);
  pinMode(VALVE_PIN, INPUT);
  pinMode(MEASURE_BEGIN_PIN, INPUT_PULLUP);
  pinMode(PUMP_PIN, INPUT);
  Wire.begin(0x50);                           // the address of the EEPROM is 0x50. The arduino should be the same.
  Wire.onReceive(receiveEvent);               // this is the interrupt initialization for the i2c data.
}

void loop()
{
  if (digitalRead(MEASURE_BEGIN_PIN) == 0)        // The arduino is instructed to start the measurement.
  {
    digitalWrite(BP_START_PIN, HIGH);             // Emulating a push on the button.
    delay(200);
    digitalWrite(BP_START_PIN, LOW);

    delay(1000);
    Serial.println("Attemp to start pump...");
    delay(2000);

    while (digitalRead(PUMP_PIN) == 1)
    {
      digitalWrite(BP_START_PIN, HIGH);             // Emulating a push on the button.
      delay(200);
      digitalWrite(BP_START_PIN, LOW);

      delay(1000);
      Serial.println("Attemp to start pump...");
      delay(2000);
    }

    Serial.println("Pump now started...");

    delay(5000);

    //need to secure that the value is already closed.
    while (digitalRead(VALVE_PIN) == 0)
    {
      bp_measure_done = 0;
      Serial.println("wait...");
      delay(2000);
    }

    delay(2000);


    Serial.println("Done reading...");

    digitalWrite(BP_START_PIN, HIGH);
    delay(200);
    digitalWrite(BP_START_PIN, LOW);

    delay(500);

    if (count == 0)
    {
      Serial.print("<");
      Serial.print('0');
      Serial.print(",");
      Serial.print('0');
      Serial.println(">");
    }
    else if (count == 35)
    {
      Serial.print("<");
      Serial.print(sys);
      Serial.print(",");
      Serial.print(dia);
      Serial.println(">");
    }
    else
    {
      Serial.print("<");
      Serial.print('1');
      Serial.print(",");
      Serial.print('1');
      Serial.println(">");
    }
    count = 0;
  }
}

void receiveEvent(int iData)   // Interrupt service routine.
{
  if ( iData > 0 )
  {
    while ( iData-- )
    {
      i2c_data_rx = Wire.read();
      //      Serial.println(i2c_data_rx);
      count++;

      if (count == 28)
      {
        sys = i2c_data_rx;
      }
      if (count == 29)
      {
        dia = i2c_data_rx;
      }
      if (count == 30)
      {
        hr = i2c_data_rx;
      }
    }
  }
}
