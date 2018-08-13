#include "SolverFactory.h"
#include "Poisson.h"
#include "FractionalStep.h"
#include "FractionalStepGCIB.h"
#include "FractionalStepELIB.h"
#include "FractionalStepDFIB.h"
#include "FractionalStepDFIBImplicit.h"
#include "FractionalStepDFIBMultiphase.h"
#include "FractionalStepMultiphase.h"
#include "FractionalStepBoussinesq.h"

std::shared_ptr<Solver> SolverFactory::create(SolverType type,
                                              const Input &input,
                                              const std::shared_ptr<const FiniteVolumeGrid2D> &grid)
{
    switch (type)
    {
    case POISSON:
        return std::make_shared<Poisson>(input, grid);
    case FRACTIONAL_STEP:
        return std::make_shared<FractionalStep>(input, grid);
    case FRACTIONAL_STEP_GHOST_CELL_IB:
        return std::make_shared<FractionalStepGCIB>(input, grid);
    case FRACTIONAL_STEP_MULTIPHASE:
        return std::make_shared<FractionalStepMultiphase>(input, grid);
    case FRACTIONAL_STEP_EULER_LAGRANGE_IB:
        return std::make_shared<FractionalStepELIB>(input, grid);
    case FRACTIONAL_STEP_DIRECT_FORCING_IB:
        return std::make_shared<FractionalStepDFIB>(input, grid);
    case FRACTIONAL_STEP_DIRECT_FORCING_IB_IMPLICIT:
        return std::make_shared<FractionalStepDFIBImplicit>(input, grid);
    case FRACTIONAL_STEP_DIRECT_FORCING_IB_MULTIPHASE:
        return std::make_shared<FractionalStepDirectForcingMultiphase>(input, grid);
    case FRACTIONAL_STEP_BOUSSINESQ:
        return std::make_shared<FractionalStepBoussinesq>(input, grid);
    default:
        return nullptr;
    }
}

std::shared_ptr<Solver> SolverFactory::create(std::string type,
                                              const Input &input,
                                              const std::shared_ptr<const FiniteVolumeGrid2D> &grid)
{
    std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c)
    {
        return std::tolower(c);
    });

    if (type == "poisson")
        return create(POISSON, input, grid);
    else if (type == "fractional step")
        return create(FRACTIONAL_STEP, input, grid);
    else if (type == "fractional step multiphase")
        return create(FRACTIONAL_STEP_MULTIPHASE, input, grid);
    else if (type == "fractional step ghost-cell")
        return create(FRACTIONAL_STEP_GHOST_CELL_IB, input, grid);
    else if (type == "fractional step euler-lagrange")
        return create(FRACTIONAL_STEP_EULER_LAGRANGE_IB, input, grid);
    else if (type == "fractional step direct-forcing")
        return create(FRACTIONAL_STEP_DIRECT_FORCING_IB, input, grid);
    else if (type == "fractional step direct-forcing implicit")
        return create(FRACTIONAL_STEP_DIRECT_FORCING_IB_IMPLICIT, input, grid);
    else if (type == "fractional step direct-forcing multiphase")
        return create(FRACTIONAL_STEP_DIRECT_FORCING_IB_MULTIPHASE, input, grid);
    else if (type == "fractional step multiphase")
        return create(FRACTIONAL_STEP_MULTIPHASE, input, grid);
    else if (type == "fractional step boussinesq")
        return create(FRACTIONAL_STEP_BOUSSINESQ, input, grid);

    throw Exception("SolverFactory", "create", "solver \"" + type + "\" is not a valid solver type.");
}

std::shared_ptr<Solver> SolverFactory::create(const Input &input,
                                              const std::shared_ptr<const FiniteVolumeGrid2D> &grid)
{
    return create(input.caseInput().get<std::string>("Solver.type"), input, grid);
}
