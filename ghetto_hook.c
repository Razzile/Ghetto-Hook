//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook.h"
#include "ghetto_hook_internal.h"

core *core_for_host() {
#ifdef __x86_64__
  return x86_64_core();
#endif
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
