// hx711.c

#include "hx711_driver.h"
#define MEDIAN_WINDOW 5  // Odd number for a simple median
int32_t readings[MEDIAN_WINDOW] = {0};  // Buffer to store the last 'n' readings
#define AVERAGE_WINDOW 10  // Size of the running average window
int32_t readings[AVERAGE_WINDOW] = {0};  // Buffer to store the last 'n' readings
int readIndex = 0;  // Current index in the buffer


// Initialize HX711
void HX711_Init(HX711* hx711, GPIO_TypeDef* clk_port, uint16_t clk_pin, GPIO_TypeDef* data_port, uint16_t data_pin) {
    // Initialize GPIOs
    hx711->clk_port = clk_port;
    hx711->clk_pin = clk_pin;
    hx711->data_port = data_port;
    hx711->data_pin = data_pin;

    // Initialize the HX711
    HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);

    // Default values for offset and scale
    hx711->offset = 0;
    hx711->scale = 1;

    // Set the default gain
    HX711_SetGain(hx711, GAIN_128);
}

// Function to add a new reading and calculate the running average
float HX711_GetAverageWeight(HX711* hx711, uint8_t times) {
    int32_t sum = 0;
    for (int i = 0; i < times; i++) {
        int32_t newReading = HX711_ReadRaw(hx711);

        // Add new reading and subtract the oldest one from the sum
        sum -= readings[readIndex];
        readings[readIndex] = newReading;
        sum += readings[readIndex];

        // Move to the next position in the buffer
        readIndex = (readIndex + 1) % AVERAGE_WINDOW;
    }
    // Calculate and return the average of the buffer's values
    return (float)(sum / AVERAGE_WINDOW) / hx711->scale;
}

// Helper function to find the median of the buffer
int32_t findMedian(int32_t arr[], int n) {
    // Sort the array (use any sorting algorithm you prefer)
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);  // Swap elements

    // Return the median
    if (n % 2 != 0)  // Odd number of elements
        return arr[n / 2];
    return (arr[(n - 1) / 2] + arr[n / 2]) / 2;  // Even number of elements
}

// Function to add a new reading and calculate the median weight
float HX711_GetMedianWeight(HX711* hx711) {
    for (int i = 0; i < MEDIAN_WINDOW; i++) {
        readings[i] = HX711_ReadRaw(hx711);
    }
    int32_t medianValue = findMedian(readings, MEDIAN_WINDOW);
    return (float)(medianValue - hx711->offset) / hx711->scale;
}


// Read raw value from HX711
int32_t HX711_ReadRaw(HX711* hx711) {
    int32_t count = 0;
    while (HAL_GPIO_ReadPin(hx711->data_port, hx711->data_pin)); // Wait for the chip to be ready

    for (uint8_t i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
        count = count << 1;
        HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(hx711->data_port, hx711->data_pin)) {
            count++;
        }
    }

    // Setting the gain for the next reading
    for (int i = 0; i < hx711->gain; i++) {
        HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
    }

    count ^= 0x800000; // Convert to 32-bit signed

    return count;
}

// Tare functionality for the HX711
void HX711_Tare(HX711* hx711, uint8_t times) {
    int32_t sum = 0;
    for (int i = 0; i < times; i++) {
        sum += HX711_ReadRaw(hx711);
    }
    hx711->offset = sum / times;
}

// Set the scale value
void HX711_SetScale(HX711* hx711, float scale) {
    hx711->scale = scale;
}

// Get weight
float HX711_GetWeight(HX711* hx711, uint8_t times) {
    int32_t sum = 0;
    for (int i = 0; i < times; i++) {
        sum += HX711_ReadRaw(hx711);
    }
    sum = sum / times - hx711->offset;
    return (float)sum / hx711->scale;
}

// Set the gain for the next reading
void HX711_SetGain(HX711* hx711, HX711_Gain gain) {
    hx711->gain = gain;
}

// Power down the HX711
void HX711_PowerDown(HX711* hx711) {
    HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
}

// Power up the HX711
void HX711_PowerUp(HX711* hx711) {
    HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
}

