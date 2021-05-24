#include <cstring>
#include <memory>
#include <cmath>
#include "../include/IVector.h"
#include "../include/ILogger.h"

namespace {
    class VectorImpl : public IVector {
    public:
        VectorImpl(size_t dim);

        IVector* clone() const;
        double const* getData() const;

        RC setData(size_t dim, double const* const& ptr_data) override;
        RC getCord(size_t index, double& val) const;
        RC setCord(size_t index, double val);
        RC scale(double multiplier);
        size_t getDim() const;

        RC inc(IVector const* const& op);
        RC dec(IVector const* const& op);

        double norm(NORM n) const;

        RC applyFunction(const std::function<double(double)>& fun);
        RC foreach(const std::function<void(double)>& fun) const;

        size_t sizeAllocated() const;
        static ILogger* logg;
        ~VectorImpl();
    private:
        size_t d;
    };
};

ILogger* VectorImpl::logg = nullptr;

VectorImpl::VectorImpl(size_t dim): d(dim){}

IVector* VectorImpl::clone() const{
    uint8_t* mem = new uint8_t[sizeof(VectorImpl) + d * sizeof(double)];
    IVector* result = new ((uint8_t*)mem) VectorImpl(d);
    memcpy((uint8_t*)result + sizeof(VectorImpl), (uint8_t*)this + sizeof(VectorImpl), d * sizeof(double));
    return (IVector*)result;
}

double const* VectorImpl::getData() const{
    double* data = (double*)((uint8_t*)this + sizeof(VectorImpl));
    return data;
}

