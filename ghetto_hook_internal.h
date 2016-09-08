//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook.h"

#define krncall(expr)                                                          \
  do {                                                                         \
    kern_return_t status = (expr);                                             \
    if (status != KERN_SUCCESS) {                                              \
      fprintf(stderr, "mach error: " #expr " [%s]\n",                          \
              mach_error_string(status));                                      \
      return false;                                                            \
    }                                                                          \
  } while (false)

typedef enum { HW_BREAKPOINT, SW_BREAKPOINT } breakpoint_type_t;

typedef struct breakpoint {
  breakpoint_type_t type;
  vm_address_t address;
} breakpoint_t;

typedef struct breakpoint_store {
  int size;
  int capacity;
  breakpoint_t *data;
} breakpoint_store_t;

typedef struct core {
  bool (*install_breakpoint)(breakpoint_t *bp);
  bool (*uninstall_breakpoint)(breakpoint_t *bp);
} core;

// TODO: all other internal function declarations
breakpoint_t *ghetto_hook_lookup_breakpoint(vm_address_t target);
