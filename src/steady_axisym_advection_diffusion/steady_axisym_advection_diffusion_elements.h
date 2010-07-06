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
//Header file for Steady axisymmetric advection diffusion elements
#ifndef OOMPH_STEADY_AXISYM_ADV_DIFF_ELEMENTS_HEADER
#define OOMPH_STEADY_AXISYM_ADV_DIFF_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "../generic/nodes.h"
#include "../generic/Qelements.h"
#include "../generic/oomph_utilities.h"

namespace oomph
{

//=============================================================
/// \short A class for all elements that solve the Steady Axisymmetric 
/// Advection Diffusion equations using isoparametric elements.
/// \f[ 
/// Pe \mathbf{w}\cdot(\mathbf{x}) \nabla u = 
/// \nabla \cdot \left( \nabla u \right) + f(\mathbf{x})    
/// \f] 
/// This contains the generic maths. Shape functions, geometric
/// mapping etc. must get implemented in derived class.
//=============================================================
class SteadyAxisymAdvectionDiffusionEquations : public virtual FiniteElement
{

public:

 /// \short Function pointer to source function fct(x,f(x)) -- 
 /// x is a Vector! 
 typedef void (*SteadyAxisymAdvectionDiffusionSourceFctPt)(const Vector<double>& x,
                                                           double& f);


 /// \short Function pointer to wind function fct(x,w(x)) -- 
 /// x is a Vector! 
 typedef void (*SteadyAxisymAdvectionDiffusionWindFctPt)(const Vector<double>& x,
                                                         Vector<double>& wind);


 /// \short Constructor: Initialise the Source_fct_pt and Wind_fct_pt 
 /// to null and set (pointer to) Peclet number to default
 SteadyAxisymAdvectionDiffusionEquations() : Source_fct_pt(0), Wind_fct_pt(0)
  {
   //Set pointer to Peclet number to the default value zero
   Pe_pt = &Default_peclet_number;
  }
 
 /// Broken copy constructor
 SteadyAxisymAdvectionDiffusionEquations(const SteadyAxisymAdvectionDiffusionEquations& dummy) 
  { 
   BrokenCopy::broken_copy("SteadyAxisymAdvectionDiffusionEquations");
  } 
 
 /// Broken assignment operator
 void operator=(const SteadyAxisymAdvectionDiffusionEquations&) 
  {
   BrokenCopy::broken_assign("SteadyAxisymAdvectionDiffusionEquations");
  }

 /// \short Return the index at which the unknown value
 /// is stored. The default value, 0, is appropriate for single-physics
 /// problems, when there is only one variable, the value that satisfies
 /// the steady axisymmetric advection-diffusion equation. 
 /// In derived multi-physics elements, this function should be overloaded
 /// to reflect the chosen storage scheme. Note that these equations require
 /// that the unknown is always stored at the same index at each node.
 virtual inline unsigned u_index_axisym_adv_diff() const 
  {
   return 0;
  }

 /// Output with default number of plot points
 void output(std::ostream &outfile) 
  {
   unsigned nplot = 5;
   output(outfile,nplot);
  }

 /// \short Output FE representation of soln: r,z,u  at 
 /// nplot^2 plot points
 void output(std::ostream &outfile, const unsigned &nplot);


 /// C_style output with default number of plot points
 void output(FILE* file_pt)
  {
   unsigned n_plot = 5;
   output(file_pt,n_plot);
  }

 /// \short C-style output FE representation of soln: r,z,u  at 
 /// n_plot^2 plot points
 void output(FILE* file_pt, const unsigned &n_plot);


 /// Output exact soln: r,z,u_exact at nplot^2 plot points
 void output_fct(std::ostream &outfile, 
                 const unsigned &nplot, 
                 FiniteElement::SteadyExactSolutionFctPt 
                 exact_soln_pt);

 /// Get error against and norm of exact solution
 void compute_error(std::ostream &outfile, 
                    FiniteElement::SteadyExactSolutionFctPt 
                    exact_soln_pt, double& error, 
                    double& norm);

 /// Access function: Pointer to source function
 SteadyAxisymAdvectionDiffusionSourceFctPt& source_fct_pt() 
  {
   return Source_fct_pt;
  }


