#include "contiki.h"
#include "features.h"
#include "rf.h"

// ------------------------------------------

typedef struct {
  int32_t x[TIME_WINDOW_SIZE];
  int32_t y[TIME_WINDOW_SIZE];
  int32_t z[TIME_WINDOW_SIZE];
} jerk_values_t;

typedef struct {
  uint32_t values[TIME_WINDOW_SIZE];
} mag_sq_values_t;

// ------------------------------------------

float feature_entropy(const int32_t window[TIME_WINDOW_SIZE]);
uint32_t feature_energy(const int32_t window[TIME_WINDOW_SIZE]);
uint32_t feature_energy_u(const uint32_t window[TIME_WINDOW_SIZE]);
int32_t feature_mean(const int32_t window[TIME_WINDOW_SIZE]);
int32_t feature_min(const int32_t window[TIME_WINDOW_SIZE]);
uint32_t feature_min_u(const uint32_t window[TIME_WINDOW_SIZE]);
int32_t feature_max(const int32_t window[TIME_WINDOW_SIZE]);
uint32_t feature_max_u(const uint32_t window[TIME_WINDOW_SIZE]);
int32_t feature_iqr(const uint32_t window[TIME_WINDOW_SIZE]);

void transform_jerk(const time_window_t *data, jerk_values_t *result);
void transform_magnitude_sq(const time_window_t *data, mag_sq_values_t *result);
void transform_jerk_magnitude_sq(const jerk_values_t *data, mag_sq_values_t *result);

// ------------------------------------------

void compute_features(const time_window_t *data, float features[])
{
  jerk_values_t jerk;
  mag_sq_values_t mag_sq;
  mag_sq_values_t jerk_mag_sq;

  // compute transforms first

  transform_jerk(data, &jerk);
  transform_magnitude_sq(data, &mag_sq);
  transform_jerk_magnitude_sq(&jerk, &jerk_mag_sq);

  // then extract all the features used by this specific RF classifier

  // tTotalAcc-energy()
  features[0] = feature_energy(data->x);
  features[1] = feature_energy(data->y);
  features[2] = feature_energy(data->z);

  // tTotalAccJerk-energy()
  features[3] = feature_energy(jerk.x);
  features[4] = feature_energy(jerk.y);
  features[5] = feature_energy(jerk.z);

  // tTotalAcc-min()
  features[6] = feature_min(data->x);
  features[7] = feature_min(data->y);
  features[8] = feature_min(data->z);

  // tTotalAccMagSq-min()
  features[9] = feature_min_u(mag_sq.values);

  // tTotalAcc-entropy()
  features[10] = feature_entropy(data->x);
  features[11] = feature_entropy(data->y);
  features[12] = feature_entropy(data->z);

  // tTotalAccMagSq-energy()
  features[13] = feature_energy_u(mag_sq.values);

  // tTotalAccMagSq-max()
  features[14] = feature_max_u(mag_sq.values);

  // tTotalAccJerkMagSq-iqr()
  features[15] = feature_iqr(jerk_mag_sq.values);

  // tTotalAccJerk-mean()
  features[16] = feature_mean(jerk.x);
  features[17] = feature_mean(jerk.y);
  features[18] = feature_mean(jerk.z);
}

// -----------------------------------------------------------

void transform_jerk(const time_window_t *data, jerk_values_t *result)
{
  int i;
  for (i = 0; i < TIME_WINDOW_SIZE - 1; i++) {
    result->x[i] = data->x[i + 1] - data->x[i];
    result->y[i] = data->y[i + 1] - data->y[i];
    result->z[i] = data->z[i + 1] - data->z[i];
  }
  result->x[i] = 0;
  result->y[i] = 0;
  result->z[i] = 0;
}

// -----------------------------------------------------------

void
transform_magnitude_sq(const time_window_t *data, mag_sq_values_t *result)
{
  int i;
  for (i = 0; i < TIME_WINDOW_SIZE; i++) {
    uint32_t x = data->x[i];
    uint32_t y = data->y[i];
    uint32_t z = data->z[i];
    result->values[i] = x * x + y * y + z * z;
  }
}

// -----------------------------------------------------------

