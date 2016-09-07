//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook.h"
#include "ghetto_hook_internal.h"

breakpoint_store_t global_store;

core *core_for_host() {
#ifdef __x86_64__
  return x86_64_core();
#endif
}

void ghetto_hook_setup_breakpoint_store(breakpoint_store_t *store) {
  store->size = 0;
  store->capacity = 10;
  store->data = malloc(sizeof(breakpoint_t) * store->capacity);
}

void ghetto_hook_try_resize_store(breakpoint_store_t *store) {
  if (store->size >= store->capacity) {
    store->capacity *= 2;
    store->data = realloc(store->data, sizeof(breakpoint_t) * store->capacity);
  }
}

void ghetto_hook_save_breakpoint(breakpoint_t bp) {
  if (global_store.capacity == 0) {
    ghetto_hook_setup_breakpoint_store(&global_store);
  }

  ghetto_hook_try_resize_store(&global_store);

  global_store.data[global_store.size++] = bp;
}

breakpoint_t *ghetto_hook_breakpoint_at_index(int index) {
  return global_store.data + index;
}

// this is very inefficient
void ghetto_hook_delete_breakpoint(int index) {
  breakpoint_t *store = malloc(sizeof(breakpoint_t) * global_store.capacity);
  for (int i = 0; i < global_store.size; i++) {
    if (i == index)
      continue;

    store[i] = global_store.data[i];
  }
  global_store.size--;
  free(global_store.data);
  global_store.data = store;
}

breakpoint_t *ghetto_hook_lookup_breakpoint(vm_address_t target) {
  for (int i = 0; i < global_store.size; i++) {
    if ((global_store.data + i)->address == target) {
      return global_store.data + i;
    }
  }
  return NULL;
}

bool ghetto_hook(vm_address_t target, vm_address_t replacement) {
  core *core = core_for_host();
  if (!core)
    return false;

  // TODO: a core_create_breakpoint() func
  breakpoint_t *bp = malloc(sizeof(breakpoint_t));
  bp->address = target;
  bp->type = SW_BREAKPOINT;
  core->install_breakpoint(bp);
}
