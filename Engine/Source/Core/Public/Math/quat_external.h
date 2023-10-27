/* 
 * 			George Terzakis 2016
 * 
 * 	      		University of Portsmouth
 * 
 *       	A Quaternion class implementation based on:
 * 
 *  "Modified Rodrigues Parameters: An Efficient Parametrization of Orientation in 3D Computer Vision and Graphics"
 *		 G. Terzakis, M. Lourakis and D. Ait-Boudaoud 
 *
*/

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include <math.h>
#include <assert.h>
#include <iostream>

namespace RigidTransforms {

typedef enum {QUATERNION_MRPs = 0, QUATERNION_AA, QUATERNION_GIBBS} QPARAM_TYPE;

template<typename P = float>
  class Quaternion 
  {
    
   
  private:
    // This clever little function does type deduction.
    // Credit goes to E. Rosten
    template<typename C> static C _gettype();
    // The QMultype and QAddtype  are dummy structs that yield product and addition
    // types of hetero-typed (or same-type) operands
    template<typename L, typename R>
    struct QMulType {typedef decltype (_gettype<L>()*_gettype<R>()) type; };
    
    template<typename L, typename R> 
    struct QSubType {typedef decltype (_gettype<L>()-_gettype<R>()) type; };
    
    template<typename L, typename R> 
    struct QAddType {typedef decltype (_gettype<L>()+_gettype<R>()) type; };
    
    
    protected: 
      P s, v1, v2, v3; // Adopting a generalized complex number notation: q = s + [v1 v2 v3] .* [i j k]
    
    public:
      /// Create the unit quaternion (1, 0, 0, 0) correponding to the identity rotation 
      inline Quaternion() { s = 1.0; v1 = v2 = v3 = 0; }
      
      /// Create a quaternion from components a, b, c, d
      template<typename Ps, 
	       typename Pv1,
	       typename Pv2,
	       typename Pv3>
      inline Quaternion(Ps s_, Pv1 v1_, Pv2 v2_, Pv3 v3_) : s(s_), v1(v1_), v2(v2_), v3(v3_) {}
      
      /// Quaternion copy constructor
      template<typename Pw>
      inline Quaternion(Quaternion<Pw> w) 
      { 
	s = w.get_s();
	v1 = w.get_v1();
	v2 = w.get_v2();
	v3 = w.get_v3();
      }
      
      /// Create a quaternion from a rotation matrix
      //inline Quaternion( const P (&R)[3][3] ) { *this = Rotation2Quaternion(R); }
      //inline Quaternion( const P (&R)[9] ) { *this = Rotation2Quaternion(R); }
      template<typename Pr>
      inline Quaternion( const Pr* R ) { *this = Rotation2Quaternion(R); }
      
      template<typename Pparam>
      inline Quaternion(const Pparam* p, QPARAM_TYPE paramtype ) 
      {
	if (paramtype == QUATERNION_MRPs ) 
	{
	  *this = Quaternion<P>::CreateFromMRPs<Pparam>(p);
	}
	else if (paramtype == QUATERNION_AA ) 
	{ 
	  *this = Quaternion<P>::exp( p );
	}
	else if (paramtype == QUATERNION_GIBBS)
	{
	  *this = Quaternion<P>::CreateFromGibbs<Pparam>(p);
	}
	else
	{
	    assert(0&&"Illegal quaternion parameter type!");
	}
      }
      
      /// normalize the quaternion
      void unit() 
      {
	
	if ( !(s!=0 || v1!=0 || v2!=0 || v3!=0 ) ) return;
	
	P invnorm = 1.0 / sqrt( s * s + v1 * v1 + v2 * v2 + v3 * v3);
	 
	s *= invnorm; v1 *= invnorm; v2 *= invnorm; v3 *= invnorm; 
      }
    
    
      inline P get_s() const { return s;}
      inline P get_v1() const { return v1;}
      inline P get_v2() const { return v2;}
      inline P get_v3() const { return v3;}
      
      /// Coerces unit norm and returns the rotation matrix corresponding to the resulting unit quaternion
      /*template<typename Pr>
      inline void RotationMatrix( Pr (&R)[3][3] )
      {
	
	// use the normalized verion of the quaternion even ...
	P invnorm = 1.0 / sqrt( s*s + v1*v1 + v2*v2 + v3*v3 );
	s *= invnorm; v1 *= invnorm; v2 *= invnorm; v3 *= invnorm;
	
	R[0][0]  = s*s + v1*v1 - v2*v2 - v3*v3;    R[0][1] = 2*( v1*v2 - s*v3 );              R[0][2] = 2*( v1*v3 + s*v2 );
	R[1][0] = 2*( v1*v2 + s*v3 );              R[1][1] = s*s - v1*v1 + v2*v2 - v3*v3;     R[1][2] = 2*( v2*v3 - s*v1 ); 
	R[2][0] = 2*( v1*v3 - s*v2 );              R[2][1] = 2*( v2*v3 + s*v1 );              R[2][2] = s*s - v1*v1 - v2*v2 + v3*v3;
	
      }*/

      /// Coerces unit norm and returns the rotation matrix corresponding to the resulting unit quaternion
      template<typename Pr>
      inline void RotationMatrix( Pr* R )
      {
	
	// use the normalized verion of the quaternion even ...
	P invnorm = 1.0 / sqrt( s*s + v1*v1 + v2*v2 + v3*v3 );
	s *= invnorm; v1 *= invnorm; v2 *= invnorm; v3 *= invnorm;
	
	R[0]  = s*s + v1*v1 - v2*v2 - v3*v3;    R[1] = 2*( v1*v2 - s*v3 );              R[2] = 2*( v1*v3 + s*v2 );
	R[3] = 2*( v1*v2 + s*v3 );              R[4] = s*s - v1*v1 + v2*v2 - v3*v3;     R[5] = 2*( v2*v3 - s*v1 ); 
	R[6] = 2*( v1*v3 - s*v2 );              R[7] = 2*( v2*v3 + s*v1 );              R[8] = s*s - v1*v1 - v2*v2 + v3*v3;
	
      }
      
      /// Quaternion multiplication
      template<typename Pr>
      inline Quaternion< typename QMulType<P, Pr>::type > Mul(const Quaternion<Pr> &rhs) const
      {
	
	Pr r = rhs.get_s(), z1 = rhs.get_v1(), z2 = rhs.get_v2(), z3 = rhs.get_v3(); 
	
	typedef typename QMulType<P, Pr>::type rType;
	
	double w  = s  * r - v1 * z1 - v2 * z2 - v3 * z3,
	       u1 = v1 * r + s  * z1 - v3 * z2 + v2 * z3,
	       u2 = v2 * r + v3 * z1 + s  * z2 - v1 * z3,
	       u3 = v3 * r - v2 * z1 + v1 * z2 + s  * z3;
	      
	return Quaternion<rType>(w, u1, u2, u3);
      }
      
     /// Scalar multiplication
     template<typename Ps>
     inline Quaternion<typename QMulType<P, Ps>::type> Muls(Ps lscalar) const
     {
  
	typedef typename QMulType<P, Ps>::type retType;
    
    
	return Quaternion<retType>(lscalar * get_s(), 
				   lscalar * get_v1(),
				   lscalar * get_v2(),
				   lscalar * get_v3() 
				  );
     }
      
      // assignment to other quaternion
      template <typename Pq>
      Quaternion& operator =(const Quaternion<Pq> &q) 
      {
	
	s = q.get_s();
	v1 = q.get_v1();
	v2 = q.get_v2();
	v3 = q.get_v3();
		
	return *this;
      }
    
      // assignment to a matrix
      template <typename Precision>
      //Quaternion& operator =( const Precision (&array)[4] ) 
      Quaternion& operator =( const Precision* array) 
      {
	s = array[0];
	v1 = array[1];
	v2 = array[2];
	v3 = array[3];
		
	return *this;
      }
      
     
      
      /// Bracket operator - array access (NOTE: Scalar part is first in the 4-vector!)
      P& operator [](int index) 
      {
	
	assert(index > -1 && index < 4 && "Invalid indexing to quaternion component");
	
	if (index == 0) return s;
	if (index == 1) return v1;
	if (index == 2) return v2;
	
	return v3;
      }
      
      
      
      
      /// Standard vector dot product with another quaternion
      template<typename Pw>
      inline typename QMulType<P, Pw>::type Dot(const Quaternion<Pw> &w) const
      {
	
	Pw r = w.get_s(), 
	   z1 = w.get_v1(), 
	   z2 = w.get_v2(), 
	   z3 = w.get_v3(); 
	
	typedef typename QMulType<P, Pw>::type rType;
	
	rType ret = s * r + v1 * z1 + v2 * z2 + v3 * z3;     
	
	return ret;
	
      }
      
      /// Standard vector dot product with a 4-vector (stored in standard array - consecutive memory locations)
      template<typename Pu>
      //inline typename QMulType<P, Pu>::type dot(const Pu (&u)[4])
      inline typename QMulType<P, Pu>::type dot(const Pu* u) 
      {
	
	typedef typename QMulType<P, Pu>::type rType;
	      
	rType ret =  s * u[0] + v1 * u[1] + v2 * u[2] + v3 * u[3];
	
	return ret;
      }
      
      /// Standard vector dot product with a 4-vector (stored in standard array)
      template<typename Pu>
      inline typename QMulType<P, Pu>::type dot(const Pu u1, 
						const Pu u2, 
						const Pu u3,
						const Pu u4
 					      ) 
      {
	
	typedef typename QMulType<P, Pu>::type rType;
	
	      
	rType ret =  s * u1 + v1 * u2 + v2 * u3 + v3 * u4;
	
	return ret;
      }
      
      
      /// The exponential maps axis-angle vectors to the quaternions
      //template<typename Pu> inline static Quaternion exp(const Pu (&u)[3]);
      template<typename Pu> inline static Quaternion exp(const Pu* u );
      template<typename Pu1,
	       typename Pu2,
	       typename Pu3> 
      inline static Quaternion exp(const Pu1 u1, const Pu2 u2, const Pu3 u3);
      
