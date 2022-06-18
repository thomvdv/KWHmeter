#include "WiFi.h"
const byte ledPin = 17;
const byte interruptPin = 16;
const char* ssid = "Area51 IoT";
const char* password =  "Afblijven01!";
volatile byte state = LOW;

//Number of pulses, used to measure energy.
unsigned long pulseCount = 0;
unsigned long lastPulseCount = 0;

//power and energy
double watts, elapsedkWh;

const unsigned long intervalInSeconds = 10;


void setup()
{
    // TODO: ophalen elapsed kWh uit NVRAM
  
    // wifi verbinding maken
    Serial.begin(115200);
 
    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");

    pinMode(ledPin, OUTPUT);
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), pulse_interrupt, FALLING);
}

void loop()
{
    digitalWrite(ledPin, state);

    // Todo echte wachttijd berekenen...
    delay(1000 * intervalInSeconds);

    //noInterrupts();
    unsigned long p = pulseCount; 
   // interrupts();

    // bereken aantal pulsen in deze wacht tijd
    // omrekenen naar Wattage    
    unsigned long deltaPulseCount = p - lastPulseCount;
    lastPulseCount = p;
    watts = (deltaPulseCount * 0.5) * (intervalInSeconds * 36);
    Serial.print("Het huidige verbruik is: ");
    Serial.print(watts,4);
    Serial.print(" watt.");
    
    // totaal kWh berekenen
    elapsedkWh = (1.0*p/(2 * 1000)); 
    Serial.print(" De opbrengst is: ");
    Serial.print(elapsedkWh,4);
    Serial.println(" kWh.");
    

    // opsturen naar MQTT broker (->  192.168.2.217)

    // todo: check of wifi connectie er nog is, anders opnieuw opzetten

    // todo: opslaan van elapsed kWh in NVRAM
}

// The interrupt routine
void pulse_interrupt()
{
    pulseCount++;
}
