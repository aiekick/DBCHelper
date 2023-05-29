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

#include "ErrorsPane.h"

#include <Gui/MainFrame.h>
#include <Headers/Build.h>
#include <imgui/imgui_internal.h>
#include <ImGuiMarkDown/imgui_markdown.h>
#include <Gui/ImWidgets.h>
#include <ctools/FileHelper.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Panes/Manager/LayoutManager.h>
#include <Modules/Dbc/DBCModel.h>
#include <Modules/Dbc/DBCFile.h>
#include <Helper/TranslationHelper.h>

#include <cinttypes> // printf zu
#include <cmath>

static int GeneratorPaneWidgetId = 0;

ErrorsPane::ErrorsPane() = default;
ErrorsPane::~ErrorsPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool ErrorsPane::Init()
{
	return true;
}

void ErrorsPane::Unit()
{

}

int ErrorsPane::DrawPanes(int vWidgetId, std::string vUserDatas)
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
			DrawParams();
		}

		ImGui::End();
	}

	return GeneratorPaneWidgetId;
}

void ErrorsPane::DrawDialogsAndPopups(std::string vUserDatas)
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

int ErrorsPane::DrawWidgets(int vWidgetId, std::string vUserDatas)
{
	UNUSED(vUserDatas);

	return vWidgetId;
}

std::string ErrorsPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<selected_error_types>" + ct::toStr((int32_t)DBCModel::s_DBCErrorTypeFlags) + "</selected_error_types>\n";

	return str;
}

bool ErrorsPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strName == "selected_error_types")
	{
		DBCModel::s_DBCErrorTypeFlags = (DBCErrorTypeFlags)ct::ivariant(strValue).GetI();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//// PUBLIC ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void ErrorsPane::ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel)
{
	m_DBCModel = vDBCModel;
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : MAIN DRAW///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool ErrorsPane::DrawErrorCategory(std::shared_ptr<DBCFile> vDBCFilePtr, const char* vLabel, const DBCErrorTypeFlags& vFlags)
{
	if (vDBCFilePtr)
	{
		const auto n = vDBCFilePtr->GetErrorCountForType(vFlags);
		const auto str = ct::toStr(" Errs %u : %s", n, vLabel);
		return ImGui::RadioButtonLabeled_BitWize<int32_t>(-1.0f, str.c_str(),
			nullptr, (int32_t*)&DBCModel::s_DBCErrorTypeFlags, (int32_t)vFlags, false, true, (int32_t)DBCErrorTypeFlags::DBC_ERROR_TYPE_NONE, false, nullptr, 0.0f);
	}

	return false;
}

void ErrorsPane::DrawParams()
{
	auto dbcModelPtr = m_DBCModel.getValidShared();
	if (!dbcModelPtr)
		return;

	auto mergedDBCFilePtr = dbcModelPtr->GetMergedDBCFile();
	if (!mergedDBCFilePtr)
		return;

	/*
	DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_STANDARD
	DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_EXTENDED
	DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE
	DBC_ERROR_TYPE_LOCAL_MSG_IS_NOT_MAPPED_TO_A_NODE
	DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE
	DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT
	DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX
	DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG

	DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG
	DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG
	*/

	bool needUpdate = false;

	if (ImGui::ContrastedButton(TranslationHelper::error_pane_select_all, nullptr, nullptr,
		ImGui::GetContentRegionAvailWidth() * 0.5f - ImGui::GetStyle().ItemInnerSpacing.x))
	{
		DBCModel::s_DBCErrorTypeFlags = DBCErrorTypeFlags::DBC_ERROR_TYPE_DEFAULT_ALL;
		needUpdate = true;
	}

	ImGui::SameLine();

	if (ImGui::ContrastedButton(TranslationHelper::error_pane_select_none, nullptr, nullptr,
		ImGui::GetContentRegionAvailWidth()))
	{
		DBCModel::s_DBCErrorTypeFlags = (DBCErrorTypeFlags)(0);
		needUpdate = true;
	}

	if (ImGui::CollapsingHeader(TranslationHelper::error_pane_local_name))
	{
		needUpdate |= ImGui::RadioButtonLabeled_BitWize<int32_t>(-1.0f, 
			TranslationHelper::error_pane_local_all, 
			TranslationHelper::error_pane_local_none,
			nullptr, (int32_t*)&DBCModel::s_DBCErrorTypeFlags, 
			(int32_t)DBCErrorTypeFlags::DBC_ERROR_TYPE_DEFAULT_ALL_LOCALS);

		if (ImGui::BeginFramedGroup(TranslationHelper::error_pane_local_msg_category_name))
		{
			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_msg_id_out_can_a,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_STANDARD);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_msg_id_out_can_b,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_EXTENDED);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_msg_size_higher_64,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_msg_not_mapped,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_IS_NOT_MAPPED_TO_A_NODE);

			ImGui::EndFramedGroup();
		}

		if (ImGui::BeginFramedGroup(TranslationHelper::error_pane_local_sig_category_name))
		{
			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_sig_bit_end_out_of_64,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_sig_bad_bit_count,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_sig_bad_min_max,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_local_sig_not_mapped,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG);

			ImGui::EndFramedGroup();
		}
	}
	
	if (ImGui::CollapsingHeader(TranslationHelper::error_pane_merge_name))
	{
		needUpdate |= ImGui::RadioButtonLabeled_BitWize<int32_t>(-1.0f, 
			TranslationHelper::error_pane_merge_all, 
			TranslationHelper::error_pane_merge_none,
			nullptr, (int32_t*)&DBCModel::s_DBCErrorTypeFlags, 
			(int32_t)DBCErrorTypeFlags::DBC_ERROR_TYPE_DEFAULT_ALL_MERGES);

		if (ImGui::BeginFramedGroup(TranslationHelper::error_pane_merge_nod_category_name))
		{
			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_merge_nod_same_name,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD);

			ImGui::EndFramedGroup();
		}
		
		if (ImGui::BeginFramedGroup(TranslationHelper::error_pane_merge_msg_category_name))
		{
			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_merge_msg_same_name,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_merge_msg_same_id,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_merge_msg_same_id_not_same_name,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG);

			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_merge_msg_same_name_not_same_id,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG);

			ImGui::EndFramedGroup();
		}

		if (ImGui::BeginFramedGroup(TranslationHelper::error_pane_merge_sig_category_name))
		{
			needUpdate |= DrawErrorCategory(mergedDBCFilePtr, 
				TranslationHelper::error_pane_merge_sig_same_name,
				DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG);

			ImGui::EndFramedGroup();
		}
	}

	if (needUpdate)
	{
		dbcModelPtr->CheckForErrorsInDBC();
	}
}
