/*
 * Global variables
 */
// Acceleration values recorded from the readAccelSensor() function
int ax = 0; int ay = 0; int az = 0; float rollF = 0; float pitchF = 0; float roll = 0; float pitch = 0;
int ppg = 0;        // PPG from readPhotoSensor() (in Photodetector tab)
int sampleTime = 0; // Time of last sample (in Sampling tab)
bool sending;

const int orientationPin = 16;
int buttonState2 = LOW;
int lastButtonState2 = LOW;
int oneHanded = 0;

const int firePin = 17;
int buttonState = LOW;
int lastButtonState = LOW;
int fired = 0;
//
const int pwmFrequency = 5000;  // Set the PWM frequency to 5KHz
const int pwmChannel = 0;       // Use PWM channel 0
const int pwmBitResolution = 8; // Set a PWM resolution of 8 bits
const int MOTOR_PIN = 18;
int now_time, previous_time = 0;
int motor_state = 0;

int lives_left = 3;
int lives_left_counter = 3;
String manipulated = "";
int indexer = 0;
String fake = "";
int checker = 0;
String empty = "";

//int direction_of_travel;

/*
 * Initialize the various components of the wearable
 */
void setup() {
  setupAccelSensor();
  setupCommunication();
  setupDisplay();
//  setupMotor();
  sending = false;
  pinMode(21, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  writeDisplay("Ready...", 1, true);
  writeDisplay("Set...", 2, false);
  writeDisplay("Play!", 3, false);
}

/*
 * The main processing loop
 */
void loop() {
  // Parse command coming from Python (either "stop" or "start")
  buttonState = digitalRead(firePin);
  buttonState2 = digitalRead(orientationPin);

  if (buttonState != lastButtonState) { //button presses send fire commands
    if (buttonState == HIGH) {
      fired = 1;
    }
    delay(50);
    lastButtonState = buttonState;
  }

  String command = receiveMessage();
  
  if(command == "stop") {
    sending = false;
    writeDisplay("Controller: Off", 0, true);
  }
  
  else if(command == "start") {
    sending = true;
    writeDisplay("Controller: On", 0, true);
  }
  
  else if(command != "") {
    manipulated = command.c_str();
    indexer = manipulated.indexOf("Lives:"); //look for life count
    if (indexer != -1) { //if there is a life count, then do the following:
      now_time = millis();
      fake = manipulated[indexer + 6]; //find the number of lives 
      if (fake == "0") {
        lives_left = 0;
      }
      else if (fake == "1") {
        lives_left = 1;
      }
      else if (fake == "2") {
        lives_left = 2;
      }
      else if (fake == "3") {
        lives_left = 3;
      }
    }
    if (lives_left < lives_left_counter) { //if the lives dips below the life counter then buzz motor and light LED
      checker++;
      previous_time = millis();
      digitalWrite(21, HIGH);
      digitalWrite(MOTOR_PIN, HIGH);
      motor_state = 1;
      lives_left_counter--;
    }
    if (now_time - previous_time >= 2000 && motor_state == 1) { //turn off led and motor if the motor was on and 2s have passed
      digitalWrite(21, LOW);
      digitalWrite(MOTOR_PIN, LOW);
      checker++;
      motor_state = 0;
      previous_time = now_time;
    }
    writeDisplayCSV(command.c_str(), 3); //write everything to display
  }
  
  // Send the orientation of the board
  if(sending && sampleSensors()) { //sending!!
    roll = roll_function(ax, ay, az);
    rollF = 0.94 * rollF + 0.06 * roll;
    pitch = pitch_function(ax, ay, az);
    pitchF = 0.94 * pitchF + 0.06 * pitch;  
    // above formula found online @ 
    // https://howtomechatronics.com/tutorials/arduino/how-to-track-orientation-with-arduino-and-adxl345-accelerometer/
    int direction_of_travel = the_direction(pitchF);
    String response = String(direction_of_travel) + ","; 
    response += String(fired);
    response +=  "," + String(oneHanded);
//    response += "," + String(pitchF);
    //+ ",";
    
    sendMessage(response);
    
    if (fired == 1) { // reset fire state at end of loop to make sure fire command isn't continuously sent
      fired = 0;
    }
  }
}
