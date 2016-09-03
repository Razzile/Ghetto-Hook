//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook_internal.h"

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

kern_return_t x86_64_core_get_thread_list(mach_port_t *ignore,
                                          size_t ignore_count, mach_port_t *out,
                                          size_t *out_count) {
  thread_act_port_array_t threads;
  mach_msg_type_number_t count;
  kern_return_t status = task_threads(mach_task_self(), &threads, &count);

  thread_act_port_array_t out_threads = malloc(256 * sizeof(mach_port_t));
  // create a mini dynamic thread class and add to it here
}
