#include "contiki.h"
#include "dev/spi.h"
#include <nrfx_spim.h>

#include <string.h>

/* See: zephyrproject/zephyr/drivers/spi/ */

static nrfx_spim_t def_spim = NRFX_SPIM_INSTANCE(2);

static const nrfx_spim_config_t def_config = {
  .frequency = NRF_SPIM_FREQ_1M,
  .mode      = NRF_SPIM_MODE_0,
  .bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST,
  .sck_pin   = 6,
  .mosi_pin  = 4,
  .miso_pin  = 14,
  .ss_pin    = 8,
  .orc       = 0,
  .miso_pull = 0,
};

static void event_handler(const nrfx_spim_evt_t *p_event, void *p_context);

/*---------------------------------------------------------------------------*/
bool
spi_arch_has_lock(const spi_device_t *dev)
{
  return false;
}
/*q---------------------------------------------------------------------------*/
bool
spi_arch_is_bus_locked(const spi_device_t *dev)
{
  return false;
}
/*---------------------------------------------------------------------------*/
struct spi_nrfx_data {
	/* struct spi_context ctx; */
	const spi_device_t *dev;
	size_t  chunk_len;
	bool    busy;
	bool    initialized;
};
static struct spi_nrfx_data v_dev_data;
static struct spi_nrfx_data *dev_data = &v_dev_data;
/*---------------------------------------------------------------------------*/
struct spi_nrfx_config {
	nrfx_spim_t	   spim;
	size_t		   max_chunk_len;
	uint32_t	   max_freq;
	nrfx_spim_config_t config;
};
static struct spi_nrfx_config v_config;
static struct spi_nrfx_config *dev_config = &v_config;
/*---------------------------------------------------------------------------*/
static inline struct spi_nrfx_data *get_dev_data(const spi_device_t *dev)
{
	return dev_data;
}
/*---------------------------------------------------------------------------*/
static inline struct spi_nrfx_config *get_dev_config(const spi_device_t *dev)
{
	return dev_config;
}
/*---------------------------------------------------------------------------*/
spi_status_t
spi_arch_lock_and_open(const spi_device_t *dev)
{
	nrfx_err_t result;
  struct spi_nrfx_config *dev_config = get_dev_config(dev);

  dev_config->spim = def_spim;
  /* dev_config->spim = NRFX_SPIM_INSTANCE(dev_config->spi_controller); */

  memcpy(&dev_config->config, &def_config, sizeof(dev_config->config));
  dev_config->config.sck_pin = dev->pin_spi_sck;
  dev_config->config.mosi_pin = dev->pin_spi_mosi;
  dev_config->config.miso_pin = dev->pin_spi_miso;
  dev_config->config.ss_pin  = dev->pin_spi_cs;

  result = nrfx_spim_init(&dev_config->spim, &dev_config->config, event_handler, dev_data);
	if (result != NRFX_SUCCESS) {
    return SPI_DEV_STATUS_EINVAL;
  }

  return SPI_DEV_STATUS_OK;
}
/*---------------------------------------------------------------------------*/
spi_status_t
spi_arch_close_and_unlock(const spi_device_t *dev)
{
  struct spi_nrfx_config *dev_config = get_dev_config(dev);
  nrfx_spim_uninit(&dev_config->spim);
  return SPI_DEV_STATUS_OK;
}
/*---------------------------------------------------------------------------*/
static void transfer_next_chunk(const spi_device_t *dev, uint8_t c, uint8_t *ret)
{
	/* struct spi_nrfx_data *dev_data = get_dev_data(dev); */
 	const struct spi_nrfx_config *dev_config = get_dev_config(dev);
  nrfx_spim_xfer_desc_t xfer;
  nrfx_err_t result;

  xfer.p_tx_buffer = &c;
  xfer.tx_length   = 1;
  xfer.p_rx_buffer = ret;
  xfer.rx_length   = 1;

  result = nrfx_spim_xfer(&dev_config->spim, &xfer, 0);
  if (result == NRFX_SUCCESS) {
    return;
  }
}
/*---------------------------------------------------------------------------*/
spi_status_t
spi_arch_transfer(const spi_device_t *dev,
                  const uint8_t *write_buf, int wlen,
                  uint8_t *inbuf, int rlen, int ignore_len)
{
  int i;
  int totlen;
  uint32_t c;

  totlen = MAX(rlen + ignore_len, wlen);

  if(totlen == 0) {
    /* Nothing to do */
    return SPI_DEV_STATUS_OK;
  }

  for(i = 0; i < totlen; i++) {
    c = i < wlen ? write_buf[i] : 0;
    transfer_next_chunk(dev, c, &inbuf[i]);
  }

  return SPI_DEV_STATUS_OK;
}
/*---------------------------------------------------------------------------*/
static void event_handler(const nrfx_spim_evt_t *p_event, void *p_context)
{
	/* struct spi_nrfx_data *dev_data = p_context; */

	if (p_event->type == NRFX_SPIM_EVENT_DONE) {
		// spi_context_update_tx(&dev_data->ctx, 1, dev_data->chunk_len);
		// spi_context_update_rx(&dev_data->ctx, 1, dev_data->chunk_len);

		// transfer_next_chunk(dev_data->dev);
	}
}
/*---------------------------------------------------------------------------*/
