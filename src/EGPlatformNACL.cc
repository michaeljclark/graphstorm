// See LICENSE for license details.

#include "EG.h"
#include "EGPlatformNACL.h"

#if defined (__native_client__)

PP_Instance pp_current_instance = NULL;
PPB_GetInterface get_browser_interface = NULL;
PPB_Core* ppb_core_interface = NULL;
PPB_Console* ppb_console_interface = NULL;
PPB_Graphics3D* ppb_g3d_interface = NULL;
PPB_InputEvent* ppb_input_event_interface = NULL;
PPB_Instance* ppb_instance_interface = NULL;
PPB_Messaging* ppb_messaging_interface = NULL;
PPB_URLLoader* ppb_urlloader_interface = NULL;
PPB_URLRequestInfo* ppb_urlrequestinfo_interface = NULL;
PPB_Var* ppb_var_interface = NULL;
PPB_View* ppb_view_interface = NULL;
PPB_KeyboardInputEvent* ppb_keyboard_input_event_interface = NULL;
PPB_MouseInputEvent* ppb_mouse_input_event_interface = NULL;
PPB_TouchInputEvent* ppb_touch_input_event_interface = NULL;
PPB_WheelInputEvent* ppb_wheel_input_event_interface = NULL;
PPB_Fullscreen* ppb_fullscreen_interface = NULL;

EGbool EGPlatformNACL::get_interfaces(PPB_GetInterface get_browser)
{
	get_browser_interface = get_browser;
	ppb_core_interface = (PPB_Core*)(get_browser(PPB_CORE_INTERFACE));
	ppb_console_interface = (PPB_Console*)get_browser(PPB_CONSOLE_INTERFACE);
  	ppb_g3d_interface = (PPB_Graphics3D*)get_browser(PPB_GRAPHICS_3D_INTERFACE);
  	ppb_input_event_interface = (PPB_InputEvent*)get_browser(PPB_INPUT_EVENT_INTERFACE);
  	ppb_instance_interface = (PPB_Instance*)get_browser(PPB_INSTANCE_INTERFACE);
  	ppb_messaging_interface = (PPB_Messaging*)(get_browser(PPB_MESSAGING_INTERFACE));
  	ppb_urlloader_interface = (PPB_URLLoader*)(get_browser(PPB_URLLOADER_INTERFACE));
  	ppb_urlrequestinfo_interface = (PPB_URLRequestInfo*)(get_browser(PPB_URLREQUESTINFO_INTERFACE));
  	ppb_var_interface = (PPB_Var*)(get_browser(PPB_VAR_INTERFACE));
  	ppb_view_interface = (PPB_View*)get_browser(PPB_VIEW_INTERFACE);
  	ppb_keyboard_input_event_interface = (PPB_KeyboardInputEvent*)get_browser(PPB_KEYBOARD_INPUT_EVENT_INTERFACE);
  	ppb_mouse_input_event_interface = (PPB_MouseInputEvent*)get_browser(PPB_MOUSE_INPUT_EVENT_INTERFACE);
  	ppb_touch_input_event_interface = (PPB_TouchInputEvent*)get_browser(PPB_TOUCH_INPUT_EVENT_INTERFACE);
  	ppb_wheel_input_event_interface = (PPB_WheelInputEvent*)get_browser(PPB_WHEEL_INPUT_EVENT_INTERFACE);
    ppb_fullscreen_interface = (PPB_Fullscreen*)get_browser(PPB_FULLSCREEN_INTERFACE);

  	return true;
}

#endif
