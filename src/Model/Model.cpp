#include "Model.h"

#include <cassert>

#include "Core/Primitives.h"
#include "Topology/Validation.h"
#include "Topology/Tools.h"
#include "Geometry/Geometry.h"
#include "Utilities/Mapped.h"

VertexId Model::CreateVertex(const Vec3& position)
{
    Vertex v{};
    v.position = position;

    return vertices_.Insert(v);
}

bool Model::RemoveVertex(VertexId id)
{
    if (!vertices_.Contains(id))
        return false;

    // TODO: remove dependent edges

    vertices_.Remove(id);

    return true;
}

const Vertex& Model::GetVertex(VertexId id) const
{
    assert(vertices_.Contains(id));
    return vertices_.Get(id);
}


std::optional<EdgeId>
Model::CreateEdge(VertexId a, VertexId b)
{
    if (!CanCreateEdge(a, b))
        return std::nullopt;

    Edge e{};
    e.a = a;
    e.b = b;

    return edges_.Insert(e);
}

bool Model::RemoveEdge(EdgeId id)
{
    if (!edges_.Contains(id))
        return false;

    edges_.Remove(id);

    return true;
}

const Edge& Model::GetEdge(EdgeId id) const
{
    assert(edges_.Contains(id));
    return edges_.Get(id);
}

std::optional<FaceId>
Model::CreateFace(std::span<const EdgeId> edges)
{
    if (!CanCreateFace(edges))
        return std::nullopt;

    Face f{};
    f.edges.assign(edges.begin(), edges.end());

    return faces_.Insert(f);
}

bool Model::RemoveFace(FaceId id)
{
    if (!faces_.Contains(id))
        return false;

    faces_.Remove(id);

    return true;
}

const Face& Model::GetFace(FaceId id) const
{
    assert(faces_.Contains(id));
    return faces_.Get(id);
}

std::optional<VolumeId>
Model::CreateVolume(std::span<const FaceId> faces)
{
    if (!CanCreateVolume(faces))
        return std::nullopt;

    Volume v{};
    v.faces.assign(faces.begin(), faces.end());

    return volumes_.Insert(v);
}

bool Model::RemoveVolume(VolumeId id)
{
    if (!volumes_.Contains(id))
        return false;

    volumes_.Remove(id);

    return true;
}

const Volume& Model::GetVolume(VolumeId id) const
{
    assert(volumes_.Contains(id));
    return volumes_.Get(id);
}

bool Model::ContainsVertex(VertexId id) const { return vertices_.Contains(id); }
bool Model::ContainsEdge(EdgeId id)   const { return edges_.Contains(id); }
bool Model::ContainsFace(FaceId id)   const { return faces_.Contains(id); }
bool Model::ContainsVolume(VolumeId id) const { return volumes_.Contains(id); }

std::span<const Vertex> Model::Vertices() const {
    return vertices_.Dense();
}

std::span<const Edge> Model::Edges() const {
    return edges_.Dense();
}

std::span<const Face> Model::Faces() const {
    return faces_.Dense();
}

std::span<const Volume> Model::Volumes() const {
    return volumes_.Dense();
}

bool Model::CanCreateEdge(VertexId a, VertexId b) const
{
    if (a == b)
        return false;

    if (!vertices_.Contains(a) || !vertices_.Contains(b))
        return false;

    // Prevent duplicate edges (undirected)
    for (const Edge& e : edges_.Dense()) {
        if ((e.a == a && e.b == b) ||
            (e.a == b && e.b == a))
            return false;
    }

    return true;
}

bool Model::CanCreateFace(std::span<const EdgeId> edges) const
{
    if (edges.size() < 3)
        return false;

    for (EdgeId eid : edges)
        if (!edges_.Contains(eid))
            return false;

    Face temp{};
    temp.edges.assign(edges.begin(), edges.end());

    if (!Topology::IsValidFace(temp, edges_))
        return false;

    auto vertexIds = Topology::ExtractVertices(temp, edges_);

    // get vertices
    const auto vertices = vertices_.Get(vertexIds);
    const auto positions = Mapped(vertices, &Vertex::position);;

    if (!Geometry::AreCoplanar(positions))
        return false;

    return true;
}

bool Model::CanCreateVolume(std::span<const FaceId> faces) const
{
    if (faces.size() < 4)
        return false;

    for (FaceId fid : faces)
        if (!faces_.Contains(fid))
            return false;

    Volume temp{};
    temp.faces.assign(faces.begin(), faces.end());

    if (!Topology::IsValidVolume(temp, faces_))
        return false;

    return true;
}




