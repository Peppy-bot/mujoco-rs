8.0.0 (MuJoCo 4.0.0)
====================

.. rubric:: Breaking changes

- MuJoCo-rs now builds against MuJoCo 4.0.0 FFI. See MuJoCo's changelog for this release:
  https://mujoco.readthedocs.io/en/4.0.0/changelog.html. See :ref:`migrate_8_0_0`.
- MuJoCo 4.0.0 removes the vertex equality of flexes. |mj_model| loses ``nJfv``,
  ``flex_vertedgeadr``, ``flex_vertedgenum``, ``flex_vertedge``, ``flex_vertmetric``,
  ``flexvert_J_rownnz``, ``flexvert_J_rowadr`` and ``flexvert_J_colind``, and |mj_data| loses
  ``flexvert_J`` and ``flexvert_length``. The ``equality`` of
  :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjFlexcompConfig` takes 0 (none), 1 (edges) or
  3 (strain); MuJoCo refuses any other value.
- :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>flex_matid` holds one material
  id per flex element (``nflexelem`` entries), not one per flex.
- The material of :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex` is a list of names:
  one name that every element uses, or one name per element.
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex::<method>set_material` takes the
  names separated by white space and
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex::<method>append_material` :sup:`new`
  adds one. ``MjsFlex::material`` and ``MjsFlex::with_material`` are removed.
- ``mjtEnableBit::mjNENABLE`` is an associated constant, not a variant: MuJoCo's count of the enable
  flags (8) equals the value of ``mjENBL_INVDISCRETE``.
- ``mjSTATE_PHYSICS``, ``mjSTATE_FULLPHYSICS`` and ``mjSTATE_INTEGRATION`` include the new
  ``mjSTATE_PLASTIC``.

.. rubric:: New features and improvements

*Elasto-plasticity*

- |mj_model| gains ``nplastic``,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>jnt_springyield` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>jnt_plasticadr` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>tendon_springyield` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>tendon_plasticadr` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>flex_yield` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>flex_plasticadr` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>flex_plasticmode` :sup:`new` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>key_plastic` :sup:`new`.
  The addresses and the plastic mode are ``unsafe`` to write, because MuJoCo indexes the plastic
  state with them. The joint, tendon and keyframe views carry the same fields.
- |mj_data| gains :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>plastic` :sup:`new`,
  the plastic state.
- :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjsJoint` and |mjs_tendon| gain
  ``springyield``, :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex` gains ``stressyield``
  and ``shear`` (the torsion shear modulus of a 1D flex), and
  :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjsKey` gains ``plastic``.

*Model and data*

- |mj_data| gains the Cartesian quaternions
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>xiquat` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>geom_xquat` :sup:`new` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>site_xquat` :sup:`new`, which the
  body, geom and site data views also carry.
- :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>tendon_type` :sup:`new` tells a
  fixed tendon from a spatial one (``MjtTendon``); it is ``unsafe`` to write.
- :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>geom_castshadow` :sup:`new` and
  ``castshadow`` of :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjsGeom` turn the shadow of
  a geom on and off. MuJoCo's contacts carry ``surfacevel``, the relative surface velocity in the
  contact frame.
- The viewer's options list the ``Speculative`` enable flag and the ``Post-process`` render flag.

*Fork additions*

- |mj_data| gains :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>model_qpos_spring_mut`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>equality_forces`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>model_mesh_vert_mut`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>model_mesh_normal_mut` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>model_mesh_texcoord_mut`.
- |mj_data| gains an allocation-free variant of the remaining Jacobian methods, which writes into
  a buffer that the caller keeps:
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_into`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_body_into`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_body_com_into`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_geom_into` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_site_into`, each with a
  fallible ``try_`` variant.

.. rubric:: Bug fixes

- A static link (``MUJOCO_STATIC_LINK_DIR``) loads OBJ, STL and GMSH mesh files and reads and
  writes ``.mjz`` archives. MuJoCo registers those codecs from constructors in objects nothing
  references, so a plain archive link drops them; the ``mujoco`` submodule's static build carries a
  table that names them.
- A static link with the ``cpp-viewer`` feature links on macOS. The build script names the Apple
  frameworks the static GLFW and Simulate resolve (Cocoa, IOKit, CoreFoundation and CoreVideo).
- A copy of |mj_model| shares the layout snapshot of the original even when the original had not
  built it yet, so
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>is_compatible_with_model`
  between the two is a pointer comparison.

.. rubric:: Other changes

- A static link names ``miniz``, the archive library the ``.mjz`` codec needs. Rebuild the static
  libraries from the updated submodule, see :ref:`static_linking`.
- The test of the model layout reads ``mjxmacro.h`` from the ``mujoco`` submodule when the crate
  root holds no downloaded release.
