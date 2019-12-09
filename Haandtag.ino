/*
  ACS C50XB Handle version 2.25
  Hardware:
  Mega 2560
  C50XB Radio Transceiver
*/

#include <EEPROM.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <U8g2lib.h>

#define DISPLAY              u8g2

Adafruit_MPL3115A2 Barometer = Adafruit_MPL3115A2();
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C DISPLAY(U8G2_R0); 

#define TRANSCEIVER_SERIAL               Serial

#define VERSION_LEN                         5
char Version[VERSION_LEN] = {'2', '.', '2', '5', '\0'};

int TRANSMISSION_CHANNEL_RANGE[2] =         {1, 40};

#define TRANSMISSION_BAND_433MHZ            0x01
#define TRANSMISSION_BAND_470MHZ            0x02
#define TRANSMISSION_BAND_868MHZ            0x03
#define TRANSMISSION_BAND_915MHZ            0x04
const PROGMEM int TRANSMISSION_BAND_RANGE[2] =            {TRANSMISSION_BAND_433MHZ, TRANSMISSION_BAND_915MHZ};

#define WIRELESS_TRANSMISSION_RATE_2400     0x01 
#define WIRELESS_TRANSMISSION_RATE_4800     0x02
#define WIRELESS_TRANSMISSION_RATE_9600     0x03
#define WIRELESS_TRANSMISSION_RATE_14400    0x04
#define WIRELESS_TRANSMISSION_RATE_19200    0x05
#define WIRELESS_TRANSMISSION_RATE_38400    0x06
#define WIRELESS_TRANSMISSION_RATE_57600    0x07
#define WIRELESS_TRANSMISSION_RATE_76800    0x08
#define WIRELESS_TRANSMISSION_RATE_115200   0x09
const PROGMEM int WIRELESS_TRANSMISSION_RATE_RANGE[2] =   {WIRELESS_TRANSMISSION_RATE_2400, WIRELESS_TRANSMISSION_RATE_115200};


#define TRANSMISSION_POWER_1dBm             0x00
#define TRANSMISSION_POWER_2dBm             0x01
#define TRANSMISSION_POWER_5dBm             0x02
#define TRANSMISSION_POWER_8dBm             0x03
#define TRANSMISSION_POWER_11dBm            0x04
#define TRANSMISSION_POWER_14dBm            0x05
#define TRANSMISSION_POWER_17dBm            0x06
#define TRANSMISSION_POWER_20dBm            0x07
const PROGMEM int TRANSMISSION_POWER_RANGE[2] =           {TRANSMISSION_POWER_1dBm, TRANSMISSION_POWER_20dBm};

#define SERIAL_TRANSMISSION_RATE_2400       0x01 
#define SERIAL_TRANSMISSION_RATE_4800       0x02
#define SERIAL_TRANSMISSION_RATE_9600       0x03
#define SERIAL_TRANSMISSION_RATE_14400      0x04
#define SERIAL_TRANSMISSION_RATE_19200      0x05
#define SERIAL_TRANSMISSION_RATE_38400      0x06
#define SERIAL_TRANSMISSION_RATE_57600      0x07
#define SERIAL_TRANSMISSION_RATE_76800      0x08
#define SERIAL_TRANSMISSION_RATE_115200     0x09
#define FACTORY_SERIAL_BAUD_RATE            SERIAL_TRANSMISSION_RATE_9600
const PROGMEM int SERIAL_TRANSMISSION_RATE_RANGE[2] =     {SERIAL_TRANSMISSION_RATE_2400, SERIAL_TRANSMISSION_RATE_115200};

#define SERIAL_DATA_7BITS                   0x01
#define SERIAL_DATA_8BITS                   0x02
#define SERIAL_DATA_9BITS                   0x03
const PROGMEM int SERIAL_DATA_BIT_RANGE[2] =              {SERIAL_DATA_7BITS, SERIAL_DATA_9BITS};

#define SERIAL_STOP_1BIT                    0x01
#define SERIAL_STOP_2BIT                    0x02
const PROGMEM int SERIAL_STOP_BIT_RANGE[2] =              {SERIAL_STOP_1BIT, SERIAL_STOP_2BIT};

#define SERIAL_PARITY_NONE                  0x01
#define SERIAL_PARITY_ODD                   0x02
#define SERIAL_PARITY_EVEN                  0x03
const PROGMEM int SERIAL_PARITY_RANGE[2] =                {SERIAL_PARITY_NONE, SERIAL_PARITY_EVEN};

const PROGMEM int NET_ID_RANGE[2] =                       {0, 255};
const PROGMEM int NODE_ID_RANGE[2] =                      {0, 255};

