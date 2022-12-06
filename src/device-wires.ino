int x;

// outputs
int opin0 = D0;
int opin1 = D1;
int opin2 = D2;
int opin3 = D3;
int opin4 = D4;

// inputs
int ipin0 = A0;
int ipin1 = A1;
int ipin2 = A2;
int ipin3 = A3;
int ipin4 = A4;

int buzzerpin = A5;

// leds
int led1 = D7;

// settings
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_ALL);

// cloud functions
int setPuzzlerMode(String mode);

// cloud variables
String serializedState;

// timers (max of 10)
Timer scanTimer(100, scan);

void setup()
{
  // outputs
  pinMode(opin0, OUTPUT);
  pinMode(opin1, OUTPUT);
  pinMode(opin2, OUTPUT);
  pinMode(opin3, OUTPUT);
  pinMode(opin4, OUTPUT);

  // inputs
  pinMode(ipin0, INPUT_PULLDOWN);
  pinMode(ipin1, INPUT_PULLDOWN);
  pinMode(ipin2, INPUT_PULLDOWN);
  pinMode(ipin3, INPUT_PULLDOWN);
  pinMode(ipin4, INPUT_PULLDOWN);

  // leds
  pinMode(led1, OUTPUT);

  // set up cloud functions
  Particle.function("setMode", setPuzzlerMode);
  Particle.variable("machineState", serializedState);

  // start timers
  scanTimer.start();
}

const int MODE_SCANNING = 0;
const int MODE_RESOLVED = 1;
const int MODE_ERROR = 2;

bool scanningHasStarted = false;

int CURRENT_MODE = 0;

int setPuzzlerMode(String mode)
{
  if (mode == "scanning")
  {
    CURRENT_MODE = MODE_SCANNING;
    if (!scanTimer.isActive())
    {
      scanTimer.start();
    }
    return 1;
  }
  else if (mode == "resolved")
  {
    CURRENT_MODE = MODE_RESOLVED;
    scanTimer.stop();
    return 1;
  }
  else if (mode == "error")
  {
    CURRENT_MODE = MODE_ERROR;
    scanTimer.stop();
    return 1;
  }
  else
  {
    CURRENT_MODE = MODE_ERROR;
    scanTimer.stop();
    return -1;
  }
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
  delay(5000);
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

int ipins[5] = {ipin0, ipin1, ipin2, ipin3, ipin4};
int state[10] = {
    opin0,
    -1,
    opin1,
    -1,
    opin2,
    -1,
    opin3,
    -1,
    opin4,
    -1,
};

// automatically store to global variable which gets
// published to cloud, also automatically
void serializeState()
{
  String serialized = String("");

  for (int oi = 0; oi < 10; oi += 2)
  {
    serialized.concat(String(oi / 2));
    serialized.concat(String(state[oi + 1]));
  }

  serializedState = serialized;
}

void scan()
{
  Log.info("--- scan start");

  scanningHasStarted = true;

  for (int oi = 0; oi < 10; oi += 2)
  {
    digitalWrite(state[oi], HIGH);

    // reset previous values
    state[oi + 1] = -1;

    // loop all input pins to chech which output-input-pairs
    // are wired together
    for (int ii = 0; ii < 5; ii++)
    {
      if (digitalRead(ipins[ii]) > 0)
      {
        state[oi + 1] = ii;
      }
    }

    digitalWrite(state[oi], LOW);
    // Log.info("opin: %d, ipin: %d", oi/2, state[oi+1]);
  }

  serializeState();
  // for (int oi = 0; oi < 10; oi += 2) {
  //     Log.info("opin: %d, ipin: %d", oi/2, state[oi+1]);
  // }
}

int notes[] = {239, 239, 213};

void loop()
{
  switch (CURRENT_MODE)
  {
  case MODE_SCANNING:
    // if (scanningHasStarted) {
    //     for (int i = 0; i < sizeof(notes); i++) {
    //         tone(buzzerpin, notes[i], 100);
    //         delay(200);
    //     }
    //     scanningHasStarted = false;
    // }

    slowBlink();
    break;
  case MODE_RESOLVED:
    pulseBlink();
    break;
  case MODE_ERROR:
    fastBlink();
    break;
  default:
    fastBlink();
  }
}
