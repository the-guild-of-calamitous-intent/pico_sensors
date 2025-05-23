////////////////////////////////////////////////
//  The MIT License (MIT)
//  Copyright (c) 2022 Kevin Walchko
//  see LICENSE for full details
////////////////////////////////////////////////
#pragma once

#include <picolibc.h>
#include <stdbool.h>
#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

constexpr int LSM6DSOX_ADDRESS     = 0x6A;
constexpr int LSM6DSOX_ADDRESS_ALT = 0x6B;

// The accelerometer/gyroscope data rate
constexpr uint8_t RATE_SHUTDOWN = 0x00;
constexpr uint8_t RATE_104_HZ   = 0x40;
constexpr uint8_t RATE_208_HZ   = 0x50;
constexpr uint8_t RATE_416_HZ   = 0x60;
constexpr uint8_t RATE_833_HZ   = 0x70;
constexpr uint8_t RATE_1660_HZ  = 0x80;
constexpr uint8_t RATE_3330_HZ  = 0x90;
constexpr uint8_t RATE_6660_HZ  = 0xA0;

// The accelerometer data range
constexpr uint8_t ACCEL_RANGE_2_G  = (0x00 << 2);
constexpr uint8_t ACCEL_RANGE_16_G = (0x01 << 2);
constexpr uint8_t ACCEL_RANGE_4_G  = (0x02 << 2);
constexpr uint8_t ACCEL_RANGE_8_G  = (0x03 << 2);

// The gyro data range
constexpr uint8_t GYRO_RANGE_125_DPS  = (0x01 << 1);
constexpr uint8_t GYRO_RANGE_250_DPS  = (0x00 << 1);
constexpr uint8_t GYRO_RANGE_500_DPS  = (0x02 << 1);
constexpr uint8_t GYRO_RANGE_1000_DPS = (0x04 << 1);
constexpr uint8_t GYRO_RANGE_2000_DPS = (0x06 << 1);

// Betaflight values
// Accel:
//   833Hz ODR, 16G, use LPF1 output
//   high performance mode
// Gyro:
//   6664Hz ODR, 2000dps
//   LPF1 cutoff 335.5Hz
//
// latch LSB/MSB at reads, pins high, pins push/pull, auto-increment reads
// disable i3c interface

// // The accelerometer/gyroscope data rate
// enum ODR : uint8_t {
//   RATE_SHUTDOWN = 0x00,
//   RATE_104_HZ   = 0x40,
//   RATE_208_HZ   = 0x50,
//   RATE_416_HZ   = 0x60,
//   RATE_833_HZ   = 0x70,
//   RATE_1_66_KHZ = 0x80,
//   RATE_3_33_KHZ = 0x90,
//   RATE_6_66_KHZ = 0xA0,
// };

// // The accelerometer data range
// enum accel_range : uint8_t {
//   ACCEL_RANGE_2_G  = (0x00 << 2),
//   ACCEL_RANGE_16_G = (0x01 << 2),
//   ACCEL_RANGE_4_G  = (0x02 << 2),
//   ACCEL_RANGE_8_G  = (0x03 << 2)
// };

// // The gyro data range
// enum gyro_range : uint8_t {
//   GYRO_RANGE_125_DPS  = (0x01 << 1),
//   GYRO_RANGE_250_DPS  = (0x00 << 1),
//   GYRO_RANGE_500_DPS  = (0x02 << 1),
//   GYRO_RANGE_1000_DPS = (0x04 << 1),
//   GYRO_RANGE_2000_DPS = (0x06 << 1)
// };

// TDA: temperature
// GDA: gyro
// XLDA: accel
//                             4   2    1
// STATUS_REG: MSB 0 0 0 0 0 TDA GDA XLDA LSB
// enum sox_available_t : uint8_t {
//   SOX_NONE            = 0,
//   SOX_ACCEL           = 1,
//   SOX_GYRO            = 2,
//   SOX_ACCEL_GYRO      = 3,
//   SOX_TEMP            = 4,
//   SOX_ACCEL_GYRO_TEMP = 7,
// };

// enum sox_error : uint8_t {
//   ERROR_NONE             = 0,
//   ERROR_WHOAMI           = 1,
//   ERROR_GYRO_RANGE       = 2,
//   ERROR_ACCEL_RANGE      = 3,
//   ERROR_ENABLE_INT_ACCEL = 9,
//   ERROR_ENABLE_INT_GYRO  = 10,
//   ERROR_CTRL1_XL         = 11,
//   ERROR_CTRL2_G          = 12,
//   ERROR_CTRL3_C          = 13,
//   ERROR_CTRL4_C          = 14,
//   ERROR_CTRL5_C          = 15,
//   ERROR_CTRL6_C          = 16,
//   ERROR_CTRL7_G          = 17,
//   ERROR_CTRL8_XL         = 18,
//   ERROR_CTRL9_XL         = 19,
//   ERROR_CTRL10_C         = 20
// };

// constexpr int MAX_CHECK = 10;
constexpr size_t LSM6DSOX_BUFFER_SIZE = 14;

typedef struct {
  vec_t a, g;
  float temperature;
  uint64_t timestamp_us;
  bool ok;
} lsm6dsox_t;

typedef union {
  struct regs_t {
    int16_t temperature; // 2b, -40C to 80C
    vec_raw_t g;         // 2*3 = 6b
    vec_raw_t a;         // 2*3 = 6b
  } regs;                // 14b
  uint32_t timestamp;
  uint8_t b[LSM6DSOX_BUFFER_SIZE];
} block_t;

typedef struct {
  i2c_inst_t *i2c;
  uint8_t addr;
  float g_scale;  // int -> float
  float a_scale;  // int -> float
  float acal[12]; // accel scale/bias
  float gcal[12]; // gyro scale/bias
  block_t block;
} lsm6dsox_i2c_t;

// ACCEL_RANGE_4_G, GYRO_RANGE_2000_DPS, RATE_104_HZ
lsm6dsox_i2c_t *lsm6dsox_i2c_init(uint8_t port, uint8_t addr,
                                  uint8_t accel_range, uint8_t gyro_range,
                                  uint8_t odr);
lsm6dsox_i2c_t *lsm6dsox_i2c_init_defaults(uint8_t port);
bool lsm6dsox_reboot(lsm6dsox_i2c_t *hw);
lsm6dsox_t lsm6dsox_read(lsm6dsox_i2c_t *hw);
bool lsm6dsox_ready(lsm6dsox_i2c_t *hw);
int32_t lsm6dsox_available(lsm6dsox_i2c_t *hw);

// sox_available_t available();

#if defined __cplusplus
}
#endif