// Contains all the settings that can be modified in the C50XB RF transceiver
struct Transceiver_Setting_Struct {
  char Transmit_Channel = 20;                                 // Each frequency band has 40 channels, numbered 1-40, seperated by 1 MHz each.
  char Transmit_Band = TRANSMISSION_BAND_433MHZ;              // There are 4 available transmission frequency bands, 433, 470, 867, and 915 MHz.
  char Transmission_Rate = WIRELESS_TRANSMISSION_RATE_2400;   // There are 9 RF transmission rates to choose from, the lowest being 2400 bps.
  char Transmission_Power = TRANSMISSION_POWER_20dBm;         // Transmission power ranges from -1 to +20dBm
  char Serial_Baud_Rate = SERIAL_TRANSMISSION_RATE_9600;      // Serial transmission rate ranges from 2400 to 11520 bps. Strandard is 9600 bps.
  char Serial_Data_Bits = SERIAL_DATA_8BITS;                  // Serial data bits can be 7, 8, or 9. Standard is 8 bits.
  char Serial_Stop_Bits = SERIAL_STOP_1BIT;                   // Number of serial stop bits, can be 1 or 2. The standard is 1 bit.
  char Serial_Parity = SERIAL_PARITY_NONE;                    // The serial parity setting, which can be NONE, ODD or EVEN. Standard is NONE.
  unsigned long Net_Id = 1;                                   // The Network ID ranges from 0 to 4294967295, as it consists of 4 bytes of data.
  unsigned int Node_Id = 0;                                   // The Node ID ranges from 0 to 65535, as it consists of 2 bytes of data.
};

#define WRITE_INSTRUCTION_BYTES     sizeof(Transceiver_Setting_Struct)
#define INITIALIZER_1               0xAA
#define INITIALIZER_2               0xFA
#define WRITE_COMMAND               0x03

const PROGMEM int Settings_Ranges[WRITE_INSTRUCTION_BYTES][2] =  {TRANSMISSION_CHANNEL_RANGE[0], TRANSMISSION_CHANNEL_RANGE[1],
                                                                 TRANSMISSION_BAND_RANGE[0], TRANSMISSION_BAND_RANGE[1],
                                                                 WIRELESS_TRANSMISSION_RATE_RANGE[0], WIRELESS_TRANSMISSION_RATE_RANGE[1],
                                                                 TRANSMISSION_POWER_RANGE[0], TRANSMISSION_POWER_RANGE[1],
                                                                 SERIAL_TRANSMISSION_RATE_RANGE[0], SERIAL_TRANSMISSION_RATE_RANGE[1],
                                                                 SERIAL_DATA_BIT_RANGE[0], SERIAL_DATA_BIT_RANGE[1],
                                                                 SERIAL_STOP_BIT_RANGE[0], SERIAL_STOP_BIT_RANGE[1],
                                                                 SERIAL_PARITY_RANGE[0], SERIAL_PARITY_RANGE[1],
                                                                 NET_ID_RANGE[0], NET_ID_RANGE[1],
                                                                 NET_ID_RANGE[0], NET_ID_RANGE[1],
                                                                 NET_ID_RANGE[0], NET_ID_RANGE[1],
                                                                 NET_ID_RANGE[0], NET_ID_RANGE[1],
                                                                 NODE_ID_RANGE[0], NODE_ID_RANGE[1],
                                                                 NODE_ID_RANGE[0], NODE_ID_RANGE[1],
                                                                 };
                                                   
// Simple check of whether or not a value is within a LOW to HIGH integer range
bool Within_Range(int value, int low, int high) {
    return !(value < low) && !(high < value);
};

// Going through a Transceiver Setting Structure byte array and check if all values fall within range, returning false if not
bool Settings_Within_Range(byte Settings[])
{
  bool Check;
  //DEBUGGING_SERIAL.println(F("Checking if settings within range!"));
  for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
  {
    Check = Within_Range(Settings[N], Settings_Ranges[N][0], Settings_Ranges[N][1]);
    if (Check == false)
    {  
      return false;
    }
  }
  return true;
}                                                                              


// The three first bytes initializes the transceiver write mode, followed by WRITE_INSTRUCTION_BYTES bytes of setting data to update the transceiver
void Update_Transceiver(byte Array[])         
{
  if (Settings_Within_Range(Array) == true)
  {
    TRANSCEIVER_SERIAL.write(INITIALIZER_1);
    TRANSCEIVER_SERIAL.write(INITIALIZER_2);
    TRANSCEIVER_SERIAL.write(WRITE_COMMAND);
    for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
    {
      TRANSCEIVER_SERIAL.write(Array[N]);
    }
  }
}

#define EEPROM_SETTINGS_ADDRESS     0

