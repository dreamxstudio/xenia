// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_MODULES_XE_DEBUG_PROTO_H_
#define FLATBUFFERS_GENERATED_MODULES_XE_DEBUG_PROTO_H_

#include "flatbuffers/flatbuffers.h"

namespace xe {
namespace debug {
namespace proto {
struct XObject;
}  // namespace proto
}  // namespace debug
}  // namespace xe

namespace xe {
namespace debug {
namespace proto {

struct Module;
struct ListModulesRequest;
struct ListModuleEntry;
struct ListModulesResponse;
struct GetModuleRequest;
struct GetModuleResponse;
struct FunctionEntry;
struct Function;
struct ListFunctionsRequest;
struct ListFunctionsResponse;
struct GetFunctionRequest;
struct GetFunctionResponse;

enum ModuleType {
  ModuleType_Kernel = 0,
  ModuleType_User = 1
};

inline const char **EnumNamesModuleType() {
  static const char *names[] = { "Kernel", "User", nullptr };
  return names;
}

inline const char *EnumNameModuleType(ModuleType e) { return EnumNamesModuleType()[e]; }

MANUALLY_ALIGNED_STRUCT(4) ListModuleEntry FLATBUFFERS_FINAL_CLASS {
 private:
  uint32_t handle_;
  uint32_t function_count_;

 public:
  ListModuleEntry(uint32_t handle, uint32_t function_count)
      : handle_(flatbuffers::EndianScalar(handle)),
        function_count_(flatbuffers::EndianScalar(function_count)) {}

  uint32_t handle() const { return flatbuffers::EndianScalar(handle_); }
  uint32_t function_count() const {
    return flatbuffers::EndianScalar(function_count_);
  }
};
STRUCT_END(ListModuleEntry, 8);

struct Module FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const xe::debug::proto::XObject *object() const { return GetStruct<const xe::debug::proto::XObject *>(4); }
  ModuleType type() const { return static_cast<ModuleType>(GetField<int8_t>(6, 0)); }
  const flatbuffers::String *name() const { return GetPointer<const flatbuffers::String *>(8); }
  const flatbuffers::String *path() const { return GetPointer<const flatbuffers::String *>(10); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<xe::debug::proto::XObject>(verifier, 4 /* object */) &&
           VerifyField<int8_t>(verifier, 6 /* type */) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 8 /* name */) &&
           verifier.Verify(name()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 10 /* path */) &&
           verifier.Verify(path()) &&
           verifier.EndTable();
  }
};

struct ModuleBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_object(const xe::debug::proto::XObject *object) { fbb_.AddStruct(4, object); }
  void add_type(ModuleType type) { fbb_.AddElement<int8_t>(6, static_cast<int8_t>(type), 0); }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) { fbb_.AddOffset(8, name); }
  void add_path(flatbuffers::Offset<flatbuffers::String> path) { fbb_.AddOffset(10, path); }
  ModuleBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  ModuleBuilder &operator=(const ModuleBuilder &);
  flatbuffers::Offset<Module> Finish() {
    auto o = flatbuffers::Offset<Module>(fbb_.EndTable(start_, 4));
    return o;
  }
};

inline flatbuffers::Offset<Module> CreateModule(flatbuffers::FlatBufferBuilder &_fbb,
   const xe::debug::proto::XObject *object = 0,
   ModuleType type = ModuleType_Kernel,
   flatbuffers::Offset<flatbuffers::String> name = 0,
   flatbuffers::Offset<flatbuffers::String> path = 0) {
  ModuleBuilder builder_(_fbb);
  builder_.add_path(path);
  builder_.add_name(name);
  builder_.add_object(object);
  builder_.add_type(type);
  return builder_.Finish();
}

struct ListModulesRequest FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct ListModulesRequestBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  ListModulesRequestBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  ListModulesRequestBuilder &operator=(const ListModulesRequestBuilder &);
  flatbuffers::Offset<ListModulesRequest> Finish() {
    auto o = flatbuffers::Offset<ListModulesRequest>(fbb_.EndTable(start_, 0));
    return o;
  }
};

inline flatbuffers::Offset<ListModulesRequest> CreateListModulesRequest(flatbuffers::FlatBufferBuilder &_fbb) {
  ListModulesRequestBuilder builder_(_fbb);
  return builder_.Finish();
}

