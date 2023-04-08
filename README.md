# Axme - The Axmol Media Engine

A cross paltform media engine for video render based on native sdk

usage: 

- https://github.com/axmolengine/axmol/blob/media-engine/core/ui/UIVideoPlayer.cpp
- https://github.com/halx99/bgfx-axplay/tree/axplay/axplay

step: 

```cpp
  using namespace ax;
  static std::unique_ptr<MediaEngineFactory> _meFactory = CreatePlatformMediaEngineFactory();
  auto me = _meFactory->CreateMediaEngine(); 

  if (me) {
    // Can be url: http://distribution.bbb3d.renderfarming.net/video/mp4/bbb_sunflower_1080p_30fps_normal.mp4
    me->SetAutoPlay(true);
    me->Open("file://D:\\test.mp4");
    
    while(me->GetState() == MEMediaState::Playing) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        bool transferred = me->TransferVideoFrame([](MEVideoFrame& frame) {
            // ... render video sample by OpenGL, DirectX, vulkan, Metal
        });
    }
    _meFactory->DestroyMediaEngine(me);
  }

```
