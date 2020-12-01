#include <EEPROM.h>
#include <stdio.h>
#include <Stream.h>
#define NCHIPS 4      //max number of LARASIC to configure

#define G_SLK 0x80    // leakage control
#define G_STB1 0x40    // Monitor selector, 1=bandgap reference, 0=temperature
#define G_STB 0x20    // 1=Monitor ON on Ch0
#define G_S16 0x10    // 1= high filter on ch16 enabled 

#define C_SBF 0x80    // Output buffer 0=down and bypassed, 1=active 
#define C_SDC 0x40    // Output coupling, 0=DC, 1=AC
#define C_ST1 0x20    // 
#define C_ST0 0x10    // Peak time selector
#define C_SG1 0x08    // 
#define C_SG0 0x04    // Gain selector
#define C_SNC 0x02    // Baseline selector, 0=900mV, 1=200mV
#define C_STS 0x01    // Test capacitance enable

#define OLD_CHIP_INDEX 0 // 3 means the one next to the SDI cable, therefore last in configuration sequence

unsigned char REG_Global0[NCHIPS];
unsigned char REG_Global[NCHIPS];
unsigned char REG_Ch[NCHIPS][16];
const int ledPin =  13;      // the number of the LED pin
const int sdoPin =  10;      // the number of the SDO pin
const int clkPin =  11;      // the number of the CLK pin
const int csPin =  12;      // the number of the CLK pin
const int rstPin =  9;      // the number of the RESET pin
const int tstPin = 8; // pin for test out
const int tD = 1; //bit rate delay, ms
const int vddPin = A0; // for sensing chip vdd
const int vddpPin = A1; // for sensing chip vddp

unsigned int PulseWidth; //test pulse duration in mcs
unsigned int PulsePeriod; // test pulse period in mcs

char buf[256];
int bufptr=-1;
void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
        Serial.println("Warning! This firmware is AdHoc for LARASIC versions chain: V4-V7-V7-V7!");
        Serial.print("LAR TPC CONTROLLER initialization...");
  PulseWidth = 20; //test pulse duration in mcs
  PulsePeriod = 10000; // test pulse period in mcs

//  SetBasicConfig();
//  SaveConfig();
  RestoreConfig();
  pinMode(ledPin, OUTPUT);      
  pinMode(sdoPin, OUTPUT);      
  pinMode(clkPin, OUTPUT);      
  pinMode(csPin, OUTPUT);      
  pinMode(rstPin, OUTPUT);      
  pinMode(tstPin, OUTPUT);      
    digitalWrite(tstPin, LOW);
    digitalWrite(ledPin, LOW);
    digitalWrite(sdoPin, LOW);
    digitalWrite(clkPin, LOW);
    digitalWrite(csPin, LOW);
//    digitalWrite(csPin, HIGH);
    digitalWrite(rstPin, HIGH);
  SendConfig();
  
      Serial.println("done.");
  PrintConfig();

}

char Command=0;
char Register[16]={0};
unsigned int Bit=LOW;
int res=0;
void(* resetFunc) (void) = 0; //declare reset function @ address 0


