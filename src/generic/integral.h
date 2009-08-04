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
//Header file for numerical integration routines based on quadrature

//Include guards to prevent multiple inclusions of the header
#ifndef OOMPH_INTEGRAL_HEADER
#define OOMPH_INTEGRAL_HEADER

 
// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//oomph-lib headers
#include "oomph_utilities.h"
#include "orthpoly.h"

namespace oomph
{



//=========================================================
/// Generic class for numerical integration schemes:
/// \f[
/// \int f(x_0,x_1...)\  dx_0 \ dx_1...  =
/// \sum_{i_{int}=0}^{\mbox{\tt nweight()} }
/// f( x_j=\mbox{\tt knot($i_{int}$,j)} )  \ \ \ \mbox{\tt weight($i_{int}$)} 
/// \f]
//=========================================================
class Integral
{
  public:

 /// Default constructor (empty)
 Integral(){};

 /// Broken copy constructor
 Integral(const Integral& dummy) 
  { 
   BrokenCopy::broken_copy("Integral");
  } 
 
 /// Broken assignment operator
 void operator=(const Integral&) 
  {
   BrokenCopy::broken_assign("Integral");
  }

 /// Virtual destructor (empty)
 virtual ~Integral(){};

 /// Return the number of integration points of the scheme. 
 virtual unsigned nweight() const=0;

 /// Return local coordinate s[j]  of i-th integration point.
 virtual double knot(const unsigned &i, const unsigned &j) const=0;

 /// Return weight of i-th integration point.
 virtual double weight(const unsigned &i) const=0;

};

//=============================================================================
/// Broken pseudo-integration scheme for points elements: Iit's not clear
/// in general what this integration scheme is supposed to. It probably
/// ought to evaluate integrals to zero but we're not sure in what
/// context this may be used. Replace by your own integration scheme
/// that does what you want! 
//=============================================================================
class PointIntegral : public Integral
{

public:

 /// Default constructor (empty)
 PointIntegral(){};

 /// Broken copy constructor
 PointIntegral(const PointIntegral& dummy) 
  { 
   BrokenCopy::broken_copy("PointIntegral");
  } 
 
 /// Broken assignment operator
 void operator=(const PointIntegral&) 
  {
   BrokenCopy::broken_assign("PointIntegral");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return 1;}

 /// \short Return coordinate s[j] (j=0) of integration point i -- 
 /// deliberately broken!
 double knot(const unsigned &i, const unsigned &j) const 
  {
   throw OomphLibError(
    "Local coordinate vector is of size zero, so this should never be called.",
    "PointIntegral::knot()",
    OOMPH_EXCEPTION_LOCATION);

   // Dummy return
   return 0.0;
  }

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return 1.0;}

}; 

//=========================================================
/// Class for multidimensional Gaussian integration rules
///
/// Empty -- just establishes the template parameters.
///
/// General logic: The template parameters correspond to those
/// of the QElement family so that the integration scheme
/// Gauss<DIM,NNODE_1D> provides the default ("full") integration
/// scheme for QElement<DIM,NNODE_1D>. "Full" integration
/// means that for linear PDEs that are discretised on a uniform 
/// mesh, all integrals arising in the Galerkin weak form of the PDE
/// are evaluated exactly. In such problems the highest-order 
/// polynomials arise from the products of the undifferentiated 
/// shape and test functions so a 4 node quad needs 
/// an integration scheme that can integrate fourth-order 
/// polynomials exactly etc.
//=========================================================
template <unsigned DIM, unsigned NPTS_1D>
class Gauss 
{
};



//=========================================================
/// 1D Gaussian integration class.
/// Two integration points. This integration scheme can 
/// integrate up to third-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for linear (two-node) elements in which the
/// highest-order polynomial is quadratic.
//=========================================================
template<>
class Gauss<1,2> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=2;
 /// Array to hold weights and knot points (defined in cc file)
 static const double Knot[2][1], Weight[2];

