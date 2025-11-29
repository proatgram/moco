#include "Surface.hpp"

#include "Matrix.hpp"

#include <numbers>

using namespace moco::wayland::implementation;
using namespace wayland::server;

Surface::SurfaceState::SurfaceState(const std::shared_ptr<Buffer> &buffer) :
    m_buffer(buffer)
{
    pixman_region32_init(&m_damageTracker);
}

Surface::SurfaceState::~SurfaceState() {
    pixman_region32_fini(&m_damageTracker);
}

Surface::SurfaceState::SurfaceState(const SurfaceState &other) {
    // Only copy if we need to
    if (m_buffer != other.m_buffer) {
        m_buffer = other.m_buffer;
    }

    pixman_region32_init(&m_damageTracker);
    pixman_region32_copy(&m_damageTracker, &other.m_damageTracker);
}

auto Surface::SurfaceState::AddBuffer(const std::shared_ptr<Buffer> &buffer) -> void {
    m_buffer = buffer;
    UpdateLogicalSurfaceDimensions();
}

auto Surface::SurfaceState::GetBuffer() const -> std::shared_ptr<Buffer> {
    return m_buffer;
}

auto Surface::SurfaceState::AddBufferDamage(int x, int y, size_t width, size_t height) -> void {
    pixman_region32_union_rect(&m_damageTracker, &m_damageTracker, x, y, width, height);
}

auto Surface::SurfaceState::AddBufferDamage(const Area &area) -> void {
    AddBufferDamage(area.x, area.y, area.width, area.height);
}

auto Surface::SurfaceState::AddSurfaceDamage(int x, int y, size_t width, size_t height) -> void {
    AddSurfaceDamage({x, y, width, height});
}

auto Surface::SurfaceState::AddSurfaceDamage(const Area &area) -> void {
    m_surfaceDamage.push_back(area);
}

auto Surface::SurfaceState::GetSurfaceDamage() const -> std::vector<Area> {
    return m_surfaceDamage;
}

auto Surface::SurfaceState::GetDamagedRegions() const -> std::vector<Area> {
    int numberOfBoxes;
    pixman_box32_t *boxes = pixman_region32_rectangles(&m_damageTracker, &numberOfBoxes);
    if (numberOfBoxes == 0) {
        return {};
    }

    std::vector<Area> damagedRegions;
    for (int i = 0; i < numberOfBoxes; i++) {
        Area area;
        area.x = boxes[i].x1;
        area.y = boxes[i].y1;
        area.width = abs(boxes[i].x2 - boxes[i].x1);
        area.height = abs(boxes[i].y2 - boxes[i].y1);
        damagedRegions.push_back(std::move(area));
    }

    return damagedRegions;
}

auto Surface::SurfaceState::SetBufferTransform(const output_transform &transorm) -> void {
    m_bufferTransform = transorm;
    UpdateLogicalSurfaceDimensions();
}

auto Surface::SurfaceState::GetBufferTransform() const -> output_transform {
    return m_bufferTransform;
}

auto Surface::SurfaceState::GetBufferScale() const -> int {
    return m_bufferScale;
}

auto Surface::SurfaceState::SetBufferScale(int scale) -> void {
    m_bufferScale = scale;
    UpdateLogicalSurfaceDimensions();
}

auto Surface::SurfaceState::GetLogicalWidth() const -> size_t {
    return m_surfaceWidth;
}

auto Surface::SurfaceState::GetLogicalHeight() const -> size_t {
    return m_surfaceHeight;
}

auto Surface::SurfaceState::SetBufferOffset(int x, int y) -> void {
    m_bufferOffsetX = x;
    m_bufferOffsetY = y;
}

auto Surface::SurfaceState::GetBufferOffset() const -> std::pair<int, int> {
    return std::make_pair<int, int>(m_bufferOffsetX, m_bufferOffsetY);
}

auto Surface::SurfaceState::GetFrameCallbacks() const -> std::vector<callback_t> {
    return m_frameCallbacks;
}

auto Surface::SurfaceState::SetOpaqueRegion(std::shared_ptr<Region> region) -> void {
    m_opaqueRegion = region;
}

auto Surface::SurfaceState::GetOpaqueRegion() const -> std::shared_ptr<Region> {
    return m_opaqueRegion;
}

auto Surface::SurfaceState::SetInputRegion(std::shared_ptr<Region> region) -> void {
    m_inputRegion = region;
}

auto Surface::SurfaceState::GetInputRegion() const -> std::shared_ptr<Region> {
    return m_inputRegion;
}

auto Surface::SurfaceState::Reset() -> void {
    pixman_region32_clear(&m_damageTracker);
    m_surfaceDamage.clear();

    m_frameCallbacks.clear();
}

// Only gets the dimensions for the logical surface, not the content within it.
auto Surface::SurfaceState::UpdateLogicalSurfaceDimensions() -> void {
    // Undo scaling done by client to the buffer
    size_t logicalWidth = m_buffer->GetWidth() / m_bufferScale;
    size_t logicalHeight = m_buffer->GetHeight() / m_bufferScale;

    // Undo transformation done by the client to the buffer
    if (m_bufferTransform == output_transform::flipped_90) {
        // If the buffer is rotated 90°, then the width and height are swapped
        std::swap(logicalWidth, logicalHeight);
    } else if (m_bufferTransform == output_transform::flipped_270) {
        // If the buffer is rotated 270°, then the width and height are swapped
        std::swap(logicalWidth, logicalHeight);
    } else if (m_bufferTransform == output_transform::_90) {
        // If the buffer is rotated 90°, then the width and height are swapped
        std::swap(logicalWidth, logicalHeight);
    } else if (m_bufferTransform == output_transform::_270) {
        // If the buffer is rotated 270°, then the width and height are swapped
        std::swap(logicalWidth, logicalHeight);
    }

    m_surfaceWidth = logicalWidth;
    m_surfaceHeight = logicalHeight;
}

