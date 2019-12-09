/*
  ACS C50XB Controller version 2.25
  Hardware:
  Arduino Mega 2560
  C50XB Radio Transceiver
*/

#include <EEPROM.h>

#define TRANSCEIVER_SERIAL                  Serial1

#define VERSION_LEN                         5
char Version[VERSION_LEN] = {'2', '.', '2', '5', '\0'};

#define TRANSMISSION_BAND_433MHZ            0x01
#define TRANSMISSION_BAND_470MHZ            0x02
#define TRANSMISSION_BAND_868MHZ            0x03
#define TRANSMISSION_BAND_915MHZ            0x04
int TRANSMISSION_BAND_RANGE[2] =            {TRANSMISSION_BAND_433MHZ, TRANSMISSION_BAND_915MHZ};

#define WIRELESS_TRANSMISSION_RATE_2400     0x01 
#define WIRELESS_TRANSMISSION_RATE_4800     0x02
#define WIRELESS_TRANSMISSION_RATE_9600     0x03
#define WIRELESS_TRANSMISSION_RATE_14400    0x04
#define WIRELESS_TRANSMISSION_RATE_19200    0x05
#define WIRELESS_TRANSMISSION_RATE_38400    0x06
#define WIRELESS_TRANSMISSION_RATE_57600    0x07
#define WIRELESS_TRANSMISSION_RATE_76800    0x08
#define WIRELESS_TRANSMISSION_RATE_115200   0x09
int WIRELESS_TRANSMISSION_RATE_RANGE[2] =   {WIRELESS_TRANSMISSION_RATE_2400, WIRELESS_TRANSMISSION_RATE_115200};


#define TRANSMISSION_POWER_1dBm             0x00
#define TRANSMISSION_POWER_2dBm             0x01
#define TRANSMISSION_POWER_5dBm             0x02
#define TRANSMISSION_POWER_8dBm             0x03
#define TRANSMISSION_POWER_11dBm            0x04
#define TRANSMISSION_POWER_14dBm            0x05
#define TRANSMISSION_POWER_17dBm            0x06
#define TRANSMISSION_POWER_20dBm            0x07
int TRANSMISSION_POWER_RANGE[2] =           {TRANSMISSION_POWER_1dBm, TRANSMISSION_POWER_20dBm};

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
int SERIAL_TRANSMISSION_RATE_RANGE[2] =     {SERIAL_TRANSMISSION_RATE_2400, SERIAL_TRANSMISSION_RATE_115200};

#define SERIAL_DATA_7BITS                   0x01
#define SERIAL_DATA_8BITS                   0x02
#define SERIAL_DATA_9BITS                   0x03
int SERIAL_DATA_BIT_RANGE[2] =              {SERIAL_DATA_7BITS, SERIAL_DATA_9BITS};

#define SERIAL_STOP_1BIT                    0x01
#define SERIAL_STOP_2BIT                    0x02
int SERIAL_STOP_BIT_RANGE[2] =              {SERIAL_STOP_1BIT, SERIAL_STOP_2BIT};

#define SERIAL_PARITY_NONE                  0x01
#define SERIAL_PARITY_ODD                   0x02
#define SERIAL_PARITY_EVEN                  0x03
int SERIAL_PARITY_RANGE[2] =                {SERIAL_PARITY_NONE, SERIAL_PARITY_EVEN};

int NET_ID_RANGE[2] =                       {0, 255};
int NODE_ID_RANGE[2] =                      {0, 255};

#define CONTROLLER_CHANNEL                  1     // This designates what channel the controller will be permanently using
int TRANSMISSION_CHANNEL_RANGE[2] =         {CONTROLLER_CHANNEL, CONTROLLER_CHANNEL};

