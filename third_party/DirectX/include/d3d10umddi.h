﻿/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  Content: Device Driver Interface
 *
 ***************************************************************************/
#ifndef _D3D10UMDDI_H
#define _D3D10UMDDI_H

#include "dxmini.h"
#include "d3dkmddi.h"
#include "d3d9types.h"
#include "d3dumddi.h"
#include "d3dkmthk.h"

#ifndef D3D10DDI_MINOR_HEADER_VERSION
#define D3D10DDI_MINOR_HEADER_VERSION 1
#endif

#include "dxgiddi.h"
#include "d3d10tokenizedprogramformat.hpp"

//----------------------------------

// Strongly-typed handles:

// Kernel Mode handles:
//----------------------------------
#if defined( __cplusplus )

//----------------------------------
#define D3D10DDI_HKM( TYPE ) \
typedef struct TYPE \
{ \
    D3DKMT_HANDLE handle; \
 \
    bool operator< ( const TYPE& o ) const \
    { return handle <  o.handle; } \
    bool operator<=( const TYPE& o ) const \
    { return handle <= o.handle; } \
    bool operator> ( const TYPE& o ) const \
    { return handle >  o.handle; } \
    bool operator>=( const TYPE& o ) const \
    { return handle >= o.handle; } \
    bool operator==( const TYPE& o ) const \
    { return handle == o.handle; } \
    bool operator!=( const TYPE& o ) const \
    { return handle != o.handle; } \
} TYPE; \
 \
inline TYPE MAKE_##TYPE##( D3DKMT_HANDLE h ) \
{ const TYPE r = { h }; return r; }

#else

//----------------------------------
#define D3D10DDI_HKM( TYPE ) \
typedef struct TYPE \
{ \
    D3DKMT_HANDLE handle; \
} TYPE;

#endif

//----------------------------------
D3D10DDI_HKM( D3D10DDI_HKMDEVICE ) // D3D10DDI_HKMDEVICE
D3D10DDI_HKM( D3D10DDI_HKMRESOURCE ) // D3D10DDI_HKMRESOURCE
D3D10DDI_HKM( D3D10DDI_HKMALLOCATION ) // D3D10DDI_HKMALLOCATION

// Runtime handles:
//----------------------------------
#if defined( __cplusplus )

#define D3D10DDI_HRT( TYPE ) \
typedef struct TYPE \
{ \
    VOID* handle; \
 \
    bool operator< ( const TYPE& o ) const \
    { return handle <  o.handle; } \
    bool operator<=( const TYPE& o ) const \
    { return handle <= o.handle; } \
    bool operator> ( const TYPE& o ) const \
    { return handle >  o.handle; } \
    bool operator>=( const TYPE& o ) const \
    { return handle >= o.handle; } \
    bool operator==( const TYPE& o ) const \
    { return handle == o.handle; } \
    bool operator!=( const TYPE& o ) const \
    { return handle != o.handle; } \
} TYPE; \
 \
inline TYPE MAKE_##TYPE##( VOID* h ) \
{ const TYPE r = { h }; return r; }
#else

#define D3D10DDI_HRT( TYPE ) \
typedef struct TYPE \
{ \
    VOID* handle; \
} TYPE;
#endif

//----------------------------------
D3D10DDI_HRT( D3D10DDI_HRTADAPTER ) // D3D10DDI_HRTADAPTER  ... this is for KT callbacks
D3D10DDI_HRT( D3D10DDI_HRTDEVICE ) // D3D10DDI_HRTDEVICE  ... this is for KT callbacks
D3D10DDI_HRT( D3D10DDI_HRTCORELAYER ) // D3D10DDI_HRTCORELAYER ... this is for the core layer callbacks
D3D10DDI_HRT( D3D10DDI_HRTRESOURCE ) // D3D10DDI_HRTRESOURCE
D3D10DDI_HRT( D3D10DDI_HRTSHADERRESOURCEVIEW ) // D3D10DDI_HRTSHADERRESOURCEVIEW
D3D10DDI_HRT( D3D10DDI_HRTRENDERTARGETVIEW ) // D3D10DDI_HRTRENDERTARGETVIEW
D3D10DDI_HRT( D3D10DDI_HRTDEPTHSTENCILVIEW ) // D3D10DDI_HRTDEPTHSTENCILVIEW
D3D10DDI_HRT( D3D10DDI_HRTSHADER ) // D3D10DDI_HRTSHADER
D3D10DDI_HRT( D3D10DDI_HRTELEMENTLAYOUT ) // D3D10DDI_HRTELEMENTLAYOUT
D3D10DDI_HRT( D3D10DDI_HRTBLENDSTATE ) // D3D10DDI_HRTBLENDSTATE
D3D10DDI_HRT( D3D10DDI_HRTDEPTHSTENCILSTATE ) // D3D10DDI_HRTDEPTHSTENCILSTATE
D3D10DDI_HRT( D3D10DDI_HRTRASTERIZERSTATE ) // D3D10DDI_HRTRASTERIZERSTATE
D3D10DDI_HRT( D3D10DDI_HRTSAMPLER ) // D3D10DDI_HRTSAMPLER
D3D10DDI_HRT( D3D10DDI_HRTQUERY ) // D3D10DDI_HRTQUERY

// Driver handles:
//----------------------------------
#if defined( __cplusplus )

#define D3D10DDI_H( TYPE ) \
typedef struct TYPE \
{ \
    VOID* pDrvPrivate; \
 \
    bool operator< ( const TYPE& o ) const \
    { return pDrvPrivate <  o.pDrvPrivate; } \
    bool operator<=( const TYPE& o ) const \
    { return pDrvPrivate <= o.pDrvPrivate; } \
    bool operator> ( const TYPE& o ) const \
    { return pDrvPrivate >  o.pDrvPrivate; } \
    bool operator>=( const TYPE& o ) const \
    { return pDrvPrivate >= o.pDrvPrivate; } \
    bool operator==( const TYPE& o ) const \
    { return pDrvPrivate == o.pDrvPrivate; } \
    bool operator!=( const TYPE& o ) const \
    { return pDrvPrivate != o.pDrvPrivate; } \
} TYPE; \
 \
inline TYPE MAKE_##TYPE##( VOID* h ) \
{ const TYPE r = { h }; return r; }
#else

#define D3D10DDI_H( TYPE ) \
typedef struct TYPE \
{ \
    VOID* pDrvPrivate; \
} TYPE;
#endif

//----------------------------------
D3D10DDI_H( D3D10DDI_HADAPTER ) // D3D10DDI_HADAPTER
D3D10DDI_H( D3D10DDI_HDEVICE ) // D3D10DDI_HDEVICE
D3D10DDI_H( D3D10DDI_HRESOURCE ) // D3D10DDI_HRESOURCE
D3D10DDI_H( D3D10DDI_HSHADERRESOURCEVIEW ) // D3D10DDI_HSHADERRESOURCEVIEW
D3D10DDI_H( D3D10DDI_HRENDERTARGETVIEW ) // D3D10DDI_HRENDERTARGETVIEW
D3D10DDI_H( D3D10DDI_HDEPTHSTENCILVIEW ) // D3D10DDI_HDEPTHSTENCILVIEW
D3D10DDI_H( D3D10DDI_HSHADER ) // D3D10DDI_HSHADER
D3D10DDI_H( D3D10DDI_HELEMENTLAYOUT ) // D3D10DDI_HELEMENTLAYOUT
D3D10DDI_H( D3D10DDI_HBLENDSTATE ) // D3D10DDI_HBLENDSTATE
D3D10DDI_H( D3D10DDI_HDEPTHSTENCILSTATE ) // D3D10DDI_HDEPTHSTENCILSTATE
D3D10DDI_H( D3D10DDI_HRASTERIZERSTATE ) // D3D10DDI_HRASTERIZERSTATE
D3D10DDI_H( D3D10DDI_HSAMPLER ) // D3D10DDI_HSAMPLER
D3D10DDI_H( D3D10DDI_HQUERY ) // D3D10DDI_HQUERY

typedef enum D3D10DDIRESOURCE_TYPE
{
    D3D10DDIRESOURCE_BUFFER      = 1,
    D3D10DDIRESOURCE_TEXTURE1D   = 2,
    D3D10DDIRESOURCE_TEXTURE2D   = 3,
    D3D10DDIRESOURCE_TEXTURE3D   = 4,
    D3D10DDIRESOURCE_TEXTURECUBE = 5,
} D3D10DDIRESOURCE_TYPE;

typedef enum D3D10_DDI_RESOURCE_USAGE
{
    D3D10_DDI_USAGE_DEFAULT    = 0,
    D3D10_DDI_USAGE_IMMUTABLE  = 1,
    D3D10_DDI_USAGE_DYNAMIC    = 2,
    D3D10_DDI_USAGE_STAGING    = 3,
} D3D10_DDI_RESOURCE_USAGE;

typedef enum D3D10_DDI_RESOURCE_BIND_FLAG
{
    D3D10_DDI_BIND_VERTEX_BUFFER     = 0x00000001L,
    D3D10_DDI_BIND_INDEX_BUFFER      = 0x00000002L,
    D3D10_DDI_BIND_CONSTANT_BUFFER   = 0x00000004L,
    D3D10_DDI_BIND_SHADER_RESOURCE   = 0x00000008L,
    D3D10_DDI_BIND_STREAM_OUTPUT     = 0x00000010L,
    D3D10_DDI_BIND_RENDER_TARGET     = 0x00000020L,
    D3D10_DDI_BIND_DEPTH_STENCIL     = 0x00000040L,
    D3D10_DDI_BIND_PIPELINE_MASK     = 0x0000007FL,

    D3D10_DDI_BIND_PRESENT           = 0x00000080L,
    D3D10_DDI_BIND_MASK              = 0x000000FFL,
} D3D10_DDI_RESOURCE_BIND_FLAG;

typedef enum D3D10_DDI_CPU_ACCESS
{
    D3D10_DDI_CPU_ACCESS_WRITE          = 0x00000001L,
    D3D10_DDI_CPU_ACCESS_READ           = 0x00000002L,
    D3D10_DDI_CPU_ACCESS_MASK          = 0x00000003L,
} D3D10_DDI_CPU_ACCESS;

typedef enum D3D10_DDI_RESOURCE_MISC_FLAG
{
    D3D10_DDI_RESOURCE_AUTO_GEN_MIP_MAP = 0x00000001L,
    D3D10_DDI_RESOURCE_MISC_SHARED = 0x00000002L,
    D3D10_DDI_RESOURCE_MISC_DISCARD_ON_PRESENT = 0x00000008L,
} D3D10_DDI_RESOURCE_MISC_FLAG;

typedef enum D3D10_DDI_MAP // for calling ID3D10Resource::Map()
{
    D3D10_DDI_MAP_READ = 1,
    D3D10_DDI_MAP_WRITE = 2,
    D3D10_DDI_MAP_READWRITE = 3,
    D3D10_DDI_MAP_WRITE_DISCARD = 4,
    D3D10_DDI_MAP_WRITE_NOOVERWRITE = 5,
} D3D10_DDI_MAP;

typedef enum D3D10_DDI_MAP_FLAG
{
    D3D10_DDI_MAP_FLAG_DONOTWAIT             = 0x00100000L,
    D3D10_DDI_MAP_FLAG_MASK                  = 0x00100000L,
} D3D10_DDI_MAP_FLAG;

// Bit fields for the CheckFormatSupport DDI for features that are optional on some formats.
#define D3D10_DDI_FORMAT_SUPPORT_SHADER_SAMPLE            0x00000001 // format can be sampled with any filter in shaders
#define D3D10_DDI_FORMAT_SUPPORT_RENDERTARGET             0x00000002 // format can be a renderTarget
#define D3D10_DDI_FORMAT_SUPPORT_BLENDABLE                0x00000004 // format is blendable (can only be set if format can be renderTarget)
#define D3D10_DDI_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET 0x00000008 // format can be used as RenderTarget with some sample count > 1.
#define D3D10_DDI_FORMAT_SUPPORT_MULTISAMPLE_LOAD         0x00000010 // format can be used as source for 'ld2dms'

typedef struct D3D10DDI_MIPINFO
{
    UINT TexelWidth;
    UINT TexelHeight;
    UINT TexelDepth;
    UINT PhysicalWidth;
    UINT PhysicalHeight;
    UINT PhysicalDepth;
} D3D10DDI_MIPINFO;

typedef struct D3D10_DDIARG_SUBRESOURCE_UP
{
    VOID*   pSysMem;
    UINT  SysMemPitch;
    UINT  SysMemSlicePitch;
} D3D10_DDIARG_SUBRESOURCE_UP;

typedef struct D3D10DDIARG_CREATERESOURCE
{
    CONST D3D10DDI_MIPINFO*              pMipInfoList;
    CONST D3D10_DDIARG_SUBRESOURCE_UP*   pInitialDataUP; // non-NULL if Usage has invariant
    D3D10DDIRESOURCE_TYPE                ResourceDimension; // Part of old Caps1

    UINT                                 Usage; // Part of old Caps1
    UINT                                 BindFlags; // Part of old Caps1
    UINT                                 MapFlags;
    UINT                                 MiscFlags;

    DXGI_FORMAT                          Format; // Totally different than D3DDDIFORMAT
    DXGI_SAMPLE_DESC                     SampleDesc;
    UINT                                 MipLevels;
    UINT                                 ArraySize;

    // Can only be non-NULL, if BindFlags has D3D10_DDI_BIND_PRESENT bit set; but not always.
    // Presence of structure is an indication that Resource could be used as a primary (ie. scanned-out),
    // and naturally used with Present (flip style). (UMD can prevent this- see dxgiddi.h)
    // If pPrimaryDesc absent, blt/ copy style is implied when used with Present.
    DXGI_DDI_PRIMARY_DESC*               pPrimaryDesc;
} D3D10DDIARG_CREATERESOURCE;

typedef struct D3D10DDIARG_OPENRESOURCE
{
    UINT                        NumAllocations;             // in : Number of open allocation structs
    D3DDDI_OPENALLOCATIONINFO*  pOpenAllocationInfo;        // in : Array of open allocation structs
    D3D10DDI_HKMRESOURCE        hKMResource;                // in : Kernel resource handle
    VOID*                       pPrivateDriverData;         // in : Ptr to per reosurce PrivateDriverData
    UINT                        PrivateDriverDataSize;      // in : Size of resource pPrivateDriverData
} D3D10DDIARG_OPENRESOURCE;

typedef struct D3D10DDI_RANGE
{
    UINT Start;
    UINT End; // one past end. (End - Start) = size
} D3D10DDI_RANGE;

typedef struct D3D10DDI_RECT
{
    D3D10DDI_RANGE X;
    D3D10DDI_RANGE Y;
} D3D10DDI_RECT;

typedef struct D3D10DDI_BOX
{
    D3D10DDI_RANGE X;
    D3D10DDI_RANGE Y;
    D3D10DDI_RANGE Z;
} D3D10DDI_BOX;

