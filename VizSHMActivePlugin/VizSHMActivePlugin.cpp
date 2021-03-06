// VizSHMActivePlugin.cpp : Defines the exported functions for the DLL application.
//


#include "VizSHMActivePlugin.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CONTAINER.h>
#include <evPlugin_VIZ.h>
#include <evPlugin_SCENE.h>
#include <evPlugin_SHARED_MEMORY.h>


#if defined(_DEBUG)
	#define TRACE _vizprintf
#else
	#define TRACE __noop
#endif

#pragma warning (disable : 4100)    // unreferenced formal parameter



// ---------------------------------------------------------------------
//  set the name of your plugin & the folder ...
// ---------------------------------------------------------------------
const char* VizSHMActivePlugin::PLUGIN_NAME = "SHMActive";              // the name of this plugin
const char* VizSHMActivePlugin::PLUGIN_FOLDER = "SHM";       // the folder for this plugin
//const char* VizSHMActivePlugin::PLUGIN_NAME = "Example";              // the name of this plugin
//const char* VizSHMActivePlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizSHMActivePlugin::PLUGIN_VERSION = 1;                  // version number
const int   VizSHMActivePlugin::PLUGIN_RELEASE = 0;                  // release number
const int   VizSHMActivePlugin::PLUGIN_PATCHLEVEL = 0;                 // patchlevel info

VizSHMActivePlugin::VizSHMActivePlugin()
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

VizSHMActivePlugin::~VizSHMActivePlugin()
{
	TRACE("Destrucotor Called");
}

VizSHMActivePlugin::VizSHMActivePlugin(const VizSHMActivePlugin * a_source)
{
	TRACE("Copy Constructor Called");

	// do deep 

	m_data_.m_nameOfSHMKey = a_source->m_data_.m_nameOfSHMKey;
	m_data_.m_scope = a_source->m_data_.m_scope;
	m_data_.m_reverseValue = a_source->m_data_.m_reverseValue;
}

int VizSHMActivePlugin::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizSHMActivePlugin::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizSHMActivePlugin::PLUGIN_NAME);
	evRegisterPluginFolder(VizSHMActivePlugin::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizSHMActivePlugin::PLUGIN_VERSION,
		VizSHMActivePlugin::PLUGIN_RELEASE,
		VizSHMActivePlugin::PLUGIN_PATCHLEVEL);

	const char *ScopeOptions[] = { " SCENE ", " VIZ MACHINE ", " NETWORK ", NULL };

	evRegisterParameterString("SHM_Key", "SHM Key Name: ", "", 30, 100, "A-Za-z0-9");
	evRegisterParameterRadio("SHM_Scope", "SHM Scope: ", 0, &ScopeOptions[0]); 
	evRegisterParameterBool("SHM_ReverseValue", "Reverse Value", false);

	// always last
	evRegisterTotalSize(sizeof(VizSHMActivePlugin));
	TRACE("End of Init Reached");
}


void VizSHMActivePlugin::Plugin_After_Load()
{
	m_prevNameOfSHMKey = m_data_.m_nameOfSHMKey;
	Plugin_Changed_Callback(ARG_PLUGIN_PARAMETERS::ARG_SHM_SCOPE);
	RespondToSharedMemoryChange(0, m_prevNameOfSHMKey);
}

void VizSHMActivePlugin::Plugin_Changed_Callback(int n_VarID)
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
	case ARG_PLUGIN_PARAMETERS::ARG_SHM_REVERSE_VALUE:
	default:

		TRACE("    unknown parameter %d changed.\n", n_VarID);
		break;
	}


	m_currentSharedMemory.register_changed_callback(m_data_.m_nameOfSHMKey);

	RespondToSharedMemoryChange(0, m_data_.m_nameOfSHMKey);

	SCENE::redraw();
}

void VizSHMActivePlugin::RespondToSharedMemoryChange(int sharedMemoryId, const char * key)
{
	TRACE("Respond to Shared Memory Called\n");
	CONTAINER c;
	c.local();

	if (c.valid())
	{
		if (m_currentSharedMemory.valid())
		{
			TRACE("Got Scene\n");
			VARIANT_VAR value;
			if (m_currentSharedMemory.has_key(key) && strcmp(key, m_data_.m_nameOfSHMKey) == 0)
			{
				m_currentSharedMemory.get_value(key, value);

				bool active;
				value.get(active);

				if (m_data_.m_reverseValue)
					active = !active;

				c.set_active(active);
				TRACE("Active Changed\n");
			}
		}
	}
}
