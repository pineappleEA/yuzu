// Copyright 2020 yuzu emulator team
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "core/hle/result.h"

namespace Kernel::Svc {

constexpr ResultCode ResultTerminationRequested{ErrorModule::Kernel, 59};
constexpr ResultCode ResultInvalidAddress{ErrorModule::Kernel, 102};
constexpr ResultCode ResultInvalidCurrentMemory{ErrorModule::Kernel, 106};
constexpr ResultCode ResultInvalidHandle{ErrorModule::Kernel, 114};
constexpr ResultCode ResultTimedOut{ErrorModule::Kernel, 117};
constexpr ResultCode ResultCancelled{ErrorModule::Kernel, 118};
constexpr ResultCode ResultInvalidEnumValue{ErrorModule::Kernel, 120};
constexpr ResultCode ResultInvalidState{ErrorModule::Kernel, 125};

} // namespace Kernel::Svc
