#ifndef AVGVALBOUNDARY_H_
#define AVGVALBOUNDARY_H_

/**
 * @ file avgvalboundary.h
 * @ brief NPDE homework AvgValBoundary code
 * @ author Simon Meierhans, edited by Oliver Rietmann
 * @ date 11.03.2019
 * @ copyright Developed at ETH Zurich
 */

#include <memory>
#include <utility>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include <lf/assemble/assemble.h>
#include <lf/base/base.h>
#include <lf/mesh/utils/utils.h>
#include <lf/uscalfe/uscalfe.h>

namespace AvgValBoundary {

/**
 * @brief Assembly of general Galerkin matrix for 2nd-order elliptic
 *        boundary value problem
 *
 * @tparam FUNC_ALPHA functor type for diffusion coefficient
 * @tparam FUNC_GAMMA functor type for reaction coefficient
 * @tparam FUNC_BETA functor type for impedance coefficient
 *
 * @param dofh DofHandler object providing mesh a local-to-global
 *        index mapping for global shape functions.
 *
 * This function computes the finite element Galerkin matrix for the
 * lowest-order Lagrangian FEM and the bilinear form on \f$H^1(\Omega)\f$
 * \f[
     (u,v) \mapsto \int\limits_{\Omega} \alpha(\mathbf{x})
        \mathbf{grad}\,u\cdot\mathfb{grad}\,v +
        \gamma(\mathbf{x})\,u\,v\,\mathrm{d}\mathbf{x} +
        \int\limits_{\Omega}\beta(\mathbf{x})\,u\,v\,\mathrm{d}S(\mathbf{x})
    \f]
 */
/* SAM_LISTING_BEGIN_1 */
template <typename FUNC_ALPHA, typename FUNC_GAMMA, typename FUNC_BETA>
Eigen::SparseMatrix<double>
compGalerkinMatrix(const lf::assemble::DofHandler &dofh, FUNC_ALPHA &&alpha,
                   FUNC_GAMMA &&gamma, FUNC_BETA &&beta) {
  // obtain mesh and set up fe_space (p.w. linear Lagrangian FEM)
  auto mesh = dofh.Mesh();
  auto fe_space =
      std::make_shared<lf::uscalfe::FeSpaceLagrangeO1<double>>(mesh);

  // get the number of degrees of freedom = dimension of FE space
  const lf::base::size_type N_dofs(dofh.NumDofs());
  // Set up an empty sparse matrix to hold the Galerkin matrix
  lf::assemble::COOMatrix<double> A(N_dofs, N_dofs);
  // Initialize ELEMENT_MATRIX_PROVIDER object
  lf::uscalfe::ReactionDiffusionElementMatrixProvider elmat_builder(
      fe_space, std::forward<FUNC_ALPHA>(alpha),
      std::forward<FUNC_GAMMA>(gamma));
  // Cell-oriented assembly over the whole computational domain
  lf::assemble::AssembleMatrixLocally(0, dofh, dofh, elmat_builder, A);

  // Add contributions of boundary term in the bilinear form using
  // a LehrFEM++ built-in high-level ENTITY_MATRIX_PROVIDER class
  auto bd_flags{lf::mesh::utils::flagEntitiesOnBoundary(mesh, 1)};
  lf::uscalfe::MassEdgeMatrixProvider edgemat_builder(
      fe_space, std::forward<FUNC_BETA>(beta), bd_flags);
  lf::assemble::AssembleMatrixLocally(1, dofh, dofh, edgemat_builder, A);
  Eigen::SparseMatrix<double> A_crs = A.makeSparse();
  return A_crs;
}

/* SAM_LISTING_END_1 */

double compH1seminorm(const lf::assemble::DofHandler &dofh,
                      const Eigen::VectorXd &u);

/**
 * @brief computes boundary functional as in exercise c)
 * @param dofh DofHandler of FEspace.
 *        u coefficient vector
 */
/* SAM_LISTING_BEGIN_2 */
template <typename FUNCTION>
double compBoundaryFunctional(const lf::assemble::DofHandler &dofh,
                              const Eigen::VectorXd &u, FUNCTION &&w) {
  double result = 0.0;
#if SOLUTION
  // constant zero mesh function
  lf::mesh::utils::MeshFunctionConstant mf_zero{0.0};
  auto A =
      compGalerkinMatrix(dofh, mf_zero, mf_zero, std::forward<FUNCTION>(w));
  const lf::base::size_type N_dofs(dofh.NumDofs());
  Eigen::VectorXd ones = Eigen::VectorXd::Ones(N_dofs);
  result = ones.transpose() * A * u;
  result = std::sqrt(result);
#else
  //====================
  // Your code goes here
  //====================
#endif
  return result;
}

/* SAM_LISTING_END_2 */

Eigen::VectorXd solveTestProblem(const lf::assemble::DofHandler &dofh);

std::vector<std::pair<unsigned int, double>>
approxBoundaryFunctionalValues(unsigned int L);

} // namespace AvgValBoundary

#endif // AVGVALBOUNDARY_H_
