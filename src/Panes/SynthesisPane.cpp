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

#include "SynthesisPane.h"

#include <Gui/MainFrame.h>
#include <Headers/Build.h>
#include <imgui/imgui_internal.h>
#include <ImGuiMarkDown/imgui_markdown.h>
#include <Gui/ImWidgets.h>
#include <ctools/FileHelper.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Panes/Manager/LayoutManager.h>
#include <Headers/Globals.h>

#include <cinttypes> // printf zu
#include <cmath>

#include <Modules/Dbc/DBCModel.h>
#include <Modules/Dbc/DBCFile.h>
#include <Modules/Dbc/DBCObjects/DBCNode.h>
#include <Modules/Dbc/DBCObjects/DBCSignal.h>
#include <Modules/Dbc/DBCObjects/DBCMessage.h>
#include <Modules/Dbc/DBCObjects/DBCNetwork.h>
#include <Modules/Dbc/DBCObjects/DBCGranularErrors.h>

static int GeneratorPaneWidgetId = 0;

SynthesisPane::SynthesisPane() = default;
SynthesisPane::~SynthesisPane() = default;

////////////////////////////////////////////////////
//// STATIC ////////////////////////////////////////
////////////////////////////////////////////////////

static ImGuiTableFlags s_synthesis_Table_Flags =
ImGuiTableFlags_BordersV
| ImGuiTableFlags_ScrollX
| ImGuiTableFlags_ScrollY
| ImGuiTableFlags_SizingFixedFit
| ImGuiTableFlags_BordersInnerH
| ImGuiTableFlags_BordersOuterH
//| ImGuiTableFlags_Resizable
| ImGuiTableFlags_RowBg
//| ImGuiTableFlags_NoBordersInBody
//| ImGuiTableFlags_Sortable
;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool SynthesisPane::Init()
{
	return true;
}

void SynthesisPane::Unit()
{

}

int SynthesisPane::DrawPanes(int vWidgetId, std::string vUserDatas)
{
	GeneratorPaneWidgetId = vWidgetId;

	if (LayoutManager::Instance()->m_Pane_Shown & m_PaneFlag)
	{
		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
		if (ImGui::BeginFlag<PaneFlags>(m_PaneName,
			&LayoutManager::Instance()->m_Pane_Shown, m_PaneFlag, flags))
		{
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
			auto win = ImGui::GetCurrentWindowRead();
			if (win->Viewport->Idx != 0)
				flags |= ImGuiWindowFlags_NoResize;// | ImGuiWindowFlags_NoTitleBar;
			else
				flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
#endif
			DrawView();
		}

		ImGui::End();
	}

	return GeneratorPaneWidgetId;
}

void SynthesisPane::DrawDialogsAndPopups(std::string vUserDatas)
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

int SynthesisPane::DrawWidgets(int vWidgetId, std::string vUserDatas)
{
	UNUSED(vUserDatas);

	return vWidgetId;
}

std::string SynthesisPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool SynthesisPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