 /// Access function: Pointer to source function. Const version
 SteadyAxisymAdvectionDiffusionSourceFctPt source_fct_pt() const 
  {
   return Source_fct_pt;
  }


 /// Access function: Pointer to wind function
 SteadyAxisymAdvectionDiffusionWindFctPt& wind_fct_pt() 
  {
   return Wind_fct_pt;
  }


 /// Access function: Pointer to wind function. Const version
 SteadyAxisymAdvectionDiffusionWindFctPt wind_fct_pt() const 
  {
   return Wind_fct_pt;
  }

 // Access functions for the physical constants

 /// Peclet number
 const double &pe() const 
  {
   return *Pe_pt;
  }

 /// Pointer to Peclet number
 double* &pe_pt() 
  {
   return Pe_pt;
  }

 /// \short Get source term at (Eulerian) position x. This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the strength of the source function might be determined by
 /// another system of equations 
 inline virtual void get_source_axisym_adv_diff(const unsigned& ipt,
                                                const Vector<double>& x,
                                                double& source) const
  {
   //If no source function has been set, return zero
   if(Source_fct_pt==0) 
    {
     source = 0.0;
    }
   else
    {
     //Get source strength
     (*Source_fct_pt)(x,source);
    }
  }

 /// \short Get wind at (Eulerian) position x and/or local coordinate s. 
 /// This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the wind function might be determined by
 /// another system of equations 
 inline virtual void get_wind_axisym_adv_diff(const unsigned& ipt,
                                              const Vector<double> &s,
                                              const Vector<double>& x,
                                              Vector<double>& wind) const
  {
   //If no wind function has been set, return zero
   if(Wind_fct_pt==0)
    {
     for(unsigned i=0;i<2;i++) 
      { 
       wind[i] = 0.0;
      }
    }
   else
    {
     //Get wind
     (*Wind_fct_pt)(x,wind);
    }
  }

 /// Get flux: \f \mbox{flux}[i] = \nabla u = \mbox{d}u / \mbox{d}x_i \f
 void get_flux(const Vector<double>& s, Vector<double>& flux) const
  {
   //Find out how many nodes there are in the element
   unsigned n_node = nnode();
   
   //Get the nodal index at which the unknown is stored
   unsigned u_nodal_index = u_index_axisym_adv_diff();

   //Set up memory for the shape and test functions
   Shape psi(n_node);
   DShape dpsidx(n_node,2);
 
   //Call the derivatives of the shape and test functions
   dshape_eulerian(s,psi,dpsidx);
     
   //Initialise to zero
   for(unsigned j=0;j<2;j++) 
    {
     flux[j] = 0.0;
    }
   
   //Loop over nodes
   for(unsigned l=0;l<n_node;l++) 
    {
     //Loop over derivative directions
     for(unsigned j=0;j<2;j++)
      {                               
       flux[j] += nodal_value(l,u_nodal_index)*dpsidx(l,j);
      }
    }
  }

 
 /// Add the element's contribution to its residual vector (wrapper)
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Call the generic residuals function with flag set to 0 and using
   //a dummy matrix
   fill_in_generic_residual_contribution_adv_diff(residuals,
                                                  GeneralisedElement::Dummy_matrix,
                                                  GeneralisedElement::Dummy_matrix,
                                                  0);
  }

 
 /// \short Add the element's contribution to its residual vector and 
 /// the element Jacobian matrix (wrapper)
 void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                       DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_adv_diff(residuals,
                                                  jacobian,
                                                  GeneralisedElement::Dummy_matrix,
                                                  1);
  }
 

 /// Return FE representation of function value u(s) at local coordinate s
 inline double interpolated_u_adv_diff(const Vector<double> &s) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Get the nodal index at which the unknown is stored
   unsigned u_nodal_index = u_index_axisym_adv_diff();

   //Local shape function
   Shape psi(n_node);

   //Find values of shape function
   shape(s,psi);

   //Initialise value of u
   double interpolated_u = 0.0;

   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_node;l++) 
    {
     interpolated_u += nodal_value(l,u_nodal_index)*psi[l];
    }

   return(interpolated_u);
  }


 ///\short Return derivative of u at point s with respect to all data
 ///that can affect its value.
 ///In addition, return the global equation numbers corresponding to the
 ///data. This is virtual so that it can be overloaded in the
 ///refineable version
 virtual void dinterpolated_u_adv_diff_ddata(const Vector<double> &s, 
                                             Vector<double> &du_ddata,
                                             Vector<unsigned> &global_eqn_number)
  {
   //Find number of nodes
   const unsigned n_node = nnode();

   //Get the nodal index at which the unknown is stored
   const unsigned u_nodal_index = u_index_axisym_adv_diff();

   //Local shape function
   Shape psi(n_node);

   //Find values of shape function
   shape(s,psi);

   //Find the number of dofs associated with interpolated u
   unsigned n_u_dof = 0;
   for(unsigned l=0;l<n_node;l++) 
    {
     int global_eqn = this->node_pt(l)->eqn_number(u_nodal_index);
     //If it's positive add to the count
     if (global_eqn >=0) 
      {
       ++n_u_dof;
      }
    }
     
   //Now resize the storage schemes
   du_ddata.resize(n_u_dof,0.0); 
   global_eqn_number.resize(n_u_dof,0);
   
   //Loop over the nodes again and set the derivatives
   unsigned count = 0;
   for(unsigned l=0;l<n_node;l++)
    {
     //Get the global equation number
     int global_eqn=this->node_pt(l)->eqn_number(u_nodal_index);
     //If it's positive
     if (global_eqn >= 0)
      {
       //Set the global equation number
       global_eqn_number[count] = global_eqn;
       //Set the derivative with respect to the unknown
       du_ddata[count] = psi[l];
       //Increase the counter
       ++count;
      }
    }
  }


 /// \short Self-test: Return 0 for OK
 unsigned self_test();

