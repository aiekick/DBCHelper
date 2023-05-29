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

#include "CStructsPane.h"

#include <Gui/MainFrame.h>

#include <imgui/imgui_internal.h>

#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Panes/Manager/LayoutManager.h>
#include <Modules/CStruct/CStructView.h>

#include <cinttypes> // printf zu
#include <cmath>

static int GeneratorPaneWidgetId = 0;

CStructsPane::CStructsPane() = default;
CStructsPane::~CStructsPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool CStructsPane::Init()
{
	return true;
}

void CStructsPane::Unit()
{

}

int CStructsPane::DrawPanes(int vWidgetId, std::string vUserDatas)
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
			DrawView();
		}

		ImGui::End();
	}

	return GeneratorPaneWidgetId;
}

void CStructsPane::DrawDialogsAndPopups(std::string vUserDatas)
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

int CStructsPane::DrawWidgets(int vWidgetId, std::string vUserDatas)
{
	UNUSED(vUserDatas);

	return vWidgetId;
}

std::string CStructsPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool CStructsPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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
//// PUBLIC ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void CStructsPane::ConnectToCStructView(ct::cWeak<CStructView> vCStructView)
{
	m_CStructView = vCStructView;
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void CStructsPane::DrawView()
{
	auto cstructViewPtr = m_CStructView.getValidShared();
	if (cstructViewPtr)
	{
		cstructViewPtr->DrawView();
	}
}