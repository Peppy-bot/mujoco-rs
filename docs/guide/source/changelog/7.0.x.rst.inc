7.0.0 (MuJoCo 3.13.0)
======================

.. rubric:: Breaking changes

*MuJoCo FFI upgraded to 3.13.0*

- MuJoCo-rs now builds against MuJoCo 3.13.0 FFI. This is a breaking dependency-level change for
  consumers tied to older MuJoCo C ABI details. See MuJoCo's changelog for this release:
  https://mujoco.readthedocs.io/en/3.13.0/changelog.html

.. rubric:: New features and improvements

*Site inside test*

- :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>inside_site` :sup:`new` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>try_inside_site` :sup:`new`
  report whether a point lies inside a site; a mesh site tests its convex hull. They wrap
  :docs-rs:`~mujoco_rs::mujoco_c::<fn>mj_insideSite`.

*New* |mj_data| *effective-metric accessors*

- The accessors that read the metric terms MuJoCo fills at a later pipeline stage are ``unsafe fn``:
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_diag`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_sdiag`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_fluid`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_ts` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_tk`, together with their
  ``_mut`` counterparts. Run the velocity stage (``forward`` or ``step1``) before you read them.
- The tendon and actuator lists
  (:docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_tid`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_ts`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_tk`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_aid`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_as` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_ak`) are sized by the
  number of entries the metric holds (``nefmT`` and ``nefmA``), not by the ``ntendon`` and
  ``nactuator`` capacities MuJoCo allocates.
