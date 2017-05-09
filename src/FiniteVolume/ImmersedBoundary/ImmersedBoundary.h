#ifndef IMMERSED_BOUNDARY_H
#define IMMERSED_BOUNDARY_H

#include "ImmersedBoundaryObject.h"
#include "CutCell.h"

class Solver;

class ImmersedBoundary
{
public:

    enum
    {
        FLUID = 1, IB = 2, SOLID = 3, FRESHLY_CLEARED = 4, BUFFER=5
    };

    enum StencilType{GHOST_CELL_BILINEAR, FORCING_POINT_LINEAR};

    ImmersedBoundary(const Input &input, const Communicator &comm, Solver &solver);

    void initCellZones();

    void update(Scalar timeStep);

    template<class T>
    void copyBoundaryTypes(const FiniteVolumeField<T>& srcField, const FiniteVolumeField<T>& destField)
    {
        for(auto& ibObj: ibObjs_)
            ibObj->addBoundaryType(destField.name(), ibObj->boundaryType(srcField.name()));
    }

    template<class T>
    Equation<T> bcs(FiniteVolumeField<T>& field) const
    {
        Equation<T> eqn(field);

        for(const auto& ibObj: ibObjs_)
            eqn += ibObj->bcs(field);

        return eqn;
    }

    std::vector<CutCell> constructCutCells(const CellGroup& cellGroup) const;

    std::vector<Ref<const ImmersedBoundaryObject>> ibObjs() const;

    bool isIbCell(const Cell &cell) const;

    void computeForce(const ScalarFiniteVolumeField& rho, const ScalarFiniteVolumeField& mu, const VectorFiniteVolumeField& u, const ScalarFiniteVolumeField& p);

protected:

    void setCellStatus();

    Solver &solver_;
    const Communicator &comm_;
    FiniteVolumeField<int> &cellStatus_;
    std::vector<std::shared_ptr<ImmersedBoundaryObject>> ibObjs_;

};

#endif