void
transform_jerk_magnitude_sq(const jerk_values_t *data, mag_sq_values_t *result)
{
  int i;
  for (i = 0; i < TIME_WINDOW_SIZE; i++) {
    int32_t x = data->x[i];
    int32_t y = data->y[i];
    int32_t z = data->z[i];
    result->values[i] = x * x + y * y + z * z;
  }
}

// -----------------------------------------------------------

//
// Entropy feature computation
//
//
// This is precomputed for specific window sizes - don't use it with other sizes!
//
// The algorithm used to construct this:
//   int i;
//   const int ws = 40;
//   for(i = 1; i < ws; i++)  {
//     float p = (float)i / ws;
//     printf("%f,\n", -p * log2(p));
//   }
//
#if TIME_WINDOW_SIZE == 32
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.156250,
    0.250000,
    0.320160,
    0.375000,
    0.418449,
    0.452820,
    0.479641,
    0.500000,
    0.514709,
    0.524397,
    0.529570,
    0.530639,
    0.527946,
    0.521782,
    0.512395,
    0.500000,
    0.484785,
    0.466917,
    0.446543,
    0.423795,
    0.398792,
    0.371641,
    0.342440,
    0.311278,
    0.278237,
    0.243393,
    0.206814,
    0.168564,
    0.128705,
    0.087290,
    0.044372,
};
#elif TIME_WINDOW_SIZE == 64
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.093750,
    0.156250,
    0.206955,
    0.250000,
    0.287349,
    0.320160,
    0.349196,
    0.375000,
    0.397979,
    0.418449,
    0.436660,
    0.452820,
    0.467098,
    0.479641,
    0.490573,
    0.500000,
    0.508018,
    0.514709,
    0.520147,
    0.524397,
    0.527521,
    0.529570,
    0.530595,
    0.530639,
    0.529744,
    0.527946,
    0.525282,
    0.521782,
    0.517477,
    0.512395,
    0.506561,
    0.500000,
    0.492734,
    0.484785,
    0.476173,
    0.466917,
    0.457035,
    0.446543,
    0.435458,
    0.423795,
    0.411568,
    0.398792,
    0.385478,
    0.371641,
    0.357291,
    0.342440,
    0.327099,
    0.311278,
    0.294988,
    0.278237,
    0.261036,
    0.243393,
    0.225316,
    0.206814,
    0.187894,
    0.168564,
    0.148832,
    0.128705,
    0.108188,
    0.087290,
    0.066016,
    0.044372,
    0.022365,
};
#elif TIME_WINDOW_SIZE == 128
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.054688,
    0.093750,
    0.126915,
    0.156250,
    0.182737,
    0.206955,
    0.229285,
    0.250000,
    0.269302,
    0.287349,
    0.304268,
    0.320160,
    0.335112,
    0.349196,
    0.362474,
    0.375000,
    0.386821,
    0.397979,
    0.408511,
    0.418449,
    0.427823,
    0.436660,
    0.444985,
    0.452820,
    0.460184,
    0.467098,
    0.473578,
    0.479641,
    0.485301,
    0.490573,
    0.495468,
    0.500000,
    0.504180,
    0.508018,
    0.511524,
    0.514709,
    0.517580,
    0.520147,
    0.522417,
    0.524397,
    0.526097,
    0.527521,
    0.528677,
    0.529570,
    0.530208,
    0.530595,
    0.530737,
    0.530639,
    0.530306,
    0.529744,
    0.528956,
    0.527946,
    0.526720,
    0.525282,
    0.523634,
    0.521782,
    0.519729,
    0.517477,
    0.515032,
    0.512395,
    0.509570,
    0.506561,
    0.503370,
    0.500000,
    0.496454,
    0.492734,
    0.488844,
    0.484785,
    0.480561,
    0.476173,
    0.471625,
    0.466917,
    0.462053,
    0.457035,
    0.451864,
    0.446543,
    0.441074,
    0.435458,
    0.429698,
    0.423795,
    0.417751,
    0.411568,
    0.405248,
    0.398792,
    0.392201,
    0.385478,
    0.378624,
    0.371641,
    0.364529,
    0.357291,
    0.349927,
    0.342440,
    0.334830,
    0.327099,
    0.319248,
    0.311278,
    0.303191,
    0.294988,
    0.286669,
    0.278237,
    0.269693,
    0.261036,
    0.252269,
    0.243393,
    0.234408,
    0.225316,
    0.216117,
    0.206814,
    0.197406,
    0.187894,
    0.178280,
    0.168564,
    0.158748,
    0.148832,
    0.138818,
    0.128705,
    0.118495,
    0.108188,
    0.097787,
    0.087290,
    0.076700,
    0.066016,
    0.055240,
    0.044372,
    0.033414,
    0.022365,
    0.011227,
};
#endif


