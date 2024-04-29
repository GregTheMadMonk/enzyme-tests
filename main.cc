#include <cassert>
#include <cmath>
#include <filesystem>
#include <format>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace stdf = std::filesystem;

extern "C" int enzyme_allocated
         , enzyme_const
         , enzyme_dup
         , enzyme_duponneed
         , enzyme_out
         , enzyme_tape;

template <typename Retval, typename... Args>
Retval __enzyme_fwddiff(Retval (*)(Args...), auto...);

template <typename Retval, typename... Args>
Retval __enzyme_autodiff(Retval (*)(Args...), auto...);

#define HAVE_TINYXML2
#define HAVE_ZLIB
#include <TNL/Meshes/Mesh.h>
#include <TNL/Meshes/TypeResolver/resolveMeshType.h>

template <typename T, template <typename...> class Template>
concept instance_of = requires (T t) {
    { [] <typename... Types> (const Template<Types...>&) {} (t) };
}; // <-- concept instnace_of

template <typename Real>
[[nodiscard]]
inline constexpr
bool isZero(Real real, Real tol = std::numeric_limits<Real>::epsilon()) {
    return std::abs(real) <= tol;
} // <-- isZero()

struct ConfigTagPermissive {};

namespace TNL::Meshes::BuildConfigTags {

// Enable only 'triangle' topology because compile times are horrible otherwise
template <typename Topology>
struct MeshCellTopologyTag<ConfigTagPermissive, Topology> {
    static constexpr bool enabled = false;
}; // <-- MeshCellTopology<ConfigTagPermissive<Topology>, Topolgoy>
template <>
struct MeshCellTopologyTag<ConfigTagPermissive, Topologies::Triangle> {
    static constexpr bool enabled = true;
}; // <-- MeshCellTopology<ConfigTagPermissive<Topology>, Topolgoy>

} // <-- namespace TNL::Meshes::BuildConfigTags

/**
 * @brief Mesh and mesh data buffers
 *
 * @tparam MeshT TNL Mesh type
 *
 * Instances are not to be created directly. Use only with @ref withMeshFile
 * context
 */
template <typename MeshT>
class Domain {
public:
    /// @brief TNL Mesh type
    using Mesh = MeshT;
    /// @brief Real type
    using Real = Mesh::RealType;

    /// @brief Mesh getter method
    [[nodiscard]]
    inline constexpr
    const Mesh& getMesh() const { return this->mesh; }

    /// @brief Get cell data layer
    [[nodiscard]]
    inline constexpr
    std::vector<Real>& getCellData(std::string_view name)
    { return this->cellData.at(name.data()); }
    [[nodiscard]]
    inline constexpr
    const std::vector<Real>& getCellData(std::string_view name) const
    { return this->cellData.at(name.data()); }

private:
    inline Domain(Mesh&& c_mesh) : mesh(std::move(c_mesh)) {}

    template <typename Func>
    friend inline bool withMeshFile(
        const stdf::path&, const std::vector<std::string>&, Func&&
    );

    /// @brief Stored mesh
    Mesh mesh;

    /// @brief Cell data buffers map
    std::unordered_map<std::string, std::vector<Real>> cellData;
}; // <-- class Domain

/**
 * @brief Mesh domain context
 *
 * Loads the mesh and some of its cell data layers into a Domain class
 * instance and passes it to a user-provided callback
 *
 * @param filename       Mesh file name
 * @param cellDataLayers Cell data layer names to load
 * @param func           User-provided callback
 */
template <typename Func>
inline bool withMeshFile(
    const stdf::path& filename,
    const std::vector<std::string>& cellDataLayers,
    Func&& func
) {
    const auto load = [&] <typename R, typename M> (R& reader, M&& mesh) {
        // TNL also has grids, which we don't need
        if constexpr (instance_of<M, TNL::Meshes::Mesh>) {
            // Set mesh
            Domain domain(std::forward<M>(mesh));
            using Real = decltype(domain)::Real;

            // Load mesh cell data
            for (const auto& cellDataLayerName : cellDataLayers) {
                const auto data = reader.readCellData(cellDataLayerName);
                std::visit(
                    [&] <typename T> (const std::vector<T>& from) {
                        auto& to = domain.cellData[cellDataLayerName];
                        for (const auto& v : from) {
                            to.push_back(static_cast<Real>(v));
                        }
                    }, data
                );
            }

            // Run whatever user function needs to run
            std::forward<Func>(func)(domain);
            return true;
        } else return false;
    }; // <-- loader()

    using TNL::Devices::Host;
    return TNL::Meshes::resolveAndLoadMesh<ConfigTagPermissive, Host>(
        load, filename, "auto"
    );
} // <-- withMeshFile()
 
