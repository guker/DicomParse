#ifndef _DICOM_BUFFER_H_
#define _DICOM_BUFFER_H_

#include <vector>
#include <malloc.h>
#include "DICOMTypes.h"

class DICOMBuffer
{
public:
    DICOMBuffer()
    {
        data_.resize(0);
        cur_pos_ = 0;
    }

    ~DICOMBuffer()
    {
        data_.resize(0);
        cur_pos_ = 0;
    }

    void resize(size_t size)
    {
        data_.resize(size);
    }

    void reserve(size_t size)
    {
        data_.reserve(size);
    }

    size_t size()
    {
        return data_.size();
    }

    void set(const char* buf,size_t count)
    {
        if(count > data_.size())
        {
            resize(count);
        }
        memcpy(&data_[0],buf,count*sizeof(char));
        cur_pos_ = 0;
    }
    
    void reset()
    {
        data_.resize(0);
        cur_pos_ = 0;
    }

    size_t Tell()
    {
        return cur_pos_;
    }

    void SkipToPos(size_t pos)
    {
        cur_pos_ = pos;
    }

    void Skip(size_t increment)
    {
        cur_pos_ += increment;
    }

    void SkipToStart()
    {
        cur_pos_ = 0;
    }

    void Read(void* ptr, size_t nbytes)
    {
        char* src = &data_[0] + cur_pos_;
        memcpy(ptr,src,nbytes*sizeof(char));
        cur_pos_ += nbytes;
    }

    doublebyte ReadDoubleByte() 
    {
        doublebyte sh = 0;
        this->Read(&sh,sizeof(doublebyte));
        // 这里检测大小端
        return (sh);
    }

    doublebyte ReadDoubleByteAsLittleEndian()
    {
        doublebyte  sh = 0;
        this->Read(&sh,sizeof(doublebyte));
        return (sh);
    }

    quadbyte  ReadQuadByte()
    {
        quadbyte sh;
        this->Read(&sh,sizeof(quadbyte));
        //  这里检测大小端
        return (sh);
    }
private:
    std::vector<char> data_;
    size_t            cur_pos_;
};

#endif