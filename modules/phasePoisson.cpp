#include <iostream>

#include "Input.h"
#include "CommandLine.h"
#include "ConstructGrid.h"
#include "Viewer.h"
#include "Poisson.h"

int main(int argc, char* argv[])
{
    using namespace std;

    Communicator::init(argc, argv);

    Input input;
    Communicator comm;
    CommandLine(argc, argv);

    input.parseInputFile();

    shared_ptr<FiniteVolumeGrid2D> gridPtr(constructGrid(input));
    gridPtr->partition(comm);


    Poisson solver(input, comm, *gridPtr);
    Viewer viewer(solver, input);

//    solver.solve(0.);
//    viewer.write(0.);

    if(comm.isMainProc())
        viewer.write(0);

    Communicator::finalize();

    return 0;
}