protected:

 /// \short Shape/test functions and derivs w.r.t. to global coords at 
 /// local coord. s; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_adv_diff(const Vector<double> &s, 
                                                   Shape &psi, 
                                                   DShape &dpsidx, 
                                                   Shape &test, 
                                                   DShape &dtestdx) 
  const = 0;

 /// \short Shape/test functions and derivs w.r.t. to global coords at 
 /// integration point ipt; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_at_knot_adv_diff(const unsigned &ipt, 
                                                           Shape &psi, 
                                                           DShape &dpsidx,
                                                           Shape &test, 
                                                           DShape &dtestdx) 
  const = 0;

 /// \short Add the element's contribution to its residual vector only 
 /// (if flag=and/or element  Jacobian matrix 
 virtual void fill_in_generic_residual_contribution_adv_diff(Vector<double> &residuals, 
                                                             DenseMatrix<double> &jacobian, 
                                                             DenseMatrix<double> &mass_matrix, 
                                                             unsigned flag); 
  
 // Physical constants

 /// Pointer to global Peclet number
 double *Pe_pt;

 /// Pointer to source function:
 SteadyAxisymAdvectionDiffusionSourceFctPt Source_fct_pt;
 
 /// Pointer to wind function:
 SteadyAxisymAdvectionDiffusionWindFctPt Wind_fct_pt;

 private:

 /// Static default value for the Peclet number
 static double Default_peclet_number;
 
  
};//End class SteadyAxisymAdvectionDiffusionEquations


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



