#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define dataPin       6

#define matrixWidth   8
#define matrixHeight  5

AudioPlayMemory          playMem1;       //xy=244,193
AudioPlaySdWav           playSdWav1;     //xy=248,114
AudioSynthWaveform       waveform1;      //xy=283,271
AudioMixer4              mixer1;         //xy=496,142
AudioAnalyzeFFT1024      fft1024_1;      //xy=671,239
AudioOutputI2S           i2s1;           //xy=698,147
AudioConnection          patchCord1(playMem1, 0, mixer1, 2);
AudioConnection          patchCord2(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord3(playSdWav1, 1, mixer1, 1);
AudioConnection          patchCord4(waveform1, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioConnection          patchCord7(mixer1, fft1024_1);
AudioControlSGTL5000     sgtl5000_1;     //xy=475,361


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
                              //matrix width
                              8,
                              //matrix height
                              5,
                              //data pin
                              6,
                              NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                              NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                              NEO_GRB + NEO_KHZ800);


#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

float band[8];


// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

// arrays holding color config data for scrolling text and for graphics drawings
const uint16_t textColors[] = { matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };
const uint16_t drawingColors[] = { GREEN, BLUE, CYAN, WHITE, RED, YELLOW, MAGENTA };

String msg     = "GR Matrix";      // text to scroll on display
int    x_size  = matrix.width();   // overall display matrix width
int    y_size  = matrix.height();  // overall display matrix height
int    msglen  = msg.length();     // number of characters in scrolling message
int    x_pos   = matrix.width();   // track where we are in the scrolling message display
int    pass    = 0;                // track how many times message has scrolled


void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(textColors[0]);
  Serial.println("NeoMatrix pixel grid test");
  Serial.println((String)"Matrix Pixel Width x Height: " + x_size + " x " + y_size);
  AudioMemory(10);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      //delay(500);
    }

  }
  //playSdWav1.play("OriginalMix.wav");

  //playSdWav1.play("tunes/BirdyDub.wav");
  delay(1000);
  playSdWav1.play("ZenDub.WAV");
  //playSdWav1.play("Beef.WAV");
  //playSdWav1.play("HalfmoonDub.WAV");
  //playSdWav1.play("tunes2/OriginalMix.WAV");

}
/*
  int fileNumber = 0;
  const char * filenames[4] = {
  "SDTEST1.WAV", "SDTEST2.WAV", "SDTEST3.WAV", "SDTEST4.WAV"
  };
*/
int noteNumber = 0;
const float noteFrequency[12] = {
  220.00,  // A3
  233.08,  // A#3
  246.94,  // B3
  261.63,  // C4
  277.18,  // C#4
  293.66,  // D4
  311.13,  // D#4
  329.63,  // E4
  349.23,  // F4
  369.99,  // F#4
  392.00,  // G4
  415.30   // G#4
};


