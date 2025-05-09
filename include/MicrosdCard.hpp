/************************************************************************
Gibson Moseley - MicrosdCard.hpp

This is built to communicate with the MicroSD card connected to the esp32, purpose is to record all relevent information
particuallry from sensors and flow data.

Used tutorial: https://randomnerdtutorials.com/esp32-microsd-card-arduino/
*************************************************************************/

#ifndef MICROCARD_HPP
#define MICROCARD_HPP

//Library Includes
#include "FS.h"
#include "SD.h"
#include "SPI.h"

//Function declerations
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);
void readCSVFilesToWebsite(String filename);
void writeBioreactorInfo(String routineName, String timeRoutine, String flowrate, String pumpStatus);
void setupMicroSDcard();

//List all files and directories from given directory
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

//Create new directory on SD card
void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

//Remove directory from SD card
void removeDir(fs::FS &fs, const char * path){
  //Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    //Serial.println("Dir removed");
  } else {
    //Serial.println("rmdir failed");
  }
}

//Read and print contents of file from SD card
void readFile(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    //Serial.println("Failed to open file for reading");
    return;
  }

 // Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

//Write message to a file, overwriting existing contents
void writeFile(fs::FS &fs, const char * path, const char * message){
  //Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    //Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    //Serial.println("File written");
  } else {
    //Serial.println("Write failed");
  }
  file.close();
}

//Append message to existing file on SD card
void appendFile(fs::FS &fs, const char * path, const char * message){
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    //Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      //Serial.println("Message appended");
  } else {
    //Serial.println("Append failed");
  }
  file.close();
}

//Rename a file on the SD card
void renameFile(fs::FS &fs, const char * path1, const char * path2){
  //Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    //Serial.println("File renamed");
  } else {
    //Serial.println("Rename failed");
  }
}

//Delete file from SD card
void deleteFile(fs::FS &fs, const char * path){
  //Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    //Serial.println("File deleted");
  } else {
    //Serial.println("Delete failed");
  }
}

//Test file read/write performance
void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    //Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    //Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
   // Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  //Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

//Read CSV file and send each line to the website
void readCSVFilesToWebsite(String filename) {
  File file = SD.open(("/" + filename + ".csv").c_str());
  if (!file) {
      Serial.println("Failed to open file");
      return;
  }

  ws.textAll("csvdata;start");

  while (file.available()) {
      String line = file.readStringUntil('\n');
      ws.textAll("csvdata;" + line);
      delay(10); //Throttle to avoid overloading
  }

  ws.textAll("csvdata;done");
  file.close();
}

//Function to make new routine files and append all necessary data to them
void writeBioreactorInfo(String routineName, String timeRoutine, String flowrate, String pumpStatus) {
  String routineNameFile = "/" + routineName + ".csv";
  File file = SD.open(routineNameFile.c_str());

  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, routineNameFile.c_str(), "Routine Name, Time (seconds), Flow (ml/min), Temp (F), Shear Stress (nPa), Pump (On/Off)\r\n");
  }
  else {
    //Do nothing
    //Serial.println("File already exists");  
  }
  file.close();
  String message = routineName + ", " + timeRoutine + ", " + flowrate + ", " + pumpStatus +"\r\n";
  appendFile(SD, routineNameFile.c_str(), message.c_str());
}

//Initial setup function to mount the SD card and verify/test
void setupMicroSDcard() {
  //Mount SD card
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  //Say if none is connected
  if(cardType == CARD_NONE){
   Serial.println("No SD card attached");
    return;
  }

  //Say card type
  //Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    //Serial.println("MMC");
  } else if(cardType == CARD_SD){
    //Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    // Serial.println("SDHC");
  } else {
    // Serial.println("UNKNOWN");
  }

  //Calculates size
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  // Serial.printf("SD Card Size: %lluMB\n", cardSize);

  //Makes a test csv file to ensure SD card is working
  File file = SD.open("/test.csv");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/test.csv", "Hello, this means SD is working. Delete if you want to reconfirm\r\n");
  }
  else {
    // Serial.println("File already exists");  
  }

  //Stop reading SD card
  file.close();
}

#endif