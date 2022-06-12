
const byte ledPin = 17;
const byte interruptPin = 16;
volatile byte state = LOW;

//Number of pulses, used to measure energy.
unsigned long pulseCount = 0;
unsigned long lastPulseCount = 0;

//Used to measure power.
unsigned long pulseTime,lastTime;

//power and energy
double power, elapsedkWh;

const unsigned long intervalInSeconds = 10;


void setup()
{
    // TODO: ophalen elapsed kWh uit NVRAM
  
    // wifi verbinding maken
    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), pulse_interrupt, FALLING);
}

void loop()
{
    digitalWrite(ledPin, state);

    // Todo echte wachttijd berekenen...
    delay(1000 * intervalInSeconds);

    noInterrupts();
    unsigned long p = pulseCount; 
    interrupts();

    // bereken aantal pulsen in deze wacht tijd
    // omrekenen naar Wattage    
    unsigned long deltaPulseCount = p - lastPulseCount;
    lastPulseCount = p;
    double watts = 0.5 * deltaPulseCount * intervalInSeconds;
    Serial.print("Het huidige verbruik is: ");
    Serial.print(watts,4);
    Serial.print(" watt.");
    
    // totaal kWh berekenen
    elapsedkWh = (1.0*p/2000); 
    Serial.print(" De opbrengst is: ");
    Serial.print(elapsedkWh,3);
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
