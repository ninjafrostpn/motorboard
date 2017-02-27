#include <stdint.h>

/* Current firmware version */
/* Put this in .fwver section so that the linker script can link it
 *    into the right place. */
const uint16_t firmware_version __attribute__ ((section (".fwver"))) = FW_VER;