public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate s[j] (j=0) of integration point i
 double knot(const unsigned &i, const unsigned &j) const 
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

}; 

//=========================================================
/// 1D Gaussian integration class.
/// Three integration points. This integration scheme can 
/// integrate up to fifth-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for quadratic (three-node) elements in which the
/// highest-order polynomial is fourth order.
//=========================================================
template<>
class Gauss<1,3> : public Integral
{
  private:
 
 /// Number of integration points in the scheme
 static const unsigned Npts=3;
 /// Array to hold weights and knot points (defined in cc file)
 static const double Knot[3][1], Weight[3];

  public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate s[j] (j=0) of integration point i
 double knot(const unsigned &i, const unsigned &j) const 
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

}; 

//=========================================================
/// 1D Gaussian integration class 
/// Four integration points. This integration scheme can 
/// integrate up to seventh-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for cubic (four-node) elements in which the
/// highest-order polynomial is sixth order.
//=========================================================
template<>
class Gauss<1,4> : public Integral
{

private:

 /// Number of integration points in the scheme
 static const unsigned Npts=4;
 /// Array to hold weight and knot points (defined in cc file)
 static const double Knot[4][1], Weight[4];

public:

 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] (j=0) of integration point i
 double knot(const unsigned &i, const unsigned &j) const 
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

}; 

//=========================================================
/// 2D Gaussian integration class.
/// 2x2 integration points. This integration scheme can 
/// integrate up to third-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for linear (four-node) elements in which the
/// highest-order polynomial is quadratic.
//=========================================================
template<>
class Gauss<2,2> : public Integral
{

private:

 /// Number of integration points in the scheme
 static const unsigned Npts=4;
 /// Array to hold the weight and know points (defined in cc file)
 static const double Knot[4][2], Weight[4];

public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}
};

