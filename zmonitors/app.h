#ifndef ZMONITORS_APP_H
#define ZMONITORS_APP_H

#include <zmonitors-backend.h>
#include <zmonitors-server.h>

#include "data-device-manager.h"
#include "monitor.h"

struct zms_app {
  struct zms_compositor* compositor;
  struct zms_backend* backend;
  struct zms_data_device_manager* data_device_manager;
  struct wl_event_source* backend_event_source;
  struct zms_monitor* primary_monitor;
};

struct zms_app* zms_app_create();

void zms_app_destroy(struct zms_app* app);

void zms_app_run(struct zms_app* app);

#endif  //  ZMONITORS_APP_H
