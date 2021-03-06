/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/kernel/kernel_state.h"

#include <gflags/gflags.h>

#include "xenia/base/assert.h"
#include "xenia/base/string.h"
#include "xenia/cpu/processor.h"
#include "xenia/emulator.h"
#include "xenia/kernel/apps/apps.h"
#include "xenia/kernel/dispatcher.h"
#include "xenia/kernel/objects/xevent.h"
#include "xenia/kernel/objects/xmodule.h"
#include "xenia/kernel/objects/xnotify_listener.h"
#include "xenia/kernel/objects/xthread.h"
#include "xenia/kernel/objects/xuser_module.h"
#include "xenia/kernel/util/shim_utils.h"
#include "xenia/kernel/xam_module.h"
#include "xenia/kernel/xboxkrnl_module.h"
#include "xenia/kernel/xboxkrnl_private.h"
#include "xenia/kernel/xobject.h"

DEFINE_bool(headless, false,
            "Don't display any UI, using defaults for prompts as needed.");
DEFINE_string(content_root, "content",
              "Root path for content (save/etc) storage.");

namespace xe {
namespace kernel {

// This is a global object initialized with the XboxkrnlModule.
// It references the current kernel state object that all kernel methods should
// be using to stash their variables.
KernelState* shared_kernel_state_ = nullptr;

KernelState* kernel_state() { return shared_kernel_state_; }

KernelState::KernelState(Emulator* emulator)
    : emulator_(emulator),
      memory_(emulator->memory()),
      object_table_(nullptr),
      has_notified_startup_(false),
      process_type_(X_PROCTYPE_USER),
      process_info_block_address_(0) {
  processor_ = emulator->processor();
  file_system_ = emulator->file_system();

  dispatcher_ = new Dispatcher(this);

  app_manager_ = std::make_unique<XAppManager>();
  user_profile_ = std::make_unique<UserProfile>();

  auto content_root = xe::to_wstring(FLAGS_content_root);
  content_root = xe::to_absolute_path(content_root);
  content_manager_ = std::make_unique<ContentManager>(this, content_root);

  object_table_ = new ObjectTable();

  assert_null(shared_kernel_state_);
  shared_kernel_state_ = this;

  process_info_block_address_ = memory_->SystemHeapAlloc(0x60);
  auto pib =
      memory_->TranslateVirtual<ProcessInfoBlock*>(process_info_block_address_);
  // TODO(benvanik): figure out what this list is.
  pib->unk_04 = pib->unk_08 = 0;
  pib->unk_0C = 0x0000007F;
  pib->unk_10 = 0x001F0000;
  pib->thread_count = 0;
  pib->unk_1B = 0x06;
  pib->kernel_stack_size = 16 * 1024;
  pib->process_type = process_type_;
  // TODO(benvanik): figure out what this list is.
  pib->unk_54 = pib->unk_58 = 0;

  apps::RegisterApps(this, app_manager_.get());
}

KernelState::~KernelState() {
  SetExecutableModule(nullptr);

  if (process_info_block_address_) {
    memory_->SystemHeapFree(process_info_block_address_);
  }

  executable_module_.reset();
  user_modules_.clear();
  kernel_modules_.clear();

  // Delete all objects.
  delete object_table_;

  // Shutdown apps.
  app_manager_.reset();

  delete dispatcher_;

  assert_true(shared_kernel_state_ == this);
  shared_kernel_state_ = nullptr;
}

KernelState* KernelState::shared() { return shared_kernel_state_; }

uint32_t KernelState::title_id() const {
  assert_not_null(executable_module_);
  return executable_module_->xex_header()->execution_info.title_id;
}

uint32_t KernelState::process_type() const {
  auto pib =
      memory_->TranslateVirtual<ProcessInfoBlock*>(process_info_block_address_);
  return pib->process_type;
}

void KernelState::set_process_type(uint32_t value) {
  auto pib =
      memory_->TranslateVirtual<ProcessInfoBlock*>(process_info_block_address_);
  pib->process_type = uint8_t(value);
}

void KernelState::RegisterModule(XModule* module) {}

void KernelState::UnregisterModule(XModule* module) {}

bool KernelState::IsKernelModule(const char* name) {
  if (!name) {
    // Executing module isn't a kernel module.
    return false;
  }
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  for (auto kernel_module : kernel_modules_) {
    if (kernel_module->Matches(name)) {
      return true;
    }
  }
  return false;
}

object_ref<XModule> KernelState::GetModule(const char* name) {
  if (!name) {
    // NULL name = self.
    // TODO(benvanik): lookup module from caller address.
    return GetExecutableModule();
  } else if (strcasecmp(name, "kernel32.dll") == 0) {
    // Some games request this, for some reason. wtf.
    return nullptr;
  }
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  for (auto kernel_module : kernel_modules_) {
    if (kernel_module->Matches(name)) {
      return retain_object(kernel_module.get());
    }
  }
  for (auto user_module : user_modules_) {
    if (user_module->Matches(name)) {
      return retain_object(user_module.get());
    }
  }
  return nullptr;
}

object_ref<XUserModule> KernelState::GetExecutableModule() {
  if (!executable_module_) {
    return nullptr;
  }
  return executable_module_;
}

void KernelState::SetExecutableModule(object_ref<XUserModule> module) {
  if (module.get() == executable_module_.get()) {
    return;
  }
  executable_module_ = std::move(module);

  auto header = executable_module_->xex_header();
  if (header) {
    auto pib = memory_->TranslateVirtual<ProcessInfoBlock*>(
        process_info_block_address_);
    pib->tls_data_size = header->tls_info.data_size;
    pib->tls_raw_data_size = header->tls_info.raw_data_size;
    pib->tls_slot_size = header->tls_info.slot_count * 4;
  }
}

void KernelState::LoadKernelModule(object_ref<XKernelModule> kernel_module) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  kernel_modules_.push_back(std::move(kernel_module));
}

object_ref<XUserModule> KernelState::LoadUserModule(const char* raw_name) {
  // Some games try to load relative to launch module, others specify full path.
  std::string name = xe::find_name_from_path(raw_name);
  std::string path(raw_name);
  if (name == raw_name) {
    path = xe::join_paths(xe::find_base_path(executable_module_->path()), name);
  }

  object_ref<XUserModule> module;
  {
    std::lock_guard<xe::recursive_mutex> lock(object_mutex_);

    // See if we've already loaded it
    for (auto& existing_module : user_modules_) {
      if (existing_module->path() == path) {
        existing_module->Retain();
        return retain_object(existing_module.get());
      }
    }

    // Module wasn't loaded, so load it.
    module = object_ref<XUserModule>(new XUserModule(this, path.c_str()));
    X_STATUS status = module->LoadFromFile(path);
    if (XFAILED(status)) {
      return nullptr;
    }

    // Retain when putting into the listing.
    module->Retain();
    user_modules_.push_back(module);
  }

  module->Dump();

  auto xex_header = module->xex_header();
  if (xex_header->exe_entry_point) {
    // Call DllMain(DLL_PROCESS_ATTACH):
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583%28v=vs.85%29.aspx
    uint64_t args[] = {
        module->handle(),
        1,  // DLL_PROCESS_ATTACH
        0,  // 0 because always dynamic
    };
    auto thread_state = XThread::GetCurrentThread()->thread_state();
    processor()->Execute(thread_state, xex_header->exe_entry_point, args,
                         xe::countof(args));
  }

  return module;
}

void KernelState::RegisterThread(XThread* thread) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  threads_by_id_[thread->thread_id()] = thread;

