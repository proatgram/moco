#include "Surface.hpp"

using namespace moco::wayland::implementation;
using namespace wayland::server;

Surface::SurfaceState::SurfaceState(const std::shared_ptr<Buffer> &buffer) :
    m_surfaceBuffer(buffer)
{
    pixman_region32_init(&m_damageTracker);
}

Surface::SurfaceState::~SurfaceState() {
    pixman_region32_fini(&m_damageTracker);
}

Surface::SurfaceState::SurfaceState(const SurfaceState &other) {
    // Only copy if we need to
    if (m_surfaceBuffer != other.m_surfaceBuffer) {
        m_surfaceBuffer = other.m_surfaceBuffer;
    }

    pixman_region32_init(&m_damageTracker);
    pixman_region32_copy(&m_damageTracker, &other.m_damageTracker);
}

auto Surface::SurfaceState::AddBuffer(const std::shared_ptr<Buffer> &buffer) -> void {
    m_surfaceBuffer = buffer;
}

auto Surface::SurfaceState::GetBuffer() const -> std::shared_ptr<Buffer> {
    return m_surfaceBuffer;
}

auto Surface::SurfaceState::AddDamage(int x, int y, int width, int height) -> void {
    pixman_region32_union_rect(&m_damageTracker, &m_damageTracker, x, y, width, height);
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

auto Surface::SurfaceState::Reset() -> void {
    pixman_region32_clear(&m_damageTracker);
}

Surface::Surface(surface_t surface, Private) :
    Surface(surface) {}

Surface::Surface(surface_t surface) :
    ObjectImplementationBase(surface)
{
    on_attach() = [this](buffer_t buffer, int x, int y) -> void {HandleAttach(buffer, x, y);};
}

auto Surface::HandleAttach(buffer_t buffer, [[maybe_unused]] int x = 0, [[maybe_unused]] int y = 0) -> void {
    // The buffer should be defined with implementation here. It's
    // undefined anyway if it's not, but since it should be defined
    // we can use `Get`, which allows us to not have to specify a format.
    // We add the buffer to the current pending state, to later be committed
    m_pendingState.AddBuffer(Buffer::Get(buffer));
}

auto Surface::HandleDamage(int x, int y, int width, int height) -> void {
    
}

auto Surface::ApplyTransformations(const std::vector<Area> &areas, int scale, output_transform transform) -> std::vector<Area> {
    std::vector<Area> transformedAreas;
    for (const Area &area : areas) {
        Area transformedArea(area);

        transformedAreas.push_back(std::move(transformedArea));
    }

    return transformedAreas;
}
