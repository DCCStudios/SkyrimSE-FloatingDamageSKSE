#include "FloatingDamageManager.h"
#include "Offsets.h"
#include "Settings.h"

namespace
{
    constexpr float kPi = 3.14159265f;
}

FloatingDamageManager::FloatingDamageManager()
{
    std::random_device rd;
    _rng.seed(rd());
    _startTime = std::chrono::steady_clock::now();
}

float FloatingDamageManager::NowSeconds() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(now - _startTime).count();
}

void FloatingDamageManager::AddFloatingNumber(RE::Actor* a_actor, float a_delta, bool a_isHeal)
{
    if (!a_actor) {
        return;
    }

    auto* settings = Settings::GetSingleton();
    if (!settings->enabled) {
        return;
    }

    if (a_isHeal && !settings->showHealing) {
        return;
    }

    if (!a_isHeal && !settings->showDamage) {
        return;
    }

    if (a_delta == 0.0f) {
        return;
    }

    if (a_actor->IsPlayerRef()) {
        return;
    }

    if (!a_actor->Is3DLoaded()) {
        return;
    }

    std::uniform_real_distribution<float> randOffset(-settings->worldOffsetXY, settings->worldOffsetXY);
    std::uniform_real_distribution<float> randAngle(0.0f, kPi * 2.0f);
    std::uniform_real_distribution<float> randDir(0.0f, 1.0f);

    Entry entry;
    entry.origin = a_actor->GetPosition();
    entry.origin.x += randOffset(_rng);
    entry.origin.y += randOffset(_rng);
    entry.origin.z += settings->worldOffsetZ;
    entry.startTime = NowSeconds();
    entry.lastUpdateTime = entry.startTime;
    entry.duration = settings->duration;
    entry.magnitude = std::abs(a_delta);
    entry.isHeal = a_isHeal;
    entry.mode = settings->animationMode;
    entry.arcAngle = randAngle(_rng);
    entry.spiralAngle = randAngle(_rng);
    entry.matrixDirection = randDir(_rng) > 0.5f ? 1.0f : -1.0f;
    entry.offset = { 0.0f, 0.0f, 0.0f };
    entry.velocity = { 0.0f, 0.0f, 0.0f };

    if (entry.mode == AnimationMode::Bounce) {
        entry.velocity.z = settings->bounceInitialVelocity;
    } else if (entry.mode == AnimationMode::Fountain) {
        std::uniform_real_distribution<float> randSpread(0.5f, 1.0f);
        float angle = randAngle(_rng);
        float spread = settings->fountainSpread;
        float horizVel = randSpread(_rng) * spread;
        entry.velocity.x = std::cos(angle) * horizVel;
        entry.velocity.y = std::sin(angle) * horizVel;

        std::uniform_real_distribution<float> randJitter(-settings->fountainRandomness, settings->fountainRandomness);
        entry.velocity.z = settings->fountainInitialVelocity * (1.0f + randJitter(_rng));
    }

    _entries.push_back(entry);
}

void FloatingDamageManager::CollectDrawItems(std::vector<FloatingDrawItem>& a_items, float a_screenWidth, float a_screenHeight)
{
    a_items.clear();

    auto* settings = Settings::GetSingleton();
    if (!settings->enabled) {
        return;
    }

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player) {
        return;
    }

    float now = NowSeconds();

    for (auto it = _entries.begin(); it != _entries.end();) {
        float age = now - it->startTime;
        if (age < 0.0f || age >= it->duration) {
            it = _entries.erase(it);
            continue;
        }

        float progress = age / it->duration;
        float delta = std::clamp(now - it->lastUpdateTime, 0.0f, 0.05f);
        it->lastUpdateTime = now;

        RE::NiPoint3 offset = ComputeOffset(*it, age, delta);
        RE::NiPoint3 worldPos{
            it->origin.x + offset.x,
            it->origin.y + offset.y,
            it->origin.z + offset.z
        };

        auto playerPos = player->GetPosition();
        float dx = worldPos.x - playerPos.x;
        float dy = worldPos.y - playerPos.y;
        float dz = worldPos.z - playerPos.z;
        float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (distance > settings->maxDistance) {
            it = _entries.erase(it);
            continue;
        }

        float screenX = 0.0f;
        float screenY = 0.0f;
        float depth = 0.0f;
        if (!WorldToScreen(worldPos, a_screenWidth, a_screenHeight, screenX, screenY, depth)) {
            ++it;
            continue;
        }

        screenX += settings->screenOffsetX * a_screenWidth;
        screenY += settings->screenOffsetY * a_screenHeight;

        FloatingDrawItem drawItem;
        drawItem.text = FormatNumber(it->magnitude, it->isHeal);
        drawItem.x = screenX;
        drawItem.y = screenY;
        drawItem.alpha = ComputeAlpha(*it, progress);
        drawItem.fontSize = settings->baseFontSize * ComputeDistanceScale(distance);

        if (it->mode == AnimationMode::Matrix && settings->matrixGreenTint && !it->isHeal) {
            drawItem.color = { 0.0f, 1.0f, 0.0f };
        } else {
            drawItem.color = it->isHeal ? settings->healColor : settings->damageColor;
        }

        a_items.push_back(std::move(drawItem));
        ++it;
    }
}

