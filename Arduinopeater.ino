// Arduinopeater - simple simplex repeater
// Authors: Ben Kuo, KK6FUT  04/2015

#define NOTE_F5 698

// variables for triggers
int squelchLevel = 300; // read this from A1 for adjustment
unsigned long HOLD_LENGTH = 1500;
int holdtimer;
//const unsigned long GLOBALTIMEOUT = 1000*60*3; // 3 minutes
const unsigned long GLOBALTIMEOUT = 1000*10; // 10 seconds

int recording = 0;
unsigned long recordtime = 0;
unsigned long playtime = 0;

// variables for identifier
const unsigned long IDTIMEOUT = 600000; // id every 10 minutes
unsigned long idtimer;
char identifier[] = "KK6FUT/R"; // automated identification required every 10 min.

// indicator LEDs
const int LED = 13; // receive LED
const int TXLED = 12; // send LED
const int PTTRELAY = 11; // relay trigger

// courtesy TONE and call indicator
const int tonepin = 10; // tone on pin 10 - goes to MIC directly
const int tonehz = 440; // a440
const unsigned long tonedelay = 100; // 100ms

//ISD17xx pinouts
const int RECORD = 2; // hold low to record
const int PLAY = 3; // hold low to play


// Misc. stuff for morse code ID
long wpm=20; // If you are feeling your oats make this 20 -- if you failed to eat your Wheaties makee it 8
long z = 1200/wpm; // convert wpm to milliseconds
long dashdelay = 3*z; // length of a dash
long dotdelay = z; // length of a dot
long dddelay = z; // length of pauses between dots and dashes
long chardelay = 3*z; // length of spacing between charactersf


// the setup routine runs once when you press reset:  
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);  // sets the digital pin as output
  pinMode(PTTRELAY, OUTPUT);
  digitalWrite(PTTRELAY, LOW);  // do not key PTT
  pinMode(tonepin, OUTPUT);
  pinMode(PLAY, OUTPUT);
  digitalWrite(PLAY, HIGH);
  pinMode(RECORD, OUTPUT);
  digitalWrite(RECORD, HIGH);
  holdtimer = 0;
  recording = 0;
  idtimer = 0;
  Serial.println("Reset.");
}

void courtesy_beep() {
  tone(tonepin,tonehz);
  delay(tonedelay);
  noTone(tonepin);
}

void station_id() {
  // for further expansion, punch out station identifier automatically every 10 minutes
  sendmessage(identifier);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A6);
  int squelchLevel = analogRead(A1);
  // print out the value you read:
  //Serial.println(squelchLevel);
  holdtimer++;
  if (sensorValue > squelchLevel) {
      holdtimer = 0; // clear hold if we detect any signal at all
      if (recording == 0) {
          digitalWrite(LED, HIGH); 
          digitalWrite(RECORD, LOW);  // hold low to record
          Serial.println("Start record.");
          recording = 1;
          recordtime = 0;
      }
      recordtime++;
  } else {
      if (recording == 1) { // we were recording
       recordtime++;
       if (holdtimer > HOLD_LENGTH) { // no squelch broken for HOLD_LENGTH, so stop recording
              digitalWrite(LED, LOW);
              digitalWrite(RECORD, HIGH);  // stop recording, level triggered
              Serial.println("Stop recording.");
              delay(1000);
              Serial.println(recordtime);
              recording = 0;
              holdtimer = 0;
              //digitalWrite(TXLED, HIGH);  // visual indicator that we are playing

              // go ahead and key PTT, and play back now that we've finished recording
              Serial.println("Start playback.");
              digitalWrite(PTTRELAY, HIGH);  // key PTT
              delay(10); // wait for PTT to trigger
              playtime = 0;

              digitalWrite(PLAY, LOW);  // start playback
              delay(1000);
             // play until we match recording time
              while (playtime<recordtime) { 
                   // busystatus = digitalRead(RDY); 
                    delay(1); // wait
                    playtime++;
              }
              digitalWrite(PLAY, HIGH);  // stop playback
       
              Serial.println("Close transmission.");
              Serial.println(playtime);
              delay(20);
              courtesy_beep(); // play courtesy beep
             // digitalWrite(TXLED, LOW);  // visual indicator that we are done
              if (idtimer > IDTIMEOUT) {
                Serial.println("Auto id.");
                Serial.println(idtimer);
                Serial.println(IDTIMEOUT);
                idtimer = 0;
                station_id(); // send out station identifier in morse code
              }
              delay(20); // hold for just a bit
              digitalWrite(PTTRELAY, LOW);  // unkey PTT
      } // if hold        
     } // if recording  
  } // else squelch
   
  delay(1);        // delay in between reads for stability
  idtimer++; // accumulate identifier timer

}



// morse code ID stuff from CW SENDER - not the most efficient, but it works

void sendmessage(char* message) {
  int i=0;
  while (message[i]>0) {
       if (message[i] == ' ') {
          delay(chardelay); // pause for space between words
       } else {
          sendchar(message[i]); // send the actual character itself
       }
       i++;
 } 
}