// Contains all the settings that can be modified in the C50XB RF transceiver
struct Transceiver_Setting_Struct {
  char Transmit_Channel = CONTROLLER_CHANNEL;                 // Each frequency band has 40 channels, numbered 1-40, seperated by 1 MHz each.
  char Transmit_Band = TRANSMISSION_BAND_433MHZ;              // There are 4 available transmission frequency bands, 433, 470, 867, and 915 MHz.
  char Transmission_Rate = WIRELESS_TRANSMISSION_RATE_2400;   // There are 9 RF transmission rates to choose from, the lowest being 2400 bps.
  char Transmission_Power = TRANSMISSION_POWER_20dBm;         // Transmission power ranges from -1 to +20dBm
  char Serial_Baud_Rate = SERIAL_TRANSMISSION_RATE_9600;      // Serial transmission rate ranges from 2400 to 11520 bps. Strandard is 9600 bps.
  char Serial_Data_Bits = SERIAL_DATA_8BITS;                  // Serial data bits can be 7, 8, or 9. Standard is 8 bits.
  char Serial_Stop_Bits = SERIAL_STOP_1BIT;                   // Number of serial stop bits, can be 1 or 2. The standard is 1 bit.
  char Serial_Parity = SERIAL_PARITY_NONE;                    // The serial parity setting, which can be NONE, ODD or EVEN. Standard is NONE.
  unsigned long Net_Id = 0;                                   // The Network ID ranges from 0 to 4294967295, as it consists of 4 bytes of data.
  unsigned int Node_Id = 0;                                   // The Node ID ranges from 0 to 65535, as it consists of 2 bytes of data.
};

#define WRITE_INSTRUCTION_BYTES     sizeof(Transceiver_Setting_Struct)
#define INITIALIZER_1               0xAA
#define INITIALIZER_2               0xFA
#define WRITE_COMMAND               0x03