//======================================================================
/// \short QSteadyAxisymAdvectionDiffusionElement elements are 
/// linear/quadrilateral/brick-shaped Axisymmetric Advection Diffusion 
/// elements with isoparametric interpolation for the function.
//======================================================================
template <unsigned NNODE_1D>
class QSteadyAxisymAdvectionDiffusionElement : public virtual QElement<2,NNODE_1D>,
public virtual SteadyAxisymAdvectionDiffusionEquations
{

private:

 /// \short Static array of ints to hold number of variables at 
 /// nodes: Initial_Nvalue[n]
 static const unsigned Initial_Nvalue;
 
public:


 ///\short Constructor: Call constructors for QElement and 
 /// Advection Diffusion equations
 QSteadyAxisymAdvectionDiffusionElement() : QElement<2,NNODE_1D>(), 
 SteadyAxisymAdvectionDiffusionEquations()
  { }

 /// Broken copy constructor
 QSteadyAxisymAdvectionDiffusionElement(const QSteadyAxisymAdvectionDiffusionElement<NNODE_1D>& dummy) 
  { 
   BrokenCopy::broken_copy("QSteadyAxisymAdvectionDiffusionElement");
  } 
 
 /// Broken assignment operator
 void operator=(const QSteadyAxisymAdvectionDiffusionElement<NNODE_1D>&) 
  {
   BrokenCopy::broken_assign("QSteadyAxisymAdvectionDiffusionElement");
  }

 /// \short  Required  # of `values' (pinned or dofs) 
 /// at node n
 inline unsigned required_nvalue(const unsigned &n) const 
  {
   return Initial_Nvalue;
  }

 /// \short Output function:  
 ///  r,z,u
 void output(std::ostream &outfile)
  {
   SteadyAxisymAdvectionDiffusionEquations::output(outfile);
  }

 /// \short Output function:  
 ///  r,z,u  at n_plot^2 plot points
 void output(std::ostream &outfile, const unsigned &n_plot)
  {
   SteadyAxisymAdvectionDiffusionEquations::output(outfile,n_plot);
  }


 /// \short C-style output function:  
 ///  r,z,u
 void output(FILE* file_pt)
  {
   SteadyAxisymAdvectionDiffusionEquations::output(file_pt);
  }

 ///  \short C-style output function:  
 ///   r,z,u at n_plot^2 plot points
 void output(FILE* file_pt, const unsigned &n_plot)
  {
   SteadyAxisymAdvectionDiffusionEquations::output(file_pt,n_plot);
  }

 /// \short Output function for an exact solution:
 ///  r,z,u_exact at n_plot^2 plot points
 void output_fct(std::ostream &outfile, 
                 const unsigned &n_plot,
                 FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
  {
   SteadyAxisymAdvectionDiffusionEquations::output_fct(outfile,n_plot,exact_soln_pt);
  }


protected:

 /// Shape, test functions & derivs. w.r.t. to global coords. Return Jacobian.
 inline double dshape_and_dtest_eulerian_adv_diff(const Vector<double> &s, 
                                                  Shape &psi, 
                                                  DShape &dpsidx, 
                                                  Shape &test, 
                                                  DShape &dtestdx) const;
 
 /// \short Shape, test functions & derivs. w.r.t. to global coords. at
 /// integration point ipt. Return Jacobian.
 inline double dshape_and_dtest_eulerian_at_knot_adv_diff(const unsigned& ipt,
                                                          Shape &psi, 
                                                          DShape &dpsidx, 
                                                          Shape &test,
                                                          DShape &dtestdx) const;

};//End class QSteadyAxisymAdvectionDiffusionElement

//Inline functions:

//======================================================================
/// \short Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================

template<unsigned NNODE_1D>
double QSteadyAxisymAdvectionDiffusionElement<NNODE_1D>::
dshape_and_dtest_eulerian_adv_diff(const Vector<double> &s,
                                   Shape &psi, 
                                   DShape &dpsidx,
                                   Shape &test, 
                                   DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian(s,psi,dpsidx);

 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   test[i] = psi[i]; 
   for(unsigned j=0;j<2;j++)
    {
     dtestdx(i,j) = dpsidx(i,j);
    }
  }

 //Return the jacobian
 return J;
}



//======================================================================
/// \short Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================

template<unsigned NNODE_1D>
double QSteadyAxisymAdvectionDiffusionElement<NNODE_1D>::
dshape_and_dtest_eulerian_at_knot_adv_diff(const unsigned &ipt,
                                           Shape &psi, 
                                           DShape &dpsidx,
                                           Shape &test, 
                                           DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);

 //Set the test functions equal to the shape functions (pointer copy)
 test = psi;
 dtestdx = dpsidx;

 //Return the jacobian
 return J;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

