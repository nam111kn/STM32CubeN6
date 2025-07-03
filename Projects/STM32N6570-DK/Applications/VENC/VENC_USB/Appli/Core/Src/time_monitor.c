#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32n6xx_hal.h"

// Define the system clock frequency (in Hz)
#define SYSTEM_CLOCK_FREQ 600000000 // 600 MHz


// Define the period for printing stats (in seconds)
#define PRINT_STATS_PERIOD 5

// Structure to hold the statistics
typedef struct {
    uint64_t maxDeltaTime;
    uint64_t minDeltaTime;
    uint64_t totalDeltaTime;
    uint64_t numCalls;
    uint64_t startTime;
    uint64_t lastCallTime;
} DeltaTimeStats;

static DeltaTimeStats stats = {0, UINT32_MAX, 0, 0, 0, 0};

// Variables to track overflow
static volatile uint32_t overflowCount = 0;
static volatile uint32_t lastCycleCount = 0;
static uint32_t initDone=0;
static uint32_t system_clock_frequency;

// Function to initialize the DWT cycle counter
void initCycleCounter() {
  
  if (initDone == 0)
  {
    system_clock_frequency = HAL_RCC_GetCpuClockFreq();
    printf ("Monitoring : Cpu : %d MHZ\n", system_clock_frequency / 1000 /1000);
    // Enable the DWT unit
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }

    // Reset the cycle counter
    DWT->CYCCNT = 0;

    // Enable the cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    initDone=1;
  }
}

// Function to handle the overflow of the cycle counter
void handleCycleCounterOverflow() {
    uint32_t currentCycleCount = DWT->CYCCNT;
    if (currentCycleCount < lastCycleCount) {
        overflowCount++;
    }
    lastCycleCount = currentCycleCount;
}

// Function to get the current time in milliseconds (mock function)
uint64_t  getCurrentTimeMillis() {
    initCycleCounter();
    handleCycleCounterOverflow();
    uint64_t totalCycles = ((uint64_t)overflowCount << 32) | DWT->CYCCNT;
    return (totalCycles / (system_clock_frequency / 1000));
}

// Function to print the statistics
void printStats() {
    printf("======================================\n");
    printf("Max Delta Time          : %10" PRIu64 " ms\n", stats.maxDeltaTime);
    printf("Min Delta Time          : %10" PRIu64 " ms\n", stats.minDeltaTime);
    printf("Average Delta Time      : %10" PRIu64 " ms\n", stats.totalDeltaTime / stats.numCalls);
    printf("Total Time              : %10" PRIu64 " ms\n", getCurrentTimeMillis() - stats.startTime);
    printf("Number of Calls         : %10" PRIu64 "   \n", stats.numCalls);
    printf("Number of Calls/Seconds : %10" PRIu64 "   \n", 1000*stats.numCalls/(getCurrentTimeMillis() - stats.startTime));
}

// Periodic function to be called
void timeMonitor() {
    uint64_t currentTime = getCurrentTimeMillis();
    uint64_t deltaTime = currentTime - stats.lastCallTime;

    // Update statistics
    if (deltaTime > stats.maxDeltaTime) {
        stats.maxDeltaTime = deltaTime;
    }
    if (deltaTime < stats.minDeltaTime) {
        stats.minDeltaTime = deltaTime;
    }
    stats.totalDeltaTime += deltaTime;
    stats.numCalls++;

    // Update the last call time
    stats.lastCallTime = currentTime;

    // Print stats every PRINT_STATS_PERIOD seconds
    if ((currentTime - stats.startTime) >= (PRINT_STATS_PERIOD * 1000)) {
        printStats();
        // Reset the start time for the next period
        stats.startTime = currentTime;
        stats.maxDeltaTime = 0;
        stats.minDeltaTime = UINT32_MAX;
        stats.numCalls = 0;
    }
}

// Initialization function to set up the timer and start the periodic calls
void testTimeMonitor() {
    // Initialize the start time and last call time
    stats.startTime = getCurrentTimeMillis();
    stats.lastCallTime = stats.startTime;

    // Set up the timer to call periodicFunction periodically
    // This is a mock setup; actual implementation will depend on the hardware timer
    while (true) {
        timeMonitor();
        // Simulate a delay for the next call (e.g., 100 ms)
        // In actual implementation, this would be handled by a hardware timer interrupt
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }
}

