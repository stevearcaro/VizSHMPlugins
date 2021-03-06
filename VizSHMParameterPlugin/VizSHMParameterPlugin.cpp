// VizSHMParameterPlugin.cpp : Defines the exported functions for the DLL application.
//


#include "VizSHMParameterPlugin.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CONTAINER.h>
#include <evPlugin_VIZ.h>
#include <evPlugin_SCENE.h>
#include <evPlugin_SHARED_MEMORY.h>
#include <evPlugin_FUNCTION.h>
#include <evPlugin_TEXTURE.h>
#include <evPlugin_IMAGE.h>
#include <evPlugin_PLUGIN_INSTANCE.h>

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
const char* VizSHMParameterPlugin::PLUGIN_NAME = "SHMParameter";              // the name of this plugin
const char* VizSHMParameterPlugin::PLUGIN_FOLDER = "SHM";       // the folder for this plugin
//const char* VizSHMParameterPlugin::PLUGIN_NAME = "Example";              // the name of this plugin
//const char* VizSHMParameterPlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizSHMParameterPlugin::PLUGIN_VERSION = 1;                  // version number
const int   VizSHMParameterPlugin::PLUGIN_RELEASE = 0;                  // release number
const int   VizSHMParameterPlugin::PLUGIN_PATCHLEVEL = 0;                 // patchlevel info


VizSHMParameterPlugin::VizSHMParameterPlugin()
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

VizSHMParameterPlugin::~VizSHMParameterPlugin()
{
	TRACE("Destrucotor Called");
}

VizSHMParameterPlugin::VizSHMParameterPlugin(const VizSHMParameterPlugin * a_source)
{
	TRACE("Copy Constructor Called");

	// do deep copy
	m_data_.m_nameOfSHMKey = a_source->m_data_.m_nameOfSHMKey;
	m_data_.m_scope = a_source->m_data_.m_scope;
	m_data_.m_container = a_source->m_data_.m_container;
}

int VizSHMParameterPlugin::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizSHMParameterPlugin::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizSHMParameterPlugin::PLUGIN_NAME);
	evRegisterPluginFolder(VizSHMParameterPlugin::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizSHMParameterPlugin::PLUGIN_VERSION,
		VizSHMParameterPlugin::PLUGIN_RELEASE,
		VizSHMParameterPlugin::PLUGIN_PATCHLEVEL);

	const char *ScopeOptions[] = { " SCENE ", " VIZ MACHINE ", " NETWORK ", NULL };
	const char *ContainerOptions[] = { " THIS ", " PREVIOUS ", " NEXT ", " PARENT ", NULL };
	const char *PositionOptions[] = { "  X  ", "  Y  ", "  Z  ", " X&Y ", " X&Z ", " Y&Z ", " ALL ", NULL };

	evRegisterParameterString("SHM_Key", "SHM Key Name: ", "", 30, 100, "A-Za-z0-9");
	evRegisterParameterRadio("SHM_Scope", "SHM Scope: ", 0, &ScopeOptions[0]);
	evRegisterParameterRadio("Container", "Container: ", 0, &ContainerOptions[0]);

	evRegisterParameterString("Plugin", "Plugin: ", "", 50, 150, "A-Za-z0-9_-");
	evRegisterParameterString("Patameter", "Parameter: ", "", 50, 150, "A-Za-z0-9_-");
	
	
	// always last
	evRegisterTotalSize(sizeof(VizSHMParameterPlugin));
	TRACE("End of Init Reached");
}


void VizSHMParameterPlugin::Plugin_After_Load()
{
	m_prevNameOfSHMKey = m_data_.m_nameOfSHMKey;
	Plugin_Changed_Callback(ARG_PLUGIN_PARAMETERS::ARG_SHM_SCOPE);

	// On load grab an possible value
	RespondToSharedMemoryChange(0, m_prevNameOfSHMKey);
}

void VizSHMParameterPlugin::Plugin_Changed_Callback(int n_VarID)
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

void VizSHMParameterPlugin::RespondToSharedMemoryChange(int sharedMemoryId, const char * key)
{
	FUNCTION f;
	
	TRACE("Respond to Shared Memory Called\n");
	CONTAINER c;
	c.local();

	c.get_function(f, VizSHMParameterPlugin::PLUGIN_NAME);
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
			TRACE(m_data_.m_nameOfSHMKey);
			if (m_currentSharedMemory.valid())
				TRACE("\nShared Memory is Valid!\n");
			else
				TRACE("\nShared Memory is NOT Valid!\n");

			if (m_currentSharedMemory.has_key(key) && strcmp( key, m_data_.m_nameOfSHMKey) == 0 )
			{
				TRACE("\n Got Key\n");
				VARIANT_VAR variant;
				m_currentSharedMemory.get_value(key, variant);
				
				TRACE(" DATA: \n");
				TRACE(m_data_.m_pluginInstance);

				PLUGIN_INSTANCE pluginInstance;
				c.get_plugin_instance(pluginInstance, m_data_.m_pluginInstance);

				TRACE(" plugin instance: %i\n", pluginInstance.id());

				if (pluginInstance.valid())
				{
					TRACE("Plugin Instance is Valid\n");
					PLUGIN_PARAMETER param;
					param.Init(pluginInstance, m_data_.m_parameter);
					//pluginInstance.get(param);
					if (param.valid())
					{
						TRACE("Setting\n");
						switch (param.get_data_type())
						{
						case PLUGIN_PARAMETER::EV_CONT:
						case PLUGIN_PARAMETER::EV_DIR_SEL:
						case PLUGIN_PARAMETER::EV_DATA:
						case PLUGIN_PARAMETER::EV_FILE_SEL:
						case PLUGIN_PARAMETER::EV_IMAGE:
						case PLUGIN_PARAMETER::EV_INFOTEXT:
						case PLUGIN_PARAMETER::EV_SUBSTANCE:
						case PLUGIN_PARAMETER::EV_TEXT:
						case PLUGIN_PARAMETER::EV_STRING:
						{
							TRACE("Getting String\n");
							STRING_TYPE value;
							variant.get(value);
							param = value.c_str();
						}
						break;
						case PLUGIN_PARAMETER::EV_SLIDER_FLOAT:
						case PLUGIN_PARAMETER::EV_FLOAT:
						{
							TRACE("Getting Double\n");
							double value;
							variant.get(value);
							param = value;
						}
						break;
						case PLUGIN_PARAMETER::EV_RADIO:
						case PLUGIN_PARAMETER::EV_DROPDOWNBOX:
						case PLUGIN_PARAMETER::EV_SLIDER_INT:
						case PLUGIN_PARAMETER::EV_INT:
						{
							TRACE("Getting Int\n");
							int value;
							variant.get(value);
							param = value;
						}
						break;
						case PLUGIN_PARAMETER::EV_BOOL:
						{
							TRACE("Getting Bool\n");
							bool value;
							variant.get(value);
							param = value;
						}
						break;
						}
						TRACE("Set\n");

						
						pluginInstance.set(param);
						//PLUGIN_PARAMETER::print_info("Rectangle");
						//param.setNotify();
						//pluginInstance.set_changed();

						TRACE("Aparently value is %f", param.get_data_float());
					}
				}
				
			}
		}
	}
}
