
#include "log/log.h"

struct log_info g_log_info;


/*
 * Dummy handler exports.
 */
const struct log_handler log_console_handler;
const struct log_handler log_cbmem_handler;
const struct log_handler log_fcb_handler;
