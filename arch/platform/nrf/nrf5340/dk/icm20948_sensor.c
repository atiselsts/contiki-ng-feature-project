#include "contiki.h"
#include "icm20948.h"
#include "dev/spi.h"

static int is_on;

/*---------------------------------------------------------------------------*/
static const spi_device_t spi_configuration_default = {
  .port_spi_sck = ICM_SPI_PORT,
  .port_spi_miso = ICM_SPI_PORT,
  .port_spi_mosi = ICM_SPI_PORT,
  .port_spi_cs = ICM_SPI_PORT,
  
  .spi_controller = ICM_SPI_CONTROLLER,
  .pin_spi_sck = ICM_SCLK_PIN,
  .pin_spi_miso = ICM_MISO_PIN,
  .pin_spi_mosi = ICM_MOSI_PIN,
  .pin_spi_cs = ICM_CS_PIN,
  .spi_bit_rate = 1000000,
  .spi_pha = 0,
  .spi_pol = 0
};

static const spi_device_t *spi_device = &spi_configuration_default;

/*---------------------------------------------------------------------------*/
static bool
init(void)
{
  // if(spi_select(spi_device) == SPI_DEV_STATUS_OK) {
  // spi_deselect(spi_device);
  // ret = spi_write(spi_device, wbuf, sizeof(wbuf));
  // ret = spi_read(spi_device, &buf, sizeof(buf));
  if(spi_acquire(spi_device) != SPI_DEV_STATUS_OK) {
    return false;
  }

  return true;

}  
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  uint8_t accel_data[3] = {1, 2, 3}; // TODO: read this from the device!

  switch (type) {
  case ICM20948_ACCEL_X:
    return accel_data[0];
  case ICM20948_ACCEL_Y:
    return accel_data[1];
  case ICM20948_ACCEL_Z:
    return accel_data[2];
  default:
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
static void
enable_sensor(int on)
{
  if(is_on == on) {
    return;
  }
  if(!on) {
    is_on = 0;
    return; /* TODO */
  }

  is_on = 1;
}
/*---------------------------------------------------------------------------*/
static int
reading_ready(void)
{
  // TODO: read this from the device!
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int enable)
{
  int rv = 0;

  switch(type) {
  case SENSORS_HW_INIT:
    /* inactive by default */
    init();
    enable_sensor(0);
    break;
  case SENSORS_ACTIVE:
    if(enable) {
      enable_sensor(1);
      rv = 1;
    } else {
      enable_sensor(0);
      rv = 0;
    }
    break;
  default:
    break;
  }
  return rv;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  switch(type) {
  case SENSORS_ACTIVE:
    return is_on;
  case SENSORS_READY:
    return reading_ready();
  default:
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(icm20948_sensor, "ICM20948", value, configure, status);
/*---------------------------------------------------------------------------*/
