/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_KERNEL_XBOXKRNL_XOBJECT_H_
#define XENIA_KERNEL_XBOXKRNL_XOBJECT_H_

#include <atomic>

#include "xenia/xbox.h"

namespace xe {
class Emulator;
class Memory;
}  // namespace xe

namespace xe {
namespace kernel {

class KernelState;

template <typename T>
class object_ref;

// http://www.nirsoft.net/kernel_struct/vista/DISPATCHER_HEADER.html
typedef struct {
  struct {
    uint8_t type;

    union {
      uint8_t abandoned;
      uint8_t absolute;
      uint8_t npx_irql;
      uint8_t signalling;
    };
    union {
      uint8_t size;
      uint8_t hand;
    };
    union {
      uint8_t inserted;
      uint8_t debug_active;
      uint8_t dpc_active;
    };
  };

  xe::be<uint32_t> signal_state;
  xe::be<uint32_t> wait_list_flink;
  xe::be<uint32_t> wait_list_blink;
} X_DISPATCH_HEADER;

// http://www.nirsoft.net/kernel_struct/vista/OBJECT_HEADER.html
struct X_OBJECT_HEADER {
  xe::be<uint32_t> pointer_count;
  union {
    xe::be<uint32_t> handle_count;
    xe::be<uint32_t> next_to_free;
  };
  xe::be<uint32_t> object_type_ptr;
  uint8_t name_info_offset;
  uint8_t handle_info_offset;
  uint8_t quota_info_offset;
  uint8_t flags;
  union {
    xe::be<uint32_t> object_create_info; // X_OBJECT_CREATE_INFORMATION
    xe::be<uint32_t> quota_block_charged;
  };
  xe::be<uint32_t> security_descriptor;

  // Object lives after this header.
  // (There's actually a body field here which is the object itself)
};

// http://www.nirsoft.net/kernel_struct/vista/OBJECT_CREATE_INFORMATION.html
struct X_OBJECT_CREATE_INFORMATION {
  xe::be<uint32_t> attributes;
  xe::be<uint32_t> root_directory_ptr;
  xe::be<uint32_t> parse_context_ptr;
  xe::be<uint32_t> probe_mode;
  xe::be<uint32_t> paged_pool_charge;
  xe::be<uint32_t> non_paged_pool_charge;
  xe::be<uint32_t> security_descriptor_charge;
  xe::be<uint32_t> security_descriptor;
  xe::be<uint32_t> security_qos_ptr;

  // Security QoS here (SECURITY_QUALITY_OF_SERVICE) too!
};

class XObject {
 public:
  enum Type {
    kTypeModule,
    kTypeThread,
    kTypeEvent,
    kTypeFile,
    kTypeSemaphore,
    kTypeNotifyListener,
    kTypeMutant,
    kTypeTimer,
    kTypeEnumerator,
  };

  XObject(KernelState* kernel_state, Type type);
  virtual ~XObject();

  Emulator* emulator() const;
  KernelState* kernel_state() const;
  Memory* memory() const;

  Type type();
  X_HANDLE handle() const;
  const std::string& name() const { return name_; }
  uint32_t guest_object() const { return guest_object_ptr_; }

  void RetainHandle();
  bool ReleaseHandle();
  void Retain();
  void Release();
  X_STATUS Delete();

  // Reference()
  // Dereference()

  void SetAttributes(const uint8_t* obj_attrs_ptr);

  X_STATUS Wait(uint32_t wait_reason, uint32_t processor_mode,
                uint32_t alertable, uint64_t* opt_timeout);
  static X_STATUS SignalAndWait(XObject* signal_object, XObject* wait_object,
                                uint32_t wait_reason, uint32_t processor_mode,
                                uint32_t alertable, uint64_t* opt_timeout);
  static X_STATUS WaitMultiple(uint32_t count, XObject** objects,
                               uint32_t wait_type, uint32_t wait_reason,
                               uint32_t processor_mode, uint32_t alertable,
                               uint64_t* opt_timeout);

  static object_ref<XObject> GetNativeObject(KernelState* kernel_state,
                                             void* native_ptr,
                                             int32_t as_type = -1);
  template <typename T>
  static object_ref<T> GetNativeObject(KernelState* kernel_state,
                                       void* native_ptr, int32_t as_type = -1) {
    return object_ref<T>(reinterpret_cast<T*>(
        GetNativeObject(kernel_state, native_ptr, as_type).release()));
  }

