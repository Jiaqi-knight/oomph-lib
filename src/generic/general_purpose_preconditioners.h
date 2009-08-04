//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
//Include guards
#ifndef OOMPH_GENERAL_PRECONDITION_HEADER
#define OOMPH_GENERAL_PRECONDITION_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "preconditioner.h"
#include "iterative_linear_solver.h"
#include "matrices.h"
#include "problem.h"
#include <algorithm>



namespace oomph
{



//=====================================================================
/// Matrix-based diagonal preconditioner
//=====================================================================
class MatrixBasedDiagPreconditioner : public Preconditioner
{
 public:
 
 ///Constructor (empty)
 MatrixBasedDiagPreconditioner(){};
 
 ///Destructor (empty)
 ~MatrixBasedDiagPreconditioner(){};
 
 /// Broken copy constructor
 MatrixBasedDiagPreconditioner(const MatrixBasedDiagPreconditioner&) 
  { 
   BrokenCopy::broken_copy("MatrixBasedDiagPreconditioner");
  } 
 
 /// Broken assignment operator
 void operator=(const MatrixBasedDiagPreconditioner&) 
  {
   BrokenCopy::broken_assign("MatrixBasedDiagPreconditioner");
  }

  /// Apply preconditioner to z, i.e. z=D^-1 
  void preconditioner_solve(const DoubleVector&r, DoubleVector &z);

  /// \short Setup the preconditioner (store diagonal) from the fully
  /// assembled matrix.
  void setup(Problem* problem_pt, DoubleMatrixBase* matrix_pt);

 private:

  /// Vector of inverse diagonal entries
  Vector<double> Inv_diag;
};

//=============================================================================
/// Matrix-based lumped preconditioner
//=============================================================================
template<typename MATRIX>
class MatrixBasedLumpedPreconditioner : public Preconditioner
{
 public:
 
 ///Constructor
 MatrixBasedLumpedPreconditioner()
  {
   // default the positive matrix boolean to false
   Positive_matrix = false;

   // set the pointers to the lumped vector to 0
   Inv_lumped_diag_pt = 0;
  };
 
 ///Destructor 
 ~MatrixBasedLumpedPreconditioner()
  {
   this->clean_up_memory();
  }
 
 /// Broken copy constructor
 MatrixBasedLumpedPreconditioner(const MatrixBasedDiagPreconditioner&) 
  { 
   BrokenCopy::broken_copy("MatrixBasedDiagPreconditioner");
  } 
 
 /// Broken assignment operator
 void operator=(const MatrixBasedLumpedPreconditioner&) 
  {
   BrokenCopy::broken_assign("MatrixBasedDiagPreconditioner");
  }

  /// Apply preconditioner to z, i.e. z=D^-1 
  void preconditioner_solve(const DoubleVector &r, DoubleVector &z);

  /// \short Setup the preconditioner (store diagonal) from the fully
  /// assembled matrix. Problem pointer is ignored.
  void setup(Problem* problem_pt, DoubleMatrixBase* matrix_pt);

  /// \short Access function to the Positive_matrix which indicates whether 
  /// lumped matrix was positive
  bool& positive_matrix()
   {
    /// paranoid check that preconditioner has been setup
#ifdef PARANOID
   if (Inv_lumped_diag_pt == 0) 
   {
    throw OomphLibError(
     "The preconditioner has not been setup.",
     "MatrixBasedDiagPreconditioner::positive_matrix()",
     OOMPH_EXCEPTION_LOCATION);
   }
#endif
    return Positive_matrix;
   }


  /// \short Access function to the inverse of the lumped vector assembled in
  /// the preconditioner setup routine
  double* inverse_lumped_vector_pt()
   {
    /// paranoid check that vector has been created
#ifdef PARANOID
   if (Inv_lumped_diag_pt == 0) 
   {
    throw OomphLibError(
     "The inverse lumped vector has not been created. Created in setup(...)",
     "MatrixBasedLumpedPreconditioner::inverse_lumped_vector_pt()",
     OOMPH_EXCEPTION_LOCATION);
   }
#endif
    return Inv_lumped_diag_pt;
   }


  /// \short Access function to number of rows for this preconditioner
  unsigned& nrow() { return Nrow; }

  /// clean up memory - just delete the inverse lumped vector
  void clean_up_memory()
   {
    delete[] Inv_lumped_diag_pt;
   }
 private:


  /// Vector of inverse diagonal entries
  double* Inv_lumped_diag_pt;

  // boolean to indicate whether the lumped matrix was positive
  bool Positive_matrix;

