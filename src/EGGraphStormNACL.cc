/*
 *  EGGraphStormNACL.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */


#include <sys/stat.h>
#include <sys/mount.h>
#include <pthread.h>

#include "EGGraphApp.h"
#include "EGPlatformNACL.h"
#include "EGGraphStormNACL.h"

#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"
#include "error_handling/error_handling.h"
#include "nacl_io/nacl_io.h"


/* EGGraphStormNACL */

static const char class_name[] = "EGGraphStormNACL";
static const char* rsrcInFilename = "/http/Resources.zip";
static const char* rsrcOutFilename = "/tmp/Resources.zip";

EGGraphStormInstanceMap EGGraphStormNACL::instances;

EGGraphStormNACL::EGGraphStormNACL(PP_Instance instance)
    : graphapp(new EGGraphApp()), instance(instance), stepping(false), graphicsInited(false), rsrcLoaded(false), appInited(false), inSwapBuffers(false)
{
}

void EGGraphStormNACL::graphicsInit(int width, int height)
{
    EGDebug("%s:%s initializing graphics\n", class_name, __func__);

    int32_t attribs[] = {
        PP_GRAPHICS3DATTRIB_RED_SIZE, 8,
        PP_GRAPHICS3DATTRIB_GREEN_SIZE, 8,
        PP_GRAPHICS3DATTRIB_BLUE_SIZE, 8,
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 0,
        PP_GRAPHICS3DATTRIB_SAMPLES, 8,
        PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 1,
        PP_GRAPHICS3DATTRIB_WIDTH, width,
        PP_GRAPHICS3DATTRIB_HEIGHT, height,
        PP_GRAPHICS3DATTRIB_NONE
    };

    context = ppb_g3d_interface->Create(instance, 0, attribs);
    int32_t success = ppb_instance_interface->BindGraphics(instance, context);
    if (success == PP_FALSE) {
        glSetCurrentContextPPAPI(0);
        EGError("%s:%s failed to set context\n", class_name, __func__);
        return;
    }
    glSetCurrentContextPPAPI(context);
    
    graphicsInited = true;
}

void EGGraphStormNACL::appInit()
{
    EGDebug("%s:%s initializing app\n", class_name, __func__);

    // init graph app
    graphapp->init();
    graphapp->graphappui = EGGraphAppUIPtr(new EGGraphAppUI(graphapp.get()));
    
    // spring layout settings
    EGSpringLayoutSettings settings(/* stiffness */        400.0f,
                                    /* repuslision */      400.0f,
                                    /* centerAttraction */ 0.0f, /* repulsion * 0.02 = 8.0f */
                                    /* damping */          0.5f,
                                    /* timeStep */         0.03f,
                                    /* maxSpeed */         1000.0f,
                                    /* maxEnergy */        0.01f);

    // setup graph
    EGGraphModifierList modifierList;
    modifierList.push_back(EGGraphModifierPtr(new EGGraphNodeColorModifier(EGGraphNodeColorTypeConstant, EGColor(0.8f ,0.8f ,0.8f ,1))));
    modifierList.push_back(EGGraphModifierPtr(new EGGraphEdgeColorModifier(EGGraphEdgeColorTypeConstant, EGColor(0.6f, 0.6f, 0.6f, 1))));

    EGGraphLayoutFactory::init();
    graphapp->graphappui->reset();

    appInited = true;
}

void EGGraphStormNACL::message(std::string message)
{
    messageLock.lock();
    messageOutbox.push_back(message);
    messageLock.unlock();
    PP_CompletionCallback cc = PP_MakeCompletionCallback(postMessage, this);
    ppb_core_interface->CallOnMainThread(0, cc, 0);
}

void EGGraphStormNACL::postMessage(void* userdata, int)
{
    EGGraphStormNACL *nacl = (EGGraphStormNACL*)userdata;
    nacl->messageLock.lock();
    for (std::list<std::string>::iterator mi = nacl->messageOutbox.begin(); mi != nacl->messageOutbox.end(); ) {
        std::string message = *mi;
        PP_Var msgvar = ppb_var_interface->VarFromUtf8(message.c_str(), message.length());
        ppb_messaging_interface->PostMessage(nacl->instance, msgvar);
        ppb_var_interface->Release(msgvar);
        mi = nacl->messageOutbox.erase(mi);
    }
    nacl->messageLock.unlock();
}

#if USE_NACL_EXCEPTIONS
static void handleExceptionCallback(void* userdata, int)
{
    char *json = (char*)userdata;
    PP_Var msgvar = ppb_var_interface->VarFromUtf8(json, strlen(json));
    ppb_messaging_interface->PostMessage(pp_current_instance, msgvar);
    ppb_var_interface->Release(msgvar);
    free(userdata);
}

