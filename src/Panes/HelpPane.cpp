// This is an open source non-commercial project. Dear PVS-Studio, please check it.
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

#include "HelpPane.h"

#include <Gui/MainFrame.h>
#include <Headers/Build.h>
#include <imgui/imgui_internal.h>
#include <ImGuiMarkDown/imgui_markdown.h>
#include <Helper/TranslationHelper.h>
#include <ctools/FileHelper.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Panes/Manager/LayoutManager.h>
#include <Modules/Dbc/DBCView.h>

#include <cinttypes> // printf zu
#include <cmath>

static int GeneratorPaneWidgetId = 0;

HelpPane::HelpPane() = default;
HelpPane::~HelpPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void LinkCallback(ImGui::MarkdownLinkCallbackData vLinkData)
{
	std::string url(vLinkData.link, vLinkData.linkLength);
	if (!vLinkData.isImage)
	{
		FileHelper::Instance()->OpenUrl(url);
	}
}

static ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData vLinkData)
{
	UNUSED(vLinkData);

	// In your application you would load an image based on data_ input. Here we just use the imgui font texture.
	ImTextureID image = ImGui::GetIO().Fonts->TexID;
	// > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
	ImGui::MarkdownImageData imageData;
	imageData.isValid = true;
	imageData.useLinkCallback = false;
	imageData.user_texture_id = image;
	imageData.size = ImVec2(40.0f, 20.0f);
	return imageData;
}

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool HelpPane::Init()
{
	return true;
}

void HelpPane::Unit()
{

}

int HelpPane::DrawPanes(int vWidgetId, std::string vUserDatas)
{
	GeneratorPaneWidgetId = vWidgetId;

	if (LayoutManager::Instance()->m_Pane_Shown & m_PaneFlag)
	{
		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;// | ImGuiWindowFlags_MenuBar;
		if (ImGui::BeginFlag<PaneFlags>(m_PaneName,
			&LayoutManager::Instance()->m_Pane_Shown, m_PaneFlag, flags))
		{
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
			auto win = ImGui::GetCurrentWindowRead();
			if (win->Viewport->Idx != 0)
				flags |= ImGuiWindowFlags_NoResize;// | ImGuiWindowFlags_NoTitleBar;
			else
				flags = ImGuiWindowFlags_NoCollapse;// | ImGuiWindowFlags_MenuBar;
#endif
			DrawHelp();
		}

		ImGui::End();
	}

	return GeneratorPaneWidgetId;
}

void HelpPane::DrawDialogsAndPopups(std::string vUserDatas)
{
	/*ImVec2 min = MainFrame::Instance()->puDisplaySize * 0.5f;
	ImVec2 max = MainFrame::Instance()->puDisplaySize;

	if (ImGuiFileDialog::Instance()->Display("GenerateFileDlg", ImGuiWindowFlags_NoDocking, min, max))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			Generator::Instance()->Generate(filePath, fileName, vProjectFile);
		}

		ImGuiFileDialog::Instance()->CloseDialog("GenerateFileDlg");
	}*/
}

int HelpPane::DrawWidgets(int vWidgetId, std::string vUserDatas)
{
	UNUSED(vUserDatas);

	return vWidgetId;
}

std::string HelpPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool HelpPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : MAIN DRAW///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void HelpPane::DrawHelp()
{
	switch (TranslationHelper::s_HelpLanguage)
	{
	case LanguageEnum::EN:
		DrawHelp_EN();
		break;
	case LanguageEnum::FR:
		DrawHelp_FR();
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : HELP EN ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void HelpPane::DrawHelp_EN()
{
	ImGui::Text("Current Version %s", BuildId);
	
	if (ImGui::CollapsingHeader("How to use ?"))
	{
		static const std::string _markdownText = u8R"(To do)";
		DrawMarkdown(_markdownText);
	}

	if (ImGui::CollapsingHeader("ChangesLog"))
	{
		static const std::string _markdownText = u8R"(To do)";
		DrawMarkdown(_markdownText);
	}
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : HELP FR ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void HelpPane::DrawHelp_FR()
{
	ImGui::Text("Version courante %s", BuildId);
	
	if (ImGui::CollapsingHeader("Comment utiliser ?"))
	{
		static const std::string _markdownText = u8R"(A faire)";
		DrawMarkdown(_markdownText);
	}

	if (ImGui::CollapsingHeader("Log des changements"))
	{
		static const std::string _markdownText = u8R"(A faire)";
		DrawMarkdown(_markdownText);
	}
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : DRAW UTILS /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void HelpPane::DrawMarkdown(const std::string& vMarkdown)
{
	ImFont* font = ImGui::GetFont();

	// You can make your own Markdown function with your prefered string container and markdown config.
	// > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
	ImGui::MarkdownConfig mdConfig;
	mdConfig.linkCallback = LinkCallback;
	mdConfig.tooltipCallback = NULL;
	mdConfig.imageCallback = ImageCallback;
	mdConfig.linkIcon = "[link]";
	mdConfig.headingFormats[0] = { font, true };
	mdConfig.headingFormats[1] = { font, true };
	mdConfig.headingFormats[2] = { font, false };
	mdConfig.userData = NULL;
	ImGui::Markdown(vMarkdown.c_str(), vMarkdown.length(), mdConfig);
}