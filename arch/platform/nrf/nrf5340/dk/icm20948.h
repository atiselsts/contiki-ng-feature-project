#ifndef ICM20948_H
#define ICM20948_H

#include "lib/sensors.h"
#include "dev/spi.h"

#include "icm20948_reg.h"
#include "ICM_20948_ENUMERATIONS.h"

#define ICM20948_ACCEL_X  0
#define ICM20948_ACCEL_Y  1
#define ICM20948_ACCEL_Z  2

typedef enum
{
    ICM_20948_Stat_Ok = 0x00, // The only return code that means all is well
    ICM_20948_Stat_Err,       // A general error
    ICM_20948_Stat_NotImpl,   // Returned by virtual functions that are not implemented
    ICM_20948_Stat_ParamErr,
    ICM_20948_Stat_WrongID,
    ICM_20948_Stat_InvalSensor, // Tried to apply a function to a sensor that does not support it (e.g. DLPF to the temperature sensor)
    ICM_20948_Stat_NoData,
    ICM_20948_Stat_SensorNotSupported,
    ICM_20948_Stat_DMPNotSupported,    // DMP not supported (no #define ICM_20948_USE_DMP)
    ICM_20948_Stat_DMPVerifyFail,      // DMP was written but did not verify correctly
    ICM_20948_Stat_FIFONoDataAvail,    // FIFO contains no data
    ICM_20948_Stat_FIFOIncompleteData, // FIFO contained incomplete data
    ICM_20948_Stat_FIFOMoreDataAvail,  // FIFO contains more data
    ICM_20948_Stat_UnrecognisedDMPHeader,
    ICM_20948_Stat_UnrecognisedDMPHeader2,
    ICM_20948_Stat_InvalDMPRegister, // Invalid DMP Register

    ICM_20948_Stat_NUM,
    ICM_20948_Stat_Unknown,
} ICM_20948_Status_e;

typedef enum
{
    ICM_20948_Internal_Acc = (1 << 0),
    ICM_20948_Internal_Gyr = (1 << 1),
    ICM_20948_Internal_Mag = (1 << 2),
    ICM_20948_Internal_Tmp = (1 << 3),
    ICM_20948_Internal_Mst = (1 << 4), // I2C Master Ineternal
} ICM_20948_InternalSensorID_bm;     // A bitmask of internal sensor IDs

typedef union
{
    int16_t i16bit[3];
    uint8_t u8bit[6];
} ICM_20948_axis3bit16_t;

typedef union
{
    int16_t i16bit;
    uint8_t u8bit[2];
} ICM_20948_axis1bit16_t;

typedef struct
{
    uint8_t a : 2;
    uint8_t g : 2;
    uint8_t reserved_0 : 4;
} ICM_20948_fss_t; // Holds full-scale settings to be able to extract measurements with units

typedef struct
{
    uint8_t a;
    uint8_t g;
} ICM_20948_dlpcfg_t; // Holds digital low pass filter settings. Members are type ICM_20948_ACCEL_CONFIG_DLPCFG_e

typedef struct
{
    uint16_t a;
    uint8_t g;
} ICM_20948_smplrt_t;

typedef struct
{
    uint8_t I2C_MST_INT_EN : 1;
    uint8_t DMP_INT1_EN : 1;
    uint8_t PLL_RDY_EN : 1;
    uint8_t WOM_INT_EN : 1;
    uint8_t REG_WOF_EN : 1;
    uint8_t RAW_DATA_0_RDY_EN : 1;
    uint8_t FIFO_OVERFLOW_EN_4 : 1;
    uint8_t FIFO_OVERFLOW_EN_3 : 1;
    uint8_t FIFO_OVERFLOW_EN_2 : 1;
    uint8_t FIFO_OVERFLOW_EN_1 : 1;
    uint8_t FIFO_OVERFLOW_EN_0 : 1;
    uint8_t FIFO_WM_EN_4 : 1;
    uint8_t FIFO_WM_EN_3 : 1;
    uint8_t FIFO_WM_EN_2 : 1;
    uint8_t FIFO_WM_EN_1 : 1;
    uint8_t FIFO_WM_EN_0 : 1;
} ICM_20948_INT_enable_t;

