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

#include <Headers/Globals.h>
#include <map>
#include <vector>
#include <string>
#include <cstdint> // uint32_t, etc..

#include "DBCObject.h"
#include <dbcppp/include/dbcppp/CApi.h>
#include <dbcppp/include/dbcppp/Network.h>

class DBCSignal : public DBCObject
{
public:
	DBCMessageSet parent_messages;
	DBCNodeSet parent_nodes;
	DBCNetwork_Weak parent_network;

	// errors
	DBCSignalSet signals_with_same_name;

	// type dbcppp
	const dbcppp::ISignal* m_Signal = nullptr;

#ifdef CAN_WE_EDIT_SIGNALS
	dbcppp::ISignal::EMultiplexer multiplexer_indicator;
	uint64_t multiplexer_switch_value = 0U;
	uint64_t start_bit = 0U;
	uint64_t bit_size = 0U;
	dbcppp::ISignal::EByteOrder byte_order = dbcppp::ISignal::EByteOrder::LittleEndian;
	dbcppp::ISignal::EValueType value_type = dbcppp::ISignal::EValueType::Unsigned;
	dbcppp::ISignal::EExtendedValueType extended_value_type = dbcppp::ISignal::EExtendedValueType::Integer;
	double factor = 0.0;
	double offset = 0.0;
	double minimum = 0.0;
	double maximum = 0.0;
	std::string unit;
	std::vector<std::string> receivers;
	std::string comment;
#endif

public:
	DBCSignal(const dbcppp::ISignal* vSignal);
};