#include "data-device-manager.h"

#include <zmonitors-util.h>

#include "data-device.h"
#include "data-source.h"

static void
zms_data_device_manager_protocol_create_data_source(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  Z_UNUSED(resource);
  zms_data_source_create(client, id);
}

static void
zms_data_device_manager_protocol_get_data_device(struct wl_client* client,
    struct wl_resource* resource, uint32_t id, struct wl_resource* seat)
{
  // TODO:
  Z_UNUSED(resource);
  Z_UNUSED(seat);
  zms_data_device_create(client, id);
}

static const struct wl_data_device_manager_interface
    data_device_manager_interface = {
        .create_data_source =
            zms_data_device_manager_protocol_create_data_source,
        .get_data_device = zms_data_device_manager_protocol_get_data_device,
};

static void
zms_data_device_manager_bind(
    struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
  struct zms_data_device_manager* data_device_manager = data;
  struct wl_resource* resource;

  resource = wl_resource_create(
      client, &wl_data_device_manager_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(
      resource, &data_device_manager_interface, data_device_manager, NULL);
}

ZMS_EXPORT struct zms_data_device_manager*
zms_data_device_manager_create(struct zms_compositor* compositor)
{
  struct zms_data_device_manager* data_device_manager;
  struct wl_global* global;

  data_device_manager = zalloc(sizeof *data_device_manager);
  if (data_device_manager == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  global =
      wl_global_create(compositor->display, &wl_data_device_manager_interface,
          3, data_device_manager, zms_data_device_manager_bind);
  if (global == NULL) {
    zms_log("failed to create a wl_data_device_manager global\n");
    goto err_global;
  }

  data_device_manager->global = global;

  return data_device_manager;

err_global:
  free(data_device_manager);

err:
  return NULL;
}

ZMS_EXPORT void
zms_data_device_manager_destroy(
    struct zms_data_device_manager* data_device_manager)
{
  wl_global_destroy(data_device_manager->global);
  free(data_device_manager);
}