typedef enum D3D10DDI_SHADERUNITTYPE
{
    D3D10DDISHADERUNITTYPE_UNDEFINED= 0,
    D3D10DDISHADERUNITTYPE_GEOMETRY = 1,
    D3D10DDISHADERUNITTYPE_VERTEX   = 2,
    D3D10DDISHADERUNITTYPE_PIXEL    = 3,
} D3D10DDI_SHADERUNITTYPE;

typedef struct D3D10DDI_MAPPED_SUBRESOURCE
{
    void * pData;
    UINT RowPitch;
    UINT DepthPitch;
} D3D10DDI_MAPPED_SUBRESOURCE;

//----------------------------------------------------------------------------------------------------------------------------------
// User mode function argument definitions
//

typedef struct D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW
{
    UINT     ElementOffset;
    UINT     ElementWidth;
} D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW;

typedef struct D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW
{
    UINT     MostDetailedMip;
    UINT     FirstArraySlice;
    UINT     MipLevels;
    UINT     ArraySize;
} D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW;

typedef struct D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW
{
    UINT     MostDetailedMip;
    UINT     FirstArraySlice;
    UINT     MipLevels;
    UINT     ArraySize;
} D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW;

typedef struct D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW
{
    UINT     MostDetailedMip;
    UINT     MipLevels;
} D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW;

typedef struct D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW
{
    UINT     MostDetailedMip;
    UINT     MipLevels;
} D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW;

typedef struct D3D10DDIARG_CREATESHADERRESOURCEVIEW
{
    D3D10DDI_HRESOURCE    hDrvResource;
    DXGI_FORMAT          Format;
    D3D10DDIRESOURCE_TYPE ResourceDimension;

    union
    {
        D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW  Buffer;
        D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW   Tex1D;
        D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW   Tex2D;
        D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW   Tex3D;
        D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW TexCube;
    };
} D3D10DDIARG_CREATESHADERRESOURCEVIEW;

typedef struct D3D10DDIARG_BUFFER_RENDERTARGETVIEW
{
    UINT     ElementOffset;
    UINT     ElementWidth;
} D3D10DDIARG_BUFFER_RENDERTARGETVIEW;

typedef struct D3D10DDIARG_TEX1D_RENDERTARGETVIEW
{
    UINT     MipSlice;
    UINT     FirstArraySlice;
    UINT     ArraySize;
} D3D10DDIARG_TEX1D_RENDERTARGETVIEW;

typedef struct D3D10DDIARG_TEX2D_RENDERTARGETVIEW
{
    UINT     MipSlice;
    UINT     FirstArraySlice;
    UINT     ArraySize;
} D3D10DDIARG_TEX2D_RENDERTARGETVIEW;

typedef struct D3D10DDIARG_TEX3D_RENDERTARGETVIEW
{
    UINT     MipSlice;
    UINT     FirstW;
    UINT     WSize;
} D3D10DDIARG_TEX3D_RENDERTARGETVIEW;

typedef struct D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW
{
    UINT     MipSlice;
    UINT     FirstArraySlice;
    UINT     ArraySize;
} D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW;

typedef struct D3D10DDIARG_CREATERENDERTARGETVIEW
{
    D3D10DDI_HRESOURCE    hDrvResource;
    DXGI_FORMAT          Format;
    D3D10DDIRESOURCE_TYPE ResourceDimension;

    union
    {
        D3D10DDIARG_BUFFER_RENDERTARGETVIEW  Buffer;
        D3D10DDIARG_TEX1D_RENDERTARGETVIEW   Tex1D;
        D3D10DDIARG_TEX2D_RENDERTARGETVIEW   Tex2D;
        D3D10DDIARG_TEX3D_RENDERTARGETVIEW   Tex3D;
        D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW TexCube;
    };
} D3D10DDIARG_CREATERENDERTARGETVIEW;

typedef struct D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW
{
    UINT     MipSlice;
    UINT     FirstArraySlice;
    UINT     ArraySize;
} D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW;

typedef struct D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW
{
    UINT     MipSlice;
    UINT     FirstArraySlice;
    UINT     ArraySize;
} D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW;

typedef struct D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW
{
    UINT     MipSlice;
    UINT     FirstArraySlice;
    UINT     ArraySize;
} D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW;

typedef struct D3D10DDIARG_CREATEDEPTHSTENCILVIEW
{
    D3D10DDI_HRESOURCE    hDrvResource;
    DXGI_FORMAT          Format;
    D3D10DDIRESOURCE_TYPE ResourceDimension;

    union
    {
        D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW   Tex1D;
        D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW   Tex2D;
        D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW TexCube;
    };
} D3D10DDIARG_CREATEDEPTHSTENCILVIEW;

typedef enum D3D10_DDI_INPUT_CLASSIFICATION
{
    D3D10_DDI_INPUT_PER_VERTEX_DATA = 0,
    D3D10_DDI_INPUT_PER_INSTANCE_DATA = 1
} D3D10_DDI_INPUT_CLASSIFICATION;

typedef struct D3D10DDIARG_INPUT_ELEMENT_DESC
{
    UINT InputSlot;
    UINT AlignedByteOffset;
    DXGI_FORMAT Format;
    D3D10_DDI_INPUT_CLASSIFICATION InputSlotClass;
    UINT InstanceDataStepRate;
    UINT InputRegister;
}       D3D10DDIARG_INPUT_ELEMENT_DESC;

typedef struct D3D10DDIARG_CREATEELEMENTLAYOUT
{
    CONST D3D10DDIARG_INPUT_ELEMENT_DESC* pVertexElements;
    UINT                                  NumElements;
} D3D10DDIARG_CREATEELEMENTLAYOUT;

typedef struct D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY
{
    UINT OutputSlot;
    UINT RegisterIndex;
    BYTE RegisterMask; // (D3D10_SB_OPERAND_4_COMPONENT_MASK >> 4), meaning 4 LSBs are xyzw respectively
} D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY;

typedef struct D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT
{
    CONST UINT*                                         pShaderCode;
    CONST D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY*  pOutputStreamDecl;
    UINT                                                NumEntries;
    UINT                                                StreamOutputStrideInBytes;
} D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT;

typedef struct D3D10DDIARG_SIGNATURE_ENTRY
{
    D3D10_SB_NAME SystemValue; // D3D10_SB_NAME_UNDEFINED if the particular entry doesn't have a system name.
    UINT Register;
    BYTE Mask;// (D3D10_SB_OPERAND_4_COMPONENT_MASK >> 4), meaning 4 LSBs are xyzw respectively
} D3D10DDIARG_SIGNATURE_ENTRY;

typedef struct D3D10DDIARG_STAGE_IO_SIGNATURES
{
// A signature is basically the union of all registers input and output by any
// shader sharing the signature.  Thus, a signature may be a superset of what a
// given shader may happen to actually input or output.  Another way to think
// of a signature is that hardware should assume for an input signature that
// the upstream stage in the pipeline may provide some or all the data in the
// signature laid out as specified.  Similarly, hardware should assume for an output
// signature that the downstream stage in the pipeline may consume some or all
// of the data in the signature laid out as specified.
//
// The reason this full signature is passed to the driver is to assist in the event
// input/output registers need to be reordered during shader compilation.
// Such reordering may depend on knowing all of the registers in the signature,
// as well as which ones have system names ("position", "clipDistance" etc),
// including registers that don't happen to be present in the current shader.
//
// The declarations within the shader code itself will show which registers
// are actually used by a particular shader (possibly a subset of these signatures).
// If some hardware doesn't need to reorder input/output registers at
// compile-time, the full signatures provided by this structure can be
// completely ignored.  The reference rasterizer, for example, doens't
// need the information provided here at all.
//
    D3D10DDIARG_SIGNATURE_ENTRY*  pInputSignature;
    UINT                          NumInputSignatureEntries;
    D3D10DDIARG_SIGNATURE_ENTRY*  pOutputSignature;
    UINT                          NumOutputSignatureEntries;
} D3D10DDIARG_STAGE_IO_SIGNATURES;

typedef enum D3D10_DDI_FILTER
{
    // Bits used in defining enumeration of valid filters:
    // bits [1:0] - mip: 0 == point, 1 == linear, 2,3 unused
    // bits [3:2] - mag: 0 == point, 1 == linear, 2,3 unused
    // bits [5:4] - min: 0 == point, 1 == linear, 2,3 unused
    // bit  [6]   - aniso
    // bit  [7]   - comparison
    // bit  [31]  - mono 1-bit (narrow-purpose filter)

    D3D10_DDI_FILTER_MIN_MAG_MIP_POINT                              = 0x00000000,
    D3D10_DDI_FILTER_MIN_MAG_POINT_MIP_LINEAR                       = 0x00000001,
    D3D10_DDI_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT                 = 0x00000004,
    D3D10_DDI_FILTER_MIN_POINT_MAG_MIP_LINEAR                       = 0x00000005,
    D3D10_DDI_FILTER_MIN_LINEAR_MAG_MIP_POINT                       = 0x00000010,
    D3D10_DDI_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR                = 0x00000011,
    D3D10_DDI_FILTER_MIN_MAG_LINEAR_MIP_POINT                       = 0x00000014,
    D3D10_DDI_FILTER_MIN_MAG_MIP_LINEAR                             = 0x00000015,
    D3D10_DDI_FILTER_ANISOTROPIC                                    = 0x00000055,
    D3D10_DDI_FILTER_COMPARISON_MIN_MAG_MIP_POINT                   = 0x00000080,
    D3D10_DDI_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR            = 0x00000081,
    D3D10_DDI_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT      = 0x00000084,
    D3D10_DDI_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR            = 0x00000085,
    D3D10_DDI_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT            = 0x00000090,
    D3D10_DDI_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR     = 0x00000091,
    D3D10_DDI_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT            = 0x00000094,
    D3D10_DDI_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR                  = 0x00000095,
    D3D10_DDI_FILTER_COMPARISON_ANISOTROPIC                         = 0x000000d5,

    D3D10_DDI_FILTER_TEXT_1BIT                                      = 0x80000000 // Only filter for R1_UNORM format

} D3D10_DDI_FILTER;

typedef enum D3D10_DDI_FILTER_TYPE
{
    D3D10_DDI_FILTER_TYPE_POINT = 0,
    D3D10_DDI_FILTER_TYPE_LINEAR = 1,
} D3D10_DDI_FILTER_TYPE;

const UINT D3D10_DDI_FILTER_TYPE_MASK = 0x00000003;
const UINT D3D10_DDI_MIN_FILTER_SHIFT = 4;
const UINT D3D10_DDI_MAG_FILTER_SHIFT = 2;
const UINT D3D10_DDI_MIP_FILTER_SHIFT = 0;
const UINT D3D10_DDI_COMPARISON_FILTERING_BIT = 0x00000080;
const UINT D3D10_DDI_ANISOTROPIC_FILTERING_BIT = 0x00000040;
const UINT D3D10_DDI_TEXT_1BIT_BIT = 0x80000000;

// encode enum entry for most filters except anisotropic filtering
#define D3D10_DDI_ENCODE_BASIC_FILTER( min, mag, mip, bComparison )                                     \
                                   ( D3D10_DDI_FILTER ) (                                               \
                                   ( ( bComparison ) ? D3D10_DDI_COMPARISON_FILTERING_BIT : 0 ) |       \
                                   ( ( min & D3D10_DDI_FILTER_TYPE_MASK ) << D3D10_DDI_MIN_FILTER_SHIFT ) |   \
                                   ( ( mag & D3D10_DDI_FILTER_TYPE_MASK ) << D3D10_DDI_MAG_FILTER_SHIFT ) |   \
                                   ( ( mip & D3D10_DDI_FILTER_TYPE_MASK ) << D3D10_DDI_MIP_FILTER_SHIFT ) )

// encode enum entry for anisotropic filtering (with or without comparison filtering)
#define D3D10_DDI_ENCODE_ANISOTROPIC_FILTER( bComparison )                                              \
                                         ( D3D10_DDI_FILTER ) (                                         \
                                         D3D10_DDI_ANISOTROPIC_FILTERING_BIT |                          \
                                         D3D10_DDI_ENCODE_BASIC_FILTER( D3D10_DDI_FILTER_TYPE_LINEAR,         \
                                                                    D3D10_DDI_FILTER_TYPE_LINEAR,         \
                                                                    D3D10_DDI_FILTER_TYPE_LINEAR,         \
                                                                    bComparison ) )

#define D3D10_DDI_DECODE_MIN_FILTER( d3d10Filter )                                                      \
                                 (D3D10_DDI_FILTER_TYPE)                                                  \
                                 ( ( d3d10Filter >> D3D10_DDI_MIN_FILTER_SHIFT ) & D3D10_DDI_FILTER_TYPE_MASK )

#define D3D10_DDI_DECODE_MAG_FILTER( d3d10Filter )                                                      \
                                 (D3D10_DDI_FILTER_TYPE)                                                  \
                                 ( ( d3d10Filter >> D3D10_DDI_MAG_FILTER_SHIFT ) & D3D10_DDI_FILTER_TYPE_MASK )

#define D3D10_DDI_DECODE_MIP_FILTER( d3d10Filter )                                                      \
                                 (D3D10_DDI_FILTER_TYPE)                                                  \
                                 ( ( d3d10Filter >> D3D10_DDI_MIP_FILTER_SHIFT ) & D3D10_DDI_FILTER_TYPE_MASK )

#define D3D10_DDI_DECODE_IS_COMPARISON_FILTER( d3d10Filter )                                            \
                                 ( d3d10Filter & D3D10_DDI_COMPARISON_FILTERING_BIT )

#define D3D10_DDI_DECODE_IS_ANISOTROPIC_FILTER( d3d10Filter )                                           \
                          ( ( d3d10Filter & D3D10_DDI_ANISOTROPIC_FILTERING_BIT ) &&                    \
                            ( D3D10_DDI_FILTER_TYPE_LINEAR == D3D10_DDI_DECODE_MIN_FILTER( d3d10Filter ) ) && \
                            ( D3D10_DDI_FILTER_TYPE_LINEAR == D3D10_DDI_DECODE_MAG_FILTER( d3d10Filter ) ) && \
                            ( D3D10_DDI_FILTER_TYPE_LINEAR == D3D10_DDI_DECODE_MIP_FILTER( d3d10Filter ) ) )

#define D3D10_DDI_DECODE_IS_TEXT_1BIT_FILTER( d3d10Filter )                                             \
                                 ( d3d10Filter == D3D10_DDI_TEXT_1BIT_BIT )


