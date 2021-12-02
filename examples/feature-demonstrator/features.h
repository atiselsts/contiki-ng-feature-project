#ifndef FEATURES_H
#define FEATURES_H

#include <stdint.h>

#define TIME_WINDOW_SIZE 32

typedef struct {
  int32_t x[TIME_WINDOW_SIZE];
  int32_t y[TIME_WINDOW_SIZE];
  int32_t z[TIME_WINDOW_SIZE];
  uint8_t length;
} time_window_t;


#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define NUM_AXIS 3

void compute_features(const time_window_t *data, float result[]);

#endif /* FEATURES_H */
