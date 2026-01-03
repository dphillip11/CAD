Roadmap:
Selection;
Buttons;
Text;

Face vertices had to be flattened in order to support webgl. As we were drawing by index we need to map indices back to our cad model primitive ids. This was prossible using gl_PrimitiveId but this is not present when the geometry shader is removed from the pipeline. Webgl does not support geometry shaders.

An alternative to this is pivotting to a more modern API such as WebGPU but for the scope of this project we will stick with webGL and see where the chips fall.

Perhaps extracting shader compilation into the build process will allow for swapping API more easily. We could also add shader compilation to our pre commit and CI flow.