static void handleException(const char *json)
{
    PP_CompletionCallback cc = PP_MakeCompletionCallback(handleExceptionCallback, strdup(json));
    ppb_core_interface->CallOnMainThread(0, cc, 0);
}
#endif

void EGGraphStormNACL::processEvent(PP_Resource input_event)
{
    static int modifiers = 0;
    
    PP_InputEvent_Type eventType = ppb_input_event_interface->GetType(input_event);
    switch (eventType) {
        case PP_INPUTEVENT_TYPE_MOUSEDOWN:
        case PP_INPUTEVENT_TYPE_MOUSEUP:
        case PP_INPUTEVENT_TYPE_MOUSEMOVE:
        {
            PP_InputEvent_MouseButton button = ppb_mouse_input_event_interface->GetButton(input_event);
            PP_Point point = ppb_mouse_input_event_interface->GetPosition(input_event);
            EGenum buttons = (button == PP_INPUTEVENT_MOUSEBUTTON_LEFT ? EGGraphAppMouseButtonLeft : 0) |
                             (button == PP_INPUTEVENT_MOUSEBUTTON_RIGHT ? EGGraphAppMouseButtonRight : 0);
            switch (eventType) {
                case PP_INPUTEVENT_TYPE_MOUSEDOWN:
                {
                    EGMouseEvent evt(EGMouseEvent::PRESSED, point.x, point.y,
                                    (button == PP_INPUTEVENT_MOUSEBUTTON_LEFT ? EGMouseEvent::LEFT_BUTTON : 0) |
                                    (button == PP_INPUTEVENT_MOUSEBUTTON_RIGHT ? EGMouseEvent::RIGHT_BUTTON : 0));
                    if (EGEventManager::postEvent(&evt)) goto out;
                    graphapp->mousePress(point.x, point.y, buttons);
                    break;
                }
                case PP_INPUTEVENT_TYPE_MOUSEUP:
                {
                    EGMouseEvent evt(EGMouseEvent::RELEASED, point.x, point.y,
                                    (button == PP_INPUTEVENT_MOUSEBUTTON_LEFT ? EGMouseEvent::LEFT_BUTTON : 0) |
                                    (button == PP_INPUTEVENT_MOUSEBUTTON_RIGHT ? EGMouseEvent::RIGHT_BUTTON : 0));
                    if (EGEventManager::postEvent(&evt)) goto out;
                    graphapp->mouseRelease(point.x, point.y, buttons);
                    break;
                }
                case PP_INPUTEVENT_TYPE_MOUSEMOVE:
                {
                    EGMouseEvent evt(EGMouseEvent::MOTION, point.x, point.y,
                                    (button == PP_INPUTEVENT_MOUSEBUTTON_LEFT ? EGMouseEvent::LEFT_BUTTON : 0) |
                                    (button == PP_INPUTEVENT_MOUSEBUTTON_RIGHT ? EGMouseEvent::RIGHT_BUTTON : 0));
                    if (EGEventManager::postEvent(&evt)) goto out;
                    graphapp->mouseMove(point.x, point.y, buttons);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case PP_INPUTEVENT_TYPE_WHEEL:
            break;
        case PP_INPUTEVENT_TYPE_CHAR:
        {
            EGuint len;
            PP_Var charvar = ppb_keyboard_input_event_interface->GetCharacterText(input_event);
            const char* charstr = ppb_var_interface->VarToUtf8(charvar, &len);
            int charcode = charstr && len == 1 ? charstr[0] : 0; // TODO UTF-8 chars greater than 1 byte
            EGKeyEvent evt(EGKeyEvent::PRESSED, 0, 0, charcode, 0, modifiers);
            EGEventManager::postEvent(&evt);
            break;
        }
        case PP_INPUTEVENT_TYPE_KEYDOWN:
        {
            int keycode = ppb_keyboard_input_event_interface->GetKeyCode(input_event);
            switch (keycode) {
                case 16: modifiers |= EGKeyEvent::MODIFIER_SHIFT; break;
                case 17: modifiers |= EGKeyEvent::MODIFIER_CTRL; break;
                case 18: modifiers |= EGKeyEvent::MODIFIER_ALT; break;
            }
            EGKeyEvent evt(EGKeyEvent::PRESSED, 0, 0, 0, keycode, modifiers);
            if (EGEventManager::postEvent(&evt)) goto out;

            switch (keycode) {
                case 'i':
                case 'I':
                    graphapp->showInfo = !graphapp->showInfo;
                    break;
                case 't':
                case 'T':
                    graphapp->showTree = !graphapp->showTree;
                    break;
            }
        }
        case PP_INPUTEVENT_TYPE_KEYUP:
        {
            int keycode = ppb_keyboard_input_event_interface->GetKeyCode(input_event);
            switch (keycode) {
                case 16: modifiers &= ~EGKeyEvent::MODIFIER_SHIFT; break;
                case 17: modifiers &= ~EGKeyEvent::MODIFIER_CTRL; break;
                case 18: modifiers &= ~EGKeyEvent::MODIFIER_ALT; break;
            }
            EGKeyEvent evt(EGKeyEvent::RELEASED, 0, 0, 0, keycode, modifiers);
            EGEventManager::postEvent(&evt);
            break;
        }
        case PP_INPUTEVENT_TYPE_TOUCHSTART:
            break;
        case PP_INPUTEVENT_TYPE_TOUCHMOVE:
            break;
        case PP_INPUTEVENT_TYPE_TOUCHEND:
            break;
        case PP_INPUTEVENT_TYPE_TOUCHCANCEL:
            break;
        default:
            break;
    }
out:
    display(false);
}

void EGGraphStormNACL::step(void *userdata, int)
{
    EGGraphStormNACL *nacl = (EGGraphStormNACL*)userdata;
    if (nacl->graphapp->graphappui->isRunning()) {
        nacl->graphapp->layoutImpl->step();
        PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::step, nacl);
        ppb_core_interface->CallOnMainThread(0, cc, 0);
    } else {
        nacl->stepping = false;
    }
}

void EGGraphStormNACL::display(EGbool reschedule)
{
    if (inSwapBuffers) {
        if (reschedule) {
            PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::displaySchedule, this);
            ppb_core_interface->CallOnMainThread(1000 / EGGraphStormNACL::HZ, cc, 0);
        }
        return;
    }
    if (!rsrcLoaded) {
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        if (!appInited) {
            appInit();
            graphapp->reshape(bufferWidth, bufferHeight);
            message("{ status: \"init-complete\" }");
            PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::step, this);
            ppb_core_interface->CallOnMainThread(0, cc, 0);
            stepping = true;
        } else if (graphapp->graphappui->isRunning() && !stepping) {
            PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::step, this);
            ppb_core_interface->CallOnMainThread(0, cc, 0);
            stepping = true;
        }
        graphapp->draw();
    }
    inSwapBuffers = true;
    if (reschedule) {
        PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::displayDoneSchedule, this);
        ppb_g3d_interface->SwapBuffers(context, cc);
    } else {
        PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::displayDone, this);
        ppb_g3d_interface->SwapBuffers(context, cc);
    }
}

