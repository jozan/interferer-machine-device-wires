#include "State/State.h"
#include "HttpClient/HttpClient.h"
#define LOGGING

IPAddress address = IPAddress(192, 168, 2, 55);
int port = 8080;
std::string apiEndpoint = "/device/device-wires";

HttpClient *client = new HttpClient(address, port);
State *state = new State();

// pins
int outputPins[] = {D0, D1, D2, D3, D4};
int inputPins[] = {A0, A1, A2, A3, A4};

// leds
int led1 = D7;

// settings
SYSTEM_MODE(SEMI_AUTOMATIC); // Disable cloud connection
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_ALL);

// TODO: should be in State class
std::string serializedState;

// timers (max of 10)
Timer scanTimer(100, scan);

void setup()
{
  System.disable(SYSTEM_FLAG_RESET_NETWORK_ON_CLOUD_ERRORS);

  WiFi.connect();

  waitFor(Serial.isConnected, 30000);

  Serial.println("WiFi: setting up...");
  waitUntil(WiFi.ready);

  if (!WiFi.ready())
  {
    Serial.println("WiFi: not ready. No connection. Aborting.");
    return;
  }

  // Set which pins are outputting
  for (auto pinOut : outputPins)
  {
    pinMode(pinOut, OUTPUT);
  }

  // Set which pins are reading
  for (auto pinIn : inputPins)
  {
    pinMode(pinIn, INPUT_PULLDOWN);
  }

  // leds
  pinMode(led1, OUTPUT);

  // start timers
  scanTimer.start();
}

void fastBlink()
{
  digitalWrite(led1, HIGH);
  delay(250);
  digitalWrite(led1, LOW);
  delay(250);
}

void slowBlink()
{
  digitalWrite(led1, HIGH);
  delay(2000);
  digitalWrite(led1, LOW);
  delay(100);
}

void pulseBlink()
{
  digitalWrite(led1, HIGH);
  delay(250);
  digitalWrite(led1, LOW);
  delay(1000);
}

int wireConfiguration[10] = {
    outputPins[0],
    -1,
    outputPins[1],
    -1,
    outputPins[2],
    -1,
    outputPins[3],
    -1,
    outputPins[4],
    -1,
};

void scan()
{
#ifdef LOGGING
  Serial.printlnf("free memory %lu", System.freeMemory());
  Log.info("--- scan start");
#endif

  for (int oi = 0; oi < 10; oi += 2)
  {
    digitalWriteFast(wireConfiguration[oi], HIGH);

    // reset previous values
    wireConfiguration[oi + 1] = -1;

    // loop all input pins to chech which output-input-pairs
    // are wired together
    for (int ii = 0; ii < 5; ii++)
    {
      if (digitalRead(inputPins[ii]) > 0)
      {
        wireConfiguration[oi + 1] = ii;
      }
    }

    digitalWriteFast(wireConfiguration[oi], LOW);
  }

  if (!WiFi.ready())
  {
    Serial.println("WiFi: not ready. No connection. Aborting.");
    delay(200);
    return;
  }

  serializedState = state->serializeState(wireConfiguration);
  delay(200);
}

void loop()
{
  if (!WiFi.ready())
  {
    fastBlink();
    return;
  }

  if (!client->isSending())
  {

    // TODO: compare with previous state, not only length
    if (serializedState.length() == 0)
    {
      slowBlink();
      return;
    }

    if (client->post(apiEndpoint, serializedState))
    {
      Serial.println("POST: success");
    }
    else
    {
      Serial.println("POST: failed");
    }
  }

  pulseBlink();
}
