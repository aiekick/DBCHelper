// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright 2021-2023 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <Gui/MainFrame.h>

// SoGlSl/3rdparty
#include <ctools/FileHelper.h>
#include <ctools/Logger.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <ctools/GLVersionChecker.h>
#include <Res/CustomFont.h>
#include <Res/CustomFont2.h>
#include <GLFW/glfw3.h>
#include <Gui/ImWidgets.h>

#include <Headers/Build.h>
#include <Helper/ThemeHelper.h>

#include <Modules/Dbc/DBCModel.h>
#include <Modules/Dbc/DBCFile.h>
#include <Modules/Dbc/DBCView.h>
#include <Modules/Matlab/MatlabModel.h>
#include <Modules/Matlab/MatlabView.h>
#include <Modules/CStruct/CStructModel.h>
#include <Modules/CStruct/CStructView.h>

#include <Panes/Manager/LayoutManager.h>
#include <Panes/DBCTreePane.h>
#include <Panes/DBCViewPane.h>
#include <Panes/MatlabPane.h>
#include <Panes/CStructsPane.h>
#include <Panes/HelpPane.h>
#include <Panes/ErrorsPane.h>
#include <Panes/DebugPane.h>
#include <Panes/SynthesisPane.h>

#include <Helper/TranslationHelper.h>

///////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////
///////////////////////////////////////////////////////

GLFWwindow* MainFrame::sMainWindow = nullptr;

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

MainFrame::MainFrame()
{
	// on met rien qui appel MainFrame::Instance() pour pas creer une boucle infinie
}

MainFrame::~MainFrame()
{

}

void MainFrame::Unit()
{
	SaveConfigFile("config.xml", "");

	m_DBCViewPtr->Unit();
	m_DBCViewPtr.reset(); 
	m_DBCViewPtr = nullptr;
	m_DBCModelPtr->Unit();
	m_DBCModelPtr.reset();
	m_DBCModelPtr = nullptr;

	m_MatlabViewPtr->Unit();
	m_MatlabViewPtr.reset();
	m_MatlabViewPtr = nullptr;
	m_MatlabModelPtr->Unit();
	m_MatlabModelPtr.reset();
	m_MatlabModelPtr = nullptr;

	m_CStructViewPtr->Unit();
	m_CStructViewPtr.reset();
	m_CStructViewPtr = nullptr;
	m_CStructModelPtr->Unit();
	m_CStructModelPtr.reset();
	m_CStructModelPtr = nullptr;

	DBCTreePane::Instance()->Unit();
	DBCViewPane::Instance()->Unit();
	MatlabPane::Instance()->Unit();
	CStructsPane::Instance()->Unit();
	HelpPane::Instance()->Unit();
	ErrorsPane::Instance()->Unit();
	DebugPane::Instance()->Unit();
	SynthesisPane::Instance()->Unit();
}

