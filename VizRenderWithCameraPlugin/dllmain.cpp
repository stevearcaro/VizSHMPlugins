/*! \file Example.cpp
*  \brief Example plugin entry.
*
* viz plugin interface callback functions ...
* all functions herein are forwarders to the CV_Example class.
*/


/**
*  @defgroup example example group
*  This is the group for example plugin
* This plugin serves as an example.
* it shows all callback methods
*
* global compile time defines
- VIZRT_USE_INLINE should be defined in the release build but not during debug.
*
*
* \todo describe all the properties
* \todo describe the combatibility and enhancements issues.
*
* \todo the following items have no order
- all the test cases have to be written.
- use doxygen tagfiles.
*  @{
*/


#include "VizRenderWithCameraPlugin.h"

#include <evObjects.h>
#include <evCommon.h>

#define USE_PLUGIN_INIT_EX              1
// #define USE_PLUGIN_INIT                 1
#define USE_PLUGIN_INIT_FUNCTION        1
#define USE_PLUGIN_COPY_FUNCTION        1
#define USE_PLUGIN_CLEANUP_FUNCTION     1
#define USE_PLUGIN_CHANGED_CALLBACK     1
// #define USE_PLUGIN_NEW_GEOM             1
//#define USE_PLUGIN_EXEC_PER_FIELD       1
//#define USE_PLUGIN_EXEC_PER_FIELD2      1
//#define USE_PLUGIN_EXEC_ACTION          1
#define USE_PLUGIN_AFTER_LOAD           1
//#define USE_PLUGIN_AFTER_SAVE           1
//#define USE_PLUGIN_GUI_STATUS           1
#define USE_PLUGIN_PRE_RENDER_OPENGL    1
//#define PLUGIN_RENDER_OPENGL            1
#define USE_PLUGIN_POST_RENDER_OPENGL   1


#if defined(_DEBUG)
#define TRACE _vizprintf
#else
#define TRACE __noop
#endif


#if defined( USE_PLUGIN_INIT_EX ) && ( USE_PLUGIN_INIT_EX > 0 )
/*! \brief new initialize plugin.
*
* is called during startup of viz to register the plugin
* viz 3.0 supports this new plugin-initialization callback function,
* which can return a error-status. This can be useful
* if you want viz to not initialize your plugin.
* (e.g.: no lizense for your plugin, no hardware found, etc.)
* PLUGIN_INIT_EX should return EV_PLUGIN_SUCCESS, if everything is fine,
* and viz should proceed to initialize this plugin.
* \sa evPluginError.h for error constants
*/
PLUGINFUNC int PLUGIN_INIT_EX()
{
	TRACE("PLUGIN_INIT_EX");
	return VizRenderWithCameraPlugin::Plugin_Init_Ex();
}
#endif


#if defined( USE_PLUGIN_INIT ) && ( USE_PLUGIN_INIT > 0 )
/*! \brief initialize plugin.
*
* This callback function is called by viz only once per session.
* The plugin needs to register its argument structure here.
* \note You have to implement this function!
*/
PLUGINFUNC void PLUGIN_INIT()
{
	VizRenderWithCameraPlugin::Plugin_Init();
}
#endif


#if defined( USE_PLUGIN_INIT_FUNCTION ) && ( USE_PLUGIN_INIT_FUNCTION > 0 )
/*! \brief initialize instance specific data
*
* this function is called every time the user assigns this plugin
* to a new container or the scene.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
*/
PLUGINFUNC void PLUGIN_INIT_FUNCTION(void * pvDataPtr)
{
	TRACE("PLUGIN_INIT_FUNCTION");
	/* CV_Example * plugin = */new(pvDataPtr)VizRenderWithCameraPlugin;
}
#endif



#if defined( USE_PLUGIN_COPY_FUNCTION ) && ( USE_PLUGIN_COPY_FUNCTION > 0 )
/*! \brief initialize instance specific data
*
* is called to construct a new instance of your plugin as a
* copy of another instance of the same plugin.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] pvSource pointer to a POD. The Plugin instance data to be copied from.
*/
PLUGINFUNC void PLUGIN_COPY_FUNCTION(void * pvDataPtr, const void * pvSource)
{
	TRACE("PLUGIN_COPY_FUNCTION");
	/* CV_Example * plugin = */new(pvDataPtr) VizRenderWithCameraPlugin(static_cast< const VizRenderWithCameraPlugin * >(pvSource));
}
#endif



