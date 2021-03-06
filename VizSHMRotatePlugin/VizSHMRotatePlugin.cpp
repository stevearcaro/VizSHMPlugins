// VizSHMRotatePlugin.cpp : Defines the exported functions for the DLL application.
//


#include "VizSHMRotatePlugin.h"

#include <evCommon.h>
#include <evPluginError.h>
#include <evPlugin_CONTAINER.h>
#include <evPlugin_VIZ.h>
#include <evPlugin_SCENE.h>
#include <evPlugin_SHARED_MEMORY.h>
#include <evPlugin_TRANSFORMATION.h>
#include <evPlugin_POSITION.h>
#include <evPlugin_FUNCTION.h>

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
const char* VizSHMRotatePlugin::PLUGIN_NAME = "SHMRotate";              // the name of this plugin
const char* VizSHMRotatePlugin::PLUGIN_FOLDER = "SHM";       // the folder for this plugin
															   //const char* VizSHMRotatePlugin::PLUGIN_NAME = "Example";              // the name of this plugin
															   //const char* VizSHMRotatePlugin::PLUGIN_FOLDER = "Folder_Example";       // the folder for this plugin
const int   VizSHMRotatePlugin::PLUGIN_VERSION = 1;                  // version number
const int   VizSHMRotatePlugin::PLUGIN_RELEASE = 0;                  // release number
const int   VizSHMRotatePlugin::PLUGIN_PATCHLEVEL = 0;                 // patchlevel info

void VizSHMRotatePlugin::DoInterp(double a_rawValue)
{
	if (m_data_.m_interpOn)
	{
		a_rawValue = ((a_rawValue / m_data_.m_interpOutOf) * m_data_.m_interpTop) + m_data_.m_interpBottom;
	}
}

void VizSHMRotatePlugin::DoClamp(double a_rawValue)
{
	if (m_data_.m_clampOn)
	{
		TRACE("Clamping\n");
		a_rawValue = (a_rawValue < m_data_.m_clampMax) ? a_rawValue : m_data_.m_clampMax; //std::min((float)raw_value, m_data_.m_clampMax);
		a_rawValue = (a_rawValue > m_data_.m_clampMin) ? a_rawValue : m_data_.m_clampMin; //std::max((float)raw_value, m_data_.m_clampMin);
		TRACE("Clamped Value %f\n", a_rawValue);
	}
}

VizSHMRotatePlugin::VizSHMRotatePlugin()
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

VizSHMRotatePlugin::~VizSHMRotatePlugin()
{
	TRACE("Destrucotor Called");
}

VizSHMRotatePlugin::VizSHMRotatePlugin(const VizSHMRotatePlugin * a_source)
{
	TRACE("Copy Constructor Called");

	// do deep copy
	m_data_.m_nameOfSHMKey = a_source->m_data_.m_nameOfSHMKey;
	m_data_.m_scope = a_source->m_data_.m_scope;
	m_data_.m_container = a_source->m_data_.m_container;

	m_data_.m_positionToEdit = a_source->m_data_.m_positionToEdit;


	m_data_.m_interpOn = a_source->m_data_.m_interpOn;
	m_data_.m_interpOutOf = a_source->m_data_.m_interpOutOf;
	m_data_.m_interpBottom = a_source->m_data_.m_interpBottom;
	m_data_.m_interpTop = a_source->m_data_.m_interpTop;

	m_data_.m_clampOn = a_source->m_data_.m_clampOn;
	m_data_.m_clampMin = a_source->m_data_.m_clampMin;
	m_data_.m_clampMax = a_source->m_data_.m_clampMax;
}

int VizSHMRotatePlugin::Plugin_Init_Ex()
{
	TRACE("Init Ex Called");
	Plugin_Init();
	return VIZERROR::Success;
}

