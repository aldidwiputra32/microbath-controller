#define TRESHOLD 2

double setPointRead = 40;
double temperature = 25.0;
extern int idTimerUploadData;
double error;
bool nearSetpoint = false;

double readSensor(){
  setPointRead = readSetpoint();

  // ganti code mu disini [start]
  temperature += 0.1;
  Serial.print("Temperature: ");
  Serial.println(temperature);
  // ganti code mu disini [finish]
  
  error = abs(setPointRead - temperature); 
  // fast interval
  if(error <= TRESHOLD){
    if(!nearSetpoint){
      nearSetpoint = true;
      timer.changeInterval(idTimerUploadData, uploadIntervalFast * 1000);
    }
  // slow interval
  }else{
    if(nearSetpoint){
      nearSetpoint = false;
      timer.changeInterval(idTimerUploadData, uploadIntervalSlow * 1000);
    }
  }
  return temperature;
}

double readSetpoint(){  
  // ganti code mu disini [start]
  Serial.print("setPointRead: ");
  Serial.println(setPointRead);
  // ganti code mu disini [finish]
  
  return setPointRead;
}