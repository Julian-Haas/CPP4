// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: Position.proto
// Protobuf C++ Version: 5.29.0-dev

#include "Position.pb.h"

#include <algorithm>
#include <type_traits>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/generated_message_tctable_impl.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace Server {

inline constexpr Position::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : x_{0},
        y_{0},
        z_{0},
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR Position::Position(::_pbi::ConstantInitialized)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(_class_data_.base()),
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(),
#endif  // PROTOBUF_CUSTOM_VTABLE
      _impl_(::_pbi::ConstantInitialized()) {
}
struct PositionDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PositionDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PositionDefaultTypeInternal() {}
  union {
    Position _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PositionDefaultTypeInternal _Position_default_instance_;
}  // namespace Server
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_Position_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_Position_2eproto = nullptr;
const ::uint32_t
    TableStruct_Position_2eproto::offsets[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
        protodesc_cold) = {
        ~0u,  // no _has_bits_
        PROTOBUF_FIELD_OFFSET(::Server::Position, _internal_metadata_),
        ~0u,  // no _extensions_
        ~0u,  // no _oneof_case_
        ~0u,  // no _weak_field_map_
        ~0u,  // no _inlined_string_donated_
        ~0u,  // no _split_
        ~0u,  // no sizeof(Split)
        PROTOBUF_FIELD_OFFSET(::Server::Position, _impl_.x_),
        PROTOBUF_FIELD_OFFSET(::Server::Position, _impl_.y_),
        PROTOBUF_FIELD_OFFSET(::Server::Position, _impl_.z_),
};

static const ::_pbi::MigrationSchema
    schemas[] ABSL_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::Server::Position)},
};
static const ::_pb::Message* const file_default_instances[] = {
    &::Server::_Position_default_instance_._instance,
};
const char descriptor_table_protodef_Position_2eproto[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
    protodesc_cold) = {
    "\n\016Position.proto\022\006Server\"+\n\010Position\022\t\n\001"
    "x\030\001 \001(\002\022\t\n\001y\030\002 \001(\002\022\t\n\001z\030\003 \001(\002b\006proto3"
};
static ::absl::once_flag descriptor_table_Position_2eproto_once;
PROTOBUF_CONSTINIT const ::_pbi::DescriptorTable descriptor_table_Position_2eproto = {
    false,
    false,
    77,
    descriptor_table_protodef_Position_2eproto,
    "Position.proto",
    &descriptor_table_Position_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_Position_2eproto::offsets,
    file_level_enum_descriptors_Position_2eproto,
    file_level_service_descriptors_Position_2eproto,
};
namespace Server {
// ===================================================================

class Position::_Internal {
 public:
};

Position::Position(::google::protobuf::Arena* arena)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(arena, _class_data_.base()) {
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(arena) {
#endif  // PROTOBUF_CUSTOM_VTABLE
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:Server.Position)
}
Position::Position(
    ::google::protobuf::Arena* arena, const Position& from)
    : Position(arena) {
  MergeFrom(from);
}
inline PROTOBUF_NDEBUG_INLINE Position::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : _cached_size_{0} {}

inline void Position::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
  ::memset(reinterpret_cast<char *>(&_impl_) +
               offsetof(Impl_, x_),
           0,
           offsetof(Impl_, z_) -
               offsetof(Impl_, x_) +
               sizeof(Impl_::z_));
}
Position::~Position() {
  // @@protoc_insertion_point(destructor:Server.Position)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void Position::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.~Impl_();
}

PROTOBUF_CONSTINIT
PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::google::protobuf::MessageLite::ClassDataFull
    Position::_class_data_ = {
        ::google::protobuf::Message::ClassData{
            &_Position_default_instance_._instance,
            &_table_.header,
            nullptr,  // OnDemandRegisterArenaDtor
            nullptr,  // IsInitialized
            &Position::MergeImpl,
#if defined(PROTOBUF_CUSTOM_VTABLE)
            ::google::protobuf::Message::GetDeleteImpl<Position>(),
            ::google::protobuf::Message::GetNewImpl<Position>(),
            ::google::protobuf::Message::GetClearImpl<Position>(), &Position::ByteSizeLong,
                &Position::_InternalSerialize,
#endif  // PROTOBUF_CUSTOM_VTABLE
            PROTOBUF_FIELD_OFFSET(Position, _impl_._cached_size_),
            false,
        },
        &Position::kDescriptorMethods,
        &descriptor_table_Position_2eproto,
        nullptr,  // tracker
};
const ::google::protobuf::MessageLite::ClassData* Position::GetClassData() const {
  ::google::protobuf::internal::PrefetchToLocalCache(&_class_data_);
  ::google::protobuf::internal::PrefetchToLocalCache(_class_data_.tc_table);
  return _class_data_.base();
}
PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<2, 3, 0, 0, 2> Position::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    3, 24,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967288,  // skipmap
    offsetof(decltype(_table_), field_entries),
    3,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    _class_data_.base(),
    nullptr,  // post_loop_handler
    ::_pbi::TcParser::GenericFallback,  // fallback
    #ifdef PROTOBUF_PREFETCH_PARSE_TABLE
    ::_pbi::TcParser::GetTable<::Server::Position>(),  // to_prefetch
    #endif  // PROTOBUF_PREFETCH_PARSE_TABLE
  }, {{
    {::_pbi::TcParser::MiniParse, {}},
    // float x = 1;
    {::_pbi::TcParser::FastF32S1,
     {13, 63, 0, PROTOBUF_FIELD_OFFSET(Position, _impl_.x_)}},
    // float y = 2;
    {::_pbi::TcParser::FastF32S1,
     {21, 63, 0, PROTOBUF_FIELD_OFFSET(Position, _impl_.y_)}},
    // float z = 3;
    {::_pbi::TcParser::FastF32S1,
     {29, 63, 0, PROTOBUF_FIELD_OFFSET(Position, _impl_.z_)}},
  }}, {{
    65535, 65535
  }}, {{
    // float x = 1;
    {PROTOBUF_FIELD_OFFSET(Position, _impl_.x_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kFloat)},
    // float y = 2;
    {PROTOBUF_FIELD_OFFSET(Position, _impl_.y_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kFloat)},
    // float z = 3;
    {PROTOBUF_FIELD_OFFSET(Position, _impl_.z_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kFloat)},
  }},
  // no aux_entries
  {{
  }},
};