      /// Create a quaternion from MRPs
      template<typename Ppsi> 
      inline static Quaternion CreateFromMRPs(const Ppsi* psi );
      
      /// Create a quaternion from a Gibbs vector (Classical Rodrigues Parameters)
      template<typename Pg> 
      inline static Quaternion CreateFromGibbs(const Pg* g );
      
      
      /// Quaternion logarithm. Maps back to an axis-angle vector
      //template<typename Pu> inline void ln( Pu (&u)[3] ) const;
      template<typename Pu> inline void ln( Pu* u ) const;
      
      template<typename Pu1, 
	       typename Pu2, 
	       typename Pu3> 
      inline void ln( Pu1 &u1, Pu2 &u2, Pu3 &u3 ) const;
      
      /// Return the MRPs of the quaternion (if not [-1, 0, 0, 0] )
      template<typename Ppsi> inline void MRPCoordinates( Ppsi* psi ) const;
      template<typename Ppsi1,
	       typename Ppsi2,
	       typename Ppsi3> 
      inline void MRPCoordinates( Ppsi1 &psi1, Ppsi2 &psi2, Ppsi3 &psi3 ) const;
      
      /// Returns the Gibbs vector of the quaternion ( retruns a vecty large vector if s = 0)
      template<typename Pg> inline void GibbsVector( Pg* g ) const;
      template<typename Pg1,
	       typename Pg2,
	       typename Pg3> 
      inline void GibbsVector( Pg1 &g1, Pg2 &g2, Pg3 &g3 ) const;
      
      /// Conjugate quaternion
      inline Quaternion<P> Conjugate() { return Quaternion<P>(s, -v1, -v2, -v3); }
      
      
      
      /// Convert a rotation to a quaternion in the "north" hemisphere (as close as possible to [1;0;0;0])
      /*template<typename Pr>
      inline static Quaternion<Pr> Rotation2Quaternion(const Pr (&R)[3][3] ) 
      {
	
	
	Pr s = 0, v1 = 0, v2 = 0, v3 = 0;

	// Taking diferent cases in order to pick the right (solution) quaternion 

        // Case #1: if R(2,2) > -R(3,3) and  R(1,1) > -R(2,2) and R(1,1) > -R(3,3)
        if ( R[1][1] >= -R[2][2] && R[0][0] >= -R[1][1] && R[0][0] >= -R[2][2] ) 
	{
	  //cout <<"Taking 1st case..."<<endl;
	  // 0.5 * Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          s =  0.5 * sqrt(1 + R[0][0] + R[1][1] + R[2][2] );           		  
	  // 0.5 * (R(2,3) - R(3,2))/Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          v1 = -0.5 * ( R[1][2] - R[2][1] ) / sqrt(1 + R[0][0] + R[1][1] + R[2][2] );
	  // 0.5 * (R(3,1) - R(1,3))/Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          v2 = -0.5 * ( R[2][0] - R[0][2] ) / sqrt(1 + R[0][0] + R[1][1] + R[2][2] );    
	  // 0.5 * (R(1,2) - R(2,1))/Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          v3 = -0.5 * ( R[0][1] - R[1][0] ) / sqrt(1 + R[0][0] + R[1][1] + R[2][2] );    
	}
	// Case #2: if ((R(2,2) < -R(3,3)) && (R(1,1) > R(2,2)) && (R(1,1) > R(3,3)))
	else if ( R[1][1] <= -R[2][2] && R[0][0] >= R[1][1] && R[0][0] >= R[2][2] ) 
	{
	  //cout <<"Taking 2nd case..."<<endl;
	  // 0.5 * (R(2,3) - R(3,2)) / Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          s = -0.5 * ( R[1][2] - R[2][1] ) / sqrt(1 + R[0][0] - R[1][1] - R[2][2] );   
	  // 0.5 * Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          v1 =  0.5 * sqrt(1 + R[0][0] - R[1][1] - R[2][2] );                               
	  // 0.5 * (R(1,2) + R(2,1))/Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          v2 =  0.5 * ( R[0][1] + R[1][0] ) / sqrt(1 + R[0][0] - R[1][1] - R[2][2] );   
	  // 0.5 * (R(3,1) + R(1,3))/Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          v3 =  0.5 * ( R[2][0] + R[0][2] ) / sqrt(1 + R[0][0] - R[1][1] - R[2][2] );   
	} 
	//if ((R(2,2) > R(3,3)) && (R(1,1) < R(2,2)) && (R(1,1) < -R(3,3)))
	else if ( R[1][1] >= R[2][2] && R[0][0] <= R[1][1] && R[0][0] <= -R[2][2] ) 
	{
	  //cout <<"Taking 3d case..."<<endl;
	  //0.5 * (R(3,1) - R(1,3))/Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  s = -0.5 * ( R[2][0] - R[0][2] ) / sqrt(1 - R[0][0] + R[1][1] - R[2][2] );     		
	  // 0.5 * (R(1,2) + R(2,1))/Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  v1 =  0.5 * ( R[0][1] + R[1][0] ) / sqrt(1 - R[0][0] + R[1][1] - R[2][2] );     
	  // 0.5 * Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  v2 =  0.5 * sqrt(1 - R[0][0] + R[1][1] - R[2][2] );                    		  		
	  // (R(2,3) + R(3,2))/Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  v3 =  0.5 * ( R[1][2] + R[2][1] ) / sqrt(1 - R[0][0] + R[1][1] - R[2][2] );     
	}
	//if ((R(2,2) < R(3,3)) && (R(1,1) < -R(2,2)) && (R(1,1) < R(3,3)))
	else if ( R[1][1] <= R[2][2] && R[0][0] <= -R[1][1] && R[0][0] <= R[2][2] ) 
	{
	  //cout <<"Taking 4th case..."<<endl;
	  // 0.5 * (R(1,2) - R(2,1))/Sqrt(1 - R(1,1) - R(2,2) + R(3,3))
          s = -0.5 * ( R[0][1] - R[1][0] ) / sqrt(1 - R[0][0] - R[1][1] + R[2][2] );                  
          // 0.5 * (R(3,1) + R(1,3))/Sqrt(1 - R(1,1) - R(2,2) + R(3,3))
          v1 =  0.5 * ( R[2][0] + R[0][2] ) / sqrt(1 - R[0][0] - R[1][1] + R[2][2] );                     
          // 0.5 * ((R(2,3) + R(3,2))/Sqrt(1 - R(1,1) - R(2,2) + R(3,3))
          v2 =  0.5 * ( R[1][2] + R[2][1] ) / sqrt(1 - R[0][0] - R[1][1] + R[2][2] );                   
          // 0.5 * Sqrt(1 - R(1,1) - R(2,2) + R(3,3));
          v3 =  0.5 * sqrt(1 - R[0][0] - R[1][1] + R[2][2] );                
	}

	// Sending the quaternion to the north hemisphere if it was found in the south
	// in order to muster all my rotations closer to q = 1 ...
	if ( s < 0  ) { s = -s; v1 = -v1; v2 = -v2; v3 = -v3;}
	    
	return Quaternion<Pr>(s, v1, v2, v3);
	    
      }*/
      