//=========================================================
/// 2D Gaussian integration class.
/// 3x3 integration points. This integration scheme can 
/// integrate up to fifth-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for quadratic (nine-node) elements in which the
/// highest-order polynomial is fourth order.
//=========================================================
template<>
class Gauss<2,3> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=9;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[9][2], Weight[9];
 
  public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate s[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
/// 2D Gaussian integration class. 
/// 4x4 integration points. This integration scheme can 
/// integrate up to seventh-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for cubic (sixteen-node) elements in which the
/// highest-order polynomial is sixth order.
//=========================================================
template<>
class Gauss<2,4> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=16;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[16][2], Weight[16];


  public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate s[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
/// 3D Gaussian integration class
/// 2x2x2 integration points. This integration scheme can 
/// integrate up to third-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for linear (eight-node) elements in which the
/// highest-order polynomial is quadratic.
//=========================================================
template<>
class Gauss<3,2> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=8;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[8][3], Weight[8];

  public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate s[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
/// 3D Gaussian integration class 
/// 3x3x3 integration points. This integration scheme can 
/// integrate up to fifth-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for quadratic (27-node) elements in which the
/// highest-order polynomial is fourth order.
//=========================================================
template<>
class Gauss<3,3> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=27;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[27][3], Weight[27];

  public:


 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
/// 3D Gaussian integration class.
/// 4x4x4 integration points. This integration scheme can 
/// integrate up to seventh-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for cubic (64-node) elements in which the
/// highest-order polynomial is sixth order.
//=========================================================
template<>
class Gauss<3,4> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=64;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[64][3], Weight[64];

  public:

 /// Default constructor (empty)
 Gauss(){};

 /// Broken copy constructor
 Gauss(const Gauss& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss&) 
  {
   BrokenCopy::broken_assign("Gauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
///\short Class for multidimensional Gaussian integration rules, 
/// over intervals other than -1 to 1, all intervals are 
/// rescaled in this case
//=========================================================
template <unsigned DIM, unsigned NPTS_1D>
class Gauss_Rescaled : public Gauss<DIM,NPTS_1D>
{
  private:

 /// Store for the lower and upper limits of integration and the range
 double Lower, Upper, Range;

  public:

 /// Default constructor (empty)
 Gauss_Rescaled(){};

 /// Broken copy constructor
 Gauss_Rescaled(const Gauss_Rescaled& dummy) 
  { 
   BrokenCopy::broken_copy("Gauss_Rescaled");
  } 
 
 /// Broken assignment operator
 void operator=(const Gauss_Rescaled&) 
  {
   BrokenCopy::broken_assign("Gauss_Rescaled");
  }
 
 /// The constructor in this case takes the lower and upper arguments
 Gauss_Rescaled(double lower, double upper) : Lower(lower), Upper(upper)
  {
   //Set the range of integration
   Range = upper - lower;
  }

 /// Return the rescaled knot values s[j] at integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return (0.5*(Gauss<DIM,NPTS_1D>::knot(i,j)*Range + Lower + Upper));}

 /// Return the rescaled weight at integration point i
 double weight(const unsigned &i) const
  {return Gauss<DIM,NPTS_1D>::weight(i)*pow(0.5*Range,static_cast<int>(DIM));}

};

//=========================================================
/// Class for Gaussian integration rules for triangles/tets.
///
/// Empty -- just establishes the template parameters
///
/// General logic: The template parameters correspond to those
/// of the TElement family so that the integration scheme
/// TGauss<DIM,NNODE_1D> provides the default ("full") integration
/// scheme for TElement<DIM,NNODE_1D>. "Full" integration
/// means that for linear PDEs that are discretised on a uniform 
/// mesh, all integrals arising in the Galerkin weak form of the PDE
/// are evaluated exactly. In such problems the highest-order 
/// polynomials arise from the products of the undifferentiated 
/// shape and test functions so a three node triangle needs 
/// an integration scheme that can integrate quadratic
/// polynomials exactly etc.
//=========================================================
template<unsigned DIM, unsigned NPTS_1D>
class TGauss
{
};


//=========================================================
/// 1D Gaussian integration class for linear "triangular" elements.
/// Two integration points. This integration scheme can 
/// integrate up to second-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for linear (two-node) elements in which the
/// highest-order polynomial is quadratic.
//=========================================================
template<>
class TGauss<1,2> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=2;

 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[2][1], Weight[2];

  public:


 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }
 
 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
/// 1D Gaussian integration class for quadratic "triangular" elements.
/// Three integration points. This integration scheme can 
/// integrate up to fifth-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for quadratic (three-node) elements in which the
/// highest-order polynomial is fourth order.
//=========================================================
template<>
class TGauss<1,3> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=3;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[3][1], Weight[3];

  public:

 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};


//=========================================================
/// 1D Gaussian integration class for cubic "triangular" elements.
/// Four integration points. This integration scheme can 
/// integrate up to seventh-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for cubic (ten-node) elements in which the
/// highest-order polynomial is sixth order.
//=========================================================
template<>
class TGauss<1,4> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=4;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[4][1], Weight[4];

  public:

 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};


//=========================================================
/// 2D Gaussian integration class for linear triangles.
/// Three integration points. This integration scheme can 
/// integrate up to second-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for linear (three-node) elements in which the
/// highest-order polynomial is quadratic.
//=========================================================
template<>
class TGauss<2,2> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=3;

 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[3][2], Weight[3];

  public:


 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }
 
 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};

//=========================================================
/// 2D Gaussian integration class for quadratic triangles.
/// Seven integration points. This integration scheme can 
/// integrate up to fifth-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for quadratic (six-node) elements in which the
/// highest-order polynomial is fourth order.
//=========================================================
template<>
class TGauss<2,3> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=7;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[7][2], Weight[7];

  public:

 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};


