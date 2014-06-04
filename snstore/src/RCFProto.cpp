
//******************************************************************************
// RCF - Remote Call Framework
//
// Copyright (c) 2005 - 2013, Delta V Software. All rights reserved.
// http://www.deltavsoft.com
//
// RCF is distributed under dual licenses - closed source or GPL.
// Consult your particular license for conditions of use.
//
// If you have not purchased a commercial license, you are using RCF 
// under GPL terms.
//
// Version: 2.0
// Contact: support <at> deltavsoft.com 
//
//******************************************************************************

#include "RCFProto.hpp"

using namespace google::protobuf;

namespace RCF {

#if RCF_PRO==1

bool isProBuild()
{
    return true;
}

#else

bool isProBuild()
{
    return false;
}

#endif

// Internal RCF interface for sending and receiving PB-serialized buffers.
RCF_BEGIN(I_Pb, "I_Pb")

    RCF_METHOD_R3(
        RCF::ByteBuffer, 
            DoProtoRpc, 
                const std::string &,    // service
                int,                    // method id
                RCF::ByteBuffer)        // Protobuf request

RCF_END(I_Pb)


// Resets the RpcController to its initial state so that it may be reused in
// a new call.  Must not be called while an RPC is in progress.
void RcfProtoController::Reset()
{
    mpRcfChannel = NULL;
    mpRcfContext = NULL;
}

RcfProtoController::RcfProtoController() : mpRcfChannel(NULL), mpRcfContext(NULL)
{
}

RcfProtoController::RcfProtoController(RcfProtoSession * context) : 
    mpRcfChannel(NULL), 
    mpRcfContext(context)
{
}

RcfProtoSession * RcfProtoController::getSession()
{
    return mpRcfContext;
}

RcfProtoChannel * RcfProtoController::getChannel()
{
    return mpRcfChannel;
}

bool RcfProtoController::Failed() const
{
    return mpRcfChannel->Failed();
}

bool RcfProtoController::Completed() const
{
    return mpRcfChannel->Completed();
}

std::string RcfProtoController::ErrorText() const
{
    return mpRcfChannel->ErrorText();
}

void RcfProtoController::StartCancel()
{
    mpRcfChannel->StartCancel();
}

void RcfProtoController::SetFailed(const std::string& reason)
{
    mpRcfContext->SetFailed(reason);
}

bool RcfProtoController::IsCanceled() const
{
    return mpRcfContext->IsCanceled();
}

void RcfProtoController::NotifyOnCancel(Closure* callback)
{
    mpRcfContext->NotifyOnCancel(callback);
}

//------------------------------------------------------------------------------
// Client side

_SwigCallback::_SwigCallback()
{
}

_SwigCallback::~_SwigCallback()
{
}

void _SwigCallback::Run()
{
    RCF_ASSERT( 0 && "Not implemented." );
}

void _SwigCallback::ProtoRpcBegin(
    _SwigCallbackArgs * pArgs,
    RcfProtoServer * server,
    RcfProtoSession * context, 
    const std::string & serviceName, 
    int methodId)
{
    RCF_ASSERT( 0 && "Not implemented." );
}

bool _SwigCallback::ValidateCertificate(
    _SwigCallbackArgs * args)
{
    RCF_ASSERT( 0 && "Not implemented." );
    return false;
}

// After a call has finished, returns true if the call failed.  The possible
// reasons for failure depend on the RPC implementation.  Failed() must not
// be called before a call has finished.  If Failed() returns true, the
// contents of the response message are undefined.
bool RcfProtoChannel::Failed()
{
    bool async = mAsyncRpcMode;
    if (async)
    {
        RCF_ASSERT(mFuture.ready());
        std::auto_ptr<RCF::Exception> ePtr = mFuture.getAsyncException();
        if (ePtr.get())
        {
            mError = *ePtr;
        }
    }

    if (mError.bad())
    {
        return true;
    }

    return false;
}

bool RcfProtoChannel::Completed()
{
    return mFuture.ready();
}

// If Failed() is true, returns a human-readable description of the error.
std::string RcfProtoChannel::ErrorText()
{
    bool async = mAsyncRpcMode;
    if (async)
    {
        RCF_ASSERT(mFuture.ready());
        std::auto_ptr<RCF::Exception> ePtr = mFuture.getAsyncException();
        if (ePtr.get())
        {
            mError = *ePtr;
        }
    }
    return mError.getErrorString();
}

// Advises the RPC system that the caller desires that the RPC call be
// canceled.  The RPC system may cancel it immediately, may wait awhile and
// then cancel it, or may not even cancel the call at all.  If the call is
// canceled, the "done" callback will still be called and the RpcController
// will indicate that the call failed at that time.
void RcfProtoChannel::StartCancel()
{
    mFuture.cancel();
}

void RcfProtoChannel::onCompletionCpp()
{
    Message * pResponse = mpResponse;
    Closure * pClosure = mpClosure;

    mpRequest = NULL;
    mpResponse = NULL;
    mpClosure = NULL;

    bool sync = !mAsyncRpcMode;
    if (sync)
    {
        // Synchronous completion.

        RCF_ASSERT(!pClosure);
        if (mError.bad())
        {
            throw mError;
        }

        if (pResponse)
        {
            pResponse->ParseFromArray(
                mResponseBuffer.getPtr(), 
                (int) mResponseBuffer.getLength());
        }
    }
    else
    {
        // Asynchronous completion.

        RCF_ASSERT(pClosure);
        std::auto_ptr<RCF::Exception> ePtr = mFuture.getAsyncException();
        if (ePtr.get())
        {
            mError = *ePtr;
        }
        else
        {
            mResponseBuffer = *mFuture;
        }

        if (mError.good() && pResponse)
        {
            pResponse->ParseFromArray(
                mResponseBuffer.getPtr(), 
                (int) mResponseBuffer.getLength());
        }
        
        pClosure->Run();
    }
}

void RcfProtoChannel::onCompletion()
{
    std::auto_ptr<RCF::Exception> ePtr = mFuture.getAsyncException();
    if (ePtr.get())
    {
        mError = *ePtr;
    }
    else
    {
        mResponseBuffer = *mFuture;
    }

    _SwigCallback * pSwigCallback = mpSwigCallback;
    mpSwigCallback = NULL;
    pSwigCallback->Run();
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4355) // warning C4355: 'this' : used in base member initializer list
#endif

RcfProtoChannel::RcfProtoChannel(const RCF::Endpoint & endpoint) : 
    mRcfClientPtr( new RcfClient<I_Pb>(endpoint) ),
    mpRequest(NULL),
    mpResponse(NULL),
    mpClosure(NULL),
    mpSwigCallback(NULL),
    mAsyncRpcMode(false),
    mSwigCallbackCpp(*this)
{
    mCompletionHandlerSwig = boost::bind(&RcfProtoChannel::onCompletion, this);
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

void RcfProtoChannel::setAsynchronousRpcMode(bool enable)
{
    mAsyncRpcMode = enable;
}

bool RcfProtoChannel::getAsynchronousRpcMode()
{
    return mAsyncRpcMode;
}

void RcfProtoChannel::setPingBackIntervalMs(int pingBackIntervalMs) { mRcfClientPtr->getClientStub().setPingBackIntervalMs(pingBackIntervalMs); }
int RcfProtoChannel::getPingBackIntervalMs()                        { return mRcfClientPtr->getClientStub().getPingBackIntervalMs(); }

void RcfProtoChannel::setHttpProxy(const std::string & httpProxy)   { mRcfClientPtr->getClientStub().setHttpProxy(httpProxy); }
std::string RcfProtoChannel::getHttpProxy()                         { return mRcfClientPtr->getClientStub().getHttpProxy(); }

void RcfProtoChannel::setHttpProxyPort(int httpProxyPort)           { mRcfClientPtr->getClientStub().setHttpProxyPort(httpProxyPort); }
int RcfProtoChannel::getHttpProxyPort()                             { return mRcfClientPtr->getClientStub().getHttpProxyPort(); }

RCF::TransportType RcfProtoChannel::getTransportType()              { return mRcfClientPtr->getClientStub().getTransportType(); }

void RcfProtoChannel::setUsername(const tstring & username)         { mRcfClientPtr->getClientStub().setUsername(username); }
tstring RcfProtoChannel::getUsername()                              { return mRcfClientPtr->getClientStub().getUsername(); }

void RcfProtoChannel::setPassword(const tstring & password)         { mRcfClientPtr->getClientStub().setPassword(password); }
tstring RcfProtoChannel::getPassword()                              { return mRcfClientPtr->getClientStub().getPassword(); }

void RcfProtoChannel::setKerberosSpn(const tstring & kerberosSpn)   { mRcfClientPtr->getClientStub().setKerberosSpn(kerberosSpn); }
tstring RcfProtoChannel::getKerberosSpn()                           { return mRcfClientPtr->getClientStub().getKerberosSpn(); }

void RcfProtoChannel::setEnableCompression(bool enableCompression)  { mRcfClientPtr->getClientStub().setEnableCompression(enableCompression); }
bool RcfProtoChannel::getEnableCompression()                        { return mRcfClientPtr->getClientStub().getEnableCompression(); }

void RcfProtoChannel::setCertificate(CertificatePtr certificatePtr) { mRcfClientPtr->getClientStub().setCertificate(certificatePtr); }
CertificatePtr RcfProtoChannel::getCertificate()                        { return mRcfClientPtr->getClientStub().getCertificate(); }

void RcfProtoChannel::setCaCertificate(CertificatePtr certificatePtr)   { mRcfClientPtr->getClientStub().setCaCertificate(certificatePtr); }
CertificatePtr RcfProtoChannel::getCaCertificate()                      { return mRcfClientPtr->getClientStub().getCaCertificate(); }

void RcfProtoChannel::setOpenSslCipherSuite(const std::string & cipherSuite)    { mRcfClientPtr->getClientStub().setOpenSslCipherSuite(cipherSuite); }
std::string RcfProtoChannel::getOpenSslCipherSuite() const                      { return mRcfClientPtr->getClientStub().getOpenSslCipherSuite(); }

void RcfProtoChannel::setEnableSchannelCertificateValidation(const tstring & peerName)  { mRcfClientPtr->getClientStub().setEnableSchannelCertificateValidation(peerName); }
tstring RcfProtoChannel::getEnableSchannelCertificateValidation() const                 { return mRcfClientPtr->getClientStub().getEnableSchannelCertificateValidation(); }

void RcfProtoChannel::setCertificateValidationCallback(CertificateValidationCb certificateValidationCb)         { mRcfClientPtr->getClientStub().setCertificateValidationCallback(certificateValidationCb); }
const RcfProtoChannel::CertificateValidationCb & RcfProtoChannel::getCertificateValidationCallback() const      { return mRcfClientPtr->getClientStub().getCertificateValidationCallback(); }

void RcfProtoChannel::setSslImplementation(SslImplementation sslImplementation)         { mRcfClientPtr->getClientStub().setSslImplementation(sslImplementation); }
SslImplementation RcfProtoChannel::getSslImplementation() const                         { return mRcfClientPtr->getClientStub().getSslImplementation(); }


bool validateCertificateSwig(Certificate * pCert, _SwigCallback * pSwigCb)
{
    _SwigCallbackArgs args;
    args.mCertificatePtr.reset(pCert, NullDeleter());

    bool certValid = pSwigCb->ValidateCertificate(&args);
    if (args.mErrorString.size() > 0)
    {
        RCF_THROW( Exception( _RcfError_CustomCertValidation(args.mErrorString) ) );
    }
    return certValid;
}

void RcfProtoChannel::_setCertificateValidationCallback(_SwigCallback * pSwigCb)
{
    if (pSwigCb)
    {
        setCertificateValidationCallback( boost::bind(
            &validateCertificateSwig, 
            _1, 
            pSwigCb) );
    }
    else
    {
        setCertificateValidationCallback( CertificateValidationCb() );
    }
}

_SwigCallbackCpp::_SwigCallbackCpp(RcfProtoChannel & channel) :
    mChannel(channel)
{
}

void _SwigCallbackCpp::Run()
{
    mChannel.onCompletionCpp();
}

// Entry point for protobuf RPC C++ code.
void RcfProtoChannel::CallMethod(
    const MethodDescriptor*     method,
    RpcController*              controller,
    const Message*              request,
    Message*                    response,
    google::protobuf::Closure*  done)
{
    RcfProtoController * pRcfController = static_cast<RcfProtoController *>(controller);
    if (pRcfController)
    {
        pRcfController->mpRcfChannel = this;
    }

    mError = RCF::Exception();
    mpRequest = request;
    mpResponse = response;
    mpClosure = done;
    
    // TODO: report error via callback
    if (!request->IsInitialized())
    {
        RCF_THROW(RCF::Exception(RCF::_RcfError_ProtobufWriteInit(typeid(*request).name())));
    }

    int byteSize = request->ByteSize();
    RCF_ASSERT_GTEQ(byteSize , 0);

    RCF::ReallocBufferPtr bufferPtr = RCF::getObjectPool().getReallocBufferPtr();
    bufferPtr->resize(byteSize);
    RCF::ByteBuffer requestBuffer(bufferPtr);
    
    char * pch = requestBuffer.getPtr();
    bool ok = request->SerializeToArray(pch, byteSize);
    RCF_VERIFY(ok, RCF::Exception(RCF::_RcfError_ProtobufWrite(typeid(*request).name())));

    std::string serviceName = method->service()->name();
    int methodId = method->index();
    _SwigCallback * pCallback = &mSwigCallbackCpp;

    // Call setAsynchronousRpcMode() automatically depending on whether a completion was passed in.
    bool doAsynchronous = done ? true : false;
    setAsynchronousRpcMode(doAsynchronous);

    CallMethodInternal(
        serviceName, 
        methodId, 
        requestBuffer,
        pCallback);
}

void RcfProtoChannel::_CallMethodSwig_WithCopy(
    const std::string&          serviceName, 
    int                         methodId, 
    const std::string&          strBuffer, 
    _SwigCallback *             closure)
{
    mRequestCopy = strBuffer;

    _CallMethodSwig(
        serviceName, 
        methodId, 
        (unsigned char *) mRequestCopy.c_str(), 
        (int) mRequestCopy.length(), 
        closure);
}

void RcfProtoChannel::_CallMethodSwig(
    const std::string&          serviceName, 
    int                         methodId, 
    char *                      szBuffer, 
    size_t                      szBufferLen,
    _SwigCallback *              closure)
{
    _CallMethodSwig(serviceName, methodId, (unsigned char *) szBuffer, (int) szBufferLen, closure);
}

void RcfProtoChannel::_CallMethodSwig(
    const std::string&          serviceName, 
    int                         methodId, 
    unsigned char *             szBuffer, 
    int                         szBufferLen,
    _SwigCallback *             closure)

{
    // Make our own copy, as szBuffer is held by the scripting language.
    //RCF::ByteBuffer requestBuffer( (char *) szBuffer, (std::size_t) szBufferLen);
    RCF::ByteBuffer requestBuffer( (size_t) szBufferLen );
    memcpy(requestBuffer.getPtr(), szBuffer, szBufferLen);

    CallMethodInternal(serviceName, methodId, requestBuffer, closure);
}

void RcfProtoChannel::CallMethodInternal(
    const std::string &         serviceName, 
    int                         methodId, 
    RCF::ByteBuffer             requestBuffer,
    _SwigCallback *             closure)
{
    bool sync = !mAsyncRpcMode;
    if (sync)
    {
        // Synchronous call.
        
        mError.clear();
        try
        {
            mResponseBuffer = mRcfClientPtr->DoProtoRpc(
                serviceName, 
                methodId, 
                requestBuffer);
        }
        catch(const RCF::Exception & e)
        {
            mError = e;
        }
        catch(const std::exception & e)
        {
            mError = Exception(e.what());
        }
        catch(...)
        {
            RCF_ASSERT(0 && "RcfProtoChannel::_CallMethodSwig() - non-std::exception derived exception.");
            mError = Exception("RcfProtoChannel::_CallMethodSwig() - non-std::exception derived exception.");
        }

        closure->Run();
    }
    else
    {
        // Asynchronous call.

        mpSwigCallback = closure;

        mFuture = mRcfClientPtr->DoProtoRpc(
            RCF::AsyncTwoway(mCompletionHandlerSwig),
            serviceName,
            methodId,
            requestBuffer);
    }
}

int RcfProtoChannel::_GetResponseBufferLength() const
{
    return (int) mResponseBuffer.getLength();
}

void RcfProtoChannel::_GetResponseBuffer(char * szBuffer, size_t bufferLen) const
{
    _GetResponseBuffer( (unsigned char *) szBuffer, (int) bufferLen);
}

void RcfProtoChannel::_GetResponseBuffer(unsigned char * buffer, int bufferLen) const
{
    RCF_ASSERT(bufferLen == mResponseBuffer.getLength());
    memcpy(buffer, mResponseBuffer.getPtr(), mResponseBuffer.getLength());
}

std::string RcfProtoChannel::_GetResponseBuffer_WithCopy() const
{
    std::string strBuffer(mResponseBuffer.getPtr(), mResponseBuffer.getLength());
    return strBuffer;
}

void RcfProtoChannel::connect()
{
    mRcfClientPtr->getClientStub().connect();
}

void RcfProtoChannel::disconnect()
{
    mRcfClientPtr->getClientStub().disconnect();
}

void RcfProtoChannel::setTransportProtocol(TransportProtocol protocol)
{
    mRcfClientPtr->getClientStub().setTransportProtocol(protocol);
}

TransportProtocol RcfProtoChannel::getTransportProtocol()
{
    return mRcfClientPtr->getClientStub().getTransportProtocol();
}

void RcfProtoChannel::setRemoteCallTimeoutMs(unsigned int remoteCallTimeoutMs)
{
    mRcfClientPtr->getClientStub().setRemoteCallTimeoutMs(remoteCallTimeoutMs);
}

unsigned int RcfProtoChannel::getRemoteCallTimeoutMs() const
{
    return mRcfClientPtr->getClientStub().getRemoteCallTimeoutMs();
}

void RcfProtoChannel::setConnectTimeoutMs(unsigned int connectTimeoutMs)
{
    mRcfClientPtr->getClientStub().setConnectTimeoutMs(connectTimeoutMs);
}

unsigned int RcfProtoChannel::getConnectTimeoutMs() const
{
    return mRcfClientPtr->getClientStub().getConnectTimeoutMs();
}

//------------------------------------------------------------------------------
// Server side

RcfProtoSession::RcfProtoSession(RCF::RcfSession & session) : 
    mRcfSession(session), 
    mServerContext(session)
{
    mRequestBuffer = mServerContext.parameters().a3.get();
}

void RcfProtoSession::_Commit(const std::string & errorMsg)
{
    RCF_ASSERT(!mServerContext.isCommitted());
    std::runtime_error e(errorMsg);
    mServerContext.commit(e);
}

void RcfProtoSession::_Commit()
{
    if (!mServerContext.isCommitted())
    {
        mServerContext.parameters().r.get() = mResponseBuffer;
        mServerContext.commit();
    }
}

// Causes Failed() to return true on the client side.  "reason" will be
// incorporated into the message returned by ErrorText().  If you find
// you need to return machine-readable information about failures, you
// should incorporate it into your response protocol buffer and should
// NOT call SetFailed().
void RcfProtoSession::SetFailed(const std::string& reason)
{
    std::string errorMsg = "RCFProto server error: " + reason;
    std::runtime_error e(errorMsg);
    mServerContext.commit(e);
}

// If true, indicates that the client canceled the RPC, so the server may
// as well give up on replying to it.  The server should still call the
// final "done" callback.
bool RcfProtoSession::IsCanceled() const
{
    return false;
}

// Asks that the given callback be called when the RPC is canceled.  The
// callback will always be called exactly once.  If the RPC completes without
// being canceled, the callback will be called after completion.  If the RPC
// has already been canceled when NotifyOnCancel() is called, the callback
// will be called immediately.
//
// NotifyOnCancel() must be called no more than once per request.
void RcfProtoSession::NotifyOnCancel(Closure* callback)
{
}

void RcfProtoSession::_GetRequestBuffer(char * szBuffer, size_t szBufferLen)
{
    _GetRequestBuffer( (unsigned char *) szBuffer, (int) szBufferLen );
}

void RcfProtoSession::_SetResponseBuffer(char * szBuffer, size_t szBufferLen)
{
    _SetResponseBuffer( (unsigned char *) szBuffer, (int) szBufferLen );
}

int RcfProtoSession::_GetRequestBufferLength()
{
    return (int) mRequestBuffer.getLength();
}

void RcfProtoSession::_GetRequestBuffer(unsigned char * szBuffer, int szBufferLen)
{
    RCF_ASSERT(mRequestBuffer.getLength() == szBufferLen);
    memcpy(szBuffer, mRequestBuffer.getPtr(), mRequestBuffer.getLength());
}

std::string RcfProtoSession::_GetRequestBuffer_WithCopy()
{
    std::string buffer(mRequestBuffer.getPtr(), mRequestBuffer.getLength());
    return buffer;
}

void RcfProtoSession::_SetResponseBuffer(unsigned char * szBuffer, int szBufferLen)
{
    mResponseBuffer = RCF::ByteBuffer(szBufferLen);
    memcpy(mResponseBuffer.getPtr(), szBuffer, szBufferLen);
}

void RcfProtoSession::_SetResponseBuffer_WithCopy(const std::string& buffer)
{
    mResponseBuffer = RCF::ByteBuffer(buffer);
}

tstring RcfProtoSession::getClientUsername()                { return mRcfSession.getClientUsername(); }
TransportProtocol RcfProtoSession::getTransportProtocol()   { return mRcfSession.getTransportProtocol(); }
TransportType RcfProtoSession::getTransportType()           { return mRcfSession.getTransportType(); }

bool RcfProtoSession::getEnableCompression()                { return mRcfSession.getEnableCompression(); }

//time_t RcfProtoSession::getConnectedAtTime() const            { return mRcfSession.getConnectedAtTime(); }

std::size_t RcfProtoSession::getConnectionDuration() const  { return mRcfSession.getConnectionDuration(); }

std::size_t RcfProtoSession::getRemoteCallCount() const         { return mRcfSession.getRemoteCallCount(); }
boost::uint64_t RcfProtoSession::getTotalBytesReceived() const  { return mRcfSession.getTotalBytesReceived(); }
boost::uint64_t RcfProtoSession::getTotalBytesSent() const      { return mRcfSession.getTotalBytesSent(); }

RcfProtoServer::RcfProtoServer(const RCF::Endpoint & endpoint) : 
    RCF::RcfServer(endpoint), mpSwigCallback(NULL)
{
    RCF::RcfServer::bind<I_Pb>(*this);
}

RcfProtoServer::RcfProtoServer() : 
    RCF::RcfServer(), mpSwigCallback(NULL)
{
    RCF::RcfServer::bind<I_Pb>(*this);
}

RcfProtoServer::~RcfProtoServer()
{
}

void RcfProtoServer::start()
{
    RcfServer::start();
}

void RcfProtoServer::stop()
{
    RcfServer::stop();
}

void RcfProtoServer::_setCallbackTable(_SwigCallback * pCallback)
{
    mpSwigCallback = pCallback;
}

void RcfProtoServer::bindService(Service & service)
{
    mProtobufServices[service.GetDescriptor()->name()] = &service;
}

RCF::ByteBuffer RcfProtoServer::DoProtoRpc( 
    const std::string &     serviceName,
    int                     methodId,
    RCF::ByteBuffer         requestBuffer)
{
    RCF::RcfSession & rcfSession = RCF::getCurrentRcfSession();
    RcfProtoSession * rcfProtoSession = new RcfProtoSession(rcfSession);
    
    if (mpSwigCallback)
    {
        _SwigCallbackArgs args;
        _SwigCallbackArgs * pArgs = &args;
        mpSwigCallback->ProtoRpcBegin(pArgs, this, rcfProtoSession, serviceName, methodId);
        if (pArgs->mErrorString.size() > 0)
        {
            rcfProtoSession->SetFailed( pArgs->mErrorString );
            rcfProtoSession->_Commit();
        }
    }
    else
    {
        ProtoRpcBeginCpp(rcfProtoSession, serviceName, methodId);
    }
    
    return RCF::ByteBuffer();
}

// C++ dispatching implementation.

void RcfProtoServer::ProtoRpcBeginCpp( 
    RcfProtoSession *       rcfProtoSession,
    const std::string &     serviceName,
    int                     methodId)
{
    RcfProtoController * controller = new RcfProtoController(rcfProtoSession);

    Service * pService = NULL;
    ProtobufServices::iterator iter = mProtobufServices.find(serviceName);
    if (iter != mProtobufServices.end())
    {
        pService = iter->second;
    }
    if (!pService)
    {
        std::string errorMsg = "The requested service does not exist on this server. Service name: " + serviceName;
        controller->SetFailed(errorMsg);
        delete controller->mpRcfContext;
        delete controller;
        return;
    }

    const ServiceDescriptor * sd = pService->GetDescriptor();
    const MethodDescriptor * md = sd->method(methodId);
    Message * pRequest = pService->GetRequestPrototype(md).New();
    Message * pResponse = pService->GetResponsePrototype(md).New();
    RCF::ByteBuffer requestBuffer = controller->mpRcfContext->mRequestBuffer;
    pRequest->ParseFromArray(requestBuffer.getPtr(), (int) requestBuffer.getLength());
    std::pair<Message *, Message *> requestResponsePair(pRequest, pResponse);
    Closure * done = NewCallback(this, &RcfProtoServer::ProtoRpcEndCpp, requestResponsePair, controller);
    pService->CallMethod(md, controller, pRequest, pResponse, done);
}

void RcfProtoServer::ProtoRpcEndCpp(
    std::pair<Message *, Message *> requestResponsePair,
    RcfProtoController * controller)
{
    Message * pRequest = requestResponsePair.first;
    Message * pResponse = requestResponsePair.second;

    RcfProtoSession & context = * controller->mpRcfContext;
    int byteSize = pResponse->ByteSize();
    context.mResponseBuffer = RCF::ByteBuffer(byteSize);
    pResponse->SerializeToArray(context.mResponseBuffer.getPtr(), (int) context.mResponseBuffer.getLength());
    context._Commit();
    delete pRequest;
    delete pResponse;
    delete controller->mpRcfContext;
    delete controller;
}

} // namespace RCF