typedef enum D3D10_DDI_COMPARISON_FUNC
{
    D3D10_DDI_COMPARISON_NEVER = 1,
    D3D10_DDI_COMPARISON_LESS = 2,
    D3D10_DDI_COMPARISON_EQUAL = 3,
    D3D10_DDI_COMPARISON_LESS_EQUAL = 4,
    D3D10_DDI_COMPARISON_GREATER = 5,
    D3D10_DDI_COMPARISON_NOT_EQUAL = 6,
    D3D10_DDI_COMPARISON_GREATER_EQUAL = 7,
    D3D10_DDI_COMPARISON_ALWAYS = 8
} D3D10_DDI_COMPARISON_FUNC;

typedef enum D3D10_DDI_TEXTURE_ADDRESS_MODE
{
    D3D10_DDI_TEXTURE_ADDRESS_WRAP = 1,
    D3D10_DDI_TEXTURE_ADDRESS_MIRROR = 2,
    D3D10_DDI_TEXTURE_ADDRESS_CLAMP = 3,
    D3D10_DDI_TEXTURE_ADDRESS_BORDER = 4,
    D3D10_DDI_TEXTURE_ADDRESS_MIRRORONCE = 5
} D3D10_DDI_TEXTURE_ADDRESS_MODE;

/* TextureCube Face identifiers */
typedef enum D3D10_DDI_TEXTURECUBE_FACE
{
    D3D10_DDI_TEXTURECUBE_FACE_POSITIVE_X = 0,
    D3D10_DDI_TEXTURECUBE_FACE_NEGATIVE_X = 1,
    D3D10_DDI_TEXTURECUBE_FACE_POSITIVE_Y = 2,
    D3D10_DDI_TEXTURECUBE_FACE_NEGATIVE_Y = 3,
    D3D10_DDI_TEXTURECUBE_FACE_POSITIVE_Z = 4,
    D3D10_DDI_TEXTURECUBE_FACE_NEGATIVE_Z = 5
} D3D10_DDI_TEXTURECUBE_FACE;

typedef struct D3D10_DDI_SAMPLER_DESC
{
    D3D10_DDI_FILTER Filter;
    D3D10_DDI_TEXTURE_ADDRESS_MODE AddressU;
    D3D10_DDI_TEXTURE_ADDRESS_MODE AddressV;
    D3D10_DDI_TEXTURE_ADDRESS_MODE AddressW;
    FLOAT MipLODBias;
    UINT MaxAnisotropy;
    D3D10_DDI_COMPARISON_FUNC ComparisonFunc;
    FLOAT BorderColor[4]; // RGBA
    FLOAT MinLOD;
    FLOAT MaxLOD;
} D3D10_DDI_SAMPLER_DESC;

typedef struct D3D10_DDIARG_CREATE_SAMPLER
{
    CONST D3D10_DDI_SAMPLER_DESC*   pSamplerDesc;
} D3D10_DDIARG_CREATE_SAMPLER;

// Flags for ClearDepthStencil
typedef enum D3D10_DDI_CLEAR_FLAG
{
    D3D10_DDI_CLEAR_DEPTH = 0x01L,
    D3D10_DDI_CLEAR_STENCIL = 0x02L,
    D3D10_DDI_CLEAR_FLAG_MASK = 0x03L,
} D3D10_DDI_CLEAR_FLAG;

typedef enum D3D10DDI_QUERY
{
    D3D10DDI_QUERY_EVENT = 0,
    D3D10DDI_QUERY_OCCLUSION,
    D3D10DDI_QUERY_TIMESTAMP,
    D3D10DDI_QUERY_TIMESTAMPDISJOINT,
    D3D10DDI_QUERY_PIPELINESTATS,
    D3D10DDI_QUERY_OCCLUSIONPREDICATE,
    D3D10DDI_QUERY_STREAMOUTPUTSTATS,
    D3D10DDI_QUERY_STREAMOVERFLOWPREDICATE,

    D3D10DDI_COUNTER_GPU_IDLE = 0x1000, // Start of "counters"
    D3D10DDI_COUNTER_VERTEX_PROCESSING,
    D3D10DDI_COUNTER_GEOMETRY_PROCESSING,
    D3D10DDI_COUNTER_PIXEL_PROCESSING,
    D3D10DDI_COUNTER_OTHER_GPU_PROCESSING,
    D3D10DDI_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION,
    D3D10DDI_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION,
    D3D10DDI_COUNTER_VERTEX_THROUGHPUT_UTILIZATION,
    D3D10DDI_COUNTER_TRISETUP_THROUGHPUT_UTILIZATION,
    D3D10DDI_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION,
    D3D10DDI_COUNTER_VERTEXSHADER_MEMORY_LIMITED,
    D3D10DDI_COUNTER_VERTEXSHADER_COMPUTATION_LIMITED,
    D3D10DDI_COUNTER_GEOMETRYSHADER_MEMORY_LIMITED,
    D3D10DDI_COUNTER_GEOMETRYSHADER_COMPUTATION_LIMITED,
    D3D10DDI_COUNTER_PIXELSHADER_MEMORY_LIMITED,
    D3D10DDI_COUNTER_PIXELSHADER_COMPUTATION_LIMITED,
    D3D10DDI_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE,
    D3D10DDI_COUNTER_TEXTURE_CACHE_HIT_RATE,

    D3D10DDI_COUNTER_DEVICE_DEPENDENT_0 = 0x40000000, // Start of "device-dependent counters"
} D3D10DDI_QUERY;

typedef struct D3D10_DDI_QUERY_DATA_TIMESTAMP_DISJOINT
{
    UINT64 Frequency;
    BOOL Disjoint;
} D3D10_DDI_QUERY_DATA_TIMESTAMP_DISJOINT;

typedef struct D3D10_DDI_QUERY_DATA_PIPELINE_STATISTICS
{
    UINT64 IAVertices;
    UINT64 IAPrimitives;
    UINT64 VSInvocations;
    UINT64 GSInvocations;
    UINT64 GSPrimitives;
    UINT64 CInvocations;
    UINT64 CPrimitives;
    UINT64 PSInvocations;
} D3D10_DDI_QUERY_DATA_PIPELINE_STATISTICS;

typedef struct D3D10_DDI_QUERY_DATA_SO_STATISTICS
{
    UINT64 NumPrimitivesWritten;
    UINT64 PrimitivesStorageNeeded;
} D3D10_DDI_QUERY_DATA_SO_STATISTICS;


typedef enum D3D10DDI_COUNTER_TYPE
{
    D3D10DDI_COUNTER_TYPE_FLOAT32,
    D3D10DDI_COUNTER_TYPE_UINT16,
    D3D10DDI_COUNTER_TYPE_UINT32,
    D3D10DDI_COUNTER_TYPE_UINT64,
} D3D10DDI_COUNTER_TYPE;

typedef struct D3D10DDI_COUNTER_INFO
{
    D3D10DDI_QUERY LastDeviceDependentCounter;
    UINT NumSimultaneousCounters;
    UINT8 NumDetectableParallelUnits;
} D3D10DDI_COUNTER_INFO;

#define D3D10DDI_QUERY_MISCFLAG_PREDICATEHINT 0x1

typedef struct D3D10DDIARG_CREATEQUERY
{
    D3D10DDI_QUERY Query;
    UINT MiscFlags;
} D3D10DDIARG_CREATEQUERY;

typedef enum D3D10_DDI_GET_DATA_FLAG
{
    D3D10_DDI_GET_DATA_DO_NOT_FLUSH = 0x01L,
} D3D10_DDI_GET_DATA_FLAG;

typedef struct D3D10DDI_VERTEX_CACHE_DESC
{
    UINT Pattern; /* bit pattern, return value must be FOUR_CC('C', 'A', 'C', 'H') */
    UINT OptMethod; /* optimization method 0 means longest strips, 1 means vertex cache based */
    UINT CacheSize; /* cache size to optimize for (only required if type is 1) */
    UINT MagicNumber; /* used to determine when to restart strips (only required if type is 1)*/
} D3D10DDI_VERTEX_CACHE_DESC;


// Keep PRIMITIVE_TOPOLOGY values in sync with earlier DX versions (HW consumes values directly).
typedef enum D3D10_DDI_PRIMITIVE_TOPOLOGY
{
    D3D10_DDI_PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_POINTLIST = 1,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_LINELIST = 2,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
    // 6 is reserved for legacy triangle fans
    // Adjacency values should be equal to (0x8 & non-adjacency):
    D3D10_DDI_PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 10,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 11,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 12,
    D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 13,
} D3D10_DDI_PRIMITIVE_TOPOLOGY;

typedef enum D3D10_DDI_PRIMITIVE
{
    D3D10_DDI_PRIMITIVE_UNDEFINED = 0,
    D3D10_DDI_PRIMITIVE_POINT = 1,
    D3D10_DDI_PRIMITIVE_LINE = 2,
    D3D10_DDI_PRIMITIVE_TRIANGLE = 3,
    // Adjacency values should be equal to (0x4 & non-adjacency):
    D3D10_DDI_PRIMITIVE_LINE_ADJ = 6,
    D3D10_DDI_PRIMITIVE_TRIANGLE_ADJ = 7,
} D3D10_DDI_PRIMITIVE;

typedef struct D3D10_DDI_VIEWPORT
{
    FLOAT TopLeftX;
    FLOAT TopLeftY;
    FLOAT Width;
    FLOAT Height;
    FLOAT MinDepth;
    FLOAT MaxDepth;
} D3D10_DDI_VIEWPORT;

typedef RECT D3D10_DDI_RECT;

typedef struct D3D10_DDI_BOX
{
    LONG left;
    LONG top;
    LONG front;
    LONG right;
    LONG bottom;
    LONG back;
} D3D10_DDI_BOX;

typedef enum D3D10_DDI_DEPTH_WRITE_MASK
{
    D3D10_DDI_DEPTH_WRITE_MASK_ZERO = 0,
    D3D10_DDI_DEPTH_WRITE_MASK_ALL = 1
} D3D10_DDI_DEPTH_WRITE_MASK;

// Keep STENCILOP values in sync with earlier DX versions (HW consumes values directly).
typedef enum D3D10_DDI_STENCIL_OP
{
    D3D10_DDI_STENCIL_OP_KEEP = 1,
    D3D10_DDI_STENCIL_OP_ZERO = 2,
    D3D10_DDI_STENCIL_OP_REPLACE = 3,
    D3D10_DDI_STENCIL_OP_INCR_SAT = 4,
    D3D10_DDI_STENCIL_OP_DECR_SAT = 5,
    D3D10_DDI_STENCIL_OP_INVERT = 6,
    D3D10_DDI_STENCIL_OP_INCR = 7,
    D3D10_DDI_STENCIL_OP_DECR = 8
} D3D10_DDI_STENCIL_OP;

typedef struct D3D10_DDI_DEPTH_STENCILOP_DESC
{
    D3D10_DDI_STENCIL_OP StencilFailOp;
    D3D10_DDI_STENCIL_OP StencilDepthFailOp;
    D3D10_DDI_STENCIL_OP StencilPassOp;
    D3D10_DDI_COMPARISON_FUNC StencilFunc;
} D3D10_DDI_DEPTH_STENCILOP_DESC;

typedef struct D3D10_DDI_DEPTH_STENCIL_DESC
{
    BOOL DepthEnable;
    D3D10_DDI_DEPTH_WRITE_MASK DepthWriteMask;
    D3D10_DDI_COMPARISON_FUNC DepthFunc;
    BOOL StencilEnable;
    BOOL FrontEnable;
    BOOL BackEnable;
    UINT8 StencilReadMask;
    UINT8 StencilWriteMask;
    D3D10_DDI_DEPTH_STENCILOP_DESC FrontFace;
    D3D10_DDI_DEPTH_STENCILOP_DESC BackFace;
} D3D10_DDI_DEPTH_STENCIL_DESC;

// Keep FILL_MODE values in sync with earlier DX versions (HW consumes values directly).
typedef enum D3D10_DDI_FILL_MODE
{
    // 1 was POINT in D3D, unused in D3D10
    D3D10_DDI_FILL_WIREFRAME = 2,
    D3D10_DDI_FILL_SOLID = 3
} D3D10_DDI_FILL_MODE;

// Keep CULL_MODE values in sync with earlier DX versions (HW consumes values directly).
typedef enum D3D10_DDI_CULL_MODE
{
    D3D10_DDI_CULL_NONE = 1,
    D3D10_DDI_CULL_FRONT = 2,
    D3D10_DDI_CULL_BACK = 3
} D3D10_DDI_CULL_MODE;

typedef enum D3D10_DDI_FRONT_WINDING
{
    D3D10_DDI_FRONT_CW = 1,
    D3D10_DDI_FRONT_CCW = 2
} D3D10_DDI_FRONT_WINDING;

typedef struct D3D10_DDI_RASTERIZER_DESC
{
    D3D10_DDI_FILL_MODE FillMode;
    D3D10_DDI_CULL_MODE CullMode;
    BOOL FrontCounterClockwise;
    INT32 DepthBias;
    FLOAT DepthBiasClamp;
    FLOAT SlopeScaledDepthBias;
    BOOL DepthClipEnable;
    BOOL ScissorEnable;
    BOOL MultisampleEnable;
    BOOL AntialiasedLineEnable;
} D3D10_DDI_RASTERIZER_DESC;

// Keep BLEND values in sync with earlier DX versions (HW consumes values directly).
typedef enum D3D10_DDI_BLEND
{
    D3D10_DDI_BLEND_ZERO = 1,
    D3D10_DDI_BLEND_ONE = 2,
    D3D10_DDI_BLEND_SRC_COLOR = 3, // PS output oN.rgb (N is current RT being blended)
    D3D10_DDI_BLEND_INV_SRC_COLOR = 4, // 1.0f - PS output oN.rgb
    D3D10_DDI_BLEND_SRC_ALPHA = 5, // PS output oN.a
    D3D10_DDI_BLEND_INV_SRC_ALPHA = 6, // 1.0f - PS output oN.a
    D3D10_DDI_BLEND_DEST_ALPHA = 7, // RT(N).a (N is current RT being blended)
    D3D10_DDI_BLEND_INV_DEST_ALPHA = 8, // 1.0f - RT(N).a
    D3D10_DDI_BLEND_DEST_COLOR = 9, // RT(N).rgb
    D3D10_DDI_BLEND_INV_DEST_COLOR = 10,// 1.0f - RT(N).rgb
    D3D10_DDI_BLEND_SRC_ALPHASAT = 11,// (f,f,f,1), f = min(1 - RT(N).a, oN.a)
    // 12 reserved (was BOTHSRCALPHA)
    // 13 reserved (was BOTH_INV_SRC_ALPHA)
    D3D10_DDI_BLEND_BLEND_FACTOR = 14,
    D3D10_DDI_BLEND_INVBLEND_FACTOR = 15,
    D3D10_DDI_BLEND_SRC1_COLOR = 16, // PS output o1.rgb
    D3D10_DDI_BLEND_INV_SRC1_COLOR = 17, // 1.0f - PS output o1.rgb
    D3D10_DDI_BLEND_SRC1_ALPHA = 18, // PS output o1.a
    D3D10_DDI_BLEND_INV_SRC1_ALPHA = 19, // 1.0f - PS output o1.a
} D3D10_DDI_BLEND;

