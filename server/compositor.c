#include "compositor.h"

#include <stdbool.h>
#include <wayland-server.h>
#include <zmonitors-server.h>

#include "output.h"
#include "region.h"
#include "seat.h"
#include "surface.h"
#include "view.h"
#include "xdg-wm-base.h"

static void
zms_compositor_protocol_create_surface(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  struct zms_compositor* compositor;

  compositor = wl_resource_get_user_data(resource);

  zms_surface_create(client, id, compositor);
}

static void
zms_compositor_protocol_create_region(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  Z_UNUSED(resource);

  zms_region_create(client, id);
}

static const struct wl_compositor_interface compositor_interface = {
    .create_surface = zms_compositor_protocol_create_surface,
    .create_region = zms_compositor_protocol_create_region,
};

static void
zms_compositor_bind(
    struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
  struct zms_compositor* compositor = data;
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_compositor_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(
      resource, &compositor_interface, compositor, NULL);
}

ZMS_EXPORT struct zms_compositor*
zms_compositor_create()
{
  struct zms_compositor* compositor;
  struct zms_compositor_private* priv;
  struct wl_display* display;
  struct wl_global* global;
  struct zms_wm_base* wm_base;
  struct zms_seat* seat;
  const char* socket;

  display = wl_display_create();
  if (display == NULL) {
    zms_log("failed to create a display\n");
    goto err_display;
  }

  compositor = zalloc(sizeof *compositor);
  if (compositor == NULL) {
    zms_log("failed to allocate memory\n");
    goto err_compositor;
  }

  priv = zalloc(sizeof *priv);
  if (priv == NULL) {
    zms_log("failed to allocate memory\n");
    goto err_priv;
  }

  global = wl_global_create(
      display, &wl_compositor_interface, 4, compositor, zms_compositor_bind);
  if (global == NULL) {
    zms_log("failed to create a compositor wl_global\n");
    goto err_global;
  }

  socket = wl_display_add_socket_auto(display);
  if (socket == NULL) {
    zms_log("failed to create a display socket\n");
    goto err_global;
  }

  wl_list_init(&priv->output_list);
  compositor->priv = priv;
  compositor->display = display;
  wl_list_init(&compositor->view_list);

  /* create global objects */

  if (wl_display_init_shm(display) == -1) {
    zms_log("failed to initialize shm\n");
    goto err_global;
  }

  wm_base = zms_wm_base_create(compositor);
  if (wm_base == NULL) {
    zms_log("failed to create a wm_base\n");
    goto err_global;
  }

  seat = zms_seat_create(compositor);
  if (seat == NULL) {
    zms_log("failed to create a seat\n");
    goto err_seat;
  }

  compositor->priv->wm_base = wm_base;
  compositor->seat = seat;

  return compositor;

err_seat:
  zms_wm_base_destroy(wm_base);

err_global:
  free(priv);

err_priv:
  free(compositor);

err_compositor:
  wl_display_destroy(display);

err_display:
  return NULL;
}

ZMS_EXPORT void
zms_compositor_destroy(struct zms_compositor* compositor)
{
  wl_list_remove(&compositor->view_list);
  zms_seat_destroy(compositor->seat);
  zms_wm_base_destroy(compositor->priv->wm_base);
  wl_display_destroy(compositor->display);
  free(compositor->priv);
  free(compositor);
}
