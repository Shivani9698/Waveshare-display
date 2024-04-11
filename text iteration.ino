#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32      // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1       // Total number of panels chained one to another

MatrixPanel_I2S_DMA *dma_display = nullptr;

// Define a list of colors for cycling
uint16_t colors[] = {
  dma_display->color444(15, 0, 0),    // Red
  dma_display->color444(0, 15, 0),    // Green
  dma_display->color444(0, 0, 15),    // Blue
  dma_display->color444(15, 15, 0),   // Yellow
  dma_display->color444(0, 15, 15)    // Cyan
};
int numColors = sizeof(colors) / sizeof(colors[0]); // Number of colors in the palette

// Smiley face emoji bitmap (8x8 pixels)
const uint8_t smileyBitmap[] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10000001,
  0b10100101,
  0b10011001,
  0b01000010,
  0b00111100
};

bool isFirstIteration = true; // Flag to track the iteration

void drawTextWithEmoji(int colorIndex) {
  dma_display->fillScreen(dma_display->color444(0, 0, 0)); // Clear screen with black

  dma_display->setTextSize(1);        // Text size 1 == 8 pixels high
  dma_display->setTextWrap(false);    // Don't wrap text

  if (isFirstIteration) {
    // Display "LEARN & GROW with" on the first iteration
    dma_display->setCursor(0, 8);
    dma_display->setTextColor(colors[(colorIndex + 1) % numColors]);
    dma_display->println("LEARN");
    dma_display->setCursor(32, 8);
    dma_display->setTextColor(colors[(colorIndex + 2) % numColors]);
    dma_display->println("&");
     dma_display->setCursor(40, 8);
    dma_display->setTextColor(colors[(colorIndex + 3) % numColors]);
    dma_display->println("GROW");
    dma_display->setCursor(20, 16);
    dma_display->setTextColor(colors[(colorIndex + 4) % numColors]);
    dma_display->println("with");
  } else {
    // Display "Intelitek Edu Sol.!" on the second iteration
    dma_display->setCursor(0, 8);
    dma_display->setTextColor(colors[(colorIndex + 5) % numColors]);
    dma_display->println("Intelitek");
     dma_display->setCursor(0, 16);
    dma_display->setTextColor(colors[(colorIndex + 6) % numColors]);
    dma_display->println("Edu");
     dma_display->setCursor(24, 16);
    dma_display->setTextColor(colors[(colorIndex + 7) % numColors]);
    dma_display->println("Sol.!");


    // Display smiley face emoji after "Intelitek Edu Sol.!"
    dma_display->drawBitmap(54, 16, smileyBitmap, 8, 8, colors[(colorIndex + 8) % numColors]);
  }

  isFirstIteration = !isFirstIteration; // Toggle the iteration flag
}

void setup() {
  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // Module width
    PANEL_RES_Y,   // Module height
    PANEL_CHAIN    // Chain length
  );

  mxconfig.gpio.e = 32;                // GPIO pin connected to display's 'E' pin
  mxconfig.clkphase = false;           // Clock phase
  mxconfig.driver = HUB75_I2S_CFG::FM6124; // Display driver type

  // Initialize display
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();                // Begin display operation
  dma_display->setBrightness8(90);      // Set brightness (0-255)
  dma_display->clearScreen();           // Clear the screen
}

void loop() {
  static unsigned long lastColorChangeTime = 0;
  static int colorIndex = 0;

  // Change colors every 2 seconds
  if (millis() - lastColorChangeTime >= 2000) {
    drawTextWithEmoji(colorIndex);
    colorIndex = (colorIndex + 1) % numColors;
    lastColorChangeTime = millis();
  }

  delay(20); // Delay between frames
}