// Keep BLENDOP values in sync with earlier DX versions (HW consumes values directly).
typedef enum D3D10_DDI_BLEND_OP
{
    D3D10_DDI_BLEND_OP_ADD = 1,
    D3D10_DDI_BLEND_OP_SUBTRACT = 2,
    D3D10_DDI_BLEND_OP_REV_SUBTRACT = 3,
    D3D10_DDI_BLEND_OP_MIN = 4, // min semantics are like min shader instruction
    D3D10_DDI_BLEND_OP_MAX = 5, // max semantics are like max shader instruction
} D3D10_DDI_BLEND_OP;

typedef enum D3D10_DDI_COLOR_WRITE_ENABLE
{
    D3D10_DDI_COLOR_WRITE_ENABLE_RED = 1,
    D3D10_DDI_COLOR_WRITE_ENABLE_GREEN = 2,
    D3D10_DDI_COLOR_WRITE_ENABLE_BLUE = 4,
    D3D10_DDI_COLOR_WRITE_ENABLE_ALPHA = 8,
    D3D10_DDI_COLOR_WRITE_ENABLE_ALL = (D3D10_DDI_COLOR_WRITE_ENABLE_RED|D3D10_DDI_COLOR_WRITE_ENABLE_GREEN|
                                        D3D10_DDI_COLOR_WRITE_ENABLE_BLUE|D3D10_DDI_COLOR_WRITE_ENABLE_ALPHA),
} D3D10_DDI_COLOR_WRITE_ENABLE;

#define D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT 8

typedef struct D3D10_DDI_BLEND_DESC
{
    BOOL AlphaToCoverageEnable; // relevant to multisample antialiasing only
    BOOL BlendEnable[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];
    D3D10_DDI_BLEND SrcBlend; // same for all RenderTargets
    D3D10_DDI_BLEND DestBlend; // same for all RenderTargets
    D3D10_DDI_BLEND_OP BlendOp; // same for all RenderTargets
    D3D10_DDI_BLEND SrcBlendAlpha; // same for all RenderTargets
    D3D10_DDI_BLEND DestBlendAlpha; // same for all RenderTargets
    D3D10_DDI_BLEND_OP BlendOpAlpha; // same for all RenderTargets
    UINT8 RenderTargetWriteMask[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];
} D3D10_DDI_BLEND_DESC;

//----------------------------------------------------------------------------------------------------------------------------------
// User mode DDI device function definitions
//
typedef VOID ( APIENTRY* PFND3D10DDI_DRAW )(
    D3D10DDI_HDEVICE, UINT, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_DRAWINDEXED )(
    D3D10DDI_HDEVICE, UINT, UINT, INT );
typedef VOID ( APIENTRY* PFND3D10DDI_DRAWINSTANCED )(
    D3D10DDI_HDEVICE, UINT, UINT, UINT, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_DRAWINDEXEDINSTANCED )(
    D3D10DDI_HDEVICE, UINT, UINT, UINT, INT, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_DRAWAUTO )(
    D3D10DDI_HDEVICE );
typedef VOID ( APIENTRY* PFND3D10DDI_IA_SETTOPOLOGY )(
    D3D10DDI_HDEVICE, D3D10_DDI_PRIMITIVE_TOPOLOGY );
typedef VOID ( APIENTRY* PFND3D10DDI_IA_SETVERTEXBUFFERS )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10DDI_HRESOURCE*, CONST UINT*, CONST UINT* );
typedef VOID ( APIENTRY* PFND3D10DDI_IA_SETINDEXBUFFER )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, DXGI_FORMAT, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_SETSHADER )(
    D3D10DDI_HDEVICE, D3D10DDI_HSHADER );
typedef VOID ( APIENTRY* PFND3D10DDI_SETINPUTLAYOUT )(
    D3D10DDI_HDEVICE, D3D10DDI_HELEMENTLAYOUT );
typedef VOID ( APIENTRY* PFND3D10DDI_SETSHADERRESOURCES )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10DDI_HSHADERRESOURCEVIEW* );
typedef VOID ( APIENTRY* PFND3D10DDI_SETCONSTANTBUFFERS )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10DDI_HRESOURCE* );
typedef VOID ( APIENTRY* PFND3D10DDI_SETSAMPLERS )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10DDI_HSAMPLER* );
typedef VOID ( APIENTRY* PFND3D10DDI_SO_SETTARGETS )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10DDI_HRESOURCE*, CONST UINT* );
typedef VOID ( APIENTRY* PFND3D10DDI_SETBLENDSTATE )(
    D3D10DDI_HDEVICE, D3D10DDI_HBLENDSTATE, const FLOAT[4], UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_SETDEPTHSTENCILSTATE )(
    D3D10DDI_HDEVICE, D3D10DDI_HDEPTHSTENCILSTATE, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_SETRASTERIZERSTATE )(
    D3D10DDI_HDEVICE, D3D10DDI_HRASTERIZERSTATE );
typedef VOID ( APIENTRY* PFND3D10DDI_SETVIEWPORTS )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10_DDI_VIEWPORT* );
typedef VOID ( APIENTRY* PFND3D10DDI_SETSCISSORRECTS )(
    D3D10DDI_HDEVICE, UINT, UINT, CONST D3D10_DDI_RECT* );
typedef VOID ( APIENTRY* PFND3D10DDI_SETRENDERTARGETS )(
    D3D10DDI_HDEVICE, CONST D3D10DDI_HRENDERTARGETVIEW*, UINT, UINT, D3D10DDI_HDEPTHSTENCILVIEW );
typedef VOID ( APIENTRY* PFND3D10DDI_SETPREDICATION )(
    D3D10DDI_HDEVICE, D3D10DDI_HQUERY, BOOL );
typedef VOID ( APIENTRY* PFND3D10DDI_QUERYBEGIN )(
    D3D10DDI_HDEVICE, D3D10DDI_HQUERY );
typedef VOID ( APIENTRY* PFND3D10DDI_QUERYEND )(
    D3D10DDI_HDEVICE, D3D10DDI_HQUERY );
typedef VOID ( APIENTRY* PFND3D10DDI_QUERYGETDATA )(
    D3D10DDI_HDEVICE, D3D10DDI_HQUERY, VOID*, UINT, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_CLEARRENDERTARGETVIEW )(
    D3D10DDI_HDEVICE, D3D10DDI_HRENDERTARGETVIEW, FLOAT[4] );
typedef VOID ( APIENTRY* PFND3D10DDI_CLEARDEPTHSTENCILVIEW )(
    D3D10DDI_HDEVICE, D3D10DDI_HDEPTHSTENCILVIEW, UINT, FLOAT, UINT8 );
typedef VOID ( APIENTRY* PFND3D10DDI_FLUSH )(
    D3D10DDI_HDEVICE );
typedef VOID ( APIENTRY* PFND3D10DDI_GENMIPS )(
    D3D10DDI_HDEVICE, D3D10DDI_HSHADERRESOURCEVIEW );

typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCEMAP )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, UINT, D3D10_DDI_MAP, UINT, D3D10DDI_MAPPED_SUBRESOURCE* );
typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCEUNMAP )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE );
typedef VOID ( APIENTRY* PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD )(
    D3D10DDI_HDEVICE, D3D10DDI_HSHADERRESOURCEVIEW, D3D10DDI_HRESOURCE );
typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCECOPYREGION )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, UINT, UINT, UINT, UINT, D3D10DDI_HRESOURCE, UINT, CONST D3D10_DDI_BOX* );
typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCECOPY )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, D3D10DDI_HRESOURCE );
typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCERESOLVESUBRESOURCE )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, UINT, D3D10DDI_HRESOURCE, UINT, DXGI_FORMAT );
typedef VOID ( APIENTRY* PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE, UINT, CONST D3D10_DDI_BOX*, CONST VOID*, UINT, UINT );
typedef VOID ( APIENTRY* PFND3D10DDI_SETTEXTFILTERSIZE )(
    D3D10DDI_HDEVICE, UINT, UINT );

// Infrequent paths:
typedef BOOL ( APIENTRY* PFND3D10DDI_RESOURCEISSTAGINGBUSY )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE );
typedef VOID ( APIENTRY* PFND3D10DDI_RELOCATEDEVICEFUNCS )(
    D3D10DDI_HDEVICE, struct D3D10DDI_DEVICEFUNCS* );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATERESOURCESIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATERESOURCE* );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_OPENRESOURCE* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATERESOURCE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATERESOURCE*, D3D10DDI_HRESOURCE, D3D10DDI_HRTRESOURCE );
typedef VOID ( APIENTRY* PFND3D10DDI_OPENRESOURCE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_OPENRESOURCE*, D3D10DDI_HRESOURCE, D3D10DDI_HRTRESOURCE );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYRESOURCE )(
    D3D10DDI_HDEVICE, D3D10DDI_HRESOURCE );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATESHADERRESOURCEVIEW )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW*, D3D10DDI_HSHADERRESOURCEVIEW, D3D10DDI_HRTSHADERRESOURCEVIEW );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYSHADERRESOURCEVIEW )(
    D3D10DDI_HDEVICE, D3D10DDI_HSHADERRESOURCEVIEW );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATERENDERTARGETVIEW* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATERENDERTARGETVIEW )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATERENDERTARGETVIEW*, D3D10DDI_HRENDERTARGETVIEW, D3D10DDI_HRTRENDERTARGETVIEW );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYRENDERTARGETVIEW )(
    D3D10DDI_HDEVICE, D3D10DDI_HRENDERTARGETVIEW );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEDEPTHSTENCILVIEW )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW*, D3D10DDI_HDEPTHSTENCILVIEW, D3D10DDI_HRTDEPTHSTENCILVIEW );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYDEPTHSTENCILVIEW )(
    D3D10DDI_HDEVICE, D3D10DDI_HDEPTHSTENCILVIEW );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEELEMENTLAYOUT* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEELEMENTLAYOUT )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEELEMENTLAYOUT*, D3D10DDI_HELEMENTLAYOUT, D3D10DDI_HRTELEMENTLAYOUT );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYELEMENTLAYOUT )(
    D3D10DDI_HDEVICE, D3D10DDI_HELEMENTLAYOUT );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_BLEND_DESC* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEBLENDSTATE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_BLEND_DESC*, D3D10DDI_HBLENDSTATE, D3D10DDI_HRTBLENDSTATE );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYBLENDSTATE )(
    D3D10DDI_HDEVICE, D3D10DDI_HBLENDSTATE );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_DEPTH_STENCIL_DESC* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEDEPTHSTENCILSTATE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_DEPTH_STENCIL_DESC*, D3D10DDI_HDEPTHSTENCILSTATE, D3D10DDI_HRTDEPTHSTENCILSTATE );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYDEPTHSTENCILSTATE )(
    D3D10DDI_HDEVICE, D3D10DDI_HDEPTHSTENCILSTATE );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_RASTERIZER_DESC* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATERASTERIZERSTATE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_RASTERIZER_DESC*, D3D10DDI_HRASTERIZERSTATE, D3D10DDI_HRTRASTERIZERSTATE );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYRASTERIZERSTATE )(
    D3D10DDI_HDEVICE, D3D10DDI_HRASTERIZERSTATE );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATESHADERSIZE )(
    D3D10DDI_HDEVICE, CONST UINT*,CONST D3D10DDIARG_STAGE_IO_SIGNATURES* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEVERTEXSHADER )(
    D3D10DDI_HDEVICE, CONST UINT*, D3D10DDI_HSHADER, D3D10DDI_HRTSHADER, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEGEOMETRYSHADER )(
    D3D10DDI_HDEVICE, CONST UINT*, D3D10DDI_HSHADER, D3D10DDI_HRTSHADER, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEPIXELSHADER )(
    D3D10DDI_HDEVICE, CONST UINT*, D3D10DDI_HSHADER, D3D10DDI_HRTSHADER, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*, D3D10DDI_HSHADER, D3D10DDI_HRTSHADER, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYSHADER )(
    D3D10DDI_HDEVICE, D3D10DDI_HSHADER );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATESAMPLERSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_SAMPLER_DESC* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATESAMPLER )(
    D3D10DDI_HDEVICE, CONST D3D10_DDI_SAMPLER_DESC*, D3D10DDI_HSAMPLER, D3D10DDI_HRTSAMPLER );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYSAMPLER )(
    D3D10DDI_HDEVICE, D3D10DDI_HSAMPLER );
typedef SIZE_T ( APIENTRY* PFND3D10DDI_CALCPRIVATEQUERYSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEQUERY* );
typedef VOID ( APIENTRY* PFND3D10DDI_CREATEQUERY )(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_CREATEQUERY*, D3D10DDI_HQUERY, D3D10DDI_HRTQUERY );
typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYQUERY )(
    D3D10DDI_HDEVICE, D3D10DDI_HQUERY );

typedef VOID ( APIENTRY* PFND3D10DDI_CHECKFORMATSUPPORT )(
    D3D10DDI_HDEVICE, DXGI_FORMAT, UINT* );
typedef VOID ( APIENTRY* PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS )(
    D3D10DDI_HDEVICE, DXGI_FORMAT, UINT, UINT* );
typedef VOID ( APIENTRY* PFND3D10DDI_CHECKVERTEXCACHE )(
    D3D10DDI_HDEVICE, D3D10DDI_VERTEX_CACHE_DESC* );
typedef VOID ( APIENTRY* PFND3D10DDI_CHECKCOUNTERINFO )(
    D3D10DDI_HDEVICE, D3D10DDI_COUNTER_INFO* );
typedef VOID ( APIENTRY* PFND3D10DDI_CHECKCOUNTER )(
    D3D10DDI_HDEVICE, D3D10DDI_QUERY, D3D10DDI_COUNTER_TYPE*, UINT*, LPSTR, UINT*, LPSTR, UINT*, LPSTR, UINT* );

typedef VOID ( APIENTRY* PFND3D10DDI_DESTROYDEVICE )(
    D3D10DDI_HDEVICE );

#ifdef D3D10PSGP
// Rasterization-only specific:
//
typedef enum D3D10_DDI_INTERPOLATION_MODE
{
    D3D10_DDI_INTERPOLATION_UNDEFINED = 0,
    D3D10_DDI_INTERPOLATION_CONSTANT = 1,
    D3D10_DDI_INTERPOLATION_LINEAR = 2,
    D3D10_DDI_INTERPOLATION_LINEAR_CENTROID = 3,
    D3D10_DDI_INTERPOLATION_LINEAR_NOPERSPECTIVE = 4,
    D3D10_DDI_INTERPOLATION_LINEAR_NOPERSPECTIVE_CENTROID = 5,
    D3D10_DDI_INTERPOLATION_LINEAR_SAMPLE = 6, // DX10.1
    D3D10_DDI_INTERPOLATION_LINEAR_NOPERSPECTIVE_SAMPLE = 7, // DX10.1
} D3D10_DDI_INTERPOLATION_MODE;

