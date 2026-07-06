#include "preferences_helper.hpp"

// Define the variables here (exactly once in the whole project)
Preferences authData;
uint32_t counter = 0;
const char* AUTHDATA_NAMESPACE = "authData";
const char* COUNTER_KEY = "counter";

void initialize_storage() {
    authData.begin(AUTHDATA_NAMESPACE, false);
    
    if (!authData.isKey(COUNTER_KEY)) {
        log_i("First boot: Initializing persistent storage with starting value: %u", counter);
        authData.putUInt(COUNTER_KEY, counter);  
    } else {
        counter = authData.getUInt(COUNTER_KEY, 0);
        log_i("Stored value found. Current counter loaded: %u", counter);
    }
    
    log_i("Persistent Storage successfully initialized.");
    authData.end();
}

boolean update_counter(uint32_t newCounter) {
    authData.begin(AUTHDATA_NAMESPACE, false);

    if (newCounter <= counter && newCounter <= counter + 256) {
        log_e("SECURITY ALERT: Received counter is outdated or replayed! Storage NOT updated. Current: %u, Received: %u", counter, newCounter);
        
        authData.end(); 
        return false;
    }

    // Accept the new counter since it is higher
    counter = newCounter;
    authData.putUInt(COUNTER_KEY, counter);
    log_i("Counter successfully updated in flash. New counter value: %u", counter);

    // Close the storage session normally
    authData.end();
    return true;
}

void reset_storage(){
    authData.begin(AUTHDATA_NAMESPACE, false);
    authData.putUInt(COUNTER_KEY, 0);
    log_i("Default Data written");
    authData.end(); 
}