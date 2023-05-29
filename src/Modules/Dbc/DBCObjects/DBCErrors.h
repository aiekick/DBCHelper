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

#include "DBCGranularErrors.h"
#include <cstdint> // uint32_t, etc..

class DBCErrors
{
public:
	uint32_t merge_count_errors = 0U; // errors due to the merge (like name / id conflict, etc..)
	DBCGranularErrors merge_errors;
	uint32_t local_count_errors = 0U; // errors internal so not due to the merge (bit issues, message size issues, message id issues, etc..)
	DBCGranularErrors local_errors;

public:
	void ClearErrors();
};