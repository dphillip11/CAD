#pragma once

#include <vector>
#include <unordered_set>
#include <algorithm>

#include "Core/Primitives.h"

namespace Topology {
    
    // -------------------------------------------------
    // Extract ordered vertex loop from a face
    // -------------------------------------------------
    template <typename EdgeContainer>
    std::vector<VertexId>
    ExtractVertices(const Face& face,
        const EdgeContainer& edges)
    {
        std::vector<VertexId> result;

        if (face.edges.size() < 3)
            return result;

        // Start with first edge
        const Edge& first = edges.Get(face.edges[0]);
        result.push_back(first.a);
        result.push_back(first.b);

        // Chain remaining edges
        for (size_t i = 1; i < face.edges.size(); ++i) {
            const Edge& e = edges.Get(face.edges[i]);
            VertexId last = result.back();

            if (e.a == last) {
                result.push_back(e.b);
            } else if (e.b == last) {
                result.push_back(e.a);
            } else {
                // Non-contiguous edge → invalid face
                return {};
            }
        }

        // Close loop: remove duplicate final vertex if needed
        if (result.front() == result.back())
            result.pop_back();

        return result;
    }

    // -------------------------------------------------
    // Extract unique vertices from a volume
    // -------------------------------------------------
    template <typename FaceContainer, typename EdgeContainer>
    std::vector<VertexId>
    ExtractVertices(const Volume& volume,
        const FaceContainer& faces,
        const EdgeContainer& edges)
    {
        std::unordered_set<VertexId> unique;

        for (FaceId fid : volume.faces) {
            const Face& face = faces.Get(fid);
            auto verts = ExtractVertices(face, edges);
            unique.insert(verts.begin(), verts.end());
        }

        return std::vector<VertexId>(unique.begin(), unique.end());
    }
} // namespace Topology