#if defined( USE_PLUGIN_CLEANUP_FUNCTION ) && ( USE_PLUGIN_CLEANUP_FUNCTION > 0 )
/*! \brief called upon destruction of this instance.
*
* this function is called if this plugin instance is going to be
* destroyed.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
*/
PLUGINFUNC void PLUGIN_CLEANUP_FUNCTION(void * pvDataPtr)
{
	TRACE("PLUGIN_CLEANUP_FUNCTION");
	VizRenderWithCameraPlugin * plugin = static_cast< VizRenderWithCameraPlugin * >(pvDataPtr);
	plugin->~VizRenderWithCameraPlugin();
}
#endif


#if defined( USE_PLUGIN_CHANGED_CALLBACK ) && ( USE_PLUGIN_CHANGED_CALLBACK > 0 )
/*! \brief change of a parameter in the GUI.
*
* is called if a parameter in the GUI has changed.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] iVarID is the index of the parameter which has been changed.
*            this index starts from 0 and increases by each registered parameter
*            calling evRegisterParameter<Type>
*/
PLUGINFUNC void PLUGIN_CHANGED_CALLBACK(void * pvDataPtr, int iVarID)
{
	TRACE("PLUGIN_CHANGED_CALLBACK");
	VizRenderWithCameraPlugin * plugin = static_cast< VizRenderWithCameraPlugin * >(pvDataPtr);
	plugin->Plugin_Changed_Callback(iVarID);
}
#endif

/*
VIZPLUGIN_API void PLUGIN_SHARED_MEMORY_VARIABLE_CHANGED(void * pInstanceData, int sharedMemoryId, const char * key)
{
	TRACE("PLUGIN_SHARED_MEMORY_VARIABLE_CHANGED");
	VizRenderWithCameraPlugin* plugin = static_cast<VizRenderWithCameraPlugin*>(pInstanceData);
	plugin->RespondToSharedMemoryChange(sharedMemoryId, key);
}
*/

#if defined( USE_PLUGIN_NEW_GEOM ) && ( USE_PLUGIN_NEW_GEOM > 0 )
/*! \brief is called only for geometry plugins.
*
* called if it's necessary to rebuild the geometry.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] iLevelOfDetail is the requested level of detail to be rebuilt.
*/
PLUGINFUNC void PLUGIN_NEW_GEOM(void *pvDataPtr, int iLevelOfDetail)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Plugin_New_Geom(iLevelOfDetail);
}
#endif


#if defined( USE_PLUGIN_EXEC_PER_FIELD ) && ( USE_PLUGIN_EXEC_PER_FIELD > 0 )
/*! \brief This function is called once for every field rendered
*
*  The exact time this callback function is called depends on its type:
* \verbatim
plugin type        call time
------------------------------------
geometry                  before geometry is drawn
container                 before drawing matrix for the container is calculated
scene                     after animation calculation
camera                    n/a
light                     n/a
\endverbatim
*
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
*/
PLUGINFUNC void PLUGIN_EXEC_PER_FIELD(void *pvDataPtr)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Exec_Per_Field_1();
}
#endif


#if defined( USE_PLUGIN_EXEC_PER_FIELD2 ) && ( USE_PLUGIN_EXEC_PER_FIELD2 > 0 )
/*! \brief This function is called once for every field rendered
*
*  The exact time this callback function is called depends on its type:
* \verbatim
plugin type        call time
------------------------------------
geometry                  never
container                 never
scene                     between fetching external camera data and recomputing the camera matrix
camera                    n/a
light                     n/a
\endverbatim
*
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
*/
PLUGINFUNC void PLUGIN_EXEC_PER_FIELD2(void *pvDataPtr)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Exec_Per_Field_2();
}
#endif



#if defined( USE_PLUGIN_EXEC_ACTION ) && ( USE_PLUGIN_EXEC_ACTION > 0 )
/*! \brief an action occured.
*
* is called if an action occured. e.g. a push button is pressed in the GUI.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] n_VarID is the index of the action which has occured.
*            this index is the same as with the registration during
*            calling evRegisterPushButton
*/
PLUGINFUNC void PLUGIN_EXEC_ACTION(void *pvDataPtr, int n_VarID)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Plugin_Exec_Action(n_VarID);
}
#endif



