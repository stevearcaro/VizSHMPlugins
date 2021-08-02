#pragma once
class VizRenderWithCameraPlugin
{

private:

	struct Plugin_Instance_Data
	{
		int m_renderWithCameraNumber;
	};
	enum ARG_PLUGIN_PARAMETERS
	{
		ARG_RENDER_WITH_CAMERA_NUMBER = 0,

		ARG_COUNT                   /*!< total number of arguments            */
	};

	/// contains the plugin data members.
	Plugin_Instance_Data m_data_;
	int m_vizCurrentCamera;


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

	VizRenderWithCameraPlugin();
	~VizRenderWithCameraPlugin();

	VizRenderWithCameraPlugin(const VizRenderWithCameraPlugin * a_source);

	static int Plugin_Init_Ex();
	static void Plugin_Init();

	void Plugin_After_Load();
	void Plugin_Changed_Callback(int n_VarID);

	void Plugin_Pre_Render_OpenGL(unsigned long ulBitMask, float fAlpha);
	void Plugin_Post_Render_OpenGL(unsigned long ulBitMask, float fAlpha);



};