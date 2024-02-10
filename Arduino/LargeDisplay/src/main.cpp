#include <Arduino.h>

int RS = 30;      // RS (A0) signal connected to Arduino digital pin 30
int WR = 31;      // /WR signal connected to Arduino digital pin 31
int RD = 32;      // /RD signal connected to Arduino digital pin 32
int RES = 33;     // /RESET signal connected to Arduino digital pin 33
int CS = 34;      // /CS signal tied to GND or driven LOW pin 34

int COLS_PER_ROW = 40;

void comm_out(unsigned char c)
{
  digitalWrite(RS, HIGH);      // A0 = H = command
  PORTA = c;
  digitalWrite(WR, LOW);
  digitalWrite(WR, HIGH);
}
void data_out(unsigned char d)
{
  digitalWrite(RS, LOW);     // A0 = L = data
  PORTA = d;
  digitalWrite(WR, LOW);
  digitalWrite(WR, HIGH);
}

void set_cursor(unsigned short address) {
    comm_out(0x46);
    data_out((unsigned char) (address & 0x00ff));
    data_out((unsigned char) (address >> 8));
    comm_out(0x42);
}

// SB1: text
void set_rowcol(int row, int col) {
  int offset = col + (row * COLS_PER_ROW);
  set_cursor((unsigned short) offset);
}

// SB2: graphics, starts at address 0x04b0
void set_rowcol_gr(int row, int col) {
  unsigned short offset = 0x04b0 + col + (row * COLS_PER_ROW);
  set_cursor((unsigned short) offset);
}


void clr()
{
  unsigned int i;
  set_cursor(0);
  for(i=0;i<10800;i++)
  {
    data_out(0x00);
  }
}

void scroll(unsigned char s) {
  comm_out(0x5A);   // Sets horizontal scroll position
  data_out(s);   // Horizontal Pixel Scroll              0x801b, 0x00   --> HPS  =   0  
}

void string_out(const char *s) {
  while(*s) {
    data_out(*(s++));
  }
}

void disp() {
  set_rowcol(10,15);
  string_out("CASTLE RUNNER");

  // cursor movement down
  comm_out(0x4f);

  set_rowcol_gr(100,15);
  data_out(0x0F);
  data_out(0x07);
  data_out(0x03);
  data_out(0x01);
  data_out(0xA8);
  data_out(0xF8);
  data_out(0x88);
  data_out(0xA8);
  data_out(0xA8);
  data_out(0x8F);
  data_out(0x80);
  data_out(0x80);
  data_out(0xA1);
  data_out(0xA2);
  data_out(0x82);
  data_out(0xFF);

  set_rowcol_gr(100,16);
  data_out(0x80);
  data_out(0x80);
  data_out(0x80);
  data_out(0x80);
  data_out(0x95);
  data_out(0x9F);
  data_out(0x91);
  data_out(0x95);
  data_out(0x95);
  data_out(0xF1);
  data_out(0x01);
  data_out(0x01);
  data_out(0x85);
  data_out(0x45);
  data_out(0x41);
  data_out(0xFF);

  // cursor movement right
  comm_out(0x4c);
}

void __disp()
{
	unsigned long i;
    char c = 'A';
    comm_out(0x46); // Set cursor address
    data_out(0xb0);
    data_out(0x04);
    comm_out(0x42); // Write to memory
    
    for(i=0;i<9600;i++)
    {
        data_out(c);
        c++;
        if (c == 'Z'+1) c = 'A';
    }
}

void setup() {
  Serial.begin(115200);

  DDRC = 0xFF;         //set PORTC (control signals) as output
  PORTC = 0x00;
  DDRA = 0xFF;         //set PORTA (data signals) as output
  PORTA = 0x00;
  pinMode(CS,OUTPUT);
  digitalWrite(CS, LOW);
  digitalWrite(RD, HIGH);
  digitalWrite(WR, HIGH);
  digitalWrite(RES, LOW);
  delay(10);
  digitalWrite(RES, HIGH);
  delay(100);

  // Initilization Process
  comm_out(0x40);   // Initializes device and display
  data_out(0x30);   // Memory Configuration Register        0x8000, 0x03   --> Config: Origin Comp = 1, Single Panel, Char Height = 8 pixels, CGROM selected
  data_out(0x87);   // Horizontal Character Size            0x8001, 0x07   --> HCS  =   8
  data_out(0x07);   // Vertical Character Size              0x8002, 0x07   --> VCS  =   8
  data_out(0x27);   // Character Bytes Per Row              0x8003, 0x27   --> C/R  =  40 chars
  data_out(0x50);   // Total Character Bytes Per Row        0x8004, 0x50   --> TC/R =  79
  data_out(0xEF);   // Frame Height Register                0x8005, 0xEF   --> FHR  = 240 pixels
  data_out(0x28);   // Horizontal Address Range 0           0x8006
  data_out(0x00);   // Horizontal Address Range 1           0x8007, 0x0028 --> HAR  =  40 addresses

  comm_out(0x44);   // Sets screen block start addresses and sizes
  data_out(0x00);   // Screen Block 1 Start Address 0       0x800b
  data_out(0x00);   // Screen Block 1 Start Address 1       0x800c, 0x0000 --> SB1A = 0x0000
  data_out(0xEF);   // Screen Block 1 Size                  0x800d, 0xef   --> SB1S = 240 pixels
  data_out(0xB0);   // Screen Block 2 Start Address 0       0x800e
  data_out(0x04);   // Screen Block 2 Start Address 1       0x800f, 0x04b0 --> SB2A = 0x04b0
  data_out(0xEF);   // Screen Block 2 Size                  0x8010, 0xef   --> SB2S = 240 pixels
  data_out(0x00);   // Screen Block 3 Start Address 0       0x8011
  data_out(0x00);   // Screen Block 3 Start Address 1       0x8012, 0x0000 --> SB3A = 0x0000
  data_out(0x00);   // Screen Block 4 Start Address 0       0x8013
  data_out(0x00);   // Screen Block 4 Start Address 1       0x8014, 0x0000 --> SB4A = 0x0000
  
  comm_out(0x5A);   // Sets horizontal scroll position
  data_out(0x00);   // Horizontal Pixel Scroll              0x801b, 0x00   --> HPS  =   0  
  
  comm_out(0x5B);   // Sets display overlay format
  data_out(0x00);   // Overlay                              0x8018, 0x00   --> two layers, SB3 is text, SB1 is text, layers are OR'd
  
  comm_out(0x5D);   // Sets Cursor Type
  data_out(0x04);   // Cursor Width                         0x8015, 0x04   --> CW   =   4
  data_out(0x86);   // Cursor Height                        0x8016, 0x86   --> CH   =   6, mode = block
  
  comm_out(0x4C);   // Sets direction of cursor movement,   0x8017, 0x4c   --> bits 00 --> Direction Right

  comm_out(0x58);   // Enable / disable display
  data_out(0x01);   // Display Enable                       0x8009, 0x01   --> DE   = 1

  comm_out(0x59);   // Display attributes
  data_out(0x55);   // Display Attribute                    0x800a, 0x55   --> SAD3 ON, No flash; SAD2 ON, No Flash, SAD1 ON, No Flash, Cursor ON, No Flash

  clr(); // Clear display
  delay(10);
  disp(); // Displays hex table image
}

void loop()
{
  unsigned char i = 0;
  unsigned char dir = 1;
  

  while(0) {
    scroll(i);

    i += dir;

    if (i == 7) {
      dir = -1;
    }
    if (i == 0) {
      dir = 1;
    }

    delay(100);
  }
}
