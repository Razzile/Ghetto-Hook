//
//  ghetto_hook
//
//  Copyright © 2016 satori. All rights reserved.
//

#include <mach/mach.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef _GHETTO_HOOK_
#define _GHETTO_HOOK_

#ifdef __cplusplus
#define API_CALL extern "C"
#else
#define API_CALL
#endif // __cplusplus

// maybe return a handle
API_CALL bool ghetto_hook(vm_address_t target, vm_address_t replace);

#endif // _GHETTO_HOOK_