// The Arduino handle settings are saved into the EEPROM memory, using the EEPROM_SETTINGS_ADDRESS as starting point
void EEPROM_Save_Settings(byte Settings[])
{
    if (Settings_Within_Range(Settings) == true)
    {
      int Address = EEPROM_SETTINGS_ADDRESS;
      for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
      {
      EEPROM.update(Address, Settings[N]);
      Address++;
      }
    }
}

int BAUD_RATES[10] = {0, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 76800, 115200};

// Uses a WRITE_INSTRUCTION_BYTES sized byte array with new settings to update the Transceiver settings and save them to EEPROM
void Change_Transceiver_Settings(byte Old_Settings[], byte New_Settings[], Transceiver_Setting_Struct* Setting_Pointer)
{
  if (sizeof(New_Settings) == sizeof(Old_Settings) && Settings_Within_Range(New_Settings) == true)
  {
    for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
    {
      Old_Settings[N] = New_Settings[N];
    }
    Update_Transceiver(Old_Settings);
    delay(50);
    EEPROM_Save_Settings(Old_Settings);
    delay(50);
    TRANSCEIVER_SERIAL.end();
    delay(50);
    TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Setting_Pointer->Serial_Baud_Rate]);
    delay(50);
  }
}

// Loads the settings saved in the EEPROM into the Arduino handle settings
void EEPROM_Load_Settings(byte Settings[])
{
    int Address = EEPROM_SETTINGS_ADDRESS;
    for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
    {
      Settings[N] = EEPROM.read(Address);
      Address++;
    }
}

#define FACTORY_SETTINGS_COMMAND      0x02

// Sends the factory reset instructions to the transceiver
void Reset_Transceiver()
{
  TRANSCEIVER_SERIAL.write(INITIALIZER_1);
  TRANSCEIVER_SERIAL.write(INITIALIZER_2);
  TRANSCEIVER_SERIAL.write(FACTORY_SETTINGS_COMMAND);
  delay(50);
}

// The Handle Settings structure is initialized
Transceiver_Setting_Struct Handle_Settings;
//Transceiver_Setting_Struct Handle_Settings_2;

#define CS_SETTING_PIN                10
#define SET_SETTING_PIN               11
#define SEND_SETTINGS_COMMAND         0x01
#define CS_ON                         LOW
#define CS_OFF                        HIGH
#define SET_ON                        LOW
#define SET_OFF                       HIGH

#define PAIRING_BUTTON_PIN          6
#define WIRECUT_BUTTON_PIN          7
#define ACTIVATE_RUN_PIN            8
#define BUTTON_4_PIN                9
#define BUTTON_ON                   LOW
#define BUTTON_OFF                  HIGH

struct Button
{
  boolean Status;
  int     Pin_No;
};

struct Buttons_Struct
{
  Button  Pairing;
  Button  WireCut;
  Button  Activate_Run;
  Button  Mode_Shift;
};

Buttons_Struct Buttons = {false,  PAIRING_BUTTON_PIN, 
                          false,  WIRECUT_BUTTON_PIN,
                          false,  ACTIVATE_RUN_PIN,
                          false,  BUTTON_4_PIN};

struct Throttle_Struct
{
  Button  Low;
  Button  Middle;
  Button  High;
};

#define THROTTLE_LOW_PIN            3
#define THROTTLE_MID_PIN            4
#define THROTTLE_HIGH_PIN           5
#define THROTTLE_ON                 HIGH

Throttle_Struct Throttle = {false,  THROTTLE_LOW_PIN, 
                            false,  THROTTLE_MID_PIN,
                            false,  THROTTLE_HIGH_PIN};

#define PAIR_STATUS_EEPROM_ADDRESS     WRITE_INSTRUCTION_BYTES + 1
#define VERSION_EEPROM_ADDRESS         PAIR_STATUS_EEPROM_ADDRESS + 1
bool Pairing_Succesful = false;

// Saves the Pairing status to the EEPROM at PAIR_STATUS_EEPROM_ADDRESS
void EEPROM_Save_Pairing_Status()
{
      int Address = PAIR_STATUS_EEPROM_ADDRESS;
      EEPROM.update(Address, Pairing_Succesful);
}

// Loads the Pairing status to the EEPROM at PAIR_STATUS_EEPROM_ADDRESS
void EEPROM_Load_Pairing_Status()
{
    int Address = PAIR_STATUS_EEPROM_ADDRESS;
    Pairing_Succesful = EEPROM.read(Address);
}

// Saves the Version string to the EEPROM at VERSION_EEPROM_ADDRESS
void Version_Save()
{
  int Address = VERSION_EEPROM_ADDRESS;
  for (int N = 0; N < VERSION_LEN; N++)
  {
    EEPROM.update(Address, Version[N]);
    Address++;
  }
}