struct ListModulesResponse FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const flatbuffers::Vector<const ListModuleEntry *> *entry() const {
    return GetPointer<const flatbuffers::Vector<const ListModuleEntry *> *>(4);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* entry */) &&
           verifier.Verify(entry()) && verifier.EndTable();
  }
};

struct ListModulesResponseBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_entry(
      flatbuffers::Offset<flatbuffers::Vector<const ListModuleEntry *>> entry) {
    fbb_.AddOffset(4, entry);
  }
  ListModulesResponseBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  ListModulesResponseBuilder &operator=(const ListModulesResponseBuilder &);
  flatbuffers::Offset<ListModulesResponse> Finish() {
    auto o = flatbuffers::Offset<ListModulesResponse>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<ListModulesResponse> CreateListModulesResponse(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<const ListModuleEntry *>> entry =
        0) {
  ListModulesResponseBuilder builder_(_fbb);
  builder_.add_entry(entry);
  return builder_.Finish();
}

struct GetModuleRequest FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  uint32_t module_id() const { return GetField<uint32_t>(4, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, 4 /* module_id */) &&
           verifier.EndTable();
  }
};

struct GetModuleRequestBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_module_id(uint32_t module_id) { fbb_.AddElement<uint32_t>(4, module_id, 0); }
  GetModuleRequestBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  GetModuleRequestBuilder &operator=(const GetModuleRequestBuilder &);
  flatbuffers::Offset<GetModuleRequest> Finish() {
    auto o = flatbuffers::Offset<GetModuleRequest>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<GetModuleRequest> CreateGetModuleRequest(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t module_id = 0) {
  GetModuleRequestBuilder builder_(_fbb);
  builder_.add_module_id(module_id);
  return builder_.Finish();
}

struct GetModuleResponse FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const Module *module() const { return GetPointer<const Module *>(4); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* module */) &&
           verifier.VerifyTable(module()) &&
           verifier.EndTable();
  }
};

struct GetModuleResponseBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_module(flatbuffers::Offset<Module> module) { fbb_.AddOffset(4, module); }
  GetModuleResponseBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  GetModuleResponseBuilder &operator=(const GetModuleResponseBuilder &);
  flatbuffers::Offset<GetModuleResponse> Finish() {
    auto o = flatbuffers::Offset<GetModuleResponse>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<GetModuleResponse> CreateGetModuleResponse(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<Module> module = 0) {
  GetModuleResponseBuilder builder_(_fbb);
  builder_.add_module(module);
  return builder_.Finish();
}

struct FunctionEntry FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  uint64_t identifier() const { return GetField<uint64_t>(4, 0); }
  uint32_t address_start() const { return GetField<uint32_t>(6, 0); }
  uint32_t address_end() const { return GetField<uint32_t>(8, 0); }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(10);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, 4 /* identifier */) &&
           VerifyField<uint32_t>(verifier, 6 /* address_start */) &&
           VerifyField<uint32_t>(verifier, 8 /* address_end */) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 10 /* name */) &&
           verifier.Verify(name()) && verifier.EndTable();
  }
};

struct FunctionEntryBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_identifier(uint64_t identifier) {
    fbb_.AddElement<uint64_t>(4, identifier, 0);
  }
  void add_address_start(uint32_t address_start) {
    fbb_.AddElement<uint32_t>(6, address_start, 0);
  }
  void add_address_end(uint32_t address_end) {
    fbb_.AddElement<uint32_t>(8, address_end, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(10, name);
  }
  FunctionEntryBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  FunctionEntryBuilder &operator=(const FunctionEntryBuilder &);
  flatbuffers::Offset<FunctionEntry> Finish() {
    auto o = flatbuffers::Offset<FunctionEntry>(fbb_.EndTable(start_, 4));
    return o;
  }
};

inline flatbuffers::Offset<FunctionEntry> CreateFunctionEntry(
    flatbuffers::FlatBufferBuilder &_fbb, uint64_t identifier = 0,
    uint32_t address_start = 0, uint32_t address_end = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  FunctionEntryBuilder builder_(_fbb);
  builder_.add_identifier(identifier);
  builder_.add_name(name);
  builder_.add_address_end(address_end);
  builder_.add_address_start(address_start);
  return builder_.Finish();
}