//=========================================================
/// 2D Gaussian integration class for cubic triangles.
/// Thirteen integration points. This integration scheme can 
/// integrate up to seventh-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for cubic (ten-node) elements in which the
/// highest-order polynomial is sixth order.
//=========================================================
template<>
class TGauss<2,4> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=13;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[13][2], Weight[13];

  public:

 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};


//=========================================================
/// 3D Gaussian integration class for tets.
/// Four integration points. This integration scheme can 
/// integrate up to second-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for linear (four-node) elements in which the
/// highest-order polynomial is quadratic.
//=========================================================
template<>
class TGauss<3,2> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=4;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[4][3], Weight[4];

  public:


 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};



//=========================================================
/// 3D Gaussian integration class for tets.
/// Eleven integration points. This integration scheme can 
/// integrate up to fourth-order polynomials exactly and
/// is therefore a suitable "full" integration scheme
/// for quadratic (ten-node) elements in which the
/// highest-order polynomial is fourth order.
/// The numbers are from Keast CMAME 55 pp339-348 (1986)
//=========================================================
template<>
class TGauss<3,3> : public Integral
{
  private:

 /// Number of integration points in the scheme
 static const unsigned Npts=11;
 /// Array to hold the weights and knots (defined in cc file)
 static const double Knot[11][3], Weight[11];

  public:


 /// Default constructor (empty)
 TGauss(){};

 /// Broken copy constructor
 TGauss(const TGauss& dummy) 
  { 
   BrokenCopy::broken_copy("TGauss");
  } 
 
 /// Broken assignment operator
 void operator=(const TGauss&) 
  {
   BrokenCopy::broken_assign("TGauss");
  }

 /// Number of integration points of the scheme
 unsigned nweight() const {return Npts;}

 /// Return coordinate x[j] of integration point i
 double knot(const unsigned &i, const unsigned &j) const
  {return Knot[i][j];}

 /// Return weight of integration point i
 double weight(const unsigned &i) const {return Weight[i];}

};



//===================================================================
/// Class for multidimensional Gauss Lobatto Legendre integration 
/// rules
/// empty - just establishes template parameters
//===================================================================
template <unsigned DIM, unsigned NPTS_1D>
class GaussLobattoLegendre 
{
};

//===================================================================
/// 1D Gauss Lobatto Legendre integration class
//===================================================================
template<unsigned NPTS_1D>
class GaussLobattoLegendre<1,NPTS_1D> : public Integral
{
private:

 /// Number of integration points in scheme
 static const unsigned Npts=NPTS_1D;
 /// Array to hold weight and knot points
 //These are not constant, because they are calculated on the fly
 double Knot[NPTS_1D][1], Weight[NPTS_1D];
 
public:

  /// Deafault constructor. Calculates and stores GLL nodes
  GaussLobattoLegendre();

  /// Number of integration points of the scheme   
  unsigned nweight() const {return Npts;}

  /// Return coordinate s[j] (j=0) of integration point i
  double knot(const unsigned &i, const unsigned &j) const
   {return Knot[i][j];}

  /// Return weight of integration point i
  double weight(const unsigned &i) const {return Weight[i];}

};


//=============================================================
/// Calculate positions and weights for the 1D Gauss Lobatto
/// Legendre integration class
//=============================================================
template<unsigned NPTS_1D>
GaussLobattoLegendre<1,NPTS_1D>::GaussLobattoLegendre()
{
 //Temporary storage for the integration points
 Vector<double> s(NPTS_1D),w(NPTS_1D);
 //call the function that calculates the points
 Orthpoly::gll_nodes(NPTS_1D,s,w);
 //Populate the arrays
 for(unsigned i=0;i<NPTS_1D;i++)
  {
   Knot[i][0]=s[i];
   Weight[i]=w[i];
  }
}


//===================================================================
/// 2D Gauss Lobatto Legendre integration class
//===================================================================
template<unsigned NPTS_1D>
class GaussLobattoLegendre<2,NPTS_1D> : public Integral
{
private:

