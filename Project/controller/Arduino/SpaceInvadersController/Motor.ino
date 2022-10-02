void setupMotor() {
  ledcSetup(pwmChannel, pwmFrequency, pwmBitResolution);
  ledcAttachPin(MOTOR_PIN, pwmChannel);
}

void activateMotor(int motorPower) {
  ledcWrite(pwmChannel, motorPower);
}

void deactivateMotor() {
  ledcWrite(pwmChannel, 0);
}
