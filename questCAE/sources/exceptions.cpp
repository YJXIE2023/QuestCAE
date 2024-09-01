/*-----------------------------------------
exceptions.hpp类实现代码
------------------------------------------*/


//系统头文件
#include <sstream>

//项目头文件
#include "includes/exceptions.hpp"

namespace Quest{

    Exception::Exception():std::exception(),mMessage("Unknown Error"),mCallStack(){
        updateWhat();
    }

    Exception::Exception(const std::string& rWhat):std::exception(),mMessage(rWhat),mCallStack(){
        updateWhat();
    }

    Exception::Exception(const std::string& rWhat, const CodeLocation& Location):std::exception(),mMessage(rWhat),mCallStack(){
        addToCallStack(Location);
        updateWhat();
    }

    Exception::Exception(const Exception& Other):std::exception(Other),mMessage(Other.mMessage),mWhat(Other.mWhat),mCallStack(Other.mCallStack){}

    Exception::~Exception() noexcept {}

    void Exception::appendMessage(const std::string& rMessage){
        mMessage.append(rMessage);
        updateWhat();
    }

    void Exception::addToCallStack(const CodeLocation& Location){
        mCallStack.push_back(Location);
        updateWhat();
    }

    void Exception::updateWhat(){
        std::stringstream buffer;
        buffer<<mMessage<<std::endl;
        if(mCallStack.empty())
            buffer<<"in Unknown Location";
        else{
            buffer<<"in "<<mCallStack[0]<<std::endl;
            for (auto i = mCallStack.begin()+1; i != mCallStack.end(); ++i)
                buffer<<"    "<<*i<<std::endl;
        }
        mWhat = buffer.str();
    }

    const char* Exception::what() const noexcept{ return mWhat.c_str(); }

    const CodeLocation Exception::where() const{
        if(mCallStack.empty())
            return CodeLocation("Unknown File","Unknown Location",0);
        return mCallStack[0];
    }

    const std::string& Exception::message() const{return mMessage;}

    std::string Exception::info() const{return "Exception";}

    void Exception::printInfo(std::ostream& Ostream) const{ Ostream<<info();}

    void Exception::printData(std::ostream& Ostream) const{
        Ostream<<"Error: "<<mMessage<<std::endl;
        Ostream<<"    in"<<where();
    }

    Exception& Exception::operator<<(const CodeLocation& TheLocation){
        addToCallStack(TheLocation);
        return *this;
    }

    Exception& Exception::operator<<(const char* rString){
        appendMessage(rString);
        return *this;
    }

    Exception& Exception::operator<<(std::ostream& (*rFunc)(std::ostream&)){
        std::stringstream buffer;
        rFunc(buffer);
        appendMessage(buffer.str());
        return *this;
    }

    std::istream& operator>>(std::istream& Istream, Exception& TheException){ return Istream; }

    std::ostream& operator<<(std::ostream& Ostream, const Exception& TheException){
        TheException.printInfo(Ostream);
        Ostream<<std::endl;
        TheException.printData(Ostream);
        
        return Ostream;
    }
}