/************************************************************************
Gibson Moseley - MicrosdCard.hpp

Used tutorial: https://randomnerdtutorials.com/esp32-microsd-card-arduino/
*************************************************************************/

#ifndef MICROCARD_HPP
#define MICROCARD_HPP

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5

SPIClass spi = SPIClass(VSPI);

void setupMicroSDcard() {
  //Serial.begin(115200);
  spi.begin(SCK, MISO, MOSI, CS);

  if (!SD.begin(CS, spi, 80000000)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

#endif