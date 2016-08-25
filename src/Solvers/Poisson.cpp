#include "Poisson.h"
#include "Laplacian.h"

Poisson::Poisson(const FiniteVolumeGrid2D &grid, const Input &input)
    :
      Solver(grid, input),
      phi(addScalarField(input, "phi")),
      gamma(addScalarField("gamma")),
      phiEqn_(phi, "phi")
{
    gamma.fill(input.caseInput().get<Scalar>("Properties.gamma", 1.));
    volumeIntegrators_ = VolumeIntegrator::initVolumeIntegrators(input, *this);
}

Scalar Poisson::solve(Scalar timeStep)
{
    phiEqn_ = (fv::laplacian(gamma, phi) + ib_.eqns(phi) == 0.);
    phiEqn_.solve();
    return phiEqn_.error();
}