template<unsigned NNODE_1D>
class FaceGeometry<QSteadyAxisymAdvectionDiffusionElement<NNODE_1D> >: 
public virtual  QElement<1,NNODE_1D>
{

  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : QElement<1,NNODE_1D>() {}

};




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//======================================================================
/// \short A class for elements that allow the imposition of an 
/// applied Robin boundary condition on the boundaries of Steady 
/// Axisymmnetric Advection Diffusion Flux elements.
/// \f[ 
/// -\Delta u \cdot \mathbk{n} + \alpha(r,z) u = \beta(r,z) 
/// \f] 
/// The element geometry is obtained from the FaceGeometry<ELEMENT> 
/// policy class.
//======================================================================
template <class ELEMENT>
class SteadyAxisymAdvectionDiffusionFluxElement : 
public virtual FaceGeometry<ELEMENT>, 
 public virtual FaceElement
{
 
public:


 /// \short Function pointer to the prescribed-beta function fct(x,beta(x)) -- 
 /// x is a Vector! 
 typedef void (*SteadyAxisymAdvectionDiffusionPrescribedBetaFctPt)(const Vector<double>& x, 
                                                                   double& beta);

 /// \short Function pointer to the prescribed-alpha function fct(x,alpha(x)) -- 
 /// x is a Vector! 
 typedef void (*SteadyAxisymAdvectionDiffusionPrescribedAlphaFctPt)(const Vector<double>& x, 
                                                                   double& alpha);


 /// \short Constructor, takes the pointer to the "bulk" element
 /// and the index of the face to be created
 SteadyAxisymAdvectionDiffusionFluxElement(FiniteElement* const &bulk_el_pt, 
                                           const int &face_index);


 /// Broken empty constructor
 SteadyAxisymAdvectionDiffusionFluxElement()
  {
   throw OomphLibError(
    "Don't call empty constructor for SteadyAxisymAdvectionDiffusionFluxElement",
    "AdvectionDiffusionFluxElement::AdvectionDiffusionFluxElement()",
    OOMPH_EXCEPTION_LOCATION);
  }

 /// Broken copy constructor
 SteadyAxisymAdvectionDiffusionFluxElement(const SteadyAxisymAdvectionDiffusionFluxElement& dummy) 
  { 
   BrokenCopy::broken_copy("SteadyAxisymAdvectionDiffusionFluxElement");
  } 
 
 /// Broken assignment operator
 void operator=(const SteadyAxisymAdvectionDiffusionFluxElement&) 
  {
   BrokenCopy::broken_assign("SteadyAxisymAdvectionDiffusionFluxElement");
  }

 /// Access function for the prescribed-beta function pointer
 SteadyAxisymAdvectionDiffusionPrescribedBetaFctPt& beta_fct_pt() 
  {
   return Beta_fct_pt;
  }

 /// Access function for the prescribed-alpha function pointer
 SteadyAxisymAdvectionDiffusionPrescribedAlphaFctPt& alpha_fct_pt() 
  {
   return Alpha_fct_pt;
  }


 /// Add the element's contribution to its residual vector
 inline void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Call the generic residuals function with flag set to 0
   //using a dummy matrix
   fill_in_generic_residual_contribution_adv_diff_flux(residuals,
                                                       GeneralisedElement::Dummy_matrix,
                                                       0);
  }


 /// \short Add the element's contribution to its residual vector and 
 /// its Jacobian matrix
 inline void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                              DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_adv_diff_flux(residuals,
                                                       jacobian,
                                                       1);
  }
 
 /// \short Output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(std::ostream &outfile) 
  {
   FiniteElement::output(outfile);
  }

 /// \short Output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(std::ostream &outfile, const unsigned &nplot)
  {
   FiniteElement::output(outfile,nplot);
  }