static inline float calc_entropy(unsigned c)
{
    return entropy_lookup_table[c];
}

float feature_entropy(const int32_t window[TIME_WINDOW_SIZE])
{
  float entropy = 0.0;
  uint8_t stats[256] = {0};
  int j;
  for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
    int v = window[j] + 128;
    stats[v]++;
  }
  for (j = 0; j < 256; ++j) {
    entropy += calc_entropy(stats[j]);
  }
  return entropy;
}

// ------------------------------------------

uint32_t feature_energy(const int32_t window[TIME_WINDOW_SIZE])
{
  int j;
  uint32_t result = 0;
  for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
    result += window[j] * window[j];
  }
  result /= TIME_WINDOW_SIZE;
  return result;
}

// ------------------------------------------

uint32_t feature_energy_u(const uint32_t window[TIME_WINDOW_SIZE])
{
  int j;
  uint32_t result = 0;
  for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
    result += window[j] * window[j];
  }
  result /= TIME_WINDOW_SIZE;
  return result;
}

// ------------------------------------------

int32_t feature_mean(const int32_t window[TIME_WINDOW_SIZE])
{
  int j;
  int32_t result = 0;
  for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
    result += window[j];
  }
  result /= TIME_WINDOW_SIZE;
  return result;
}

// ------------------------------------------

int32_t feature_min(const int32_t window[TIME_WINDOW_SIZE])
{
  int j;
  int32_t result = window[0];
  for (j = 1; j < TIME_WINDOW_SIZE; ++j) {
    if (window[j] < result) {
      result = window[j];
    }
  }
  return result;
}

// ------------------------------------------

uint32_t feature_min_u(const uint32_t window[TIME_WINDOW_SIZE])
{
  int j;
  uint32_t result = window[0];
  for (j = 1; j < TIME_WINDOW_SIZE; ++j) {
    if (window[j] < result) {
      result = window[j];
    }
  }
  return result;
}

// ------------------------------------------

int32_t feature_max(const int32_t window[TIME_WINDOW_SIZE])
{
  int j;
  int32_t result = window[0];
  for (j = 1; j < TIME_WINDOW_SIZE; ++j) {
    if (window[j] > result) {
      result = window[j];
    }
  }
  return result;
}

// ------------------------------------------

uint32_t feature_max_u(const uint32_t window[TIME_WINDOW_SIZE])
{
  int j;
  uint32_t result = window[0];
  for (j = 1; j < TIME_WINDOW_SIZE; ++j) {
    if (window[j] > result) {
      result = window[j];
    }
  }
  return result;
}

// ------------------------------------------

int32_t feature_iqr(const uint32_t window[TIME_WINDOW_SIZE])
{
  int j;
  int32_t q25 = 0, q75 = 0;
  // put all data in bins and walk through the bins while the nth element is found
  int8_t stats[256] = {0};

  for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
    int v = window[j] + 128;
    stats[v]++;
  }
  int c = 0;
  bool q25_set = false;
  bool q75_set = false;
  for (j = 0; j < 256; ++j) {
    if (stats[j]) {
      c += stats[j];
      if (c >= TIME_WINDOW_SIZE / 4 &&  !q25_set) {
        q25_set = true;
        q25 = j - 128;
      }
      if (c >= TIME_WINDOW_SIZE * 3 / 4 && !q75_set) {
        q75_set = true;
        q75 = j - 128;
        break;
      }
    }
  }
  return q75 - q25;
}

// ------------------------------------------