void loop()
{
  while (Serial.available() > 0 && bufptr<255 && buf[bufptr] != 0xD ) {bufptr++; buf[bufptr] = Serial.read(); }
  if(bufptr>=255) Serial.println("Input buffer overflow!"); 
  if( buf[bufptr] == 0xD) 
   { 
     buf[bufptr] = 0;
     Serial.print("Received command: ");
     Serial.println(buf);
//     Serial.println(", processing...");
     res=sscanf(buf,"%c %s %u",&Command, Register, &Bit);
     if(res>0)
     { 
     switch(Command)
      {
      case 'C':
      if(res!=3) {Serial.println("Wrong command!"); break; }
      if(strcmp(Register,"STS")==0) 
      {
       sprintf(buf,"Set Channel Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_STS); if(Bit>0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_STS; }
      }
      else if(strcmp(Register,"SNC")==0) 
      {
       sprintf(buf,"Set Channel Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_SNC); if(Bit>0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_SNC; }
      }
      else if(strcmp(Register,"SG")==0) 
      {
       sprintf(buf,"Set Channel Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_SG0); if((Bit & 0x1) >0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_SG0; }
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_SG1); if((Bit & 0x2) >0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_SG1; }
      }
      else if(strcmp(Register,"ST")==0) 
      {
       sprintf(buf,"Set Channel Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_ST0); if((Bit & 0x1) >0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_ST0; }
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_ST1); if((Bit & 0x2) >0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_ST1; }
      }
      else if(strcmp(Register,"SDC")==0) 
      {
       sprintf(buf,"Set Channel Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_SDC); if(Bit>0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_SDC; }
      }
      else if(strcmp(Register,"SBF")==0) 
      {
       sprintf(buf,"Set Channel Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++) for(int ch=0;ch<16;ch++)  { REG_Ch[i][ch]= REG_Ch[i][ch] & (~C_SBF); if(Bit>0) REG_Ch[i][ch]= REG_Ch[i][ch] | C_SBF; }
      }
      
      else Serial.println("Nonexistent bit in register!");
      break;

      case 'G':
      if(res!=3) {Serial.println("Wrong command!"); break; }
      if(strcmp(Register,"S16")==0) 
      {
       sprintf(buf,"Set Global Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++)  { REG_Global[i]= REG_Global[i] & (~G_S16); if(Bit>0) REG_Global[i]= REG_Global[i] | G_S16; }
      }
      else if(strcmp(Register,"STB")==0) 
      {
       sprintf(buf,"Set Global Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++)  { REG_Global[i]= REG_Global[i] & (~G_STB); if(Bit>0) REG_Global[i]= REG_Global[i] | G_STB; }
      }
      else if(strcmp(Register,"STB1")==0) 
      {
       sprintf(buf,"Set Global Register bit %s to %u.",Register,Bit); 
       for(int i=0;i<NCHIPS;i++)  { REG_Global[i]= REG_Global[i] & (~G_STB1); if(Bit>0) REG_Global[i]= REG_Global[i] | G_STB1; }
       Serial.println(buf);
      }
      else if(strcmp(Register,"SLK")==0) 
      {
       sprintf(buf,"Set Global Register bit %s to %u.",Register,Bit); 
       Serial.println(buf);
       for(int i=0;i<NCHIPS;i++)  { REG_Global[i]= REG_Global[i] & (~G_SLK); if(Bit>0) REG_Global[i]= REG_Global[i] | G_SLK; }
      }

      else if(strcmp(Register,"TPD")==0) 
      {
       PulseWidth=Bit;
         if(PulsePeriod>PulseWidth && PulseWidth>0) Serial.print("Test pulse generator enabled: ");
         else Serial.print("Test pulse generator disabled: ");
         Serial.print("period= ");Serial.print(PulsePeriod);
         Serial.print(" mcs, duration= ");Serial.print(PulseWidth);
         Serial.println(" mcs.");

      }
      else if(strcmp(Register,"TPP")==0) 
      {
       PulsePeriod=Bit;
         if(PulsePeriod>PulseWidth && PulseWidth>0) Serial.print("Test pulse generator enabled: ");
         else Serial.print("Test pulse generator disabled: ");
         Serial.print("period= ");Serial.print(PulsePeriod);
         Serial.print(" mcs, duration= ");Serial.print(PulseWidth);
         Serial.println(" mcs.");

      }
      
      else Serial.println("Nonexistent bit in register!");
      break;
      
      case 'S':
      SaveConfig();
      Serial.println("Saved config in EEPROM.");
      break;

      case 'B':
      SetBasicConfig();
      Serial.println("Reversed to basic config.");
      break;
      
      case 'R':
      RestoreConfig();
      Serial.println("Restored config from EEPROM.");
      break;
      
      case 'T':
      SendConfig();
      Serial.println("Sent config to LARASIC board.");
      break;

      case '0':
      Serial.println("Resetting controller...");
      resetFunc();  //call reset
      break;

      case 'P':
      Serial.println("Current config:");
      PrintConfig();  
      break;
 
      case 'V':
      Serial.println("Power supply parameters:");
      PrintVoltages();  
      break;
    
      case 'H':
      PrintHelp();  
      break;
      
      default: 
      Serial.println("Command not recognized!"); 
      break;
      }
     }
     else 
     {
     Serial.println("Command not recognized!"); 
     }
     
     bufptr=-1;
//        Serial.println("Warning! This firmware is AdHoc for LARASIC vesrions chain: V4-V7-V7-V7!");
   }
   
if(PulsePeriod>PulseWidth && PulseWidth>0)
{
  delayMicroseconds(PulsePeriod-PulseWidth);
  digitalWrite(ledPin, HIGH);
  noInterrupts();                 //time critical
  digitalWrite(tstPin, HIGH);
  delayMicroseconds(PulseWidth);
  digitalWrite(tstPin, LOW);
  interrupts();                   //end of time critical
  digitalWrite(ledPin, LOW);
}  
else 
{
  digitalWrite(tstPin, LOW);
  digitalWrite(ledPin, LOW);
}
//  delay(300);
//    digitalWrite(ledPin, HIGH);
//  delay(300);
}

void SaveConfig()
{
  for(int i=0;i<NCHIPS;i++)
  {
   EEPROM.write(i*17, REG_Global[i]);
   for(int ch=0;ch<16;ch++) EEPROM.write(i*17+ch+1, REG_Ch[i][ch]);
  }
}

void RestoreConfig()
{
   for(int i=0;i<NCHIPS;i++)
  {
   REG_Global0[i] = 0x40;
   REG_Global[i] = EEPROM.read(i*17);
   for(int ch=0;ch<16;ch++)  REG_Ch[i][ch] = EEPROM.read(i*17+ch+1);
  }

}

void SendConfig()
{
    digitalWrite(rstPin, LOW); //Global reset
  delay(tD);
    digitalWrite(rstPin, HIGH);
  delay(tD);

  digitalWrite(csPin, HIGH);
  digitalWrite(ledPin, HIGH); //set CS high

  for(int i=NCHIPS-1;i>=0;i--)
  {
   for(int ch=15;ch>=0;ch--) SendByte(REG_Ch[i][ch]);
   SendByte(REG_Global[i]);
   if(i!=OLD_CHIP_INDEX) SendByte(REG_Global0[i]);
  }
  digitalWrite(csPin, LOW); //latch shift register
  digitalWrite(ledPin, LOW); //latch shift register
  delay(tD);
//  digitalWrite(csPin, HIGH);
//  digitalWrite(ledPin, HIGH); //latch shift register
  delay(tD);
  
}

void SendByte(unsigned char Byte)
{
 for(int b=0;b<8;b++) 
 {
  digitalWrite(clkPin, HIGH);
  if( ((Byte<<b) & 0x80) == 0)  { digitalWrite(sdoPin, LOW);digitalWrite(ledPin, LOW);}
  else { digitalWrite(sdoPin, HIGH);digitalWrite(ledPin, HIGH);}
  delay(tD);
  digitalWrite(clkPin, LOW);
  delay(tD);
 }
}

void SetBasicConfig()
{
  for(int i=NCHIPS-1;i>=0;i--)
  {
   for(int ch=15;ch>=0;ch--)   REG_Ch[i][ch]= C_SBF | C_ST1 | C_ST0 | C_SG1 | C_SG0 | C_SNC; 
   REG_Global0[i] = 0x40;
   REG_Global[i]= 0x00;
  }
}

void PrintConfig()
{
  int Byte;
        Serial.println("Warning! This firmware is AdHoc for LARASIC vesrions chain: V4-V7-V7-V7!");
    Serial.print("Channel: ");
   // Serial.print("CHIP N : ");
    for(int ch=15;ch>=0;ch--)  
    { 
//    Byte=REG_Ch[i][ch];
     Serial.print("0"); Serial.print( ch,HEX ); Serial.print(" : "); 
    }
    Serial.println(" ");

    for(int i=NCHIPS-1;i>=0;i--)
  {
    Serial.print("CHIP "); Serial.print(i);      Serial.print(" : ");
    for(int ch=15;ch>=0;ch--)  
    { 
    Byte=REG_Ch[i][ch];  
    Serial.print( Byte,HEX ); Serial.print(" : "); 
    }
//    Serial.println(" ");
    Byte= REG_Global[i];
    Serial.print("GLOBAL ");  Serial.print(Byte,HEX);
    Byte= REG_Global0[i];
    if(i!=OLD_CHIP_INDEX) { Serial.print(" "); Serial.print(Byte,HEX);}

    Serial.println(" ");
  // REG_Global[i]= 0x00;
  }
  if(PulsePeriod>PulseWidth && PulseWidth>0) Serial.print("Test pulse generator enabled: ");
  else Serial.print("Test pulse generator disabled: ");
  
  Serial.print("period= ");Serial.print(PulsePeriod);
  Serial.print(" mcs, duration= ");Serial.print(PulseWidth);
  Serial.println(" mcs.");
  PrintVoltages();
}

void      PrintVoltages()
{
  float vdd = float(analogRead(vddPin))/1024.*5.;
  float vddp = float(analogRead(vddpPin))/1024.*5.;
  Serial.print("Vdd = ");Serial.print(vdd);Serial.print(" V; Vddp = ");Serial.print(vddp);Serial.println(" V.");
}


void PrintHelp()
{
        Serial.println("Warning! This firmware is AdHoc for LARASIC vesrions chain: V4-V7-V7-V7!");
      Serial.println("************* Command help *****************");
      Serial.println("C BIT VALUE  set register bit in channel register");
      Serial.println("G BIT VALUE  set register bit in global register");
      Serial.println("G TPD VALUE  set test pulse duration, microseconds, setting to 0 disables test pulse.");
      Serial.println("G TPP VALUE  set test pulse period, microseconds, setting to 0 disables test pulse.");
      Serial.println("P print current config");
      Serial.println("S save current config into EEPROM");
      Serial.println("R restore config from EEPROM");
      Serial.println("B return to basic config (routine readout mode)");
      Serial.println("T initiate transmission of config to frontend board");
      Serial.println("V display power supply output voltages in V");    
      Serial.println("0 reset controller");
      Serial.println("H print this help page");
      Serial.println("********************************************");
        Serial.println("Warning! This firmware is AdHoc for LARASIC vesrions chain: V4-V7-V7-V7!");
}