void EGGraphStormNACL::displaySchedule(void *userdata, int)
{
    EGGraphStormNACL *nacl = (EGGraphStormNACL*)userdata;
    nacl->display(true);
}

void EGGraphStormNACL::displayDoneSchedule(void *userdata, int)
{
    EGGraphStormNACL *nacl = (EGGraphStormNACL*)userdata;
    nacl->inSwapBuffers = false;
    PP_CompletionCallback cc = PP_MakeCompletionCallback(EGGraphStormNACL::displaySchedule, nacl);
    ppb_core_interface->CallOnMainThread(1000 / EGGraphStormNACL::HZ, cc, 0);
}

void EGGraphStormNACL::displayDone(void *userdata, int)
{
    EGGraphStormNACL *nacl = (EGGraphStormNACL*)userdata;
    nacl->inSwapBuffers = false;
}

static void* LoadResources(void* userdata)
{
    EGGraphStormNACL *nacl = (EGGraphStormNACL*)userdata;
    FILE *rsrcInFile, *rsrcOutFile;
    struct stat statbuf;

    // get file length
    if (stat(rsrcInFilename, &statbuf) < 0) {
        EGError("%s:%s failed to stat %s: %s\n", class_name, __func__, rsrcInFilename, strerror(errno));
        return NULL;
    }
    EGDebug("%s:%s resource file size %d bytes\n", class_name, __func__, (int)statbuf.st_size);

    // open input file
    if ((rsrcInFile = fopen(rsrcInFilename, "r")) == NULL) {
        EGError("%s:%s failed to open %s: %s\n", class_name, __func__, rsrcInFilename, strerror(errno));
        return NULL;
    }

    // open output file
    if ((rsrcOutFile = fopen(rsrcOutFilename, "w")) == NULL) {
        EGError("%s:%s failed to open %s: %s\n", class_name, __func__, rsrcOutFilename, strerror(errno));
        fclose(rsrcInFile);
        return NULL;
    }

    // copy input file to output file
    char buf[40996];
    int len, totalCopied = 0;
    EGDebug("%s:%s copying %s to %s\n", class_name, __func__, rsrcInFilename, rsrcOutFilename);
    while((len = fread(buf, 1, sizeof(buf), rsrcInFile)) > 0) {
        if (fwrite(buf, 1, len, rsrcOutFile) != len) {
            EGError("%s:%s short write\n", class_name, __func__);
            break;
        }
        totalCopied += len;
    }

    // close files
    fclose(rsrcInFile);
    fclose(rsrcOutFile);

    if (totalCopied == (int)statbuf.st_size) {
        EGDebug("%s:%s copied %d bytes\n", class_name, __func__, totalCopied);

        // mount into resource filesystem
        EGResource::registerZip(EGResource::getFileAsResource(rsrcOutFilename));
        
        // init fonts
        EGText::init();
        
        // indicate to display thread that resources have been loaded
        nacl->rsrcLoaded = true;
    } else {
        EGError("%s:%s copied %d out of %d bytes\n", class_name, __func__, totalCopied, (int)statbuf.st_size);
    }

    return NULL;
}