      /// Convert a rotation to a quaternion in the "north" hemisphere (as close as possible to [1;0;0;0]).
      template<typename Pr>
     inline static Quaternion<Pr> Rotation2Quaternion(const Pr* R )
      {
	
	Pr s = 0, v1 = 0, v2 = 0, v3 = 0;
	
	// Taking diferent cases in order to pick the right (solution) quaternion 

        // Case #1: if R(2,2) > -R(3,3) and  R(1,1) > -R(2,2) and R(1,1) > -R(3,3)
        if ( R[4] >= -R[8] && R[0] >= -R[4] && R[0] >= -R[8] ) 
	{
	  //cout <<"Taking 1st case..."<<endl;
	  // 0.5 * Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          s =  0.5 * sqrt(1 + R[0] + R[4] + R[8] );           		  
	  // 0.5 * (R(2,3) - R(3,2))/Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          v1 = -0.5 * ( R[5] - R[7] ) / sqrt(1 + R[0] + R[4] + R[8] );
	  // 0.5 * (R(3,1) - R(1,3))/Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          v2 = -0.5 * ( R[6] - R[2] ) / sqrt(1 + R[0] + R[4] + R[8] );    
	  // 0.5 * (R(1,2) - R(2,1))/Sqrt(1 + R(1,1) + R(2,2) + R(3,3))
          v3 = -0.5 * ( R[1] - R[3] ) / sqrt(1 + R[0] + R[4] + R[8] );    
	}
	// Case #2: if ((R(2,2) < -R(3,3)) && (R(1,1) > R(2,2)) && (R(1,1) > R(3,3)))
	else if ( R[4] <= -R[8] && R[0] >= R[4] && R[0] >= R[8] ) 
	{
	  //cout <<"Taking 2nd case..."<<endl;
	  // 0.5 * (R(2,3) - R(3,2)) / Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          s = -0.5 * ( R[5] - R[7] ) / sqrt(1 + R[0] - R[4] - R[8] );   
	  // 0.5 * Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          v1 =  0.5 * sqrt(1 + R[0] - R[4] - R[8] );                               
	  // 0.5 * (R(1,2) + R(2,1))/Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          v2 =  0.5 * ( R[1] + R[3] ) / sqrt(1 + R[0] - R[4] - R[8] );   
	  // 0.5 * (R(3,1) + R(1,3))/Sqrt(1 + R(1,1) - R(2,2) - R(3,3))
          v3 =  0.5 * ( R[6] + R[2] ) / sqrt(1 + R[0] - R[4] - R[8] );   
	} 
	//if ((R(2,2) > R(3,3)) && (R(1,1) < R(2,2)) && (R(1,1) < -R(3,3)))
	else if ( R[4] >= R[8] && R[0] <= R[4] && R[0] <= -R[8] ) 
	{
	  //cout <<"Taking 3d case..."<<endl;
	  //0.5 * (R(3,1) - R(1,3))/Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  s = -0.5 * ( R[6] - R[2] ) / sqrt(1 - R[0] + R[4] - R[8] );     		
	  // 0.5 * (R(1,2) + R(2,1))/Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  v1 =  0.5 * ( R[1] + R[3] ) / sqrt(1 - R[0] + R[4] - R[8] );     
	  // 0.5 * Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  v2 =  0.5 * sqrt(1 - R[0] + R[4] - R[8] );                    		  		
	  // (R(2,3) + R(3,2))/Sqrt(1 - R(1,1) + R(2,2) - R(3,3))
	  v3 =  0.5 * ( R[5] + R[7] ) / sqrt(1 - R[0] + R[4] - R[8] );     
	}
	//if ((R(2,2) < R(3,3)) && (R(1,1) < -R(2,2)) && (R(1,1) < R(3,3)))
	else if ( R[4] <= R[8] && R[0] <= -R[4] && R[0] <= R[8] ) 
	{
	  //cout <<"Taking 4th case..."<<endl;
	  // 0.5 * (R(1,2) - R(2,1))/Sqrt(1 - R(1,1) - R(2,2) + R(3,3))
          s = -0.5 * ( R[1] - R[3] ) / sqrt(1 - R[0] - R[4] + R[8] );                  
          // 0.5 * (R(3,1) + R(1,3))/Sqrt(1 - R(1,1) - R(2,2) + R(3,3))
          v1 =  0.5 * ( R[6] + R[2] ) / sqrt(1 - R[0] - R[4] + R[8] );                     
          // 0.5 * ((R(2,3) + R(3,2))/Sqrt(1 - R(1,1) - R(2,2) + R(3,3))
          v2 =  0.5 * ( R[5] + R[7] ) / sqrt(1 - R[0] - R[4] + R[8] );                   
          // 0.5 * Sqrt(1 - R(1,1) - R(2,2) + R(3,3));
          v3 =  0.5 * sqrt(1 - R[0] - R[4] + R[8] );                
	}

	// Sending the quaternion as close to the 1 as possible
	if ( (s - 1)*(s - 1) > (s + 1)*(s + 1) ) { s = -s; v1 = -v1; v2 = -v2; v3 = -v3;}
	    
	return Quaternion<Pr>(s, v1, v2, v3);
	    
      }
      
      
     /* /// Rotation matrix derivative wrt quaternion scalar part (as a 3x3 arrays)
      template<typename Pr>
      inline void RotationJacWRTscalar( Pr (&D)[3][3] ) const 
      {
	  
	D[0][0] =  2 * s;     D[0][1] = -2 * v3;    D[0][2] =  2 * v2;
	D[1][0] =  2 * v3;    D[1][1] =  2 * s;     D[1][2] = -2 * v1;
	D[2][0] = -2 * v2;    D[2][1] =  2 * v1;    D[2][2] =  2 * s;
	
      }
      
      /// Rotation matrix derivative WRT quaternion vector part (3x3 arrays)
      template<typename Pr>
      inline void RotationJacWRTvector( int i, Pr (&D)[3][3] ) const 
      {
	assert(i > -1 && i < 3 && "Quaternion index out-of-bounds");
	
	if (i == 0) 
	{  
	  D[0][0] = 2 * v1;    D[0][1] =  2 * v2;    D[0][2] =  2 * v3;
	  D[1][0] = 2 * v2;    D[1][1] = -2 * v1;    D[1][2] = -2 * s;
	  D[2][0] = 2 * v3;    D[2][1] =  2 * s;     D[2][2] = -2 * v1;
	
	} 
	else if (i == 1) 
	{
	  D[0][0] = -2 * v2;   D[0][1] = 2 * v1;     D[0][2] =  2 * s;
	  D[1][0] =  2 * v1;   D[1][1] = 2 * v2;     D[1][2] =  2 * v3;
	  D[2][0] = -2 * s;    D[2][1] = 2 * v3;     D[2][2] = -2 * v2;
	
	} 
	else // i == 2
	{ 
	  D[0][0] = -2 * v3;   D[0][1] = -2 * s;    D[0][2] = 2 * v1;
	  D[1][0] =  2 * s;    D[1][1] = -2 * v3;   D[1][2] = 2 * v2;
	  D[2][0] =  2 * v1;   D[2][1] =  2 * v2;   D[2][2] = 2 * v3;
	}
	  
      }*/
      
      /// Rotation matrix derivative wrt quaternion scalar part (as a 9-element array)
      //inline void RotationJacWRTscalar( P (&D)[9] ) const 
      template<typename Pr>
      inline void RotationJacWRTscalar( Pr* D ) const 
      {
	  
	D[0] =  2 * s;     D[1] = -2 * v3;    D[2] =  2 * v2;
	D[3] =  2 * v3;    D[4] =  2 * s;     D[5] = -2 * v1;
	D[6] = -2 * v2;    D[7] =  2 * v1;    D[8] =  2 * s;
	
      }
      
      /// Rotation matrix derivative WRT quaternion vector part (3x3 arrays)
      //inline void RotationJacWRTvector( int i, P (&D)[9] ) const 
      template<typename Pr>
      inline void RotationJacWRTvector( int i, Pr* D ) const
      {
	assert(i > -1 && i < 3 && "Quaternion index out-of-bounds");
	
	if (i == 0) 
	{  
	  D[0] = 2 * v1;    D[1] =  2 * v2;    D[2] =  2 * v3;
	  D[3] = 2 * v2;    D[4] = -2 * v1;    D[5] = -2 * s;
	  D[6] = 2 * v3;    D[7] =  2 * s;     D[8] = -2 * v1;
	
	} 
	else if (i == 1) 
	{
	  D[0] = -2 * v2;   D[1] = 2 * v1;     D[2] =  2 * s;
	  D[3] =  2 * v1;   D[4] = 2 * v2;     D[5] =  2 * v3;
	  D[6] = -2 * s;    D[7] = 2 * v3;     D[8] = -2 * v2;
	
	} 
	else // i == 2
	{ 
	  D[0] = -2 * v3;   D[1] = -2 * s;     D[2] = 2 * v1;
	  D[3] =  2 * s;    D[4] = -2 * v3;    D[5] = 2 * v2;
	  D[6] =  2 * v1;   D[7]=   2 * v2;    D[8] = 2 * v3;
	}
	  
      }
      
      /// Rotation matrix derivative wrt quaternion components as tensor elements
      /// 0 : s
      /// 1 : v1
      /// 2 : v2
      /// 3 : v3
      P RotationJacWRTquat(int row, int col, int i) 
      {
	
	assert(i > -1 && i < 5 && row > -1 && row <4 && col >-1 && col <4 && "Quaternion or Rotation matrix index out-of-bounds");
	
	
	switch(i) 
	{
	  case 0 : /* dRdqi(0, 0) = 2 * q0;    dRdqi(0, 1) = 2 * (-q3);  dRdqi(0, 2) = 2 * q2;
		      dRdqi(1, 0) = 2 * q3;    dRdqi(1, 1) = 2 * q0;     dRdqi(1, 2) = 2 * (-q1);
		      dRdqi(2, 0) = 2 * (-q2); dRdqi(2, 1) = 2 * q1;     dRdqi(2, 2) = 2 * q0;
		      */
	    
		    if (row == col)  return 2 * s;
		    else 
		      if ( row + col == 1 ) return ( col == 1 ) ? -2 * v3 : 2 * v3;
		      else 
			if ( row + col == 2 ) return ( col == 0 ) ? -2 * v2 : 2 * v2;
			else
			  if ( row + col == 3 ) return ( col == 2 ) ? -2 * v1 : 2 * v1;
		    
		  break;
	  case 1:
		  /* dRdqi(0, 0) = 2 * q1;    dRdqi(0, 1) = 2 * q2;     dRdqi(0, 2) = 2 * q3;
		     dRdqi(1, 0) = 2 * q2;    dRdqi(1, 1) = 2 * (-q1);  dRdqi(1, 2) = 2 * (-q0);
		     dRdqi(2, 0) = 2 * q3;    dRdqi(2, 1) = 2 * q0;     dRdqi(2, 2) = 2 * (-q1); */
	    
		  if ( row == col ) return ( col == 0 ) ? 2 * v1 : -2 * v1;
		  else 
		    if ( row + col == 1 ) return 2 * v2;
		    else 
		      if ( row + col == 2 ) return 2 * v3;
		      else 
			if ( row + col == 3 ) return (col == 2 ) ? -2 * s : 2 * s; 
	  
		break;
	  
	  case 2: /* dRdqi(0, 0) = 2 * (-q2); dRdqi(0, 1) = 2 * q1;     dRdqi(0, 2) = 2 * q0;
		     dRdqi(1, 0) = 2 * q1;    dRdqi(1, 1) = 2 * q2;     dRdqi(1, 2) = 2 * q3;
		      dRdqi(2, 0) = 2 * (-q0); dRdqi(2, 1) = 2 * q3;     dRdqi(2, 2) = 2 * (-q2); */
	    
		if ( row == col ) return ( col == 1 ) ? 2 * v2 : -2 * v2;
		  else 
		    if ( row + col == 1 ) return 2 * v1;
		    else 
		      if ( row + col == 2 ) return ( col == 2 ) ? 2 * s : -2 * s;
			else 
			  if ( row + col == 3 ) return 2 * v3;
	
		  break;
	  case 3: /* dRdqi(0, 0) = 2 * (-q3); dRdqi(0, 1) = 2 * (-q0);  dRdqi(0, 2) = 2 * q1;
		     dRdqi(1, 0) = 2 * q0;    dRdqi(1, 1) = 2 * (-q3);  dRdqi(1, 2) = 2 * q2;
		     dRdqi(2, 0) = 2 * q1;    dRdqi(2, 1) = 2 * q2;     dRdqi(2, 2) = 2 * q3; */
	        
		if ( row == col ) return ( col == 2 ) ? 2 * v3 : -2 * v3;
		  else 
		    if ( row + col == 1 ) return ( col == 1 ) ? -2 * s : 2 * s;
		    else
		      if (row + col == 2 ) return 2 * v1;
		      else 
			if (row + col == 3 ) return 2 * v2;
	  
	  
	  }
	  
	assert(0 && "Unreachable region!");
	
	return 0;
      }
      
      
      
      
      /*/// Returns the quaternion derivative wrt MRPs in a 3x3 array
      //   with center of projection at [-1 ; 0 ; 0 ; 0] (i.e., the scalar 
      //   part is first in the 4-tuple)
      template<typename Pd>
      inline void QuaternionJacWRTMRPs( Pd (&J)[4][3] ) 
      {
	// derivative of the scalar part
	J[0][0] = -v1 * (1 + s);     J[0][1] = -v2 * (1 + s);         J[0][2] = -v3 * (1 + s);
	
	
	// Derivative of the vector part
	J[1][0] = -v1 * v1 + ( s + 1 );    J[1][1] = -v1 * v2;                  J[1][2] = -v1 * v3;
	J[2][0] = -v2 * v1;           	   J[2][1] = -v2 * v2 + ( s + 1 );      J[2][2] = -v2 * v3;
	J[3][0] = -v3 * v1;           	   J[3][1] = -v3 * v2;                  J[3][2] = -v3 * v3 + ( s + 1 );
	
      }*/
      
