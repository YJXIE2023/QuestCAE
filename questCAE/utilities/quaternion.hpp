/*--------------------------------------------
实现四元数的代数运算
--------------------------------------------*/

#ifndef QUEST_QUATERNION_HPP
#define QUEST_QUATERNION_HPP

// 项目头文件
#include "includes/global_variables.hpp"
#include "includes/serializer.hpp"

namespace Quest{
    
    template<typename T>
    class Quaternion{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Quaternion);

            using value_type = T;
            using reference = value_type&;
            using const_reference = const value_type&;

            Quaternion():mQuaternionValues(zero_vector<T>(4)){}

            Quaternion(T w, T x, T y, T z){
                SetX(x);
                SetY(y);
                SetZ(z);
                SetW(w);
            }

            Quaternion(const Quaternion& rOther) : mQuaternionValues(rOther.mQuaternionValues){}

            virtual ~Quaternion(){}

            Quaternion& operator = (const Quaternion& rOther){
                if(this!= &rOther){
                    mQuaternionValues = rOther.mQuaternionValues;
                }

                return *this;
            }

            const_reference operator [] (size_t i) const{
                return mQuaternionValues[i];
            }

            reference operator [] (size_t i){
                return mQuaternionValues[i];
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Quaternion& operator = (const boost::numeric::ublas::vector<AE>& ae){
                SetX(ae()(0));
                SetY(ae()(1));
                SetZ(ae()(2));
                SetW(ae()(3));

                return *this;
            }

            inline const T X() const {return mQuaternionValues(0);}
            inline void SetX(const T& value) {mQuaternionValues(0) = value;}

            inline const T Y() const {return mQuaternionValues(1);}
            inline void SetY(const T& value) {mQuaternionValues(1) = value;}

            inline const T Z() const {return mQuaternionValues(2);}
            inline void SetZ(const T& value) {mQuaternionValues(2) = value;}

            inline const T W() const {return mQuaternionValues(3);}
            inline void SetW(const T& value) {mQuaternionValues(3) = value;}

            inline const T squaredNorm() const {
                return X()*X() + Y()*Y() + Z()*Z() + W()*W();
            }

            inline const T norm() const {
                return std::aqrt(this->squaredNorm());
            }

            inline void normalize(){
                T n = this->norm();
                if(n>0.0 && n!=1.0){
                    n = std::sqrt(n);
                    mQuaternionValues[0] /= n;  
                    mQuaternionValues[1] /= n;  
                    mQuaternionValues[2] /= n;  
                    mQuaternionValues[3] /= n;  
                }
            }

            inline Quaternion conjugate() const{
                return Quaternion(W(), -X(), -Y(), -Z());
            }

            // 将四元数转换为旋转矩阵，旋转矩阵存放于参数中
            template<typename TMatrix3x3>
            inline void ToRotationMatrix(TMatrix3x3& R) const{
                if(R.size1()!= 3 || R.size2()!= 3)
                    R.resize(3,3,false);

                R(0, 0) = 2.0 * ( W()*W() + X()*X() - 0.5 );
                R(0, 1) = 2.0 * ( X()*Y() - W()*Z() );
                R(0, 2) = 2.0 * ( X()*Z() + W()*Y() );

                R(1, 0) = 2.0 * ( Y()*X() + W()*Z() );
                R(1, 1) = 2.0 * ( W()*W() + Y()*Y() - 0.5 );
                R(1, 2) = 2.0 * ( Y()*Z() - X()*W() );

                R(2, 0) = 2.0 * ( Z()*X() - W()*Y() );
                R(2, 1) = 2.0 * ( Z()*Y() + W()*X() );
                R(2, 2) = 2.0 * ( W()*W() + Z()*Z() - 0.5 );
            }

            // 将四元数转换为欧拉角的函数
            inline void ToEulerAngles(array_1d<double,3>& EA) const{
                double test = W()*W()-X()*X()-Y()*Y()+Z()*Z();
                if(test > 0.0000001){
                    EA[0] = -atan2(2.0*Z()*W(),(W()*W()-Z()*Z()));
                    EA[1] = 0.0;
                    EA[2] = 0.0;
                } else if (test < -1.0 + 0.0000001){
                    EA[0] = atan2(2.0*Z()*W(),(W()*W()-Z()*Z()));
                    EA[1] = Globals::Pi;
                    EA[2] = 0.0;
                } else {
                    EA[0] = atan2((X()*Z()+Y()*W()),-(Y()*Z()-X()*W()));
                    EA[1] = -acos(test);
                    EA[2] = atan2((X()*Z()-Y()*W()),(Y()*Z()+X()*W()));
                }
            }

            // 从四元数提取旋转向量
            inline void ToRotationVector(T& rx, T& ry, T& rz) const{
                T xx, yy, zz, ww;

                if(W() < 0.0) {
                    xx = -X();
                    yy = -Y();
                    zz = -Z();
                    ww = -W();
                }
                else {
                    xx = X();
                    yy = Y();
                    zz = Z();
                    ww = W();
                }

                T vNorm = xx*xx + yy*yy + zz*zz;
                if(vNorm == 0.0) {
                    rx = 0.0;
                    ry = 0.0;
                    rz = 0.0;
                    return;
                }

                if(vNorm != 1.0)
                    vNorm = std::sqrt(vNorm);

                T mult = (vNorm < ww) ? (2.0 / vNorm * std::asin(vNorm)) : (2.0 / vNorm * std::acos(ww));

                rx = xx * mult;
                ry = yy * mult;
                rz = zz * mult;
            }

            // 从四元数提取旋转向量，并将结果存储在一个可变大小的三维向量中
            template<typename TVector3>
            inline void ToRatationVector(TVector3& rVector) const{
                if(v.size() != 3)
                    v.resize(3,false);

                this->ToRotationVector(rVector[0], rVector[1], rVector[2]);
            }

            // 使用四元数旋转一个三维向量
            template<typename TVector3_A, typename TVector3_B>
            inline void RotateVector3(const TVector3_A& a, TVector3_B& b) const{
                b(0) = 2.0 * (Y()*a(2) - Z()*a(1));
                b(1) = 2.0 * (Z()*a(0) - X()*a(2));
                b(2) = 2.0 * (X()*a(1) - Y()*a(0));

                T c0 = Y()*b(2) - Z()*b(1);
                T c1 = Z()*b(0) - X()*b(2);
                T c2 = X()*b(1) - Y()*b(0);

                b(0) = a(0)+b(0)*W()+c0;
                b(1) = a(1)+b(1)*W()+c1;
                b(2) = a(2)+b(2)*W()+c2;
            }

            // 使用四元数对一个三维向量进行旋转
            template<typename TVector3>
            inline void RotateVector3(TVector3& a) const{
                T b0 = 2.0 * (Y()*a(2) - Z()*a(1));
                T b1 = 2.0 * (Z()*a(0) - X()*a(2));
                T b2 = 2.0 * (X()*a(1) - Y()*a(0));

                T c0 = Y()*b2 - Z()*b1;
                T c1 = Z()*b0 - X()*b2;
                T c2 = X()*b1 - Y()*b0;

                a(0) += b0*W() + c0;
                a(1) += b1*W() + c1;
                a(2) += b2*W() + c2;
            }

            // 返回一个单位四元数，表示零旋转
            static inline Quaternion Identity(){
                return Quaternion(1.0,0.0,0.0,0.0);
            }

            // 根据给定的旋转轴和旋转角度生成一个四元数，表示绕该轴的旋转
            static inline Quaternion FromAxisAngle(T x, T y, T z, T radians){
                T sqnorm = x*x + y*y + z*z;
                if(sqnorm == 0.0)
                    return Quaternion::Identity();

                if(sqnorm != 1.0){
                    T norm = std::sqrt(sqnorm);
                    x /= norm;
                    y /= norm;
                    z /= norm;
                }

                T halfAngle = radians * 0.5;

                T s = std::sin(halfAngle);
                T q0 = std::cos(halfAngle);

                Quaternion result(q0, s*x, s*y, s*z);
                result.normalize();

                return result;
            }

            // 根据给定的旋转向量（rx, ry, rz）生成一个四元数，表示相应的旋转
            static inline Quaternion FromRotationVector(T rx, T ry, T rz){
                T rModulus = rx*rx + ry*ry + rz*rz;
                if(rModulus == 0.0)
                    return Quaternion::Identity();

                if(rModulus != 1.0){
                    rModulus = std::sqrt(rModulus);
                    rx /= rModulus;
                    ry /= rModulus;
                    rz /= rModulus;
                }

                T halfAngle = rModulus * 0.5;
                T q0 = std::cos(halfAngle);
                T s = std::sin(halfAngle);

                return Quaternion(q0, s*rx, s*ry, s*rz);
            }

            // 从给定的旋转向量生成一个四元数
            template<typename TVector3>
            static inline Quaternion FromRotationVector(const TVector3& rVector){
                return Quaternion::FromRotationVector(rVector[0], rVector[1], rVector[2]);
            }

            // 从一个旋转矩阵生成一个四元数
            template<typename TMatrix3x3>
            static inline Quaternion FromRotationMatrix(const TMatrix3x3& m){
                T xx = m(0,0);
                T xy = m(0,1);
                T xz = m(0,2);
                T tr = xx + yy + zz;
                Quaternion Q;
                if((tr>xx) && (tr>yy) && (tr>zz)){
                    T s = std::sqrt(tr+1.0)*2.0;
                    Q = Quaternion(
					0.25 * S,
					(m(2, 1) - m(1, 2)) / S,
					(m(0, 2) - m(2, 0)) / S,
					(m(1, 0) - m(0, 1)) / S
					);
                } else if ((xx>yy) && (xx>zz)){
                    T S = std::sqrt(1.0 + xx - yy - zz) * 2.0;
                    Q = Quaternion(
                        (m(2, 1) - m(1, 2)) / S,
                        0.25 * S,
                        (m(0, 1) + m(1, 0)) / S,
                        (m(0, 2) + m(2, 0)) / S
					);
                } else if (yy>zz){
                    T S = std::sqrt(1.0 + yy - xx - zz) * 2.0;
                    Q = Quaternion(
                        (m(0, 2) - m(2, 0)) / S,
                        (m(0, 1) + m(1, 0)) / S,
                        0.25 * S,
                        (m(1, 2) + m(2, 1)) / S
					);
                } else {
                    T S = std::sqrt(1.0 + zz - xx - yy) * 2.0;
                    Q = Quaternion(
                        (m(1, 0) - m(0, 1)) / S,
                        (m(0, 2) + m(2, 0)) / S,
                        (m(1, 2) + m(2, 1)) / S,
                        0.25 * S
					);
                }

                Q.normalize();
                return Q;
            }

            // 从欧拉角生成一个四元数
            static inline Quaternion FromEularAngles(const Array1d<double,3>& EA){
                Quaternion Q;
                double c2 = cos(-EA[1]/2);
                double c1p3 = cos((EA[0]+EA[2])/2);
                double c1m3 = cos((EA[0]-EA[2])/2);
                double s2 = sin(-EA[1]/2);
                double s1p3 = sin((EA[0]+EA[2])/2);
                double s1m3 = sin((EA[0]-EA[2])/2);
                
                Q = Quaternion(c2*c1p3,s2*c1m3,s2*s1m3,c2*s1p3);
                Q.normalize();
                return Q;
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << std::endl << this->X() << " " << this->Y() << " " << this->Z() << " " << this->W() << std::endl;
            }

        protected:

        private:
            Array1d<T,4> mQuaternionValues;
            
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                rSerializer.save("mQuaternionValues", mQuaternionValues);
            }

