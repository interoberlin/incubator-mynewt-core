/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include "syscfg/syscfg.h"
#include <flash_map/flash_map.h>
#include <os/os.h>

#include <hal/hal_bsp.h>
#include <hal/hal_system.h>
#include <hal/hal_flash.h>
#if MYNEWT_VAL(BOOT_SERIAL)
#include <sysinit/sysinit.h>
#endif
#include <console/console.h>
#include "bootutil/image.h"
#include "bootutil/bootutil.h"

#define BOOT_AREA_DESC_MAX  (256)
#define AREA_DESC_MAX       (BOOT_AREA_DESC_MAX)

int
main(void)
{
    struct boot_rsp rsp;

    hal_bsp_init();

#if MYNEWT_VAL(BOOT_SERIAL)
    sysinit();
#else
    flash_map_init();
#endif


/*
 * The BOOT_FORCE syscfg.yml parameter
 * allows flashing and debugging nRF51 and nRF52 app ELFs
 * from the PC via GDB/Eclipse
 * without having to the invoke 'newt create-image'
 * to add a valid image header.
 * If BOOT_FORCE is enabled, the image in slot 0 will be booted
 * even when the image header is not present, intact or valid.
 */
#if MYNEWT_VAL(BOOT_FORCE)
    // Disregard function return code
    boot_go(&rsp);

    /*
     * Flash address of the app image slot 0 + image header size,
     * see
     *  bsp/nrf51dk-16kbram/nrf51xxaa.ld
     * and
     *  bsp/nrf52dk/nrf52xxaa.ld
     */
    // TODO: Replace with address variable from linker script
    uint32_t app0_origin = 0x00008000;
    // TODO: Replace with _imghdr_size variable from linker script
    uint8_t  app0_header_size = 0x20;
    hal_system_start((void*) (app0_origin + app0_header_size));
#else
    int rc;
    rc = boot_go(&rsp);
    assert(rc == 0);

    hal_system_start((void *)(rsp.br_image_addr + rsp.br_hdr->ih_hdr_size));
#endif

    return 0;
}
