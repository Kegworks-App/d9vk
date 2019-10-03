#include "d3d11sdklayers.h"

#include "../util/com/com_object.h"
#include "../util/log/log.h"

namespace dxvk {
  class D3D11InfoQueue : public ComObject<ID3D11InfoQueue> {
  public:
    D3D11InfoQueue() {

    }

    ~D3D11InfoQueue() {

    }
    
    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID                  riid,
            void**                  ppvObject) {
              Logger::warn("InfoQueue::QueryInterface");
              return S_OK;
            }

    HRESULT AddApplicationMessage(
      D3D11_MESSAGE_SEVERITY Severity,
      LPCSTR pDescription
    ) {
              Logger::warn("InfoQueue::AddApplicationMessage");
      return S_OK;
    }

    HRESULT AddMessage(
  D3D11_MESSAGE_CATEGORY Category,
  D3D11_MESSAGE_SEVERITY Severity,
  D3D11_MESSAGE_ID       ID,
  LPCSTR                 pDescription
) {
              Logger::warn("InfoQueue::AddMessage");
      return S_OK;
}

HRESULT AddRetrievalFilterEntries(
  D3D11_INFO_QUEUE_FILTER *pFilter
) {
              Logger::warn("InfoQueue::AddRetrievalFilterEntries");
  return S_OK;
}

HRESULT AddStorageFilterEntries(
  D3D11_INFO_QUEUE_FILTER *pFilter
) {
              Logger::warn("InfoQueue::AddStorageFilterEntries");
  return S_OK;
}

void ClearRetrievalFilter(

) {
              Logger::warn("InfoQueue::ClearRetrievalFilter");

}

void ClearStorageFilter(

) {
              Logger::warn("InfoQueue::ClearStorageFilter");

}

void ClearStoredMessages(

) {
              Logger::warn("InfoQueue::ClearStoredMessages");

}

BOOL GetBreakOnCategory(
  D3D11_MESSAGE_CATEGORY Category
) {
              Logger::warn("InfoQueue::GetBreakOnCategory");
  return false;
}

BOOL GetBreakOnID(
  D3D11_MESSAGE_ID ID
) {
              Logger::warn("InfoQueue::GetBreakOnID");
  return false;
}

BOOL GetBreakOnSeverity(
  D3D11_MESSAGE_SEVERITY Severity
) {
              Logger::warn("InfoQueue::GetBreakOnSeverity");
  return false;
}

HRESULT GetMessage(
  UINT64        MessageIndex,
  D3D11_MESSAGE *pMessage,
  SIZE_T        *pMessageByteLength
) {
              Logger::warn("InfoQueue::GetMessage");
  return S_OK;
}

UINT64 GetMessageCountLimit(

) {
              Logger::warn("InfoQueue::GetMessageCountLimit");
  return 0;
}

BOOL GetMuteDebugOutput(

) {
              Logger::warn("InfoQueue::GetMuteDebugOutput");
  return false;
}

UINT64 GetNumMessagesAllowedByStorageFilter(

) {
              Logger::warn("InfoQueue::GetNumMessagesAllowedByStorageFilter");
  return 0;
}

UINT64 GetNumMessagesDeniedByStorageFilter(

) {
              Logger::warn("InfoQueue::GetNumMessagesDeniedByStorageFilter");
  return 0;
}

UINT64 GetNumMessagesDiscardedByMessageCountLimit(

) {
              Logger::warn("InfoQueue::GetNumMessagesDiscardedByMessageCountLimit");
  return 0;
}

UINT64 GetNumStoredMessages(

) {
              Logger::warn("InfoQueue::GetNumStoredMessages");
  return 0;
}

UINT64 GetNumStoredMessagesAllowedByRetrievalFilter(

) {
              Logger::warn("InfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter");
  return 0;
}

HRESULT GetRetrievalFilter(
  D3D11_INFO_QUEUE_FILTER *pFilter,
  SIZE_T                  *pFilterByteLength
) {
              Logger::warn("InfoQueue::GetRetrievalFilter");
  return S_OK;
}

UINT GetRetrievalFilterStackSize(

) {
              Logger::warn("InfoQueue::GetRetrievalFilterStackSize");
  return 0;
}

HRESULT GetStorageFilter(
  D3D11_INFO_QUEUE_FILTER *pFilter,
  SIZE_T                  *pFilterByteLength
) {
              Logger::warn("InfoQueue::GetStorageFilter");
  return S_OK;
}

UINT GetStorageFilterStackSize(

) {
              Logger::warn("InfoQueue::GetStorageFilterStackSize");
  return 0;
}

void PopRetrievalFilter() {
              Logger::warn("InfoQueue::PopRetrievalFilter");
}

void PopStorageFilter() {
              Logger::warn("InfoQueue::PopStorageFilter");

}

HRESULT PushCopyOfRetrievalFilter() {
              Logger::warn("InfoQueue::PushCopyOfRetrievalFilter");
  return S_OK;
}

HRESULT PushCopyOfStorageFilter() {
              Logger::warn("InfoQueue::PushCopyOfStorageFilter");
  return S_OK;
}

HRESULT PushEmptyRetrievalFilter() {
              Logger::warn("InfoQueue::PushEmptyRetrievalFilter");
  return S_OK;
}

HRESULT PushEmptyStorageFilter() {
              Logger::warn("InfoQueue::PushEmptyStorageFilter");
  return S_OK;
}

HRESULT PushRetrievalFilter(
  D3D11_INFO_QUEUE_FILTER *pFilter
) {
              Logger::warn("InfoQueue::PushRetrievalFilter");
  return S_OK;
}

HRESULT PushStorageFilter(
  D3D11_INFO_QUEUE_FILTER *pFilter
) {
              Logger::warn("InfoQueue::PushStorageFilter");
  return S_OK;
}

HRESULT SetBreakOnCategory(
  D3D11_MESSAGE_CATEGORY Category,
  BOOL                   bEnable
) {
              Logger::warn("InfoQueue::SetBreakOnCategory");
  return S_OK;
}

HRESULT SetBreakOnID(
  D3D11_MESSAGE_ID ID,
  BOOL             bEnable
) {
              Logger::warn("InfoQueue::SetBreakOnID");
  return S_OK;
}

HRESULT SetBreakOnSeverity(
  D3D11_MESSAGE_SEVERITY Severity,
  BOOL                   bEnable
) {
              Logger::warn("InfoQueue::SetBreakOnSeverity");
  return S_OK;
}

HRESULT SetMessageCountLimit(
  UINT64 MessageCountLimit
) {
              Logger::warn("InfoQueue::SetMessageCountLimit");
  return S_OK;
}

void SetMuteDebugOutput(
  BOOL bMute
) {
              Logger::warn("InfoQueue::SetMuteDebugOutput");

}
  };
}