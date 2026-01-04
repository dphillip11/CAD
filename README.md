Face vertices had to be flattened in order to support webgl. As I was drawing by index I needed to map indices back to our cad model primitive ids. This was prossible using gl_PrimitiveId but this is not present when the geometry shader is removed from the pipeline. Webgl does not support geometry shaders.

An alternative to this is pivotting to a more modern API such as WebGPU but for the scope of this project we will stick with webGL and see where the chips fall.

Perhaps extracting shader compilation into the build process will allow for swapping API more easily. I could also add shader compilation to our pre commit and CI flow.

Translation of faces brought some new ideas with it, particularly related to how to handle geometry invalidation.

A face can be considered a coplanar polygon but when multiple faces connect then a transformation of one face affects the geometry of the others.

I could impose limitations on the manipulation of geometry to enforce a sense of physical correctness or allow faces to split into coplay sections.

The latter tends towards a more sculpting type approach which could be quite fun. This aligns more closely with the rendering primitives too.

Some ideas that could be interesting:
- procedurally generated meshes
- sculpting brushes
- rotational extrusion
- converting coarse meshes to smooth geometry, Catmull-Clark

