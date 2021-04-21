
#ifndef _GENIE_APP_H_
#define _GENIE_APP_H_

#include "base/genie_cmds.h"
#include "base/genie_event.h"
#include "base/genie_flash.h"
#include "base/genie_reset.h"
#include "base/tri_tuple.h"
#ifdef MESH_MODEL_VENDOR_TIMER
#include "base/vendor_timers.h"
#endif

#ifdef CONFIG_GENIE_OTA
#include "bluetooth/host/profile/ais_srv/ais_service.h"
#endif

#ifdef CONFIG_BEKEN_OTA
#include "bluetooth/host/profile/ota_srv/ota_service.h"

#endif

#include "bluetooth/mesh/genie_mesh.h"
#include "mesh_model.h"
#include "light_board.h"

/**
 * @brief The initialization api for genie sdk. The application always
 *        involks this api to initialize AIS/Mesh procedure for bussiness.
 */
void genie_init(void);
void dut_init(void);

#endif  //_GENIE_APP_H_
