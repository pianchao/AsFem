// this is a test input file for mesh generation test

[mesh]
  type=asfem
  dim=2
  xmax=10.0
  ymax=2.0
  nx=200
  ny=40
  meshtype=quad4
[end]

[dofs]
name=c
[end]

[elmts]
  [elmt1]
    type=diffusion
    dofs=c
    mate=mate1
  [end]
[end]

[mates]
  [mate1]
    type=constdiffusion
    params=1.0e2
  [end]
[end]

[timestepping]
  type=be
  dt=1.0e-5
  time=1.0e-3
  adaptive=true
  optiters=3
[end]

[projection]
name=projc dcx dcy
[end]


[bcs]
  [flux]
    type=nodalneumann
    dof=c
    value=-0.1
    boundary=left
  [end]
[end]

[job]
  type=transient
  debug=dep
[end]
