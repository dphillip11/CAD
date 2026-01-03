┌────────────────────────────┐
│        Rendering           │  GPU, draw calls, buffers
├────────────────────────────┤
│        Primitives          │  Topology & validation
├────────────────────────────┤
│        Geometry            │  Math on positions, no knowledge of topological structs
├────────────────────────────┤
│        Storage             │  Dense/Sparse containers
├────────────────────────────┤
│        Core Types          │  Vec3, IDs, POD structs
└────────────────────────────┘

## Enforcement Rules (Non-Negotiable)

* No IDs/Primitives in Geometry
* No Vec3 math in Topology
* No validation in data structs
* No rendering outside Renderer

All boundary crossings are explicit
Templates live where they are visible

Violations should be rejected in code review.

## Design Philosophy

Topology defines what exists.
Geometry defines where it is.
Rendering defines how it is drawn.

This separation:
* enables partial / invalid editing states
* improves cache coherence
* supports future Boolean operations
* avoids architectural dead ends

src/
├─ core/
│  ├─ Vec3.h
│  ├─ AABB.h
│  ├─ Id.h
│  └─ Primitives.h
│
├─ geometry/
│  ├─ Geometry.h
│  ├─ Colinearity.cpp
│  ├─ Coplanarity.cpp
│  └─ Epsilon.h
│
├─ storage/
│  ├─ DenseSparse.h
│  └─ IdAllocator.h
│
├─ primitives/
│  ├─ Vertex.h
│  ├─ Edge.h
│  ├─ Face.h
│  ├─ Volume.h
│  │
│  ├─ Extraction.h
│  ├─ TopologyValidation.h
│  └─ GeometryValidation.h
│
├─ model/
│  ├─ CADModel.h
│  ├─ CADModel.cpp
│  ├─ EditCommands.h
│  ├─ UndoRedo.h
│  └─ ChangeEvents.h
|
├─ modelView/
|   ├─ Vertex, Edge, Face, Volume
│
├─ renderer/
│  ├─ Renderer.h
│  ├─ Renderer.cpp
│  ├─ RenderCache.h
│  ├─ RenderCache.cpp
│  │
│  ├─ passes/
│  │  ├─ LinePass.h
│  │  ├─ TrianglePass.h
|  |  ├─ PickerPass.h 
│  │
│  └─ gpu/
│     ├─ VertexBuffer.h
│     ├─ IndexBuffer.h
│     └─ Pipeline.h
│
├─ app/
│  ├─ Application.h
│  └─ main.cpp
│
└─ ARCHITECTURE.md


Rendering is split into how and what, the render pass deals with the more static configuration.
GPU resources are allocated to the gpu by the resources class, updating the buffers(vertices, indices, uniforms) and changing buffer configuration should be handled by the renderer class and this will be the hub for responding to callbacks once the render loop becomes event driven.

