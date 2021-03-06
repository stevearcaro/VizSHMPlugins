// VizSHMImagePlugin.cpp : Defines the exported functions for the DLL application.
//


#include "VizSHMImagePlugin.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CONTAINER.h>
#include <evPlugin_VIZ.h>
#include <evPlugin_SCENE.h>
#include <evPlugin_SHARED_MEMORY.h>
#include <evPlugin_FUNCTION.h>
#include <evPlugin_TEXTURE.h>
#include <evPlugin_IMAGE.h>

#include <vizOGL.h>

#include <algorithm>




#if defined(_DEBUG)
	#define TRACE _vizprintf
#else
	#define TRACE __noop
#endif

#pragma warning (disable : 4100)    // unreferenced formal parameter



// ---------------------------------------------------------------------
//  set the name of your plugin & the folder ...
// ---------------------------------------------------------------------
const char* VizSHMImagePlugin::PLUGIN_NAME = "SHMImage";              // the name of this plugin
const char* VizSHMImagePlugin::PLUGIN_FOLDER = "SHM";       // the folder for this plugin
//const char* VizSHMImagePlugin::PLUGIN_NAME = "Example";              // the name of this plugin
//const char* VizSHMImagePlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizSHMImagePlugin::PLUGIN_VERSION = 1;                  // version number
const int   VizSHMImagePlugin::PLUGIN_RELEASE = 0;                  // release number
const int   VizSHMImagePlugin::PLUGIN_PATCHLEVEL = 0;                 // patchlevel info


VizSHMImagePlugin::VizSHMImagePlugin()
{
	//m_prevNameOfSHMKey = (char*)"default";

	TRACE("Constructor Called\n");
	SCENE s;
	s.local();

	TRACE("Got Scene\n");
	if (s.valid())
	{

		TRACE("Scene Valid\n");
		SHARED_MEMORY shr;
		s.get_shared_memory(shr);

		//shr.register_changed_callback("key");
		m_currentSharedMemory = shr;
		TRACE("Registered\n");
	}
	

}

VizSHMImagePlugin::~VizSHMImagePlugin()
{
	TRACE("Destrucotor Called");
}

VizSHMImagePlugin::VizSHMImagePlugin(const VizSHMImagePlugin * a_source)
{
	TRACE("Copy Constructor Called");

	// do deep copy
	m_data_.m_nameOfSHMKey = a_source->m_data_.m_nameOfSHMKey;
	m_data_.m_scope = a_source->m_data_.m_scope;
	m_data_.m_container = a_source->m_data_.m_container;
}

int VizSHMImagePlugin::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizSHMImagePlugin::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizSHMImagePlugin::PLUGIN_NAME);
	evRegisterPluginFolder(VizSHMImagePlugin::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizSHMImagePlugin::PLUGIN_VERSION,
		VizSHMImagePlugin::PLUGIN_RELEASE,
		VizSHMImagePlugin::PLUGIN_PATCHLEVEL);

	const char *ScopeOptions[] = { " SCENE ", " VIZ MACHINE ", " NETWORK ", NULL };
	const char *ContainerOptions[] = { " THIS ", " PREVIOUS ", " NEXT ", " PARENT ", NULL };
	const char *PositionOptions[] = { "  X  ", "  Y  ", "  Z  ", " X&Y ", " X&Z ", " Y&Z ", " ALL ", NULL };

	evRegisterParameterString("SHM_Key", "SHM Key Name: ", "", 30, 100, "A-Za-z0-9");
	evRegisterParameterRadio("SHM_Scope", "SHM Scope: ", 0, &ScopeOptions[0]);
	evRegisterParameterRadio("Container", "Container: ", 0, &ContainerOptions[0]);

	evRegisterParameterString("Prefix", "Prefix: ", "", 50, 150, "A-Za-z0-9.\\/");
	evRegisterParameterString("Suffix", "Suffix: ", "", 50, 150, "A-Za-z0-9.\\/");
	evRegisterParameterBool("HideOnEmpty", "Hide On Empty: ", true);
	// always last
	evRegisterTotalSize(sizeof(VizSHMImagePlugin));
	TRACE("End of Init Reached");
}


void VizSHMImagePlugin::Plugin_After_Load()
{
	m_prevNameOfSHMKey = m_data_.m_nameOfSHMKey;
	Plugin_Changed_Callback(ARG_PLUGIN_PARAMETERS::ARG_SHM_SCOPE);
	RespondToSharedMemoryChange(0, m_prevNameOfSHMKey);
}