#if defined( USE_PLUGIN_AFTER_LOAD ) && ( USE_PLUGIN_AFTER_LOAD > 0 )
/*! \brief after scene load.
*
* This function is called after viz loaded a scene that includes this
* plugin.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
*/
PLUGINFUNC void PLUGIN_AFTER_LOAD(void *pvDataPtr)
{
	VizRenderWithCameraPlugin * plugin = static_cast< VizRenderWithCameraPlugin * >(pvDataPtr);
	plugin->Plugin_After_Load();
}
#endif



#if defined( USE_PLUGIN_AFTER_SAVE ) && ( USE_PLUGIN_AFTER_SAVE > 0 )
/*! \brief after scene save.
*
* This function is called for each instance after viz saves.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
*/
PLUGINFUNC void PLUGIN_AFTER_SAVE(void *pvDataPtr)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Plugin_After_Save();
}
#endif



#if defined( USE_PLUGIN_GUI_STATUS ) && ( USE_PLUGIN_GUI_STATUS > 0 )
/*! \brief status request for the GUI.
*
* This function is called everytime the GUI wants to refresh the
* state of the user-interface.
* This is the place where your plugin can set the UI
* enabled/disabled state.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \note You can remove it, if your plugin does not change the GUI state.
*/
PLUGINFUNC void PLUGIN_GUI_STATUS(void *pvDataPtr)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Plugin_Gui_Status();
}
#endif


#if defined( USE_PLUGIN_PRE_RENDER_OPENGL ) && ( USE_PLUGIN_PRE_RENDER_OPENGL > 0 )
/*! \brief prior rendering step.
*
* This function is called by viz prior to the render step.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] ulBitMask the bitmask states
* \param[in] fAlpha is the transparency of the container. if you are going to use color in the OpenGL part
*            you have to multiply your alpha with the fAlpha parameter.
* \sa PLUGIN_RENDER_OPENGL(), PLUGIN_POST_RENDER_OPENGL()
*
*/
PLUGINFUNC void PLUGIN_PRE_RENDER_OPENGL(void *pvDataPtr, unsigned long ulBitMask, float fAlpha)
{
	VizRenderWithCameraPlugin * plugin = static_cast< VizRenderWithCameraPlugin * >(pvDataPtr);
	plugin->Plugin_Pre_Render_OpenGL(ulBitMask, fAlpha);
}
#endif



#if defined( USE_PLUGIN_RENDER_OPENGL ) && ( USE_PLUGIN_RENDER_OPENGL > 0 )
/*! \brief during rendering.
*
* This function is called by viz prior to the render step.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] ulBitMask the bitmask states
* \param[in] fAlpha is the transparency of the container. if you are going to use color in the OpenGL part
*            you have to multiply your alpha with the fAlpha parameter.
*
*/
PLUGINFUNC void PLUGIN_RENDER_OPENGL(void *pvDataPtr, unsigned long ulBitMask, float fAlpha)
{
	CV_Example * plugin = static_cast< CV_Example * >(pvDataPtr);
	plugin->Plugin_Render_OpenGL(ulBitMask, fAlpha);
}
#endif


#if defined( USE_PLUGIN_POST_RENDER_OPENGL ) && ( USE_PLUGIN_POST_RENDER_OPENGL > 0 )
/*! \brief after rendering.
*
* This function is called by viz at the end of a rendering cycle.
* \param[in] pvDataPtr pointer to a POD. The Plugin instance data is supposed
*            to reside here.
* \param[in] ulBitMask bitmask states
* \param[in] fAlpha is the transparency of the container. if you are going to use color in the OpenGL part
*            you have to multiply your alpha with the fAlpha parameter.
*
*/
PLUGINFUNC void PLUGIN_POST_RENDER_OPENGL(void *pvDataPtr, unsigned long ulBitMask, float fAlpha)
{
	VizRenderWithCameraPlugin * plugin = static_cast< VizRenderWithCameraPlugin * >(pvDataPtr);
	plugin->Plugin_Post_Render_OpenGL(ulBitMask, fAlpha);
}
#endif

/** @} */ // end of example


