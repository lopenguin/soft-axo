#include "axStack.h"

// CONSTANT DEFINITIONS:            // higher time bound to end observing
const int SIZE =            2;                        // how many values to average
const int NUM_PHASES_A =    6;                        // number of phases in first quaternion
const int NUM_PHASES =      6;                        // number of gait phases per cycle
const float phase_prop[] =  {0.006863623,             // phase proportions
                             0.11058171,
                             0.23520932,
                             0.39913204,
                             0.5220804,
                             0.72646666
                            };
const int DELAY =            20;

float last_time = 0;
float last_a = 0;
float last_d = 0;
float slope_polarity_a = 0;
float slope_polarity_d = 0;
float calculated = 0;

int phase = 1;
int prev_phase = 0;
int total_phases = 1;
float phase_times[NUM_PHASES];
float timer;
bool full = false;

int counter = 0;

double times[] = {102625.2, 102646.4, 102667.6, 102688.9, 102710.3, 102731.7, 102753.1, 102774.5, 102795.9, 102817.3, 102838.7, 102860.1, 102881.5, 102902.9, 102924.3, 102945.7, 102967.1, 102988.5, 103009.7, 103030.9, 103052.1, 103073.3, 103094.5, 103115.6, 103136.6, 103157.7, 103178.7, 103199.7, 103220.7, 103241.8, 103262.8, 103283.8, 103304.9, 103326.1, 103347.2, 103368.2, 103389.2, 103410.4, 103431.6, 103452.8, 103473.9, 103494.9, 103516.0, 103537.0, 103558.0, 103579.0, 103600.1, 103621.1, 103642.1, 103663.1, 103684.1, 103705.1, 103726.2, 103747.3, 103768.4, 103789.4, 103810.4, 103831.4, 103852.4, 103873.5, 103894.6, 103915.8, 103937.0, 103958.1, 103979.2, 104000.4, 104021.5, 104042.7, 104063.9, 104085.1, 104106.3, 104127.5, 104148.6};
double a[] = {704.3, 707.8, 722.19995, 743.0, 759.8, 772.3, 781.3, 786.2, 783.9, 780.9, 777.2, 773.5, 769.2, 764.9, 761.8, 760.3, 759.89996, 760.6, 762.8, 765.60004, 770.2, 778.5, 787.4, 795.1, 800.2, 802.39996, 806.5, 810.4, 811.7, 809.1, 809.7, 811.9, 814.80005, 819.80005, 826.8, 833.7, 838.5, 840.0, 838.7, 835.39996, 832.0, 828.2, 824.4, 820.9, 817.8, 815.0, 812.2, 809.2, 806.2, 803.0, 799.6, 796.3, 793.1, 790.2, 787.5, 785.0, 782.80005, 780.39996, 778.0, 775.39996, 772.3, 768.89996, 765.0, 760.7, 756.60004, 752.30005, 747.9, 743.0, 737.9, 732.9, 728.5, 725.3, 722.7};
double d[] = { -61.6, -63.699997, -73.299995, -82.0, -92.4, -102.3, -104.8, -105.1, -111.100006, -112.1, -108.3, -107.299995, -107.7, -106.9, -104.6, -101.8, -101.899994, -104.1, -104.9, -102.3, -96.7, -90.8, -89.100006, -90.8, -93.1, -97.2, -105.3, -116.4, -122.100006, -123.0, -111.299995, -91.0, -81.9, -83.0, -69.6, -61.3, -59.6, -51.399998, -46.100002, -44.2, -43.3, -41.5, -38.899998, -35.399998, -32.3, -29.4, -27.0, -24.800001, -23.0, -22.800001, -22.5, -21.5, -21.3, -21.5, -22.1, -22.9, -23.7, -23.800001, -24.1, -24.800001, -25.3, -26.0, -27.0, -28.699999, -30.9, -33.100002, -36.5, -41.0, -47.2, -52.0, -58.0, -62.5, -68.1};
axStack a_slopes;
axStack d_slopes;

void setup() {

  Serial.begin(9600);


  a_slopes.initialize(SIZE, 0);
  d_slopes.initialize(SIZE, 0);

  last_a = a[0];
  last_d = d[0];
  last_time = times[0];
  timer = millis();
}

void loop() {
  if (counter < 72) {
    counter++;
  } else {
    counter = 0;
  }

  predict(times[counter], a[counter], d[counter]);



  Serial.print((double)counter / 72);
  Serial.print(" ");

  Serial.print(calculated);
  Serial.print(" ");
  Serial.println((double)counter / 72 - calculated);
  delay(1);
}


void predict(float t, float a, float d) {
  updatePhases();
  timing(phase);

  float curr_slope_a = (a - last_a) / (t - last_time);
  float curr_slope_d = (d - last_d) / (t - last_time);
  slope_polarity_a = a_slopes.get_average();
  a_slopes.enqueue(curr_slope_a);
  slope_polarity_d = d_slopes.get_average();
  d_slopes.enqueue(curr_slope_d);

  last_a = a;
  last_d = d;
  last_time = t;
  delay(DELAY);
}

void updatePhases() {
  if (phase < 3 && (slope_polarity_a < 0) != (a_slopes.get_average() < 0)) {
    total_phases++;
  }
  if (phase == 3 && (slope_polarity_d < 0) && (d_slopes.get_average() > 0) && millis() - timer >= 0 * DELAY) {
    total_phases++;
  }
  if (phase == 4 && (slope_polarity_a > 0) && (a_slopes.get_average() < 0)) {
    total_phases++;
  }
  if (phase == 5 && ((slope_polarity_d > 0) && (d_slopes.get_average() < 0))) {
    total_phases++;
  }

  if (phase == 6 && (slope_polarity_a < 0) && (a_slopes.get_average() > 0)) {
    total_phases++;
  }
  phase = total_phases % NUM_PHASES;
  if (phase == 0) {
    phase = NUM_PHASES;
  }
  if (phase != prev_phase) {
    phase_times[phase - 1] = millis() - timer;
    timer = millis();
  }
  prev_phase = phase;
}

void timing(int p) {
  if (!isFull(phase_times)) {
    calculated = phase_prop[p - 1];
  } else {
    float start = 0;
    float total = 0;
    for (int i = 0; i < p - 1; i++) {
      start += phase_times[(i + 1) % NUM_PHASES];
    }
    for (int i = 0; i < NUM_PHASES; i++) {
      total += phase_times[i];
    }
    calculated = ((millis() - timer + start) / total);
  }
}

bool isFull(float arr[]) {
  int arr_length = sizeof(arr) / sizeof(arr[0]);
  if (!full) {
    for (int i = 0; i < arr_length; i++) {
      if (arr[i] <= 0) {
        return false;
      }
    }
    full = true;
  }
  return true;
}
