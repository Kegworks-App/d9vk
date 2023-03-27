#pragma once

#include "d3d9_adapter.h"
#include "d3d9_interop.h"

#include "../dxvk/dxvk_instance.h"

namespace dxvk {

  /**
  * \brief D3D9 interface implementation
  *
  * Implements the IDirect3DDevice9Ex interfaces
  * which provides the way to get adapters and create other objects such as \ref IDirect3DDevice9Ex.
  * similar to \ref DxgiFactory but for D3D9.
  */
  class D3D9InterfaceEx final : public ComObjectClamp<IDirect3D9Ex> {

  public:

    D3D9InterfaceEx(bool bExtended);

    ~D3D9InterfaceEx();

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void* pInitializeFunction);

    UINT STDMETHODCALLTYPE GetAdapterCount();

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(
            UINT                    Adapter,
            DWORD                   Flags,
            D3DADAPTER_IDENTIFIER9* pIdentifier);

    UINT STDMETHODCALLTYPE GetAdapterModeCount(UINT Adapter, D3DFORMAT Format);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceType(
            UINT       Adapter,
            D3DDEVTYPE DevType,
            D3DFORMAT  AdapterFormat,
            D3DFORMAT  BackBufferFormat,
            BOOL       bWindowed);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceFormat(
            UINT            Adapter,
            D3DDEVTYPE      DeviceType,
            D3DFORMAT       AdapterFormat,
            DWORD           Usage,
            D3DRESOURCETYPE RType,
            D3DFORMAT       CheckFormat);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(
            UINT                Adapter,
            D3DDEVTYPE          DeviceType,
            D3DFORMAT           SurfaceFormat,
            BOOL                Windowed,
            D3DMULTISAMPLE_TYPE MultiSampleType,
            DWORD*              pQualityLevels);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(
            UINT       Adapter,
            D3DDEVTYPE DeviceType,
            D3DFORMAT  AdapterFormat,
            D3DFORMAT  RenderTargetFormat,
            D3DFORMAT  DepthStencilFormat);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceFormatConversion(
            UINT       Adapter,
            D3DDEVTYPE DeviceType,
            D3DFORMAT  SourceFormat,
            D3DFORMAT  TargetFormat);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceCaps(
            UINT       Adapter,
            D3DDEVTYPE DeviceType,
            D3DCAPS9*  pCaps);

    HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(UINT Adapter);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateDevice(
            UINT                   Adapter,
            D3DDEVTYPE             DeviceType,
            HWND                   hFocusWindow,
            DWORD                  BehaviorFlags,
            D3DPRESENT_PARAMETERS* pPresentationParameters,
            IDirect3DDevice9**     ppReturnedDeviceInterface);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumAdapterModes(
            UINT            Adapter,
            D3DFORMAT       Format,
            UINT            Mode,
            D3DDISPLAYMODE* pMode);

    // Ex Methods

    UINT STDMETHODCALLTYPE GetAdapterModeCountEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumAdapterModesEx(
            UINT                  Adapter,
      const D3DDISPLAYMODEFILTER* pFilter,
            UINT                  Mode,
            D3DDISPLAYMODEEX*     pMode);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAdapterDisplayModeEx(
            UINT                Adapter,
            D3DDISPLAYMODEEX*   pMode,
            D3DDISPLAYROTATION* pRotation);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateDeviceEx(
            UINT                   Adapter,
            D3DDEVTYPE             DeviceType,
            HWND                   hFocusWindow,
            DWORD                  BehaviorFlags,
            D3DPRESENT_PARAMETERS* pPresentationParameters,
            D3DDISPLAYMODEEX*      pFullscreenDisplayMode,
            IDirect3DDevice9Ex**   ppReturnedDeviceInterface);

    DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAdapterLUID(UINT Adapter, LUID* pLUID);

    const D3D9Options& GetOptions() { return m_d3d9Options; }

    D3D9Adapter* GetAdapter(UINT Ordinal) {
      return Ordinal < m_adapters.size()
        ? &m_adapters[Ordinal]
        : nullptr;
    }

    bool IsExtended() { return m_extended; }

    Rc<DxvkInstance> GetInstance() { return m_instance; }

  private:

    void CacheModes(D3D9Format Format);

    static const char* GetDriverDllName(DxvkGpuVendor vendor);

    Rc<DxvkInstance>              m_instance;

    bool                          m_extended;

    D3D9Options                   m_d3d9Options;

    std::vector<D3D9Adapter>      m_adapters;

    D3D9VkInteropInterface        m_d3d9Interop;

  };

}