struct Function FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  uint64_t identifier() const { return GetField<uint64_t>(4, 0); }
  uint32_t address_start() const { return GetField<uint32_t>(6, 0); }
  uint32_t address_end() const { return GetField<uint32_t>(8, 0); }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(10);
  }
  const flatbuffers::String *disasm_ppc() const {
    return GetPointer<const flatbuffers::String *>(12);
  }
  const flatbuffers::String *disasm_hir_raw() const {
    return GetPointer<const flatbuffers::String *>(14);
  }
  const flatbuffers::String *disasm_hir_opt() const {
    return GetPointer<const flatbuffers::String *>(16);
  }
  const flatbuffers::String *disasm_machine_code() const {
    return GetPointer<const flatbuffers::String *>(18);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, 4 /* identifier */) &&
           VerifyField<uint32_t>(verifier, 6 /* address_start */) &&
           VerifyField<uint32_t>(verifier, 8 /* address_end */) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 10 /* name */) &&
           verifier.Verify(name()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 12 /* disasm_ppc */) &&
           verifier.Verify(disasm_ppc()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier,
                                               14 /* disasm_hir_raw */) &&
           verifier.Verify(disasm_hir_raw()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier,
                                               16 /* disasm_hir_opt */) &&
           verifier.Verify(disasm_hir_opt()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier,
                                               18 /* disasm_machine_code */) &&
           verifier.Verify(disasm_machine_code()) && verifier.EndTable();
  }
};

struct FunctionBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_identifier(uint64_t identifier) {
    fbb_.AddElement<uint64_t>(4, identifier, 0);
  }
  void add_address_start(uint32_t address_start) {
    fbb_.AddElement<uint32_t>(6, address_start, 0);
  }
  void add_address_end(uint32_t address_end) {
    fbb_.AddElement<uint32_t>(8, address_end, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(10, name);
  }
  void add_disasm_ppc(flatbuffers::Offset<flatbuffers::String> disasm_ppc) {
    fbb_.AddOffset(12, disasm_ppc);
  }
  void add_disasm_hir_raw(
      flatbuffers::Offset<flatbuffers::String> disasm_hir_raw) {
    fbb_.AddOffset(14, disasm_hir_raw);
  }
  void add_disasm_hir_opt(
      flatbuffers::Offset<flatbuffers::String> disasm_hir_opt) {
    fbb_.AddOffset(16, disasm_hir_opt);
  }
  void add_disasm_machine_code(
      flatbuffers::Offset<flatbuffers::String> disasm_machine_code) {
    fbb_.AddOffset(18, disasm_machine_code);
  }
  FunctionBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  FunctionBuilder &operator=(const FunctionBuilder &);
  flatbuffers::Offset<Function> Finish() {
    auto o = flatbuffers::Offset<Function>(fbb_.EndTable(start_, 8));
    return o;
  }
};

inline flatbuffers::Offset<Function> CreateFunction(
    flatbuffers::FlatBufferBuilder &_fbb, uint64_t identifier = 0,
    uint32_t address_start = 0, uint32_t address_end = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<flatbuffers::String> disasm_ppc = 0,
    flatbuffers::Offset<flatbuffers::String> disasm_hir_raw = 0,
    flatbuffers::Offset<flatbuffers::String> disasm_hir_opt = 0,
    flatbuffers::Offset<flatbuffers::String> disasm_machine_code = 0) {
  FunctionBuilder builder_(_fbb);
  builder_.add_identifier(identifier);
  builder_.add_disasm_machine_code(disasm_machine_code);
  builder_.add_disasm_hir_opt(disasm_hir_opt);
  builder_.add_disasm_hir_raw(disasm_hir_raw);
  builder_.add_disasm_ppc(disasm_ppc);
  builder_.add_name(name);
  builder_.add_address_end(address_end);
  builder_.add_address_start(address_start);
  return builder_.Finish();
}

struct ListFunctionsRequest FLATBUFFERS_FINAL_CLASS
    : private flatbuffers::Table {
  uint32_t module_id() const { return GetField<uint32_t>(4, 0); }
  uint32_t function_index_start() const { return GetField<uint32_t>(6, 0); }
  uint32_t function_index_end() const { return GetField<uint32_t>(8, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, 4 /* module_id */) &&
           VerifyField<uint32_t>(verifier, 6 /* function_index_start */) &&
           VerifyField<uint32_t>(verifier, 8 /* function_index_end */) &&
           verifier.EndTable();
  }
};

