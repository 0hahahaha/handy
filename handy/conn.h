#pragma once
#include "handy.h"

namespace handy {

//Tcp���ӣ�ʹ�����ü���
struct TcpConn: public std::enable_shared_from_this<TcpConn> {
    //Tcp���ӵ��ĸ�״̬
    enum State { Invalid, Handshaking, Connected, Closed, Failed, };
    //Tcp���캯����ʵ�ʿ��õ�����Ӧ��ͨ��createConnection����
    TcpConn();
    virtual ~TcpConn();
    //�ɴ����������ͣ���������ָ��
    template<class C=TcpConn> static TcpConnPtr createConnection(EventBase* base, const std::string& host, short port, int timeout=0) {
        TcpConnPtr con(new C); con->isClient_ = true; return con->connect(base, host, port, timeout) ? NULL : con; 
    }
    template<class C=TcpConn> static TcpConnPtr createConnection(EventBase* base, int fd, Ip4Addr local, Ip4Addr peer) {
        TcpConnPtr con(new C); con->attach(base, fd, local, peer); return con; 
    }

    bool isClient() { return isClient_; }
    //automatically managed context. allocated when first used, deleted when destruct
    template<class T> T& context() { return ctx_.context<T>(); }

    EventBase* getBase() { return channel_ ? channel_->getBase() : NULL; }
    State getState() { return state_; }
    //TcpConn���������������
    Buffer& getInput() { return input_; }
    Buffer& getOutput() { return output_; }

    Channel* getChannel() { return channel_; }
    bool writable() { return channel_ ? channel_->writeEnabled(): false; }

    //��������
    void sendOutput() { send(output_); }
    void send(Buffer& msg);
    void send(const char* buf, size_t len);
    void send(const std::string& s) { send(s.data(), s.size()); }
    void send(const char* s) { send(s, strlen(s)); }

    //���ݵ���ʱ�ص�
    void onRead(const TcpCallBack& cb) { readcb_ = cb; };
    //��tcp��������дʱ�ص�
    void onWritable(const TcpCallBack& cb) { writablecb_ = cb;}
    //tcp״̬�ı�ʱ�ص�
    void onState(const TcpCallBack& cb) { statecb_ = cb; }
    //tcp���лص�
    void addIdleCB(int idle, const TcpCallBack& cb);

    //��Ϣ�������
    void setCodec(CodecBase* codec) { codec_.reset(codec); }
    //��Ϣ�ص����˻ص���onRead�ص�ֻ��һ����Ч�������õ���Ч
    void onMsg(const MsgCallBack& cb);
    //������Ϣ
    void sendMsg(Slice msg);

    //cleanupNowָ���Ƿ�����������ص�channel��
    void close(bool cleanupNow=false);

    //Զ�̵�ַ���ַ���
    std::string str() { return peer_.toString(); }
public:
    Channel* channel_;
    Buffer input_, output_;
    Ip4Addr local_, peer_;
    State state_;
    TcpCallBack readcb_, writablecb_, statecb_;
    std::list<IdleId> idleIds_;
    AutoContext ctx_, internalCtx_;
    bool isClient_;
    std::unique_ptr<CodecBase> codec_;
    void handleRead(const TcpConnPtr& con);
    void handleWrite(const TcpConnPtr& con);
    ssize_t isend(const char* buf, size_t len);
    void cleanup(const TcpConnPtr& con);
    int connect(EventBase* base, const std::string& host, short port, int timeout);
    void attach(EventBase* base, int fd, Ip4Addr local, Ip4Addr peer);
    virtual int readImp(int fd, void* buf, size_t bytes) { return ::read(fd, buf, bytes); }
    virtual int writeImp(int fd, const void* buf, size_t bytes) { return ::write(fd, buf, bytes); }
    virtual int handleHandshake(const TcpConnPtr& con);
};

//Tcp������
struct TcpServer {
    //abort if bind failed
    TcpServer(EventBases* bases, const std::string& host, short port);
    ~TcpServer() { delete listen_channel_; }
    Ip4Addr getAddr() { return addr_; }
    void onConnCreate(const std::function<TcpConnPtr()>& cb) { createcb_ = cb; }
    void onConnRead(const TcpCallBack& cb) { readcb_ = cb; }
private:
    EventBase* base_;
    EventBases* bases_;
    Ip4Addr addr_;
    Channel* listen_channel_;
    TcpCallBack readcb_;
    std::function<TcpConnPtr()> createcb_;
    void handleAccept();
};

}