void loop() {
  if (fft1024_1.available()) {

    Serial.print("FFT: ");
    for (int i = 0; i < 8; i++) { // 0-25  -->  DC to 1.25 kHz
      float n = fft1024_1.read(i);
      //printNumber((1- n)* 1000);
      band[0] = fft1024_1.read(0);      //0  //0
      band[1] = fft1024_1.read(1);      //0  //1
      band[2] = fft1024_1.read(2, 3);   //1  //2
      band[3] = fft1024_1.read(3, 5);   //2  //3
      band[4] = fft1024_1.read(4, 6);   //3  //4
      band[5] = fft1024_1.read(6, 14);  //6  //6
      band[6] = fft1024_1.read(14, 32); //12 //10
      band[7] = fft1024_1.read(32, 45); //23 //18
      printNumber(band[i]);
    }
    Serial.println();
  }



  if (fft1024_1.available()) {
    // each time new FFT data is available
    // print to the Arduino Serial Monitor

    Serial.print("FFT: ");
    /*
      printNumber(fft1024_1.read(0));
      printNumber(fft1024_1.read(1));
      printNumber(fft1024_1.read(2,3));
      printNumber(fft1024_1.read(4,6));
      printNumber(fft1024_1.read(7,10));
      printNumber(fft1024_1.read(11,15));
      printNumber(fft1024_1.read(16,22));
      printNumber(fft1024_1.read(23,32));
      printNumber(fft1024_1.read(33,46));
      printNumber(fft1024_1.read(47,66));
      printNumber(fft1024_1.read(67,93));
      printNumber(fft1024_1.read(94,131));
      printNumber(fft1024_1.read(132,184));
      printNumber(fft1024_1.read(185,257));
      printNumber(fft1024_1.read(258,359));
      printNumber(fft1024_1.read(360,511));
    */

  }
  matrix.fillScreen(0);       // clear display
  matrix.setCursor(x_pos, 0); // set cursor at current horizontal scrolling position and top of display (y=0)
  audio_visualizer();
  delay(100);
  //testPixels();       // light up individual pixels at random colors
  //delay(1000);
  //drawLines();        // show how to draw lines between x,y coordinates
  //delay(1000);
  //drawRectangles();   // show how to draw rectangles
  //delay(1000);
  //drawCircles();      // show how to draw circles
  //delay(1000);
  //drawTriangles();    // show how to draw triangles
  //delay(1000);
  //drawBars();         // draw some lines resembling an audio spectrum analyzer
  //delay(1000);
  matrix.show();
  //delay(100);
}

