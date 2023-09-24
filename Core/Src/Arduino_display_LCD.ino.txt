#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
uint8_t a;
int k=0;
char temp;
char str[8];
char str2[4];
int j=3;
int i=0;
LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySerial(2,3);//rx,tx
void setup()
{
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  mySerial.begin(9600);
  lcd.setCursor(1,0);
  lcd.print("LED RED :");
}
void loop()
{
  if (mySerial.available())
  { 
    temp = (char)mySerial.read();
    Serial.print(temp);
    str[i]=temp;
    i++;
    if (i==4)
    {
       i=0;
       lcd.setCursor(10,0);
        lcd.print(str[1]);
       lcd.print(str[2]);
       lcd.print(str[3]);
       lcd.setCursor(1,1);
       if (str[0]=='1')
       {
         lcd.print("LED GREEN ON ");
       }
       else  {
          lcd.print("LED GREEN OFF");
       }
    }
  }
}