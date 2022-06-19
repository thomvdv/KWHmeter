
#include "WiFi.h"
#include <PubSubClient.h>

const char* mqtt_server = "192.168.2.217";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const byte ledPin = 17;
const byte interruptPin = 16;
const char* ssid = "XXXXXXX";
const char* password =  "XXXXXXX";
volatile byte state = LOW;

//Number of pulses, used to measure energy.
unsigned long pulseCount = 0;
unsigned long lastPulseCount = 0;

//power and energy
long actual_power;
double total_energy;

const unsigned long intervalInSeconds = 10;


// The interrupt routine
void pulse_interrupt()
{
    pulseCount++;
}

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
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
    pinMode(ledPin, OUTPUT);
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), pulse_interrupt, FALLING);

    client.setServer(mqtt_server, 1883);
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("kWhMeter")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void ha_mqtt_discovery() 
{
  char energy_conf[]= "{\"name\":\"ThomsMeter01_energy\","
                      "\"device_class\":\"energy\","
                      "\"unique_id\":\"ThomsMeter01_energy\","
                      "\"unit_of_measurement\":\"kWh\","
                      "\"value_template\":\"{{value_json.energy}}\","
                      "\"state_topic\":\"ThomsMeter01/state\"}";
  client.publish("homeassistant/sensor/ThomsMeter01/energy/config", energy_conf);
  
  char power_conf[] = "{\"name\":\"ThomsMeter01_power\","
                      "\"device_class\":\"power\","
                      "\"unique_id\":\"ThomsMeter01_power\","                      
                      "\"unit_of_measurement\":\"W\","
                      "\"value_template\":\"{{value_json.power}}\","
                      "\"state_topic\":\"ThomsMeter01/state\"}";                     
  client.publish("homeassistant/sensor/ThomsMeter01/power/config", power_conf);  
}

void publish( double energy, double power )
{
  // Construct payload like:
  // {"energy":123,"power":456}
  String payload;
  payload = "{\"energy\":" + String(energy) + ",\"power\":" + String(power) + "}";
  
  client.publish("ThomsMeter01/state", payload.c_str());
}

void loop()
{
  if (!client.connected()) {
    mqtt_reconnect();
    ha_mqtt_discovery();
  }
  client.loop();

  long now = millis();
  long deltaMillis = now - lastMsg;
  if (deltaMillis > 1000 * intervalInSeconds) {
    lastMsg = now;  

    noInterrupts();
    unsigned long p = pulseCount; 
    interrupts();

    // bereken aantal pulsen in deze wacht tijd
    // omrekenen naar Wattage    
    unsigned long deltaPulseCount = p - lastPulseCount;
    Serial.print("Delta pulsen: ");
    Serial.println(deltaPulseCount);
    Serial.print("Pulsen totaal: ");
    Serial.println(p);
    
    lastPulseCount = p;
    actual_power = (deltaPulseCount * 0.5) * ((deltaMillis/100) * 36);
    Serial.print("Het huidige verbruik is: ");
    Serial.print(actual_power);
    Serial.print(" watt.");
    
    // totaal kWh berekenen
    total_energy = (1.0*p/(2 * 1000)); 
    Serial.print(" De opbrengst is: ");
    Serial.print(total_energy,4);
    Serial.println(" kWh.");

    publish(total_energy, actual_power);
    
  }


    // todo: check of wifi connectie er nog is, anders opnieuw opzetten

    // todo: opslaan van elapsed kWh in NVRAM
}
