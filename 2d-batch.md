# Design points

Has immediate and possibly retained mode

  - Immediate does not preserve batches, its rigid by design
  - Retained has zero overhead for non-changing scene (except rendering of course)

Batch processor is configured and immutable around:

  - Vertex bindings (format)
  - Index size
  - Primitive type
  - Render state: Cull mode, Face winding, Compare, Stencil, Blend factors
  - Sampler state
  - Shader (?)

Batch processor can be reused with implicit flush if one of these changed:

 - Shader uniforms
 - Texture handle (but not the type)
 - Compatible shader (?)

It makes sense to keep around setup-independant batch processor singleton to
deal with drawing order when switching around.