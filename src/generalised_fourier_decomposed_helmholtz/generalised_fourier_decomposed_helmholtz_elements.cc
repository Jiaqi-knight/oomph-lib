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
//Non-inline functions for Helmholtz elements
#include "generalised_fourier_decomposed_helmholtz_elements.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


namespace oomph
{


 //========================================================================
 /// Helper namespace for functions required for Helmholtz computations
 //========================================================================
  namespace Legendre_functions_helper
  {

 //========================================================================
 // Factorial
 //========================================================================
   double factorial(const unsigned& l)
   {
    if(l==0) return 1.0;
    return double(l*factorial(l-1));
   }


 //========================================================================
 /// Legendre polynomials depending on one parameter
 //========================================================================
   double plgndr1(const unsigned& n, const double& x)
   {
    unsigned i;
    double pmm,pmm1;
    double pmm2=0;

 #ifdef PARANOID
    // Shout if things went wrong  
    if (std::fabs(x) > 1.0)
     {
      std::ostringstream error_stream;    
      error_stream << "Bad arguments in routine plgndr1: x=" << x 
                   << " but should be less than 1 in absolute value.\n";  
      throw OomphLibError(error_stream.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
 #endif

    // Compute pmm : if l=m it's finished
    pmm=1.0;
    if (n == 0) 
     {
      return pmm;
     }

    pmm1=x*1.0;
    if (n == 1) 
     {
      return pmm1;
     }
    else
     {
      for (i=2;i<=n;i++) 
       {
        pmm2=(x*(2*i-1)*pmm1-(i-1)*pmm)/i;
        pmm=pmm1;
        pmm1=pmm2;
       }
      return pmm2;
     }

   }//end of plgndr1



 //========================================================================
 // Legendre polynomials depending on two parameters
 //========================================================================
   double plgndr2(const unsigned& l, const unsigned& m, const double& x)
   {
    unsigned i,ll;
    double fact,pmm,pmmp1,somx2;
    double pll=0.0;

 #ifdef PARANOID
    // Shout if things went wrong  
    if (std::fabs(x) > 1.0)
     {
      std::ostringstream error_stream;    
      error_stream << "Bad arguments in routine plgndr2: x=" << x 
                   << " but should be less than 1 in absolute value.\n";  
      throw OomphLibError(error_stream.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
 #endif

    // This one is easy...
    if (m > l)
     {
      return 0.0;
     }

    // Compute pmm : if l=m it's finished
    pmm=1.0;
    if (m > 0)
     {
      somx2=sqrt((1.0-x)*(1.0+x));
      fact=1.0;
      for (i=1;i<=m;i++) 
       {
        pmm *= -fact*somx2;
        fact += 2.0;
       }
     }
    if (l == m) return pmm;

    // Compute pmmp1 : if l=m+1 it's finished
    else 
     {
      pmmp1=x*(2*m+1)*pmm;
      if (l == (m+1))
       {
        return pmmp1;
       }
      // Compute pll : if l>m+1 it's finished
      else 
       {
        for (ll=m+2;ll<=l;ll++) 
         {
          pll=(x*(2*ll-1)*pmmp1-(ll+m-1)*pmm)/(ll-m);
          pmm=pmmp1;
          pmmp1=pll;
         }
        return pll;
       }
     }
   }//end of plgndr2

  } // end namespace



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


//======================================================================
/// Set the data for the number of Variables at each node, always two
/// (real and imag part) in every case
//======================================================================
 template<unsigned NNODE_1D>
 const unsigned QGeneralisedFourierDecomposedHelmholtzElement<NNODE_1D>::
 Initial_Nvalue=2;


//======================================================================
/// Compute element residual Vector and/or element Jacobian matrix 
/// 
/// flag=1: compute both
/// flag=0: compute only residual Vector
///
/// Pure version without hanging nodes
//======================================================================
 void  GeneralisedFourierDecomposedHelmholtzEquations::
 fill_in_generic_residual_contribution_generalised_fourier_decomposed_helmholtz
 (Vector<double> &residuals, 
  DenseMatrix<double> &jacobian, 
  const unsigned& flag) 
 {
  //Find out how many nodes there are
  const unsigned n_node = nnode();
  
  //Set up memory for the shape and test functions
  Shape psi(n_node), test(n_node);
  DShape dpsidx(n_node,2), dtestdx(n_node,2);
  
  //Set the value of n_intpt
  const unsigned n_intpt = integral_pt()->nweight();
  
  //Integers to store the local equation and unknown numbers
  int local_eqn_real=0, local_unknown_real=0;
  int local_eqn_imag=0, local_unknown_imag=0;
  
  //Loop over the integration points
  for(unsigned ipt=0;ipt<n_intpt;ipt++)
   {
    //Get the integral weight
    double w = integral_pt()->weight(ipt);
    
    //Call the derivatives of the shape and test functions
    double J = 
     dshape_and_dtest_eulerian_at_knot_generalised_fourier_decomposed_helmholtz
     (ipt,psi,dpsidx,test,dtestdx);
    
     //Premultiply the weights and the Jacobian
     double W = w*J;
    
    //Calculate local values of unknown
    //Allocate and initialise to zero
    std::complex<double> interpolated_u(0.0,0.0);
    Vector<double> interpolated_x(2,0.0);
    Vector< std::complex<double> > interpolated_dudx(2);
    
    //Calculate function value and derivatives:
    //-----------------------------------------
    // Loop over nodes
    for(unsigned l=0;l<n_node;l++) 
     {
      // Loop over directions
      for(unsigned j=0;j<2;j++)
       {
        interpolated_x[j] += raw_nodal_position(l,j)*psi(l);
       }

      //Get the nodal value of the helmholtz unknown
      const std::complex<double> u_value(
       raw_nodal_value
       (l,u_index_generalised_fourier_decomposed_helmholtz().real()),
       raw_nodal_value
       (l,u_index_generalised_fourier_decomposed_helmholtz().imag()));

      //Add to the interpolated value
      interpolated_u += u_value*psi(l);
      
      // Loop over directions
      for(unsigned j=0;j<2;j++)
       {
        interpolated_dudx[j] += u_value*dpsidx(l,j);
       }
     }
    
    //Get source function
    //-------------------
    std::complex<double> source(0.0,0.0);
    get_source_generalised_fourier_decomposed_helmholtz
     (ipt,interpolated_x,source);
    
    double r = interpolated_x[0];
    double n = (double)generalised_fourier_wavenumber();
    double n_squared = n*n;
     
    //Determine absorption factor
    //--------------------------
    double alpha=0.0;
    get_alpha_generalised_fourier_decomposed_helmholtz
     (ipt,interpolated_x,alpha);

    //Determine sound speed
    //---------------------
    double c=0.0;
    get_c_generalised_fourier_decomposed_helmholtz(ipt,interpolated_x,c); 


    /// \short Use soundspeed, absorption and frequency 
    /// to precompute expressions to be subsequently used
    double c_squared = c * c;
    double alpha_squared = alpha * alpha;
    double omega_squared = omega() * omega();


   // Declare a vector of complex numbers for pml weights on the Laplace bit
   Vector< std::complex<double> > pml_stiffness_weight(2);
   // Declare a complex number for pml weights on the mass matrix bit
   std::complex<double> pml_mass_weight = std::complex<double>(1.0,0.0);

   //  All the PML weights that participate in the assemby process 
   // are computed here. pml_stiffness_weight will contain the entries
   // for the Laplace bit, while pml_mass_weight contains the contributions
   // to the Helmholtz bit. Both default to 1.0, should the PML not be 
   // enabled via enable_pml.
   compute_pml_coefficients(ipt, interpolated_x, 
                            pml_stiffness_weight, 
                            pml_mass_weight); 


   //Declare the complex r variable 
   std::complex<double> complex_r = std::complex<double>(1.0,0.0);

   // Determine the complex r variable.  The variable is
   // only complex once it enters the right pml domain or either
   // of the two corner pml domains, otherwise it acts like the
   // variable r.
   compute_complex_r(ipt, interpolated_x, complex_r);


   // Compute the coefficients needed for the residuals.
   // The variables below use the complex variable r and the 
   // various pml weights.
   std::complex<double> complex_r_squared = (complex_r*complex_r);
   std::complex<double> div_comp_r = 
    (1.0 / std::complex<double> (
      complex_r_squared.real(), complex_r_squared.imag()) );
   double expr1 = 
    (omega_squared/c_squared)-alpha_squared-(n_squared*div_comp_r.real() );
   double expr2 = ((2.0*omega()*alpha)/c) - (n_squared*div_comp_r.imag() );
   
   
   
   // Assemble residuals and Jacobian
   //--------------------------------
    
   // Loop over the test functions
   for(unsigned l=0;l<n_node;l++)
    {
     
     // first, compute the real part contribution 
     //-------------------------------------------
     
     //Get the local equations
     local_eqn_real = 
      nodal_local_eqn
      (l,u_index_generalised_fourier_decomposed_helmholtz().real());
     local_eqn_imag = 
      nodal_local_eqn
      (l,u_index_generalised_fourier_decomposed_helmholtz().imag());

     /*IF it's not a boundary condition*/
     if(local_eqn_real >= 0)
      {
       // Add body force/source term and Helmholtz bit
       residuals[local_eqn_real] +=
        ((pml_mass_weight.real()*
          (expr2*interpolated_u.imag()-expr1*interpolated_u.real() ))
         +(pml_mass_weight.imag()*
           (expr1*interpolated_u.imag()+expr2*interpolated_u.real()))
         )*test(l)*r*W;
       
       // The Laplace bit
       for (unsigned k=0;k<2;k++)
        {
         residuals[local_eqn_real] += 
          (pml_stiffness_weight[k].real()*interpolated_dudx[k].real() - 
           pml_stiffness_weight[k].imag()*interpolated_dudx[k].imag() )*
          dtestdx(l,k)*r*W;
        }
       
       
       // Calculate the jacobian
       //-----------------------
       if(flag)
        {
         //Loop over the velocity shape functions again
         for(unsigned l2=0;l2<n_node;l2++)
          { 
           local_unknown_real = 
            nodal_local_eqn
            (l2,u_index_generalised_fourier_decomposed_helmholtz().real());
           local_unknown_imag =
            nodal_local_eqn
            (l2,u_index_generalised_fourier_decomposed_helmholtz().imag());
           
           
           //If at a non-zero degree of freedom add in the entry
           if(local_unknown_real >= 0)
            {              
             // Add the helmholtz contribution
             jacobian(local_eqn_real,local_unknown_real) += 
              ( -(pml_mass_weight.real()*(expr1)) + 
                (pml_mass_weight.imag()*expr2 ) )*psi(l2)*test(l)*r*W;
             
             //Add contribution to elemental Matrix
             for (unsigned k=0;k<2;k++)
              {
               jacobian(local_eqn_real,local_unknown_real) +=
                pml_stiffness_weight[k].real()*dpsidx(l2,k)*dtestdx(l,k)*r*W;
              }
            }
           //If at a non-zero degree of freedom add in the entry
           if(local_unknown_imag >= 0)
            {              
             // Add the helmholtz contribution
             jacobian(local_eqn_real,local_unknown_imag) += 
              ( (pml_mass_weight.real()*expr2) + 
                (pml_mass_weight.imag()*expr1 ) )*psi(l2)*test(l)*r*W;
             
             //Add contribution to elemental Matrix
             for (unsigned k=0;k<2;k++)
              {
               jacobian(local_eqn_real,local_unknown_imag) +=
                -pml_stiffness_weight[k].imag()*dpsidx(l2,k)*dtestdx(l,k)*r*W;
              }
            }
           
          } // end of loop over velocity shape functions
        } // end of if(flag)
      }
     
     // Second, compute the imaginary part contribution 
     //------------------------------------------------
     
     //Get the local equation
     local_eqn_imag = 
      nodal_local_eqn
      (l,u_index_generalised_fourier_decomposed_helmholtz().imag());
     local_eqn_real = 
      nodal_local_eqn
      (l,u_index_generalised_fourier_decomposed_helmholtz().real());
     
     /*IF it's not a boundary condition*/
     if(local_eqn_imag >= 0)
      {
       // Add body force/source term and Helmholtz bit
       residuals[local_eqn_imag] += 
        ((-pml_mass_weight.real()*
          (expr1*interpolated_u.imag()+expr2*interpolated_u.real() ))
         +(pml_mass_weight.imag()*
           (expr2*interpolated_u.imag()-expr1*interpolated_u.real() ))
         )*test(l)*r*W;
       
       // The Laplace bit itself
       for (unsigned k=0;k<2;k++)
        {
         residuals[local_eqn_imag] +=
          (pml_stiffness_weight[k].real()*interpolated_dudx[k].imag() + 
           pml_stiffness_weight[k].imag()*interpolated_dudx[k].real())*
          dtestdx(l,k)*r*W;
        }
       
       // Calculate the jacobian
       //-----------------------
       if(flag)
        {
         //Loop over the velocity shape functions again
         for(unsigned l2=0;l2<n_node;l2++)
          { 
           local_unknown_imag = nodal_local_eqn
            (l2,u_index_generalised_fourier_decomposed_helmholtz().imag());
           local_unknown_real = 
            nodal_local_eqn
            (l2,u_index_generalised_fourier_decomposed_helmholtz().real());
           
           //If at a non-zero degree of freedom add in the entry
           if(local_unknown_imag >= 0)
            {              
             //Add the helmholtz contribution
             jacobian(local_eqn_imag,local_unknown_imag) += 
              ((-pml_mass_weight.real()*expr1)+
               (pml_mass_weight.imag()*expr2) )*psi(l2)*test(l)*r*W;
             
             //Add contribution to elemental Matrix
             for (unsigned k=0;k<2;k++)
              {
               jacobian(local_eqn_imag,local_unknown_imag) +=
                pml_stiffness_weight[k].real()*dpsidx(l2,k)*dtestdx(l,k)*r*W;
              }
            }
           //If at a non-zero degree of freedom add in the entry
           if(local_unknown_real >= 0)
            {              
             //Add the helmholtz contribution
             jacobian(local_eqn_imag,local_unknown_real) += 
              ((-pml_mass_weight.real()*expr2)-
               (pml_mass_weight.imag()*expr1) )*psi(l2)*test(l)*r*W;
             
             //Add contribution to elemental Matrix
             for (unsigned k=0;k<2;k++)
              {
               jacobian(local_eqn_imag,local_unknown_real) +=
                pml_stiffness_weight[k].imag()*dpsidx(l2,k)*dtestdx(l,k)*r*W;
              }
            }
          }
        }
      }
    }  
   } // End of loop over integration points
 }
 
 


 
 
//======================================================================
/// Self-test:  Return 0 for OK
//======================================================================
 unsigned  GeneralisedFourierDecomposedHelmholtzEquations::self_test()
 {
  
  bool passed=true;
  
  // Check lower-level stuff
  if (FiniteElement::self_test()!=0)
   {
    passed=false;
   }
  
  // Return verdict
  if (passed)
   {
    return 0;
   }
  else
   {
    return 1;
   }
 }
 
 
 
//======================================================================
/// Output function:
///
///   r,z,u_re,u_imag  
///
/// nplot points in each coordinate direction
//======================================================================
 void  GeneralisedFourierDecomposedHelmholtzEquations::output
 (std::ostream &outfile, 
  const unsigned &nplot)
 {
  
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Tecplot header info
  outfile << tecplot_zone_string(nplot);
  
  // Loop over plot points
  unsigned num_plot_points=nplot_points(nplot);
  for (unsigned iplot=0;iplot<num_plot_points;iplot++)
   {
    
    // Get local coordinates of plot point
    get_s_plot(iplot,nplot,s);
    std::complex<double> 
     u(interpolated_u_generalised_fourier_decomposed_helmholtz(s));
    for(unsigned i=0;i<2;i++) 
     {
      outfile << interpolated_x(s,i) << " ";
     }
    outfile << u.real() << " " << u.imag() << std::endl;   
    
   }
  
  // Write tecplot footer (e.g. FE connectivity lists)
  write_tecplot_zone_footer(outfile,nplot);
  
 }
 



//======================================================================
/// Output function for real part of full time-dependent solution
///
///  u = Re( (u_r +i u_i) exp(-i omega t)
///
/// at phase angle omega t = phi.
///
///  r,z,u
///
/// Output at nplot points in each coordinate direction
//======================================================================
 void  GeneralisedFourierDecomposedHelmholtzEquations::output_real
 (std::ostream &outfile, 
  const double& phi,
  const unsigned &nplot)
 {
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Tecplot header info
  outfile << tecplot_zone_string(nplot);
  
  // Loop over plot points
  unsigned num_plot_points=nplot_points(nplot);
  for (unsigned iplot=0;iplot<num_plot_points;iplot++)
   {
    
    // Get local coordinates of plot point
    get_s_plot(iplot,nplot,s);
    std::complex<double> 
     u(interpolated_u_generalised_fourier_decomposed_helmholtz(s));
    for(unsigned i=0;i<2;i++) 
     {
      outfile << interpolated_x(s,i) << " ";
     }
    outfile << u.real()*cos(phi)+u.imag()*sin(phi) << std::endl;   
    
   }
  
  // Write tecplot footer (e.g. FE connectivity lists)
  write_tecplot_zone_footer(outfile,nplot);
  
 }
 
 
//======================================================================
/// C-style output function:
///
///   r,z,u 
///
/// nplot points in each coordinate direction
//======================================================================
 void  GeneralisedFourierDecomposedHelmholtzEquations::output(FILE* file_pt,
                                                   const unsigned &nplot)
 {
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Tecplot header info
  fprintf(file_pt,"%s",tecplot_zone_string(nplot).c_str());
  
  // Loop over plot points
  unsigned num_plot_points=nplot_points(nplot);
  for (unsigned iplot=0;iplot<num_plot_points;iplot++)
   {
    // Get local coordinates of plot point
    get_s_plot(iplot,nplot,s);
    std::complex<double> 
     u(interpolated_u_generalised_fourier_decomposed_helmholtz(s));
    
    for(unsigned i=0;i<2;i++) 
     {
      fprintf(file_pt,"%g ",interpolated_x(s,i));
     }
    
    for(unsigned i=0;i<2;i++) 
     {
      fprintf(file_pt,"%g ",interpolated_x(s,i));
     }
    fprintf(file_pt,"%g ",u.real());
    fprintf(file_pt,"%g \n",u.imag());
    
   }
  
  // Write tecplot footer (e.g. FE connectivity lists)
  write_tecplot_zone_footer(file_pt,nplot);
 }
 
 
 
//======================================================================
 /// Output exact solution
 /// 
 /// Solution is provided via function pointer.
 /// Plot at a given number of plot points.
 ///
 ///   r,z,u_exact
//======================================================================
 void GeneralisedFourierDecomposedHelmholtzEquations::output_fct(
  std::ostream &outfile, 
  const unsigned &nplot, 
  FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
 {
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Vector for coordintes
  Vector<double> x(2);
  
  // Tecplot header info
  outfile << tecplot_zone_string(nplot);
  
  // Exact solution Vector 
  Vector<double> exact_soln(2);
  
  // Loop over plot points
  unsigned num_plot_points=nplot_points(nplot);
  for (unsigned iplot=0;iplot<num_plot_points;iplot++)
   {
    // Get local coordinates of plot point
    get_s_plot(iplot,nplot,s);
    
    // Get x position as Vector
    interpolated_x(s,x);
    
    // Get exact solution at this point
    (*exact_soln_pt)(x,exact_soln);
    
    //Output r,z,u_exact
    for(unsigned i=0;i<2;i++)
     {
      outfile << x[i] << " ";
     }
    outfile << exact_soln[0] << " " <<  exact_soln[1] << std::endl;  
   }
  
  // Write tecplot footer (e.g. FE connectivity lists)
  write_tecplot_zone_footer(outfile,nplot);
 }
 
 
 
//======================================================================
/// Output function for real part of full time-dependent fct
///
///  u = Re( (u_r +i u_i) exp(-i omega t)
///
/// at phase angle omega t = phi.
///
///   r,z,u
///
/// Output at nplot points in each coordinate direction
//======================================================================
 void GeneralisedFourierDecomposedHelmholtzEquations::output_real_fct(
  std::ostream &outfile, 
  const double& phi,
  const unsigned &nplot, 
  FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
 {
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Vector for coordintes
  Vector<double> x(2);
  
  // Tecplot header info
  outfile << tecplot_zone_string(nplot);
  
  // Exact solution Vector 
  Vector<double> exact_soln(2);
  
  // Loop over plot points
  unsigned num_plot_points=nplot_points(nplot);
  for (unsigned iplot=0;iplot<num_plot_points;iplot++)
   {
    // Get local coordinates of plot point
    get_s_plot(iplot,nplot,s);
    
    // Get x position as Vector
    interpolated_x(s,x);
    
    // Get exact solution at this point
    (*exact_soln_pt)(x,exact_soln);
    
    //Output x,y,...,u_exact
    for(unsigned i=0;i<2;i++)
     {
      outfile << x[i] << " ";
     }
    outfile << exact_soln[0]*cos(phi)+ exact_soln[1]*sin(phi) << std::endl;  
   }
  
  // Write tecplot footer (e.g. FE connectivity lists)
  write_tecplot_zone_footer(outfile,nplot);
 }
 



//======================================================================
 /// Validate against exact solution
 /// 
 /// Solution is provided via function pointer.
 /// Plot error at a given number of plot points.
 ///
//======================================================================
 void GeneralisedFourierDecomposedHelmholtzEquations::compute_error(
  std::ostream &outfile, 
  FiniteElement::SteadyExactSolutionFctPt exact_soln_pt,
  double& error, double& norm)
 { 
  
  // Initialise
  error=0.0;
  norm=0.0;
  
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Vector for coordintes
  Vector<double> x(2);
  
  //Find out how many nodes there are in the element
  unsigned n_node = nnode();
  
  Shape psi(n_node);
  
  //Set the value of n_intpt
  unsigned n_intpt = integral_pt()->nweight();
  
  // Tecplot 
  outfile << "ZONE" << std::endl;
  
  // Exact solution Vector 
  Vector<double> exact_soln(2);
  
  //Loop over the integration points
  for(unsigned ipt=0;ipt<n_intpt;ipt++)
   {
    
    //Assign values of s
    for(unsigned i=0;i<2;i++)
     {
      s[i] = integral_pt()->knot(ipt,i);
     }
    
    //Get the integral weight
    double w = integral_pt()->weight(ipt);
    
    // Get jacobian of mapping
    double J=J_eulerian(s);
    
    //Premultiply the weights and the Jacobian
    double W = w*J;
    
    // Get x position as Vector
    interpolated_x(s,x);
    
    // Get FE function value
    std::complex<double> u_fe=
     interpolated_u_generalised_fourier_decomposed_helmholtz(s);
    
    // Get exact solution at this point
    (*exact_soln_pt)(x,exact_soln);
    
    //Output r,z,error
    for(unsigned i=0;i<2;i++)
     {
      outfile << x[i] << " ";
     }
    outfile << exact_soln[0] << " "  << exact_soln[1] << " " 
            << exact_soln[0]-u_fe.real() << " " << exact_soln[1]-u_fe.imag() 
            << std::endl;  
    
    // Add to error and norm
    norm+=(exact_soln[0]*exact_soln[0]+exact_soln[1]*exact_soln[1])*W;
    error+=( (exact_soln[0]-u_fe.real())*(exact_soln[0]-u_fe.real())+
             (exact_soln[1]-u_fe.imag())*(exact_soln[1]-u_fe.imag()) )*W;
    
   }
 }
 
 

//======================================================================
 /// Compute norm of fe solution
//======================================================================
 void GeneralisedFourierDecomposedHelmholtzEquations::compute_norm
 (double& norm)
 { 
  
  // Initialise
  norm=0.0;
  
  //Vector of local coordinates
  Vector<double> s(2);
  
  // Vector for coordintes
  Vector<double> x(2);
  
  //Find out how many nodes there are in the element
  unsigned n_node = nnode();
  
  Shape psi(n_node);
  
  //Set the value of n_intpt
  unsigned n_intpt = integral_pt()->nweight();
  
  //Loop over the integration points
  for(unsigned ipt=0;ipt<n_intpt;ipt++)
   {
    
    //Assign values of s
    for(unsigned i=0;i<2;i++)
     {
      s[i] = integral_pt()->knot(ipt,i);
     }
    
    //Get the integral weight
    double w = integral_pt()->weight(ipt);
    
    // Get jacobian of mapping
    double J=J_eulerian(s);
    
    //Premultiply the weights and the Jacobian
    double W = w*J;
    
    // Get FE function value
    std::complex<double> u_fe=
     interpolated_u_generalised_fourier_decomposed_helmholtz(s);
    
    // Add to  norm
    norm+=(u_fe.real()*u_fe.real()+u_fe.imag()*u_fe.imag())*W;
    
   }
 }
 
 
 
 
 
//====================================================================
// Force build of templates
//====================================================================
template class QGeneralisedFourierDecomposedHelmholtzElement<2>;
template class QGeneralisedFourierDecomposedHelmholtzElement<3>;
template class QGeneralisedFourierDecomposedHelmholtzElement<4>;

}