struct ListFunctionsRequestBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_module_id(uint32_t module_id) {
    fbb_.AddElement<uint32_t>(4, module_id, 0);
  }
  void add_function_index_start(uint32_t function_index_start) {
    fbb_.AddElement<uint32_t>(6, function_index_start, 0);
  }
  void add_function_index_end(uint32_t function_index_end) {
    fbb_.AddElement<uint32_t>(8, function_index_end, 0);
  }
  ListFunctionsRequestBuilder(flatbuffers::FlatBufferBuilder &_fbb)
      : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ListFunctionsRequestBuilder &operator=(const ListFunctionsRequestBuilder &);
  flatbuffers::Offset<ListFunctionsRequest> Finish() {
    auto o =
        flatbuffers::Offset<ListFunctionsRequest>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<ListFunctionsRequest> CreateListFunctionsRequest(
    flatbuffers::FlatBufferBuilder &_fbb, uint32_t module_id = 0,
    uint32_t function_index_start = 0, uint32_t function_index_end = 0) {
  ListFunctionsRequestBuilder builder_(_fbb);
  builder_.add_function_index_end(function_index_end);
  builder_.add_function_index_start(function_index_start);
  builder_.add_module_id(module_id);
  return builder_.Finish();
}

struct ListFunctionsResponse FLATBUFFERS_FINAL_CLASS
    : private flatbuffers::Table {
  const flatbuffers::Vector<flatbuffers::Offset<FunctionEntry>> *entry() const {
    return GetPointer<
        const flatbuffers::Vector<flatbuffers::Offset<FunctionEntry>> *>(4);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* entry */) &&
           verifier.Verify(entry()) && verifier.VerifyVectorOfTables(entry()) &&
           verifier.EndTable();
  }
};

struct ListFunctionsResponseBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_entry(flatbuffers::Offset<
      flatbuffers::Vector<flatbuffers::Offset<FunctionEntry>>> entry) {
    fbb_.AddOffset(4, entry);
  }
  ListFunctionsResponseBuilder(flatbuffers::FlatBufferBuilder &_fbb)
      : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ListFunctionsResponseBuilder &operator=(const ListFunctionsResponseBuilder &);
  flatbuffers::Offset<ListFunctionsResponse> Finish() {
    auto o =
        flatbuffers::Offset<ListFunctionsResponse>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<ListFunctionsResponse> CreateListFunctionsResponse(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FunctionEntry>>>
        entry = 0) {
  ListFunctionsResponseBuilder builder_(_fbb);
  builder_.add_entry(entry);
  return builder_.Finish();
}

struct GetFunctionRequest FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  uint64_t identifier() const { return GetField<uint64_t>(4, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, 4 /* identifier */) &&
           verifier.EndTable();
  }
};

struct GetFunctionRequestBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_identifier(uint64_t identifier) {
    fbb_.AddElement<uint64_t>(4, identifier, 0);
  }
  GetFunctionRequestBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  GetFunctionRequestBuilder &operator=(const GetFunctionRequestBuilder &);
  flatbuffers::Offset<GetFunctionRequest> Finish() {
    auto o = flatbuffers::Offset<GetFunctionRequest>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<GetFunctionRequest> CreateGetFunctionRequest(
    flatbuffers::FlatBufferBuilder &_fbb, uint64_t identifier = 0) {
  GetFunctionRequestBuilder builder_(_fbb);
  builder_.add_identifier(identifier);
  return builder_.Finish();
}

struct GetFunctionResponse FLATBUFFERS_FINAL_CLASS
    : private flatbuffers::Table {
  const Function *function() const { return GetPointer<const Function *>(4); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* function */) &&
           verifier.VerifyTable(function()) && verifier.EndTable();
  }
};

struct GetFunctionResponseBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_function(flatbuffers::Offset<Function> function) {
    fbb_.AddOffset(4, function);
  }
  GetFunctionResponseBuilder(flatbuffers::FlatBufferBuilder &_fbb)
      : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  GetFunctionResponseBuilder &operator=(const GetFunctionResponseBuilder &);
  flatbuffers::Offset<GetFunctionResponse> Finish() {
    auto o = flatbuffers::Offset<GetFunctionResponse>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<GetFunctionResponse> CreateGetFunctionResponse(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<Function> function = 0) {
  GetFunctionResponseBuilder builder_(_fbb);
  builder_.add_function(function);
  return builder_.Finish();
}

}  // namespace proto
}  // namespace debug
}  // namespace xe

#endif  // FLATBUFFERS_GENERATED_MODULES_XE_DEBUG_PROTO_H_