int Settings_Ranges[WRITE_INSTRUCTION_BYTES][2] = {TRANSMISSION_CHANNEL_RANGE[0], TRANSMISSION_CHANNEL_RANGE[1],
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

// Going through a Transceiver Setting Structure byte array and check if all values fall within range, resetting settings if not
bool Settings_Within_Range(byte Settings[])
{
  bool Check;
  for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
  {
    Check = Within_Range(Settings[N], Settings_Ranges[N][0], Settings_Ranges[N][1]);
    if (Check == false)
    {  
      Serial.println("Out of range, no changes to settings");
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
//    Serial.println("These are the settings sent by Update Transceiver");
//    // Send the same update parameters to the serial port for debugging purposes
//    for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
//    {
//      Serial.println(Array[N]);
//    }
//    Serial.println("The transceiver settings have been updated");
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
      //Serial.println("The EEPROM has been updated");
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
      //Serial.println("The handle settings have been changed");
      Update_Transceiver(Old_Settings);
      EEPROM_Save_Settings(Old_Settings);
      TRANSCEIVER_SERIAL.end();
      delay(50);
      TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Setting_Pointer->Serial_Baud_Rate]);
      delay(50);
      //Serial.println("Starting new serial baud rate");
      //Serial.println(BAUD_RATES[(int)Setting_Pointer->Serial_Baud_Rate]);
      Serial.end();
      delay(50);
      Serial.begin(BAUD_RATES[(int)Setting_Pointer->Serial_Baud_Rate]);
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
      // Send the changed settings to the serial port for debugging
//      byte Value;
//      Value = Settings[N];
//      Serial.print("The EPROM values: ");
//      Serial.println(Value);
    }
//    Serial.println("The EEPROM has been loaded into handle settings");
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
Transceiver_Setting_Struct Handle_Settings_2;
char input;

#define CS_SETTING_PIN                23
#define SET_SETTING_PIN               22
#define SEND_SETTINGS_COMMAND         0x01
#define CS_ON                         LOW
#define CS_OFF                        HIGH
#define SET_ON                        LOW
#define SET_OFF                       HIGH

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
    EEPROM.update(Address+N, Version[N]);
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

#define SERIAL_TIMEOUT            500
#define PAIRING_BUTTON_PIN        28
#define PAIRING_OFF               HIGH

// LED and relay related variables
#define LED_1_PIN                 38
#define LED_2_PIN                 39
#define LED_3_PIN                 40
#define LED_4_PIN                 41
#define LED_ON                    HIGH
#define LED_OFF                   LOW
#define RELAY_LOW_PIN             42
#define RELAY_MID_PIN             43
#define RELAY_HIGH_PIN            44
#define RELAY_CUT_PIN             45
#define RELAY_ON                  LOW
#define RELAY_OFF                 HIGH

struct Button
{
  boolean Status;
  int     Pin_No;
};

Button Pairing_Button = {false, PAIRING_BUTTON_PIN};

void setup() 
{
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
  Serial.begin(9600);
  delay(100);
  Serial.print("Go time!");
  Serial.print('\n');
  
  // Setting up the pairing button, relays, and LEDs
  pinMode(Pairing_Button.Pin_No, INPUT);
  pinMode(LED_1_PIN, OUTPUT); 
  pinMode(LED_2_PIN, OUTPUT); 
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);
  digitalWrite(RELAY_LOW_PIN, RELAY_OFF);
  digitalWrite(RELAY_MID_PIN, RELAY_OFF);
  digitalWrite(RELAY_HIGH_PIN, RELAY_OFF);
  digitalWrite(RELAY_CUT_PIN, RELAY_OFF);
  pinMode(RELAY_LOW_PIN, OUTPUT); 
  pinMode(RELAY_MID_PIN, OUTPUT);  
  pinMode(RELAY_HIGH_PIN, OUTPUT); 
  pinMode(RELAY_CUT_PIN, OUTPUT); 
 
  /* 
  The version string saved in the EEPROM is checked, and if correct
  the Handle Settings structure is updated with the EEPROM values,
  whereafter the transceiver serial baud rate is reset to factory settings
  and the Handle Settings uploaded to the device, and the EEPROM serial baud rate reestablished
  */ 
  char Version_String[VERSION_LEN];
  Version_Load(Version_String);
//  Serial.println(Version_String);
//  Serial.println(Version);
  if (strcmp(Version_String, Version) == 0)
  {
  //Serial.println("Version is correct!");
    EEPROM_Load_Settings((byte*)&Handle_Settings);
    delay(10);
    EEPROM_Load_Pairing_Status();
    delay(50);
  }
  else
  {
    //Serial.println("Version number wrong, resetting settings to standard");
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
  Update_Transceiver((byte*)&Handle_Settings);
  //EEPROM_Save_Settings((byte*)&Handle_Settings);
  delay(50);
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Handle_Settings.Serial_Baud_Rate]);
  delay(50);

  //Handle_Settings_2.Transmission_Power = 7;
  //Handle_Settings_2.Serial_Baud_Rate = 1;    
  Change_Transceiver_Settings((byte*)&Handle_Settings, (byte*)&Handle_Settings_2, &Handle_Settings);
  delay(50);
  
  //Instructions for the transceiver to send it's parameters in SET ON mode
  Serial.println("First debug state");
  TRANSCEIVER_SERIAL.write(INITIALIZER_1);
  TRANSCEIVER_SERIAL.write(INITIALIZER_2);
  TRANSCEIVER_SERIAL.write(SEND_SETTINGS_COMMAND);
  delay(50);
    
  // Read input from the TRANSCEIVER_SERIAL channel and send it to the Serial channel for debugging purposes
  while (TRANSCEIVER_SERIAL.available() > 0)
  { 
    input = TRANSCEIVER_SERIAL.read();
    Serial.print(input, HEX);
    Serial.print(" ");
    Serial.println((int)input);
  }
  
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
  TRANSCEIVER_SERIAL.setTimeout(SERIAL_TIMEOUT);

  // Indicate version number with blinks
  digitalWrite(LED_1_PIN, LED_OFF); 
  digitalWrite(LED_2_PIN, LED_OFF);
  digitalWrite(LED_3_PIN, LED_OFF);
  digitalWrite(LED_4_PIN, LED_OFF);

  // Do some flashy stuff to indicate the version blinks are coming
  digitalWrite(LED_1_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_1_PIN, LED_OFF); 
  digitalWrite(LED_2_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_2_PIN, LED_OFF); 
  digitalWrite(LED_3_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_3_PIN, LED_OFF); 
  digitalWrite(LED_4_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_4_PIN, LED_OFF); 
  digitalWrite(LED_1_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_1_PIN, LED_OFF); 
  digitalWrite(LED_2_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_2_PIN, LED_OFF); 
  digitalWrite(LED_3_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_3_PIN, LED_OFF); 
  digitalWrite(LED_4_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_4_PIN, LED_OFF);
  digitalWrite(LED_1_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_1_PIN, LED_OFF); 
  digitalWrite(LED_2_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_2_PIN, LED_OFF); 
  digitalWrite(LED_3_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_3_PIN, LED_OFF); 
  digitalWrite(LED_4_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_4_PIN, LED_OFF); 
  digitalWrite(LED_1_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_1_PIN, LED_OFF); 
  digitalWrite(LED_2_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_2_PIN, LED_OFF); 
  digitalWrite(LED_3_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_3_PIN, LED_OFF); 
  digitalWrite(LED_4_PIN, LED_ON);
  delay(100);
  digitalWrite(LED_4_PIN, LED_OFF);
  delay(1000);

  /* Will blink with each LED according to the digit in the version number.
  *  LED1 corresponds to the first digit, LED2 to the second, and LED3 to the last.
  *  For 2.18, that would be 2 blinks with LED1, 1 blink with LED2, and 8 blinks with LED3.
  */
  for (int N = 0; N < Version[0] - '0'; N++)
  {
    digitalWrite(LED_1_PIN, LED_ON);
    delay(300);
    digitalWrite(LED_1_PIN, LED_OFF);
    delay(300); 
  }
  for (int N = 0; N < Version[2] - '0'; N++)
  {
    digitalWrite(LED_2_PIN, LED_ON);
    delay(300);
    digitalWrite(LED_2_PIN, LED_OFF);
    delay(300); 
  }
  for (int N = 0; N < Version[3] - '0'; N++)
  {
    digitalWrite(LED_3_PIN, LED_ON);
    delay(300);
    digitalWrite(LED_3_PIN, LED_OFF);
    delay(300); 
  }
  delay(1000);
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