bool MainFrame::Init()
{
	SetProjectTitle("");

	ImGui::CustomStyle::Init();
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".dbc", ImVec4(0.3f, 0.8f, 0.3f, 0.8f), ICON_NDP2_FILE_DOCUMENT);

	// important to init for avoid crash
	TranslationHelper::Instance();

	m_DBCModelPtr = std::make_shared<DBCModel>();
	m_DBCModelPtr->Init();
	m_DBCViewPtr = std::make_shared<DBCView>();
	m_DBCViewPtr->Init();
	m_DBCViewPtr->ConnectToDBCModel(m_DBCModelPtr);

	m_MatlabModelPtr = std::make_shared<MatlabModel>();
	m_MatlabModelPtr->Init();
	m_MatlabModelPtr->ConnectToDBCModel(m_DBCModelPtr);
	m_MatlabViewPtr = std::make_shared<MatlabView>();
	m_MatlabViewPtr->Init();
	m_MatlabViewPtr->ConnectToMatlabModel(m_MatlabModelPtr);

	m_CStructModelPtr = std::make_shared<CStructModel>();
	m_CStructModelPtr->Init();
	m_CStructModelPtr->ConnectToDBCModel(m_DBCModelPtr);
	m_CStructViewPtr = std::make_shared<CStructView>();
	m_CStructViewPtr->Init();
	m_CStructViewPtr->ConnectToCStructModel(m_CStructModelPtr);

	DBCTreePane::Instance()->Init();
	DBCTreePane::Instance()->ConnectToDBCView(m_DBCViewPtr);
	DBCViewPane::Instance()->Init();
	DBCViewPane::Instance()->ConnectToDBCView(m_DBCViewPtr);
	MatlabPane::Instance()->Init();
	MatlabPane::Instance()->ConnectToMatlabView(m_MatlabViewPtr);
	CStructsPane::Instance()->Init();
	CStructsPane::Instance()->ConnectToCStructView(m_CStructViewPtr);
	ErrorsPane::Instance()->Init();
	ErrorsPane::Instance()->ConnectToDBCModel(m_DBCModelPtr);
	HelpPane::Instance()->Init();
	DebugPane::Instance()->Init();
	SynthesisPane::Instance()->Init();
	SynthesisPane::Instance()->ConnectToDBCModel(m_DBCModelPtr);

	ThemeHelper::Instance();
	LoadConfigFile("config.xml");

	LayoutManager::Instance()->Init(TranslationHelper::layout_menu_name, TranslationHelper::layout_menu_help);
	LayoutManager::Instance()->AddPane(DBCTreePane::Instance(), TranslationHelper::layout_dbc_tree_pane_name, (1 << 0), PaneDisposal::LEFT, true, true);
	LayoutManager::Instance()->AddPane(DBCViewPane::Instance(), TranslationHelper::layout_dbc_view_pane_name, (1 << 1), PaneDisposal::CENTRAL, true, true);
	LayoutManager::Instance()->AddPane(MatlabPane::Instance(), TranslationHelper::layout_matlab_tools_pane_name, (1 << 2), PaneDisposal::CENTRAL, false, false);
	LayoutManager::Instance()->AddPane(CStructsPane::Instance(), TranslationHelper::layout_cstruct_tools_pane_name, (1 << 3), PaneDisposal::CENTRAL, false, false);
	LayoutManager::Instance()->AddPane(HelpPane::Instance(), TranslationHelper::layout_help_pane_name, (1 << 4), PaneDisposal::RIGHT, false, false);
	LayoutManager::Instance()->AddPane(ErrorsPane::Instance(), TranslationHelper::layout_errors_control_pane_name, (1 << 5), PaneDisposal::RIGHT, true, true);
	LayoutManager::Instance()->AddPane(SynthesisPane::Instance(), TranslationHelper::layout_errors_summary_pane_name, (1 << 6), PaneDisposal::BOTTOM, true, true);
	LayoutManager::Instance()->AddPane(DebugPane::Instance(), TranslationHelper::layout_tools_pane_name, (1 << 7), PaneDisposal::CENTRAL, false, false);

	LayoutManager::Instance()->SetPaneDisposalSize(PaneDisposal::LEFT, 300.0f);
	LayoutManager::Instance()->SetPaneDisposalSize(PaneDisposal::RIGHT, 300.0f);

	if (!m_DBCFileToLoad.empty())
	{
		if (m_DBCModelPtr->OpenDBCFile(m_DBCFileToLoad))
		{
			SetProjectTitle(m_DBCFileToLoad);
		}
	}

	if (!m_DBCFilesToMerge.empty())
	{
		m_DBCModelPtr->MergeDBCFiles(m_DBCFilesToMerge);
	}

	return true;
}