  /// Number of integration points in scheme
  static const unsigned long int Npts=NPTS_1D*NPTS_1D;

  /// Array to hold weight and knot points
  double Knot[NPTS_1D*NPTS_1D][2], 
    Weight[NPTS_1D*NPTS_1D]; // COULDN'T MAKE THESE
  // const BECAUSE THEY ARE CALCULATED (at least currently)

public:

  /// Deafault constructor. Calculates and stores GLL nodes
  GaussLobattoLegendre();

  /// Number of integration points of the scheme   
  unsigned nweight() const {return Npts;}

  /// Return coordinate s[j] (j=0) of integration point i
  double knot(const unsigned  &i, const unsigned &j) const
  {return Knot[i][j];}

  /// Return weight of integration point i
  double weight(const unsigned &i) const {return Weight[i];}

};

//=============================================================
/// Calculate positions and weights for the 2D Gauss Lobatto
/// Legendre integration class
//=============================================================

template<unsigned NPTS_1D>
GaussLobattoLegendre<2,NPTS_1D>::GaussLobattoLegendre()
{
 //Tempoarary storage for the 1D knots and weights
  Vector<double> s(NPTS_1D),w(NPTS_1D);
  //Call the function to populate the array
  Orthpoly::gll_nodes(NPTS_1D,s,w);
  int i_fast=0, i_slow=0;
  for(unsigned i=0;i<NPTS_1D*NPTS_1D;i++){
   if (i_fast == NPTS_1D){i_fast=0;i_slow++;}
   Knot[i][0]=s[i_fast];
   Knot[i][1]=s[i_slow];
   Weight[i]=w[i_fast]*w[i_slow];
   i_fast++;
  }
}




//===================================================================
/// 3D Gauss Lobatto Legendre integration class
//===================================================================
template<unsigned NPTS_1D>
class GaussLobattoLegendre<3,NPTS_1D> : public Integral
{
private:

  /// Number of integration points in scheme
  static const unsigned long int Npts=NPTS_1D*NPTS_1D*NPTS_1D;

  /// Array to hold weight and knot points
  double Knot[NPTS_1D*NPTS_1D*NPTS_1D][3], 
   Weight[NPTS_1D*NPTS_1D*NPTS_1D]; // COULDN'T MAKE THESE
  // const BECAUSE THEY ARE CALCULATED (at least currently)

public:

  /// Deafault constructor. Calculates and stores GLL nodes
  GaussLobattoLegendre();
  
  /// Number of integration points of the scheme   
  unsigned nweight() const {return Npts;}

  /// Return coordinate s[j] (j=0) of integration point i
  double knot(const unsigned  &i, const unsigned &j) const
  {return Knot[i][j];}

  /// Return weight of integration point i
  double weight(const unsigned &i) const {return Weight[i];}

};

//=============================================================
/// Calculate positions and weights for the 3D Gauss Lobatto
/// Legendre integration class
//=============================================================

template<unsigned NPTS_1D>
GaussLobattoLegendre<3,NPTS_1D>::GaussLobattoLegendre()
{
 //Tempoarary storage for the 1D knots and weights
  Vector<double> s(NPTS_1D),w(NPTS_1D);
  //Call the function to populate the array
  Orthpoly::gll_nodes(NPTS_1D,s,w);
  for(unsigned k=0;k<NPTS_1D;k++)
   {
    for(unsigned j=0;j<NPTS_1D;j++)
     {
      for(unsigned i=0;i<NPTS_1D;i++)
       {
        unsigned index = NPTS_1D*NPTS_1D*k + NPTS_1D*j + i;
        Knot[index][0]=s[i];
        Knot[index][1]=s[j];
        Knot[index][2]=s[k]; 
        Weight[index]=w[i]*w[j]*w[k];
       }
     }
   }
}





}

#endif
