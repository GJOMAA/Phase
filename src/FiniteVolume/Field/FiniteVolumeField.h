#ifndef FINITE_VOLUME_FIELD
#define FINITE_VOLUME_FIELD

#include <deque>

#include "Field.h"
#include "FiniteVolumeGrid2D.h"
#include "Input.h"
#include "Vector.h"

template<class T>
class FiniteVolumeField : public Field<T>
{
public:
    enum BoundaryType
    {
        FIXED, NORMAL_GRADIENT, SYMMETRY, OUTFLOW
    };

    //- Constructors
    FiniteVolumeField(const FiniteVolumeGrid2D &grid, const std::string &name);

    FiniteVolumeField(const Input &input, const FiniteVolumeGrid2D &grid, const std::string &name);

    FiniteVolumeField(const FiniteVolumeField &other);

    FiniteVolumeField(const FiniteVolumeGrid2D &grid, const std::string &name, const T& val);

    //- Initialization
    void fill(const T &val);

    void fillInterior(const T &val);

    //- Boundaries
    void copyBoundaryTypes(const FiniteVolumeField &other);

    BoundaryType boundaryType(const Patch &patch) const;

    BoundaryType boundaryType(const Face& face) const;

    T boundaryRefValue(const Patch &patch) const;

    std::pair<BoundaryType, T> boundaryInfo(const Face &face) const;

    void interpolateFaces(const std::function<Scalar(const Face& face)>& alpha);

    void setBoundaryFaces();

    //- Face-centered values
    const std::vector<T> &faces() const
    { return faces_; }

    std::vector<T> &faces()
    { return faces_; }

    //- Node-centered values
    void initNodes()
    { nodes_.resize(grid.nNodes()); }

    std::vector<T> &nodes()
    { return nodes_; }

    const std::vector<T> &nodes() const
    { return nodes_; }

    bool hasNodalValues() const
    { return !nodes_.empty(); }

    //- Access operators
    T &operator()(const Cell &cell)
    { return std::vector<T>::operator[](cell.id()); }

    const T &operator()(const Cell &cell) const
    { return std::vector<T>::operator[](cell.id()); }

    T &operator()(const Face &face)
    { return faces_[face.id()]; }

    const T &operator()(const Face &face) const
    { return faces_[face.id()]; }

    T &operator()(const Node &node)
    { return nodes_[node.id()]; }

    const T &operator()(const Node &node) const
    { return nodes_[node.id()]; }

    //- Field history
    FiniteVolumeField &savePreviousTimeStep(Scalar timeStep, int nPreviousFields);

    FiniteVolumeField &savePreviousIteration();

    FiniteVolumeField &oldField(int i)
    { return previousTimeSteps_[i]->second; }

    const FiniteVolumeField &oldField(int i) const
    { return previousTimeSteps_[i]->second; }

    Scalar oldTimeStep(int i)
    { return previousTimeSteps_[i]->first; }

    FiniteVolumeField &prevIteration()
    { return *previousIteration_.front(); }

    const FiniteVolumeField &prevIteration() const
    { return *previousIteration_.front(); }

    //- Vectorization
    Vector vectorize() const;

    //- Operators
    FiniteVolumeField &operator=(const FiniteVolumeField &rhs);

    FiniteVolumeField &operator=(const Vector &rhs);

    FiniteVolumeField &operator+=(const FiniteVolumeField &rhs);

    FiniteVolumeField &operator-=(const FiniteVolumeField &rhs);

    FiniteVolumeField &operator*=(const FiniteVolumeField<Scalar> &rhs);

    FiniteVolumeField &operator*=(Scalar rhs);

    FiniteVolumeField &operator/=(Scalar lhs);

    FiniteVolumeField &operator/=(const FiniteVolumeField<Scalar> &rhs);

    const FiniteVolumeGrid2D &grid;

protected:

    typedef std::pair<Scalar, FiniteVolumeField<T>> PreviousField;

    void setBoundaryTypes(const Input &input);

    void setBoundaryRefValues(const Input &input);

    std::map<Label, std::pair<BoundaryType, T> > patchBoundaries_;

    std::vector<T> faces_, nodes_;

    std::vector<std::shared_ptr<PreviousField>> previousTimeSteps_;
    std::vector<std::shared_ptr<FiniteVolumeField<T>>> previousIteration_;
};

template<class T>
void interpolateNodes(FiniteVolumeField<T> &field);

template<class T>
FiniteVolumeField<T>
smooth(const Field<T> &field, const std::vector<std::vector<Ref<const Cell> > > &rangeSearch, Scalar epsilon);

#include "FiniteVolumeField.tpp"

#endif
