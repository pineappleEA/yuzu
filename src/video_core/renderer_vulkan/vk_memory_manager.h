// Copyright 2019 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "common/common_types.h"
#include "video_core/renderer_vulkan/wrapper.h"

namespace Vulkan {

class MemoryMap;
class VKDevice;
class VKMemoryAllocation;
class VKMemoryCommitImpl;

using VKMemoryCommit = std::unique_ptr<VKMemoryCommitImpl>;

class VKMemoryManager final {
public:
    explicit VKMemoryManager(const VKDevice& device_);
    VKMemoryManager(const VKMemoryManager&) = delete;
    ~VKMemoryManager();

    /**
     * Commits a memory with the specified requeriments.
     * @param requirements Requirements returned from a Vulkan call.
     * @param host_visible Signals the allocator that it *must* use host visible and coherent
     *                     memory. When passing false, it will try to allocate device local memory.
     * @returns A memory commit.
     */
    VKMemoryCommit Commit(const VkMemoryRequirements& requirements, bool host_visible);

    /// Commits memory required by the buffer and binds it.
    VKMemoryCommit Commit(const vk::Buffer& buffer, bool host_visible);

    /// Commits memory required by the image and binds it.
    VKMemoryCommit Commit(const vk::Image& image, bool host_visible);

private:
    /// Allocates a chunk of memory.
    bool AllocMemory(VkMemoryPropertyFlags wanted_properties, u32 type_mask, u64 size);

    /// Tries to allocate a memory commit.
    VKMemoryCommit TryAllocCommit(const VkMemoryRequirements& requirements,
                                  VkMemoryPropertyFlags wanted_properties);

    const VKDevice& device;                                       ///< Device handler.
    const VkPhysicalDeviceMemoryProperties properties;            ///< Physical device properties.
    std::vector<std::unique_ptr<VKMemoryAllocation>> allocations; ///< Current allocations.
};

class VKMemoryCommitImpl final {
    friend VKMemoryAllocation;
    friend MemoryMap;

public:
    explicit VKMemoryCommitImpl(const VKDevice& device_, VKMemoryAllocation* allocation_,
                                const vk::DeviceMemory& memory_, u64 begin_, u64 end_);
    ~VKMemoryCommitImpl();

    /// Maps a memory region and returns a pointer to it.
    /// It's illegal to have more than one memory map at the same time.
    MemoryMap Map(u64 size, u64 offset = 0) const;

    /// Maps the whole commit and returns a pointer to it.
    /// It's illegal to have more than one memory map at the same time.
    MemoryMap Map() const;

    /// Returns the Vulkan memory handler.
    VkDeviceMemory GetMemory() const {
        return *memory;
    }

    /// Returns the start position of the commit relative to the allocation.
    VkDeviceSize GetOffset() const {
        return static_cast<VkDeviceSize>(interval.first);
    }

private:
    /// Unmaps memory.
    void Unmap() const;

    const VKDevice& device;           ///< Vulkan device.
    const vk::DeviceMemory& memory;   ///< Vulkan device memory handler.
    std::pair<u64, u64> interval{};   ///< Interval where the commit exists.
    VKMemoryAllocation* allocation{}; ///< Pointer to the large memory allocation.
};

/// Holds ownership of a memory map.
class MemoryMap final {
public:
    explicit MemoryMap(const VKMemoryCommitImpl* commit_, std::span<u8> span_)
        : commit{commit_}, span{span_} {}

    ~MemoryMap() {
        if (commit) {
            commit->Unmap();
        }
    }

    /// Prematurely releases the memory map.
    void Release() {
        commit->Unmap();
        commit = nullptr;
    }

    /// Returns a span to the memory map.
    [[nodiscard]] std::span<u8> Span() const noexcept {
        return span;
    }

    /// Returns the address of the memory map.
    [[nodiscard]] u8* Address() const noexcept {
        return span.data();
    }

    /// Returns the address of the memory map;
    [[nodiscard]] operator u8*() const noexcept {
        return span.data();
    }

private:
    const VKMemoryCommitImpl* commit{}; ///< Mapped memory commit.
    std::span<u8> span;                 ///< Address to the mapped memory.
};

} // namespace Vulkan