template <typename Mesh>
struct TraceLog {
    std::vector<typename Mesh::PointType>       points;
    std::vector<typename Mesh::RealType>        steps;
    std::vector<typename Mesh::GlobalIndexType> cells;
}; // <-- struct TraceLog

/**
 * @brief Trace from a starting point until the end of mesh in a specified
 *        direction
 *
 * @param mesh  mesh to trace over
 * @param data  mesh cell data layer
 * @param pos   starting point
 * @param vel   trace velocity
 *
 * @return Full path data
 */
template <typename Mesh>
[[nodiscard]]
inline 
TraceLog<Mesh> traceFrom(
    const Mesh& mesh,
    const std::vector<typename Mesh::RealType>& data,
    typename Mesh::PointType pos,
    typename Mesh::PointType vel
) {
    using Real  = Mesh::RealType;
    using Point = Mesh::PointType;
    using GI = Mesh::GlobalIndexType;
    using LI = Mesh::LocalIndexType;

    constexpr auto cellDim = Mesh::getMeshDimension();
    constexpr auto edgeDim = cellDim - 1;

    const auto cells = mesh.template getEntitiesCount<cellDim>();
    const auto edges = mesh.template getEntitiesCount<edgeDim>();

    const auto trace_next = [&] () -> std::optional<Point> {

        std::optional<Point> ret = std::nullopt;
        Real dst = std::numeric_limits<Real>::max();

        for (GI edgeIdx = 0; edgeIdx < edges; ++edgeIdx) {
            // Go over every edge
            // Get edge point indices
            const auto p1i = mesh.template getSubentityIndex<edgeDim, 0>(
                edgeIdx, 0
            );
            const auto p2i = mesh.template getSubentityIndex<edgeDim, 0>(
                edgeIdx, 1
            );

            // Get points
            const auto p1 = mesh.getPoint(p1i);
            const auto p2 = mesh.getPoint(p2i);

            // Get points positions relative to pos
            const Point r1 = p1 - pos;
            const Point r2 = p2 - pos;

            const auto sgn = [&vel] (const Point& r) {
                // 3rd coordinate of cross product
                return r[0] * vel[1] - r[1] * vel[0];
            }; // <-- sgn()

            // Trajectory does not intersect with the edge
            if (sgn(r1) * sgn(r2) > 0) continue;

            // Find intersection point
            const Point r = p2 - p1;
            assert(!isZero((r, r)));

            // Edge normal
            constexpr auto normalized = [] (Point point) {
                return point / std::sqrt((point, point));
            }; // <-- normalized()
            const Point n = normalized(Point{ -r[1], r[0] });

            // Projections
            const auto distProj = (p1 - pos, n);
            assert(isZero((p1 - pos, n) - (p2 - pos, n)));
            const auto velProj  = (vel, n);

            if (distProj * velProj < 0) continue;
            if (isZero(distProj))       continue;

            const Point step = distProj / velProj * vel;
            const auto newDst = (step, step);
            if (newDst < dst) {
                ret = pos + step;
                dst = newDst;
            }
        }

        return ret;
    }; // <-- trace_next()

    TraceLog<Mesh> acc{ { pos }, {} };

    while (true) {
        // Trace the next point of intersection with grid edge
        const auto next = trace_next();
        if (!next.has_value()) break;

        const auto step = *next - pos;
        const auto step_len = std::sqrt((step, step));

        // Find the current cell
        for (GI cellIdx = 0; cellIdx < cells; ++cellIdx) {
            // Assume convex cell. Compare with center
            const auto center = [&] () -> Point {
                const auto lPoints =
                    mesh.template getSubentitiesCount<
                        cellDim, 0
                    >(cellIdx);

                Point p{};
                for (LI lpi = 0; lpi < lPoints; ++lpi) {
                    p += mesh.getPoint(
                        mesh.template getSubentityIndex<cellDim, 0>(
                            cellIdx, lpi
                        )
                    );
                }

                return p / lPoints;
            } (); // <-- center

            const auto lEdges =
                mesh.template getSubentitiesCount<cellDim, edgeDim>(
                    cellIdx
                );

            const auto inside = [&] () -> bool {
                const Point p = pos + step / 2.0f;
                for (LI lEdgeIdx = 0; lEdgeIdx < lEdges; ++lEdgeIdx) {
                    const auto edgeIdx =
                        mesh.template getSubentityIndex<
                            cellDim, edgeDim
                        >(cellIdx, lEdgeIdx);

                    const auto p1 = mesh.getPoint(
                        mesh.template getSubentityIndex<edgeDim, 0>(
                            edgeIdx, 0
                        )
                    );
                    const auto p2 = mesh.getPoint(
                        mesh.template getSubentityIndex<edgeDim, 0>(
                            edgeIdx, 1
                        )
                    );

                    const Point r = p2 - p1;
                    const Point n{ -r[1], r[0] };
                    const Point d = p  - p1;
                    const Point c = center - p1;

                    if ((n, d) * (n, c) < 0) return false;
                }

                return true;
            } (); // <-- inside

            if (inside) {
                acc.cells.push_back(cellIdx);
                break;
            }
        }

        pos = *next;
        acc.points.push_back(pos);
        acc.steps.push_back(step_len);
    }

    return acc;
} // <-- traceFrom()

