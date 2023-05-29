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

#include "CStructView.h"

#include <Headers/Globals.h>
#include <ctools/FileHelper.h>
#include <Gui/MainFrame.h>
#include <Gui/ImWidgets.h>

#include <Modules/CStruct/CStructModel.h>

////////////////////////////////////////////////////
//// CTOR/DTOR /////////////////////////////////////
////////////////////////////////////////////////////

CStructView::CStructView()
{

}

CStructView::~CStructView()
{

}

////////////////////////////////////////////////////
//// INIT/UNIT /////////////////////////////////////
////////////////////////////////////////////////////

bool CStructView::Init()
{
	return true;
}

void CStructView::Unit()
{

}

void CStructView::Clear()
{

}

////////////////////////////////////////////////////
//// CONNECTION TO THE MODEL ///////////////////////
////////////////////////////////////////////////////

void CStructView::ConnectToCStructModel(ct::cWeak<CStructModel> vCStructModel)
{
	m_CStructModel = vCStructModel;
}

////////////////////////////////////////////////////
//// IMGUI DRAW ////////////////////////////////////
////////////////////////////////////////////////////

void CStructView::DrawView()
{
#ifdef USE_CSTRUCT_MODULES
	auto cstructModelPtr = m_CStructModel.getValidShared();
	if (!cstructModelPtr)
		return;

	if (ImGui::CollapsingHeader("C Structure Generation from the DBC"))
	{
		if (ImGui::ContrastedButton("Generate C Structures"))
		{
			cstructModelPtr->ComputeCStructStructures();
		}
		
		if (!cstructModelPtr->GetCStructStructures().empty())
		{
			static ImGuiTableFlags flags =
				ImGuiTableFlags_BordersV
				| ImGuiTableFlags_BordersOuterH
				| ImGuiTableFlags_Resizable
				| ImGuiTableFlags_RowBg
				| ImGuiTableFlags_NoBordersInBody
				//| ImGuiTableFlags_Sortable
				;

			if (ImGui::BeginTable("##tace", 2, flags))
			{
				ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
				ImGui::TableSetupColumn("Msg Name", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Structure", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();

				for (const auto& strs : cstructModelPtr->GetCStructStructures())
				{
					bool node_is_opened = false;
					if (ImGui::TableNextColumn())
					{
						if (ImGui::ContrastedButton("Copy to Clipboard"))
						{
							FileHelper::Instance()->SaveInClipBoard(MainFrame::sMainWindow, strs.second);
						}
						ImGui::SameLine();
						node_is_opened = ImGui::TreeNode((void*)&strs.first, "%s", strs.first.c_str());
					}
					if (ImGui::TableNextColumn())
					{
						if (node_is_opened)
						{
							ImGui::Text("%s", strs.second.c_str());
						}
						else
						{
							ImGui::Text("");
						}
					}

					if (node_is_opened)
						ImGui::TreePop();
				}

				ImGui::EndTable();
			}
		}
	}
#endif
}