typedef struct D3D10DDIARG_REGISTER_DESC
{
    D3D10_DDI_INTERPOLATION_MODE    InterpolationMode;
    // One bit for each register component, starting from LSB
    UINT                        UsageMask;
} D3D10DDIARG_REGISTER_DESC;

typedef struct D3D10DDIARG_VERTEXPIPELINEOUTPUT
{
    // The number of 4-component registers in the vertex data
    UINT                        NumRegisters;
    // The number of clip distances in the vertex data
    UINT                        NumClipDistances;
    // The number of cull distances in the vertex data
    UINT                        NumCullDistances;
    // The position offset in bytes in the vertex data
    UINT                        PositionOffset;
    // The viewport index offset in bytes in the vertex data. -1 when it is not defined
    UINT                        ViewportIndexOffset;
    // The render target array index offset in bytes  in the vertex data. -1 when it is not defined
    UINT                        RenderTargetArrayIndexOffset;
    // Pointer to the register descriptors. The number of descriptors is NumRegisters.
    CONST D3D10DDIARG_REGISTER_DESC*  pRegisterDescs;
    // Pointer to the array of offsets in bytes in the vertex data for clip distances.
    // The number of elements in the array is NumClipDistances.
    CONST UINT*                 pClipDistanceOffsets;
    // Pointer to the array of offsets in bytes in the vertex data for cull distances.
    // The number of elements in the array is NumCullDistances.
    CONST UINT*                 pCullDistanceOffsets;
} D3D10DDIARG_VERTEXPIPELINEOUTPUT;

typedef VOID ( APIENTRY* PFND3D10DDI_RESETPRIMITIVEID)(
    D3D10DDI_HDEVICE, BOOL );
typedef VOID ( APIENTRY* PFND3D10DDI_SETVERTEXPIPELINEOUTPUT)(
    D3D10DDI_HDEVICE, CONST D3D10DDIARG_VERTEXPIPELINEOUTPUT* );

#endif // D3D10PSGP

//----------------------------------------------------------------------------------------------------------------------------------
// User mode device function table
//
typedef struct D3D10DDI_DEVICEFUNCS
{
// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
// !!! BEGIN HIGH-FREQUENCY !!!
    PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP               pfnDefaultConstantBufferUpdateSubresourceUP;
    PFND3D10DDI_SETCONSTANTBUFFERS                        pfnVsSetConstantBuffers;
    PFND3D10DDI_SETSHADERRESOURCES                        pfnPsSetShaderResources;
    PFND3D10DDI_SETSHADER                                 pfnPsSetShader;
    PFND3D10DDI_SETSAMPLERS                               pfnPsSetSamplers;
    PFND3D10DDI_SETSHADER                                 pfnVsSetShader;
    PFND3D10DDI_DRAWINDEXED                               pfnDrawIndexed;
    PFND3D10DDI_DRAW                                      pfnDraw;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicIABufferMapNoOverwrite;
    PFND3D10DDI_RESOURCEUNMAP                             pfnDynamicIABufferUnmap;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicConstantBufferMapDiscard;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicIABufferMapDiscard;
    PFND3D10DDI_RESOURCEUNMAP                             pfnDynamicConstantBufferUnmap;
    PFND3D10DDI_SETCONSTANTBUFFERS                        pfnPsSetConstantBuffers;
    PFND3D10DDI_SETINPUTLAYOUT                            pfnIaSetInputLayout;
    PFND3D10DDI_IA_SETVERTEXBUFFERS                       pfnIaSetVertexBuffers;
    PFND3D10DDI_IA_SETINDEXBUFFER                         pfnIaSetIndexBuffer;
// !!! END HIGH-FREQUENCY !!!

// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
// !!! BEGIN MIDDLE-FREQUENCY !!!
    PFND3D10DDI_DRAWINDEXEDINSTANCED                      pfnDrawIndexedInstanced;
    PFND3D10DDI_DRAWINSTANCED                             pfnDrawInstanced;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicResourceMapDiscard;
    PFND3D10DDI_RESOURCEUNMAP                             pfnDynamicResourceUnmap;
    PFND3D10DDI_SETCONSTANTBUFFERS                        pfnGsSetConstantBuffers;
    PFND3D10DDI_SETSHADER                                 pfnGsSetShader;
    PFND3D10DDI_IA_SETTOPOLOGY                            pfnIaSetTopology;
    PFND3D10DDI_RESOURCEMAP                               pfnStagingResourceMap;
    PFND3D10DDI_RESOURCEUNMAP                             pfnStagingResourceUnmap;
    PFND3D10DDI_SETSHADERRESOURCES                        pfnVsSetShaderResources;
    PFND3D10DDI_SETSAMPLERS                               pfnVsSetSamplers;
    PFND3D10DDI_SETSHADERRESOURCES                        pfnGsSetShaderResources;
    PFND3D10DDI_SETSAMPLERS                               pfnGsSetSamplers;
    PFND3D10DDI_SETRENDERTARGETS                          pfnSetRenderTargets;
    PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD    pfnShaderResourceViewReadAfterWriteHazard;
    PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD              pfnResourceReadAfterWriteHazard;
    PFND3D10DDI_SETBLENDSTATE                             pfnSetBlendState;
    PFND3D10DDI_SETDEPTHSTENCILSTATE                      pfnSetDepthStencilState;
    PFND3D10DDI_SETRASTERIZERSTATE                        pfnSetRasterizerState;
    PFND3D10DDI_QUERYEND                                  pfnQueryEnd;
    PFND3D10DDI_QUERYBEGIN                                pfnQueryBegin;
    PFND3D10DDI_RESOURCECOPYREGION                        pfnResourceCopyRegion;
    PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP               pfnResourceUpdateSubresourceUP;
    PFND3D10DDI_SO_SETTARGETS                             pfnSoSetTargets;
    PFND3D10DDI_DRAWAUTO                                  pfnDrawAuto;
    PFND3D10DDI_SETVIEWPORTS                              pfnSetViewports;
    PFND3D10DDI_SETSCISSORRECTS                           pfnSetScissorRects;
    PFND3D10DDI_CLEARRENDERTARGETVIEW                     pfnClearRenderTargetView;
    PFND3D10DDI_CLEARDEPTHSTENCILVIEW                     pfnClearDepthStencilView;
    PFND3D10DDI_SETPREDICATION                            pfnSetPredication;
    PFND3D10DDI_QUERYGETDATA                              pfnQueryGetData;
    PFND3D10DDI_FLUSH                                     pfnFlush;
    PFND3D10DDI_GENMIPS                                   pfnGenMips;
    PFND3D10DDI_RESOURCECOPY                              pfnResourceCopy;
    PFND3D10DDI_RESOURCERESOLVESUBRESOURCE                pfnResourceResolveSubresource;
// !!! END MIDDLE-FREQUENCY !!!

// Infrequent paths:
    PFND3D10DDI_RESOURCEMAP                               pfnResourceMap;
    PFND3D10DDI_RESOURCEUNMAP                             pfnResourceUnmap;
    PFND3D10DDI_RESOURCEISSTAGINGBUSY                     pfnResourceIsStagingBusy;
    PFND3D10DDI_RELOCATEDEVICEFUNCS                       pfnRelocateDeviceFuncs;
    PFND3D10DDI_CALCPRIVATERESOURCESIZE                   pfnCalcPrivateResourceSize;
    PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE             pfnCalcPrivateOpenedResourceSize;
    PFND3D10DDI_CREATERESOURCE                            pfnCreateResource;
    PFND3D10DDI_OPENRESOURCE                              pfnOpenResource;
    PFND3D10DDI_DESTROYRESOURCE                           pfnDestroyResource;
    PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE         pfnCalcPrivateShaderResourceViewSize;
    PFND3D10DDI_CREATESHADERRESOURCEVIEW                  pfnCreateShaderResourceView;
    PFND3D10DDI_DESTROYSHADERRESOURCEVIEW                 pfnDestroyShaderResourceView;
    PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE           pfnCalcPrivateRenderTargetViewSize;
    PFND3D10DDI_CREATERENDERTARGETVIEW                    pfnCreateRenderTargetView;
    PFND3D10DDI_DESTROYRENDERTARGETVIEW                   pfnDestroyRenderTargetView;
    PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE           pfnCalcPrivateDepthStencilViewSize;
    PFND3D10DDI_CREATEDEPTHSTENCILVIEW                    pfnCreateDepthStencilView;
    PFND3D10DDI_DESTROYDEPTHSTENCILVIEW                   pfnDestroyDepthStencilView;
    PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE              pfnCalcPrivateElementLayoutSize;
    PFND3D10DDI_CREATEELEMENTLAYOUT                       pfnCreateElementLayout;
    PFND3D10DDI_DESTROYELEMENTLAYOUT                      pfnDestroyElementLayout;
    PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE                 pfnCalcPrivateBlendStateSize;
    PFND3D10DDI_CREATEBLENDSTATE                          pfnCreateBlendState;
    PFND3D10DDI_DESTROYBLENDSTATE                         pfnDestroyBlendState;
    PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE          pfnCalcPrivateDepthStencilStateSize;
    PFND3D10DDI_CREATEDEPTHSTENCILSTATE                   pfnCreateDepthStencilState;
    PFND3D10DDI_DESTROYDEPTHSTENCILSTATE                  pfnDestroyDepthStencilState;
    PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE            pfnCalcPrivateRasterizerStateSize;
    PFND3D10DDI_CREATERASTERIZERSTATE                     pfnCreateRasterizerState;
    PFND3D10DDI_DESTROYRASTERIZERSTATE                    pfnDestroyRasterizerState;
    PFND3D10DDI_CALCPRIVATESHADERSIZE                     pfnCalcPrivateShaderSize;
    PFND3D10DDI_CREATEVERTEXSHADER                        pfnCreateVertexShader;
    PFND3D10DDI_CREATEGEOMETRYSHADER                      pfnCreateGeometryShader;
    PFND3D10DDI_CREATEPIXELSHADER                         pfnCreatePixelShader;
    PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT pfnCalcPrivateGeometryShaderWithStreamOutput;
    PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT      pfnCreateGeometryShaderWithStreamOutput;
    PFND3D10DDI_DESTROYSHADER                             pfnDestroyShader;
    PFND3D10DDI_CALCPRIVATESAMPLERSIZE                    pfnCalcPrivateSamplerSize;
    PFND3D10DDI_CREATESAMPLER                             pfnCreateSampler;
    PFND3D10DDI_DESTROYSAMPLER                            pfnDestroySampler;
    PFND3D10DDI_CALCPRIVATEQUERYSIZE                      pfnCalcPrivateQuerySize;
    PFND3D10DDI_CREATEQUERY                               pfnCreateQuery;
    PFND3D10DDI_DESTROYQUERY                              pfnDestroyQuery;

    PFND3D10DDI_CHECKFORMATSUPPORT                        pfnCheckFormatSupport;
    PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS             pfnCheckMultisampleQualityLevels;
    PFND3D10DDI_CHECKCOUNTERINFO                          pfnCheckCounterInfo;
    PFND3D10DDI_CHECKCOUNTER                              pfnCheckCounter;

    PFND3D10DDI_DESTROYDEVICE                             pfnDestroyDevice;
    PFND3D10DDI_SETTEXTFILTERSIZE                         pfnSetTextFilterSize;

#ifdef D3D10PSGP
// Rasterization-only specific:
    PFND3D10DDI_RESETPRIMITIVEID                          pfnResetPrimitiveID;
    PFND3D10DDI_SETVERTEXPIPELINEOUTPUT                   pfnSetVertexPipelineOutput;
#endif // D3D10PSGP

} D3D10DDI_DEVICEFUNCS;

#if D3D10DDI_MINOR_HEADER_VERSION >= 1
//----------------------------------------------------------------------------------------------------------------------------------
// User mode function argument definitions 10.1
//

// The Quality Level value for standard multisample pattern is D3D10_1_DDIARG_STANDARD_MULTISAMPLE_PATTERN.
// To expose support for the standard multisample pattern for a given sample count, the driver
// needs to expose at least one standard quality level via CheckMultisampleQualityLevels,
// and that will allow D3D10_1_DDIARG_STANDARD_MULTISAMPLE_PATTERN to be used.  If the vendor
// has no proprietary sample pattern they wish to expose, just the standard pattern, then
// they can just implement the standard pattern for both quality level 0 as well as
// quality level D3D10_1_DDIARG_STANDARD_MULTISAMPLE_PATTERN.  In this case, CheckMultisampleQualityLevels
// would return 1, which allows applications to to request quality level 0 or D3D10_1_DDIARG_STANDARD_MULTISAMPLE_PATTERN,
// and both will give the same behavior.
//
// For every sample count where STANDARD_MULTISAMPLE_PATTERN is supported, a sibling pattern
// must be supported: D3D10_1_DDIARG_CENTER_MULTISAMPLE_PATTERN, which just has the same number of samples,
// except they all overlap the center of the pixel.
//
// This enum matches D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS in the sdk header.
typedef enum D3D10_1_DDIARG_STANDARD_MULTISAMPLE_QUALITY_LEVELS
{
    D3D10_1_DDIARG_STANDARD_MULTISAMPLE_PATTERN = 0xffffffff,
    D3D10_1_DDIARG_CENTER_MULTISAMPLE_PATTERN = 0xfffffffe
} D3D10_1_DDIARG_STANDARD_MULTISAMPLE_QUALITY_LEVELS;

typedef struct D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW
{
    UINT MostDetailedMip;
    UINT MipLevels;
    UINT First2DArrayFace;
    UINT NumCubes;
} D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW;

typedef struct D3D10_1DDIARG_CREATESHADERRESOURCEVIEW
{
    D3D10DDI_HRESOURCE    hDrvResource;
    DXGI_FORMAT           Format;
    D3D10DDIRESOURCE_TYPE ResourceDimension;

    union
    {
        D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW    Buffer;
        D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW     Tex1D;
        D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW     Tex2D;
        D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW     Tex3D;
        D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW TexCube;
    };
} D3D10_1DDIARG_CREATESHADERRESOURCEVIEW;

typedef struct D3D10_DDI_RENDER_TARGET_BLEND_DESC1
{
    BOOL BlendEnable;
    D3D10_DDI_BLEND SrcBlend;
    D3D10_DDI_BLEND DestBlend;
    D3D10_DDI_BLEND_OP BlendOp;
    D3D10_DDI_BLEND SrcBlendAlpha;
    D3D10_DDI_BLEND DestBlendAlpha;
    D3D10_DDI_BLEND_OP BlendOpAlpha;
    UINT8 RenderTargetWriteMask; // D3D10_DDI_COLOR_WRITE_ENABLE
} D3D10_DDI_RENDER_TARGET_BLEND_DESC1;

