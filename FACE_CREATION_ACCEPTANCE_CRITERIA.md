# Face Creation Mode - Acceptance Criteria

## Mode Activation

- [ ] User can toggle into Face Creation mode (keyboard shortcut or UI button)
- [ ] Visual indicator shows current mode is active
- [ ] Exiting mode clears all in-progress creation state

## Vertex Placement - Ground Plane (First 3 Vertices)

- [ ] Clicking empty space places vertex on ground plane (y=0)
- [ ] Clicking on existing face places vertex at 3D intersection point on that face
- [ ] Each vertex appears as a highlighted/distinct visual marker
- [ ] First vertex placed successfully

## Edge Creation (Automatic)

- [ ] Second vertex automatically creates edge between vertex 1 and 2
- [ ] Edge renders as preview line connecting vertices
- [ ] Third vertex creates edge from vertex 2 to 3
- [ ] Each subsequent vertex creates edge from previous to current

## Coplanar Snapping (4+ Vertices)

- [ ] After 3 vertices placed, working plane is calculated
- [ ] Mouse hover shows preview sphere snapped to working plane
- [ ] Fourth and subsequent clicks place vertices constrained to working plane
- [ ] Plane normal calculated correctly from first 3 vertices

## Face Closure

- [ ] When mouse is near first vertex (within threshold), snap indicator appears
- [ ] Clicking near first vertex closes the loop with final edge
- [ ] Face is created automatically with all edges
- [ ] New face renders correctly with standard face material
- [ ] Creation state clears after successful face creation
- [ ] Minimum 3 vertices required to close face

## Validation

- [ ] Cannot create face with less than 3 vertices
- [ ] All vertices must be coplanar (within tolerance)
- [ ] Edges must form a closed loop
- [ ] Face creation fails gracefully with error feedback if invalid

## Visual Feedback

- [ ] Preview vertices render distinctly (highlighted spheres)
- [ ] Preview edges render as dashed/different style from final geometry
- [ ] Hover position shows sphere at next vertex location
- [ ] First vertex highlights when in snap-to-close range
- [ ] Working plane optionally visualized after 3 vertices

## Undo/Redo Support

- [ ] Each vertex creation is individually undoable
- [ ] Each edge creation is individually undoable
- [ ] Face creation is undoable
- [ ] Redo works correctly for all operations
- [ ] Undo during face creation removes most recent vertex/edge

## Cancellation

- [ ] ESC key exits face creation mode
- [ ] ESC key with no geometry placed: clean exit
- [ ] ESC key with partial geometry: prompts or auto-undoes placed elements
- [ ] State fully cleared on cancellation

## Edge Cases

- [ ] Clicking on first vertex immediately (2 vertices) does not create invalid face
- [ ] Non-coplanar click attempts (4+ vertices) project to working plane
- [ ] Very small faces (< epsilon dimensions) handled appropriately
- [ ] Self-intersecting edge loops prevented or handled

## Integration

- [ ] Face creation works on Ubuntu, macOS, and Web builds
- [ ] Created faces integrate with existing selection/extrusion system
- [ ] Created faces appear in model's face list correctly
- [ ] Created geometry persists through save/load (if applicable)

## Performance

- [ ] Hover position updates at interactive framerate (>30fps)
- [ ] Ray-plane intersection calculations do not cause lag
- [ ] Preview rendering does not significantly impact performance
