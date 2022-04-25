#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <EEPROM.h>


PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String UID;
long int updateBalance;
String walletValue;
unsigned int keypadInput = 1000;
//unsigned carddebitInput = 2000;
void setup() {
  Serial.begin(115200);
  Serial.println("NDEF Writer");
  nfc.begin();
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void tagReader() {
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    Serial.println();
    Serial.println(tag.getUidString());
    UID = tag.getUidString();
    UID.replace(" ", "");
    Serial.println(UID);
    if (tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();
      //int recordCount = message.getRecord();
      NdefRecord record = message.getRecord(0);
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);
      String cardUnit = "";
      for (int c = 3; c < payloadLength; c++) {
        cardUnit += (char)payload[c];
      }
      Serial.println(cardUnit.toInt());
      cardTopup(cardUnit.toInt());
    }
  }

  delay(2000);
}

void tagWriter(String cardValue) {
  Serial.println("\nPlace a formatted Mifare Classic or Ultralight NFC tag on the reader.");
  if (nfc.tagPresent()) {
    NdefMessage message = NdefMessage();
    message.addTextRecord(cardValue);
    //message.addUriRecord("http://NTS.nodexihub.com");
    bool success = nfc.write(message);
    if (success) {
      Serial.println("Success. Try reading this tag with your phone.");
    } else {
      Serial.println("Write failed.");
    }
  }
  delay(2000);
}

void store_data(int addr, String value) {
  int value_size = value.length();
  for (int i = 0; i < value_size; i++) {
    EEPROM.write(addr + i, value[i]);
  }
  EEPROM.write(addr + value_size, '\0');
}

String read_data(int addr) {
  char data[100];
  int len = 0;
  char k = EEPROM.read(addr);
  while (k != '\0') {
    k = EEPROM.read(addr + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return data;
}

void readerTopup() {
  if (Serial.available()) {
    updateBalance = Serial.readString().toInt() + read_data(0).toInt();
    store_data(0, String(updateBalance));
    Serial.println(updateBalance);
  }
}

void cardTopup(int cardUnit) {
  updateBalance = read_data(0).toInt() - keypadInput;
  Serial.println(updateBalance);
  store_data(0, String(updateBalance));
  int cardUpdate = cardUnit + keypadInput;
  tagWriter(String(cardUpdate));
}
void cardDebit(int cardUnit) {
  int cardUpdate = cardUnit - keypadInput;
  tagWriter(String(cardUpdate));
  updateBalance = read_data(0).toInt() + keypadInput;
  store_data(0, String(updateBalance));
}

void loop() {
  if (Serial.available()) {
    char mode = Serial.read();
    switch (mode) {
      case '1':
        Serial.println("Working");
        tagReader();
        break;
      case '0':
        Serial.println("perfectly");
        break;
    }
  }
}
