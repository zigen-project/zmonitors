#ifndef ZMONITORS_BACKEND_VIRTUAL_OBJECT_H
#define ZMONITORS_BACKEND_VIRTUAL_OBJECT_H

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_virtual_object {
  void *user_data;
  const struct zms_virtual_object_interface *interface;

  struct zgn_virtual_object *proxy;
  struct zms_backend *backend;

  struct zms_signal destroy_signal;
};

struct zms_virtual_object *zms_virtual_object_create(void *user_data,
    const struct zms_virtual_object_interface *interface,
    struct zms_backend *backend);

void zms_virtual_object_destroy(struct zms_virtual_object *virtual_object);

#endif  //  ZMONITORS_BACKEND_VIRTUAL_OBJECT_H