PROTOBUF_NOINLINE void Position::Clear() {
// @@protoc_insertion_point(message_clear_start:Server.Position)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.x_, 0, static_cast<::size_t>(
      reinterpret_cast<char*>(&_impl_.z_) -
      reinterpret_cast<char*>(&_impl_.x_)) + sizeof(_impl_.z_));
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

#if defined(PROTOBUF_CUSTOM_VTABLE)
        ::uint8_t* Position::_InternalSerialize(
            const MessageLite& base, ::uint8_t* target,
            ::google::protobuf::io::EpsCopyOutputStream* stream) {
          const Position& this_ = static_cast<const Position&>(base);
#else   // PROTOBUF_CUSTOM_VTABLE
        ::uint8_t* Position::_InternalSerialize(
            ::uint8_t* target,
            ::google::protobuf::io::EpsCopyOutputStream* stream) const {
          const Position& this_ = *this;
#endif  // PROTOBUF_CUSTOM_VTABLE
          // @@protoc_insertion_point(serialize_to_array_start:Server.Position)
          ::uint32_t cached_has_bits = 0;
          (void)cached_has_bits;

          // float x = 1;
          if (::absl::bit_cast<::uint32_t>(this_._internal_x()) != 0) {
            target = stream->EnsureSpace(target);
            target = ::_pbi::WireFormatLite::WriteFloatToArray(
                1, this_._internal_x(), target);
          }

          // float y = 2;
          if (::absl::bit_cast<::uint32_t>(this_._internal_y()) != 0) {
            target = stream->EnsureSpace(target);
            target = ::_pbi::WireFormatLite::WriteFloatToArray(
                2, this_._internal_y(), target);
          }

          // float z = 3;
          if (::absl::bit_cast<::uint32_t>(this_._internal_z()) != 0) {
            target = stream->EnsureSpace(target);
            target = ::_pbi::WireFormatLite::WriteFloatToArray(
                3, this_._internal_z(), target);
          }

          if (PROTOBUF_PREDICT_FALSE(this_._internal_metadata_.have_unknown_fields())) {
            target =
                ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
                    this_._internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
          }
          // @@protoc_insertion_point(serialize_to_array_end:Server.Position)
          return target;
        }

#if defined(PROTOBUF_CUSTOM_VTABLE)
        ::size_t Position::ByteSizeLong(const MessageLite& base) {
          const Position& this_ = static_cast<const Position&>(base);
#else   // PROTOBUF_CUSTOM_VTABLE
        ::size_t Position::ByteSizeLong() const {
          const Position& this_ = *this;
#endif  // PROTOBUF_CUSTOM_VTABLE
          // @@protoc_insertion_point(message_byte_size_start:Server.Position)
          ::size_t total_size = 0;

          ::uint32_t cached_has_bits = 0;
          // Prevent compiler warnings about cached_has_bits being unused
          (void)cached_has_bits;

          ::_pbi::Prefetch5LinesFrom7Lines(&this_);
           {
            // float x = 1;
            if (::absl::bit_cast<::uint32_t>(this_._internal_x()) != 0) {
              total_size += 5;
            }
            // float y = 2;
            if (::absl::bit_cast<::uint32_t>(this_._internal_y()) != 0) {
              total_size += 5;
            }
            // float z = 3;
            if (::absl::bit_cast<::uint32_t>(this_._internal_z()) != 0) {
              total_size += 5;
            }
          }
          return this_.MaybeComputeUnknownFieldsSize(total_size,
                                                     &this_._impl_._cached_size_);
        }

void Position::MergeImpl(::google::protobuf::MessageLite& to_msg, const ::google::protobuf::MessageLite& from_msg) {
  auto* const _this = static_cast<Position*>(&to_msg);
  auto& from = static_cast<const Position&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Server.Position)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (::absl::bit_cast<::uint32_t>(from._internal_x()) != 0) {
    _this->_impl_.x_ = from._impl_.x_;
  }
  if (::absl::bit_cast<::uint32_t>(from._internal_y()) != 0) {
    _this->_impl_.y_ = from._impl_.y_;
  }
  if (::absl::bit_cast<::uint32_t>(from._internal_z()) != 0) {
    _this->_impl_.z_ = from._impl_.z_;
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Position::CopyFrom(const Position& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Server.Position)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}


void Position::InternalSwap(Position* PROTOBUF_RESTRICT other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::google::protobuf::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Position, _impl_.z_)
      + sizeof(Position::_impl_.z_)
      - PROTOBUF_FIELD_OFFSET(Position, _impl_.x_)>(
          reinterpret_cast<char*>(&_impl_.x_),
          reinterpret_cast<char*>(&other->_impl_.x_));
}

::google::protobuf::Metadata Position::GetMetadata() const {
  return ::google::protobuf::Message::GetMetadataImpl(GetClassData()->full());
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace Server
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::std::false_type
    _static_init2_ PROTOBUF_UNUSED =
        (::_pbi::AddDescriptors(&descriptor_table_Position_2eproto),
         ::std::false_type{});
#include "google/protobuf/port_undef.inc"