  auto pib =
      memory_->TranslateVirtual<ProcessInfoBlock*>(process_info_block_address_);
  pib->thread_count = pib->thread_count + 1;
}

void KernelState::UnregisterThread(XThread* thread) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  auto it = threads_by_id_.find(thread->thread_id());
  if (it != threads_by_id_.end()) {
    threads_by_id_.erase(it);
  }

  auto pib =
      memory_->TranslateVirtual<ProcessInfoBlock*>(process_info_block_address_);
  pib->thread_count = pib->thread_count - 1;
}

void KernelState::OnThreadExecute(XThread* thread) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);

  // Must be called on executing thread.
  assert_true(XThread::GetCurrentThread() == thread);

  // Call DllMain(DLL_THREAD_ATTACH) for each user module:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583%28v=vs.85%29.aspx
  auto thread_state = thread->thread_state();
  for (auto user_module : user_modules_) {
    auto xex_header = user_module->xex_header();
    if (xex_header->exe_entry_point) {
      uint64_t args[] = {
          user_module->handle(),
          2,  // DLL_THREAD_ATTACH
          0,  // 0 because always dynamic
      };
      processor()->Execute(thread_state, xex_header->exe_entry_point, args,
                           xe::countof(args));
    }
  }
}

void KernelState::OnThreadExit(XThread* thread) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);

  // Must be called on executing thread.
  assert_true(XThread::GetCurrentThread() == thread);

  // Call DllMain(DLL_THREAD_DETACH) for each user module:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583%28v=vs.85%29.aspx
  auto thread_state = thread->thread_state();
  for (auto user_module : user_modules_) {
    auto xex_header = user_module->xex_header();
    if (xex_header->exe_entry_point) {
      uint64_t args[] = {
          user_module->handle(),
          3,  // DLL_THREAD_DETACH
          0,  // 0 because always dynamic
      };
      processor()->Execute(thread_state, xex_header->exe_entry_point, args,
                           xe::countof(args));
    }
  }
}