// Loads the Version string to the EEPROM at VERSION_EEPROM_ADDRESS
void Version_Load(char Version_String[VERSION_LEN])
{
  int Address = VERSION_EEPROM_ADDRESS;
  for (int N = 0; N < VERSION_LEN; N++)
  {
    Version_String[N] = EEPROM.read(Address);
    Address++;
  }
}

#define SERIAL_TIMEOUT      500
#define DISPLAY_LINE_1      8
#define DISPLAY_LINE_2      16
#define DISPLAY_LINE_3      24
#define DISPLAY_LINE_4      32

float Baseline_Altitude;

void setup() 
{
  // Initialize the u8g2 display
  DISPLAY.begin();
  DISPLAY.setFont(u8g2_font_5x7_tf);

  // Initialize all the buttons to their respective pins
  pinMode(Buttons.Pairing.Pin_No, INPUT);
  pinMode(Buttons.WireCut.Pin_No, INPUT); 
  pinMode(Buttons.Activate_Run.Pin_No, INPUT); 
  pinMode(Buttons.Mode_Shift.Pin_No, INPUT);
  pinMode(Throttle.Low.Pin_No, INPUT);
  pinMode(Throttle.Middle.Pin_No, INPUT);
  pinMode(Throttle.High.Pin_No, INPUT);
  
  /* 
  Setting up the SET and CS pins. The SET pin must be ON for configuration of the transceiver.
  The CS pin must be OFF for the transceiver to function at all.
  */ 
  pinMode(SET_SETTING_PIN, OUTPUT);
  pinMode(CS_SETTING_PIN, OUTPUT);
  delay(50);
  digitalWrite(CS_SETTING_PIN, CS_ON); // Turn CS on
  delay(50);
  digitalWrite(CS_SETTING_PIN, CS_OFF); // Turn CS off
  delay(50);
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
  delay(50);
  digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
  delay(100);
  
  /* 
  The version string saved in the EEPROM is checked, and if correct
  the Handle Settings structure is updated with the EEPROM values,
  whereafter the transceiver serial baud rate is reset to factory settings
  and the Handle Settings uploaded to the device, and the EEPROM serial baud rate reestablished
  */ 
  char Version_String[VERSION_LEN];
  Version_Load(Version_String);
  if (strcmp(Version_String, Version) == 0) // If the loaded string is equal to the code's version, proceed
  {
  DISPLAY.drawStr(0, DISPLAY_LINE_1, "Version correct");
  DISPLAY.sendBuffer();
  delay(1000);
  EEPROM_Load_Settings((byte*)&Handle_Settings);
  delay(50);
  EEPROM_Load_Pairing_Status();
  delay(50);
  }
  else
  {
    DISPLAY.drawStr(0, DISPLAY_LINE_1, "Version incorrect");
    DISPLAY.sendBuffer();
    delay(1000);
    Version_Save();
    EEPROM_Save_Pairing_Status();
    EEPROM_Save_Settings((byte*)&Handle_Settings);
  }
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Handle_Settings.Serial_Baud_Rate]);
  delay(50);
  Reset_Transceiver();
  delay(50);
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)FACTORY_SERIAL_BAUD_RATE]);
  delay(50);
  Handle_Settings.Net_Id = 1;
  Update_Transceiver((byte*)&Handle_Settings);
  //EEPROM_Save_Settings((byte*)&Handle_Settings);
  delay(50);
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Handle_Settings.Serial_Baud_Rate]);
  delay(50);
  //Handle_Settings_2.Serial_Baud_Rate = 3;  
  //Change_Transceiver_Settings((byte*)&Handle_Settings, (byte*)&Handle_Settings_2, &Handle_Settings);
  delay(50);
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off

  // Set font, serial timeout, baseline altitude and start the barometer
  DISPLAY.setFont(u8g2_font_5x7_tf);
  TRANSCEIVER_SERIAL.setTimeout(SERIAL_TIMEOUT);
  Barometer.begin();
  delay(50);
  Baseline_Altitude = Barometer.getAltitude();
  

}

struct Datagram_Struct {
  byte Heartbeat = 0;
  int Altitude_Meters = 0;
  bool Cut_Wire = false;
  bool Throttle_Low = false;
  bool Throttle_Mid = false;
  bool Throttle_High = false;
  bool Activate_Run = false;
  byte Checksum = 0;
};

// Datagram related variables
Datagram_Struct Datagram;
#define DATAGRAM_BYTES                sizeof(Datagram_Struct)
Datagram_Struct* Datagram_Pointer =   &Datagram;
byte* Datagram_Byte_Pointer =         (byte*)Datagram_Pointer;
byte Sent_Checksum;
byte Received_Datagram[DATAGRAM_BYTES];
byte Received_Datagram_Sum;


