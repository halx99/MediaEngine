# axplay - The simple media player based on Axmol Media Engine

A cross paltform media engine for video render based on native sdk

usage: 

- https://github.com/axmolengine/axmol/blob/dev/core/ui/UIMediaPlayer.cpp
- https://github.com/halx99/bgfx-axplay/tree/axplay/axplay

step: 

```cpp
  using namespace ax;
  static std::unique_ptr<MediaEngineFactory> _meFactory = MediaEngineFactory::create();
  auto me = _meFactory->createMediaEngine(); 

  if (me) {
    // Can be url: http://distribution.bbb3d.renderfarming.net/video/mp4/bbb_sunflower_1080p_30fps_normal.mp4
    me->setAutoPlay(true);
    me->open("file:///D:\\test.mp4");
    
    while(me->getState() == MEMediaState::Playing) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        bool transferred = me->transferVideoFrame();
    }
    _meFactory->destroyMediaEngine(me);
  }

```
