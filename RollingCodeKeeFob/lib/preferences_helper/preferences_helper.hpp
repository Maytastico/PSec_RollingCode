#ifndef PREFERENCES_HELPER_HPP
#define PREFERENCES_HELPER_HPP

#include <Preferences.h>
#include <stdint.h> // Standard C library required for fixed-width integers like uint32_t

// ============================================================================
// GLOBAL VARIABLES (Declaration Only)
// ============================================================================
// The 'extern' keyword tells the compiler that these variables are allocated 
// in memory elsewhere (inside preferences_helper.cpp). This prevents the 
// linker from throwing "multiple definition" errors when this header is 
// included in both main.cpp and other source files.

/**
 * @brief Global object handling the Non-Volatile Storage (NVS) flash operations.
 */
extern Preferences authData;

/**
 * @brief Global runtime variable holding the current rolling code counter.
 */
extern uint32_t counter;

/**
 * @brief The key string used to look up the counter value inside the NVS flash.
 */
extern const char* COUNTER_KEY;


// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

/**
 * @brief Opens the NVS storage, checks for an existing counter, and loads it.
 *        If no counter exists (first boot), it initializes the flash with the default value.
 */
void initialize_storage();

/**
 * @brief Validates and safely updates the rolling code counter in the flash memory.
 * 
 * @param newCounter The fresh counter value received over the air from the KeyFob.
 * @return boolean Returns 'true' if the counter was higher and successfully updated.
 *                 Returns 'false' if the counter was old/replayed (security alert).
 */
boolean update_counter(uint32_t newCounter);

/**
 * @brief Commits a fresh rolling counter value directly to the non-volatile storage (NVS).
 * 
 * This function bypasses security and sequence validation to immediately overwrite 
 * the persistent counter tracking. It opens a write-enabled session within the 
 * designated namespace, flashes the new 32-bit unsigned integer to the storage hardware, 
 * issues a confirmation log, and safely closes the flash interface session.
 * 
 * @note Primarily used by the transmitting device (KeyFob) to permanently remember its own 
 *       incremented state, or during initial provisioning.
 * 
 * @param newCounter The raw 32-bit rolling code value to be saved into flash memory.
 */
void store_counter(uint32_t newCounter);

#endif // PREFERENCES_HELPER_HPP