void MainFrame::Display(const ImVec2& vPos, const ImVec2& vSize)
{
	m_DisplayPos = vPos;
	m_DisplaySize = vSize;

	ImGui::CustomStyle::ResetCustomId();

	if (ImGui::BeginMainMenuBar())
	{
		DrawMainMenuBar();

		// ImGui Infos
		auto io = ImGui::GetIO();
		const auto label = ct::toStr("%.1f f/s", ImGui::GetIO().Framerate);
		const auto size = ImGui::CalcTextSize(label.c_str());

		static float s_translation_menu_size = 0.0f;
		
		ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - s_translation_menu_size - ImGui::GetStyle().FramePadding.x * 2.0f);
		ImGui::Text("%s", label.c_str());

		s_translation_menu_size = TranslationHelper::Instance()->DrawMenu();

		ImGui::EndMainMenuBar();
	}

	if (LayoutManager::Instance()->BeginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode))
	{
		//MainFrame::sCentralWindowHovered |= LayoutManager::Instance()->IsDockSpaceHoleHovered();

		LayoutManager::Instance()->EndDockSpace();
	}

	ThemeHelper::Instance()->Draw();
	ImGui::CustomStyle::pushId = LayoutManager::Instance()->DisplayPanes(ImGui::CustomStyle::pushId);

	//Draw();
	DrawFileDialogs();
	LayoutManager::Instance()->InitAfterFirstDisplay(vSize);
}

void MainFrame::SetProjectTitle(const std::string& vProjectTitle)
{
	std::string str;

	if (vProjectTitle.empty())
		str = ct::toStr("DBCHelper beta v%s", BuildId);
	else
		str = ct::toStr("DBCHelper beta v%s - DBC %s", BuildId, vProjectTitle.c_str());

	glfwSetWindowTitle(MainFrame::sMainWindow, str.c_str());
}

void MainFrame::NewFrame()
{
	ImGuiFileDialog::Instance()->puAnyWindowsHovered = false;
}

////////////////////////////////////////////////
//// PRIVATE ///////////////////////////////////
////////////////////////////////////////////////

void MainFrame::DrawMainMenuBar()
{
	if (ImGui::BeginMenu(TranslationHelper::mainframe_menubar_file))
	{
		if (ImGui::MenuItem(TranslationHelper::mainframe_menubar_open_dbc))
		{
			// open one dbc file
			ImGuiFileDialog::Instance()->OpenModal("OpenDBCFilesDialog", 
				TranslationHelper::mainframe_dialog_open_dbc, ".dbc",
				puOpenFileDialog.filePath, puOpenFileDialog.filePathName, 1, nullptr);
		}

		if (m_DBCModelPtr->IsLoaded())
		{
			// merge many dbc's file's
			if (ImGui::MenuItem(TranslationHelper::mainframe_menubar_merge_dbc))
			{
				ImGuiFileDialog::Instance()->OpenModal("MergeDBCFilesDialog", 
					TranslationHelper::mainframe_dialog_merge_dbc, ".dbc",
					puOpenFileDialog.filePath, puOpenFileDialog.filePathName, 0, nullptr);
			}

			// export the merged dbc file
			if (ImGui::MenuItem(TranslationHelper::mainframe_menubar_reload))
			{
				m_DBCModelPtr->Reload();
			}

			// export the merged dbc file
			if (ImGui::MenuItem(TranslationHelper::mainframe_menubar_export))
			{
				const char* filters = "DBC File{.dbc},Human Readable File{.txt}";
				//const char* filters = "DBC File{.dbc},Human Readable File{.txt},C Code File{.c}";
				ImGuiFileDialog::Instance()->OpenModal("ExportDBCFilesDialog", 
					TranslationHelper::mainframe_dialog_export, filters,
					puSaveFileDialog.filePath, puSaveFileDialog.filePathName, 0, nullptr);
			}

			if (ImGui::MenuItem(TranslationHelper::mainframe_menubar_close))
			{
				m_DBCModelPtr->Clear();
			}
		}

		ImGui::EndMenu();
	}

	ImGui::Spacing();

	LayoutManager::Instance()->DisplayMenu(m_DisplaySize);

	if (ImGui::BeginMenu(TranslationHelper::mainframe_menubar_settings))
	{
		ThemeHelper::Instance()->DrawMenu();

		ImGui::EndMenu();
	}

	ImGui::Spacing();
}

