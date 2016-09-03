//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "ghetto_hook_internal.h"

core *x86_64_core_create();

// typedef struct x86_64_core {
//   bool (*install_breakpoint)(breakpoint_t *bp);
//   bool (*uninstall_breakpoint)(breakpoint_t *bp);
//   void (*apply_redirection)(thread_state_t state);
// } x86_64_core;
