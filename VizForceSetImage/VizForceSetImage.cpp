#include "VizForceSetImage.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CONTAINER.h>
#include <evPlugin_VIZ.h>
#include <evPlugin_TEXTURE.h>
#include <evPlugin_IMAGE.h>

#if defined(_DEBUG)
#define TRACE _vizprintf
#else
#define TRACE __noop
#endif

#pragma warning (disable : 4100)    // unreferenced formal parameter



// ---------------------------------------------------------------------
//  set the name of your plugin & the folder ...
// ---------------------------------------------------------------------
const char* VizForceSetImage::PLUGIN_NAME = "ArcForceSetImage";              // the name of this plugin
const char* VizForceSetImage::PLUGIN_FOLDER = "Arcaro";       // the folder for this plugin
															//const char* VizSHMImagePlugin::PLUGIN_NAME = "Example";              // the name of this plugin
															//const char* VizSHMImagePlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizForceSetImage::PLUGIN_VERSION = 1;                  // version number
const int   VizForceSetImage::PLUGIN_RELEASE = 0;                  // release number
const int   VizForceSetImage::PLUGIN_PATCHLEVEL = 1;                 // patchlevel info




VizForceSetImage::VizForceSetImage()
{

	TRACE("Constructor Called\n");
	

}


VizForceSetImage::~VizForceSetImage()
{
	TRACE("Destructor Called");
}

VizForceSetImage::VizForceSetImage(const VizForceSetImage * a_source)
{
	TRACE("Copy Constructor Called");

	// do deep copy
}

int VizForceSetImage::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizForceSetImage::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizForceSetImage::PLUGIN_NAME);
	evRegisterPluginFolder(VizForceSetImage::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizForceSetImage::PLUGIN_VERSION,
		VizForceSetImage::PLUGIN_RELEASE,
		VizForceSetImage::PLUGIN_PATCHLEVEL);

	evRegisterParameterString("image_path", "Image Path: ", "", 100, 200, "A-Za-z0-9.\\/");
	evRegisterPushButton("force_set_image", "Force Set Image", ARG_PLUGIN_PARAMETERS::BUTTON_FORCE_SET_IMAGE);
	//evRegisterPushButton("reload_image", "Reload Image", ARG_PLUGIN_PARAMETERS::BUTTON_RELOAD_IMAGE);

	
	// always last
	evRegisterTotalSize(sizeof(VizForceSetImage));
	TRACE("End of Init Reached");
}


void VizForceSetImage::Plugin_Exec_Action(int n_VarId)
{
	switch (n_VarId)
	{
		case ARG_PLUGIN_PARAMETERS::BUTTON_FORCE_SET_IMAGE:
		{
			TRACE("Force Setting the Image\n");
			CONTAINER c;
			c.local();

			//std::string fullString = "D:\\steve\\Desktop\\whiteRoom.jpg";

			if (c.valid())
			{
				TEXTURE tex;
				c.get_texture(tex);

				if (!tex.valid())
				{
					TRACE("Setting texture firt time\n");
					c.create_texture(m_data_.m_imagePath);

				}
				else
				{
					TRACE("Deleting and recreating texture\n");

					//c.delete_texture();

					
					// Instead of deleting and recreating the texture, delete and receate the image
					// This preserves all the texture properties like scale and wrap mode
					// This code also immediately deletes the old image
					IMAGE image;
					image.createImage(m_data_.m_imagePath);
					
					IMAGE img;
					tex.get_image(img);
					

					tex.set_image(image);
					
					img.delete_image();
					//c.create_texture(image);
				}
			}

			break;
		}
		/*
		case ARG_PLUGIN_PARAMETERS::BUTTON_RELOAD_IMAGE:
		{
			TRACE("Function Called: Reload Image\n");
			CONTAINER c;    
			c.local();

			if (c.valid())
			{
				TEXTURE tex;
				c.get_texture(tex);

				if (tex.valid())
				{

					IMAGE img;
					tex.get_image(img);

					if (img.valid())
					{
					TRACE("Reloading the Image\n");
					img.rebuild_texture();
					}
				}
			}

			break;
		}*/
	}

	SCENE::redraw();
}