auto Surface::SurfaceState::ConvertSurfaceToBuffer(const Area &area) -> Area {
    // First, we need to convert from logical surface coordiantes
    // back to physical buffer coordinates. Helper object to do that.
    helper::Transformation transformation(area.x, area.y);

    size_t newWidth = area.width;
    size_t newHeight = area.height;

    // Move origin to center of the surface so the
    // transformations work correctly.
    transformation.AddTranslation(GetLogicalWidth() / -2, GetLogicalHeight() / -2);

    // Perform the transformation to get to the buffer state
    output_transform transform = GetBufferTransform();
    if (transform == output_transform::flipped) {
        transformation.AddYReflect();
    } else if (transform == output_transform::flipped_90) {
        transformation.AddYReflect();
        transformation.AddRotate(std::numbers::pi / 2.0);
        std::swap(newWidth, newHeight);
    } else if (transform == output_transform::flipped_180) {
        transformation.AddYReflect();
        transformation.AddRotate(std::numbers::pi);
    } else if (transform == output_transform::flipped_270) {
        transformation.AddYReflect();
        transformation.AddRotate((3.0 / 2.0) * std::numbers::pi);
        std::swap(newWidth, newHeight);
    } else if (transform == output_transform::_90) {
        transformation.AddRotate(std::numbers::pi / 2.0);
        std::swap(newWidth, newHeight);
    } else if (transform == output_transform::_180) {
        transformation.AddRotate(std::numbers::pi);
    } else if (transform == output_transform::_270) {
        transformation.AddRotate((3.0 / 2.0) * std::numbers::pi);
        std::swap(newWidth, newHeight);
    }

    // Translate points back to the "original" position
    transformation.AddTranslation(GetLogicalWidth() * 2, GetLogicalHeight() * 2);

    // Add the buffer scale
    transformation.AddScale(GetBufferScale(), GetBufferScale());

    // Handle buffer offset
    transformation.AddTranslation(m_bufferOffsetX * -1, m_bufferOffsetY * -1);

    auto [newX, newY] = transformation.GetTransformedPoints();

    return Area{.x = newX,
                .y = newY,
                .width = newWidth,
                .height = newHeight};
}

Surface::Surface(surface_t surface, Private) :
    Surface(surface) {}

Surface::Surface(surface_t surface) :
    ObjectImplementationBase(surface)
{
    on_attach() = [this](buffer_t buffer, int x, int y) -> void {HandleAttach(buffer, x, y);};
    on_damage() = [this](int x, int y, int width, int height) -> void {HandleDamage(x, y, width, height);};
    on_frame() = [this](callback_t callback) -> void {HandleFrame(callback);};
    on_set_opaque_region() = [this](region_t region) -> void {HandleSetOpaqueRegion(region);};
    on_set_input_region() = [this](region_t region) -> void {HandleSetInputRegion(region);};
    on_commit() = [this]() -> void {HandleCommit();};
    on_set_buffer_transform() = [this](output_transform transform) -> void {HandleSetBufferTransform(transform);};
    on_set_buffer_scale() = [this](int scale) -> void {HandleSetBufferScale(scale);};
    on_damage_buffer() = [this](int x, int y, int width, int height) -> void {HandleDamageBuffer(x, y, width, height);};
    on_offset() = [this](int x, int y) -> void {HandleOffset(x, y);};
}

auto Surface::HandleAttach(buffer_t buffer, [[maybe_unused]] int x = 0, [[maybe_unused]] int y = 0) -> void {
    // The buffer should be defined with implementation here. It's
    // undefined anyway if it's not, but since it should be defined
    // we can use `Get`, which allows us to not have to specify a format.
    // We add the buffer to the current pending state, to later be committed.
    m_pendingState.AddBuffer(Buffer::Get(buffer));
}

auto Surface::HandleDamage(int x, int y, size_t width, size_t height) -> void {
    // Surface damage withh be converted to buffer local upon commit
    m_pendingState.AddSurfaceDamage(x, y, width, height);
}

auto Surface::HandleFrame(callback_t callback) -> void {
    m_pendingState.AddFrameCallback(callback);
}

auto Surface::HandleSetOpaqueRegion(region_t region) -> void {
    m_pendingState.SetOpaqueRegion(Region::Get(region));
}

auto Surface::HandleSetInputRegion(region_t region) -> void {
    m_pendingState.SetInputRegion(Region::Get(region));
}

auto Surface::HandleCommit() -> void {
    // Once we get the commit request, we know that no other
    // transactions will happen, thus we can safely convert
    // surface to buffer.
    for (const Area &surfaceArea : m_pendingState.GetSurfaceDamage()) {
        m_pendingState.AddBufferDamage(m_pendingState.ConvertSurfaceToBuffer(surfaceArea));
    }

    m_activeStates.push(m_pendingState);

    m_pendingState.Reset();
}

auto Surface::HandleSetBufferTransform(output_transform transform) -> void {
    m_pendingState.SetBufferTransform(transform);
}

auto Surface::HandleSetBufferScale(int scale) -> void {
    m_pendingState.SetBufferScale(scale);
}

auto Surface::HandleDamageBuffer(int x, int y, size_t width, size_t height) -> void {
    m_pendingState.AddBufferDamage({x, y, width, height});
}

auto Surface::HandleOffset(int x, int y) -> void {
    m_pendingState.SetBufferOffset(x, y);
}