typedef struct D3D10_1_DDI_BLEND_DESC
{
    BOOL AlphaToCoverageEnable; // relevant to multisample antialiasing only
    BOOL IndependentBlendEnable; // if FALSE, then first entry in RenderTarget array is replicated to other entries
    D3D10_DDI_RENDER_TARGET_BLEND_DESC1 RenderTarget[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];
} D3D10_1_DDI_BLEND_DESC;

//----------------------------------------------------------------------------------------------------------------------------------
// User mode DDI device function definitions 10.1
//
typedef VOID ( APIENTRY* PFND3D10_1DDI_RELOCATEDEVICEFUNCS )(
    D3D10DDI_HDEVICE, struct D3D10_1DDI_DEVICEFUNCS* );
typedef SIZE_T ( APIENTRY* PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE )(
    D3D10DDI_HDEVICE, CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW* );
typedef VOID ( APIENTRY* PFND3D10_1DDI_CREATESHADERRESOURCEVIEW )(
    D3D10DDI_HDEVICE, CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*, D3D10DDI_HSHADERRESOURCEVIEW, D3D10DDI_HRTSHADERRESOURCEVIEW );
typedef SIZE_T ( APIENTRY* PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE )(
    D3D10DDI_HDEVICE, CONST D3D10_1_DDI_BLEND_DESC* );
typedef VOID ( APIENTRY* PFND3D10_1DDI_CREATEBLENDSTATE )(
    D3D10DDI_HDEVICE, CONST D3D10_1_DDI_BLEND_DESC*, D3D10DDI_HBLENDSTATE, D3D10DDI_HRTBLENDSTATE );

//----------------------------------------------------------------------------------------------------------------------------------
// User mode device function table 10.1
//
typedef struct D3D10_1DDI_DEVICEFUNCS
{
// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
// !!! BEGIN HIGH-FREQUENCY !!!
    PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP               pfnDefaultConstantBufferUpdateSubresourceUP;
    PFND3D10DDI_SETCONSTANTBUFFERS                        pfnVsSetConstantBuffers;
    PFND3D10DDI_SETSHADERRESOURCES                        pfnPsSetShaderResources;
    PFND3D10DDI_SETSHADER                                 pfnPsSetShader;
    PFND3D10DDI_SETSAMPLERS                               pfnPsSetSamplers;
    PFND3D10DDI_SETSHADER                                 pfnVsSetShader;
    PFND3D10DDI_DRAWINDEXED                               pfnDrawIndexed;
    PFND3D10DDI_DRAW                                      pfnDraw;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicIABufferMapNoOverwrite;
    PFND3D10DDI_RESOURCEUNMAP                             pfnDynamicIABufferUnmap;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicConstantBufferMapDiscard;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicIABufferMapDiscard;
    PFND3D10DDI_RESOURCEUNMAP                             pfnDynamicConstantBufferUnmap;
    PFND3D10DDI_SETCONSTANTBUFFERS                        pfnPsSetConstantBuffers;
    PFND3D10DDI_SETINPUTLAYOUT                            pfnIaSetInputLayout;
    PFND3D10DDI_IA_SETVERTEXBUFFERS                       pfnIaSetVertexBuffers;
    PFND3D10DDI_IA_SETINDEXBUFFER                         pfnIaSetIndexBuffer;
// !!! END HIGH-FREQUENCY !!!

// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
// !!! BEGIN MIDDLE-FREQUENCY !!!
    PFND3D10DDI_DRAWINDEXEDINSTANCED                      pfnDrawIndexedInstanced;
    PFND3D10DDI_DRAWINSTANCED                             pfnDrawInstanced;
    PFND3D10DDI_RESOURCEMAP                               pfnDynamicResourceMapDiscard;
    PFND3D10DDI_RESOURCEUNMAP                             pfnDynamicResourceUnmap;
    PFND3D10DDI_SETCONSTANTBUFFERS                        pfnGsSetConstantBuffers;
    PFND3D10DDI_SETSHADER                                 pfnGsSetShader;
    PFND3D10DDI_IA_SETTOPOLOGY                            pfnIaSetTopology;
    PFND3D10DDI_RESOURCEMAP                               pfnStagingResourceMap;
    PFND3D10DDI_RESOURCEUNMAP                             pfnStagingResourceUnmap;
    PFND3D10DDI_SETSHADERRESOURCES                        pfnVsSetShaderResources;
    PFND3D10DDI_SETSAMPLERS                               pfnVsSetSamplers;
    PFND3D10DDI_SETSHADERRESOURCES                        pfnGsSetShaderResources;
    PFND3D10DDI_SETSAMPLERS                               pfnGsSetSamplers;
    PFND3D10DDI_SETRENDERTARGETS                          pfnSetRenderTargets;
    PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD    pfnShaderResourceViewReadAfterWriteHazard;
    PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD              pfnResourceReadAfterWriteHazard;
    PFND3D10DDI_SETBLENDSTATE                             pfnSetBlendState;
    PFND3D10DDI_SETDEPTHSTENCILSTATE                      pfnSetDepthStencilState;
    PFND3D10DDI_SETRASTERIZERSTATE                        pfnSetRasterizerState;
    PFND3D10DDI_QUERYEND                                  pfnQueryEnd;
    PFND3D10DDI_QUERYBEGIN                                pfnQueryBegin;
    PFND3D10DDI_RESOURCECOPYREGION                        pfnResourceCopyRegion;
    PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP               pfnResourceUpdateSubresourceUP;
    PFND3D10DDI_SO_SETTARGETS                             pfnSoSetTargets;
    PFND3D10DDI_DRAWAUTO                                  pfnDrawAuto;
    PFND3D10DDI_SETVIEWPORTS                              pfnSetViewports;
    PFND3D10DDI_SETSCISSORRECTS                           pfnSetScissorRects;
    PFND3D10DDI_CLEARRENDERTARGETVIEW                     pfnClearRenderTargetView;
    PFND3D10DDI_CLEARDEPTHSTENCILVIEW                     pfnClearDepthStencilView;
    PFND3D10DDI_SETPREDICATION                            pfnSetPredication;
    PFND3D10DDI_QUERYGETDATA                              pfnQueryGetData;
    PFND3D10DDI_FLUSH                                     pfnFlush;
    PFND3D10DDI_GENMIPS                                   pfnGenMips;
    PFND3D10DDI_RESOURCECOPY                              pfnResourceCopy;
    PFND3D10DDI_RESOURCERESOLVESUBRESOURCE                pfnResourceResolveSubresource;
// !!! END MIDDLE-FREQUENCY !!!

// Infrequent paths:
    PFND3D10DDI_RESOURCEMAP                               pfnResourceMap;
    PFND3D10DDI_RESOURCEUNMAP                             pfnResourceUnmap;
    PFND3D10DDI_RESOURCEISSTAGINGBUSY                     pfnResourceIsStagingBusy;
    PFND3D10_1DDI_RELOCATEDEVICEFUNCS                     pfnRelocateDeviceFuncs;
    PFND3D10DDI_CALCPRIVATERESOURCESIZE                   pfnCalcPrivateResourceSize;
    PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE             pfnCalcPrivateOpenedResourceSize;
    PFND3D10DDI_CREATERESOURCE                            pfnCreateResource;
    PFND3D10DDI_OPENRESOURCE                              pfnOpenResource;
    PFND3D10DDI_DESTROYRESOURCE                           pfnDestroyResource;
    PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE       pfnCalcPrivateShaderResourceViewSize;
    PFND3D10_1DDI_CREATESHADERRESOURCEVIEW                pfnCreateShaderResourceView;
    PFND3D10DDI_DESTROYSHADERRESOURCEVIEW                 pfnDestroyShaderResourceView;
    PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE           pfnCalcPrivateRenderTargetViewSize;
    PFND3D10DDI_CREATERENDERTARGETVIEW                    pfnCreateRenderTargetView;
    PFND3D10DDI_DESTROYRENDERTARGETVIEW                   pfnDestroyRenderTargetView;
    PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE           pfnCalcPrivateDepthStencilViewSize;
    PFND3D10DDI_CREATEDEPTHSTENCILVIEW                    pfnCreateDepthStencilView;
    PFND3D10DDI_DESTROYDEPTHSTENCILVIEW                   pfnDestroyDepthStencilView;
    PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE              pfnCalcPrivateElementLayoutSize;
    PFND3D10DDI_CREATEELEMENTLAYOUT                       pfnCreateElementLayout;
    PFND3D10DDI_DESTROYELEMENTLAYOUT                      pfnDestroyElementLayout;
    PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE               pfnCalcPrivateBlendStateSize;
    PFND3D10_1DDI_CREATEBLENDSTATE                        pfnCreateBlendState;
    PFND3D10DDI_DESTROYBLENDSTATE                         pfnDestroyBlendState;
    PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE          pfnCalcPrivateDepthStencilStateSize;
    PFND3D10DDI_CREATEDEPTHSTENCILSTATE                   pfnCreateDepthStencilState;
    PFND3D10DDI_DESTROYDEPTHSTENCILSTATE                  pfnDestroyDepthStencilState;
    PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE            pfnCalcPrivateRasterizerStateSize;
    PFND3D10DDI_CREATERASTERIZERSTATE                     pfnCreateRasterizerState;
    PFND3D10DDI_DESTROYRASTERIZERSTATE                    pfnDestroyRasterizerState;
    PFND3D10DDI_CALCPRIVATESHADERSIZE                     pfnCalcPrivateShaderSize;
    PFND3D10DDI_CREATEVERTEXSHADER                        pfnCreateVertexShader;
    PFND3D10DDI_CREATEGEOMETRYSHADER                      pfnCreateGeometryShader;
    PFND3D10DDI_CREATEPIXELSHADER                         pfnCreatePixelShader;
    PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT pfnCalcPrivateGeometryShaderWithStreamOutput;
    PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT      pfnCreateGeometryShaderWithStreamOutput;
    PFND3D10DDI_DESTROYSHADER                             pfnDestroyShader;
    PFND3D10DDI_CALCPRIVATESAMPLERSIZE                    pfnCalcPrivateSamplerSize;
    PFND3D10DDI_CREATESAMPLER                             pfnCreateSampler;
    PFND3D10DDI_DESTROYSAMPLER                            pfnDestroySampler;
    PFND3D10DDI_CALCPRIVATEQUERYSIZE                      pfnCalcPrivateQuerySize;
    PFND3D10DDI_CREATEQUERY                               pfnCreateQuery;
    PFND3D10DDI_DESTROYQUERY                              pfnDestroyQuery;

    PFND3D10DDI_CHECKFORMATSUPPORT                        pfnCheckFormatSupport;
    PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS             pfnCheckMultisampleQualityLevels;
    PFND3D10DDI_CHECKCOUNTERINFO                          pfnCheckCounterInfo;
    PFND3D10DDI_CHECKCOUNTER                              pfnCheckCounter;

    PFND3D10DDI_DESTROYDEVICE                             pfnDestroyDevice;
    PFND3D10DDI_SETTEXTFILTERSIZE                         pfnSetTextFilterSize;

    // Start additional 10.1 entries:
    PFND3D10DDI_RESOURCECOPY                              pfnResourceConvert;
    PFND3D10DDI_RESOURCECOPYREGION                        pfnResourceConvertRegion;

#ifdef D3D10PSGP
// Rasterization-only specific:
    PFND3D10DDI_RESETPRIMITIVEID                          pfnResetPrimitiveID;
    PFND3D10DDI_SETVERTEXPIPELINEOUTPUT                   pfnSetVertexPipelineOutput;
#endif // D3D10PSGP

} D3D10_1DDI_DEVICEFUNCS;

#endif

typedef VOID (APIENTRY CALLBACK *PFND3D10DDI_SETERROR_CB)( D3D10DDI_HRTCORELAYER, HRESULT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_VS_CONSTBUF_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_PS_SRV_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_PS_SHADER_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_PS_SAMPLER_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_VS_SHADER_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_PS_CONSTBUF_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_IA_INPUTLAYOUT_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_IA_VERTEXBUF_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_IA_INDEXBUF_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_GS_CONSTBUF_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_GS_SHADER_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_IA_PRIMITIVE_TOPOLOGY_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_VS_SRV_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_VS_SAMPLER_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_GS_SRV_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_GS_SAMPLER_CB)( D3D10DDI_HRTCORELAYER, UINT, UINT );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_OM_RENDERTARGETS_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_OM_BLENDSTATE_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_OM_DEPTHSTATE_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_RS_RASTSTATE_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_SO_TARGETS_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_RS_VIEWPORTS_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_RS_SCISSOR_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_STATE_TEXTFILTERSIZE_CB)( D3D10DDI_HRTCORELAYER );
typedef void (APIENTRY CALLBACK *PFND3D10DDI_DISABLE_DEFERRED_STAGING_RESOURCE_DESTRUCTION_CB)( D3D10DDI_HRTCORELAYER );

typedef struct D3D10DDI_CORELAYER_DEVICECALLBACKS
{
    PFND3D10DDI_SETERROR_CB pfnSetErrorCb;
    PFND3D10DDI_STATE_VS_CONSTBUF_CB pfnStateVsConstBufCb;
    PFND3D10DDI_STATE_PS_SRV_CB pfnStatePsSrvCb;
    PFND3D10DDI_STATE_PS_SHADER_CB pfnStatePsShaderCb;
    PFND3D10DDI_STATE_PS_SAMPLER_CB pfnStatePsSamplerCb;
    PFND3D10DDI_STATE_VS_SHADER_CB pfnStateVsShaderCb;
    PFND3D10DDI_STATE_PS_CONSTBUF_CB pfnStatePsConstBufCb;
    PFND3D10DDI_STATE_IA_INPUTLAYOUT_CB pfnStateIaInputLayoutCb;
    PFND3D10DDI_STATE_IA_VERTEXBUF_CB pfnStateIaVertexBufCb;
    PFND3D10DDI_STATE_IA_INDEXBUF_CB pfnStateIaIndexBufCb;
    PFND3D10DDI_STATE_GS_CONSTBUF_CB pfnStateGsConstBufCb;
    PFND3D10DDI_STATE_GS_SHADER_CB pfnStateGsShaderCb;
    PFND3D10DDI_STATE_IA_PRIMITIVE_TOPOLOGY_CB pfnStateIaPrimitiveTopologyCb;
    PFND3D10DDI_STATE_VS_SRV_CB pfnStateVsSrvCb;
    PFND3D10DDI_STATE_VS_SAMPLER_CB pfnStateVsSamplerCb;
    PFND3D10DDI_STATE_GS_SRV_CB pfnStateGsSrvCb;
    PFND3D10DDI_STATE_GS_SAMPLER_CB pfnStateGsSamplerCb;
    PFND3D10DDI_STATE_OM_RENDERTARGETS_CB pfnStateOmRenderTargetsCb;
    PFND3D10DDI_STATE_OM_BLENDSTATE_CB pfnStateOmBlendStateCb;
    PFND3D10DDI_STATE_OM_DEPTHSTATE_CB pfnStateOmDepthStateCb;
    PFND3D10DDI_STATE_RS_RASTSTATE_CB pfnStateRsRastStateCb;
    PFND3D10DDI_STATE_SO_TARGETS_CB pfnStateSoTargetsCb;
    PFND3D10DDI_STATE_RS_VIEWPORTS_CB pfnStateRsViewportsCb;
    PFND3D10DDI_STATE_RS_SCISSOR_CB pfnStateRsScissorCb;
    PFND3D10DDI_DISABLE_DEFERRED_STAGING_RESOURCE_DESTRUCTION_CB pfnDisableDeferredStagingResourceDestruction;
    PFND3D10DDI_STATE_TEXTFILTERSIZE_CB pfnStateTextFilterSizeCb;
} D3D10DDI_CORELAYER_DEVICECALLBACKS;

