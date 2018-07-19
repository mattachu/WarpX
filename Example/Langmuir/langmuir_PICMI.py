import numpy as np
from pywarpx import picmi

nx = 64
ny = 64
nz = 64

xmin = -20.e-6
ymin = -20.e-6
zmin = -20.e-6
xmax = +20.e-6
ymax = +20.e-6
zmax = +20.e-6

uniform_plasma = picmi.UniformDistribution(density=1.e25, upper_bound=[0., None, None], directed_velocity=[0.1, 0., 0.])

electrons = picmi.Species(particle_type='electron', name='electrons', initial_distribution=uniform_plasma)

grid = picmi.Cartesian3DGrid(number_of_cells = [nx, ny, nz],
                             lower_bound = [xmin, ymin, zmin],
                             upper_bound = [xmax, ymax, zmax],
                             lower_boundary_conditions = ['periodic', 'periodic', 'periodic'],
                             upper_boundary_conditions = ['periodic', 'periodic', 'periodic'],
                             moving_window_velocity = [0., 0., 0.],
                             max_grid_size=32, max_level=0, coord_sys=0)

solver = picmi.ElectromagneticSolver(grid=grid, cfl=1.)

sim = picmi.Simulation(solver = solver,
                       verbose = 1,
                       max_steps = 40,
                       plot_int = 1,
                       current_deposition_algo = 3,
                       charge_deposition_algo = 0,
                       field_gathering_algo = 0,
                       particle_pusher_algo = 0)

sim.add_species(electrons, layout=picmi.GriddedLayout(n_macroparticle_per_cell=[2,2,2], grid=grid))

# write_inputs will create an inputs file that can be used to run
# with the compiled version.
sim.write_input_file(inputs_name='inputs_from_PICMI')

# Alternatively, sim.step will run WarpX, controlling it from Python
#sim.step()