protected:

 /// \short Function to compute the shape and test functions and to return 
 /// the Jacobian of mapping between local and global (Eulerian)
 /// coordinates
 inline double shape_and_test(const Vector<double> &s, 
                              Shape &psi, 
                              Shape &test) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Get the shape functions
   shape(s,psi);

   //Set the test functions to be the same as the shape functions
   for(unsigned i=0;i<n_node;i++) 
    {
     test[i] = psi[i];
    }

   //Return the value of the jacobian
   return J_eulerian(s);
  }


 /// \short Function to compute the shape and test functions and to return 
 /// the Jacobian of mapping between local and global (Eulerian)
 /// coordinates
 inline double shape_and_test_at_knot(const unsigned &ipt,
                                      Shape &psi, 
                                      Shape &test) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Get the shape functions
   shape_at_knot(ipt,psi);

   //Set the test functions to be the same as the shape functions
   for(unsigned i=0;i<n_node;i++) 
    {
     test[i] = psi[i];
    }

   //Return the value of the jacobian
   return J_eulerian_at_knot(ipt);
  }

 /// \short Function to calculate the prescribed beta at a given spatial
 /// position
 void get_beta(const Vector<double>& x, double& beta)
  {
   //If the function pointer is zero return zero
   if(Beta_fct_pt == 0)
    {
     beta = 0.0;
    }
   //Otherwise call the function
   else
    {
     (*Beta_fct_pt)(x,beta);
    }
  }

 /// \short Function to calculate the prescribed alpha at a given spatial
 /// position
 void get_alpha(const Vector<double>& x, double& alpha)
  {
   //If the function pointer is zero return zero
   if(Alpha_fct_pt == 0)
    {
     alpha = 0.0;
    }
   //Otherwise call the function
   else
    {
     (*Alpha_fct_pt)(x,alpha);
    }
  }

private:


 /// \short Add the element's contribution to its residual vector.
 /// flag=1(or 0): do (or don't) compute the Jacobian as well. 
 void fill_in_generic_residual_contribution_adv_diff_flux(Vector<double> &residuals, 
                                                          DenseMatrix<double> &jacobian, 
                                                          unsigned flag);
 
 
 /// Function pointer to the (global) prescribed-beta function
 SteadyAxisymAdvectionDiffusionPrescribedBetaFctPt Beta_fct_pt;

 /// Function pointer to the (global) prescribed-alpha function
 SteadyAxisymAdvectionDiffusionPrescribedAlphaFctPt Alpha_fct_pt;

 /// The index at which the unknown is stored at the nodes
 unsigned U_index_adv_diff;


};//End class SteadyAxisymAdvectionDiffusionFluxElement


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


//===========================================================================
/// \short Constructor, takes the pointer to the "bulk" element and the index
/// of the face to be created 
//===========================================================================
template<class ELEMENT>
SteadyAxisymAdvectionDiffusionFluxElement<ELEMENT>::
SteadyAxisymAdvectionDiffusionFluxElement(FiniteElement* const &bulk_el_pt, 
                                          const int &face_index) : 
FaceGeometry<ELEMENT>(), FaceElement()

{ 

#ifdef PARANOID
 {
  //Check that the element is not a refineable 3d element
  ELEMENT* elem_pt = new ELEMENT;
  //If it's three-d
  if(elem_pt->dim()==3)
   {
    //Is it refineable
    if(dynamic_cast<RefineableElement*>(elem_pt))
     {
      //Issue a warning
      OomphLibWarning(
       "This flux element will not work correctly if nodes are hanging\n",
       "SteadyAxisymAdvectionDiffusionFluxElement::Constructor",
       OOMPH_EXCEPTION_LOCATION);
     }
   }
 }
#endif
 
 //Let the bulk element build the FaceElement, i.e. setup the pointers 
 //to its nodes (by referring to the appropriate nodes in the bulk
 //element), etc.
 bulk_el_pt->build_face_element(face_index,this);
 
 //Initialise the prescribed-beta function pointer to zero
 Beta_fct_pt = 0;
 
 //Set up U_index_adv_diff. Initialise to zero, which probably won't change
 //in most cases, oh well, the price we pay for generality
 U_index_adv_diff = 0;
 
 //Cast to the appropriate AdvectionDiffusionEquation so that we can
 //find the index at which the variable is stored
 //We assume that the dimension of the full problem is the same
 //as the dimension of the node, if this is not the case you will have
 //to write custom elements, sorry
 SteadyAxisymAdvectionDiffusionEquations* eqn_pt =   
 dynamic_cast<SteadyAxisymAdvectionDiffusionEquations*>(bulk_el_pt);

 //If the cast has failed die
 if(eqn_pt==0)
  {
     std::string error_string =
      "Bulk element must inherit from SteadyAxisymAdvectionDiffusionEquations.";
     error_string += 
      "Nodes are two dimensional, but cannot cast the bulk element to\n";
     error_string += "SteadyAxisymAdvectionDiffusionEquations<2>\n.";
     error_string += 
      "If you desire this functionality, you must implement it yourself\n";
     
     throw OomphLibError(
      error_string,
      "AdvectionDiffusionFluxElement::AdvectionDiffusionFluxElement()",
      OOMPH_EXCEPTION_LOCATION);
  }
 else
  {
   //Read the index from the (cast) bulk element.
   U_index_adv_diff = eqn_pt->u_index_axisym_adv_diff();
  }


}