Datagram_Struct Datagram;

// Datagram transmission-related variables
#define DATAGRAM_BYTES                         sizeof(Datagram_Struct)
#define DATAGRAM_SENDING_TIMEOUT               500
Datagram_Struct* Datagram_Pointer =            &Datagram;
byte* Datagram_Byte_Pointer =                  (byte*)Datagram_Pointer;
unsigned long int Datagram_Sending_Timestamp = 0;
unsigned long int New_Timestamp;
unsigned long int Transmission_Succes_Timer;
unsigned long int Shutdown_Timer;
byte Sent_Checksum;
byte Received_Datagram[DATAGRAM_BYTES];
byte Received_Datagram_Sum;

// Micellaneous variables
#define SHUTDOWN_TIMEOUT          2000
bool Blink =                      false;
bool Transmission_Blink =         false;
byte Backup_Heartbeat;
char Serial_Input;

void loop() 
{ 
  // Blink with LED 3 and 4 to indicate loop traversal
  if (Blink == false)
  {
    digitalWrite(LED_3_PIN, LED_ON); 
    digitalWrite(LED_4_PIN, LED_OFF);
    Blink = true;
  }
  else
  {
    digitalWrite(LED_3_PIN, LED_OFF);
    digitalWrite(LED_4_PIN, LED_ON);
    Blink = false;
  }
  
  // Check to see if the controller is paired correctly with a handle, and proceed with transmission if true
  if (Pairing_Succesful == true)
  {
    // Calculate the datagram's checksum and use it to update the datagram
    Datagram.Checksum = 0;
    Sent_Checksum = 0;
    for (int N = 0; N < DATAGRAM_BYTES-1;N++)
    {
      Sent_Checksum += Datagram_Byte_Pointer[N];
    }
    Datagram.Checksum = Sent_Checksum;
    // Only send off a transmission if more than DATAGRAM_SENDING_TIMEOUT time has passed since last datagram sending
    New_Timestamp = millis();
    if (Datagram_Sending_Timestamp + DATAGRAM_SENDING_TIMEOUT < New_Timestamp)
    {
      TRANSCEIVER_SERIAL.write(Version);
      for (int N = 0; N < DATAGRAM_BYTES;N++)
      {
        TRANSCEIVER_SERIAL.write((Datagram_Byte_Pointer[N]));
      }
    TRANSCEIVER_SERIAL.flush();
    }
    
    // Check to see if the correct version string is sent back, and proceed to read DATAGRAM_BYTES bytes into Received Datagram
    if (TRANSCEIVER_SERIAL.find(Version) == true)  
    {
        TRANSCEIVER_SERIAL.readBytes(Received_Datagram, DATAGRAM_BYTES);
        Received_Datagram_Sum = 0;
        for (int N = 0; N < DATAGRAM_BYTES-1;N++)
        {
          Received_Datagram_Sum += Received_Datagram[N];
        }
    // If the checksums are identical and the datagram received was sent with the correct heartbeat then update the controller datagram
        if (Received_Datagram_Sum == Received_Datagram[DATAGRAM_BYTES-1] && Received_Datagram[0] == Datagram.Heartbeat)
        {
          for (int N = 0; N < DATAGRAM_BYTES-1;N++)
          {
             Datagram_Byte_Pointer[N] = Received_Datagram[N];
          }
          Datagram.Heartbeat += 1;
          Transmission_Succes_Timer = millis();

          // Blink with LED 1 and 2 to indicate successful transmission
          if (Transmission_Blink == false)
          {
            digitalWrite(LED_1_PIN, LED_ON);
            digitalWrite(LED_2_PIN, LED_OFF);
            Transmission_Blink = true;
          }
          else
          {
            digitalWrite(LED_1_PIN, LED_OFF);
            digitalWrite(LED_2_PIN, LED_ON);
            Transmission_Blink = false;
          }
        }
     }
     else
     {
       digitalWrite(LED_1_PIN, LED_OFF);
       digitalWrite(LED_2_PIN, LED_OFF);
     }
  }
  else
  {
     digitalWrite(LED_1_PIN, LED_OFF);
     digitalWrite(LED_2_PIN, LED_OFF);
  }

  // Update status of the pairing button
  if (digitalRead(Pairing_Button.Pin_No) == PAIRING_OFF)
  {
    Pairing_Button.Status = false;
  }
  else
  {
    Pairing_Button.Status = true;
  }
  
  // If the pairing button is pressed and no run is active, prevent datagram transmission
  if (Pairing_Button.Status == true && Datagram.Activate_Run == false)
  {
     Datagram_Struct Backup_Datagram;
     Datagram = Backup_Datagram;
  }
  
  /* 
  If no succesful transmission has been completed in SHUTDOWN_TIMEOUT milliseconds
  or the run hasn't been activated yet, revert datagram to default settings,
  except for the heartbeat, which continues
  */ 
  Shutdown_Timer = millis();
  if (Transmission_Succes_Timer + SHUTDOWN_TIMEOUT < Shutdown_Timer || Datagram.Activate_Run == false)
  {
    Datagram_Struct Backup_Datagram;
    Backup_Heartbeat = Datagram.Heartbeat;
    Datagram = Backup_Datagram;
    Datagram.Heartbeat = Backup_Heartbeat;
  }

  // Use the datagram to control the relays
  if (Datagram.Throttle_Low == true)
  {
     digitalWrite(RELAY_LOW_PIN, RELAY_ON);
  }
  else
  {
    digitalWrite(RELAY_LOW_PIN, RELAY_OFF);
  }
  
  if (Datagram.Throttle_Mid == true && Datagram.Throttle_Low == true)
  {
     digitalWrite(RELAY_MID_PIN, RELAY_ON);
  }
  else
  {
    digitalWrite(RELAY_MID_PIN, RELAY_OFF);
  }
  
  if (Datagram.Throttle_High == true && Datagram.Throttle_Mid == true && Datagram.Throttle_Low == true)
  {
     digitalWrite(RELAY_HIGH_PIN, RELAY_ON);
  }
  else
  {
    digitalWrite(RELAY_HIGH_PIN, RELAY_OFF);
  }

  if (Datagram.Cut_Wire == true)
  {
     digitalWrite(RELAY_CUT_PIN, RELAY_ON);
  }
  else
  {
    digitalWrite(RELAY_CUT_PIN, RELAY_OFF);
  }
  
  // Pair transceivers if pairing button is pressed and no run is active
  if (Pairing_Button.Status == true && Datagram.Activate_Run == false)
  {
     Pair_Transceivers((byte*)&Handle_Settings, &Handle_Settings);
     delay(500);
  }

  // Serial input commands for debugging
  while (Serial.available() > 0)
  {
    Serial_Input = Serial.read();
    if (Serial_Input == 'P')
    {
      Pair_Transceivers((byte*)&Handle_Settings, &Handle_Settings);
      delay(500);
    }
    if (Serial_Input == 'D')
    {
      digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
      delay(50);
      Serial.println("Second debug state");
      TRANSCEIVER_SERIAL.write(INITIALIZER_1);
      TRANSCEIVER_SERIAL.write(INITIALIZER_2);
      TRANSCEIVER_SERIAL.write(SEND_SETTINGS_COMMAND);
      delay(1000);
      while (TRANSCEIVER_SERIAL.available() > 0)
      { 
        input = TRANSCEIVER_SERIAL.read();
        Serial.print(input, HEX);
        Serial.print(" ");
        Serial.println(input);
      }
      digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
    }
  }
}

