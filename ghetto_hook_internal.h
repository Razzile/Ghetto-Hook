//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook.h"

typedef enum { HW_BREAKPOINT, SW_BREAKPOINT } breakpoint_type_t;

typedef struct breakpoint {
  breakpoint_type_t type;
  vm_address_t address;
  // possibly callback
} breakpoint_t;

struct core {
  bool (*apply_breakpoint)(breakpoint_t bp);
  bool (*reset_breakpoint)(breakpoint_t bp);
  void (*apply_redirection)(thread_state_t state);
};
