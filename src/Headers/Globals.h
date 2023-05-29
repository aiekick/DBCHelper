#pragma once

#include <ctools/cTools.h>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <cstdint> // uint64_t

enum class DBCObjectTypeEnum
{
	NONE = 0,
	NETWORK,
	NODE,
	MESSAGE,
	SIGNAL,
	Count
};

enum class DBCFile_ExportTypeEnum
{
	EXPORT_TYPE_DBC = 0,
	EXPORT_TYPE_HUMAN,
	EXPORT_TYPE_C
};

enum class DBCErrorTypeFlags : int32_t
{
	DBC_ERROR_TYPE_NONE = 0,

	DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_STANDARD = (1 << 0),
	DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_EXTENDED = (1 << 1),
	DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE = (1 << 2),
	DBC_ERROR_TYPE_LOCAL_MSG_IS_NOT_MAPPED_TO_A_NODE = (1 << 3),
	DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE = (1 << 4),
	DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT = (1 << 5),
	DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX = (1 << 6),
	DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG = (1 << 7),

	// all local errors
	DBC_ERROR_TYPE_DEFAULT_ALL_LOCALS = ((1 << 8) - 1),

	DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD = (1 << 8),
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG = (1 << 9),
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG = (1 << 10),
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG = (1 << 11),
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG = (1 << 12),
	DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG = (1 << 13),

	// all merge errors
	DBC_ERROR_TYPE_DEFAULT_ALL_MERGES = (((1 << 14) - 1)) & ~DBC_ERROR_TYPE_DEFAULT_ALL_LOCALS,

	DBC_ERROR_TYPE_DEFAULT_ALL = ((1 << 14) - 1),
};

class DBCFile;
class DBCObject;
class DBCNetwork;
class DBCNode;
class DBCMessage;
class DBCSignal;

using DBCFile_Shared = std::shared_ptr<DBCFile>;
using DBCObject_Shared = std::shared_ptr<DBCObject>;
using DBCNetwork_Shared = std::shared_ptr<DBCNetwork>;
using DBCNode_Shared = std::shared_ptr<DBCNode>;
using DBCMessage_Shared = std::shared_ptr<DBCMessage>;
using DBCSignal_Shared = std::shared_ptr<DBCSignal>;

using DBCFile_Weak = ct::cWeak<DBCFile>;
using DBCObject_Weak = ct::cWeak<DBCObject>;
using DBCNetwork_Weak = ct::cWeak<DBCNetwork>;
using DBCNode_Weak = ct::cWeak<DBCNode>;
using DBCMessage_Weak = ct::cWeak<DBCMessage>;
using DBCSignal_Weak = ct::cWeak<DBCSignal>;

using DBCNetworkSet = std::set<DBCNetwork_Weak>;
using DBCNodeSet = std::set<DBCNode_Weak>;
using DBCMessageSet = std::set<DBCMessage_Weak>;
using DBCSignalSet = std::set<DBCSignal_Weak>;

using DBCNetworkVector = std::vector<DBCNetwork_Weak>;
using DBCNodeVector = std::vector<DBCNode_Weak>;
using DBCMessageVector = std::vector<DBCMessage_Weak>;
using DBCSignalVector = std::vector<DBCSignal_Weak>;

using DBCNetworksNames = std::map<std::string, DBCNetwork_Weak>;
using DBCNodesNames = std::map<std::string, DBCNode_Weak>;
using DBCMessagesNames = std::map<std::string, DBCMessage_Weak>;
using DBCMessagesIds = std::map<uint64_t, DBCMessage_Weak>;
using DBCSignalsNames = std::map<std::string, DBCSignal_Weak>;

#define USE_CSTRUCT_MODULES
#define USE_MATLAB_MODULES