// TODO: BK: Refactor for "evolving" kernel data.
typedef struct D3D10DDIARG_CREATEDEVICE
{
    D3D10DDI_HRTDEVICE              hRTDevice;              // in:  Runtime handle
    UINT                            Interface;              // in:  Interface version
    UINT                            Version;                // in:  Runtime Version
    CONST D3DDDI_DEVICECALLBACKS*   pKTCallbacks;           // in:  Pointer to runtime callbacks that invoke kernel
    union
    {
        D3D10DDI_DEVICEFUNCS*           pDeviceFuncs;       // in/out: Driver d3d function table
#if D3D10DDI_MINOR_HEADER_VERSION >= 1
        D3D10_1DDI_DEVICEFUNCS*         p10_1DeviceFuncs;   // in/out: (Use when Interface == D3D10_1_DDI_INTERFACE_VERSION)
#endif
    };

    // TODO: make this better/more robust or something or other (johnket)
    D3D10DDI_HDEVICE                hDrvDevice;             // in:  Driver private handle/ storage.
    DXGI_DDI_BASE_ARGS              DXGIBaseDDI;            // in/out
    D3D10DDI_HRTCORELAYER           hRTCoreLayer;           // in:  CoreLayer handle
    CONST D3D10DDI_CORELAYER_DEVICECALLBACKS* pUMCallbacks;  // in:  callbacks that stay in usermode
    UINT                            Flags;                  // in:  D3D10DDI_CREATEDEVICE_FLAG_*
} D3D10DDIARG_CREATEDEVICE;

typedef struct D3D10DDIARG_CALCPRIVATEDEVICESIZE
{
    UINT                          Interface;          // in:  Interface version
    UINT                          Version;            // in:  Runtime Version
    UINT                          Flags;              // in:  D3D10DDI_CREATEDEVICE_FLAG_*
} D3D10DDIARG_CALCPRIVATEDEVICESIZE;

#define D3D10DDI_CREATEDEVICE_FLAG_DISABLE_EXTRA_THREAD_CREATION 0x1

typedef SIZE_T (APIENTRY *PFND3D10DDI_CALCPRIVATEDEVICESIZE)(D3D10DDI_HADAPTER, CONST D3D10DDIARG_CALCPRIVATEDEVICESIZE*);
typedef HRESULT (APIENTRY *PFND3D10DDI_CREATEDEVICE)(D3D10DDI_HADAPTER, IN D3D10DDIARG_CREATEDEVICE*);
typedef HRESULT (APIENTRY *PFND3D10DDI_CLOSEADAPTER)(D3D10DDI_HADAPTER);

typedef struct D3D10DDI_ADAPTERFUNCS
{
    PFND3D10DDI_CALCPRIVATEDEVICESIZE         pfnCalcPrivateDeviceSize;
    PFND3D10DDI_CREATEDEVICE                  pfnCreateDevice;
    PFND3D10DDI_CLOSEADAPTER                  pfnCloseAdapter;
} D3D10DDI_ADAPTERFUNCS;

typedef struct D3D10DDIARG_OPENADAPTER
{
    D3D10DDI_HRTADAPTER            hRTAdapter;         // in/out:  Runtime handle/out: Driver handle
    D3D10DDI_HADAPTER              hAdapter;           // out: Driver handle
    UINT                           Interface;          // in:  Interface version
    UINT                           Version;            // in:  Runtime version
    CONST D3DDDI_ADAPTERCALLBACKS* pAdapterCallbacks;  // in:  Pointer to runtime callbacks
    D3D10DDI_ADAPTERFUNCS*         pAdapterFuncs;      // out: Driver function table
} D3D10DDIARG_OPENADAPTER;

typedef HRESULT (APIENTRY *PFND3D10DDI_OPENADAPTER)(D3D10DDIARG_OPENADAPTER*);

//----------------------------------------------------------------------------------------------------------------------------------
// Versioning
//
// Related to
// D3D10DDIARG_OPENADAPTER::Interface
// D3D10DDIARG_OPENADAPTER::Version
// D3D10DDIARG_CREATEDEVICE::Interface
// D3D10DDIARG_CREATEDEVICE::Version
// D3D10DDIARG_CALCPRIVATEDEVICESIZE::Interface
// D3D10DDIARG_CALCPRIVATEDEVICESIZE::Version
//
// THE DRIVER MUST PAY ATTENTION TO THE Interface FIELD.
//
// The most significant 16 bits of the Interface field is the DDI Major Version. This value should always be 10.
//
// The least significant 16 bits of the Interface field is the DDI Minor Version. This value is bumped everytime a DDI breaking
// change is introduced (or can be bumped artifically to signify a stronger version change). The #defines associate which DDI
// minor version is associated with which marketedable version number (ie. D3D10_0 == x, D3D10_1 == y, where y > x).
//
// The driver is suggested to pay attention only to the most significant 16 bits of the Version field.
//
// The most significant 16 bits of the Version field is the DDI Build Version. This value is manually bumped everytime there,
// is a non-breaking DDI change. The driver may come to depened on each non-breaking DDI change over time, so it is advisable
// for the driver to ensure that the passed in DDI Build Version is >= to the *_BUILD_VERSION of the current driver;
// and fail out if incompatable (perhaps while also providing a registry workaround).
//
// The least significant 16 bits of the Version field is the DDI Revision Version. It mostly is used to special case the
// driver based on bugs present in the API. The driver must succeed creation for all values, but may change behavior
// depending on certain values. Be sure to compare with these values using >=, since the numbers may rise arbitrarily due
// to runtime fixes. Also, do not use "> (previous broken version)" (rather than ">= working version") because new revisions
// may appear that have version numbers between the two known numbers and do not contain the required fixes. Case in point
// is the linked adapter QFE: any further d3d10 runtime updates might result in a version like ((4<<16)|6001), if run on
// a system missing the linked adapter QFE.
//
#define D3D10_DDI_MAJOR_VERSION 10
#define D3D10_0_DDI_MINOR_VERSION 1
#define D3D10_0_DDI_INTERFACE_VERSION ((D3D10_DDI_MAJOR_VERSION << 16) | D3D10_0_DDI_MINOR_VERSION)
#define D3D10_0_DDI_BUILD_VERSION 4
#define D3D10_0_DDI_VERSION_VISTA_GOLD                          ( ( 4 << 16 ) | 6000 )
#define D3D10_0_DDI_VERSION_VISTA_GOLD_WITH_LINKED_ADAPTER_QFE  ( ( 4 << 16 ) | 6008 )
#define D3D10_0_DDI_IS_LINKED_ADAPTER_QFE_PRESENT(Version)  (Version >= D3D10_0_DDI_VERSION_VISTA_GOLD_WITH_LINKED_ADAPTER_QFE)

#if D3D10DDI_MINOR_HEADER_VERSION >= 1
#define D3D10_1_DDI_MINOR_VERSION 2
#define D3D10_1_DDI_INTERFACE_VERSION ((D3D10_DDI_MAJOR_VERSION << 16) | D3D10_1_DDI_MINOR_VERSION)
#define D3D10_1_DDI_BUILD_VERSION 1
//Note: d3d10_1 doesn't currently ship on vista gold. This definition is included for completeness in the event
//that it does at some point in the future:
#define D3D10_1_DDI_VERSION_VISTA_GOLD                          ( ( 1 << 16 ) | 6000 )
#define D3D10_1_DDI_VERSION_VISTA_SP1                           ( ( 1 << 16 ) | 6008 )
#define D3D10_1_DDI_IS_LINKED_ADAPTER_QFE_PRESENT(Version)  (Version >= D3D10_1_DDI_VERSION_VISTA_SP1)

#define D3D10on9_DDI_MINOR_VERSION 0
#define D3D10on9_DDI_INTERFACE_VERSION ((D3D10_DDI_MAJOR_VERSION << 16) | D3D10on9_DDI_MINOR_VERSION)
#define D3D10on9_DDI_BUILD_VERSION 0

