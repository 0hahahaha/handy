#pragma once
#include "slice.h"
#include "net.h"
namespace handy {

struct CodecBase {
    // > 0 ������������Ϣ����Ϣ����msg�У�������ɨ����ֽ���
    // == 0 ����������Ϣ
    // < 0 ��������
    virtual int tryDecode(Slice data, Slice& msg) = 0;
    virtual void encode(Slice msg, Buffer& buf) = 0;
};

//��\r\n��β����Ϣ
struct LineCodec: public CodecBase{
    virtual int tryDecode(Slice data, Slice& msg);
    virtual void encode(Slice msg, Buffer& buf);
};

//�������ȵ���Ϣ
struct LengthCodec:public CodecBase {
    virtual int tryDecode(Slice data, Slice& msg);
    virtual void encode(Slice msg, Buffer& buf);
};

};