void senddash() {
  digitalWrite(LED, HIGH); 
  tone(tonepin,NOTE_F5);
  delay(dashdelay);
  noTone(tonepin);
  digitalWrite(LED, LOW);
  delay(dddelay);
}

void senddot() {
  digitalWrite(LED, HIGH);
  tone(tonepin,NOTE_F5);
  delay(dotdelay);
  noTone(tonepin);
  digitalWrite(LED, LOW);
  delay(dddelay);
}

void  sendchar(char letter) {
  if (letter == 'A') {
    senddot();
    senddash(); 
  } 
  else if (letter == 'B') {
    senddash(); // B
    senddot();
    senddot();
    senddot();
  } 
  else if (letter == 'C') {
    senddash(); // C
    senddot();
    senddash();
    senddot();
  } 
  else if (letter == 'D') {
    senddash();   //D
    senddot();
    senddot();
  } 
  else if (letter == 'E') {
    senddot();   //E
  } 
  else if (letter == 'F') {
    senddot();   
    senddot();   
    senddash();   
    senddot();  
  } 
  else if (letter == 'G') {
    senddash();  //G
    senddash();
    senddot();
  } 
  else if (letter == 'H') {
    senddot();  //H
    senddot();
    senddot();
    senddot(); 
  } 
  else if (letter == 'I') {
    senddot();    //I
    senddot();
  } 
  else if (letter == 'J') {
    senddot();    //J
    senddash();
    senddash();
    senddash();   
  } 
  else if (letter == 'K') {
    senddash();    //K
    senddot();
    senddash();   
  } 
  else if (letter == 'L') {
    senddot();   //L
    senddash();    
    senddot();   
    senddot(); 
  } 
  else if (letter =='M') {
    senddash();    //M
    senddash();
  } 
  else if (letter =='N') {
    senddash();    //N
    senddot();
  } 
  else if (letter == 'O') {
    senddash();  //O
    senddash();  
    senddash();  
  } 
  else if (letter == 'P') {
    senddot();    //P
    senddash();
    senddash();
    senddot();  
  } 
  else if (letter == 'Q') { 
    senddash(); //Q
    senddash();
    senddot();
    senddash();
  } 
  else if (letter == 'R') {
    senddot();  //R
    senddash();  
    senddot();  
  } 
  else if (letter == 'S') {
    senddot();
    senddot();
    senddot();
  } 
  else if (letter == 'T') {
    senddash();  //T
  } 
  else if (letter == 'U') {
    senddot();   //U
    senddot();
    senddash();
  } 
  else if (letter == 'V') {
    senddot();    //V
    senddot();
    senddot();
    senddash();
  } 
  else if (letter == 'W') {
    senddot();  //W
    senddash();
    senddash();
  } 
  else if (letter == 'X') {
    senddash();
    senddot();  
    senddot();  
    senddash();
  } 
  else if (letter == 'Y') {
    senddash();    //Y
    senddot();
    senddash();
    senddash();
  } 
  else if (letter == 'Z') {
    senddash();   // Z
    senddash();
    senddot();
    senddot();
  } 
  else if (letter == '1') {
    senddot();    //1
    senddash();
    senddash();
    senddash();
    senddash();
  }
  else if (letter == '2') {
    senddot();    //2
    senddot();
    senddash();
    senddash();
    senddash();
  } 
  else if (letter == '3') {
    senddot();     //3
    senddot();
    senddot();
    senddash();
    senddash();
  } 
   else if (letter == '4') {
    senddot();     //4
    senddot();
    senddot();
    senddot();
    senddash();
  } 
  else if (letter == '5') {
    senddot();
    senddot();
    senddot();
    senddot();
    senddot();  
  } 
  else if (letter == '6') {
    senddash();    //6
    senddot();
    senddot();
    senddot();
    senddot();
  } 
  else if (letter == '7') {
    senddash();    //7
    senddash();
    senddot();
    senddot();
    senddot();
  } 
  else if (letter == '8') {
    senddash();    //8
    senddash();
    senddash();
    senddot();
    senddot();
  } 
  else if (letter == '9') {
    senddash();  //9
    senddash();
    senddash();
    senddash();
    senddot();
  } 
  else if (letter == '0') {
    senddash();    //
    senddash();
    senddash();
    senddash();
    senddash();
  } 
  else if (letter == '?') {
    senddot();   //?
    senddot();
    senddash();
    senddash();
    senddot();
    senddot();
  } 
  else if (letter == '/') {
    senddash();  ///
    senddot();
    senddot();
    senddash();
    senddot();
  } 
  else if (letter == ',') {
    senddash();    //,
    senddash();
    senddot();
    senddot();
    senddash();
    senddash();
  } 
  else if (letter == '-') {
    senddash();    // -
    senddot();
    senddot();
    senddot();
    senddash();
  }
  
  else if (letter == ']') {
    senddot();    // Error in typing
    senddot();
    senddot();
    senddot();
    senddot();
    senddot();
    senddot();
    senddot();
  }
  else if (letter == '.') {
    senddot();    // .
    senddash();
    senddot();
    senddash();
    senddot();
    senddash();
  }



    delay(chardelay);
}

