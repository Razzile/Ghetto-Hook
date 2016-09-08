//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include "x86_64_core.h"
#include "ghetto_hook_internal.h"
#include <pthread.h>
#include <stdio.h>

extern boolean_t mach_exc_server(mach_msg_header_t *, mach_msg_header_t *);

// structure representing the format of debug register 7
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

kern_return_t catch_mach_exception_raise(mach_port_t exception_port,
                                         mach_port_t thread, mach_port_t task,
                                         exception_type_t type,
                                         exception_data_t code,
                                         mach_msg_type_number_t code_count) {
  //
  //
  return x86_64_core_exception_callback(thread, type); // other params?
}

kern_return_t catch_mach_exception_raise_state(
    mach_port_t exception_port, exception_type_t exception,
    exception_data_t code, mach_msg_type_number_t code_count, int *flavor,
    thread_state_t in_state, mach_msg_type_number_t in_state_count,
    thread_state_t out_state, mach_msg_type_number_t *out_state_count) {
  return KERN_FAILURE;
}

kern_return_t catch_mach_exception_raise_state_identity(
    mach_port_t exception_port, mach_port_t thread, mach_port_t task,
    exception_type_t exception, exception_data_t code,
    mach_msg_type_number_t code_count, int *flavor, thread_state_t in_state,
    mach_msg_type_number_t in_state_count, thread_state_t out_state,
    mach_msg_type_number_t *out_state_count) {
  return KERN_FAILURE;
}

void *x86_64_core_mach_server_thread(void *arg) {
  mach_port_t exc_port = *(mach_port_t *)arg;
  kern_return_t kr;

  while (1) {
    if ((kr = mach_msg_server_once(mach_exc_server, 4096, exc_port, 0)) !=
        KERN_SUCCESS) {
      fprintf(stderr, "mach_msg_server_once: error %s\n",
              mach_error_string(kr));
    }
  }
  return NULL;
}

core *x86_64_core_create_core() {
  static core *core = NULL;
  if (!core) {
    core = malloc(sizeof(struct core));
  }
  return core;
}

bool x86_64_core_setup_core() {
  mach_port_t exc_port;
  mach_port_t task = mach_task_self();
  pthread_t exception_thread;
  int err;

  mach_msg_type_number_t maskCount = 1;
  exception_mask_t mask;
  exception_handler_t handler;
  exception_behavior_t behavior;
  thread_state_flavor_t flavor;

  krncall(mach_port_allocate(task, MACH_PORT_RIGHT_RECEIVE, &exc_port));

  krncall(mach_port_insert_right(task, exc_port, exc_port,
                                 MACH_MSG_TYPE_MAKE_SEND));

  krncall(task_get_exception_ports(task, EXC_MASK_ALL, &mask, &maskCount,
                                   &handler, &behavior, &flavor));

  if ((err = pthread_create(&exception_thread, NULL,
                            x86_64_core_mach_server_thread, &exc_port)) != 0) {
    fprintf(stderr, "pthread_create server_thread: %s\n", strerror(err));
    return false;
  }

  pthread_detach(exception_thread);

  krncall(task_set_exception_ports(task, EXC_MASK_BREAKPOINT, exc_port,
                                   EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,
                                   flavor));

  return true;
}

kern_return_t x86_64_core_exception_callback(mach_port_t thread,
                                             exception_type_t type) {
  // we are only interested in breakpoints
  if (type != EXC_BREAKPOINT)
    return KERN_FAILURE;

  vm_address_t exception_address = x86_64_core_get_thread_address(thread);
  breakpoint_t *bp = ghetto_hook_lookup_breakpoint(exception_address);
  if (bp) {
    x86_64_core_apply_thread_redirection(thread, bp->address);
  }
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

vm_address_t x86_64_core_get_thread_address(mach_port_t thread) {
  x86_thread_state64_t thread_state;
  mach_msg_type_number_t count;
  thread_get_state(thread, x86_THREAD_STATE64, (thread_state_t)&thread_state,
                   &count);

  return thread_state.__rip & ~0x1;
}

void x86_64_core_apply_thread_redirection(mach_port_t thread,
                                          vm_address_t target) {
  x86_thread_state64_t thread_state;
  mach_msg_type_number_t count;
  thread_get_state(thread, x86_THREAD_STATE64, (thread_state_t)&thread_state,
                   &count);

  vm_address_t addr = x86_64_core_get_thread_address(thread);
  vm_address_t stack = thread_state.__rsp;
  stack -= 0x8; // possibly save new rsp
  *(vm_address_t *)stack = addr;
  thread_state.__rip = target;

  thread_set_state(thread, x86_THREAD_STATE64, (thread_state_t)&thread_state,
                   x86_THREAD_STATE64_COUNT);
}

bool x86_64_core_write_address(task_t task, vm_address_t address, void *data,
                               size_t size, bool force) {
  if (force) {
    krncall(vm_protect(task, address, size, false,
                       VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE |
                           VM_PROT_COPY));
    krncall(vm_write(task, address, (vm_offset_t)data, size));

    krncall(
        vm_protect(task, address, size, false, VM_PROT_READ | VM_PROT_EXECUTE));
  } else {
    krncall(vm_write(task, address, (vm_offset_t)data, size));
  }
  return true;
}

bool x86_64_core_read_address(task_t task, vm_address_t address, void *data,
                              size_t size) {
  krncall(vm_read_overwrite(task, address, size, (vm_address_t)data, &size));
}
