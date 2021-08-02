#pragma once
class VizForceSetImage
{
private:
	struct Plugin_Instance_Data
	{
		char* m_imagePath;
	};
	enum ARG_PLUGIN_PARAMETERS
	{
		IMAGE_PATH = 0,
		BUTTON_FORCE_SET_IMAGE,
		//BUTTON_RELOAD_IMAGE,
		ARG_COUNT               /*!< total number of arguments            */
	};

	/// contains the plugin data members.
	Plugin_Instance_Data m_data_;
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

	VizForceSetImage();
	~VizForceSetImage();

	VizForceSetImage(const VizForceSetImage * a_source);

	static int Plugin_Init_Ex();
	static void Plugin_Init();

	//void Plugin_After_Load();
	void Plugin_Exec_Action(int n_VarID);
};

