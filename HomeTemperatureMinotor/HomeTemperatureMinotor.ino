#include <DHT22.h>
#include <stdio.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define DHT22_PIN 7
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t degree[8]  = {0x3,0x3,0x3,0x0,0x0,0x0,0x0};

double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity/100);
  double Td = (b * temp) / (a - temp);
  return Td;
}
//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

LiquidCrystal_I2C lcd(0x27,16,2); 
DHT22 HTM_DHT22(DHT22_PIN);

void setup(void)
{
  Serial.begin(9600);
  Serial.println("DHT22 is starting....");
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degree);
  lcd.home();
  lcd.print("Welcome to HTM!");
}

void loop(void)
{
  DHT22_ERROR_t errorCode;
  
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  delay(5000);
  
  Serial.print("Requesting data...");
  errorCode = HTM_DHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      Serial.print("Got Data ");
      Serial.print(HTM_DHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(HTM_DHT22.getHumidity());
      Serial.println("%");
      
      Serial.print("DewPoint ");
      Serial.println(dewPointFast(HTM_DHT22.getTemperatureC(), HTM_DHT22.getHumidity()));
      // Alternately, with integer formatting which is clumsier but more compact to store and
	  // can be compared reliably for equality:
	  //	  
      char buf[128];
      sprintf(buf, "Integer-only reading: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
                   HTM_DHT22.getTemperatureCInt()/10, abs(HTM_DHT22.getTemperatureCInt()%10),
                   HTM_DHT22.getHumidityInt()/10, HTM_DHT22.getHumidityInt()%10);
      Serial.println(buf);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("HOME ");
      lcd.print(HTM_DHT22.getTemperatureC());
      lcd.printByte(0);
      lcd.print("C");
      
      lcd.setCursor(0,1);
      lcd.print("HOME ");
      lcd.print(Fahrenheit(HTM_DHT22.getTemperatureC()));
      lcd.printByte(0);
      lcd.print("F");
     
      delay(5000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Humidity ");
      lcd.print(HTM_DHT22.getHumidity());
      lcd.print("%");
      
      lcd.setCursor(0,1);
      lcd.print("DewPoint ");
      lcd.print(dewPointFast(HTM_DHT22.getTemperatureC(), HTM_DHT22.getHumidity()));
      lcd.printByte(0);
      lcd.print("C");
     
      
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(HTM_DHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(HTM_DHT22.getHumidity());
      Serial.println("%");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled to quick ");
      break;
  }
}
