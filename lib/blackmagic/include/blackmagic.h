
#pragma once

#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG 1
#endif

#ifndef PLATFORM_PRINTF
#define PLATFORM_PRINTF platform_printf
#endif

#ifndef CONFIG_BMDA
#define CONFIG_BMDA 0
#endif

#ifndef NO_LIBOPENCM3
#define NO_LIBOPENCM3
#endif

#include <gdb_if.h>
#include <general.h>
#include <platform.h>

#include "platform.h"
#include "version.h"