void VizSHMRotatePlugin::Plugin_Init()
{
	TRACE("Init Called");
	// register the plugin name (& type) to viz.
	evRegisterPlugin(VizSHMRotatePlugin::PLUGIN_NAME);
	evRegisterPluginFolder(VizSHMRotatePlugin::PLUGIN_FOLDER);
	evRegisterPluginType(EV_FUNCTION_CONTAINER);
	// optionally, but a good idea in general
	evRegisterPluginVersion(VizSHMRotatePlugin::PLUGIN_VERSION,
		VizSHMRotatePlugin::PLUGIN_RELEASE,
		VizSHMRotatePlugin::PLUGIN_PATCHLEVEL);

	const char *ScopeOptions[] = { " SCENE ", " VIZ MACHINE ", " NETWORK ", NULL };
	const char *ContainerOptions[] = { " THIS ", " PREVIOUS ", " NEXT ", " PARENT ", NULL };
	const char *PositionOptions[] = { "  X  ", "  Y  ", "  Z  ", " X&Y ", " X&Z ", " Y&Z ", " ALL ", NULL };

	evRegisterParameterString("SHM_Key", "SHM Key Name: ", "", 30, 100, "A-Za-z0-9");
	evRegisterParameterRadio("SHM_Scope", "SHM Scope: ", 0, &ScopeOptions[0]);
	evRegisterParameterRadio("Container", "Container: ", 0, &ContainerOptions[0]);

	evRegisterParameterRadio("POS_Option", "Position(s) To Edit: ", 0, &PositionOptions[0]);

	evRegisterParameterBool("InterpOn", "Interpolate On: ", false);

	evRegisterParameterFloat("InterpOutOf", "Interp Out Of ( % = 100 ): ", 100, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
	evRegisterParameterFloat("InterpMin", "Interp Bottom: ", 0, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
	evRegisterParameterFloat("InterpMax", "Interp Top: ", 100, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());

	evRegisterParameterBool("ClampOn", "Clamp: ", false);
	evRegisterParameterFloat("ClampMin", "Clamp Min: ", 0, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
	evRegisterParameterFloat("ClampMax", "Clamp Max: ", 100, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());



	// always last
	evRegisterTotalSize(sizeof(VizSHMRotatePlugin));
	TRACE("End of Init Reached");
}


void VizSHMRotatePlugin::Plugin_After_Load()
{
	m_prevNameOfSHMKey = m_data_.m_nameOfSHMKey;
	Plugin_Changed_Callback(ARG_PLUGIN_PARAMETERS::ARG_SHM_SCOPE);
	RespondToSharedMemoryChange(0, m_prevNameOfSHMKey);
}

void VizSHMRotatePlugin::Plugin_Changed_Callback(int n_VarID)
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
				if (shr.valid())
					m_currentSharedMemory = shr;
			}
		}
		else if (m_data_.m_scope == 1) //strcmp(m_data_.m_scope, " VIZ MACHINE "))
		{
			SHARED_MEMORY shr;
			VIZ::get_global_shared_memory(shr);
			if (shr.valid())
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
	case ARG_PLUGIN_PARAMETERS::ARG_POSITION_TO_EDIT:
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

void VizSHMRotatePlugin::RespondToSharedMemoryChange(int sharedMemoryId, const char * key)
{
	FUNCTION f;

	TRACE("Respond to Shared Memory Called\n");
	CONTAINER c;
	c.local();

	c.get_function(f, VizSHMRotatePlugin::PLUGIN_NAME);
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

				TRANSFORMATION transform;
				c.get_transformation(transform);

				VIZ_POSITION pos;
				transform.get_position(pos);

				float x, y, z;
				pos.get(&x, &y, &z);


				VARIANT_VAR value;
				m_currentSharedMemory.get_value(key, value);

				VARIANT_VAR_ARRAY rawValues;
				double rawValue0, rawValue1, rawValue2;


				if (m_data_.m_positionToEdit < 3)
				{
					value.get(rawValue0);

					DoInterp(rawValue0);
					DoClamp(rawValue0);
				}
				else if (m_data_.m_positionToEdit > 2 && m_data_.m_positionToEdit < 7)
				{
					TRACE("Trying to get array\n");
					value.get(rawValues);
					if (rawValues.get_size() > 0)
					{
						rawValues[0].get(rawValue0);
						DoInterp(rawValue0);
						DoClamp(rawValue0);
					}
					if (rawValues.get_size() > 1)
					{
						rawValues[1].get(rawValue1);
						DoInterp(rawValue1);
						DoClamp(rawValue1);
					}
					if (rawValues.get_size() > 2)
					{
						rawValues[2].get(rawValue2);

						DoInterp(rawValue2);
						DoClamp(rawValue2);
						TRACE("Got Vector3 Size: %d Values: %f %f %f\n", rawValues.get_size(), rawValue0, rawValue1, rawValue2);
					}

				}

				// Edit Position
				switch (m_data_.m_positionToEdit)
				{
				case 0: // X
					if (rawValues.get_size() > 0)
						pos.set((float)rawValue0, y, z);
					break;
				case 1: // Y
					if (rawValues.get_size() > 0)
						pos.set(x, (float)rawValue0, z);
					break;
				case 2: // Z
					if (rawValues.get_size() > 0)
						pos.set(x, y, (float)rawValue0);
					break;

				case 3: // X&Y
					if (rawValues.get_size() > 1)
						pos.set(rawValue0, rawValue1, z);
					break;

				case 4: // X&Z
					if (rawValues.get_size() > 1)
						pos.set(rawValue0, y, rawValue1);
					break;

				case 5: // Y&Z
					if (rawValues.get_size() > 1)
						pos.set(x, rawValue0, rawValue1);
					break;

				case 6: // ALL
					if (rawValues.get_size() > 2)
						pos.set(rawValue0, rawValue1, rawValue2);
					break;

				}

				TRACE("Active Changed\n");
			}
		}
	}
}
