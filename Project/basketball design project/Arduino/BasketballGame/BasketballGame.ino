const int buttonPin = 17; const int redLED = 21; 
const int yellowLED = 16; const int blueLED = 19;
const int motor_pin = 18;

int buttonState = LOW;
int lastButtonState = LOW;

int ppg = 0;        // PPG from readPhotoSensor() (in Photodetector tab)
int sampleTime = 0; // Time of last sample (in Sampling tab)
bool game_on = false;
bool lights = false;
bool sending;
int anything = 0;

int now_time = 0; int previous_time = 0;

int indexer = -1;
int indexer_end = -1;
String buzz_me = "";
String end_me = "";

void setup() {
  setupCommunication();
  setupDisplay();
  setupPhotoSensor();
  sending = false;
  writeDisplay("LET'S BALL", 0, true);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(motor_pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) { //button presses send fire commands
    if (buttonState == HIGH) {
      game_on = true;
    }
    delay(50);
    lastButtonState = buttonState;
  }

  String command = receiveMessage();
  buzz_me = command.c_str();
  indexer = buzz_me.indexOf("POINT");
  end_me = command.c_str();
  indexer_end = end_me.indexOf("GAME OVER!!");
  now_time = millis();
  
  if (command == "stop") {
    sending = false;
    writeDisplay("Game off", 0, true);
  }

  else if (command == "start") {
    sending = true; 
    writeDisplay("Game on", 0, true);
  }

  else if (command != "" && indexer == -1) {
    writeDisplayCSV(command.c_str(), 3);
  }

  if (indexer != -1) { //maybe potential issue w timing for the stuff not turning on (think space invaders type error)
//    anything++;
    previous_time = millis();
    digitalWrite(blueLED, HIGH);
    digitalWrite(yellowLED, HIGH);
    // blue and yellow bc steph curry w the shot boi (dubs in 6)
    digitalWrite(motor_pin, HIGH);
    lights = true;
  }

  if (now_time - previous_time >= 2000 && lights) {
//    anything++;
    digitalWrite(blueLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(motor_pin, LOW);
    lights = false;
    previous_time = now_time;
  }

  if (indexer_end != -1) {
    digitalWrite(redLED, HIGH);
    digitalWrite(motor_pin, HIGH);
    delay(4000);
    digitalWrite(redLED, LOW);
    digitalWrite(motor_pin, LOW);
  }
  
  if (sending && sampleSensors()) { //&& game_on
    String response = String(sampleTime) + ",";
    response += String(ppg);
    response += "," + String(anything);
    sendMessage(response);
  }
}