  // number of rows in preconditioner
  unsigned Nrow;
};



//=============================================================================
/// \short Class for a compressed-matrix coefficent (for either CC or CR
/// matrices). Contains the (row or column) index and value of a 
/// coefficient in a compressed row or column.
/// Currently only used in ILU(0) for CCDoubleMatrices to allow the 
/// coefficients in each compressed column [row] to be sorted by 
/// their row [column] index.
//=============================================================================
class CompressedMatrixCoefficient
{

  public:

 /// Constructor (no arguments)
 CompressedMatrixCoefficient(){}

 /// Constructor (takes the index and value as arguments)
 CompressedMatrixCoefficient(const unsigned& index, const double& value)
  {
   // store the index and value
   Index = index;
   Value = value;
  }

 /// Destructor (does nothing)
 ~CompressedMatrixCoefficient(){}

  /// Copy Constructor. Not Broken. Required for STL sort function. 
 CompressedMatrixCoefficient(const CompressedMatrixCoefficient& a) 
  { 
   Index = a.index();
   Value = a.value();   
  } 
 
 /// Assignment Operator. Not Broken. Required for STL sort function.
 void operator=(const CompressedMatrixCoefficient& a) 
  { 
   Index = a.index();
   Value = a.value();
  } 

 /// Less Than Operator (for the STL sort function)
 bool operator<(const CompressedMatrixCoefficient& a) const
  {
   return Index < a.index();
  }

 /// access function for the coefficient's (row or column) index
 unsigned& index() { return Index; }

 /// access function for the coefficient value
 double& value() { return Value; }

 /// \short Access function for the coefficient's (row or column_ index 
 /// (const version)
 unsigned index() const { return Index; }

 /// access function for the coefficient's value (const version)
 double value() const { return Value; }

  private:

 /// the row or column index of the compressed-matrix coefficient
 unsigned Index;

 /// the value of the compressed-matrix coefficient
 double Value;

};





//=============================================================================
/// ILU(0) Preconditioner
//=============================================================================
template <typename MATRIX> 
class ILUZeroPreconditioner : public Preconditioner
{
};


//=============================================================================
/// ILU(0) Preconditioner for matrices of CCDoubleMatrix Format
//=============================================================================
template <> 
class ILUZeroPreconditioner<CCDoubleMatrix> : public Preconditioner
{
 
 public :
  
  ///Constructor (empty)
  ILUZeroPreconditioner(){};
 
 ///Destructor (empty)
 ~ILUZeroPreconditioner(){};
 

 /// Broken copy constructor
 ILUZeroPreconditioner(const ILUZeroPreconditioner&) 
  { 
   BrokenCopy::broken_copy("ILUZeroPreconditioner");
  } 
 
 /// Broken assignment operator
 void operator=(const ILUZeroPreconditioner&) 
  {
   BrokenCopy::broken_assign("ILUZeroPreconditioner");
  }

 
 /// Apply preconditioner to r 
 void preconditioner_solve(const DoubleVector&r, DoubleVector &z);

 /// \short Setup the preconditioner (store diagonal) from the fully
 /// assembled matrix. Problem pointer is ignored.
 void setup(Problem* problem_pt, DoubleMatrixBase* matrix_pt);
 
  private:

 /// Column start for upper triangular matrix 
 Vector<unsigned> U_column_start;

 /// \short Row entry for the upper triangular matrix (each element of the 
 /// vector contains the row index and coefficient) 
 Vector<CompressedMatrixCoefficient> U_row_entry;

 /// Column start for lower triangular matrix 
 Vector<unsigned> L_column_start;

 /// \short Row entry for the lower triangular matrix (each element of the 
 /// vector contains the row index and coefficient)
 Vector<CompressedMatrixCoefficient> L_row_entry;

};


//=============================================================================
/// ILU(0) Preconditioner for matrices of CRDoubleMatrix Format
//=============================================================================
template <> 
class ILUZeroPreconditioner<CRDoubleMatrix> : public Preconditioner
{
 
 public :
  
  ///Constructor (empty)
  ILUZeroPreconditioner(){};
 

 /// Broken copy constructor
 ILUZeroPreconditioner(const ILUZeroPreconditioner&) 
  { 
   BrokenCopy::broken_copy("ILUZeroPreconditioner");
  } 
 
 /// Broken assignment operator
 void operator=(const ILUZeroPreconditioner&) 
  {
   BrokenCopy::broken_assign("ILUZeroPreconditioner");
  }
 
 ///Destructor (empty)
 ~ILUZeroPreconditioner(){};
 
 /// Apply preconditioner to r
 void preconditioner_solve(const DoubleVector&r, DoubleVector &z);
 
 /// \short Setup the preconditioner (store diagonal) from the fully
 /// assembled matrix. Problem pointer is ignored.
 void setup(Problem* problem_pt, DoubleMatrixBase* matrix_pt);
 
 
  private:


 /// Row start for upper triangular matrix 
 Vector<unsigned> U_row_start;

 /// \short column entry for the upper triangular matrix (each element of the 
 /// vector contains the column index and coefficient) 
 Vector<CompressedMatrixCoefficient> U_row_entry;
 
 /// Row start for lower triangular matrix 
 Vector<unsigned> L_row_start;

 /// \short column entry for the lower triangular matrix (each element of the 
 /// vector contains the column index and coefficient) 
 Vector<CompressedMatrixCoefficient> L_row_entry;
};

//=============================================================================
/// \short A preconditioner for performing inner iteration preconditioner 
/// solves. The template argument SOLVER specifies the inner iteration
/// solver (which must be derived from IterativeLinearSolver) and the
/// template argument PRECONDITIONER specifies the preconditioner for the 
/// inner iteration iterative solver.\n
/// Note: For no preconditioning use the IdentityPreconditioner.
//=============================================================================
template <class SOLVER, class PRECONDITIONER> 
class InnerIterationPreconditioner : public Preconditioner
{
  public:

 /// Constructor
 InnerIterationPreconditioner()
  {
   // create the solver
   Solver_pt = new SOLVER;

   // create the preconditioner
   Preconditioner_pt = new PRECONDITIONER;

#ifdef PARANOID
   // paranoid check that the solver is an iterative solver and 
   // the preconditioner is a preconditioner
   if (dynamic_cast<IterativeLinearSolver* >(Solver_pt) == 0)
    {
     throw OomphLibError(
      "The template argument SOLVER must be of type IterativeLinearSolver",
      "InnerIterationPreconditioner::InnerIterationPreconditioner()",
      OOMPH_EXCEPTION_LOCATION);     
    }
   if (dynamic_cast<Preconditioner*>(Preconditioner_pt) == 0)
    {
     throw OomphLibError(
      "The template argument PRECONDITIONER must be of type Preconditioner",
      "InnerIterationPreconditioner::InnerIterationPreconditioner()",
      OOMPH_EXCEPTION_LOCATION);     
    }
#endif

   // ensure the solver does not re-setup the preconditioner
   Solver_pt->setup_preconditioner_before_solve() = false;

   // pass the preconditioner to the solver
   Solver_pt->preconditioner_pt() = Preconditioner_pt;
  }

 // destructor
 ~InnerIterationPreconditioner()
  {
   delete Solver_pt;
   delete Preconditioner_pt;
  }

 // clean the memory
 void clean_up_memory()
  {
   Preconditioner_pt->clean_up_memory();
   Solver_pt->clean_up_memory();
  }

 /// \short Preconditioner setup method. Setup the preconditioner for the inner
 /// iteration solver.
 void setup(Problem* problem_pt, DoubleMatrixBase* matrix_pt)
  {
   
   // set the distribution
   DistributableLinearAlgebraObject* dist_pt = 
    dynamic_cast<DistributableLinearAlgebraObject*>
    (matrix_pt);
   if (dist_pt != 0)
    {
     Distribution_pt->rebuild(dist_pt->distribution_pt());
    }
   else
    {
     Distribution_pt->rebuild(problem_pt->communicator_pt(),
                              matrix_pt->nrow(),false);
    }

   // setup the inner iteration preconditioner
   Preconditioner_pt->setup(problem_pt,matrix_pt);

   // setup the solverready for resolve
   unsigned max_iter = Solver_pt->max_iter();
   Solver_pt->max_iter() = 1;
   DoubleVector x(Distribution_pt,0.0);
   DoubleVector y(x);
   Solver_pt->enable_resolve();
   Solver_pt->solve(matrix_pt,x,y);
   Solver_pt->max_iter() = max_iter;
  }
 
 /// \short Preconditioner solve method. Performs the specified number
 /// of Krylov iterations preconditioned with the specified preconditioner
 void preconditioner_solve(const DoubleVector &r, DoubleVector &z)
  {
   Solver_pt->resolve(r,z);
  }

 /// Access to convergence tolerance of the inner iteration solver
 double& tolerance() {return Solver_pt->tolerance();}
 
 /// Access to max. number of iterations of the inner iteration solver
 unsigned& max_iter() {return Solver_pt->max_iter();} 

 ///
 SOLVER* solver_pt() { return Solver_pt; }

 /// 
 PRECONDITIONER* preconditioner_pt() { return Preconditioner_pt; }

  private:

 /// pointer to the underlying solver
 SOLVER* Solver_pt;

 /// pointer to the underlying preconditioner
 PRECONDITIONER* Preconditioner_pt;
};
}
#endif