  virtual void* GetWaitHandle() { return 0; }

 protected:
  // Creates the kernel object for guest code to use. Typically not needed.
  uint8_t* CreateNative(uint32_t size);
  void SetNativePointer(uint32_t native_ptr, bool uninitialized = false);

  static uint32_t TimeoutTicksToMs(int64_t timeout_ticks);

  KernelState* kernel_state_;

 private:
  std::atomic<int32_t> handle_ref_count_;
  std::atomic<int32_t> pointer_ref_count_;

  Type type_;
  X_HANDLE handle_;
  std::string name_;  // May be zero length.

  // Guest pointer for kernel object. Remember: X_OBJECT_HEADER precedes this
  // if we allocated it!
  uint32_t guest_object_ptr_;
  bool allocated_guest_object_;
};

template <typename T>
class object_ref {
 public:
  object_ref() noexcept : value_(nullptr) {}
  object_ref(nullptr_t) noexcept : value_(nullptr) {}
  object_ref& operator=(nullptr_t) noexcept {
    reset();
    return (*this);
  }

  explicit object_ref(T* value) noexcept : value_(value) {
    // Assumes retained on call.
  }
  explicit object_ref(const object_ref& right) noexcept {
    reset(right.get());
    if (value_) value_->Retain();
  }
  template <class V, class = typename std::enable_if<
                         std::is_convertible<V*, T*>::value, void>::type>
  object_ref(const object_ref<V>& right) noexcept {
    reset(right.get());
    if (value_) value_->Retain();
  }

  object_ref(object_ref&& right) noexcept : value_(right.release()) {}
  object_ref& operator=(object_ref&& right) noexcept {
    object_ref(std::move(right)).swap(*this);
    return (*this);
  }
  template <typename V>
  object_ref& operator=(object_ref<V>&& right) noexcept {
    object_ref(std::move(right)).swap(*this);
    return (*this);
  }

  object_ref& operator=(const object_ref& right) noexcept {
    object_ref(right).swap(*this);
    return (*this);
  }
  template <typename V>
  object_ref& operator=(const object_ref<V>& right) noexcept {
    object_ref(right).swap(*this);
    return (*this);
  }

  void swap(object_ref& right) noexcept {
    std::_Swap_adl(value_, right.value_);
  }

  ~object_ref() noexcept {
    if (value_) {
      value_->Release();
      value_ = nullptr;
    }
  }

  typename std::add_lvalue_reference<T>::type operator*() const {
    return (*get());
  }

  T* operator->() const noexcept {
    return std::pointer_traits<T*>::pointer_to(**this);
  }

  T* get() const noexcept { return value_; }

  template <typename V>
  V* get() const noexcept {
    return reinterpret_cast<V*>(value_);
  }

  explicit operator bool() const noexcept { return value_ != nullptr; }

  T* release() noexcept {
    T* value = value_;
    value_ = nullptr;
    return value;
  }

  static void accept(T* value) {
    reset(value);
    value->Release();
  }

  void reset() noexcept { object_ref().swap(*this); }

  void reset(T* value) noexcept { object_ref(value).swap(*this); }

 private:
  T* value_ = nullptr;
};

template <class _Ty>
bool operator==(const object_ref<_Ty>& _Left, nullptr_t) noexcept {
  return (_Left.get() == (_Ty*)0);
}

template <class _Ty>
bool operator==(nullptr_t, const object_ref<_Ty>& _Right) noexcept {
  return ((_Ty*)0 == _Right.get());
}

template <class _Ty>
bool operator!=(const object_ref<_Ty>& _Left, nullptr_t _Right) noexcept {
  return (!(_Left == _Right));
}

template <class _Ty>
bool operator!=(nullptr_t _Left, const object_ref<_Ty>& _Right) noexcept {
  return (!(_Left == _Right));
}

template <typename T>
object_ref<T> retain_object(T* ptr) {
  if (ptr) ptr->Retain();
  return object_ref<T>(ptr);
}

}  // namespace kernel
}  // namespace xe

#endif  // XENIA_KERNEL_XBOXKRNL_XOBJECT_H_
