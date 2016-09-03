//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook_internal.h"

struct dr7_ctx {
  uint8_t flags_dr0 : 2;
  uint8_t flags_dr1 : 2;
  uint8_t flags_dr2 : 2;
  uint8_t flags_dr3 : 2;

  char unused;

  uint8_t trigger_dr0 : 2;
  uint8_t size_dr0 : 2;
  uint8_t trigger_dr1 : 2;
  uint8_t size_dr1 : 2;
  uint8_t trigger_dr2 : 2;
  uint8_t size_dr2 : 2;
  uint8_t trigger_dr3 : 2;
  uint8_t size_dr3 : 2;
};

core *x86_64_core_create() {
  static core *core = NULL;
  if (!core) {
    core = malloc(sizeof(struct core));
  }
  return core;
}

bool x86_64_core_install_breakpoint(breakpoint_t *bp) {
  if (!bp)
    return false;

  switch (bp->type) {
  case SW_BREAKPOINT: {
    // write 0xCC at addr
  } break;

  case HW_BREAKPOINT: {
    // get slots left
    // get thread states (exluding self)
    // apply to dr7 and dr[n]

  } break;
  }
}

bool x86_64_core_uninstall_breakpoint(breakpoint_t *bp) {
  // oh boy
}

int x86_64_core_get_hw_breakpoint_slots() { return 4; }

bool x86_64_core_install_hw_breakpoint(vm_address_t address) {
  static int hw_bp_count = 0;
  mach_port_t exclude[] = {mach_thread_self()};
  mach_port_t *threads;
  size_t count;
  int slots = x86_64_core_get_hw_breakpoint_slots();

  if (slots <= hw_bp_count)
    return false;

  x86_64_core_get_thread_list(exclude, 1, &threads, &count);

  for (int i = 0; i < count; i++) {
    x86_debug_state64_t debug_state;
    mach_msg_type_number_t count = x86_DEBUG_STATE64_COUNT;
    krncall(thread_get_state(threads[i], x86_DEBUG_STATE64,
                             (thread_state_t)&debug_state, &count));

    struct dr7_ctx dr7 = *(struct dr7_ctx *)&debug_state.__dr7;

    switch (hw_bp_count) {
    case 0: {
      dr7.flags_dr0 |= 0x1;   // local
      dr7.size_dr0 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr0 |= 0x0; // trigger on code exec

      debug_state.__dr0 = address;
      break;
    }

    case 1: {
      dr7.flags_dr1 |= 0x1;   // local
      dr7.size_dr1 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr1 |= 0x0; // trigger on code exec

      debug_state.__dr1 = address;
      break;
    }

    case 2: {
      dr7.flags_dr2 |= 0x1;   // local
      dr7.size_dr2 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr2 |= 0x0; // trigger on code exec

      debug_state.__dr2 = address;
      break;
    }

    case 3: {
      dr7.flags_dr3 |= 0x1;   // local
      dr7.size_dr3 |= 0x0;    // 1 byte breakpoint
      dr7.trigger_dr3 |= 0x0; // trigger on code exec

      debug_state.__dr3 = address;
      break;
    }

    default: {
      // all debug registers in use
      return false;
    }
    }

    krncall(thread_set_state(threads[i], x86_DEBUG_STATE64,
                             (thread_state_t)&debug_state,
                             x86_DEBUG_STATE64_COUNT));
    hw_bp_count++;
  }
}

kern_return_t x86_64_core_get_thread_list(mach_port_t *ignore,
                                          size_t ignore_count,
                                          mach_port_t **out,
                                          size_t *out_count) {
  thread_act_port_array_t threads;
  mach_msg_type_number_t count;
  kern_return_t status = task_threads(mach_task_self(), &threads, &count);

  mach_port_t out_threads[100];
  for (int i = 0; i < count; i++) {
    bool add = true;
    for (int j = 0; j < ignore_count; j++) {
      if (threads[i] == ignore[j]) {
        add = false;
        break;
      }
    }
    if (add) {
      out_threads[*out_count++] = threads[i];
    }
  }

  *out = malloc(sizeof(mach_port_t) * *out_count);
  memcpy(*out, out_threads, *out_count);
  return status;
}