void SynthesisPane::ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel)
{
	m_DBCModel = vDBCModel;
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : MAIN DRAW///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void SynthesisPane::DrawView()
{
	auto dbcModelPtr = m_DBCModel.getValidShared();
	if (!dbcModelPtr)
		return;

	auto mergedDBCFilePtr = dbcModelPtr->GetMergedDBCFile();
	if (!mergedDBCFilePtr)
		return;

	ImGuiID id = ImGui::GetID(mergedDBCFilePtr.get());
	if (ImGui::BeginTableEx("##SynthesisPane_DrawView", id, 6, s_synthesis_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn("Network", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Node", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Object", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Error(s)", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto node : mergedDBCFilePtr->GetContainerNodesByNames())
		{
			auto nodePtr = node.second.getValidShared();
			if (nodePtr)
			{
				auto totalErrors = nodePtr->local_count_errors + nodePtr->merge_count_errors;
				if (totalErrors)
				{
					if (ImGui::TableNextColumn()) // Parent Network
					{
						auto parentPtr = nodePtr->parent_network.getValidShared();
						ImGui::Text("%s", (parentPtr ? parentPtr->name.c_str() : "Unknown"));
					}
					if (ImGui::TableNextColumn()) // Parent Node
					{
						
					}
					if (ImGui::TableNextColumn()) // Parent Message
					{

					}
					if (ImGui::TableNextColumn()) // Object Type
					{
						ImGui::Text("%s", "Node");
					}
					if (ImGui::TableNextColumn()) // Object
					{
						ImGui::Text("%s", nodePtr->name.c_str());
					}
					if (ImGui::TableNextColumn()) // Error
					{
						nodePtr->local_errors.DrawErrors();
						nodePtr->merge_errors.DrawErrors();
					}
				}				
			}
		}

		for (auto message : mergedDBCFilePtr->GetContainerMessagesByNames())
		{
			auto messagePtr = message.second.getValidShared();
			if (messagePtr)
			{
				auto totalErrors = messagePtr->local_count_errors + messagePtr->merge_count_errors;
				if (totalErrors)
				{
					if (ImGui::TableNextColumn()) // Parent Network
					{
						auto parentPtr = messagePtr->parent_network.getValidShared();
						ImGui::Text("%s", (parentPtr ? parentPtr->name.c_str() : "Unknown"));
					}
					if (ImGui::TableNextColumn()) // Parent Node
					{
						if (!messagePtr->parent_nodes.empty())
						{
							if (messagePtr->parent_nodes.size() == 1U)
							{
								auto parentPtr = (*messagePtr->parent_nodes.begin()).getValidShared();
								ImGui::Text("%s", (parentPtr ? parentPtr->name.c_str() : "Unknown"));
							}
							else
							{
								std::string nodes;
								for (auto pn : messagePtr->parent_nodes)
								{
									auto pnPtr = pn.getValidShared();
									if (pnPtr)
									{
										if (!nodes.empty())
											nodes += ", ";
										nodes += pnPtr->name;
									}
								}

								ImGui::Text("%s", nodes.c_str());
							}
						}
					}
					if (ImGui::TableNextColumn()) // Parent Message
					{

					}
					if (ImGui::TableNextColumn()) // Object Type
					{
						ImGui::Text("%s", "Message");
					}
					if (ImGui::TableNextColumn()) // Object
					{
						ImGui::Text("%s", messagePtr->name.c_str());
					}
					if (ImGui::TableNextColumn()) // Error
					{
						messagePtr->local_errors.DrawErrors();
						messagePtr->merge_errors.DrawErrors();
					}
				}
			}
		}

		for (auto signal : mergedDBCFilePtr->GetContainerSignalsByNames())
		{
			auto signalPtr = signal.second.getValidShared();
			if (signalPtr)
			{
				auto totalErrors = signalPtr->local_count_errors + signalPtr->merge_count_errors;
				if (totalErrors)
				{
					if (ImGui::TableNextColumn()) // Parent Network
					{
						auto parentPtr = signalPtr->parent_network.getValidShared();
						ImGui::Text("%s", (parentPtr ? parentPtr->name.c_str() : "Unknown"));
					}
					if (ImGui::TableNextColumn()) // Parent Node
					{
						if (!signalPtr->parent_nodes.empty())
						{
							if (signalPtr->parent_nodes.size() == 1U)
							{
								auto parentPtr = (*signalPtr->parent_nodes.begin()).getValidShared();
								ImGui::Text("%s", (parentPtr ? parentPtr->name.c_str() : "Unknown"));
							}
							else
							{
								std::string nodes;
								for (auto pn : signalPtr->parent_nodes)
								{
									auto pnPtr = pn.getValidShared();
									if (pnPtr)
									{
										if (!nodes.empty())
											nodes += ", ";
										nodes += pnPtr->name;
									}
								}

								ImGui::Text("%s", nodes.c_str());
							}
						}
					}
					if (ImGui::TableNextColumn()) // Parent Message
					{
						if (!signalPtr->parent_messages.empty())
						{
							if (signalPtr->parent_messages.size() == 1U)
							{
								auto parentPtr = (*signalPtr->parent_messages.begin()).getValidShared();
								ImGui::Text("%s", (parentPtr ? parentPtr->name.c_str() : "Unknown"));
							}
							else
							{
								std::string messages;
								for (auto pn : signalPtr->parent_messages)
								{
									auto pnPtr = pn.getValidShared();
									if (pnPtr)
									{
										if (!messages.empty())
											messages += ", ";
										messages += pnPtr->name;
									}
								}

								ImGui::Text("%s", messages.c_str());
							}
						}
					}
					if (ImGui::TableNextColumn()) // Object Type
					{
						ImGui::Text("%s", "Message");
					}
					if (ImGui::TableNextColumn()) // Object
					{
						ImGui::Text("%s", signalPtr->name.c_str());
					}
					if (ImGui::TableNextColumn()) // Error
					{
						signalPtr->local_errors.DrawErrors();
						signalPtr->merge_errors.DrawErrors();
					}
				}
			}
		}

		ImGui::EndTable();
	}
}
