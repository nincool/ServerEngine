// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: NFDefine.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_NFDefine_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_NFDefine_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3009000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3009000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_NFDefine_2eproto LIBPROTOC_EXPORT
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct LIBPROTOC_EXPORT TableStruct_NFDefine_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_NFDefine_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace OuterMsg {

enum EServerState : int {
  EST_CRASH = 0,
  EST_NARMAL = 1,
  EST_MAINTEN = 2,
  EServerState_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  EServerState_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool EServerState_IsValid(int value);
constexpr EServerState EServerState_MIN = EST_CRASH;
constexpr EServerState EServerState_MAX = EST_MAINTEN;
constexpr int EServerState_ARRAYSIZE = EServerState_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* EServerState_descriptor();
template<typename T>
inline const std::string& EServerState_Name(T enum_t_value) {
  static_assert(::std::is_same<T, EServerState>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function EServerState_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    EServerState_descriptor(), enum_t_value);
}
inline bool EServerState_Parse(
    const std::string& name, EServerState* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<EServerState>(
    EServerState_descriptor(), name, value);
}
enum ELoginMode : int {
  ELM_LOGIN = 0,
  ELM_REGISTER = 1,
  ELM_AUTO_REGISTER_LOGIN = 2,
  ELoginMode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ELoginMode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool ELoginMode_IsValid(int value);
constexpr ELoginMode ELoginMode_MIN = ELM_LOGIN;
constexpr ELoginMode ELoginMode_MAX = ELM_AUTO_REGISTER_LOGIN;
constexpr int ELoginMode_ARRAYSIZE = ELoginMode_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ELoginMode_descriptor();
template<typename T>
inline const std::string& ELoginMode_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ELoginMode>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ELoginMode_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ELoginMode_descriptor(), enum_t_value);
}
inline bool ELoginMode_Parse(
    const std::string& name, ELoginMode* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ELoginMode>(
    ELoginMode_descriptor(), name, value);
}
enum RecordOperatorType : int {
  RECORD_OT_ADD = 0,
  RECORD_OT_REMOVE = 1,
  RECORD_OT_MODIFY = 2,
  RECORD_OT_CLEAR = 3,
  RECORD_OT_UNKNOW = 4,
  RecordOperatorType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  RecordOperatorType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool RecordOperatorType_IsValid(int value);
constexpr RecordOperatorType RecordOperatorType_MIN = RECORD_OT_ADD;
constexpr RecordOperatorType RecordOperatorType_MAX = RECORD_OT_UNKNOW;
constexpr int RecordOperatorType_ARRAYSIZE = RecordOperatorType_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* RecordOperatorType_descriptor();
template<typename T>
inline const std::string& RecordOperatorType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, RecordOperatorType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function RecordOperatorType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    RecordOperatorType_descriptor(), enum_t_value);
}
inline bool RecordOperatorType_Parse(
    const std::string& name, RecordOperatorType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<RecordOperatorType>(
    RecordOperatorType_descriptor(), name, value);
}
enum EGameEventCode : int {
  EGEC_SUCCESS = 0,
  EGEC_UNKOWN_ERROR = 1,
  EGEC_ACCOUNT_EXIST = 2,
  EGEC_ACCOUNTPWD_INVALID = 3,
  EGEC_SELECTSERVER_FAIL = 4,
  EGEC_CREATE_ROLE_EXIT = 5,
  EGEC_CREATE_ROLE_NAME_REPEAT = 6,
  EGEC_CREATE_ROLE_DB_ERROR = 7,
  EGEC_ACCOUNTVERIFY_TIMEOUT = 8,
  EGEC_CREATE_PUB_NAME_REPEAT = 9,
  EGEC_CREATE_GUILD_NAME_REPEAT = 10,
  EGEC_CREATE_PUB_DB_ERROR = 11,
  EGEC_CREATE_GUILD_DB_ERROR = 12,
  EGEC_EXIST_SENSITIVE_WORD = 13,
  EGEC_LENGTH_ERROR = 14,
  EGEC_CAPITAL_LACK = 15,
  EGEC_ITEM_LACK = 16,
  EGEC_CREATE_PLATFORM_DB_ERROR = 17,
  EGEC_NOEXIST_PLATFORM_DB = 18,
  EGEC_PLAYER_FULL = 19,
  EGEC_PLAYER_CLOSURE = 20,
  EGameEventCode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  EGameEventCode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool EGameEventCode_IsValid(int value);
constexpr EGameEventCode EGameEventCode_MIN = EGEC_SUCCESS;
constexpr EGameEventCode EGameEventCode_MAX = EGEC_PLAYER_CLOSURE;
constexpr int EGameEventCode_ARRAYSIZE = EGameEventCode_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* EGameEventCode_descriptor();
template<typename T>
inline const std::string& EGameEventCode_Name(T enum_t_value) {
  static_assert(::std::is_same<T, EGameEventCode>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function EGameEventCode_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    EGameEventCode_descriptor(), enum_t_value);
}
inline bool EGameEventCode_Parse(
    const std::string& name, EGameEventCode* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<EGameEventCode>(
    EGameEventCode_descriptor(), name, value);
}
enum ObjectType : int {
  OBJECT_TYPE_UNKNOW = 0,
  OBJECT_TYPE_SCENE = 1,
  OBJECT_TYPE_PLAYER = 2,
  OBJECT_TYPE_NPC = 4,
  OBJECT_TYPE_ITEM = 8,
  OBJECT_TYPE_HELPER = 16,
  OBJECT_TYPE_CONTAINER = 32,
  OBJECT_TYPE_PUB = 64,
  OBJECT_TYPE_GUILD = 128,
  ObjectType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ObjectType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool ObjectType_IsValid(int value);
constexpr ObjectType ObjectType_MIN = OBJECT_TYPE_UNKNOW;
constexpr ObjectType ObjectType_MAX = OBJECT_TYPE_GUILD;
constexpr int ObjectType_ARRAYSIZE = ObjectType_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ObjectType_descriptor();
template<typename T>
inline const std::string& ObjectType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ObjectType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ObjectType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ObjectType_descriptor(), enum_t_value);
}
inline bool ObjectType_Parse(
    const std::string& name, ObjectType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ObjectType>(
    ObjectType_descriptor(), name, value);
}
enum RankListMsgID : int {
  MSG_RANK_NONE = 0,
  MSG_RANK_CREATE = 1,
  MSG_RANK_DELETE = 2,
  MSG_RANK_SET_SCORE = 3,
  MSG_RANK_SET_DATA = 4,
  MSG_RANK_GET_DATA = 5,
  MSG_RANK_GET_RANK = 6,
  MSG_RANK_GET_RANGE = 7,
  MSG_RANK_GET_DATA_ARRAY = 8,
  MSG_RANK_SWAP_RANK = 9,
  MSG_RANK_GET_RANK_ARRAY = 10,
  RankListMsgID_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  RankListMsgID_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool RankListMsgID_IsValid(int value);
constexpr RankListMsgID RankListMsgID_MIN = MSG_RANK_NONE;
constexpr RankListMsgID RankListMsgID_MAX = MSG_RANK_GET_RANK_ARRAY;
constexpr int RankListMsgID_ARRAYSIZE = RankListMsgID_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* RankListMsgID_descriptor();
template<typename T>
inline const std::string& RankListMsgID_Name(T enum_t_value) {
  static_assert(::std::is_same<T, RankListMsgID>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function RankListMsgID_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    RankListMsgID_descriptor(), enum_t_value);
}
inline bool RankListMsgID_Parse(
    const std::string& name, RankListMsgID* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<RankListMsgID>(
    RankListMsgID_descriptor(), name, value);
}
enum RankListCode : int {
  RANK_CODE_SUCCEED = 0,
  RANK_CODE_UNKNOW = 1,
  RANK_CODE_NAME_EMPTY = 2,
  RANK_CODE_EXIST = 3,
  RANK_CODE_NOT_EXIST = 4,
  RANK_CODE_ORDER_EMPTY = 5,
  RANK_CODE_DATA_EMPTY = 6,
  RANK_CODE_SCORE_NOT_MATCH = 7,
  RANK_CODE_OVER_RANGE = 8,
  RANK_CODE_ERROR_PARSE = 9,
  RANK_CODE_ERROR_SERIALIZE = 10,
  RANK_CODE_NO_ON_LIST = 11,
  RANK_CODE_DB_GET_DATA_FAILED = 12,
  RANK_CODE_PARAM_ERROR = 13,
  RankListCode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  RankListCode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool RankListCode_IsValid(int value);
constexpr RankListCode RankListCode_MIN = RANK_CODE_SUCCEED;
constexpr RankListCode RankListCode_MAX = RANK_CODE_PARAM_ERROR;
constexpr int RankListCode_ARRAYSIZE = RankListCode_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* RankListCode_descriptor();
template<typename T>
inline const std::string& RankListCode_Name(T enum_t_value) {
  static_assert(::std::is_same<T, RankListCode>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function RankListCode_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    RankListCode_descriptor(), enum_t_value);
}
inline bool RankListCode_Parse(
    const std::string& name, RankListCode* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<RankListCode>(
    RankListCode_descriptor(), name, value);
}
enum OsType : int {
  OS_TYPE_UNKNOWN = 0,
  OS_TYPE_IOS = 1,
  OS_TYPE_ANDROID = 2,
  OS_TYPE_PC = 3,
  OsType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  OsType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
LIBPROTOC_EXPORT bool OsType_IsValid(int value);
constexpr OsType OsType_MIN = OS_TYPE_UNKNOWN;
constexpr OsType OsType_MAX = OS_TYPE_PC;
constexpr int OsType_ARRAYSIZE = OsType_MAX + 1;

LIBPROTOC_EXPORT const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* OsType_descriptor();
template<typename T>
inline const std::string& OsType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, OsType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function OsType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    OsType_descriptor(), enum_t_value);
}
inline bool OsType_Parse(
    const std::string& name, OsType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<OsType>(
    OsType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace OuterMsg

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::OuterMsg::EServerState> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::EServerState>() {
  return ::OuterMsg::EServerState_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::ELoginMode> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::ELoginMode>() {
  return ::OuterMsg::ELoginMode_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::RecordOperatorType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::RecordOperatorType>() {
  return ::OuterMsg::RecordOperatorType_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::EGameEventCode> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::EGameEventCode>() {
  return ::OuterMsg::EGameEventCode_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::ObjectType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::ObjectType>() {
  return ::OuterMsg::ObjectType_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::RankListMsgID> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::RankListMsgID>() {
  return ::OuterMsg::RankListMsgID_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::RankListCode> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::RankListCode>() {
  return ::OuterMsg::RankListCode_descriptor();
}
template <> struct is_proto_enum< ::OuterMsg::OsType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::OuterMsg::OsType>() {
  return ::OuterMsg::OsType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_NFDefine_2eproto