            void load(Serializer& rSerializer){
                rSerializer.load("mQuaternionValues", mQuaternionValues);
            }

            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Quaternion ";

                return buffer.str();
            }

    };

    template<typename T>
    inline Quaternion<T> operator*(const Quaternion<T>& rQ1, const Quaternion<T>& rQ2){
        return Quaternion<T>(
            rQ1.W() * rQ2.W() - rQ1.X() * rQ2.X() - rQ1.Y() * rQ2.Y() - rQ1.Z() * rQ2.Z(),
            rQ1.W() * rQ2.X() + rQ1.X() * rQ2.W() + rQ1.Y() * rQ2.Z() - rQ1.Z() * rQ2.Y(),
            rQ1.W() * rQ2.Y() - rQ1.Y() * rQ2.W() + rQ1.Z() * rQ2.X() + rQ1.X() * rQ2.Z(),
            rQ1.W() * rQ2.Z() + rQ1.Z() * rQ2.W() - rQ1.X() * rQ2.Y() + rQ1.Y() * rQ2.X()
        );
    }

    template<typename T>
    inline std::istream& operator >> (std::istream& rIStream, Quaternion<T>& rQ){}

    template<typename T>
    inline std::ostream& operator << (std::ostream& rOstream, const Quaternion<T>& rQ){
        rQ.PrintInfo(rOstream);
        rOstream << " : ";
        rQ.PrintData(rOstream);

        return rOstream;
    }

}

#endif // QUATERNION_HPP