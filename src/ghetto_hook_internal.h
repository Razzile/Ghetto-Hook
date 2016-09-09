//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include <mach/mach.h>
#include <stdbool.h>

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
  char *original; // unused for HW
  int len;        // unused for HW
} breakpoint_t;

typedef struct breakpoint_store {
  int size;
  int capacity;
  breakpoint_t *data;
} breakpoint_store_t;

typedef bool (*install_breakpoint_function_t)(breakpoint_t *);
typedef bool (*uninstall_breakpoint_function_t)(breakpoint_t *);

typedef struct core {
  install_breakpoint_function_t install_func;
  uninstall_breakpoint_function_t uninstall_func;
} core;

// TODO: all other internal function declarations
breakpoint_t *ghetto_hook_lookup_breakpoint(vm_address_t target);