// Mode selection related variables
char* Handle_Modes[] =                {"Free", "Altitude"};
PROGMEM enum Modes                    {Free, // No height restriction on throttle
                                       Altitude, // Altitude-restriction on throttle
                                      };
enum Modes Mode_Selection =           Altitude;
#define NUMBER_OF_MODES               sizeof(Modes)


// Active/inactive run state display related variables
int Loop_Counter =                  0;
int8_t Circle_Positions[6] =        {97, 102, 107, 112, 118, 122};
#define CIRCLE_POSITION_MAX         sizeof(Circle_Positions)
int Danger_Zones_X[4] =             {0, 25, 0, 25};
int Danger_Zones_Y[4] =             {9, 9, 17, 17};
unsigned int Danger_Zone_Counter =  0;


// Battery and battery graphics related variables
int Battery_Charge;
unsigned int Battery_Counter;
#define BATTERY_ANALOG_INPUT_PIN          0
#define NUMBER_OF_BATTERY_RANGES          5
#define BATTERY_FLASH_FREQUENCY           1000
int16_t Battery_Ranges[5] =               {591, 641, 691, 741, 791};
bool Battery_Blink =                      false;
unsigned long int Battery_Flash_Timer =   millis();
unsigned long int Battery_Timestamp;



// Altitude-mode height-dependent activation-related variables
bool Active_Run_Button_Press =        false;
bool High_Speed_Allowed =             false;
#define HIGH_SPEED_HEIGHT             20
#define DEACTIVE_HIGH_SPEED_HEIGHT    17

// Miscellaneous variables
#define BUZZER_PIN                  12
bool Transmission_Succes =          false;
byte Old_Heartbeat =                0;
char Input;