void FloatingDamageManager::Clear()
{
    _entries.clear();
}

bool FloatingDamageManager::WorldToScreen(const RE::NiPoint3& a_world, float a_screenWidth, float a_screenHeight, float& a_x, float& a_y, float& a_depth) const
{
    if (!g_viewPort) {
        return false;
    }

    float screenX = 0.0f;
    float screenY = 0.0f;
    float depth = 0.0f;

    RE::NiCamera::WorldPtToScreenPt3(
        (float(*)[4])g_worldToCamMatrix,
        *g_viewPort,
        a_world,
        screenX,
        screenY,
        depth,
        1e-5f);

    if (depth < 0.0f) {
        return false;
    }

    a_x = screenX * a_screenWidth;
    a_y = (1.0f - screenY) * a_screenHeight;
    a_depth = depth;
    return true;
}

RE::NiPoint3 FloatingDamageManager::ComputeOffset(Entry& a_entry, float a_age, float a_delta)
{
    auto* settings = Settings::GetSingleton();
    RE::NiPoint3 offset = a_entry.offset;

    switch (a_entry.mode) {
    case AnimationMode::Classic:
        offset.z = settings->classicRiseSpeed * a_age;
        break;
    case AnimationMode::Arc: {
        float progress = a_age / a_entry.duration;
        float horizontal = settings->arcSpread * progress;
        offset.x = std::cos(a_entry.arcAngle) * horizontal;
        offset.y = std::sin(a_entry.arcAngle) * horizontal;

        float half = a_entry.duration * 0.5f;
        float v0 = (settings->arcHeight + 0.5f * settings->arcGravity * half * half) / half;
        float height = v0 * a_age - 0.5f * settings->arcGravity * a_age * a_age;
        offset.z = std::max(0.0f, height);
        break;
    }
    case AnimationMode::Bounce:
        a_entry.velocity.z -= settings->bounceGravity * a_delta;
        a_entry.offset.z += a_entry.velocity.z * a_delta;
        if (a_entry.offset.z < settings->bounceGroundLevel) {
            a_entry.offset.z = settings->bounceGroundLevel;
            a_entry.velocity.z = -a_entry.velocity.z * settings->bounceDamping;
        }
        offset = a_entry.offset;
        break;
    case AnimationMode::Spiral: {
        float angle = a_entry.spiralAngle + settings->spiralSpeed * a_age * 2.0f * kPi;
        float radius = settings->spiralRadius + (settings->spiralExpand * a_age * settings->spiralRadius);
        offset.x = std::cos(angle) * radius;
        offset.y = std::sin(angle) * radius;
        offset.z = settings->spiralRiseSpeed * a_age;
        break;
    }
    case AnimationMode::Fountain:
        a_entry.velocity.z -= settings->fountainGravity * a_delta;
        a_entry.offset.x += a_entry.velocity.x * a_delta;
        a_entry.offset.y += a_entry.velocity.y * a_delta;
        a_entry.offset.z += a_entry.velocity.z * a_delta;
        offset = a_entry.offset;
        break;
    case AnimationMode::Matrix: {
        float travel = settings->matrixFallSpeed * a_age;
        travel = std::min(travel, settings->matrixTravelDistance);
        offset.z = -a_entry.matrixDirection * travel;
        break;
    }
    default:
        break;
    }

    return offset;
}

float FloatingDamageManager::ComputeAlpha(const Entry& a_entry, float a_progress) const
{
    auto* settings = Settings::GetSingleton();
    float fadeStart = (a_entry.mode == AnimationMode::Matrix) ? settings->matrixFadeDelay : settings->classicFadeStart;
    if (a_progress <= fadeStart) {
        return 1.0f;
    }

    float denom = std::max(0.001f, 1.0f - fadeStart);
    float t = (a_progress - fadeStart) / denom;
    return std::clamp(1.0f - t, 0.0f, 1.0f);
}

float FloatingDamageManager::ComputeDistanceScale(float a_distance) const
{
    auto* settings = Settings::GetSingleton();
    if (!settings->scaleWithDistance) {
        return 1.0f;
    }

    float minScale = settings->distanceScaleMin;
    float maxScale = settings->distanceScaleMax;
    float scale = maxScale - ((a_distance / 1000.0f) * (maxScale - minScale));
    return std::clamp(scale, minScale, maxScale);
}

std::string FloatingDamageManager::FormatNumber(float a_value, bool a_isHeal) const
{
    int rounded = static_cast<int>(std::round(a_value));
    if (a_isHeal && Settings::GetSingleton()->showPlusForHealing) {
        return std::format("+{}", rounded);
    }
    return std::format("{}", rounded);
}

