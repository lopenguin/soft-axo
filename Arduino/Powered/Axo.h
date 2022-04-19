/*
Sensor and motor carrier class for ankle exoskeleton. Designed to abstract
essential but lower-level functions away from Powered.ino.

Lorenzo Shaikewitz, 4/19/2022
*/

class Axo {
public:
    Axo()

    /* SENSOR FUNCTIONS */
    // starts up IMU, FSR, load cell, AND motor pots.
    // Freezes if any sensor fails to initialize. Call once in setup().
    bool beginSensors();
    // reads IMU, FSR, and load cell (NO motor pots). Uses metro timers, so
    // call repeatedly in loop (no delays needed)
    bool readSensors();

private:

};