/**
 * Called when the NaCl module is instantiated on the web page. The identifier
 * of the new instance will be passed in as the first argument (this value is
 * generated by the browser and is an opaque handle).  This is called for each
 * instantiation of the NaCl module, which is each time the <embed> tag for
 * this module is encountered.
 *
 * If this function reports a failure (by returning @a PP_FALSE), the NaCl
 * module will be deleted and DidDestroy will be called.
 * @param[in] instance The identifier of the new instance representing this
 *     NaCl module.
 * @param[in] argc The number of arguments contained in @a argn and @a argv.
 * @param[in] argn An array of argument names.  These argument names are
 *     supplied in the <embed> tag, for example:
 *       <embed id="nacl_module" dimensions="2">
 *     will produce two arguments, one named "id" and one named "dimensions".
 * @param[in] argv An array of argument values.  These are the values of the
 *     arguments listed in the <embed> tag.  In the above example, there will
 *     be two elements in this array, "nacl_module" and "2".  The indices of
 *     these values match the indices of the corresponding names in @a argn.
 * @return @a PP_TRUE on success.
 */
static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[])
{
    pp_current_instance = instance;

    EGDebug("%s:%s\n", class_name, __func__);

#if USE_NACL_EXCEPTIONS
    /* Request exception callbacks with JSON. */
    EHRequestExceptionsJson(handleException);
    
    /* Report back if the request was honored. */
    if (!EHHanderInstalled()) {
        EGDebug("%s:%s execption handling enabled\n", class_name, __func__);
    } else {
        EGDebug("%s:%s execption handling not available\n", class_name, __func__);
    }
#endif
    
    // init nacl io
    nacl_io_init_ppapi(instance, get_browser_interface);

    // mount /tmp using memfs
    mount("", "/tmp", "memfs", 0, "");

    // mount /http using httpfs
    mount("" /* source URI */, "/http" /* target */, "httpfs", 0, "");

    // create instance
    EGGraphStormNACL *nacl = new EGGraphStormNACL(instance);
    EGGraphStormNACL::instances.insert(EGGraphStormInstanceEntry(instance, EGGraphStormNACLPtr(nacl)));

    // start resource loader thread
    EGDebug("%s:%s Creating resource loader thread\n", class_name, __func__);
    if (pthread_create(&nacl->rsrcThread, NULL, LoadResources, nacl)) {
        EGError("%s:%s failed to create resource loader thread\n", class_name, __func__);
    }
    
    return PP_TRUE;
}

static PP_Bool InputEvent_HandleInputEvent(PP_Instance instance, PP_Resource input_event)
{
    EGGraphStormInstanceIterator nacli = EGGraphStormNACL::instances.find(instance);
    if (nacli != EGGraphStormNACL::instances.end()) {
        EGGraphStormNACLPtr naclptr = (*nacli).second;
        naclptr->processEvent(input_event);
    }

    return PP_TRUE;
}

/**
 * Called when the NaCl module is destroyed. This will always be called,
 * even if DidCreate returned failure. This routine should deallocate any data
 * associated with the instance.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 */
static void Instance_DidDestroy(PP_Instance instance)
{
    EGDebug("%s:%s\n", class_name, __func__);
    EGGraphStormNACL::instances.erase(instance);
}