#define INITIALIZER                      "Initializing"
#define INITIALIZER_END                  '*'
#define PAIRING_TIMEOUT_MILLISECONDS     5000
#define WRITE_INSTRUCTION_END            'E'
#define VERSION_END                      'T'
unsigned long int Pairing_Timer;
unsigned long int Pairing_Timestamp;
char Empty_Transceiver;

void Pair_Transceivers(byte Old_Settings[], Transceiver_Setting_Struct* Controller)
{
  // Resetting the transceiver to enable talk with handle
  digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
  delay(50);
  Reset_Transceiver();
  delay(50);     
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)FACTORY_SERIAL_BAUD_RATE]); // Establish a serial connection using the standard baud rate
  delay(50);
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
  delay(50);

  // Make sure the transceiver is empty before proceeding to pair
  while (TRANSCEIVER_SERIAL.available() > 0)
  {
    Empty_Transceiver = TRANSCEIVER_SERIAL.read();
  }
  
   // Send handshake to handle until a response is given or the PAIRING_TIMEOUT_MILLISECONDS milliseconds has passed
  Pairing_Timestamp = millis();
  bool Data_Received = true;
  // Send the initial handshake in two parts, the first part there only to help establish proper communication
  while (TRANSCEIVER_SERIAL.available() == 0)
  {
    TRANSCEIVER_SERIAL.write(INITIALIZER);
    TRANSCEIVER_SERIAL.write(VERSION_END);
    delay(100);
    TRANSCEIVER_SERIAL.write(Version);
    TRANSCEIVER_SERIAL.write(VERSION_END);
    Serial.write('.');
    Pairing_Timer = millis();
    if (Pairing_Timer > Pairing_Timestamp + PAIRING_TIMEOUT_MILLISECONDS)
    {
      Data_Received = false;
      break;
    }
    delay(1000);
  }
  delay(50);
  
  // If data was received, check if the signal was correct, with redundancy.
  bool Handshake = false;
  if (Data_Received == true)
  {
    String Received_Shake = TRANSCEIVER_SERIAL.readStringUntil(VERSION_END);
    if (Received_Shake == Version)
    {
      Handshake = true;
      Serial.println("Handshake true!");
    }
  }
  
  // If correct handshake was sent back, change ID, send the initializer followed by new settings, and update controller's EEPROM+settings.
  if (Data_Received == true && Handshake == true)
  {
    Controller->Net_Id += 1;
    TRANSCEIVER_SERIAL.write(INITIALIZER);
    TRANSCEIVER_SERIAL.write(INITIALIZER_END);
    delay(100);
    for (int N = 0; N < WRITE_INSTRUCTION_BYTES; N++)
    {
       TRANSCEIVER_SERIAL.write(Old_Settings[N]);
    }
    TRANSCEIVER_SERIAL.write(WRITE_INSTRUCTION_END);
    delay(50);
    digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
    delay(50);
    EEPROM_Save_Settings(Old_Settings);
    delay(50);
    Update_Transceiver(Old_Settings);
    delay(50);
    Serial.println("Pairing succesful!");
  }
  else
  {
    digitalWrite(SET_SETTING_PIN, SET_ON); // Turn SET on
    delay(50);
    Update_Transceiver(Old_Settings);
    Serial.println("No data was received! Terminating pairing function");
  }
  
  // Save the pairing status to the EEPROM
  if (Data_Received == true && Handshake == true)
  {
    Pairing_Succesful = true;
    EEPROM_Save_Pairing_Status();
  }
  else
  {
    Pairing_Succesful = false;
    EEPROM_Save_Pairing_Status();
  }
  
  delay(50);
  TRANSCEIVER_SERIAL.end();
  delay(50);
  TRANSCEIVER_SERIAL.begin(BAUD_RATES[(int)Controller->Serial_Baud_Rate]);
  delay(50);
  digitalWrite(SET_SETTING_PIN, SET_OFF); // Turn SET off
}