/// @brief Weighted sum of all cells' "densities"
template <typename Mesh>
Mesh::RealType pathtrace(
    const Mesh& mesh,
    const std::vector<typename Mesh::RealType>& data,
    const std::vector<typename Mesh::RealType>& startPoint,
    const std::vector<typename Mesh::RealType>& direction
) {
    const auto traceLog = traceFrom(
        mesh, data,
        { startPoint.at(0), startPoint.at(1) },
        { direction.at(0), direction.at(1) }
    );

    typename Mesh::RealType acc{};
    for (std::size_t step = 0; step < traceLog.steps.size(); ++step) {
        acc += data[traceLog.cells[step]] * traceLog.steps[step];
    }
    return acc;
} // <-- wtrace()

int main() {
    withMeshFile(
        "data.vtu", { "numbers" },
        [] <typename Mesh> (Domain<Mesh>& domain) {
            // Mesh dimension
            constexpr auto cellDim = Mesh::getMeshDimension();

            // Print out the data
            domain.getMesh().template forAll<cellDim>(
                [&domain] (auto cellIdx) {
                    std::cout << std::format(
                        "Cell {} scalar data {}\n",
                        cellIdx, domain.getCellData("numbers")[cellIdx]
                    );
                }
            );

            // Simple path trace
            const auto traceLog = traceFrom(
                domain.getMesh(), domain.getCellData("numbers"),
                { -0.7, -0.7 }, { 0.1, 0.2 }
            );

            const auto steps = traceLog.steps.size();
            for (std::size_t step = 0; step < steps; ++step) {
                std::cout << std::format(
                    "[{}] Distance {} in cell {}\n",
                    step, traceLog.steps.at(step), traceLog.cells.at(step)
                );
            }

            using Real = Mesh::RealType;
            const std::vector<Real> start{ -0.7, -0.7 };
            const std::vector<Real> dir  {  0.1,  0.2 };
            std::cout << pathtrace(
                domain.getMesh(), domain.getCellData("numbers"),
                start, dir
            ) << '\n';

            std::vector<Real> dstart(start.size());
            std::vector<Real> ddir(dir.size());
            std::vector<Real> ddata(domain.getCellData("numbers").size());

            __enzyme_autodiff(
                &pathtrace<Mesh>,
                enzyme_const, &domain.getMesh(),
                enzyme_dup, &domain.getCellData("numbers"), &ddata,
                enzyme_dup, &start, &dstart,
                enzyme_dup, &dir, &ddir
            );

            std::cout << "ddata = [ ";
            for (auto e : ddata) std::cout << e << ' ';
            std::cout << "]\n";

            std::cout << "dstart = [ ";
            for (auto e : dstart) std::cout << e << ' ';
            std::cout << "]\n";

            std::cout << "ddir = [ ";
            for (auto e : ddir) std::cout << e << ' ';
            std::cout << "]\n";
        }
    );
    return 0;
}
