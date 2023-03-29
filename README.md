# Anme - A Native Media Engine

A cross paltform media engine for video render based on native sdk

usage: refer to: https://github.com/axmolengine/axmol/blob/media-engine/core/ui/UIVideoPlayer.cpp

step: 

```cpp
  using namespace ax;
  static std::unique_ptr<MediaEngineFactory> _meFactory = CreatePlatformMediaEngineFactory();
  auto me = _meFactory->CreateMediaEngine(); 

  MEVideoTextueSample sample;
  if (me) {
    // Can be url: http://distribution.bbb3d.renderfarming.net/video/mp4/bbb_sunflower_1080p_30fps_normal.mp4
    me->Open("file://D:\\test.mp4");
    
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        if (me->GetLastVideoSample(sample)) {
            // ... render video sample by OpenGL, DirectX, vulkan, Metal
        }
    }
  }

```