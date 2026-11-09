.. _migrate_8_0_0:

Migrating to 8.0.0
==================

8.0.0 binds MuJoCo 4.0.0. Read MuJoCo's own migration notes in its
`changelog <https://mujoco.readthedocs.io/en/4.0.0/changelog.html>`_ first: most of the breaking
changes of this release come from the model and the simulation, not from the wrappers.


Flex materials are per element
------------------------------

:docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>flex_matid` holds one entry per
flex element. Index it with the element address of the flex.

.. code-block:: rust

    // Before
    let matid = model.flex_matid()[flex_id];

    // After
    let first_element = model.flex_elemadr()[flex_id] as usize;
    let matid = model.flex_matid()[first_element + element];

The material of :docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex` is a list of names.

.. code-block:: rust

    // Before
    flex.with_material("cloth");

    // After: one name that every element uses, or one name per element.
    flex.set_material("cloth");


Flex vertex equality
--------------------

MuJoCo 4.0.0 removes the vertex equality of flexes and the arrays it used (``flexvert_J``,
``flexvert_length``, ``flex_vertedge`` and the others). Give a cloth continuum elasticity with the
``discrete`` integrator, or an edge equality: set the ``equality`` of
:docs-rs:`~mujoco_rs::wrappers::mj_editing::<struct>MjFlexcompConfig` to 1 (edges) or 3 (strain).
