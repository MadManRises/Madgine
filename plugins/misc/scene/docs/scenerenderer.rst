SceneRenderer
=============

.. plugin:: SceneRenderer

The SceneRenderer plugin draws the scene of a SceneManager into a RenderTarget.

``SceneRenderPass`` is a forward renderer with the directional light (``SceneManager::mAmbientLightDirection``) and up to
two shadow casting ``PointLight`` components. It writes the HDR image into target 0 and its bright parts (emissive colour
and pixels brighter than 1) into target 1 of the RenderTarget it is added to. Bloom and tone mapping are up to the user,
e.g. with a ``BloomPass``:

.. code-block:: cpp

    hdr = context->createRenderTexture(size, { .mFormat = FORMAT_RGBA16F, .mTextureCount = 2 });
    hdr->addRenderPass(&sceneRenderPass);

    bloom.setInput(hdr.get(), 0, hdr.get(), 1);
    output = context->createRenderTexture(size, { .mFormat = FORMAT_RGBA16F, .mTextureCount = 2, .mFlipFlop = true });
    output->addRenderPass(&bloom);

Custom lighting
---------------

For a lighting model of its own (e.g. deferred shading with many lights), a project can combine these building blocks
with a render pass of its own:

* ``GBufferRenderPass`` renders the surface attributes of all meshes into the target it is added to (three colour
  textures and depth, optionally multisampled): albedo, view space normal (octahedral encoded, see
  ``Madgine/render/shadinglanguage/normalencoding.hlsl``) and shininess, emissive colour. The layout is documented in
  ``gbufferrenderpass.h``.
* ``ShadowRenderPass`` renders the shadow map of the directional light. ``mBounds`` optionally limits it to the part of
  the view inside a world box. ``light.hlsl`` provides the light structures and the filtered shadow lookup; define
  ``LIGHT_NO_RESOURCES`` before including it to leave out the resources of the forward renderer.
* ``ShadowSceneRenderData`` collects the shadow casting meshes, e.g. for shadow maps of further lights.

Lights are entity components at the position of the entity's ``Transform``:

* ``PointLight``: ``mColor``, ``mIntensity``, ``mRange``, ``mCastShadows``.
* ``SpotLight``: like ``PointLight``, shining along the local +Z axis, with ``mInnerAngle``/``mOuterAngle`` (half angles in
  degrees).

Materials can emit light with ``GPUMeshData::Material::mEmissiveColor``.
