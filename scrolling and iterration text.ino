#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define PANEL_RES_X 64      // Width of each panel module
#define PANEL_RES_Y 32      // Height of each panel module
#define PANEL_CHAIN 1       // Number of panels chained together

MatrixPanel_I2S_DMA *dma_display = nullptr;

// Define a palette of vibrant colors
uint16_t colors[] = {
  dma_display->color444(255, 0, 0),    // Red
  dma_display->color444(0, 255, 0),    // Green
  dma_display->color444(0, 0, 255),    // Blue
  dma_display->color444(255, 255, 0),  // Yellow
 // dma_display->color444(0, 255, 255)   // Cyan
};
int numColors = sizeof(colors) / sizeof(colors[0]); // Total number of colors in the palette

// Custom bitmap images for emojis (8x8 pixels)
const uint8_t emojiBitmaps[][8] = {
  {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10100101,
    0b10011001,
    0b01000010,
    0b00111100
  },
  // Add more custom bitmap images for additional emojis as needed
};

// Text messages and associated emoji indices
struct MessageWithEmoji {
  const char *message;
  int emojiIndex;
};

const MessageWithEmoji scrollMessages[] = {
  {"Innovate with passion!", 0},       // Emoji index 0
  {"Embrace challenges and Grow!", 0}, // Emoji index 1
  {"Create,Iterate,Succeed!", 0},        // Emoji index 2
  {"Push Boundries,Think Big!", 0},    // Emoji index 3
  
};
int numMessages = sizeof(scrollMessages) / sizeof(scrollMessages[0]); // Total number of messages

int currentMessageIndex = 0; // Index of the current scrolling message

void drawScrollingText(const char *message, int emojiIndex, int colorIndex) {
  dma_display->fillScreen(dma_display->color444(0, 0, 0)); // Clear screen with black

  dma_display->setTextSize(1);        // Text size 1 == 8 pixels high
  dma_display->setTextWrap(false);    // Don't wrap text

  int xPos = PANEL_RES_X; // Initial x position off the right edge
  int yPos1 = 8;           // Y position for the main scrolling text
  int yPos2 = 16;          // Y position for the emoji
  int yPos3 = 24;          // Y position for the additional text

  int messageLength = strlen(message);
  int emojiWidth = 8;

  // Scroll text from right to left
  for (int i = 0; i < messageLength + emojiWidth + 1; i++) {
    dma_display->fillScreen(dma_display->color444(0, 0, 0)); // Clear screen with black

    // Print the main scrolling text
    dma_display->setCursor(xPos - i * 8, yPos1);
    dma_display->setTextColor(colors[colorIndex % numColors+1]);
    dma_display->print(message);

    // Print emoji on the second line
    if (i >= messageLength && i < messageLength + emojiWidth) {
      // Draw emoji bitmap
      const uint8_t *bitmap = emojiBitmaps[emojiIndex];
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
          if (bitmap[y] & (1 << (7 - x))) {
            dma_display->drawPixel(xPos - (messageLength + emojiWidth - i) * 8 + x, yPos2 + y, colors[colorIndex % numColors]);
          }
        }
      }
    }

    // Print additional text on the third line
    dma_display->setCursor(0, yPos3);
    dma_display->setTextColor(colors[(colorIndex + i) % numColors]);
    dma_display->println("B");
 dma_display->setCursor(9, yPos3);
    dma_display->setTextColor(colors[(colorIndex + i) % numColors]);
    dma_display->println("Limitless!");

    delay(200); // Delay between frames
  }
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
  dma_display->setBrightness8(255);      // Set brightness (0-255)
  dma_display->clearScreen();           // Clear the screen
}

void loop() {
  static unsigned long lastChangeTime = 0;
  static int colorIndex = 0;

  // Change colors and display scrolling text every 5 seconds
  if (millis() - lastChangeTime >= 5000) {
    const char *message = scrollMessages[currentMessageIndex].message;
    int emojiIndex = scrollMessages[currentMessageIndex].emojiIndex;
    drawScrollingText(message, emojiIndex, colorIndex);

    currentMessageIndex = (currentMessageIndex + 1) % numMessages;
    colorIndex = (colorIndex + 2) % numColors;
    lastChangeTime = millis();
  }

  delay(50); // Delay between frames
}
