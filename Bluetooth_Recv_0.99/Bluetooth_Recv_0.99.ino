#define chNum 4
#define ON LOW
#define OFF HIGH
#define SEC 1000
String incomingMsg; // for incoming serial data
unsigned int outputs[chNum] = {9,10,11,12};
bool updateStates = false;
bool timerOn[chNum] = {false};
//bool sendStates = false;
bool states[chNum];
bool nextStates[chNum];
unsigned long timeDelay = 0;
unsigned long startTime;
unsigned long endTime[chNum];
unsigned long prevTime[chNum];
char sendCh = 's';
char timerCh = 't';

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.  
  for (int i=0; i<chNum; i++){
      states[i] = nextStates[i] = OFF;
      pinMode(outputs[i], OUTPUT);
      digitalWrite(outputs[i], states[i]);
  }
  Serial.begin(115200);
  Serial.setTimeout(100);
}

void readSerial(){
  // read the incoming byte:
      startTime = millis();
      incomingMsg = Serial.readString();
      if (incomingMsg[0] = 's'){
          int channel = incomingMsg[1] - '1';
          char newState = (incomingMsg[2] == 't') ? ON : OFF;      
      // without timer
          if( incomingMsg[2] == '\n'){
            states[channel] = newState;
            timerOn[channel] = false;
            updateStates = true;                                  
          }
      //trun on timer
          else{ 
            incomingMsg.remove(0, 3);
            timeDelay = incomingMsg.toInt();                       
            endTime[channel] = startTime + timeDelay*SEC;
            prevTime[channel] = startTime;     
            nextStates[channel] = newState; 
            timerOn[channel] = true;                
          }    
      }   
      //Serial.print("I received: ");
      //Serial.println(incomingMsg);
}

void checkTimers(){
  unsigned long curTime = millis();
  for (int i=0; i<chNum; i++){    
      if (timerOn[i]){
        if (curTime > endTime[i]){
          states[i] = nextStates[i];    
          timerOn[i] = false;
          updateStates = true;
        } 
        if (curTime - prevTime[i] >= SEC){
          Serial.print(timerCh);
          Serial.print(i+1);
          Serial.println(long(endTime[i] - curTime)/SEC);
          prevTime[i] += SEC;
        }
      }
  }
}

void sendStates(){
    Serial.print(sendCh);
    for (int i=0; i<chNum; i++)  
      Serial.print((states[i] == ON) ? 't' : 'f');
    Serial.println();
}

// the loop function runs over and over again forever
void loop() { 
  if (Serial.available() > 0) {
      readSerial();
  }
  // check if delay is on
  if (timerOn){
      checkTimers();     
  } 
  // update states of relays and send current states
  if (updateStates){ 
    for (int i=0; i<chNum; i++)   
      digitalWrite(outputs[i], states[i]);
    updateStates = false;
    sendStates();    
  }

}