void VizSHMImagePlugin::Plugin_Changed_Callback(int n_VarID)
{
	m_currentSharedMemory.unregister_changed_callback(m_prevNameOfSHMKey);

	switch (n_VarID)
	{
	case ARG_PLUGIN_PARAMETERS::ARG_NAME_OF_SHM_KEY:
	{
		m_prevNameOfSHMKey = m_data_.m_nameOfSHMKey;
		
		break;
	}
	case ARG_PLUGIN_PARAMETERS::ARG_SHM_SCOPE:
	{
		if (m_data_.m_scope == 0) //strcmp( m_data_.m_scope, " SCENE "))
		{
			SCENE s;
			s.local();
			if (s.valid())
			{
				SHARED_MEMORY shr;
				s.get_shared_memory(shr);
				if(shr.valid())
					m_currentSharedMemory = shr;
			}
		}
		else if (m_data_.m_scope == 1) //strcmp(m_data_.m_scope, " VIZ MACHINE "))
		{
			SHARED_MEMORY shr;
			VIZ::get_global_shared_memory(shr);
			if(shr.valid())
				m_currentSharedMemory = shr;
		}
		else if (m_data_.m_scope == 2) //strcmp(m_data_.m_scope, " NETWORK "))
		{
			SHARED_MEMORY shr;
			VIZ::get_distributed_shared_memory(shr);
			if (shr.valid())
				m_currentSharedMemory = shr;
		}

		break;
	}
	default:

		TRACE("    unknown parameter %d changed.\n", n_VarID);
		break;
	}

	TRACE("Registering Callback\n");
	m_currentSharedMemory.register_changed_callback(m_data_.m_nameOfSHMKey);

	TRACE("Respond To Changed\n");
	RespondToSharedMemoryChange(0, m_data_.m_nameOfSHMKey);
	TRACE("After Respond to change\n");

	SCENE::redraw();
}

void VizSHMImagePlugin::RespondToSharedMemoryChange(int sharedMemoryId, const char * key)
{
	FUNCTION f;
	
	TRACE("Respond to Shared Memory Called\n");
	CONTAINER c;
	c.local();

	c.get_function(f, VizSHMImagePlugin::PLUGIN_NAME);
	bool active;
	f.get_active(active);
	if (!active)
	{
		TRACE("NOT ACTIVE\n");
		return;
	}

	switch (m_data_.m_container)
	{
	case 0:
		//c.local();
		break;
	case 1:
		c.prev();
		break;
	case 2:
		c.next();
		break;
	case 3:
		c.level_up();
		break;
	}


	if (c.valid())
	{
		if (m_currentSharedMemory.valid())
		{
			TRACE("Got Shared memory\n");
			TRACE("KEY: ");
			TRACE(key);

			if (m_currentSharedMemory.has_key(key) && strcmp(key, m_data_.m_nameOfSHMKey) == 0)
			{
				VARIANT_VAR variant;
				m_currentSharedMemory.get_value(key, variant);
				//std::string value;
				STRING_TYPE value;
				variant.get(value);

				std::string fullString = "";
				fullString.append(m_data_.m_prefix);
				fullString.append(value.c_str());
				fullString.append(m_data_.m_suffix);

				TEXTURE tex;
				c.get_texture(tex);

				if (!tex.valid())
				{
					TRACE("Setting texture firt time\n");
					c.create_texture(fullString.c_str());
					
				}
				else
				{
					TRACE("Deleting and recreating texture\n");
					// **** Experimenting with OpenGL to replace texture with same name ****
					//int glTexHandle;
					//tex.get_texture_handle(&glTexHandle);

					c.delete_texture();

					IMAGE image;
					image.createImage(fullString.c_str());

					c.create_texture(image);


					//IMAGE image;
					//tex.get_image(image);
					
					//image.delete_image();

					
					//c.delete_texture();
					//c.create_texture(fullString.c_str());

					
					//glDeleteTextures(1, (GLuint *)glTexHandle);
					//vizGL_glDeleteTextures(1, (GLuint *)glTexHandle);

					// *** below is original code ****
					//IMAGE image;
					//image.createImage(fullString.c_str());
					//
					//tex.set_image(image);
				}

				// Check exists
				struct stat buffer;
				if (stat(fullString.c_str(), &buffer) == 0)
					c.set_active(true);
				else
				{
					if (m_data_.m_hideOnEmpty)
						c.set_active(false);
				}
				
			}
		}
	}
}
