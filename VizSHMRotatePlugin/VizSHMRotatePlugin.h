#pragma once

#if defined(_DEBUG)
#define TRACE _vizprintf
#else
#define TRACE __noop
#endif

#include <evPlugin_SHARED_MEMORY.h>

#include <algorithm>

class VizSHMRotatePlugin
{

private:
	struct Plugin_Instance_Data
	{
		char* m_nameOfSHMKey;
		int m_scope;
		int m_container;

		int m_positionToEdit;

		bool m_interpOn;
		float m_interpOutOf;
		float m_interpBottom;
		float m_interpTop;

		bool m_clampOn;
		float m_clampMin;
		float m_clampMax;
	};
	enum ARG_PLUGIN_PARAMETERS
	{
		ARG_NAME_OF_SHM_KEY = 0,            /*!< corresponds to lpsz_clip_file        */
		ARG_SHM_SCOPE,
		ARG_CONTAINER,
		ARG_POSITION_TO_EDIT,
		ARG_INTERP_ON,
		ARG_INTERP_OUT_OF,
		ARG_INTERP_BOTTOM,
		ARG_INTERP_TOP,
		ARG_CLAMP_ON,
		ARG_CLAMP_MIN,
		ARG_CLAMP_MAX,

		ARG_COUNT                   /*!< total number of arguments            */
	};

	/// contains the plugin data members.
	Plugin_Instance_Data m_data_;
	char* m_prevNameOfSHMKey;
	SHARED_MEMORY m_currentSharedMemory;
	//static std::vector< std::string > vLabels;

	void DoInterp(double a_rawValue);
	void DoClamp(double a_rawValue);

public:

	/// name of the plugin as it appears in the application
	static const char * PLUGIN_NAME;
	/// folder in which this plugin should appear in the application
	static const char * PLUGIN_FOLDER;
	/// version number
	static const int    PLUGIN_VERSION;
	/// release number
	static const int    PLUGIN_RELEASE;
	/// patchlevel number
	static const int    PLUGIN_PATCHLEVEL;

	VizSHMRotatePlugin(void);
	~VizSHMRotatePlugin();

	VizSHMRotatePlugin(const VizSHMRotatePlugin * a_source);

	static int Plugin_Init_Ex();
	static void Plugin_Init();

	void Plugin_After_Load();
	void Plugin_Changed_Callback(int n_VarID);


	void RespondToSharedMemoryChange(int a_sharedMemoryId, const char * a_key);

};