typedef union
{
    ICM_20948_axis3bit16_t raw;
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
    } axes;
} ICM_20948_axis3named_t;

typedef struct
{
    ICM_20948_axis3named_t acc;
    ICM_20948_axis3named_t gyr;
    ICM_20948_axis3named_t mag;
    union
    {
        ICM_20948_axis1bit16_t raw;
        int16_t val;
    } tmp;
    ICM_20948_fss_t fss; // Full-scale range settings for this measurement
    uint8_t magStat1;
    uint8_t magStat2;
} ICM_20948_AGMT_t;

struct icm20948_data {
	const spi_device_t *spi_dev;

	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	uint16_t accel_sensitivity_shift;

	int16_t temp;

	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;

	uint16_t gyro_sensitivity_x10;

  ICM_20948_fss_t fss; // Full-scale range settings for this measurement

	bool accel_en;
	bool gyro_en;
	bool sensor_started;

  uint8_t _last_bank;              // Keep track of which bank was selected last - to avoid unnecessaryry writes
  uint8_t _last_mems_bank;         // Keep track of which bank was selected last - to avoid unnecessary writes
  int32_t _gyroSF;                 // Use this to record the GyroSF, calculated by inv_icm20948_set_gyro_sf
  int8_t _gyroSFpll;
};

typedef struct {
  struct icm20948_data data;
} ICM_20948_Device_t;

struct icm20948_config {
	const char *spi_label;
	uint16_t spi_addr;
	uint32_t frequency;
	uint32_t slave;
	uint8_t int_pin;
	uint8_t int_flags;
	const char *int_label;
	const char *gpio_label;
};

int icm20948_init_interrupt(const spi_device_t *dev);

//typedef struct device ICM_20948_Device_t;

// Device Level
ICM_20948_Status_e ICM_20948_set_bank(ICM_20948_Device_t *pdev, uint8_t bank);                                 // Sets the bank
ICM_20948_Status_e ICM_20948_sw_reset(ICM_20948_Device_t *pdev);                                               // Performs a SW reset
ICM_20948_Status_e ICM_20948_sleep(ICM_20948_Device_t *pdev, bool on);                                         // Set sleep mode for the chip
ICM_20948_Status_e ICM_20948_low_power(ICM_20948_Device_t *pdev, bool on);                                     // Set low power mode for the chip
//ICM_20948_Status_e ICM_20948_set_clock_source(ICM_20948_Device_t *pdev, ICM_20948_PWR_MGMT_1_CLKSEL_e source); // Choose clock source
ICM_20948_Status_e ICM_20948_get_who_am_i(ICM_20948_Device_t *pdev, uint8_t *whoami);                          // Return whoami in out prarmeter
ICM_20948_Status_e ICM_20948_check_id(ICM_20948_Device_t *pdev);                                               // Return 'ICM_20948_Stat_Ok' if whoami matches ICM_20948_WHOAMI
ICM_20948_Status_e ICM_20948_data_ready(ICM_20948_Device_t *pdev);                                             // Returns 'Ok' if data is ready

  // Internal Sensor Options
  ICM_20948_Status_e ICM_20948_set_sample_mode(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_LP_CONFIG_CYCLE_e mode); // Use to set accel, gyro, and I2C master into cycled or continuous modes
  ICM_20948_Status_e ICM_20948_set_full_scale(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_fss_t fss);
  ICM_20948_Status_e ICM_20948_set_dlpf_cfg(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_dlpcfg_t cfg);
  ICM_20948_Status_e ICM_20948_enable_dlpf(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, bool enable);
  ICM_20948_Status_e ICM_20948_set_sample_rate(ICM_20948_Device_t *pdev, ICM_20948_InternalSensorID_bm sensors, ICM_20948_smplrt_t smplrt);


ICM_20948_Status_e ICM_20948_execute_w(const ICM_20948_Device_t *dev, uint8_t regaddr, uint8_t *pdata, uint32_t len);
ICM_20948_Status_e ICM_20948_execute_r(const ICM_20948_Device_t *dev, uint8_t regaddr, uint8_t *pdata, uint32_t len);

int icm20948_sensor_init(const spi_device_t *cdev);

extern const struct sensors_sensor icm20948_sensor;

#endif /* ICM20948_H */
