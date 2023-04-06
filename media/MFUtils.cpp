#if defined(_WIN32)

#include "MFUtils.h"

#include <windows.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>

#pragma comment(lib, "mf")
#pragma comment(lib, "Mfplat")
#pragma comment(lib, "mfplay")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "shlwapi")

namespace MFUtils
{
struct MFMedia
{
    MFMedia() : _hr(MFStartup(MF_VERSION)) {}
    ~MFMedia() { MFShutdown(); }
    HRESULT _hr;
};

HRESULT InitializeMFOnce()
{
    static MFMedia s_mfMedia;
    return s_mfMedia._hr;
}

std::string_view GuidToString(const GUID& SubType)
{
    using namespace std::string_view_literals;
#    undef TEXT
#    define __TEXT(quote) quote##sv
#    define TEXT(str) __TEXT(str)
    if (SubType == GUID_NULL)
        return TEXT("Null");

    // image formats
    if (SubType == MFImageFormat_JPEG)
        return TEXT("Jpeg");
    if (SubType == MFImageFormat_RGB32)
        return TEXT("RGB32");

    // stream formats
    if (SubType == MFStreamFormat_MPEG2Transport)
        return TEXT("MPEG-2 Transport");
    if (SubType == MFStreamFormat_MPEG2Program)
        return TEXT("MPEG-2 Program");

    // video formats
    if (SubType == MFVideoFormat_RGB32)
        return TEXT("RGB32");
    if (SubType == MFVideoFormat_ARGB32)
        return TEXT("ARGB32");
    if (SubType == MFVideoFormat_RGB24)
        return TEXT("RGB24");
    if (SubType == MFVideoFormat_RGB555)
        return TEXT("RGB525");
    if (SubType == MFVideoFormat_RGB565)
        return TEXT("RGB565");
    if (SubType == MFVideoFormat_RGB8)
        return TEXT("RGB8");
    if (SubType == MFVideoFormat_AI44)
        return TEXT("AI44");
    if (SubType == MFVideoFormat_AYUV)
        return TEXT("AYUV");
    if (SubType == MFVideoFormat_YUY2)
        return TEXT("YUY2");
    if (SubType == MFVideoFormat_YVYU)
        return TEXT("YVYU");
    if (SubType == MFVideoFormat_YVU9)
        return TEXT("YVU9");
    if (SubType == MFVideoFormat_UYVY)
        return TEXT("UYVY");
    if (SubType == MFVideoFormat_NV11)
        return TEXT("NV11");
    if (SubType == MFVideoFormat_NV12)
        return TEXT("NV12");
    if (SubType == MFVideoFormat_YV12)
        return TEXT("YV12");
    if (SubType == MFVideoFormat_I420)
        return TEXT("I420");
    if (SubType == MFVideoFormat_IYUV)
        return TEXT("IYUV");
    if (SubType == MFVideoFormat_Y210)
        return TEXT("Y210");
    if (SubType == MFVideoFormat_Y216)
        return TEXT("Y216");
    if (SubType == MFVideoFormat_Y410)
        return TEXT("Y410");
    if (SubType == MFVideoFormat_Y416)
        return TEXT("Y416");
    if (SubType == MFVideoFormat_Y41P)
        return TEXT("Y41P");
    if (SubType == MFVideoFormat_Y41T)
        return TEXT("Y41T");
    if (SubType == MFVideoFormat_Y42T)
        return TEXT("Y42T");
    if (SubType == MFVideoFormat_P210)
        return TEXT("P210");
    if (SubType == MFVideoFormat_P216)
        return TEXT("P216");
    if (SubType == MFVideoFormat_P010)
        return TEXT("P010");
    if (SubType == MFVideoFormat_P016)
        return TEXT("P016");
    if (SubType == MFVideoFormat_v210)
        return TEXT("v210");
    if (SubType == MFVideoFormat_v216)
        return TEXT("v216");
    if (SubType == MFVideoFormat_v410)
        return TEXT("v410");
    if (SubType == MFVideoFormat_MP43)
        return TEXT("MP43");
    if (SubType == MFVideoFormat_MP4S)
        return TEXT("MP4S");
    if (SubType == MFVideoFormat_M4S2)
        return TEXT("M4S2");
    if (SubType == MFVideoFormat_MP4V)
        return TEXT("MP4V");
    if (SubType == MFVideoFormat_WMV1)
        return TEXT("WMV1");
    if (SubType == MFVideoFormat_WMV2)
        return TEXT("WMV2");
    if (SubType == MFVideoFormat_WMV3)
        return TEXT("WMV3");
    if (SubType == MFVideoFormat_WVC1)
        return TEXT("WVC1");
    if (SubType == MFVideoFormat_MSS1)
        return TEXT("MSS1");
    if (SubType == MFVideoFormat_MSS2)
        return TEXT("MSS2");
    if (SubType == MFVideoFormat_MPG1)
        return TEXT("MPG1");
    if (SubType == MFVideoFormat_DVSL)
        return TEXT("DVSL");
    if (SubType == MFVideoFormat_DVSD)
        return TEXT("DVSD");
    if (SubType == MFVideoFormat_DVHD)
        return TEXT("DVHD");
    if (SubType == MFVideoFormat_DV25)
        return TEXT("DV25");
    if (SubType == MFVideoFormat_DV50)
        return TEXT("DV50");
    if (SubType == MFVideoFormat_DVH1)
        return TEXT("DVH1");
    if (SubType == MFVideoFormat_DVC)
        return TEXT("DVC");
    if (SubType == MFVideoFormat_H264)
        return TEXT("H264");
    if (SubType == MFVideoFormat_MJPG)
        return TEXT("MJPG");
    if (SubType == MFVideoFormat_420O)
        return TEXT("420O");
    if (SubType == MFVideoFormat_HEVC)
        return TEXT("HEVC");
    if (SubType == MFVideoFormat_HEVC_ES)
        return TEXT("HEVC ES");

#    if (WINVER >= _WIN32_WINNT_WIN8)
    if (SubType == MFVideoFormat_H263)
        return TEXT("H263");
#    endif

    if (SubType == MFVideoFormat_H264_ES)
        return TEXT("H264 ES");
    if (SubType == MFVideoFormat_MPEG2)
        return TEXT("MPEG-2");
    return "UNKNOWN"sv;
}
}  // namespace MFUtils
#endif
