// VizSHMObjectPlugin.cpp : Defines the exported functions for the DLL application.
//


#include "VizSHMObjectPlugin.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CONTAINER.h>
#include <evPlugin_VIZ.h>
#include <evPlugin_SCENE.h>
#include <evPlugin_SHARED_MEMORY.h>
#include <evPlugin_FUNCTION.h>
#include <evPlugin_GEOMETRY.h>
#include <evPlugin_TEXT_CONTROL.h>

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
const char* VizSHMObjectPlugin::PLUGIN_NAME = "SHMText";              // the name of this plugin
const char* VizSHMObjectPlugin::PLUGIN_FOLDER = "SHM";       // the folder for this plugin
//const char* VizSHMObjectPlugin::PLUGIN_NAME = "Example";              // the name of this plugin
//const char* VizSHMObjectPlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizSHMObjectPlugin::PLUGIN_VERSION = 1;                  // version number
const int   VizSHMObjectPlugin::PLUGIN_RELEASE = 0;                  // release number
const int   VizSHMObjectPlugin::PLUGIN_PATCHLEVEL = 0;                 // patchlevel info


VizSHMObjectPlugin::VizSHMObjectPlugin()
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

VizSHMObjectPlugin::~VizSHMObjectPlugin()
{
	TRACE("Destrucotor Called");
}

VizSHMObjectPlugin::VizSHMObjectPlugin(const VizSHMObjectPlugin * a_source)
{
	TRACE("Copy Constructor Called");

	// do deep copy
	m_data_.m_nameOfSHMKey = a_source->m_data_.m_nameOfSHMKey;
	m_data_.m_scope = a_source->m_data_.m_scope;
	m_data_.m_container = a_source->m_data_.m_container;
}

int VizSHMObjectPlugin::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizSHMObjectPlugin::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizSHMObjectPlugin::PLUGIN_NAME);
	evRegisterPluginFolder(VizSHMObjectPlugin::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizSHMObjectPlugin::PLUGIN_VERSION,
		VizSHMObjectPlugin::PLUGIN_RELEASE,
		VizSHMObjectPlugin::PLUGIN_PATCHLEVEL);

	const char *ScopeOptions[] = { " SCENE ", " VIZ MACHINE ", " NETWORK ", NULL };
	const char *ContainerOptions[] = { " THIS ", " PREVIOUS ", " NEXT ", " PARENT ", NULL };
	const char *PositionOptions[] = { "  X  ", "  Y  ", "  Z  ", " X&Y ", " X&Z ", " Y&Z ", " ALL ", NULL };

	evRegisterParameterString("SHM_Key", "SHM Key Name: ", "", 30, 100, "A-Za-z0-9");
	evRegisterParameterRadio("SHM_Scope", "SHM Scope: ", 0, &ScopeOptions[0]);
	evRegisterParameterRadio("Container", "Container: ", 0, &ContainerOptions[0]);

	evRegisterParameterString("Prefix", "Prefix: ", "", 50, 150, "A-Za-z0-9.\\/");
	evRegisterParameterString("Suffix", "Suffix: ", "", 50, 150, "A-Za-z0-9.\\/");
	// always last
	evRegisterTotalSize(sizeof(VizSHMObjectPlugin));
	TRACE("End of Init Reached");
}


void VizSHMObjectPlugin::Plugin_After_Load()
{
	m_prevNameOfSHMKey = m_data_.m_nameOfSHMKey;
	Plugin_Changed_Callback(ARG_PLUGIN_PARAMETERS::ARG_SHM_SCOPE);
	RespondToSharedMemoryChange(0, m_prevNameOfSHMKey);
}

void VizSHMObjectPlugin::Plugin_Changed_Callback(int n_VarID)
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

void VizSHMObjectPlugin::RespondToSharedMemoryChange(int sharedMemoryId, const char * key)
{
	FUNCTION f;
	
	TRACE("Respond to Shared Memory Called\n");
	CONTAINER c;
	c.local();

	c.get_function(f, VizSHMObjectPlugin::PLUGIN_NAME);
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

				// Depth first search

				// On current container
				CONTAINER currentContainer = c;

				
				// Loop breadth (next)
				bool hasNext = true;
				while (hasNext == true)
				{

					// Loop (depth) level down
					do
					{
						// move down
						currentContainer.level_down();
						// check all plugins
						//currentContainer.
						// update values


					} while (currentContainer.has_level_down());

					// move to next container or break loop
					if (currentContainer.has_next())
					{
						currentContainer.next();
						hasNext = true;
					}
					else
						hasNext = false;
				}


			}
		}
	}
}