void MainFrame::DrawFileDialogs()
{
	// open one dbc file
	if (ImGuiFileDialog::Instance()->Display("OpenDBCFilesDialog", 
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			puOpenFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			puOpenFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			if (m_DBCModelPtr->OpenDBCFile(puOpenFileDialog.filePathName))
			{
				SetProjectTitle(puOpenFileDialog.filePathName);
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}

	// merge many dbc's file's
	if (ImGuiFileDialog::Instance()->Display("MergeDBCFilesDialog",
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			puOpenFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			puOpenFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			const auto selection = ImGuiFileDialog::Instance()->GetSelection();

			std::vector<std::string> arr;
			for (auto sel : selection)
				arr.push_back(sel.second);

			m_DBCModelPtr->MergeDBCFiles(arr);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	// export the merged dbc file
	if (ImGuiFileDialog::Instance()->Display("ExportDBCFilesDialog",
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			puOpenFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			puOpenFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			puOpenFileDialog.ext = ImGuiFileDialog::Instance()->GetCurrentFilter();

			if (puOpenFileDialog.ext == "DBC File")
				m_DBCModelPtr->ExportToFile(
					puOpenFileDialog.filePathName, DBCFile_ExportTypeEnum::EXPORT_TYPE_DBC);
			else if (puOpenFileDialog.ext == "Human Readable File")
				m_DBCModelPtr->ExportToFile(
					puOpenFileDialog.filePathName, DBCFile_ExportTypeEnum::EXPORT_TYPE_HUMAN);
			/*else if (puOpenFileDialog.ext == "C Code File")
				m_DBCModelPtr->ExportToFile(
					puOpenFileDialog.filePathName, DBCFile_ExportTypeEnum::EXPORT_TYPE_C);*/
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

std::string MainFrame::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	// the rest
	str += ThemeHelper::Instance()->getXml(vOffset, "app");
	str += LayoutManager::Instance()->getXml(vOffset, "app");
	str += TranslationHelper::Instance()->getXml(vOffset, "app");
	str += ErrorsPane::Instance()->getXml(vOffset, "app");
	str += DebugPane::Instance()->getXml(vOffset, "app");
	str += puOpenFileDialog.getXml(vOffset, vUserDatas);
	str += puSaveFileDialog.getXml(vOffset, vUserDatas);
	str += vOffset + "<igfd_bookmarks>" + ImGuiFileDialog::Instance()->SerializeBookmarks() + "</igfd_bookmarks>\n";
	str += vOffset + "<consoleverbose>" + ct::toStr(Logger::Instance()->ConsoleVerbose ? "true" : "false") + "</consoleverbose>\n";

	if (m_DBCModelPtr)
	{
		if (m_DBCModelPtr->GetBaseDBCFile())
		{
			str += vOffset + "<MainDBCFile>" + m_DBCModelPtr->GetBaseDBCFile()->filePathName + "</MainDBCFile>\n";
		}

		if (!m_DBCModelPtr->GetContainterMergedDBCFiles().empty())
		{
			str += vOffset + "<MergedDBCFiles>\n";

			for (auto file : m_DBCModelPtr->GetContainterMergedDBCFiles())
			{
				if (file.second)
				{
					str += vOffset + "\t<file>" + file.second->filePathName + "</file>\n";
				}
			}

			str += vOffset + "</MergedDBCFiles>\n";
		}
	}

	return str;
}

bool MainFrame::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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
	
	ThemeHelper::Instance()->setFromXml(vElem, vParent, "app");
	LayoutManager::Instance()->setFromXml(vElem, vParent, "app");
	TranslationHelper::Instance()->setFromXml(vElem, vParent, "app");
	ErrorsPane::Instance()->setFromXml(vElem, vParent, "app");
	DebugPane::Instance()->setFromXml(vElem, vParent, "app");

	if (strName == "MainDBCFile")
	{
		m_DBCFileToLoad = strValue;
	}

	if (strParentName == "MergedDBCFiles")
	{
		if (strName == "file")
		{
			m_DBCFilesToMerge.push_back(strValue);
		}
	}

	puOpenFileDialog.setFromXml(vElem, vParent, vUserDatas);
	puSaveFileDialog.setFromXml(vElem, vParent, vUserDatas);

	return true; // continue for explore childs. need to return false if we want explore child ourselves
}
