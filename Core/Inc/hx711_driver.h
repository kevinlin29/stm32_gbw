// hx711.h

#ifndef HX711_H
#define HX711_H

#include "stm32f4xx_hal.h"

// Define pins and ports for HX711 data and clock
#define HX711_CLK_PORT GPIOA
#define HX711_CLK_PIN  GPIO_PIN_0
#define HX711_DATA_PORT GPIOA
#define HX711_DATA_PIN  GPIO_PIN_1

// HX711 structure
typedef struct {
    GPIO_TypeDef* clk_port;   // Clock port
    uint16_t clk_pin;         // Clock pin
    GPIO_TypeDef* data_port;  // Data port
    uint16_t data_pin;        // Data pin
    int offset;               // Offset for tare weight
    float scale;              // Scale factor for weight conversion
} HX711;

// Gain selection for the HX711
typedef enum {
    GAIN_128 = 1,
    GAIN_64  = 3,
    GAIN_32  = 2
} HX711_Gain;

// Function prototypes
void HX711_Init(HX711* hx711, GPIO_TypeDef* clk_port, uint16_t clk_pin, GPIO_TypeDef* data_port, uint16_t data_pin);
float HX711_GetAverageWeight(HX711* hx711, uint8_t times);
float HX711_GetMedianWeight(HX711* hx711);
int32_t HX711_ReadRaw(HX711* hx711);
void HX711_Tare(HX711* hx711, uint8_t times);
void HX711_SetScale(HX711* hx711, float scale);
float HX711_GetWeight(HX711* hx711, uint8_t times);
void HX711_SetGain(HX711* hx711, HX711_Gain gain);
void HX711_PowerDown(HX711* hx711);
void HX711_PowerUp(HX711* hx711);

#endif // HX711_H