object_ref<XThread> KernelState::GetThreadByID(uint32_t thread_id) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  XThread* thread = nullptr;
  auto it = threads_by_id_.find(thread_id);
  if (it != threads_by_id_.end()) {
    thread = it->second;
  }
  return retain_object(thread);
}

void KernelState::RegisterNotifyListener(XNotifyListener* listener) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  notify_listeners_.push_back(retain_object(listener));

  // Games seem to expect a few notifications on startup, only for the first
  // listener.
  // http://cs.rin.ru/forum/viewtopic.php?f=38&t=60668&hilit=resident+evil+5&start=375
  if (!has_notified_startup_ && listener->mask() & 0x00000001) {
    has_notified_startup_ = true;
    // XN_SYS_UI (on, off)
    listener->EnqueueNotification(0x00000009, 1);
    listener->EnqueueNotification(0x00000009, 0);
    // XN_SYS_SIGNINCHANGED x2
    listener->EnqueueNotification(0x0000000A, 1);
    listener->EnqueueNotification(0x0000000A, 1);
    // XN_SYS_INPUTDEVICESCHANGED x2
    listener->EnqueueNotification(0x00000012, 0);
    listener->EnqueueNotification(0x00000012, 0);
    // XN_SYS_INPUTDEVICECONFIGCHANGED x2
    listener->EnqueueNotification(0x00000013, 0);
    listener->EnqueueNotification(0x00000013, 0);
  }
}

void KernelState::UnregisterNotifyListener(XNotifyListener* listener) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  for (auto& it = notify_listeners_.begin(); it != notify_listeners_.end();
       ++it) {
    if ((*it).get() == listener) {
      notify_listeners_.erase(it);
      break;
    }
  }
}

void KernelState::BroadcastNotification(XNotificationID id, uint32_t data) {
  std::lock_guard<xe::recursive_mutex> lock(object_mutex_);
  for (auto it = notify_listeners_.begin(); it != notify_listeners_.end();
       ++it) {
    (*it)->EnqueueNotification(id, data);
  }
}

void KernelState::CompleteOverlapped(uint32_t overlapped_ptr, X_RESULT result) {
  CompleteOverlappedEx(overlapped_ptr, result, result, 0);
}

void KernelState::CompleteOverlappedEx(uint32_t overlapped_ptr, X_RESULT result,
                                       uint32_t extended_error,
                                       uint32_t length) {
  auto ptr = memory()->TranslateVirtual(overlapped_ptr);
  XOverlappedSetResult(ptr, result);
  XOverlappedSetExtendedError(ptr, extended_error);
  XOverlappedSetLength(ptr, length);
  X_HANDLE event_handle = XOverlappedGetEvent(ptr);
  if (event_handle) {
    auto ev = object_table()->LookupObject<XEvent>(event_handle);
    if (ev) {
      ev->Set(0, false);
    }
  }
  if (XOverlappedGetCompletionRoutine(ptr)) {
    X_HANDLE thread_handle = XOverlappedGetContext(ptr);
    auto thread = object_table()->LookupObject<XThread>(thread_handle);
    if (thread) {
      uint32_t routine = XOverlappedGetCompletionRoutine(ptr);
      uint64_t args[] = {
          result, length, overlapped_ptr,
      };
      // TODO(benvanik): queue APC on the thread that requested the overlapped
      // operation.
      assert_always();
      // THIS IS WRONG, for testing only:
      processor()->Execute(XThread::GetCurrentThread()->thread_state(), routine,
                           args, xe::countof(args));
    }
  }
}

void KernelState::CompleteOverlappedImmediate(uint32_t overlapped_ptr,
                                              X_RESULT result) {
  CompleteOverlappedImmediateEx(overlapped_ptr, result, result, 0);
}

void KernelState::CompleteOverlappedImmediateEx(uint32_t overlapped_ptr,
                                                X_RESULT result,
                                                uint32_t extended_error,
                                                uint32_t length) {
  auto ptr = memory()->TranslateVirtual(overlapped_ptr);
  XOverlappedSetContext(ptr, XThread::GetCurrentThreadHandle());
  CompleteOverlappedEx(overlapped_ptr, result, extended_error, length);
}

}  // namespace kernel
}  // namespace xe
