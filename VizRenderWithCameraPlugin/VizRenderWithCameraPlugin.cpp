#include "VizRenderWithCameraPlugin.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CAMERA.h>
#include <evPlugin_SCENE.h>

#if defined(_DEBUG)
#define TRACE _vizprintf
#else
#define TRACE __noop
#endif

#pragma warning (disable : 4100)    // unreferenced formal parameter

// ---------------------------------------------------------------------
//  set the name of your plugin & the folder ...
// ---------------------------------------------------------------------
const char* VizRenderWithCameraPlugin::PLUGIN_NAME = "RenderWithCamera";              // the name of this plugin
const char* VizRenderWithCameraPlugin::PLUGIN_FOLDER = "Arcaro";       // the folder for this plugin
																	 //const char* VizLumaShaderPlugin::PLUGIN_NAME = "Example";              // the name of this plugin
																	 //const char* VizLumaShaderPlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizRenderWithCameraPlugin::PLUGIN_VERSION = 1;                  // version number
const int   VizRenderWithCameraPlugin::PLUGIN_RELEASE = 0;                  // release number
const int   VizRenderWithCameraPlugin::PLUGIN_PATCHLEVEL = 0;                 // patchlevel info



VizRenderWithCameraPlugin::VizRenderWithCameraPlugin()
{
	TRACE("Constructor Called\n");
}


VizRenderWithCameraPlugin::~VizRenderWithCameraPlugin()
{
	TRACE("Destrucotor Called");
}

VizRenderWithCameraPlugin::VizRenderWithCameraPlugin(const VizRenderWithCameraPlugin * a_source)
{

}

int VizRenderWithCameraPlugin::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizRenderWithCameraPlugin::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizRenderWithCameraPlugin::PLUGIN_NAME);
	evRegisterPluginFolder(VizRenderWithCameraPlugin::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizRenderWithCameraPlugin::PLUGIN_VERSION,
		VizRenderWithCameraPlugin::PLUGIN_RELEASE,
		VizRenderWithCameraPlugin::PLUGIN_PATCHLEVEL);

	evRegisterParameterInt("Camera", "Camera:", 1, 0, 24);

	// always last
	evRegisterTotalSize(sizeof(VizRenderWithCameraPlugin));
	TRACE("End of Init Reached");
}

void VizRenderWithCameraPlugin::Plugin_After_Load()
{
	SCENE scene;
	scene.local();
	// Set the current camera to be used
	TRACE("Setting to camera 10\n");
	scene.set_current_camera(10);

}

void VizRenderWithCameraPlugin::Plugin_Changed_Callback(int n_VarID)
{
	switch (n_VarID)
	{
	case ARG_PLUGIN_PARAMETERS::ARG_RENDER_WITH_CAMERA_NUMBER:
	{
		

		//TRACE("Scene: %i vizCurrentCamera: %i Setting to Camera: %i \n", scene.GetID(), m_vizCurrentCamera, m_data_.m_renderWithCameraNumber);
		// Set the current camera to be used
		//scene.set_current_camera(m_data_.m_renderWithCameraNumber);
		
		
	}
	break;
	}


	SCENE::redraw();
}
					
void VizRenderWithCameraPlugin::Plugin_Pre_Render_OpenGL(unsigned long ulBitMask, float fAlpha)
{
	// Save the current camera
	SCENE scene;
	scene.local();
	scene.get_current_camera(&m_vizCurrentCamera);
	
	// Set the current camera to be used
	scene.set_current_camera(m_data_.m_renderWithCameraNumber);// m_data_.m_renderWithCameraNumber);
	TRACE("PRE  Scene: %i vizCurrentCamera: %i Setting to Camera: %i \n", 
		scene.GetID(), 
		m_vizCurrentCamera, 
		m_data_.m_renderWithCameraNumber);
		
	
}

void VizRenderWithCameraPlugin::Plugin_Post_Render_OpenGL(unsigned long ulBitMask, float fAlpha)
{
	// Reset back to viz's original camera
	SCENE scene;
	scene.local();
	//scene.set_current_camera(m_vizCurrentCamera);

	//TRACE("POST Scene: %i vizCurrentCamera: %i Setting to Camera: %i \n",
	//	scene.GetID(),
	//	m_vizCurrentCamera,
	//	m_data_.m_renderWithCameraNumber);
}