/**
 * Called when the position, the size, or the clip rect of the element in the
 * browser that corresponds to this NaCl module has changed.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 * @param[in] position The location on the page of this NaCl module. This is
 *     relative to the top left corner of the viewport, which changes as the
 *     page is scrolled.
 * @param[in] clip The visible region of the NaCl module. This is relative to
 *     the top left of the plugin's coordinate system (not the page).  If the
 *     plugin is invisible, @a clip will be (0, 0, 0, 0).
 */
static void Instance_DidChangeView(PP_Instance instance,
                                   PP_Resource view_resource)
{
    EGGraphStormInstanceIterator nacli = EGGraphStormNACL::instances.find(instance);
    if (nacli != EGGraphStormNACL::instances.end()) {
        EGGraphStormNACL *nacl = (*nacli).second.get();
        struct PP_Rect position;
        ppb_view_interface->GetRect(view_resource, &position);        
        if (!nacl->graphicsInited) {
            // initialize graphics
            nacl->graphicsInit(position.size.width, position.size.height);

            // register for input events
            ppb_input_event_interface->RequestInputEvents(instance, PP_INPUTEVENT_CLASS_TOUCH | PP_INPUTEVENT_CLASS_MOUSE |
                                PP_INPUTEVENT_CLASS_KEYBOARD | PP_INPUTEVENT_CLASS_TOUCH);

            // start display loop
            nacl->display(true);
            
        } else {
            // resize buffer
            ppb_g3d_interface->ResizeBuffers(nacl->context, position.size.width, position.size.height);
        }
        nacl->bufferWidth = position.size.width;
        nacl->bufferHeight = position.size.height;
        nacl->graphapp->reshape(position.size.width, position.size.height);
    }
}

/**
 * Notification that the given NaCl module has gained or lost focus.
 * Having focus means that keyboard events will be sent to the NaCl module
 * represented by @a instance. A NaCl module's default condition is that it
 * will not have focus.
 *
 * Note: clicks on NaCl modules will give focus only if you handle the
 * click event. You signal if you handled it by returning @a true from
 * HandleInputEvent. Otherwise the browser will bubble the event and give
 * focus to the element on the page that actually did end up consuming it.
 * If you're not getting focus, check to make sure you're returning true from
 * the mouse click in HandleInputEvent.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 * @param[in] has_focus Indicates whether this NaCl module gained or lost
 *     event focus.
 */
static void Instance_DidChangeFocus(PP_Instance instance, PP_Bool has_focus) {}

/**
 * Handler that gets called after a full-frame module is instantiated based on
 * registered MIME types.  This function is not called on NaCl modules.  This
 * function is essentially a place-holder for the required function pointer in
 * the PPP_Instance structure.
 * @param[in] instance The identifier of the instance representing this NaCl
 *     module.
 * @param[in] url_loader A PP_Resource an open PPB_URLLoader instance.
 * @return PP_FALSE.
 */
static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance,
                                           PP_Resource url_loader)
{
  /* NaCl modules do not need to handle the document load function. */
  return PP_FALSE;
}

/**
 * Entry points for the module.
 * Initialize needed interfaces: PPB_Core, PPB_Messaging and PPB_Var.
 * @param[in] a_module_id module ID
 * @param[in] get_browser pointer to PPB_GetInterface
 * @return PP_OK on success, any other value on failure.
 */
PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id,
                                       PPB_GetInterface get_browser)
{
    if (!EGPlatformNACL::get_interfaces(get_browser)) {
        return PP_ERROR_FAILED;
    }
    if (!glInitializePPAPI(get_browser)) {
        return PP_ERROR_FAILED;
    }
    return PP_OK;
}

/**
 * Returns an interface pointer for the interface of the given name, or NULL
 * if the interface is not supported.
 * @param[in] interface_name name of the interface
 * @return pointer to the interface
 */
PP_EXPORT const void* PPP_GetInterface(const char* interface_name)
{
    if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
        static PPP_Instance instance_interface = {
            &Instance_DidCreate,
            &Instance_DidDestroy,
            &Instance_DidChangeView,
            &Instance_DidChangeFocus,
            &Instance_HandleDocumentLoad, 
        };
        return &instance_interface;
  } else if (strcmp(interface_name, PPP_INPUT_EVENT_INTERFACE) == 0) {
      static PPP_InputEvent input_event_interface = {
            &InputEvent_HandleInputEvent
      };
      return &input_event_interface;
  }
  
  return NULL;
}

/**
 * Called before the plugin module is unloaded.
 */
PP_EXPORT void PPP_ShutdownModule() {}
