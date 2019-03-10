// See LICENSE for license details.

#pragma once

#if defined(__native_client__)

#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_graphics_3d.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_input_event.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_url_request_info.h"
#include "ppapi/c/ppb_fullscreen.h"
#include "ppapi/c/ppb_console.h"
#include "ppapi/c/ppb_view.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppp_messaging.h"
#include "ppapi/c/ppp_graphics_3d.h"

extern PP_Instance pp_current_instance;
extern PPB_GetInterface get_browser_interface;
extern PPB_Messaging* ppb_messaging_interface;
extern PPB_Var* ppb_var_interface;
extern PPB_Core* ppb_core_interface;
extern PPB_Console* ppb_console_interface;
extern PPB_Graphics3D* ppb_g3d_interface;
extern PPB_Instance* ppb_instance_interface;
extern PPB_InputEvent* ppb_input_event_interface;
extern PPB_KeyboardInputEvent* ppb_keyboard_input_event_interface;
extern PPB_MouseInputEvent* ppb_mouse_input_event_interface;
extern PPB_TouchInputEvent* ppb_touch_input_event_interface;
extern PPB_WheelInputEvent* ppb_wheel_input_event_interface;
extern PPB_URLRequestInfo* ppb_urlrequestinfo_interface;
extern PPB_URLLoader* ppb_urlloader_interface;
extern PPB_Fullscreen* ppb_fullscreen_interface;
extern PPB_View* ppb_view_interface;

struct EGPlatformNACL
{
	static EGbool get_interfaces(PPB_GetInterface get_browser);
};

#endif