RC VectorImpl::setData(size_t dim, double const* const& ptr_data){
    if(dim == 0){
        if(logg){
            logg->log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::MISMATCHING_DIMENSIONS;
    }
    if(ptr_data == nullptr){
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    double* data = new ((uint8_t*)this + sizeof(VectorImpl)) double[dim];
    if(data == nullptr){
        if(logg){
            logg->log(RC::ALLOCATION_ERROR, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::ALLOCATION_ERROR;
    }
    this->d = dim;
    memcpy((uint8_t*)data, (uint8_t*) ptr_data, dim * sizeof(double));
    return RC::SUCCESS;
}

RC VectorImpl::getCord(size_t index, double& val) const{
    if(index >= d){
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    val = ((double*)((uint8_t*)this + sizeof(VectorImpl)))[index];
    return RC::SUCCESS;
}

RC VectorImpl::setCord(size_t index, double val){
    if(index >= d){
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    if(std::isnan(val)){
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    ((double*)((uint8_t*)this + sizeof(VectorImpl)))[index] = val;
    return RC::SUCCESS;
}

RC VectorImpl::scale(double multiplier){
    if(multiplier == INFINITY){
        if(logg){
            logg->log(RC::INFINITY_OVERFLOW, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INFINITY_OVERFLOW;
    }
    if(std::isnan(multiplier)){
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    double* data = (double*)((uint8_t*)this + sizeof(VectorImpl));
    for(size_t i = 0; i < d; i++){
        if(fabs(data[i] * multiplier) == INFINITY){
            if(logg){
                logg->log(RC::INFINITY_OVERFLOW, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
            }
            return RC::INFINITY_OVERFLOW;
        }
        data[i] *= multiplier;
    }
    return RC::SUCCESS;
}

size_t VectorImpl::getDim() const{
    return d;
}

RC VectorImpl::inc(IVector const* const& op){
    if(op->getDim() != d){
        if(logg){
            logg->log(RC::INDEX_OUT_OF_BOUND, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INDEX_OUT_OF_BOUND;
    }
    double* data1 = (double*)((uint8_t*)this + sizeof(VectorImpl));
    double* data2 = (double*)((uint8_t*)op + sizeof(VectorImpl));
    for(size_t i = 0; i < d; i++){
        data1[i] += data2[i];
    }
    return RC::SUCCESS;
}

RC VectorImpl::dec(IVector const* const& op){
    if(op->getDim() != d){
        if(logg){
            logg->log(RC::INDEX_OUT_OF_BOUND, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INDEX_OUT_OF_BOUND;
    }
    double* data1 = (double*)((uint8_t*)this + sizeof(VectorImpl));
    double* data2 = (double*)((uint8_t*)op + sizeof(VectorImpl));
    for(size_t i = 0; i < d; i++){
        data1[i] -= data2[i];
    }
    return RC::SUCCESS;
}

double VectorImpl::norm(NORM n) const{
    double result = 0;
    double* data = (double*)((uint8_t*)this + sizeof(VectorImpl));
    switch(n){
    case NORM::FIRST:
        for(size_t i = 0; i < d; i++){
            result += data[i];
        }
    case NORM::SECOND:
        for(size_t i = 0; i < d; i++){
            result += data[i];
        }
        result = sqrt(result);
        return result;
    case NORM::CHEBYSHEV:
        result = data[0];
        for(size_t i = 1; i < d; i++){
            if(data[i] > result){
                result = data[i];
            }
        }
        return result;
    default:
        return NAN;
    }
}

RC VectorImpl::applyFunction(const std::function<double(double)>& fun){
    if(fun == NULL){
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    for(size_t i = 0; i < d; i++){
       double num;
       this->getCord(i, num);
       this->setCord(i, fun(num));
    }
    return RC::SUCCESS;
}

RC VectorImpl::foreach(const std::function<void(double)>& fun) const{
    if(fun == nullptr)
        if(logg){
            logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    for(size_t i = 0; i < d; i++){
        double num;
        this->getCord(i, num);
        fun(num);
    }
    return RC::SUCCESS;
}

size_t VectorImpl::sizeAllocated() const{
    size_t _size = sizeof(VectorImpl) + d * sizeof(double);
    return _size;
}

IVector* IVector::createVector(size_t dim, double const* const& ptr_data){
    //VectorImpl::logg = ILogger::createLogger();
    if(dim == 0){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return nullptr;
    }
    if(ptr_data == nullptr){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return nullptr;
    }
    uint8_t* mem = new uint8_t[sizeof(VectorImpl) + dim * sizeof(double)];
    IVector* result = new ((uint8_t*)mem) VectorImpl(dim);
    double* data = new ((uint8_t*)mem + sizeof(VectorImpl)) double[dim];
    memcpy((double*)data, (double*)ptr_data, dim * sizeof(double));
    return result;
}

RC IVector::copyInstance(IVector* const dest, IVector const* const& src){
    if(dest == src){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::ALLOCATION_ERROR, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::ALLOCATION_ERROR;
    }
    if(dest->getDim() != src->getDim()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    if(src == nullptr){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    if(dest == nullptr){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    memcpy((uint8_t*)dest + sizeof(VectorImpl), (uint8_t*)src + sizeof(VectorImpl), src->getDim() * sizeof(double));
    return RC::SUCCESS;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src){
    if(dest->getDim() != src->getDim()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    if(dest->sizeAllocated() != src->sizeAllocated()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    if(src == nullptr){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    if(dest == nullptr){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return RC::INVALID_ARGUMENT;
    }
    memmove((uint8_t*)dest, (uint8_t*)src, src->sizeAllocated());
    delete src;
    src = nullptr;
    return RC::SUCCESS;
}

IVector* IVector::add(IVector const* const& op1, IVector const* const& op2){
    if(op1->getDim() != op2->getDim()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return nullptr;
    }
    IVector* result = op1->clone();
    result->inc(op2);
    return result;
}

IVector* IVector::sub(IVector const* const& op1, IVector const* const& op2){
    if(op1->getDim() != op2->getDim()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return nullptr;
    }
    IVector* result = op1->clone();
    result->dec(op2);
    return result;
}

RC IVector::setLogger(ILogger* const logger){
    delete VectorImpl::logg;
    VectorImpl::logg = logger;
    return RC::SUCCESS;
}

double IVector::dot(IVector const* const& op1, IVector const* const& op2){
    double result = 0.;
    if(op1->getDim() != op2->getDim()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return NAN;
    }
    const double* arr1 = op1->getData();
    const double* arr2 = op2->getData();
    for(size_t i = 0; i < op1->getDim(); i++){
        result += arr1[i] * arr2[i];
        if(result == INFINITY){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INFINITY_OVERFLOW, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
            return NAN;
        }
    }
    return result;
}

bool IVector::equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol){
    if(op1->getDim() != op2->getDim()){
        if(VectorImpl::logg){
            VectorImpl::logg->log(RC::INVALID_ARGUMENT, ILogger::Level::WARNING, __FILE__, __FUNCTION__, __LINE__);
        }
        return false;
    }

    IVector* time_vec = sub(op1, op2);
    if(time_vec->norm(n) > tol){
        return false;
    }
    return true;
}

VectorImpl::~VectorImpl() {}

IVector::~IVector(){}
