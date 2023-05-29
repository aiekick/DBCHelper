// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "DebugPane.h"

#include <Gui/MainFrame.h>
#include <Headers/Build.h>
#include <imgui/imgui_internal.h>
#include <ImGuiMarkDown/imgui_markdown.h>

#include <ctools/FileHelper.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Panes/Manager/LayoutManager.h>
#include <Modules/Dbc/DBCView.h>

#include <cinttypes> // printf zu
#include <cmath>

static int GeneratorPaneWidgetId = 0;

///////////////////////////////////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static int s_TypeIndex = 0;
static float s_Factor = 1.0f;
static float s_Offset = 0.0f;
static float s_Min = 0.0f;
static float s_Max = 1.0f;

static ImGuiTableFlags s_Table_Flags =
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
//// CTOR / DTOR //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

DebugPane::DebugPane() = default;
DebugPane::~DebugPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool DebugPane::Init()
{
	return true;
}

void DebugPane::Unit()
{

}

int DebugPane::DrawPanes(int vWidgetId, std::string vUserDatas)
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
			DrawDebug();
		}

		ImGui::End();
	}

	return GeneratorPaneWidgetId;
}

void DebugPane::DrawDialogsAndPopups(std::string vUserDatas)
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

int DebugPane::DrawWidgets(int vWidgetId, std::string vUserDatas)
{
	UNUSED(vUserDatas);

	return vWidgetId;
}

std::string DebugPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + ct::toStr("\t<type>%i</type>\n", s_TypeIndex);
	str += vOffset + ct::toStr("\t<factor>%.5f</factor>\n", s_Factor);
	str += vOffset + ct::toStr("\t<offset>%.5f</offset>\n", s_Offset);
	str += vOffset + ct::toStr("\t<min>%.5f</min>\n", s_Min);
	str += vOffset + ct::toStr("\t<max>%.5f</max>\n", s_Max);

	return str;
}

bool DebugPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strName == "type")
		s_TypeIndex = ct::ivariant(strValue).GetI();
	if (strName == "factor")
		s_Factor = ct::fvariant(strValue).GetF();
	if (strName == "offset")
		s_Offset = ct::fvariant(strValue).GetF();
	if (strName == "min")
		s_Min = ct::fvariant(strValue).GetF();
	if (strName == "max")
		s_Max = ct::fvariant(strValue).GetF();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE : MAIN DRAW///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void DebugPane::DrawDebug()
{
	static uint32_t s_bitSize = 0U;
	static float s_brut_min = 0.0f;
	static float s_brut_max = 0.0f;
	static float s_new_min = 0.0f;
	static float s_new_max = 0.0f;
	static bool s_change = true;

	if (s_change)
	{
		double best_brut = 0.0;

		s_brut_min = (s_Min - s_Offset) / s_Factor;
		s_brut_max = (s_Max - s_Offset) / s_Factor;

		s_TypeIndex = 0;
		if (s_brut_min < 0.0f || s_brut_max < 0.0f)
			s_TypeIndex = 1;

		if (s_TypeIndex == 0)
		{
			best_brut = (abs(s_Max) - s_Offset) / s_Factor;
		}
		else
		{
			best_brut = (ct::maxi(abs(s_Min), abs(s_Max)) - s_Offset) / s_Factor;
		}

		s_bitSize = (uint32_t)ceil(log(abs(best_brut) + 1) / log(2));

		// excel
		//colonne signed / unsigned
		//SI(OU(((W2-V2)/U2<0);((X2-V2)/U2<0)); "Signed"; "Unsigned")
		// colonne bit size
		//SI(OU(((W2-V2)/U2<0);((X2-V2)/U2<0));PLAFOND.MATH(log(abs((max(abs(W2);abs(X2))-V2)/U2)+1)/log(2));PLAFOND.MATH(log(abs((abs(X2)-V2)/U2)+1)/log(2)))


		if (s_TypeIndex == 0)
		{
			s_new_min = 0.0;
			s_new_max = powf(2, (float)s_bitSize) - 1;
		}
		else
		{
			s_bitSize++;
			s_new_min = -powf(2, (float)s_bitSize - 1);
			s_new_max = powf(2, (float)s_bitSize - 1) - 1;
		}
	}

	s_change = false;

	if (ImGui::BeginTable("##DrawViewDBCFile_AllNetworks", 6, s_Table_Flags, ImVec2(0,80)))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn("Bit Count", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Factor", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Min", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Max", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		if (ImGui::TableNextColumn())
		{
			ImGui::LabelText("##text", "%u", s_bitSize);
		}
		if (ImGui::TableNextColumn())
		{
			ImGui::PushItemWidth(60.0f);
			ImGui::LabelText("##Unsigned", "%s", (s_TypeIndex == 0 ? "Unsigned": "Signed"));
			ImGui::PopItemWidth();
		}
		if (ImGui::TableNextColumn())
		{
			s_change |= ImGui::InputFloatDefault(150.0f, "##Factor", &s_Factor, 1.0f, "%.3f", "%.3f", true, 0.001f, 1.0f);
		}
		if (ImGui::TableNextColumn())
		{
			s_change |= ImGui::InputFloatDefault(150.0f, "##Offset", &s_Offset, 0.0f, "%.3f", "%.3f", true, 1.0f, 10.0f);
		}
		if (ImGui::TableNextColumn())
		{
			s_change |= ImGui::InputFloatDefault(150.0f, "##Min", &s_Min, 0.0f, "%.3f", "%.3f", true, 1.0f, 10.0f);
		}
		if (ImGui::TableNextColumn())
		{
			s_change |= ImGui::InputFloatDefault(150.0f, "##Max", &s_Max, 0.0f, "%.3f", "%.3f", true, 1.0f, 10.0f);
		}

		ImGui::EndTable();
	}

	ImGui::Text("Can Brut Values Min %.5f and Max %.5f", s_brut_min, s_brut_max);
	ImGui::Text("BitSize of % u give Min %.5f and Max %.5f", s_bitSize, s_new_min, s_new_max);
}
