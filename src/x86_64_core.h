//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#pragma once

#include "ghetto_hook_internal.h"

core *x86_64_core_create();

// typedef struct x86_64_core {
//   bool (*install_breakpoint)(breakpoint_t *bp);
//   bool (*uninstall_breakpoint)(breakpoint_t *bp);
//   void (*apply_redirection)(thread_state_t state);
// } x86_64_core;

void *x86_64_core_mach_server_thread(void *arg);

core *x86_64_core_get_core();
bool x86_64_core_setup_core(); // possibly take a core* param

kern_return_t x86_64_core_exception_callback(mach_port_t thread,
                                             exception_type_t type);

bool x86_64_core_install_breakpoint(breakpoint_t *bp);
bool x86_64_core_uninstall_breakpoint(breakpoint_t *bp);

bool x86_64_core_install_sw_breakpoint(vm_address_t address);

int x86_64_core_get_hw_breakpoint_slots();
bool x86_64_core_install_hw_breakpoint(vm_address_t address);

kern_return_t x86_64_core_get_thread_list(mach_port_t *ignore,
                                          size_t ignore_count,
                                          mach_port_t **out, size_t *out_count);
vm_address_t x86_64_core_get_thread_address(mach_port_t thread);

void x86_64_core_apply_thread_redirection(mach_port_t thread,
                                          vm_address_t target);

bool x86_64_core_write_address(task_t task, vm_address_t address, void *data,
                               size_t size, bool force);

bool x86_64_core_read_address(task_t task, vm_address_t address, void *data,
                              size_t size);
