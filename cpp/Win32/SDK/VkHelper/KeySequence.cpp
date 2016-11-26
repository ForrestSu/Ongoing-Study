#include "KeySequence.h"

#include <vector>

#include "../../../bitsutils.hpp"
#include "../../../join.hpp"
#include "../../../split.hpp"
#include "AccelHelper.h"


std::wstring KeySequence::plus_ = L"+";


KeySequence::KeySequence(VkUtils::vk_t vk)
    : ctrl_(false), shift_(false), alt_(false), vk_(0)
{
    bool ok = VkUtils::IsKnownVkCode(vk) && !VkUtils::IsModifier(vk);
    //Assert(ok, "Invalid virtual key code!");
    if (ok)
        vk_ = vk;
}

KeySequence::KeySequence(const std::wstring &keySequence, const std::wstring &delimiter)
    : ctrl_(false), shift_(false), alt_(false), vk_(0)
{
    CreateFromString(keySequence, delimiter);
}

KeySequence::KeySequence(ACCEL accel)
    : ctrl_(false), shift_(false), alt_(false), vk_(0)
{
    bool ok = IsValidAccel(accel);
    Assert(ok, "Invalid ACCEL!");
    if (ok) {
        vk_ = accel.key;
        ctrl_ = IsBitsSet<int>(accel.fVirt, FCONTROL);
        shift_ = IsBitsSet<int>(accel.fVirt, FSHIFT);
        alt_ = IsBitsSet<int>(accel.fVirt, FALT);
    }
}

KeySequence::KeySequence(HotKey hotKey)
    : ctrl_(false), shift_(false), alt_(false), vk_(0)
{
    CreateFromHotKey(hotKey.wVk, hotKey.wModifiers);
}

KeySequence::KeySequence(WORD wVirtualKey, WORD wHotKeyModifiers)
    : ctrl_(false), shift_(false), alt_(false), vk_(0)
{
    CreateFromHotKey(wVirtualKey, wHotKeyModifiers);
}

bool KeySequence::IsValid() const
{
    return VkUtils::IsKnownVkCode(vk_) && !VkUtils::IsModifier(vk_);
}

std::wstring KeySequence::ToString() const
{
    if (!IsValid())
        return L"";

    std::vector<std::wstring> keys;
    keys.reserve(4);

    if (ctrl_)
        keys.push_back(VkUtils::GetVkName(VK_CONTROL));
    if (shift_)
        keys.push_back(VkUtils::GetVkName(VK_SHIFT));
    if (alt_)
        keys.push_back(VkUtils::GetVkName(VK_MENU));

    keys.push_back(VkUtils::GetVkName(vk_));

    return join(keys.cbegin(), keys.cend(), plus_);
}

ACCEL KeySequence::ToAccel() const
{
    ACCEL accel;

    if (IsValid()) {
        accel.key = vk_;

        if (ctrl_)
            SetBits(accel.fVirt, static_cast<BYTE>(FCONTROL));
        if (shift_)
            SetBits(accel.fVirt, static_cast<BYTE>(FSHIFT));
        if (alt_)
            SetBits(accel.fVirt, static_cast<BYTE>(FALT));
    }

    return accel;
}

HotKey KeySequence::ToHotKey() const
{
    HotKey hotKey;

    if (IsValid()) {
        hotKey.wVk = vk_;

        if (ctrl_)
            SetBits(hotKey.wModifiers, static_cast<WORD>(HOTKEYF_CONTROL));
        if (shift_)
            SetBits(hotKey.wModifiers, static_cast<WORD>(HOTKEYF_SHIFT));
        if (alt_)
            SetBits(hotKey.wModifiers, static_cast<WORD>(HOTKEYF_ALT));
    }

    return hotKey;
}

void KeySequence::CreateFromString(const std::wstring &keySequence, const std::wstring &delimiter)
{
    std::vector<std::wstring> keys = split(keySequence, delimiter.empty() ? plus_ : delimiter);
    if (keys.empty() || keys.size() > 4)
        return;

    bool isCtrlSet = false;
    bool isShiftSet = false;
    bool isAltSet = false;
    bool isVkSet = false;
    VkUtils::vk_t code = 0, vk = 0;

    for (std::vector<std::wstring>::const_iterator it = keys.cbegin(); it != keys.cend(); ++it) {
        code = VkUtils::GetVkCode(*it);

        switch (code) {
        case VK_CONTROL:
            if (!isCtrlSet)
                isCtrlSet = true;
            else // Found duplicate control key
                return;
            break;
        case VK_SHIFT:
            if (!isShiftSet)
                isShiftSet = true;
            else
                return;
            break;
        case VK_MENU:
            if (!isAltSet)
                isAltSet = true;
            else
                return;
            break;
        default:
            if (!isVkSet) {
                isVkSet = true;
                vk = code;
            } else {
                return;
            }
            break;
        }
    }

    if (!isVkSet)
        return;

    vk_ = vk;
    ctrl_ = isCtrlSet;
    shift_ = isShiftSet;
    alt_ = isAltSet;
}

void KeySequence::CreateFromHotKey(WORD wVirtualKey, WORD wHotKeyModifiers)
{
    bool ok = IsValidHotKey(wVirtualKey, wHotKeyModifiers);
    Assert(ok, "Invalid hot key!");
    if (ok) {
        vk_ = wVirtualKey;
        ctrl_ = IsBitsSet<int>(wHotKeyModifiers, HOTKEYF_CONTROL);
        shift_ = IsBitsSet<int>(wHotKeyModifiers, HOTKEYF_SHIFT);
        alt_ = IsBitsSet<int>(wHotKeyModifiers, HOTKEYF_ALT);
    }
}