void loop() 
{

  DISPLAY.clearBuffer();
  
  // Button 4 debug information to display
  if (digitalRead(Buttons.Mode_Shift.Pin_No) == BUTTON_ON && digitalRead(Buttons.Pairing.Pin_No) == BUTTON_ON)
  {
  while (TRANSCEIVER_SERIAL.available() > 0)
  {
    Input = TRANSCEIVER_SERIAL.read();
  }
  digitalWrite(SET_SETTING_PIN, SET_ON);
  delay(50);
  TRANSCEIVER_SERIAL.write(INITIALIZER_1);
  TRANSCEIVER_SERIAL.write(INITIALIZER_2);
  TRANSCEIVER_SERIAL.write(SEND_SETTINGS_COMMAND);
  delay(50);
  DISPLAY.clear();
  DISPLAY.drawStr(0, DISPLAY_LINE_4, "Debug");
  DISPLAY.sendBuffer();
  delay(2000);
  while (TRANSCEIVER_SERIAL.available() > 0)
  {
    Input = TRANSCEIVER_SERIAL.read();
    DISPLAY.setCursor(40, DISPLAY_LINE_4);
    DISPLAY.print(Input, DEC);
    DISPLAY.sendBuffer();
    delay(300);
    DISPLAY.clear();
  }
    digitalWrite(SET_SETTING_PIN, SET_OFF);
  }
  
  // Check all input and update status
  if (digitalRead(Buttons.Pairing.Pin_No) == BUTTON_OFF)
  {
    Buttons.Pairing.Status = false;
  }
  else
  {
    Buttons.Pairing.Status = true;
  }

  if (digitalRead(Buttons.WireCut.Pin_No) == BUTTON_OFF)
  {
    Datagram.Cut_Wire = false;
  }
  else
  {
    Datagram.Cut_Wire = true;
  }

  if (digitalRead(Buttons.Activate_Run.Pin_No) == BUTTON_ON && Pairing_Succesful == true)
  {
    Active_Run_Button_Press = true;
  }

  if (digitalRead(Buttons.Activate_Run.Pin_No) == BUTTON_OFF && Active_Run_Button_Press == true)
  {  
    if (Datagram.Activate_Run == false)
    {
      Datagram.Activate_Run = true;
    }
    else
    {
      Datagram.Activate_Run = false;
    }
    Active_Run_Button_Press = false;
  }

  // Check mode selection and engage the barometer if necessary
  if (Mode_Selection == Altitude)
  {
     Datagram.Altitude_Meters = (int)(Barometer.getAltitude() - Baseline_Altitude);
     DISPLAY.drawStr(0, DISPLAY_LINE_4, "Alt: ");
     DISPLAY.setCursor(22, DISPLAY_LINE_4);
     DISPLAY.print(Datagram.Altitude_Meters);
     if (Datagram.Altitude_Meters > HIGH_SPEED_HEIGHT)
     {
        High_Speed_Allowed = true;
     }
     if (High_Speed_Allowed == true && Datagram.Altitude_Meters < DEACTIVE_HIGH_SPEED_HEIGHT)
     {
        High_Speed_Allowed = false;
     }
  }
  if (Mode_Selection == Free)
  {
     Datagram.Altitude_Meters = 0; 
  }


  // Check throttle status and update according to conditions
  if (digitalRead(Throttle.Low.Pin_No) == THROTTLE_ON)
  {Throttle.Low.Status = true;Datagram.Throttle_Low = true;DISPLAY.drawBox(110,26,5,5);}
  else
  {Throttle.Low.Status = false;Datagram.Throttle_Low = false;DISPLAY.drawFrame(110,26,5,5);}

  if (digitalRead(Throttle.Middle.Pin_No) == THROTTLE_ON && Datagram.Throttle_Low == true)
  {Throttle.Middle.Status = true;Datagram.Throttle_Mid = true;DISPLAY.drawBox(116,24,5,7);}
  else
  {Throttle.Middle.Status = false;Datagram.Throttle_Mid = false; DISPLAY.drawFrame(116,24,5,7);}

  if (Mode_Selection == Altitude)
  {
    if (digitalRead(Throttle.High.Pin_No) == THROTTLE_ON && Datagram.Throttle_Mid == true && Datagram.Throttle_Low == true && High_Speed_Allowed == true)
    {Throttle.High.Status = true;Datagram.Throttle_High = true;DISPLAY.drawBox(122,22,5,9);}
    else
    {Throttle.High.Status = false;Datagram.Throttle_High = false;DISPLAY.drawFrame(122,22,5,9);}
  }
  else
  {
    if (digitalRead(Throttle.High.Pin_No) == THROTTLE_ON && Datagram.Throttle_Mid == true && Datagram.Throttle_Low == true)
    {Throttle.High.Status = true;Datagram.Throttle_High = true;DISPLAY.drawBox(122,22,5,9);}
    else
    {Throttle.High.Status = false;Datagram.Throttle_High = false;DISPLAY.drawFrame(122,22,5,9);}
  }
  
  // If Pairing button is pressed, attempt to connect to controller
  if (Buttons.Pairing.Status == true && Datagram.Activate_Run == false) 
  {
    Buttons.Activate_Run.Status = false;
    Datagram.Activate_Run = false;
    DISPLAY.clear();
    DISPLAY.setFont(u8g2_font_crox4hb_tf);
    DISPLAY.drawStr(0, DISPLAY_LINE_2, "Pairing");
    DISPLAY.setCursor(0, DISPLAY_LINE_4);
    DISPLAY.print(F("v"));
    DISPLAY.setCursor(10, DISPLAY_LINE_4);
    DISPLAY.print(Version);
    DISPLAY.sendBuffer();
    DISPLAY.setFont(u8g2_font_5x7_tf);
    while (TRANSCEIVER_SERIAL.available() > 0)
    {
    Input = TRANSCEIVER_SERIAL.read(); // Read all to clear buffer
    }
    Pair_Transceivers((byte*)&Handle_Settings, &Handle_Settings);
    DISPLAY.clear();
    Baseline_Altitude = Barometer.getAltitude();
  }
  
  // DATAGRAM TRANSMISSION - If both paired and receiving serial data, check incoming datagram
  Transmission_Succes = false;
  if (Pairing_Succesful == true && Serial.available() > 0)
  {    
    if (Serial.find(Version) == true) // If the version is correct, read incoming datagram bytes into Received_Datagram
    {
      Serial.readBytes(Received_Datagram, DATAGRAM_BYTES);
      Received_Datagram_Sum = 0;
      for (int N = 0; N < DATAGRAM_BYTES-1;N++)  // Calculate checksum of Received_Datagram
      {
        Received_Datagram_Sum += Received_Datagram[N];
      }
      // Proceed to send datagram if calculated and received checksum are equal
      if (Received_Datagram_Sum == Received_Datagram[DATAGRAM_BYTES-1])
      {
        Old_Heartbeat = Datagram.Heartbeat;
        Datagram.Heartbeat = Received_Datagram[0];
        Sent_Checksum = 0;
        Datagram.Checksum = 0;
        for (int N = 0; N < DATAGRAM_BYTES-1;N++) // Calculate the outgoing datagram's checksum
        {
          Sent_Checksum += Datagram_Byte_Pointer[N];
        }
        Datagram.Checksum = Sent_Checksum;
        while (Serial.available() > 0) // Empty the serial buffer before sending the datagram
        {
          Serial.read();
        }
        Serial.write(Version); // Send both the version string and the whole datagram
        for (int N = 0; N < DATAGRAM_BYTES;N++)
        {
          Serial.write((Datagram_Byte_Pointer[N]));
        }
        if (Old_Heartbeat != Datagram.Heartbeat)
        {
          Transmission_Succes = true;
        }
      }
    }
  }
  
  // Draw various things on the display
  DISPLAY.setCursor(60, DISPLAY_LINE_4);
  DISPLAY.print(F("Throttle: "));
  if (Pairing_Succesful == true)
  {
    DISPLAY.setCursor(60, DISPLAY_LINE_1);
    DISPLAY.print(F("Ch: "));
    DISPLAY.setCursor(77, DISPLAY_LINE_1);
    DISPLAY.print(Handle_Settings.Transmit_Channel, DEC);
  }
  else
  {
    DISPLAY.drawStr(60, DISPLAY_LINE_1, "Not Paired");
  }
  
  // Draw glyph that represents the activate run status
  if (Pairing_Succesful == true)
  {
    if (Datagram.Activate_Run == true)
    {
      DISPLAY.setFont(u8g2_font_open_iconic_check_2x_t);
      DISPLAY.setCursor(19, 0);
      DISPLAY.drawGlyph(8,17, 65);
      DISPLAY.setFont(u8g2_font_open_iconic_embedded_1x_t);
      DISPLAY.drawGlyph(Danger_Zones_X[Danger_Zone_Counter],Danger_Zones_Y[Danger_Zone_Counter], 71);
      if (Transmission_Succes == true)
      {
        if (Danger_Zone_Counter == 3)
        {
          Danger_Zone_Counter = 0;
        }
        else
        {
          Danger_Zone_Counter += 1;
        }
      }
      DISPLAY.setFont(u8g2_font_5x7_tf);
    }
    else
    {
      DISPLAY.setFont(u8g2_font_open_iconic_check_2x_t);
      DISPLAY.drawGlyph(8,17, 66);
      DISPLAY.setFont(u8g2_font_5x7_tf);
    }
  }
  
  // Draw moving discs if transmission is succesful
  if (Pairing_Succesful == true)
  {
    if (Transmission_Succes == true)
    {
       DISPLAY.drawDisc(Circle_Positions[Loop_Counter], DISPLAY_LINE_1-4, 3, U8G2_DRAW_ALL);
       if (Loop_Counter == CIRCLE_POSITION_MAX-1)
       {
         Loop_Counter = 0;
       }
       else
       {
         Loop_Counter += 1;
       }
    }
    else
    {
      DISPLAY.drawDisc(Circle_Positions[Loop_Counter], DISPLAY_LINE_1-4, 3, U8G2_DRAW_ALL);
    }
  }
  
  // Draw a battery! The analog input ranges from 841.1 when the batteries are fully charged, to 591.1 when depleted
  DISPLAY.drawRFrame(40, 6, 16, 26, 3);
  DISPLAY.drawFrame(44, 3, 8, 3);
  Battery_Charge = analogRead(BATTERY_ANALOG_INPUT_PIN);
  Battery_Counter = 0;
  for (int N = 0; N < NUMBER_OF_BATTERY_RANGES; N++)
  {
    if (Battery_Charge > Battery_Ranges[N])
    {
      Battery_Counter += 1;
    }
  }
  for (int N = 0; N < Battery_Counter; N++)
  {
    DISPLAY.drawBox(43, 26-(N*4), 10, 3); 
  }
  Battery_Flash_Timer = millis();
  if (Battery_Counter < 3 && Battery_Timestamp + BATTERY_FLASH_FREQUENCY < Battery_Flash_Timer)
  {
     if (Battery_Blink == false)
     {
        Battery_Blink = true;
        Battery_Timestamp = millis();
     }
     else
     {
        Battery_Blink = false;
        Battery_Timestamp = millis();
     }
  }
  if (Battery_Counter > 2)
  {
    Battery_Blink = false;
  }
  if (Battery_Blink == true)
  {
    DISPLAY.setFont(u8g2_font_open_iconic_embedded_1x_t);
    DISPLAY.drawGlyph(45, 19, 71);
    DISPLAY.setFont(u8g2_font_5x7_tf);
  }
  
  // If the Mode_Shift button is clicked the mode will change and inactivate run
  if (digitalRead(Buttons.Mode_Shift.Pin_No) == BUTTON_ON && Datagram.Activate_Run == false) 
  {
    if (Mode_Selection == (NUMBER_OF_MODES-1)) 
    {
      Mode_Selection = 0;
    }
    else 
    {
      Mode_Selection = Mode_Selection + 1;
    }
    DISPLAY.drawStr(60, DISPLAY_LINE_2, "Mode: ");
    DISPLAY.drawStr(90, DISPLAY_LINE_2, Handle_Modes[Mode_Selection]);
    DISPLAY.sendBuffer();
    Datagram.Activate_Run = false;
    Baseline_Altitude = Barometer.getAltitude();
    delay(500);
  }
  DISPLAY.drawStr(60, DISPLAY_LINE_2, "Mode: ");
  DISPLAY.drawStr(90, DISPLAY_LINE_2, Handle_Modes[Mode_Selection]);
  
  DISPLAY.sendBuffer();
}

