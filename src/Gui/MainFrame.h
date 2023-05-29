#pragma once

#include <ctools/cTools.h>
#include <Headers/Globals.h>
#include <ctools/ConfigAbstract.h>

class OpenFileDialogStruct : public conf::ConfigAbstract
{
public:
	std::string filePathName;
	std::string filePath;
	std::string ext;

	OpenFileDialogStruct()
	{
		filePath = ".";
	}

	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override
	{
		UNUSED(vUserDatas);

		std::string str;
		str += vOffset + "<OpenFileDialog>\n";
		str += vOffset + "\t<filepathname>" + filePathName + "</filepathname>\n";
		str += vOffset + "\t<filepath>" + filePath + "</filepath>\n";
		str += vOffset + "\t<ext>" + ext + "</ext>\n";
		str += vOffset + "</OpenFileDialog>\n";
		return str;
	}

	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override
	{
		UNUSED(vUserDatas);

		// The value of this child identifies the name of this element
		std::string strName = "";
		std::string strValue = "";
		std::string strParentName = "";

		strName = vElem->Value();
		if (vElem->GetText())
			strValue = vElem->GetText();
		if (vParent != nullptr)
			strParentName = vParent->Value();

		if (strParentName == "OpenFileDialog")
		{
			if (strName == "filepathname")
				filePathName = strValue;
			if (strName == "filepath")
				filePath = strValue;
			if (strName == "ext")
				ext = strValue;
		}

		return true;
	}
};

class SavePictureDialogStruct : public conf::ConfigAbstract
{
public:
	std::string filePathName;
	std::string filePath;
	std::string ext;

	SavePictureDialogStruct()
	{
		filePath = ".";
	}

	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override
	{
		UNUSED(vUserDatas);

		std::string str;
		str += vOffset + "<SaveFileDialog>\n";
		str += vOffset + "\t<filepathname>" + filePathName + "</filepathname>\n";
		str += vOffset + "\t<filepath>" + filePath + "</filepath>\n";
		str += vOffset + "\t<ext>" + ext + "</ext>\n";
		str += vOffset + "</SaveFileDialog>\n";
		return str;
	}

	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override
	{
		UNUSED(vUserDatas);

		// The value of this child identifies the name of this element
		std::string strName = "";
		std::string strValue = "";
		std::string strParentName = "";

		strName = vElem->Value();
		if (vElem->GetText())
			strValue = vElem->GetText();
		if (vParent != nullptr)
			strParentName = vParent->Value();

		if (strParentName == "SaveFileDialog")
		{
			if (strName == "filepathname")
				filePathName = strValue;
			if (strName == "filepath")
				filePath = strValue;
			if (strName == "ext")
				ext = strValue;
		}

		return true;
	}
};

class DBCView;
class DBCModel;
class CStructView;
class CStructModel;
class MatlabView;
class MatlabModel;
struct GLFWwindow;
class MainFrame : public conf::ConfigAbstract
{
public:
	static GLFWwindow* sMainWindow;

private:
	ImVec2 m_DisplayPos = ImVec2(0, 0);
	ImVec2 m_DisplaySize = ImVec2(1280, 720);
	OpenFileDialogStruct puOpenFileDialog;
	SavePictureDialogStruct puSaveFileDialog;
	std::shared_ptr<DBCView> m_DBCViewPtr = nullptr;
	std::shared_ptr<DBCModel> m_DBCModelPtr = nullptr;
	std::shared_ptr<CStructView> m_CStructViewPtr = nullptr;
	std::shared_ptr<CStructModel> m_CStructModelPtr = nullptr;
	std::shared_ptr<MatlabView> m_MatlabViewPtr = nullptr;
	std::shared_ptr<MatlabModel> m_MatlabModelPtr = nullptr;
	std::string m_DBCFileToLoad;
	std::vector<std::string> m_DBCFilesToMerge;

public:
	bool Init();
	void Unit();
	void NewFrame();
	void Display(const ImVec2& vPos, const ImVec2& vSize);
	void SetProjectTitle(const std::string& vProjectTitle);

private: // imgui
	void DrawMainMenuBar();
	void DrawFileDialogs();

private: // functionnal

public: // configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

public:
	static MainFrame* Instance()
	{
		static MainFrame _instance;
		return &_instance;
	}

protected:
	MainFrame(); // Prevent construction
	MainFrame(const MainFrame&) {}; // Prevent construction by copying
	MainFrame& operator =(const MainFrame&) { return *this; }; // Prevent assignment
	~MainFrame(); // Prevent unwanted destruction
};