void audio_visualizer() {
  matrix.fillScreen(0);

  for (int i = 0; i < x_size; i++) {
    if (band[i] < .01) {
      matrix.drawPixel(0, 0, BLACK);
    }
    else if (band[i] < .025) {
      matrix.drawPixel(i, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[i] < .075) {
      matrix.drawPixel(i, 0, RED);
      matrix.drawPixel(i, 1, MAGENTA);
    }
    else if (band[i] < .1) {
      matrix.drawPixel(i, 0, RED);
      matrix.drawPixel(i, 1, MAGENTA);
      matrix.drawPixel(i, 2, YELLOW);
    }
    else if (band[i] < .150) {
      matrix.drawPixel(i, 0, RED);
      matrix.drawPixel(i, 1, MAGENTA);
      matrix.drawPixel(i, 2, YELLOW);
      matrix.drawPixel(i, 3, GREEN);
    }
    else {
      matrix.drawPixel(i, 0, RED);
      matrix.drawPixel(i, 1, MAGENTA);
      matrix.drawPixel(i, 2, YELLOW);
      matrix.drawPixel(i, 3, GREEN);
      matrix.drawPixel(i, 4, CYAN);
    }
  }
  /*
    if (band[0] < .01) {
      matrix.drawPixel(0, 0, BLACK);
    }
    else if (band[0] < .025) {
      matrix.drawPixel(0, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[0] < .075) {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
    }
    else if (band[0] < .1) {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
      matrix.drawPixel(0, 2, YELLOW);
    }
    else if (band[0] < .150) {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
      matrix.drawPixel(0, 2, YELLOW);
      matrix.drawPixel(0, 3, GREEN);
    }
    else {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
      matrix.drawPixel(0, 2, YELLOW);
      matrix.drawPixel(0, 3, GREEN);
      matrix.drawPixel(0, 4, CYAN);
    }

    //band 1
    if (band[1] < .01) {
      matrix.drawPixel(1, 0, BLACK);
    }
    else if (band[1] < .025) {
      matrix.drawPixel(1, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[1] < .075) {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[0] < .1) {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      matrix.drawPixel(1, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[0] < .150) {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      matrix.drawPixel(1, 2, YELLOW);
      matrix.drawPixel(1, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      matrix.drawPixel(1, 2, YELLOW);
      matrix.drawPixel(1, 3, GREEN);
      matrix.drawPixel(1, 4, CYAN);
    }

    //band 2
    if (band[2] < .01) {
      matrix.drawPixel(2, 0, BLACK);
    }
    else if (band[2] < .025) {
      matrix.drawPixel(2, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[2] < .075) {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[2] < .1) {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      matrix.drawPixel(2, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[2] < .150) {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      matrix.drawPixel(2, 2, YELLOW);
      matrix.drawPixel(1, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      matrix.drawPixel(2, 2, YELLOW);
      matrix.drawPixel(2, 3, GREEN);
      matrix.drawPixel(2, 4, CYAN);
    }

    //band 3
    if (band[3] < .01) {
      matrix.drawPixel(3, 0, BLACK);
    }
    else if (band[3] < .025) {
      matrix.drawPixel(3, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[3] < .075) {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[3] < .1) {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      matrix.drawPixel(3, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[3] < .150) {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      matrix.drawPixel(3, 2, YELLOW);
      matrix.drawPixel(3, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      matrix.drawPixel(3, 2, YELLOW);
      matrix.drawPixel(3, 3, GREEN);
      matrix.drawPixel(3, 4, CYAN);
    }

    //band 4
    if (band[4] < .01) {
      matrix.drawPixel(4, 0, BLACK);
    }
    else if (band[4] < .025) {
      matrix.drawPixel(4, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[4] < .075) {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[4] < .1) {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      matrix.drawPixel(4, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[4] < .150) {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      matrix.drawPixel(4, 2, YELLOW);
      matrix.drawPixel(4, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      matrix.drawPixel(4, 2, YELLOW);
      matrix.drawPixel(4, 3, GREEN);
      matrix.drawPixel(4, 4, CYAN);
    }


    //band 5
    if (band[5] < .01) {
      matrix.drawPixel(5, 0, BLACK);
    }
    else if (band[5] < .025) {
      matrix.drawPixel(5, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[5] < .075) {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[5] < .1) {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      matrix.drawPixel(5, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[5] < .150) {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      matrix.drawPixel(5, 2, YELLOW);
      matrix.drawPixel(5, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      matrix.drawPixel(5, 2, YELLOW);
      matrix.drawPixel(5, 3, GREEN);
      matrix.drawPixel(5, 4, CYAN);
    }

    //band 6
    if (band[6] < .01) {
      matrix.drawPixel(6, 0, BLACK);
    }
    else if (band[6] < .025) {
      matrix.drawPixel(6, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[6] < .075) {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[6] < .1) {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      matrix.drawPixel(6, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[6] < .150) {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      matrix.drawPixel(6, 2, YELLOW);
      matrix.drawPixel(6, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      matrix.drawPixel(6, 2, YELLOW);
      matrix.drawPixel(6, 3, GREEN);
      matrix.drawPixel(6, 4, CYAN);
    }

    //band 7
    if (band[7] < .01) {
      matrix.drawPixel(0, 0, BLACK);
    }
    else if (band[7] < .025) {
      matrix.drawPixel(7, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[7] < .075) {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[7] < .1) {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      matrix.drawPixel(7, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[7] < .150) {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      matrix.drawPixel(7, 2, YELLOW);
      matrix.drawPixel(7, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      matrix.drawPixel(7, 2, YELLOW);
      matrix.drawPixel(7, 3, GREEN);
      matrix.drawPixel(7, 4, CYAN);
    }
  */
  /*  if (band[0] < .01) {
      matrix.drawPixel(0, 0, BLACK);
    }
    else if (band[0] < .055) {
      matrix.drawPixel(0, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[0] < .155) {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
    }
    else if (band[0] < .195) {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
      matrix.drawPixel(0, 2, YELLOW);
    }
    else if (band[0] < .250) {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
      matrix.drawPixel(0, 2, YELLOW);
      matrix.drawPixel(0, 3, GREEN);
    }
    else {
      matrix.drawPixel(0, 0, RED);
      matrix.drawPixel(0, 1, MAGENTA);
      matrix.drawPixel(0, 2, YELLOW);
      matrix.drawPixel(0, 3, GREEN);
      matrix.drawPixel(0, 4, CYAN);
    }

    //band 1
    if (band[1] < .01) {
      matrix.drawPixel(1, 0, BLACK);
    }
    else if (band[1] < .055) {
      matrix.drawPixel(1, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[1] < .155) {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[0] < .195) {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      matrix.drawPixel(1, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[0] < .250) {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      matrix.drawPixel(1, 2, YELLOW);
      matrix.drawPixel(1, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(1, 0, RED);
      matrix.drawPixel(1, 1, MAGENTA);
      matrix.drawPixel(1, 2, YELLOW);
      matrix.drawPixel(1, 3, GREEN);
      matrix.drawPixel(1, 4, CYAN);
    }

    //band 2
    if (band[2] < .01) {
      matrix.drawPixel(2, 0, BLACK);
    }
    else if (band[2] < .055) {
      matrix.drawPixel(2, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[2] < .155) {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[2] < .195) {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      matrix.drawPixel(2, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[2] < .250) {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      matrix.drawPixel(2, 2, YELLOW);
      matrix.drawPixel(1, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(2, 0, RED);
      matrix.drawPixel(2, 1, MAGENTA);
      matrix.drawPixel(2, 2, YELLOW);
      matrix.drawPixel(2, 3, GREEN);
      matrix.drawPixel(2, 4, CYAN);
    }

    //band 3
    if (band[3] < .01) {
      matrix.drawPixel(3, 0, BLACK);
    }
    else if (band[3] < .055) {
      matrix.drawPixel(3, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[3] < .155) {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[3] < .195) {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      matrix.drawPixel(3, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[3] < .250) {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      matrix.drawPixel(3, 2, YELLOW);
      matrix.drawPixel(3, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(3, 0, RED);
      matrix.drawPixel(3, 1, MAGENTA);
      matrix.drawPixel(3, 2, YELLOW);
      matrix.drawPixel(3, 3, GREEN);
      matrix.drawPixel(3, 4, CYAN);
    }

    //band 4
    if (band[4] < .01) {
      matrix.drawPixel(4, 0, BLACK);
    }
    else if (band[4] < .055) {
      matrix.drawPixel(4, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[4] < .155) {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[4] < .195) {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      matrix.drawPixel(4, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[4] < .250) {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      matrix.drawPixel(4, 2, YELLOW);
      matrix.drawPixel(4, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(4, 0, RED);
      matrix.drawPixel(4, 1, MAGENTA);
      matrix.drawPixel(4, 2, YELLOW);
      matrix.drawPixel(4, 3, GREEN);
      matrix.drawPixel(4, 4, CYAN);
    }


    //band 5
    if (band[5] < .01) {
      matrix.drawPixel(5, 0, BLACK);
    }
    else if (band[5] < .055) {
      matrix.drawPixel(5, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[5] < .155) {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[5] < .195) {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      matrix.drawPixel(5, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[5] < .250) {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      matrix.drawPixel(5, 2, YELLOW);
      matrix.drawPixel(5, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(5, 0, RED);
      matrix.drawPixel(5, 1, MAGENTA);
      matrix.drawPixel(5, 2, YELLOW);
      matrix.drawPixel(5, 3, GREEN);
      matrix.drawPixel(5, 4, CYAN);
    }

    //band 6
    if (band[6] < .01) {
      matrix.drawPixel(6, 0, BLACK);
    }
    else if (band[6] < .055) {
      matrix.drawPixel(6, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[6] < .155) {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[6] < .195) {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      matrix.drawPixel(6, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[6] < .250) {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      matrix.drawPixel(6, 2, YELLOW);
      matrix.drawPixel(6, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(6, 0, RED);
      matrix.drawPixel(6, 1, MAGENTA);
      matrix.drawPixel(6, 2, YELLOW);
      matrix.drawPixel(6, 3, GREEN);
      matrix.drawPixel(6, 4, CYAN);
    }

    //band 7
    if (band[7] < .01) {
      matrix.drawPixel(0, 0, BLACK);
    }
    else if (band[7] < .055) {
      matrix.drawPixel(7, 0, RED);
      //matrix.drawPixel(0,1, RED);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[7] < .155) {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      //matrix.drawPixel(0,2, MAGENTA);
      //matrix.drawPixel(0,3, MAGENTA);
      //matrix.drawPixel(0,4, MAGENTA);
    }
    else if (band[7] < .195) {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      matrix.drawPixel(7, 2, YELLOW);
      //matrix.drawPixel(0,3, RED);
      //matrix.drawPixel(0,4, RED);
    }
    else if (band[7] < .250) {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      matrix.drawPixel(7, 2, YELLOW);
      matrix.drawPixel(7, 3, GREEN);
      //matrix.drawPixel(1,4, CYAN);
    }

    else {
      matrix.drawPixel(7, 0, RED);
      matrix.drawPixel(7, 1, MAGENTA);
      matrix.drawPixel(7, 2, YELLOW);
      matrix.drawPixel(7, 3, GREEN);
      matrix.drawPixel(7, 4, CYAN);
    }*/
  matrix.show();

}


void testPixels() {
  int rnd;
  matrix.fillScreen(0);
  for (int i = 0; i < x_size; i++) {
    for (int j = 0; j < y_size; j++) {
      rnd = random(0, 7);
      matrix.drawPixel(i, j, drawingColors[rnd]);
      matrix.show();
      //delay(75);
    }
  }
}

void drawLines() {
  matrix.fillScreen(0);
  matrix.drawLine(0, 0, 7, 7, BLUE);
  matrix.show();
  //delay(200);
  matrix.drawLine(7, 0, 0, 7, YELLOW);
  matrix.show();
  //delay(200);
  matrix.drawLine(4, 0, 4, 7, RED);
  matrix.show();
  //delay(200);
  matrix.drawLine(0, 3, 7, 3, WHITE);
  matrix.show();
  //delay(200);
}

void drawRectangles() {
  matrix.fillScreen(0);
  for (int i = 0; i < matrix.width() / 2; i++) {
    matrix.drawRect(i, i, matrix.width() - i * 2 , matrix.width() - i * 2 , drawingColors[i]);
    matrix.show();
    //delay(200);
  }
}

void drawCircles() {
  matrix.fillScreen(0);
  for (int i = 0; i < matrix.width() / 2 - 1; i++) {
    matrix.drawCircle(matrix.width() / 2 - 1, matrix.width() / 2 - 1, i + 1, drawingColors[i]);
    matrix.show();
    //delay(200);
  }
}

void drawTriangles() {
  matrix.fillScreen(0);
  matrix.drawTriangle(0, 3, 3, 6, 6, 3, GREEN);
  matrix.show();
  //delay(200);
  matrix.drawTriangle(6, 1, 6, 7, 3, 4, MAGENTA);
  matrix.show();
  //delay(200);
  matrix.drawTriangle(0, 7, 7, 0, 0, 0, YELLOW);
  matrix.show();
  //delay(200);
}

void drawBars() {

  int rnd;
  for (int i = 0; i < 10; i++) {
    matrix.fillScreen(0);
    rnd = random(1, 7);
    matrix.drawLine(0, 7, 0, rnd, GREEN);
    matrix.drawPixel(0, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(1, 7, 1, rnd, GREEN);
    matrix.drawPixel(1, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(2, 7, 2, rnd, GREEN);
    matrix.drawPixel(2, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(3, 7, 3, rnd, GREEN);
    matrix.drawPixel(3, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(4, 7, 4, rnd, GREEN);
    matrix.drawPixel(4, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(5, 7, 5, rnd, GREEN);
    matrix.drawPixel(5, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(6, 7, 6, rnd, GREEN);
    matrix.drawPixel(6, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(7, 7, 7, rnd, GREEN);
    matrix.drawPixel(7, rnd - 1, RED);
    matrix.show();
    //delay(300);
  }
}

void printNumber(float n) {

  if (n >= 0.024) {
    Serial.print(n, 3);
    Serial.print(" ");
  } else {
    Serial.print("   -  "); // don't print "0.00"
  }
}

/*
  if (n > 0.25) {
  Serial.print("***** ");
  } else if (n > 0.18) {
  Serial.print(" ***  ");
  } else if (n > 0.06) {
  Serial.print("  *   ");
  } else if (n > 0.005) {
  Serial.print("  .   ");
  }
*/