      /// Returns the quaternion derivative wrt MRPs in a 12 array
      //   with center of projection at [-1 ; 0 ; 0 ; 0] (i.e., the scalar 
      //   part is first in the 4-tuple)
      template<typename Pd>
      inline void QuaternionJacWRTMRPs( Pd* J ) 
      {
	// derivative of the scalar part
	J[0] = -v1 * (1 + s);     J[1] = -v2 * (1 + s);         J[2] = -v3 * (1 + s);
	
	// Derivative of the vector part
	J[3] = -v1 * v1 + ( s + 1 );    J[4]  = -v1 * v2;                  J[5]  = -v1 * v3;
	J[6] = -v2 * v1;           	J[7]  = -v2 * v2 + ( s + 1 );      J[8]  = -v2 * v3;
	J[9] = -v3 * v1;           	J[10] = -v3 * v2;                  J[11] = -v3 * v3 + ( s + 1 );
	
      }
      
      /*
      /// Returns the quaternion derivative wrt Axis-Angle in a 12 array (3x4)
      template<typename Pd>
      inline void QuaternionJacWRTAA( Pd* J ) 
      {
	// We need the quaternion logarithm:
	P u[3];
	ln(u);
	
	P u1 = u[0], u2 = u[1], u3 = u[2];
	P theta = sqrt( u1*u1 + u2*u2 + u3*u3 );
	P theta_h = theta * 0.5; 
	P inv_theta = 1.0 / theta;
	P inv_theta3 = inv_theta * inv_theta * inv_theta;
	
	// Column #1 
	J[0]  = -0.5 * u1 * sin(theta_h) * inv_theta;
	J[3]  = inv_theta3 * ( theta*theta * sin(theta_h) + 0.5 * u1*u1*theta * cos(theta_h) -u1*u1 * sin(theta_h) ); 
	J[6]  = inv_theta3 * u1*u2 * ( 0.5*theta * cos(theta_h) - sin(theta_h) );
	J[9]  = inv_theta3 * u1*u3 * ( 0.5*theta * cos(theta_h) - sin(theta_h) );
	
	// Column #2
	J[1]  = -0.5 * u2 * sin(theta_h) * inv_theta;
	J[4]  = inv_theta3 * u1*u2 * (0.5* theta*cos(theta_h) - sin(theta_h) );
	J[7]  = inv_theta3 * (theta*theta * sin(theta_h) + 0.5*u2*u2*theta * cos(theta_h) - u2*u2 * sin(theta_h) );
	J[10] = inv_theta3 * u2*u3 * (0.5*theta * cos(theta_h) - sin(theta_h) );
	
	// Column #3
	J[2]  = -0.5 * u3 * sin(theta_h) * inv_theta;
	J[5]  = inv_theta3 * u1*u3 * (0.5*theta * cos(theta_h) - sin(theta_h) );
	J[8]  = inv_theta3 * u2*u3 * (0.5*theta * cos(theta_h) - sin(theta_h) );
	J[11] = inv_theta3 * (theta*theta * sin(theta_h) + 0.5*u3*u3*theta * cos(theta_h) - u3*u3*sin(theta_h) ); 
	
      }
      */
      
      /// Jacobian of quaternion scalar part WRT Axis - Angle parameters
      template<typename Pd>
      inline void QuaternionScalarJacWRTAA( Pd* J ) 
      {
	// Easy: the derivative is -1/2 * v (minus one-half the vector part)
	J[0] = -0.5 * v1; J[1]= -0.5 * v2; J[2] = -0.5 * v3;
      }
      
      /// Jacobian of quaternion vector part WRT Axis - Angle parameters
      template<typename Pd>
      inline void QuaternionVectorJacWRTAA( Pd* J ) 
      {
	// We need the quaternion logarithm:
	P u[3];
	ln(u);
	
	P u1 = u[0], u2 = u[1], u3 = u[2];
	P theta = sqrt( u1*u1 + u2*u2 + u3*u3 );
	
	if (theta < 10e-5)
	{
	  // return the identity if the angle too small
	  J[0] = J[4] = J[8] = 1;
	  J[1] = J[2] = J[3] = J[5] = J[6] = J[7] = 0;
	}
	else 
	{
	  P theta_h = theta * 0.5; 
	  P inv_theta = 1.0 / theta;
	  P inv_theta3 = inv_theta * inv_theta * inv_theta;
	
	
	  // Column #1 
	  J[0]  = inv_theta3 * ( theta*theta * sin(theta_h) + 0.5 * u1*u1*theta * cos(theta_h) -u1*u1 * sin(theta_h) ); 
	  J[3]  = inv_theta3 * u1*u2 * ( 0.5*theta * cos(theta_h) - sin(theta_h) );
	  J[6]  = inv_theta3 * u1*u3 * ( 0.5*theta * cos(theta_h) - sin(theta_h) );
	
	  // Column #2
	  J[1]  = inv_theta3 * u1*u2 * (0.5* theta*cos(theta_h) - sin(theta_h) );
	  J[4]  = inv_theta3 * (theta*theta * sin(theta_h) + 0.5*u2*u2*theta * cos(theta_h) - u2*u2 * sin(theta_h) );
	  J[7] = inv_theta3 * u2*u3 * (0.5*theta * cos(theta_h) - sin(theta_h) );
	
	  // Column #3
	  J[2]  = inv_theta3 * u1*u3 * (0.5*theta * cos(theta_h) - sin(theta_h) );
	  J[5]  = inv_theta3 * u2*u3 * (0.5*theta * cos(theta_h) - sin(theta_h) );
	  J[8] = inv_theta3 * (theta*theta * sin(theta_h) + 0.5*u3*u3*theta * cos(theta_h) - u3*u3*sin(theta_h) ); 
	}
      }
      
      /// Returns the derivative of the i-th quaternion components WRT modfied Rodrigues parameters
      //  (stereographic projection coordinates) with center of projection at [-1 ; 0 ; 0 ; 0]
      template<typename Pd>
      inline void QuaternionScalarJacWRTMRPs(Pd* J) 
      {
	// derivative of the scalar part
	J[0] = -v1 * (1 + s);     J[1] = -v2 * (1 + s);         J[2] = -v3 * (1 + s);
	    
      }
      
      /// Returns the derivative of the quaternion vector part WRT modified Rodrigues parameters
      //  (stereographic projection coordinates) with center of projection at [-1 ; 0 ; 0 ; 0]
      // in a 9-array
      template<typename Pd>
      inline void QuaternionVectorJacWRTMRPs(Pd* J) 
      {
	// Derivative of the vector part
	J[0] = -v1 * v1 + ( s + 1 );    J[1] = -v1 * v2;                  J[2] = -v1 * v3;
	J[3] = -v2 * v1;           	J[4] = -v2 * v2 + ( s + 1 );      J[5] = -v2 * v3;
	J[6] = -v3 * v1;           	J[7] = -v3 * v2;                  J[8] = -v3 * v3 + ( s + 1 );
	
      }
      
     /* /// Returns the derivative of the quaternion vector part WRT modified Rodrigues parameters in a 3x3-array
      template<typename Pd>
      inline void QuaternionVectorJacWRTMRPs(Pd (&J)[3][3]) 
      {
	// Derivative of the vector part
	J[0][0] = -v1 * v1 + ( s + 1 );    J[0][1] = -v1 * v2;                  J[0][2] = -v1 * v3;
	J[1][0] = -v2 * v1;           	   J[1][1] = -v2 * v2 + ( s + 1 );      J[1][2] = -v2 * v3;
	J[2][0] = -v3 * v1;           	   J[2][1] = -v3 * v2;                  J[2][2] = -v3 * v3 + ( s + 1 );
	
      } */
      
