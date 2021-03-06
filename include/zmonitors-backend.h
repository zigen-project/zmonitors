#ifndef ZMONITORS_BACKEND_H
#define ZMONITORS_BACKEND_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include <zmonitors-util.h>

/* backend */

struct zms_backend;

struct zms_backend_interface {
  void (*gain_ray_capability)(void* data);
  void (*lose_ray_capability)(void* data);
  void (*gain_keyboard_capability)(void* data);
  void (*lose_keyboard_capability)(void* data);
  void (*keyboard_keymap)(
      void* data, uint32_t format, int32_t fd, uint32_t size);
};

struct zms_backend* zms_backend_create(
    void* user_data, const struct zms_backend_interface* interface);

void zms_backend_destroy(struct zms_backend* backend);

bool zms_backend_connect(struct zms_backend* backend, const char* socket);

int zms_backend_get_fd(struct zms_backend* backend);

int zms_backend_dispatch(struct zms_backend* backend);

int zms_backend_flush(struct zms_backend* backend);

int zms_backend_dispatch_pending(struct zms_backend* backend);

/* virtual object */

struct zms_virtual_object;

struct zms_virtual_object_interface {
  void (*ray_enter)(void* data, uint32_t serial, vec3 origin, vec3 direction);
  void (*ray_leave)(void* data, uint32_t serial);
  void (*ray_motion)(void* data, uint32_t time, vec3 origin, vec3 direction);
  void (*ray_button)(void* data, uint32_t serial, uint32_t time,
      uint32_t button, uint32_t state);
  void (*keyboard_enter)(void* data, uint32_t serial, struct wl_array* keys);
  void (*keyboard_leave)(void* data, uint32_t serial);
  void (*keyboard_key)(
      void* data, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
  void (*keyboard_modifiers)(void* data, uint32_t serial,
      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
      uint32_t group);
};

/* cuboid window */

struct zms_cuboid_window_private;

struct zms_cuboid_window_interface {
  const struct zms_virtual_object_interface base;
  void (*moved)(void* data, vec3 face_direction);
};

struct zms_cuboid_window {
  struct zms_cuboid_window_private* priv;
  struct zms_backend* backend;
  struct zms_virtual_object* virtual_object;

  vec3 half_size;
  versor quaternion;

  void (*configured)(void* user_data, struct zms_cuboid_window* cuboid_window);
};

struct zms_cuboid_window* zms_cuboid_window_create(void* user_data,
    const struct zms_cuboid_window_interface* interface,
    struct zms_backend* backend, vec3 half_size, versor quaternion);

void zms_cuboid_window_destroy(struct zms_cuboid_window* cuboid_window);

void zms_cuboid_window_commit(struct zms_cuboid_window* cuboid_window);

void zms_cuboid_window_move(
    struct zms_cuboid_window* cuboid_window, uint32_t serial);

void zms_cuboid_window_rotate(
    struct zms_cuboid_window* cuboid_window, versor quaternion);

/* opengl shader */

struct zms_opengl_shader_program;

struct zms_opengl_shader_program* zms_opengl_shader_program_create(
    struct zms_backend* backend, const char* vertex_shader,
    size_t vertex_shader_size, const char* fragment_shader,
    size_t fragment_shader_size);

void zms_opengl_shader_program_destroy(
    struct zms_opengl_shader_program* program);

void zms_opengl_shader_program_set_uniform_variable_mat4(
    struct zms_opengl_shader_program* program, const char* location, mat4 mat);

void zms_opengl_shader_program_set_uniform_variable_vec3(
    struct zms_opengl_shader_program* program, const char* location, vec3 vec);

/* opengl vertex buffer*/

struct zms_opengl_vertex_buffer;

struct zms_opengl_vertex_buffer* zms_opengl_vertex_buffer_create(
    struct zms_backend* backend, size_t size);

void zms_opengl_vertex_buffer_destroy(
    struct zms_opengl_vertex_buffer* vertex_buffer);

int zms_opengl_vertex_buffer_get_fd(
    struct zms_opengl_vertex_buffer* vertex_buffer);

/* opengl texture */

struct zms_opengl_texture;

struct zms_opengl_texture* zms_opengl_texture_create_by_fd(
    struct zms_backend* backend, int fd, struct zms_screen_size size);

static inline struct zms_opengl_texture*
zms_opengl_texture_create(
    struct zms_backend* backend, struct zms_screen_size size)
{
  return zms_opengl_texture_create_by_fd(backend, -1, size);
}

void zms_opengl_texture_destroy(struct zms_opengl_texture* texture);

int zms_opengl_texture_get_fd(struct zms_opengl_texture* texture);

/* opengl component */

struct zms_opengl_component_private;

struct zms_opengl_component {
  struct zms_opengl_component_private* priv;
};

struct zms_opengl_component* zms_opengl_component_create(
    struct zms_virtual_object* virtual_object);

void zms_opengl_component_destroy(struct zms_opengl_component* component);

void zms_opengl_component_attach_vertex_buffer(
    struct zms_opengl_component* component,
    struct zms_opengl_vertex_buffer* vertex_buffer);

void zms_opengl_component_attach_shader_program(
    struct zms_opengl_component* component,
    struct zms_opengl_shader_program* shader);

void zms_opengl_component_attach_texture(
    struct zms_opengl_component* component, struct zms_opengl_texture* texture);

void zms_opengl_component_texture_updated(
    struct zms_opengl_component* component);

void zms_opengl_component_set_min(
    struct zms_opengl_component* component, uint32_t min);

void zms_opengl_component_set_count(
    struct zms_opengl_component* component, uint32_t count);

void zms_opengl_component_set_topology(
    struct zms_opengl_component* component, uint32_t topology);

void zms_opengl_component_add_vertex_attribute(
    struct zms_opengl_component* component, uint32_t index, uint32_t size,
    uint32_t type, uint32_t normalized, uint32_t stride, uint32_t pointer);

/* frame callback */

struct zms_frame_callback_private;

struct zms_frame_callback {
  struct zms_frame_callback_private* priv;
  struct wl_list link;  // removed from the list when destroyed
};

typedef void (*zms_frame_callback_func_t)(void* data, uint32_t time);

struct zms_frame_callback* zms_frame_callback_create(
    struct zms_virtual_object* virtual_object, void* data,
    zms_frame_callback_func_t callback_func);

void zms_frame_callback_destroy(struct zms_frame_callback* frame_callback);

#endif  //  ZMONITORS_BACKEND_H