#endif
#endif // _D3D10UMDDI_H
// NOTE: The following constants are generated from the d3d10 hardware spec.  Do not edit these values directly.
#ifndef _D3D10_CONSTANTS
#define _D3D10_CONSTANTS
const UINT D3D10_16BIT_INDEX_STRIP_CUT_VALUE = 0xffff;
const UINT D3D10_32BIT_INDEX_STRIP_CUT_VALUE = 0xffffffff;
const UINT D3D10_8BIT_INDEX_STRIP_CUT_VALUE = 0xff;
const UINT D3D10_ARRAY_AXIS_ADDRESS_RANGE_BIT_COUNT = 9;
const UINT D3D10_CLIP_OR_CULL_DISTANCE_COUNT = 8;
const UINT D3D10_CLIP_OR_CULL_DISTANCE_ELEMENT_COUNT = 2;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT = 14;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENTS = 4;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT = 15;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS = 4;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT = 15;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_READS_PER_INST = 1;
const UINT D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_READ_PORTS = 1;
const UINT D3D10_COMMONSHADER_FLOWCONTROL_NESTING_LIMIT = 64;
const UINT D3D10_COMMONSHADER_IMMEDIATE_CONSTANT_BUFFER_REGISTER_COMPONENTS = 4;
const UINT D3D10_COMMONSHADER_IMMEDIATE_CONSTANT_BUFFER_REGISTER_COUNT = 1;
const UINT D3D10_COMMONSHADER_IMMEDIATE_CONSTANT_BUFFER_REGISTER_READS_PER_INST = 1;
const UINT D3D10_COMMONSHADER_IMMEDIATE_CONSTANT_BUFFER_REGISTER_READ_PORTS = 1;
const UINT D3D10_COMMONSHADER_IMMEDIATE_VALUE_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_COMMONSHADER_INPUT_RESOURCE_REGISTER_COMPONENTS = 1;
const UINT D3D10_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT = 128;
const UINT D3D10_COMMONSHADER_INPUT_RESOURCE_REGISTER_READS_PER_INST = 1;
const UINT D3D10_COMMONSHADER_INPUT_RESOURCE_REGISTER_READ_PORTS = 1;
const UINT D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT = 128;
const UINT D3D10_COMMONSHADER_SAMPLER_REGISTER_COMPONENTS = 1;
const UINT D3D10_COMMONSHADER_SAMPLER_REGISTER_COUNT = 16;
const UINT D3D10_COMMONSHADER_SAMPLER_REGISTER_READS_PER_INST = 1;
const UINT D3D10_COMMONSHADER_SAMPLER_REGISTER_READ_PORTS = 1;
const UINT D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT = 16;
const UINT D3D10_COMMONSHADER_SUBROUTINE_NESTING_LIMIT = 32;
const UINT D3D10_COMMONSHADER_TEMP_REGISTER_COMPONENTS = 4;
const UINT D3D10_COMMONSHADER_TEMP_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_COMMONSHADER_TEMP_REGISTER_COUNT = 4096;
const UINT D3D10_COMMONSHADER_TEMP_REGISTER_READS_PER_INST = 3;
const UINT D3D10_COMMONSHADER_TEMP_REGISTER_READ_PORTS = 3;
const UINT D3D10_COMMONSHADER_TEXCOORD_RANGE_REDUCTION_MAX = 10;
const INT D3D10_COMMONSHADER_TEXCOORD_RANGE_REDUCTION_MIN = -10;
const INT D3D10_COMMONSHADER_TEXEL_OFFSET_MAX_NEGATIVE = -8;
const UINT D3D10_COMMONSHADER_TEXEL_OFFSET_MAX_POSITIVE = 7;
#define D3D10_DEFAULT_BLEND_FACTOR_ALPHA	( 1.0f )
#define D3D10_DEFAULT_BLEND_FACTOR_BLUE	( 1.0f )
#define D3D10_DEFAULT_BLEND_FACTOR_GREEN	( 1.0f )
#define D3D10_DEFAULT_BLEND_FACTOR_RED	( 1.0f )
#define D3D10_DEFAULT_BORDER_COLOR_COMPONENT	( 0.0f )
const UINT D3D10_DEFAULT_DEPTH_BIAS = 0;
#define D3D10_DEFAULT_DEPTH_BIAS_CLAMP	( 0.0f )
#define D3D10_DEFAULT_MAX_ANISOTROPY	( 16.0f )
#define D3D10_DEFAULT_MIP_LOD_BIAS	( 0.0f )
const UINT D3D10_DEFAULT_RENDER_TARGET_ARRAY_INDEX = 0;
const UINT D3D10_DEFAULT_SAMPLE_MASK = 0xffffffff;
const UINT D3D10_DEFAULT_SCISSOR_ENDX = 0;
const UINT D3D10_DEFAULT_SCISSOR_ENDY = 0;
const UINT D3D10_DEFAULT_SCISSOR_STARTX = 0;
const UINT D3D10_DEFAULT_SCISSOR_STARTY = 0;
#define D3D10_DEFAULT_SLOPE_SCALED_DEPTH_BIAS	( 0.0f )
const UINT D3D10_DEFAULT_STENCIL_READ_MASK = 0xff;
const UINT D3D10_DEFAULT_STENCIL_REFERENCE = 0;
const UINT D3D10_DEFAULT_STENCIL_WRITE_MASK = 0xff;
const UINT D3D10_DEFAULT_VIEWPORT_AND_SCISSORRECT_INDEX = 0;
const UINT D3D10_DEFAULT_VIEWPORT_HEIGHT = 0;
#define D3D10_DEFAULT_VIEWPORT_MAX_DEPTH	( 0.0f )
#define D3D10_DEFAULT_VIEWPORT_MIN_DEPTH	( 0.0f )
const UINT D3D10_DEFAULT_VIEWPORT_TOPLEFTX = 0;
const UINT D3D10_DEFAULT_VIEWPORT_TOPLEFTY = 0;
const UINT D3D10_DEFAULT_VIEWPORT_WIDTH = 0;
#define D3D10_FLOAT16_FUSED_TOLERANCE_IN_ULP	( 0.6 )
#define D3D10_FLOAT32_MAX	( 3.402823466e+38f )
#define D3D10_FLOAT32_TO_INTEGER_TOLERANCE_IN_ULP	( 0.6f )
#define D3D10_FLOAT_TO_SRGB_EXPONENT_DENOMINATOR	( 2.4f )
#define D3D10_FLOAT_TO_SRGB_EXPONENT_NUMERATOR	( 1.0f )
#define D3D10_FLOAT_TO_SRGB_OFFSET	( 0.055f )
#define D3D10_FLOAT_TO_SRGB_SCALE_1	( 12.92f )
#define D3D10_FLOAT_TO_SRGB_SCALE_2	( 1.055f )
#define D3D10_FLOAT_TO_SRGB_THRESHOLD	( 0.0031308f )
#define D3D10_FTOI_INSTRUCTION_MAX_INPUT	( 2147483647.999f )
#define D3D10_FTOI_INSTRUCTION_MIN_INPUT	( -2147483648.999f )
#define D3D10_FTOU_INSTRUCTION_MAX_INPUT	( 4294967295.999f )
#define D3D10_FTOU_INSTRUCTION_MIN_INPUT	( 0.0f )
const UINT D3D10_GS_INPUT_PRIM_CONST_REGISTER_COMPONENTS = 1;
const UINT D3D10_GS_INPUT_PRIM_CONST_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_GS_INPUT_PRIM_CONST_REGISTER_COUNT = 1;
const UINT D3D10_GS_INPUT_PRIM_CONST_REGISTER_READS_PER_INST = 2;
const UINT D3D10_GS_INPUT_PRIM_CONST_REGISTER_READ_PORTS = 1;
const UINT D3D10_GS_INPUT_REGISTER_COMPONENTS = 4;
const UINT D3D10_GS_INPUT_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_GS_INPUT_REGISTER_COUNT = 16;
const UINT D3D10_GS_INPUT_REGISTER_READS_PER_INST = 2;
const UINT D3D10_GS_INPUT_REGISTER_READ_PORTS = 1;
const UINT D3D10_GS_INPUT_REGISTER_VERTICES = 6;
const UINT D3D10_GS_OUTPUT_ELEMENTS = 32;
const UINT D3D10_GS_OUTPUT_REGISTER_COMPONENTS = 4;
const UINT D3D10_GS_OUTPUT_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_GS_OUTPUT_REGISTER_COUNT = 32;
const UINT D3D10_IA_DEFAULT_INDEX_BUFFER_OFFSET_IN_BYTES = 0;
const UINT D3D10_IA_DEFAULT_PRIMITIVE_TOPOLOGY = 0;
const UINT D3D10_IA_DEFAULT_VERTEX_BUFFER_OFFSET_IN_BYTES = 0;
const UINT D3D10_IA_INDEX_INPUT_RESOURCE_SLOT_COUNT = 1;
const UINT D3D10_IA_INSTANCE_ID_BIT_COUNT = 32;
const UINT D3D10_IA_INTEGER_ARITHMETIC_BIT_COUNT = 32;
const UINT D3D10_IA_PRIMITIVE_ID_BIT_COUNT = 32;
const UINT D3D10_IA_VERTEX_ID_BIT_COUNT = 32;
const UINT D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT = 16;
const UINT D3D10_IA_VERTEX_INPUT_STRUCTURE_ELEMENTS_COMPONENTS = 64;
const UINT D3D10_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT = 16;
const UINT D3D10_INTEGER_DIVIDE_BY_ZERO_QUOTIENT = 0xffffffff;
const UINT D3D10_INTEGER_DIVIDE_BY_ZERO_REMAINDER = 0xffffffff;
#define D3D10_LINEAR_GAMMA	( 1.0f )
#define D3D10_MAX_BORDER_COLOR_COMPONENT	( 1.0f )
#define D3D10_MAX_DEPTH	( 1.0f )
const UINT D3D10_MAX_MAXANISOTROPY = 16;
const UINT D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT = 32;
#define D3D10_MAX_POSITION_VALUE	( 3.402823466e+34f )
const UINT D3D10_MAX_TEXTURE_DIMENSION_2_TO_EXP = 17;
#define D3D10_MIN_BORDER_COLOR_COMPONENT	( 0.0f )
#define D3D10_MIN_DEPTH	( 0.0f )
const UINT D3D10_MIN_MAXANISOTROPY = 0;
#define D3D10_MIP_LOD_BIAS_MAX	( 15.99f )
#define D3D10_MIP_LOD_BIAS_MIN	( -16.0f )
const UINT D3D10_MIP_LOD_FRACTIONAL_BIT_COUNT = 6;
const UINT D3D10_MIP_LOD_RANGE_BIT_COUNT = 8;
#define D3D10_MULTISAMPLE_ANTIALIAS_LINE_WIDTH	( 1.4f )
const UINT D3D10_NONSAMPLE_FETCH_OUT_OF_RANGE_ACCESS_RESULT = 0;
const UINT D3D10_PIXEL_ADDRESS_RANGE_BIT_COUNT = 13;
const UINT D3D10_PRE_SCISSOR_PIXEL_ADDRESS_RANGE_BIT_COUNT = 15;
const UINT D3D10_PS_FRONTFACING_DEFAULT_VALUE = 0xFFFFFFFF;
const UINT D3D10_PS_FRONTFACING_FALSE_VALUE = 0x00000000;
const UINT D3D10_PS_FRONTFACING_TRUE_VALUE = 0xFFFFFFFF;
const UINT D3D10_PS_INPUT_REGISTER_COMPONENTS = 4;
const UINT D3D10_PS_INPUT_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_PS_INPUT_REGISTER_COUNT = 32;
const UINT D3D10_PS_INPUT_REGISTER_READS_PER_INST = 2;
const UINT D3D10_PS_INPUT_REGISTER_READ_PORTS = 1;
#define D3D10_PS_LEGACY_PIXEL_CENTER_FRACTIONAL_COMPONENT	( 0.0f )
const UINT D3D10_PS_OUTPUT_DEPTH_REGISTER_COMPONENTS = 1;
const UINT D3D10_PS_OUTPUT_DEPTH_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_PS_OUTPUT_DEPTH_REGISTER_COUNT = 1;
const UINT D3D10_PS_OUTPUT_REGISTER_COMPONENTS = 4;
const UINT D3D10_PS_OUTPUT_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_PS_OUTPUT_REGISTER_COUNT = 8;
#define D3D10_PS_PIXEL_CENTER_FRACTIONAL_COMPONENT	( 0.5f )
const UINT D3D10_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT = 4096;
const UINT D3D10_REQ_BUFFER_RESOURCE_TEXEL_COUNT_2_TO_EXP = 27;
const UINT D3D10_REQ_CONSTANT_BUFFER_ELEMENT_COUNT = 4096;
const UINT D3D10_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT = 4096;
const UINT D3D10_REQ_DRAWINDEXED_INDEX_COUNT_2_TO_EXP = 32;
const UINT D3D10_REQ_DRAW_VERTEX_COUNT_2_TO_EXP = 32;
const UINT D3D10_REQ_FILTERING_HW_ADDRESSABLE_RESOURCE_DIMENSION = 8192;
const UINT D3D10_REQ_GS_INVOCATION_32BIT_OUTPUT_COMPONENT_LIMIT = 1024;
const UINT D3D10_REQ_IMMEDIATE_CONSTANT_BUFFER_ELEMENT_COUNT = 4096;
const UINT D3D10_REQ_MAXANISOTROPY = 16;
const UINT D3D10_REQ_MIP_LEVELS = 14;
const UINT D3D10_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES = 2048;
const UINT D3D10_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT = 4096;
const UINT D3D10_REQ_RENDER_TO_BUFFER_WINDOW_WIDTH = 8192;
const UINT D3D10_REQ_RESOURCE_SIZE_IN_MEGABYTES = 128;
const UINT D3D10_REQ_RESOURCE_VIEW_COUNT_PER_CONTEXT_2_TO_EXP = 20;
const UINT D3D10_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT = 4096;
const UINT D3D10_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION = 512;
const UINT D3D10_REQ_TEXTURE1D_U_DIMENSION = 8192;
const UINT D3D10_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION = 512;
const UINT D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION = 8192;
const UINT D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION = 2048;
const UINT D3D10_REQ_TEXTURECUBE_DIMENSION = 8192;
const UINT D3D10_RESINFO_INSTRUCTION_MISSING_COMPONENT_RETVAL = 0;
const UINT D3D10_SHADER_MAJOR_VERSION = 4;
const UINT D3D10_SHADER_MINOR_VERSION = 0;
const UINT D3D10_SHIFT_INSTRUCTION_PAD_VALUE = 0;
const UINT D3D10_SHIFT_INSTRUCTION_SHIFT_VALUE_BIT_COUNT = 5;
const UINT D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT = 8;
const UINT D3D10_SO_BUFFER_MAX_STRIDE_IN_BYTES = 2048;
const UINT D3D10_SO_BUFFER_MAX_WRITE_WINDOW_IN_BYTES = 256;
const UINT D3D10_SO_BUFFER_SLOT_COUNT = 4;
const UINT D3D10_SO_DDI_REGISTER_INDEX_DENOTING_GAP = 0xffffffff;
const UINT D3D10_SO_MULTIPLE_BUFFER_ELEMENTS_PER_BUFFER = 1;
const UINT D3D10_SO_SINGLE_BUFFER_COMPONENT_LIMIT = 64;
#define D3D10_SRGB_GAMMA	( 2.2f )
#define D3D10_SRGB_TO_FLOAT_DENOMINATOR_1	( 12.92f )
#define D3D10_SRGB_TO_FLOAT_DENOMINATOR_2	( 1.055f )
#define D3D10_SRGB_TO_FLOAT_EXPONENT	( 2.4f )
#define D3D10_SRGB_TO_FLOAT_OFFSET	( 0.055f )
#define D3D10_SRGB_TO_FLOAT_THRESHOLD	( 0.04045f )
#define D3D10_SRGB_TO_FLOAT_TOLERANCE_IN_ULP	( 0.5f )
const UINT D3D10_STANDARD_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_STANDARD_COMPONENT_BIT_COUNT_DOUBLED = 64;
const UINT D3D10_STANDARD_MAXIMUM_ELEMENT_ALIGNMENT_BYTE_MULTIPLE = 4;
const UINT D3D10_STANDARD_PIXEL_COMPONENT_COUNT = 128;
const UINT D3D10_STANDARD_PIXEL_ELEMENT_COUNT = 32;
const UINT D3D10_STANDARD_VECTOR_SIZE = 4;
const UINT D3D10_STANDARD_VERTEX_ELEMENT_COUNT = 16;
const UINT D3D10_STANDARD_VERTEX_TOTAL_COMPONENT_COUNT = 64;
const UINT D3D10_SUBPIXEL_FRACTIONAL_BIT_COUNT = 8;
const UINT D3D10_SUBTEXEL_FRACTIONAL_BIT_COUNT = 6;
const UINT D3D10_TEXEL_ADDRESS_RANGE_BIT_COUNT = 18;
const UINT D3D10_UNBOUND_MEMORY_ACCESS_RESULT = 0;
const UINT D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX = 15;
const UINT D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE = 16;
const UINT D3D10_VIEWPORT_BOUNDS_MAX = 16383;
const INT D3D10_VIEWPORT_BOUNDS_MIN = -16384;
const UINT D3D10_VS_INPUT_REGISTER_COMPONENTS = 4;
const UINT D3D10_VS_INPUT_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_VS_INPUT_REGISTER_COUNT = 16;
const UINT D3D10_VS_INPUT_REGISTER_READS_PER_INST = 2;
const UINT D3D10_VS_INPUT_REGISTER_READ_PORTS = 1;
const UINT D3D10_VS_OUTPUT_REGISTER_COMPONENTS = 4;
const UINT D3D10_VS_OUTPUT_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_VS_OUTPUT_REGISTER_COUNT = 16;
const UINT D3D10_WHQL_CONTEXT_COUNT_FOR_RESOURCE_LIMIT = 10;
const UINT D3D10_WHQL_DRAWINDEXED_INDEX_COUNT_2_TO_EXP = 25;
const UINT D3D10_WHQL_DRAW_VERTEX_COUNT_2_TO_EXP = 25;
const UINT D3D_MAJOR_VERSION = 10;
const UINT D3D_MINOR_VERSION = 0;
const UINT D3D_SPEC_DATE_DAY = 8;
const UINT D3D_SPEC_DATE_MONTH = 8;
const UINT D3D_SPEC_DATE_YEAR = 2006;
#define D3D_SPEC_VERSION	( 1.050005 )
#endif
// NOTE: The following constants are generated from the d3d10_1 hardware spec.  Do not edit these values directly.
#ifndef _D3D10_1_CONSTANTS
#define _D3D10_1_CONSTANTS
const UINT D3D10_1_DEFAULT_SAMPLE_MASK = 0xffffffff;
#define D3D10_1_FLOAT16_FUSED_TOLERANCE_IN_ULP	( 0.6 )
#define D3D10_1_FLOAT32_TO_INTEGER_TOLERANCE_IN_ULP	( 0.6f )
const UINT D3D10_1_GS_INPUT_REGISTER_COUNT = 32;
const UINT D3D10_1_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT = 32;
const UINT D3D10_1_IA_VERTEX_INPUT_STRUCTURE_ELEMENTS_COMPONENTS = 128;
const UINT D3D10_1_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT = 32;
const UINT D3D10_1_PS_OUTPUT_MASK_REGISTER_COMPONENTS = 1;
const UINT D3D10_1_PS_OUTPUT_MASK_REGISTER_COMPONENT_BIT_COUNT = 32;
const UINT D3D10_1_PS_OUTPUT_MASK_REGISTER_COUNT = 1;
const UINT D3D10_1_SHADER_MAJOR_VERSION = 4;
const UINT D3D10_1_SHADER_MINOR_VERSION = 1;
const UINT D3D10_1_SO_BUFFER_MAX_STRIDE_IN_BYTES = 2048;
const UINT D3D10_1_SO_BUFFER_MAX_WRITE_WINDOW_IN_BYTES = 256;
const UINT D3D10_1_SO_BUFFER_SLOT_COUNT = 4;
const UINT D3D10_1_SO_MULTIPLE_BUFFER_ELEMENTS_PER_BUFFER = 1;
const UINT D3D10_1_SO_SINGLE_BUFFER_COMPONENT_LIMIT = 64;
const UINT D3D10_1_STANDARD_VERTEX_ELEMENT_COUNT = 32;
const UINT D3D10_1_SUBPIXEL_FRACTIONAL_BIT_COUNT = 8;
const UINT D3D10_1_VS_INPUT_REGISTER_COUNT = 32;
const UINT D3D10_1_VS_OUTPUT_REGISTER_COUNT = 32;
#endif