      /// Derivatives of rotated vector (R*u) wrt stereographic coordinates
      /// Stores the result in a 3x3 array
      /*template<typename Pu, typename Pd> 
      inline void RotatedVectorJacWRTMRPs(const Pu* u, Pd (&D)[3][3] ) const
      {
	// Hard-coding the multiplication DRDq * DqDpsi * u using the chain rule
	// Column #1 : Derivative wrt psi0
	//  ( DRDq0 * Dq0Dpsi0 + DRDq1 * Dq1Dpsi0 + DRDq2 * Dq2Dpsi0 + DRDq3 * Dq3Dpsi0 ) * u
	
	
	D[0][0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s )           * u[0] +
		  2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) ) * u[1] +
		  2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) ) * u[2];
	    
	
	D[1][0] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) )   * u[0] + 
		  2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) )      * u[1] + 
		  2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) ) * u[2];
	    
	
	D[2][0] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) )   * u[0] +
		  2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) ) * u[1] +
		  2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) ) * u[2];
	
	
	// Column #2: Derivative wrt psi1
	//  ( DRDq0 * Dq0Dpsi1 + DRDq1 * Dq1Dpsi1 + DRDq2 * Dq2Dpsi1 + DRDq3 * Dq3Dpsi1 ) * u
	D[0][1] =  2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) )      * u[0] +
		   2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) )  * u[1] +
	           2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) ) * u[2];
		  
	D[1][1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) ) * u[0] +
		  2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s )       * u[1] + 
		  2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) ) * u[2];
	
	D[2][1] =  2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) ) * u[0] +
		   2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) )   * u[1] +
		   2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) ) * u[2];
	
	// Column #3: Derivative wrt psi2
	//  ( DRDq0 * Dq0Dpsi2 + DRDq1 * Dq1Dpsi2 + DRDq2 * Dq2Dpsi2 + DRDq3 * Dq3Dpsi2 ) * u
	
	D[0][2] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) )   * u[0] + 
		  2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) ) * u[1] +
		  2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) )     * u[2];
	
	D[1][2] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) )   * u[0] +
		  2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) ) * u[1] + 
		  2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) )   * u[2];
	
	D[2][2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) ) * u[0] +
		  2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) ) * u[1] +
		  2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) )   * u[2];
		      
      }*/
      
      
      /// Derivatives of rotated vector (R*u) wrt MRPs
      /// Stores the result in a 9 - array
      template<typename Pu, typename Pd> 
      inline void RotatedVectorJacWRTMRPs(const Pu* u, Pd* D ) const
      {
	// Hard-coding the multiplication DRDq * DqDpsi * u using the chain rule
	// Column #1 : Derivative wrt psi0
	//  ( DRDq0 * Dq0Dpsi0 + DRDq1 * Dq1Dpsi0 + DRDq2 * Dq2Dpsi0 + DRDq3 * Dq3Dpsi0 ) * u
	
	D[0] =  2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s )           * u[0] + 
		  2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) ) * u[1] + 
	          2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) ) * u[2];
		  
	D[3] =   2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) )   * u[0] + 
		   2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) )      * u[1] + 
		   2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) ) * u[2];
	
	D[6] =  2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) )   * u[0] + 
		  2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) ) * u[1] +
		  2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) ) * u[2];
	
	// Column #2: Derivative wrt psi1
	//  ( DRDq0 * Dq0Dpsi1 + DRDq1 * Dq1Dpsi1 + DRDq2 * Dq2Dpsi1 + DRDq3 * Dq3Dpsi1 ) * u
	D[1] =   2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) )      * u[0] +
		   2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) )  * u[1] +
	           2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) ) * u[2];
		  
	D[4] =  2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) ) * u[0] +
		  2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s )       * u[1] + 
		  2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) ) * u[2];
	
	D[7] =   2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) ) * u[0] +
		   2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) )   * u[1] +
		   2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) ) * u[2];
	
	// Column #3: Derivative wrt psi2
	//  ( DRDq0 * Dq0Dpsi2 + DRDq1 * Dq1Dpsi2 + DRDq2 * Dq2Dpsi2 + DRDq3 * Dq3Dpsi2 ) * u
	
	D[2] =  2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) )   * u[0] + 
		  2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) ) * u[1] +
		  2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) )     * u[2];
	
	D[5] =  2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) )   * u[0] +
		  2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) ) * u[1] + 
		  2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) )   * u[2];
	
	D[8] =  2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) ) * u[0] +
		  2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) ) * u[1] +
		  2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) )   * u[2];
		      
      }
      
      
      /// Derivatives of rotated vector by the transposed rotation (R'*u) wrt MRPs
      /// Stores the result in a 3x3 - array
      /*template<typename Pu, typename Pd> 
      inline void RotatedTranspVectorJacWRTMRPs(const Pu* u, Pd (&D)[3][3] ) 
      {
	// Hard-coding the multiplication DRDq * DqDpsi * u using the chain rule
	// Column #1 : Derivative wrt psi0
	//  ( DRtDq0 * Dq0Dpsi0 + DRtDq1 * Dq1Dpsi0 + DRtDq2 * Dq2Dpsi0 + DRtDq3 * Dq3Dpsi0 ) * u
	D[0][0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s )           * u[0] + 
	          2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) ) * u[1] + 
	          2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) )      * u[2];
	       
	D[1][0] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) ) * u[0] + 
		  2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) )    * u[1] + 
		  2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) )    * u[2];
	       
	D[2][0] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) )   * u[0] +
		  2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) ) * u[1] +
		  2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) ) 	 * u[2];
	
	// Column #2: Derivative wrt psi1
	//  ( DRtDq0 * Dq0Dpsi1 + DRtDq1 * Dq1Dpsi1 + DRtDq2 * Dq2Dpsi1 + DRtDq3 * Dq3Dpsi1 ) * u
	D[0][1] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) )     * u[0] +
		  2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) )   * u[1] +
		  2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) ) * u[2];
	
	D[1][1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) ) * u[0] +
		  2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s )         * u[1] + 
		  2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) )   * u[2];
	       
	D[2][1] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) ) * u[0] +
		  2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) )    * u[1] +
		  2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) )  * u[2];
	
	// Column #3: Derivative wrt psi2
	//  ( DRtDq0 * Dq0Dpsi2 + DRtDq1 * Dq1Dpsi2 + DRtDq2 * Dq2Dpsi2 + DRtDq3 * Dq3Dpsi2 ) * u
	
	D[0][2] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) ) * u[0] + 
		  2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) )   * u[1] +
		  2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) )   * u[2];
	       
	D[1][2] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) ) * u[0] +
		  2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) )   * u[1] + 
		  2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) )     * u[2]; 
	       
	D[2][2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) ) * u[0] +
		  2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) ) * u[1] +
		  2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) )   * u[2];
	             
      }*/
       /// Derivatives of rotated vector by the transposed rotation (R'*u) wrt stereographic coordinates
      /// Stores the result in a 9 - array
      template<typename Pu, typename Pd> 
      inline void RotatedTranspVectorJacWRTMRPs(const Pu* u, Pd* D ) 
      {
	// Hard-coding the multiplication DRDq * DqDpsi * u using the chain rule
	// Column #1 : Derivative wrt psi0
	//  ( DRtDq0 * Dq0Dpsi0 + DRtDq1 * Dq1Dpsi0 + DRtDq2 * Dq2Dpsi0 + DRtDq3 * Dq3Dpsi0 ) * u
	D[0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s )           * u[0] + 
	       2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) ) * u[1] + 
	       2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) )      * u[2];
	       
	D[3] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) ) * u[0] + 
	       2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) )    * u[1] + 
	       2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) )    * u[2];
	       
	D[6] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) )   * u[0] +
	       2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) ) * u[1] +
	       2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) ) 	 * u[2];
	
	// Column #2: Derivative wrt psi1
	//  ( DRtDq0 * Dq0Dpsi1 + DRtDq1 * Dq1Dpsi1 + DRtDq2 * Dq2Dpsi1 + DRtDq3 * Dq3Dpsi1 ) * u
	D[1] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) )     * u[0] +
	       2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) )   * u[1] +
	       2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) ) * u[2];
	
	D[4] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) ) * u[0] +
	       2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s )         * u[1] + 
	       2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) )   * u[2];
	       
	D[7] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) ) * u[0] +
	       2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) )    * u[1] +
	       2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) )  * u[2];
	
	// Column #3: Derivative wrt psi2
	//  ( DRtDq0 * Dq0Dpsi2 + DRtDq1 * Dq1Dpsi2 + DRtDq2 * Dq2Dpsi2 + DRtDq3 * Dq3Dpsi2 ) * u
	
	D[2] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) ) * u[0] + 
	       2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) )   * u[1] +
	       2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) )   * u[2];
	       
	D[5] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) ) * u[0] +
	       2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) )   * u[1] + 
	       2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) )     * u[2]; 
	       
	D[8] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) ) * u[0] +
	       2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) ) * u[1] +
	       2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) )   * u[2];
	             
      }
      
      /// Fills the rotation matrix derivatives with respect 
      /// to the i-th stereographic coordinates in a 3x3 array.
      /*template<typename Pd>
      inline void RotationJacWRTMRPs( unsigned int i, Pd (&D)[3][3] ) 
      {
	
	assert(i < 3 && "Invalid stereographic coordinate index (must be 0, 1, 2)");
	
	switch(i) 
	{
	  case 0 : // [dR / dpsi0]
	    
	    // Row #1
	    D[0][0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s );
	    D[0][1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	    D[0][2] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );
	    
	    // Row #2
	    D[1][0] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) ); 
	    D[1][1] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) ); 
	    D[1][2] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );
	    
	    // Row #3
	    D[2][0] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );
	    D[2][1] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );
	    D[2][2] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	
	  break;
	 
	  case 1 : // [dR / dpsi1]
	    
	    // Row #1
	    D[0][0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	    D[0][1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	    D[0][2] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	
	    // Row #2
	    D[1][0] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	    D[1][1] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	    D[1][2] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	
	    // Row #3
	    D[2][0] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	    D[2][1] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );
	    D[2][2] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	  break;
	  
	  case 2: // [dR / dpsi2]
	    
	    // Row #1
	    D[0][0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) );
	    D[0][1] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	    D[0][2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	
	    // Row #2
	    D[1][0] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	    D[1][1] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	    D[1][2] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	
	    // Row #3
	    D[2][0] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );
	    D[2][1] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) );
	    D[2][2] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
	  
	  break;
	  
	  default: assert(0 && "Unreachable region"); break;
	}
	
      }
      */
      
      /// Fills the rotation matrix derivatives with respect 
      /// to the i-th modified Rodriguues parameter in a 9x9 array.
      template<typename Pd>
      inline void RotationJacWRTMRPs( unsigned int i, Pd* D ) 
      {
	
	assert( i < 3 && "Invalid stereographic coordinate index (must be 0, 1, 2)");
	
	switch(i) 
	{
	  case 0 : // [dR / dpsi0]
	    
	    // Row #1
	    D[0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s );
	    D[1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	    D[2] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );
	    
	    // Row #2
	    D[3] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) ); 
	    D[4] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) ); 
	    D[5] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );
	    
	    // Row #3
	    D[6] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );
	    D[7] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );
	    D[8] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	
	  break;
	 
	  case 1 : // [dR / dpsi1]
	    
	    // Row #1
	    D[0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	    D[1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	    D[2] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	
	    // Row #2
	    D[3] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	    D[4] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	    D[5] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	
	    // Row #3
	    D[6] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	    D[7] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );
	    D[8] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	  break;
	  
	  case 2: // [dR / dpsi2]
	    
	    // Row #1
	    D[0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) );
	    D[1] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	    D[2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	
	    // Row #2
	    D[3] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	    D[4] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	    D[5] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	
	    // Row #3
	    D[6] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );
	    D[7] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) );
	    D[8] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
	  
	  break;
	  
	  default: assert(0 && "Unreachable region"); break;
	}
	
      }
      
      
      /// Fills the rotation matrix derivatives in terms of the 3 Modified Rodrigues Parameters.
      /// Each derivative is a matrix and gets stored in  9-arrays D1, D2, D3
      template<typename Pd1, typename Pd2, typename Pd3>
      inline void RotationJacWRTMRPs( Pd1* D1, Pd2* D2, Pd3* D3 ) 
      {
	
	// [dR / dψ1]	    
	// Row #1
	D1[0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s );
	D1[1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	D1[2] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );    
	// Row #2
	D1[3] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) ); 
	D1[4] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) ); 
	D1[5] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );    
	// Row #3
	D1[6] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );
	D1[7] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );
	D1[8] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	
	// [dR / dψ2]
	// Row #1
	D2[0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	D2[1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	D2[2] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	// Row #2
	D2[3] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	D2[4] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	D2[5] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	// Row #3
	D2[6] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	D2[7] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );
	D2[8] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	// [dR / dψ3]    
	// Row #1
	D3[0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) );
	D3[1] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	D3[2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	// Row #2
	D3[3] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	D3[4] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	D3[5] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	// Row #3
	D3[6] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );
	D3[7] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) );
	D3[8] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
		
      }
      
      /*/// Fills the rotation matrix derivatives in terms of the 3 Modified Rodrigues Parameters.
      /// Each derivative is a matrix and gets stored in  3x3-arrays D1, D2, D3
      template<typename Pd1, typename Pd2, typename Pd3>
      inline void RotationJacWRTMRPs( Pd1 (&D1)[3][3], Pd2 (&D2)[3][3], Pd3 (&D3)[3][3] ) 
      {
	// [dR / dψ1]	    
	// Row #1
	D1[0][0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s );
	D1[0][1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	D1[0][2] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );    
	// Row #2
	D1[1][0] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) ); 
	D1[1][1] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) ); 
	D1[1][2] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );    
	// Row #3
	D1[2][0] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );
	D1[2][1] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );
	D1[2][2] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	
	// [dR / dψ2]
	// Row #1
	D2[0][0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	D2[0][1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	D2[0][2] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	// Row #2
	D2[1][0] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	D2[1][1] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	D2[1][2] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	// Row #3
	D2[2][0] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	D2[2][1] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );
	D2[2][2] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	// [dR / dψ3]    
	// Row #1
	D3[0][0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) );
	D3[0][1] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	D3[0][2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	// Row #2
	D3[1][0] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	D3[1][1] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	D3[1][2] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	// Row #3
	D3[2][0] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );
	D3[2][1] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) );
	D3[2][2] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
		
      }
      */
      
      /*/// Stores the Jacobian of the transposed rotation matrix wrt MRP coordinates in a 3x3 array
      template<typename Pd>
      inline void RotationTranspJacWRTMRPs( int i, Pd (&D)[3][3] )
      {
	
	assert(i > -1 && i < 3 && "Invalid MRP index");
	
	switch(i) 
	{
	  case 0 : // [dR' / dψ0]
	    // Row #1
	    D[0][0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s ); 
	    D[0][1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) );
	    D[0][2] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );
	    
	    // Row #2
	    D[1][0] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	    D[1][1] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) );
	    D[1][2] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );
	       
	    // Row #3
	    D[2][0] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );
	    D[2][1] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );
	    D[2][2] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	    
	    break;
	    
	  case 1: // [dR' / dψ1]
	    
	    // Row #1
	    D[0][0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	    D[0][1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	    D[0][2] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	
	    // Row #2
	    D[1][0] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	    D[1][1] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	    D[1][2] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );
	       
	    // Row #3
	    D[2][0] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	    D[2][1] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	    D[2][2] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	    break;
	  
	  case 2: // [dR' / dψ2]
	  
	    // Row #1
	    D[0][0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) ); 
	    D[0][1] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	    D[0][2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );
	       
	    // Row #2
	    D[1][0] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	    D[1][1] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	    D[1][2] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) ); 
	       
	    // Row #3
	    D[2][0] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	    D[2][1] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	    D[2][2] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
	
	    break;
	    
	  default: // should not be here1
		   assert(0 && "Unreachable region!");
	    break;
	}
	
      }*/
      
      /// Fills the rotation matrix derivatives in terms of the 3 Modified Rodrigues Parameters.
      /// The derivative is a 3D vector and gets stored in a 3-array, dr
      template<typename Pd>
      inline void RotationElementJacWRTMRPs( unsigned int r, unsigned int c, Pd* dr ) 
      {
	assert(r < 3 && c < 3 && "Invalid rotation matrix indexing!");
	
	switch(r)
	{
	  case 0: 
	    // Row #1
	    switch(c)
	    {
	      case 0:   // [dr11 / dψ]
		dr[0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s );
		dr[1] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) );
		dr[2] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) );
		break;
	      
	      case 1: // [dr12 / dψ]
		dr[0] = 2 * ( v2 * (-2*v1*v1 + s + 1) + v1*v3 * (2*s + 1) );
		dr[1] = 2 * ( v1 * (-2*v2*v2 + s + 1) + v2*v3 * (2*s + 1) );
		dr[2] = 2 * ( -s * (-2*v3*v3 + s + 1) +    v3 * (v3 - 2*v1*v2) );
		break;
	      
	      case 2: // [dr13 / dψ]
		dr[0] = 2 * ( v3 * (-2*v1*v1 + s + 1) - v1*v2 * (2*s + 1) );
		dr[1] = 2 * (  s * (-2*v2*v2 + s + 1) -    v2 * (v2 + 2*v1*v3) );
		dr[2] = 2 * ( v1 * (-2*v3*v3 + s + 1) - v2*v3 * (2*s + 1) );
		break;
	    }
	 break;
	    
	    case 1 :
		// Row #2
	      switch(c) 
	      {
		case 0: // [dr21 / dψ]
		  dr[0] = 2 * ( v2 * (-2*v1*v1 + s + 1) - v1*v3 * (2*s + 1) );
		  dr[1] = 2 * ( v1 * (-2*v2*v2 + s + 1) - v2*v3 * (2*s + 1) );
		  dr[2] = 2 * (  s * (-2*v3*v3 + s + 1) -    v3 * (v3 + 2*v1*v2) );
		  break;
		
		case 1: // [dr22 / dψ]
		  dr[0] = 2 * v1 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
		  dr[1] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
		  dr[2] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
		  break;
		
		case 2: // [dr23 / dψ]
		  dr[0] = 2 * ( -s * (-2*v1*v1 + s + 1) +    v1 * (v1 - 2*v2*v3) );
		  dr[1] = 2 * ( v3 * (-2*v2*v2 + s + 1) + v1*v2 * (2*s + 1) );
		  dr[2] = 2 * ( v2 * (-2*v3*v3 + s + 1) + v1*v3 * (2*s + 1) );
		  break;
		  
	      }
	  break;
	  
		case 2 : 
		  // row #3
		  switch(c)
		  {
		    case 0: // [dr31 / dψ]
		      dr[0] = 2 * ( v3 * (-2*v1*v1 + s + 1) + v1*v2 * (2*s + 1) );
		      dr[1] = 2 * ( -s * (-2*v2*v2 + s + 1) +    v2 * (v2 - 2*v1*v3) );
		      dr[2] = 2 * ( v1 * (-2*v3*v3 + s + 1) + v2*v3 * (2*s + 1) );
		      break;
		    
		    case 1: // [dr32 / dψ]
		      dr[0] = 2 * (  s * (-2*v1*v1 + s + 1) - v1 * (v1 + 2*v2*v3) );
		      dr[1] = 2 * ( v3 * (-2*v2*v2 + s + 1) -v1*v2 * (2*s + 1) );
		      dr[2] = 2 * ( v2 * (-2*v3*v3 + s + 1) -v1*v3 * (2*s + 1) );
		      break;
		      
		    case 2: // [dr33 / dψ]
		      dr[0] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 - (1 + s)*(1 + s) );
		      dr[1] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (1 + s)*(1 + s) );
		      dr[2] = 2 * v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s );
		      break;
		    
		  }
	  break;
	}

	
      }
    
      /// Stores the Jacobian of the transposed rotation matrix wrt MRPs in a 9-array
      template<typename Pd>
      inline void RotationTranspJacWRTMRPs( unsigned int i, Pd* D )
      {
	assert( i < 3 && "Invalid MRP index");
	
	switch(i) 
	{
	  case 0 : // [dR' / dψ0]
	    // Row #1
	    D[0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s ); 
	    D[1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) );
	    D[2] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );
	    
	    // Row #2
	    D[3] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	    D[4] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) );
	    D[5] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );
	       
	    // Row #3
	    D[6] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );
	    D[7] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );
	    D[8] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	    
	    break;
	    
	  case 1: // [dR' / dψ1]
	    
	    // Row #1
	    D[0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	    D[1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	    D[2] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	
	    // Row #2
	    D[3] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	    D[4] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	    D[5] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );
	       
	    // Row #3
	    D[6] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	    D[7] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	    D[8] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	    break;
	  
	  case 2: // [dR' / dψ2]
	  
	    // Row #1
	    D[0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) ); 
	    D[1] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	    D[2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );
	       
	    // Row #2
	    D[3] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	    D[4] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	    D[5] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) ); 
	       
	    // Row #3
	    D[6] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	    D[7] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	    D[8] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
	    
	    break;
	
	  default: // should not be here1
		   assert(0 && "Unreachable region!");
	    break;
	}
	
      }
    
      /// Stores the Jacobian tensor of the transposed rotation matrix wrt MRPs in 3 9-arrays
      template<typename Pd1, typename Pd2, typename Pd3>
      inline void RotationTranspJacWRTMRPs( Pd1* D1, Pd2* D2, Pd3* D3 )
      {
	
	// [dR' / dψ1]
	// Row #1
	D1[0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s ); 
	D1[1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) );
	D1[2] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );    
	// Row #2
	D1[3] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	D1[4] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) );
	D1[5] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );       
	// Row #3
	D1[6] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );
	D1[7] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );
	D1[8] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	    
	// [dR' / dψ1]
	// Row #1
	D2[0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	D2[1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	D2[2] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	// Row #2
	D2[3] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	D2[4] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	D2[5] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );       
	// Row #3
	D2[6] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	D2[7] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	D2[8] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	// [dR' / dψ3]
	// Row #1
	D3[0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) ); 
	D3[1] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	D3[2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );       
	// Row #2
	D3[3] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	D3[4] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	D3[5] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) );        
	// Row #3
	D3[6] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	D3[7] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	D3[8] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
	
      }
    
    
      /*/// Stores the Jacobian tensor of the transposed rotation matrix wrt MRP coordinates in 3 3x3-arrays
      template<typename Pd1, typename Pd2, typename Pd3>
      inline void RotationTranspJacWRTMRPs( Pd1 (&D1)[3][3], Pd2 (&D2)[3][3], Pd3 (&D3)[3][3] )
      {
	
	// [dR' / dψ1]
	// Row #1
	D1[0][0] = 2 * v1 * ( -v1*v1 + v2*v2 + v3*v3 + 1 - s*s ); 
	D1[0][1] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) - v1*v3 * ( 2*s + 1 ) );
	D1[0][2] = 2 * ( v3 * ( -2*v1*v1 + s + 1) + v1*v2*(2*s + 1) );    
	// Row #2
	D1[1][0] = 2 * ( v2 * ( -2*v1*v1 + s + 1 ) + v1*v3 * ( 2*s + 1 ) );
	D1[1][1] = 2 * v1 * ( -v2*v2 +v1*v1 + v3*v3 - (s + 1)*(s + 1) );
	D1[1][2] = 2 * ( s * ( -2*v1*v1 + s + 1 ) - v1*(v1 + 2*v2*v3) );       
	// Row #3
	D1[2][0] = 2 * ( v3 * ( -2*v1*v1 + s + 1 ) - v1*v2 * ( 2*s + 1 ) );
	D1[2][1] = 2 * ( -s * ( -2*v1*v1 + s + 1 ) + v1 * ( v1 - 2*v2*v3 ) );
	D1[2][2] = 2 * v1 * ( -v3*v3 + v1*v1 + v2*v2 -(1 + s)*(1 + s) );
	    
	// [dR' / dψ1]
	// Row #1
	D2[0][0] = 2 * v2 * ( -v1*v1 + v2*v2 + v3*v3 - (1+s)*(1+s) );
	D2[0][1] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) - v2*v3*(2*s + 1) );
	D2[0][2] = 2 * ( -s * ( -2*v2*v2 + s + 1 ) + v2*(v2 - 2*v1*v3) );
	// Row #2
	D2[1][0] = 2 * ( v1 * ( -2*v2*v2 + s + 1 ) + v2*v3*( 2*s + 1 ) );
	D2[1][1] = 2 * v2 * ( -v2*v2 + v1*v1 + v3*v3 + 1 - s*s );
	D2[1][2] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) - v1*v2*(2*s + 1) );       
	// Row #3
	D2[2][0] = 2 * ( s * ( -2*v2*v2 + s + 1 ) - v2*( v2 + 2*v1*v3 ) );
	D2[2][1] = 2 * ( v3 * ( -2*v2*v2 + s + 1 ) + v1*v2*(2*s + 1) );
	D2[2][2] = 2 * v2 * ( -v3*v3 + v1*v1 + v2*v2 - (s + 1)*(s + 1) );
	
	// [dR' / dψ3]
	// Row #1
	D3[0][0] = 2 * v3 * ( -v1*v1 + v2*v2 + v3*v3 - (1 + s)*(1 + s) ); 
	D3[0][1] = 2 * ( s * ( -2*v3*v3 + s + 1 ) -v3*(v3 + 2*v1*v2) );
	D3[0][2] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) + v2*v3*(2*s + 1) );       
	// Row #2
	D3[1][0] = 2 * ( -s * ( -2*v3*v3 + s + 1 ) + v3 * (v3 - 2*v1*v2) );
	D3[1][1] = 2 * v3 * ( -v2*v2 + v1*v1 + v3*v3 - (1 + s)*(1 + s) );
	D3[1][2] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) - v1*v3*(2*s + 1) );        
	// Row #3
	D3[2][0] = 2 * ( v1 * ( -2*v3*v3 + s + 1 ) - v2*v3*(2*s + 1) );
	D3[2][1] = 2 * ( v2 * ( -2*v3*v3 + s + 1 ) + v1*v3*(2*s + 1) );
	D3[2][2] = 2 * ( v3 * ( -v3*v3 + v1*v1 + v2*v2 + 1 - s*s ) );
	
      }
      */
      /// Update a quaternion from a perturbation δ in its MRPs
      /// Use this function to avoid unnecessary conversions to parameters
      template<typename Pdelta>
      inline Quaternion<P> UpdateFromDeltaMRPs( const Pdelta* delta )
      {
	P v_dot_delta = v1 * delta[0] + v2 * delta[1] +  v3 * delta[2];
	P sq_norm_delta = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
	P factor = 1.0 / ( 1 + v_dot_delta + 0.5 * ( 1 + s ) * sq_norm_delta );
	
	P s_new = factor * ( s - v_dot_delta - 0.5 * ( 1 + s ) * sq_norm_delta );
	P v1_new = factor * ( v1 + ( 1 + s ) * delta[0] );
	P v2_new = factor * ( v2 + ( 1 + s ) * delta[1] );
	P v3_new = factor * ( v3 + ( 1 + s ) * delta[2] );
	
	Quaternion<P> ret;
	ret[0] = s_new;
	ret[1] = v1_new;
	ret[2] = v2_new;
	ret[3] = v3_new;
	
	return ret;
	
      }
      
      /// Update the quaternion from a perturbation δ in its MRPs
      /// Use this function to avoid unnecessary conversions to parameters
      template<typename Pdelta1, 
	       typename Pdelta2, 
	       typename Pdelta3>
      inline Quaternion<P> UpdateFromDeltaMRPs( Pdelta1 delta1, Pdelta2 delta2, Pdelta3 delta3 )
      {
	P v_dot_delta = v1 * delta1 + v2 * delta2 +  v3 * delta3;
	P sq_norm_delta = delta1 * delta1 + delta2 * delta2 + delta3 * delta3;
	P factor = 1.0 / ( 1 + v_dot_delta + 0.5 * ( 1 + s ) * sq_norm_delta );
	
	P s_new = factor * ( s - v_dot_delta - 0.5 * ( 1 + s ) * sq_norm_delta );
	P v1_new = factor * ( v1 + ( 1 + s ) * delta1 );
	P v2_new = factor * ( v2 + ( 1 + s ) * delta2 );
	P v3_new = factor * ( v3 + ( 1 + s ) * delta3 );
	
	Quaternion<P> ret;
	ret[0] = s_new;
	ret[1] = v1_new;
	ret[2] = v2_new;
	ret[3] = v3_new;
	
	return ret;
      }
      
       ///////////////////////////// Interpolation ///////////////////////////////////
      
      // Spherical Linear intERPolation (SLERP)
      template<typename P0, typename P1, typename Pt>
      inline static Quaternion< typename QMulType<P0, typename QMulType<P1, Pt>::type >::type > SLERP(const Quaternion<P0> &q0, 
												      const Quaternion<P1> &q1,
												      Pt t
												      ) 
      {
	// Get a better name for the return type 
	typedef typename QMulType<P0, typename QMulType<P1, Pt>::type>::type RetType;
	
	// The margin for near-zero approximation
	const RetType epsilon = 0.00001;
	
	// Compute the cosine of the angle between the two quaternions
	RetType cosPhi = q0.Dot(q1);
	
	
	// Now get the angle betwen q0 and q1
	RetType Phi = acos ( cosPhi );
	
	// Case #1 : If very small angle, then do linear interpolation
	if (cosPhi > 1 - epsilon)
	{
	  // the angle is very small and interpolation is practically linear
	  Quaternion<RetType> res =  q0.Muls( (1 - t) ) + q1.Muls( t );
	  // normalize
	  res.unit();
	  
	  return res;
	}
	// Case #2: cosPhi > -1 + ε (i.e., we are NOT close to π)
	if (cosPhi > -1 + epsilon ) 
	{
	  auto invSinPhi = 1.0 / sin( Phi );
	
	  return q0.Muls( sin( (1 - t) * Phi ) * invSinPhi ) +  q1.Muls( sin( t * Phi ) * invSinPhi );
	}
	
	// We are too close to π, so simply return q0
	return q0;
	
      }
      
      
  };
  
  
  
  
      
  /// Returns a quaternion from MRPs
  template<typename Precision>
  template<typename Ppsi>
  inline Quaternion<Precision> Quaternion<Precision>::CreateFromMRPs( const Ppsi* psi ) 
  {
    
    const Precision normsq = psi[0]*psi[0] + psi[1]*psi[1] + psi[2]*psi[2];
    const Precision inv_one_plus_normsq = 1.0 / (1 + normsq);
    
    Precision v1_ = 2 * psi[0] * inv_one_plus_normsq,
	      v2_ = 2 * psi[1] * inv_one_plus_normsq,
	      v3_ = 2 * psi[2] * inv_one_plus_normsq,
	      s_ = (1 - normsq) * inv_one_plus_normsq;

      return Quaternion<Precision>(s_, v1_, v2_, v3_);    
  }
      
  
  /// Returns a quaternion from a Gibbs vector
  template<typename Precision>
  template<typename Pg>
  inline Quaternion<Precision> Quaternion<Precision>::CreateFromGibbs( const Pg* g ) 
  {
    
    // There exist two antipodal quaternions for each Gobbs vector:
    // 
    // q1 = +[g; 1] / sqrt(1 + g'*g)
    // q1 = -[g; 1] / sqrt(1 + g'*g)
    // 
    // We choose the one that is closer to 1.
    
    
    const Precision s_ = 1.0 / sqrt(1 + g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);
    Precision v1_ = g[0] * s_, 
	      v2_ = g[1] * s_, 
	      v3_ = g[2] * s_;
		    
    
      if ( (s_ - 1)*(s_ - 1) < (s_ + 1)*(s_ + 1) )
      {
	return Quaternion<Precision>(s_, v1_, v2_, v3_);    
      }
      
      return Quaternion<Precision>(-s_, -v1_, -v2_, -v3_);    
  }
  
  
  
  /// Returns a quaternion from an axis-angle vector 
  template<typename Precision>
  template<typename Pu>
  //inline Quaternion<Precision> Quaternion<Precision>::exp( const Pu (&u)[3] ) 
  inline Quaternion<Precision> Quaternion<Precision>::exp( const Pu* u ) 
  {	
    // obtaining the norm of u (i.e., the angle of the rotation)
    Precision theta_sq = u[0] * u[0] + u[1] * u[1] + u[2] * u[2];
    Precision theta = sqrt(theta_sq);
	
    const Precision inv_theta = 1.0 / theta;
	
    // NOTE: This idea comes from TooN! 
    //	 Since we need to divide by the angle,
    //       it is wise to approximate sin(theta/2) near zero
    //       in order to avoid numerical issues
    //
    //       So, for values less than 10-4 we use the formula
    //              sin(theta/2) / theta ~=  0.5 - (1/48)*theta_sq
    const Precision coeff = theta_sq / 48.0;
	
    if (theta < 0.0001) 
    {
      return Quaternion<Precision>( cos(0.5 * theta) , coeff * u[0], coeff * u[1], coeff * u[2]);
    }
    
    return Quaternion<Precision>( cos(0.5 * theta) , 
				  sin(0.5 * theta) * inv_theta * u[0], 
				  sin(0.5 * theta) * inv_theta * u[1],
				  sin(0.5 * theta) * inv_theta * u[2]
				);
  }
  
  
  template<typename Precision>
  template<typename Pu1, typename Pu2, typename Pu3>
  inline Quaternion<Precision> Quaternion<Precision>::exp( const Pu1 u1, const Pu2 u2, const Pu3 u3 ) 
  {	
    // obtaining the norm of u (i.e., the angle of the rotation)
    Precision theta_sq = u1 * u1 + u2 * u2 + u3 * u3;
    Precision theta = sqrt(theta_sq);
	
    const Precision inv_theta = 1.0 / theta;
	
    // NOTE: This idea comes from TooN! 
    //	 Since we need to divide by the angle,
    //       it is wise to approximate sin(theta/2) near zero
    //       in order to avoid numerical issues
    //
    //       So, for values less than 10-4 we use the formula
    //              sin(theta/2) / theta ~=  0.5 - (1/48)*theta_sq
    const Precision coeff = theta_sq / 48.0;
	
    if (theta < 0.0001) 
    {
      return Quaternion<Precision>( cos(0.5 * theta) , coeff * u1, coeff * u2, coeff * u3);
    }
    
    
    return Quaternion<Precision>( cos(0.5 * theta) , 
				    sin(0.5 * theta) * inv_theta * u1, 
				    sin(0.5 * theta) * inv_theta * u2,
				    sin(0.5 * theta) * inv_theta * u3
				    );
    
  }
  
  template<typename Precision>
  template<typename Ppsi>
  inline void Quaternion<Precision>::MRPCoordinates( Ppsi* psi ) const
  {
    
    //assert( s != -1 && "Quaternion is the South Pole!");
    Ppsi s_per = s;
    // perturbing s a little to the side in order to get finite coordinates
    if ( fabs(s_per + 1 ) < 0.000001 ) s_per = -0.99999;
    
    const Ppsi inv_one_plus_s = 1.0 / (1 + s_per);
    
    psi[0] = (Ppsi)v1 * inv_one_plus_s; 
    psi[1] = (Ppsi)v2 * inv_one_plus_s; 
    psi[2] = (Ppsi)v3 * inv_one_plus_s;
    
  }

  template<typename Precision>
  template<typename Ppsi1, 
	   typename Ppsi2, 
	   typename Ppsi3>
  inline void Quaternion<Precision>::MRPCoordinates( Ppsi1 &psi1, Ppsi2 &psi2, Ppsi3 &psi3 ) const
  {
    
    //assert( s != -1 && "Quaternion is the South Pole!");
    Precision s_per = s;
    // perturbing s a little to the side in order to get finite coordinates
    if ( fabs(s_per + 1 ) < 0.000001 ) s_per = -0.99999;
    
    const Precision inv_one_plus_s = 1.0 / (1 + s_per);
    
    psi1 = v1 * inv_one_plus_s; 
    psi2 = v2 * inv_one_plus_s; 
    psi3 = v3 * inv_one_plus_s;
    
  }
  
  template<typename Precision>
  template<typename Pg>
  inline void Quaternion<Precision>::GibbsVector( Pg* g ) const
  {
    
    assert( fabs(s) > 10e-10 && "Quaternion scalar part is 0!");
    
    const Pg inv_s = 1.0 / s;
    
    g[0] = v1 * inv_s; 
    g[1] = v2 * inv_s; 
    g[2] = v3 * inv_s;
    
  }

  template<typename Precision>
  template<typename Pg1, 
	   typename Pg2, 
	   typename Pg3>
  inline void Quaternion<Precision>::GibbsVector( Pg1 &g1, Pg2 &g2, Pg3 &g3 ) const
  {
    
    assert( fabs(s) > 10e-10 && "Quaternion scalar part is 0!");
    
    const Precision inv_s = 1.0 / s;
    
    g1 = v1 * inv_s; 
    g2 = v2 * inv_s; 
    g3 = v3 * inv_s;
    
  }
  
  /// The quaternion logarithm: NOTE: It returns the rotation angle and NOT the actual logarithm, which is half the rotation angle/
  template<typename Precision>
  template<typename Pu>
  inline void Quaternion<Precision>::ln(Pu* u ) const
  {
   // computing angle in [0, pi]
   Precision theta = 2 * acos(s);
   
   if ( fabs(theta) > 10e-5 ) 
   {  
     const Pu inv_sin_div_theta = theta / sin(0.5 * theta);
     u[0] = (Pu)v1 * inv_sin_div_theta;
     u[1] = (Pu)v2 * inv_sin_div_theta;
     u[2] = (Pu)v3 * inv_sin_div_theta;
       
   } 
   else 
   {  
      const Pu inv_apr_sin_div_theta = 1.0 / ( 0.5 - theta * theta / 48.0 );
      u[0] = (Pu)v1 * inv_apr_sin_div_theta;
      u[1] = (Pu)v2 * inv_apr_sin_div_theta;
      u[2] = (Pu)v3 * inv_apr_sin_div_theta;
   }
   
  }
  
  template<typename Precision>
  template<typename Pu1, 
	   typename Pu2, 
	   typename Pu3>
  inline void Quaternion<Precision>::ln(Pu1 &u1, Pu2 &u2, Pu3 &u3 ) const
  {
   // computing angle in [0, pi]
   Precision theta = 2 * acos(s);
   
   if ( fabs(theta) > 10e-5 ) 
   {  
     const Precision inv_sin_div_theta = theta / sin(0.5 * theta);
     u1 = v1 * inv_sin_div_theta;
     u2 = v2 * inv_sin_div_theta;
     u3 = v3 * inv_sin_div_theta;
       
   } 
   else 
   {  
      const Precision inv_apr_sin_div_theta = 1.0 / ( 0.5 - theta * theta / 48.0 );
      u1 = v1 * inv_apr_sin_div_theta;
      u2 = v2 * inv_apr_sin_div_theta;
      u3 = v3 * inv_apr_sin_div_theta;
   }
   
  }
  
  
  /// Quaternion addition
  template<typename Pl, typename Pr>
  inline Quaternion<typename Quaternion<>::QAddType<Pl, Pr>::type> operator +(const Quaternion<Pl> &ql, const Quaternion<Pr> &qr) 
  {
  
    typedef typename Quaternion<>::QAddType<Pl, Pr>::type retType;
    
    
    return Quaternion<retType>( ql.get_s() + qr.get_s(),
				ql.get_v1() + qr.get_v1(),
				ql.get_v2() + qr.get_v2(),
				ql.get_v3() + qr.get_v3()
		               );
				
  }
  
  ///Quaternion negation
  template <typename Pq>
  inline Quaternion<Pq> operator -(const Quaternion<Pq> &q) 
  {
	
    return Quaternion<Pq>( -q.get_s(), -q.get_v1(), -q.get_v2, -q.get_v3() );
	  
  }
  
  ///Quaternion subtraction
  template <typename Pl, typename Pr>
  inline Quaternion<typename Quaternion<>::QSubType<Pl, Pr>::type> operator -(const Quaternion<Pl> &lhs, const Quaternion<Pr> &rhs) 
  {
	
    typedef typename Quaternion<>::QSubType<Pl, Pr>::type retType;
    
    return Quaternion<retType>( lhs.get_s() - rhs.get_s(), 
				lhs.get_v1() - rhs.get_v1(), 
				lhs.get_v2() - rhs.get_v2(),
				lhs.get_v3() - rhs.get_v3() );
	  
  }
  
  
  
  
  /// Write a quaternion to a stream 
  template <typename Precision>
  inline std::ostream& operator << (std::ostream& os, const Quaternion<Precision> &q) 
  {

      return os << "["<<q.get_s()<<", "<<q.get_v1()<<", "<<q.get_v2()<<", "<<q.get_v3()<<"]";
  }

   /*
   /// Read a quaternion from a stream 
   template <typename Precision>
   inline std::istream& operator >>(std::istream& is, const Quaternon<Precision> &q) {
	
      is >> stuff...;
      return is;
    }
    */


}


#endif