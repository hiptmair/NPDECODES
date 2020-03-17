/**
 * @ file avgvalboundary_main.cc
 * @ brief NPDE homework AvgValBoundary code
 * @ author Simon Meierhans
 * @ date 11.03.2019
 * @ copyright Developed at ETH Zurich
 */

#include <iostream>
#include <memory>

#include <Eigen/Core>

#include <lf/assemble/assemble.h>
#include <lf/io/io.h>
#include <lf/mesh/hybrid2d/hybrid2d.h>
#include <lf/mesh/utils/utils.h>
#include <lf/uscalfe/uscalfe.h>

#include "avgvalboundary.h"

/* SAM_LISTING_BEGIN_1 */
int main() {
  // read in mesh and set up finite element space
  auto mesh_factory = std::make_unique<lf::mesh::hybrid2d::MeshFactory>(2);
  lf::io::GmshReader reader(std::move(mesh_factory),
                            CURRENT_SOURCE_DIR "/../meshes/square.msh");
  auto mesh = reader.mesh();
  // obtain dofh for lagrangian finite element space
  auto fe_space =
      std::make_shared<lf::uscalfe::FeSpaceLagrangeO1<double>>(mesh);
  const lf::assemble::DofHandler &dofh{fe_space->LocGlobMap()};
  // Solve test problem
  Eigen::VectorXd mu = AvgValBoundary::solveTestProblem(dofh);
  // compute H1 seminorm of the solution
  double h1s_norm = AvgValBoundary::compH1seminorm(dofh, mu);
  // compute boundary functional
  lf::mesh::utils::MeshFunctionConstant mf_identity{1.0};
  double boundary_functional =
      AvgValBoundary::compBoundaryFunctional(dofh, mu, mf_identity);

  std::cout << "H1s-norm: " << h1s_norm << "\n";
  std::cout << "F: " << boundary_functional << "\n";

  //====================
  // Your code goes here
  //====================
  return 0;
}
/* SAM_LISTING_END_1 */
