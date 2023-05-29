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

#pragma once

#include <Panes/Abstract/AbstractPane.h>
#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <vector>
#include <string>

class DebugPane : public AbstractPane, public conf::ConfigAbstract
{
public:
	bool Init() override;
	void Unit() override;
	int DrawPanes(int vWidgetId, std::string vUserDatas) override;
	void DrawDialogsAndPopups(std::string vUserDatas) override;
	int DrawWidgets(int vWidgetId, std::string vUserDatas) override;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;

private:
	void DrawDebug();

public: // singleton
	static DebugPane *Instance()
	{
		static DebugPane _instance;
		return &_instance;
	}

protected:
	DebugPane(); // Prevent construction
	DebugPane(const DebugPane&) {}; // Prevent construction by copying
	DebugPane& operator =(const DebugPane&) { return *this; }; // Prevent assignment
	~DebugPane(); // Prevent unwanted destruction};
};