// Pairing function related variables
#define INITIALIZER                      "Initializing"
#define INITIALIZER_END                  '*'
#define PAIRING_TIMEOUT_MILLISECONDS     5000
#define WRITE_INSTRUCTION_END            'E'
#define VERSION_END                      'T'
unsigned long int Pairing_Timer;
unsigned long int Pairing_Timestamp;

void Pair_Transceivers(byte Old_Settings[], Transceiver_Setting_Struct* Handle)
{
  // Resetting the transceiver to enable talk with controller
  digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
  delay(50);
  Reset_Transceiver(); // Resets the transceiver to factory default settings
  delay(50);  
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)FACTORY_SERIAL_BAUD_RATE]); // Establish a serial connection using the standard baud rate
  delay(50);
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
  delay(50);
  
  // Waiting for PAIRING_TIMEOUT_MILLISECONDS milliseconds for the handshake signal from the controller
  Pairing_Timestamp = millis();
  bool Data_Received = true;
  while (TRANSCEIVER_SERIAL.available() == 0)
  {
    Pairing_Timer = millis();
    if (Pairing_Timer > Pairing_Timestamp + PAIRING_TIMEOUT_MILLISECONDS) // Will keep waiting until a certain time has passed
    {
      Data_Received = false;
      break;
    }
  }
  delay(150); // Wait for the whole message to be received
  
  // If data was received, check if the signal was correct and close the serial communication to flush
  bool Handshake = false;
  if (Data_Received == true)
  {
    
    String Received_Shake = TRANSCEIVER_SERIAL.readStringUntil('T');
    Received_Shake = TRANSCEIVER_SERIAL.readStringUntil('T');
    if (Received_Shake == Version)
    {
      Handshake = true;
    }
    delay(1000); // Wait for another transmission
    Received_Shake = TRANSCEIVER_SERIAL.readStringUntil('T');
    Received_Shake = TRANSCEIVER_SERIAL.readStringUntil('T');
    if (Received_Shake == Version)
    {
      Handshake = true;
    }
  }

  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)FACTORY_SERIAL_BAUD_RATE]);
  delay(50);
  
  // If handshake was correct, send one back continuously until data is received
  if (Handshake == true)
  {
    Pairing_Timestamp = millis();
    while (TRANSCEIVER_SERIAL.available() == 0)
    {
      TRANSCEIVER_SERIAL.write(Version);
      TRANSCEIVER_SERIAL.write(VERSION_END);
      TRANSCEIVER_SERIAL.flush();
      Pairing_Timer = millis();
      if (Pairing_Timer > Pairing_Timestamp + PAIRING_TIMEOUT_MILLISECONDS) // Will keep sending until a certain time has passed
      {
        Data_Received = false;
        break;
      }
      delay(1000); // The delay is necessary for incoming data to come through
    } 
  }  
  delay(250);
  
  // If data was sent back, check the initializing string and then use the following byte array to change the handle/transceiver settings
  bool Correct_Initializer = false;
  if (Data_Received == true && Handshake == true)
  {
    String Received_Ini = TRANSCEIVER_SERIAL.readStringUntil(INITIALIZER_END);
    if (Received_Ini == INITIALIZER)
    {
      Correct_Initializer = true;
      byte Received_Settings[WRITE_INSTRUCTION_BYTES]; 
      TRANSCEIVER_SERIAL.readBytesUntil(WRITE_INSTRUCTION_END, Received_Settings, WRITE_INSTRUCTION_BYTES+1);
      delay(50);
      digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
      delay(50);
      Change_Transceiver_Settings(Old_Settings, Received_Settings, Handle);                  
    }
  }

  // If any part of the pairing fails, revert to default handle settings
  if (Data_Received == false || Handshake == false || Correct_Initializer == false)
  {
    Transceiver_Setting_Struct Backup_Settings;
    digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
    delay(100);
    Change_Transceiver_Settings(Old_Settings, (byte*)&Backup_Settings, Handle);
    delay(50);
  }

  // Save the pairing status to the EEPROM
  if (Data_Received == true && Handshake == true && Correct_Initializer == true)
  {
    Pairing_Succesful = true;
    EEPROM_Save_Pairing_Status();
  }
  else
  {
    Pairing_Succesful = false;
    EEPROM_Save_Pairing_Status();
  }

  // Restart serial communication based on the handle settings
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Handle->Serial_Baud_Rate]);
  delay(250);
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
}