//===========================================================================
/// \short Compute the element's residual vector and the (zero) Jacobian 
/// matrix for the Robin boundary condition:
/// \f[ 
/// \Delta u \cdot \mathbf{n} + \alpha (\mathbf{x}) = \beta (\mathbf{x})
/// \f] 
//===========================================================================
template<class ELEMENT>
void SteadyAxisymAdvectionDiffusionFluxElement<ELEMENT>::
fill_in_generic_residual_contribution_adv_diff_flux(Vector<double> &residuals, 
                                                    DenseMatrix<double> &jacobian, 
                                                    unsigned flag)
{
 //Find out how many nodes there are
 const unsigned n_node = nnode();

 // Locally cache the index at which the variable is stored
 const unsigned u_index_axisym_adv_diff = U_index_adv_diff;
  
 //Set up memory for the shape and test functions
 Shape psif(n_node), testf(n_node);
 
 //Set the value of n_intpt
 const unsigned n_intpt = integral_pt()->nweight();
 
 //Set the Vector to hold local coordinates
 Vector<double> s(1);
 
 //Integers used to store the local equation number and local unknown
 //indices for the residuals and jacobians
 int local_eqn=0, local_unknown=0;

 //Loop over the integration points
 //--------------------------------
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {

   //Assign values of s
   for(unsigned i=0;i<1;i++) 
    {
     s[i] = integral_pt()->knot(ipt,i);
    }
   
   //Get the integral weight
   double w = integral_pt()->weight(ipt);
   
   //Find the shape and test functions and return the Jacobian
   //of the mapping
   double J = shape_and_test(s,psif,testf);

   //Premultiply the weights and the Jacobian
   double W = w*J;

   //Calculate local values of the solution and its derivatives
   //Allocate
   double interpolated_u=0.0;
   Vector<double> interpolated_x(2,0.0);
      
   //Calculate position
   for(unsigned l=0;l<n_node;l++) 
    {
     //Get the value at the node
     double u_value = raw_nodal_value(l,u_index_axisym_adv_diff);
     interpolated_u += u_value*psif(l);
     //Loop over coordinate direction
     for(unsigned i=0;i<2;i++)
      {
       interpolated_x[i] += nodal_position(l,i)*psif(l);
      }
    }
   
   //Get the imposed beta (beta=flux when alpha=0.0)
   double beta;
   get_beta(interpolated_x,beta);

   //Get the imposed alpha
   double alpha;
   get_alpha(interpolated_x,alpha);
   
   //r is the first position component
   double r = interpolated_x[0];

   //Now add to the appropriate equations
   
   //Loop over the test functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Set the local equation number
     local_eqn = nodal_local_eqn(l,u_index_axisym_adv_diff);
     /*IF it's not a boundary condition*/
     if(local_eqn >= 0)
      {
       //Add the prescribed beta terms
       residuals[local_eqn] -= r*(beta - alpha*interpolated_u)*testf(l)*W;
            
       //Calculate the Jacobian
       //----------------------
       if ( (flag) && (alpha!=0.0) )
        {
         //Loop over the velocity shape functions again
         for(unsigned l2=0;l2<n_node;l2++)
          {
           //Set the number of the unknown
           local_unknown = nodal_local_eqn(l2,u_index_axisym_adv_diff);
           
           //If at a non-zero degree of freedom add in the entry
           if(local_unknown >= 0)
            {
             jacobian(local_eqn,local_unknown) +=
              r*alpha*psif[l2]*testf[l]*W;
            }
          }
        }
       
      }
    } //end loop over test functions

  } //end loop over integration points

}//end fill_in_generic_residual_contribution_adv_diff_flux


} //End SteadyAxisymAdvectionDiffusionFluxElement class

#endif