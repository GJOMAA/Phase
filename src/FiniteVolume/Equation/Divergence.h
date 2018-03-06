#ifndef DIVERGENCE_H
#define DIVERGENCE_H

#include "Equation.h"
#include "JacobianField.h"

namespace fv
{
    template<typename T>
    Equation<T> div(const VectorFiniteVolumeField &u,
                    FiniteVolumeField<T> &phi,
                    const CellGroup &cells,
                    Scalar theta = 1.)
    {
        Equation<T> eqn(phi);

        const VectorFiniteVolumeField &u0 = u.oldField(0);

        for (const Cell &cell: cells)
        {
            for (const InteriorLink &nb: cell.neighbours())
            {
                Scalar flux = dot(u(nb.face()), nb.outwardNorm());
                Scalar flux0 = dot(u0(nb.face()), nb.outwardNorm());

                eqn.add(cell, cell, theta * std::max(flux, 0.));
                eqn.add(cell, nb.cell(), theta * std::min(flux, 0.));
                eqn.addSource(cell, (1. - theta) * std::max(flux0, 0.) * phi(cell));
                eqn.addSource(cell, (1. - theta) * std::min(flux0, 0.) * phi(nb.cell()));
            }

            for (const BoundaryLink &bd: cell.boundaries())
            {
                Scalar flux = dot(u(bd.face()), bd.outwardNorm());
                Scalar flux0 = dot(u0(bd.face()), bd.outwardNorm());

                switch (phi.boundaryType(bd.face()))
                {
                    case FiniteVolumeField<T>::FIXED:
                        eqn.addSource(cell, theta * flux * phi(bd.face()));
                        eqn.addSource(cell, (1. - theta) * flux0 * phi(bd.face()));
                        break;

                    case FiniteVolumeField<T>::NORMAL_GRADIENT:
                        eqn.add(cell, cell, theta * flux);
                        eqn.add(cell, cell, (1. - theta) * flux);
                        break;

                    case FiniteVolumeField<T>::SYMMETRY:break;

                    default:throw Exception("fv", "div<T>", "unrecognized or unspecified boundary type.");
                }
            }
        }

        return eqn;
    }

    template<class T>
    Equation<T> divc(const VectorFiniteVolumeField &u,
                     FiniteVolumeField<T> &phi,
                     const CellGroup &cells,
                     Scalar theta = 1.)
    {
        Equation<T> eqn(phi);
        const VectorFiniteVolumeField &u0 = u.oldField(0);
        const FiniteVolumeField<T> &phi0 = phi.oldField(0);

        for (const Cell &cell: cells)
        {
            for (const InteriorLink &nb: cell.neighbours())
            {
                Scalar flux = theta * dot(u(nb.face()), nb.outwardNorm());
                Scalar flux0 = (1. - theta) * dot(u0(nb.face()), nb.outwardNorm());

                Scalar g = nb.cell().volume() / (cell.volume() + nb.cell().volume());

                eqn.add(cell, cell, g * flux);
                eqn.add(cell, nb.cell(), (1. - g) * flux);
                eqn.addSource(cell, flux0 * (g * phi0(cell) + (1. - g) * phi0(nb.cell())));
            }

            for (const BoundaryLink &bd: cell.boundaries())
            {
                Scalar flux = theta * dot(u(bd.face()), bd.outwardNorm());
                Scalar flux0 = (1. - theta) * dot(u(bd.face()), bd.outwardNorm());

                switch (phi.boundaryType(bd.face()))
                {
                    case FiniteVolumeField<T>::FIXED:
                        eqn.addSource(cell, flux * phi(bd.face()));
                        eqn.addSource(cell, flux0 * phi0(bd.face()));
                        break;

                    case FiniteVolumeField<T>::NORMAL_GRADIENT:
                        eqn.add(cell, cell, flux);
                        eqn.addSource(cell, flux0 * phi0(cell));
                        break;

                    case FiniteVolumeField<T>::SYMMETRY:
                        break;

                    default:throw Exception("fv", "divc<T>", "unrecognized or unspecified boundary type.");
                }
            }
        }

        return eqn;
    }

    template<class T>
    Equation<T> div(const VectorFiniteVolumeField &u,
                    FiniteVolumeField<T> &phi,
                    Scalar theta = 1.)
    {
        return div(u, phi, u.grid()->cellZone("fluid"), theta);
    }

    template<class T>
    Equation<T> divc(const VectorFiniteVolumeField &u,
                     FiniteVolumeField<T> &phi,
                     Scalar theta = 1.)
    {
        return divc(u, phi, u.grid()->cellZone("fluid"), theta);
    }


    Equation<Vector2D> div(const VectorFiniteVolumeField &phiU,
                           const JacobianField &gradU,
                           VectorFiniteVolumeField &u);
}

#endif
